#ifndef __INET_MACROS_H__
#define __INET_MACROS_H__
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

#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SHRB(X,Y)		((X>>(Y*8)) & 255)
#define DIG2INET(a3,a2,a1,a0)	(a3 + (a2 << 8) + (a1 << 16) + (a0 << 24))

/* FIXME INET2DIG2 */
#define ARR8(X,I)		((uint8_t*)(X))[I]
#define INET2DIG2(X)		ARR8(X,0), ARR8(X,1), ARR8(X,2), ARR8(X,3)

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define INET2DIG(X)		SHRB(X,0), SHRB(X,1), SHRB(X,2), SHRB(X,3)
#define INET2DIG_BE(X)		SHRB(X,0), SHRB(X,1), SHRB(X,2), SHRB(X,3)
#define INET2DIG_LE(X)		SHRB(X,3), SHRB(X,2), SHRB(X,1), SHRB(X,0)

#define DIG2INETL(a0,a1,a2,a3)	(uint32_t)(a3 + (a2 << 8) + (a1 << 16) + (a0 << 24))
#define DIG2INETB(a0,a1,a2,a3)	(uint32_t)(a0 + (a1 << 8) + (a2 << 16) + (a3 << 24))
#elif __BYTE_ORDER == __BIG_ENDIAN                                                                                                         
#define INET2DIG(X)		SHRB(X,3), SHRB(X,2), SHRB(X,1), SHRB(X,0)
#define INET2DIG_BE(X)		SHRB(X,3), SHRB(X,2), SHRB(X,1), SHRB(X,0)
#define INET2DIG_LE(X)		SHRB(X,0), SHRB(X,1), SHRB(X,2), SHRB(X,3)


#define DIG2INETL(a0,a1,a2,a3)	(uint32_t)(a0 + (a1 << 8) + (a2 << 16) + (a3 << 24))
#define DIG2INETB(a0,a1,a2,a3)	(uint32_t)(a3 + (a2 << 8) + (a1 << 16) + (a0 << 24))
#else
#error  "Please fix <bits/endian.h>"                                                                                                       
#endif

extern int net_sock_keepalive(int s, int rep, int tmout);
extern int init_listen_socket(int *s, int port, int flag);

#ifdef __cplusplus
}
#endif

#endif
