/*\
 *  
 *  Copyright (C) 2004-2014 VOCORD, Inc. <info@vocord.com>
 *
 * This file is part of the P3SS API/ABI/VERIFICATION system.
 *
 * The P3SS API/ABI/VERIFICATION system is free software; you can
 * redistribute it and/or modify it under the terms of the
 * GNU Lesser General Public License
 * as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * The part of the P3SS API/ABI/VERIFICATION system
 * is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with the part of the P3SS API/ABI/VERIFICATION system;
 * if not, see <http://www.gnu.org/licenses/>.
 *
 *
 *       Component for conform portion of the system.
\*/

/*\
 *
 * net sockets support helper
 *
 * Authors
 * 	Alexander V. Rostovtsev <alexr@vocord.com> 2005
 *
\*/
//#define _GNU_SOURCE		1
#include <errno.h>

#define TRACE_PRIVATE_PREFIX	1
#include <trace.h>

#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include <arpa/inet.h>
#include <netdb.h>

#include "net-sockets.h"

static char     *trace_prefix = "NET:";

int inet_ipv4_addr_setup(
	const char		*name,
	struct sockaddr_in	*addr,
	int			port
)
{
	struct hostent	*hp;

	hp = gethostbyname(name);
	if (!hp) {
		ETRACE("Invalid hostname for dst: %s\n", name);
		return 1;
	}

	bcopy(hp->h_addr,(char *)&addr->sin_addr, (size_t)hp->h_length);

	addr->sin_port = htons((uint16_t)port);
	addr->sin_family = AF_INET;

	return 0;
}

int net_sock_keepalive(int s, int rep, int tmout)
{
	int res, opt = 1;
	
	TRACE_FUNC(7, setsockopt(s, SOL_TCP, TCP_NODELAY, &opt, (socklen_t)sizeof(opt)), res, " res=%d\n");
	
	TRACE_FUNC(7, setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &opt, (socklen_t)sizeof(opt)), res, " res=%d\n");
#ifndef __CYGWIN__
	TRACE_FUNC(7, setsockopt(s, SOL_TCP, TCP_KEEPIDLE, &opt, (socklen_t)sizeof(opt)), res, " res=%d\n");
	TRACE_FUNC(7, setsockopt(s, SOL_TCP, TCP_KEEPINTVL, &tmout, (socklen_t)sizeof(tmout)), res, " res=%d\n");
	TRACE_FUNC(7, setsockopt(s, SOL_SOCKET, TCP_KEEPCNT, &rep, (socklen_t)sizeof(rep)), res, " res=%d\n");
#endif
	return res;	
}

int create_connected_socket(int *s, in_addr_t s_addr, int port, int flag)
{
	int err;
	struct sockaddr_in rem_addr;
	socklen_t slen = sizeof(rem_addr);

	*s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(*s == -1) {
		ETRACE("socket error: ");
		return errno;
	}

	rem_addr.sin_family	 	= AF_INET;
	rem_addr.sin_port 	 	= htons((uint16_t)port);
	rem_addr.sin_addr.s_addr	= htonl(s_addr);

	TRACEP(0, "try to connect to: [%08" PRIx32 "]:[%04x]\n", rem_addr.sin_addr.s_addr, rem_addr.sin_port);

	if(flag) {
		net_sock_keepalive(*s, 2, 4);

		if(fcntl(*s, F_SETFL, O_NONBLOCK)) {
			ETRACE("==fcntl(s, F_SETFL, O_NONBLOCK): ");
			close(*s);
			return errno;
		}
	}

	err = connect(*s, (struct sockaddr*)&rem_addr, slen);
	if(err) {
		TRACEP(0, "err: cannot connect to [%08lx:%d] by reason: %s\n",
				(unsigned long)s_addr, port, strerror(errno));
		close(*s);
		return errno;
	}

	return 0;
}

