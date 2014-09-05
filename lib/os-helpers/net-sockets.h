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
#ifndef __LIB_NET_SOCKETS_H__
#define __LIB_NET_SOCKETS_H__
#include <sys/socket.h>
#include <netinet/in.h>


# if __BYTE_ORDER == __BIG_ENDIAN
/* The host byte order is the same as network byte order,
   so these functions are all just identity.  */
# define NTOHL(X)	(X)
# define NTOHS(X)	(X)
# define HTONL(X)	(X)
# define HTONS(X)	(X)
# else
#  if __BYTE_ORDER == __LITTLE_ENDIAN
#   define NTOHL(X)	__BSWAP_CONSTANT_32 (X)
#   define NTOHS(X)	__BSWAP_CONSTANT_16 (X)
#   define HTONL(X)	__BSWAP_CONSTANT_32 (X)
#   define HTONS(X)	__BSWAP_CONSTANT_16 (X)
#  endif
# endif

/* Swap bytes in 32 bit value.  */
#define __BSWAP_CONSTANT_32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) |		      \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

/* Swap bytes in 16 bit value.  */
#define __BSWAP_CONSTANT_16(x) \
     ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))

#ifdef __CYGWIN__
struct ip_mreqn

{
	struct in_addr	imr_multiaddr; /* IP multicast address of group */
	struct in_addr	imr_address;   /* local IP address of interface */
	int		imr_ifindex;
};
#endif

typedef struct 
{
	struct ip_mreqn		mreq;

	uint16_t		dst_port;	/* MCAST destination port */
	uint16_t		mport;		/* self-opened port	  */

	struct sockaddr_in	mcast;

	struct sockaddr_in	ipv4_src;
	uint8_t			mac[6];
} mcast_req_t;

#ifdef __cplusplus
extern "C" {
#endif

int init_listen_socket_mcast(int *s, mcast_req_t *m);

extern int inet_ipv4_addr_setup(
	const char		*name,
	struct sockaddr_in	*addr,
	int			port
);

extern int net_sock_keepalive(int s, int rep, int tmout);
extern int init_listen_socket(int *s, int port, int flag);
extern int init_listen_socket_inaddr(
	int		*s,
	in_addr_t	s_addr,
	int		port,
	int		flag
);

extern int create_connected_socket(
		int *s, 
		in_addr_t s_addr, 
		int port, 
		int flag);

#ifdef __cplusplus
}
#endif


#endif
