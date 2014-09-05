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
 *   Header file for TIME module
 *
\*/
#ifndef __TIMERS_TIME_H__
#define __TIMERS_TIME_H__
#include <sys/time.h>
#include <time.h>

/*\
 *
 *  static inline function definitions
 * 
\*/
static inline void	normalize_timer(struct timeval *tv)
{
	while(tv->tv_usec >= 1000000) {
		tv->tv_sec++;
		tv->tv_usec -= 1000000;
	}
}

static inline int	sub_timer(struct timeval *tv, struct timeval ts)
{
	if(tv->tv_usec < ts.tv_usec) {
		tv->tv_usec += 1000000;
		tv->tv_sec--;
	};

	tv->tv_usec -= ts.tv_usec;
	tv->tv_sec  -= ts.tv_sec;

	if(tv->tv_usec < 0)
		return -1;

	if(tv->tv_sec < 0)
		return -1;

	return 0;
}

static inline void	add_timer(struct timeval *tv, struct timeval ts)
{
	tv->tv_usec += ts.tv_usec;
  
	while(tv->tv_usec > 1000000) {
		tv->tv_usec -= 1000000;
		tv->tv_sec++;
	};

	tv->tv_sec += ts.tv_sec;
}
#endif