int init_listen_socket_inaddr(int *s, in_addr_t s_addr, int port, int flag)
{
	struct sockaddr_in	serv_addr;
	int			opt;

	*s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(*s == -1) {
		ETRACE("socket error: ");
		return errno;
	}
	
	TRACEP(0, "Socket [%d] created...\n", *s);

	serv_addr.sin_family	 	= AF_INET;
	serv_addr.sin_port 	 	= htons((uint16_t)port);
	serv_addr.sin_addr.s_addr	= htonl(s_addr);

	errno = 0;

	opt = 1;
	setsockopt(*s, SOL_SOCKET, SO_REUSEADDR, &opt, (socklen_t)sizeof(opt));
	if(errno) {
		ETRACE("setsockopt: ");
		close(*s);
		return errno;
	}
	
	TRACEP(0, "binding to port: %d\n", port);
	bind(*s,(struct sockaddr *)&serv_addr, (socklen_t)sizeof(serv_addr));
	if(errno) {
		ETRACE("bind: ");
		close(*s);
		return errno;
	}

	if(flag) {
		net_sock_keepalive(*s, 2, 4);

		if(fcntl(*s, F_SETFL, O_NONBLOCK)) {
			ETRACE("==fcntl(s, F_SETFL, O_NONBLOCK): ");
			close(*s);
			return errno;
		}
	}

	listen(*s, 4);
	if(errno) {
		ETRACE("listen problem: ");
		close(*s);
		return errno;
	}
	
	TRACEP(0, "socket [%d] listened\n", *s);
	
	return 0;
}

int init_listen_socket(int *s, int port, int flag)
{
	return init_listen_socket_inaddr(s, htonl(INADDR_ANY), port, flag);
}

int init_listen_socket_mcast(int *s, mcast_req_t *m)
{
	int	err, on;
	struct 	ifreq ifr;
	struct 	sockaddr_in	ssin;

	/* setup source addr */
	ssin.sin_family		= AF_INET;
	ssin.sin_addr.s_addr	= INADDR_ANY;
	ssin.sin_port		= htons(m->mport); /* proto daemon listen port */

	/* setup socket */
	*s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (*s == -1) {
		TRACEP(0, "mcast socket creation error");
		return -1;
	}

	err = bind(*s, (struct sockaddr*)&ssin, (socklen_t)sizeof(ssin));
	if(err) {
		TRACEP(0, "Due Bind to 520 x,x,x,x ");
		return -1;
	}

	/*************************************************************/
	/* get eth0 IP address */

	bzero(ifr.ifr_name, sizeof(ifr.ifr_name));
	strncpy(ifr.ifr_name, "eth0", sizeof(ifr.ifr_name));
	ifr.ifr_addr.sa_family = AF_INET;
	err = ioctl (*s, (unsigned long)SIOCGIFADDR, &ifr);
	if(!err) {
		memcpy(&m->ipv4_src, &ifr.ifr_addr, sizeof(struct sockaddr_in));
	} else {
		TRACEP (0, "when ioctl(s, SIOCGIFADDR, &ifr)\n");
	}

	/* get eth0 MAC address */
	err = ioctl(*s, (unsigned long)SIOCGIFHWADDR, &ifr);
	if(!err) {
		memcpy(&m->mac[0], ifr.ifr_hwaddr.sa_data, sizeof(m->mac));
	} else {
		TRACEP(0, "when ioctl(s, SIOCGIFHWADDR, &ifr)\n");
	}
	
#if 1
	on = 1;
	err = setsockopt(*s, SOL_SOCKET, SO_BROADCAST, &on, (socklen_t)sizeof (on));
	if(err) {
		TRACEP(0, "Due setsockopt SO_BROADCAST");
		close(*s);
		return (-1);
	}
#endif
	/* setup multicast listening */
	m->mreq.imr_ifindex          = 0;
	m->mreq.imr_multiaddr.s_addr = m->mcast.sin_addr.s_addr;
	m->mreq.imr_address.s_addr   = INADDR_ANY;

	/* become member of multicast group for receiving packets */
	err = setsockopt(*s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&m->mreq, (socklen_t)sizeof(m->mreq));
	if(err) {
		TRACEP(0, "add into mcast group");
	}

	/* disable loopback to self */
	on = 0;
	err = setsockopt (*s, IPPROTO_IP, IP_MULTICAST_LOOP, (void*)&on, (socklen_t)sizeof(on));
	if(err) {
		TRACEP(0, "disable mcast loopback");
	}
	
	/* set multicast TTL to 4 */
	on = 4;
	err = setsockopt(*s, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&on, (socklen_t)sizeof(on));
	if(err) {
		TRACEP(0, "set mcast ttl to:%d", on);
	}

	TRACEP(0, "mcast socket [%d] opened\n", *s);
	
	return 0;
}
