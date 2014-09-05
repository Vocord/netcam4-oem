#ifndef __NET_SYSLOG_H__
#define __NET_SYSLOG_H__
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
 * syslog over ip helper
 *
\*/
#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int setup_syslog_addr(char *addr);
extern int setup_syslog_sockaddr(struct sockaddr_in* s_addr);
extern int setup_syslog(void *p, FILE** F, char* prefix, int pid);
extern int setup_syslogf(void *p, FILE** F, char* prefix, int pid, int flags);

extern void get_current_syslog_addr(struct sockaddr_in *s_addr);

#ifdef __cplusplus
}
#endif
#endif
