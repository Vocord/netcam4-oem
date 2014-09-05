#ifndef __OS_HELPERS_PTHREADS_H__
#define __OS_HELPERS_PTHREADS_H__
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
 * Header file for Pthreads helper functions and macros
\*/

#ifdef __CYGWIN__
#define PRIpthread_t		"p"
#else
#define PRIpthread_t		"08lx"
#endif

#define	CREATE_THREAD(RES, F, PRIV, RET) { \
	RES =  pthread_create(&RET, NULL, F, PRIV);	\
	if(RES) {					\
		ETRACE("==pthread_create:");		\
	} else {					\
		TRACEP(0, "==" #RET " id: %"PRIpthread_t"\n", RET);\
	}						\
}

#define	THREAD_JOIN(RES, ID, RET) { \
	TRACEP(0, "==wait thread id: %"PRIpthread_t"\n", ID);	\
	RES = pthread_join(ID, &RET);			\
	if(RES) {					\
		ETRACE("==pthread_join:");		\
	}						\
	TRACEP(0, "==thread id: %"PRIpthread_t" ecode:%d\n", ID, RET?*(int*)RET:-1); \
}

#endif
