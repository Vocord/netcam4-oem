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
 * SYSLOG via net support helper
 *
 * Authors
 * 	Alexander V. Rostovtsev <alexr@vocord.com> 2005
 *
\*/
#define _GNU_SOURCE		1
#include <inttypes.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>

#include <sys/time.h>

#include <os-helpers/net-syslog.h>
#include <trace.h>

#include <os-helpers/inet_macros.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

static char		perror_str[512];
static int		syslog_socket;

static struct sockaddr_in	syslog_serv = {
	.sin_addr = { .s_addr = DIG2INETB(192,168,10,1) }
};

static char		syslog_s[1024];
static char		*syslog_ss   = syslog_s;
static size_t		syslogs_max  = sizeof(syslog_s) - 1;
static size_t		syslogs_offs = 0;

static inline void do_syslog(const char *buf, size_t len)
{
	ssize_t			res	= 0;
	static unsigned int	log_cnt	= 0;

	len = len<syslogs_max ?len :syslogs_max;

	int	errno_old = errno;

	snprintf(syslog_ss-9, 10, "%08x ", log_cnt);
	memcpy(syslog_ss, buf, len);
	syslog_ss[len] = 0;		/* make string ASCIZ */

	/* Do syslog */
	res = sendto(
	    syslog_socket,
	    syslog_s,
	    len+syslogs_offs,
	    0 /* MSG_DONTWAIT */,
	    &syslog_serv, sizeof(syslog_serv)
	);

	if(res != len+syslogs_offs) {
		PERROR(perror_str, "%s:%d:syslog(%d bytes) = %d\n", __func__, __LINE__, len, res);
		errno = errno_old;
	}

	log_cnt++;
}

static ssize_t my_log_writer(void *p, const char *buf, size_t size)
{
	size_t		b = 0;	/* str begin */
	size_t		e = 0;	/* str end   */

	static	int	cr = 0;
	
	/* extract lines from buf & log it */
	while(size) {
		if(buf[e] == '\n') {
			if(!(cr && (e==b))) {
				do_syslog(buf+b, e - b);
				cr=0;
			}

			b = e+1;
		}

		/* move pos */
		e++;
		size--;
	}

	if(b<e) {
		do_syslog(buf+b, e - b);
		cr=1;
	}

//	e = write(fd_stdout, buf, size);
#if DEBUG_LOGGING
	printf("[%d->%zu]\"", size, e);
	fflush(stdout);
	write(0, buf, e);
	write(0, "\"", 1);
#endif
	return (ssize_t)e;
}

cookie_io_functions_t my_io_functions = {
	.write = (cookie_write_function_t*)&my_log_writer,
};

int setup_syslog_addr(char *addr)
{
	struct hostent		*hp;

	hp = gethostbyname(addr);
	if (!hp) {
		fprintf(stderr,"Invalid address for syslog serv: %s\n", addr);
		return 1;
	}

	bcopy(hp->h_addr,(char *)&syslog_serv.sin_addr, (size_t)hp->h_length);

	return 0;
}

int setup_syslog_sockaddr(struct sockaddr_in* s_addr)
{
	bcopy((char*)&s_addr->sin_addr, (char*)&syslog_serv.sin_addr, sizeof(s_addr->sin_addr));

	return 0;
}

void get_current_syslog_addr(struct sockaddr_in *s_addr)
{
	memcpy(s_addr, &syslog_serv, sizeof(*s_addr));
}

int setup_syslogf(void *p, FILE** F, char* prefix, int pid, int flags)
{
	/* setup prefix */
	FILE	*T	= fopen("/proc/sys/kernel/hostname","r");
	char	tmp[40] = "unknown";

	if(!fgets(tmp, sizeof(tmp), T))
		ETRACE("When reading hostname");

	fclose(T);

	tmp[strlen(tmp)>1?strlen(tmp)-1:0] = 0;
	    
	syslogs_offs = (size_t)9 + (size_t)snprintf(syslog_s, sizeof(syslog_s), "<%d>%s:%s[%d] ", LOG_LOCAL6|LOG_DEBUG, tmp, prefix, pid);

	syslog_ss   = syslog_s             + syslogs_offs;	/* ptr to TRACES */
	syslogs_max = sizeof(syslog_s) - 1 - syslogs_offs;	/* max TRACE len */

	syslog_socket = socket(PF_INET, SOCK_DGRAM, 0);

	syslog_serv.sin_family	 = AF_INET;
	syslog_serv.sin_port 	 = htons(514);
//	syslog_serv.sin_addr.s_addr = htonl(DIG2INETL(192,168,10,1));

//	openlog(prefix, LOG_CONS|LOG_NDELAY|LOG_NOWAIT|LOG_PID, LOG_LOCAL6);

	/* setup TRACE stream */
	(*F) = fopencookie (p, "w", my_io_functions);
	if(!(*F)) {
		(*F) = stdout;

		TRACE(0, "failed to create TRACE flow FALLING BACK to stdout\n");
	}

	return 0;
}

int setup_syslog(void *p, FILE** F, char* prefix, int pid)
{
	return setup_syslogf(p, F, prefix, pid, LOG_LOCAL6|LOG_DEBUG);
}
