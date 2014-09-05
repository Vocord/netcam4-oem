#ifndef __TRACE_H__
#define __TRACE_H__
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
 *  Description: trace OS specific functions header.
 *
 *       Author: alexr
 *      License: LGPL
 *   
 *
 *  tab-size=8
\*/
#ifdef __cplusplus
extern "C" {
#endif

#ifndef TRACE_LEVEL
#define TRACE_LEVEL	3
#endif

#ifdef ETRACE
#undef ETRACE
#endif

#ifdef ETRACEPP
#undef ETRACEPP
#endif

#ifdef ETRACEP
#undef ETRACEP
#endif

#ifdef TRACE
#undef TRACE
#endif

#ifdef TRACEP
#undef TRACEP
#endif

#ifdef TRACEPNF
#undef TRACEPNF
#endif

#ifdef TRACEF
#undef TRACEF
#endif

#ifdef TRACE_FLUSH
#undef TRACE_FLUSH
#endif

#ifdef PERROR
#undef PERROR
#endif

    
#if TRACE_LEVEL >= 0
#ifdef __KERNEL__
/* printk prototype:) */
#include <linux/kernel.h>

#define TRACE(LEVEL, FORMAT, ARGS... )				\
{								\
	if(LEVEL <= TRACE_LEVEL) {				\
		printk( FORMAT, ## ARGS );			\
	};							\
}

/* ifdef __KERNEL__ End */
#else
/* if ndef __KERNEL__ */
#ifndef __USE_POSIX
#define __USE_POSIX     1
#endif

#ifndef __USE_UNIX98
#define __USE_UNIX98    1
#endif

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef __CYGWIN__
#include <sys/syscall.h>
#endif

#ifdef DECLARE_TRACE_VARS
#undef DECLARE_TRACE_VARS
#endif

#define TRACE_VAR		I

#define DECLARE_TRACE_VARS	FILE *I
#define DECLARE_ETRACE_VARS	FILE *E

#if ENABLE_FUNC_TRC
#ifndef TLS_ENABLED_TMP
#if TLS_ENABLED
#define TLS_ENABLED_TMP		__thread
#else
#define TLS_ENABLED_TMP
#endif
#endif
#define DECLARE_TRC_VARSG	TLS_ENABLED_TMP char *TRC_BUFFER;
#define DECLARE_TRC_VARSS	static TLS_ENABLED_TMP char *TRC_BUFFER;
#define SET_TRC_VAR(V)		TRC_BUFFER = V;

#define TRC(X, Y...)		\
do { \
	snprintf(TRC_BUFFER, (size_t)1024, "%s:%d "X, __func__, __LINE__, ## Y); \
} while (0)
#else
#define TRC(X, Y...)		;
#define DECLARE_TRC_VARSS	;
#define DECLARE_TRC_VARSG	;
#define SET_TRC_VAR(V)		;
#endif

extern DECLARE_TRACE_VARS;

#ifndef TRACE_PRIVATE_PREFIX
extern char	*trace_prefix;
#else
static char	*trace_prefix;
#endif

#define TRACE_FLUSHL(LEVEL)					\
do {								\
    { if(LEVEL <= TRACE_LEVEL) fflush(I); }			\
} while (0)

#define TRACE_FLUSH()						\
do {								\
	fflush(I);						\
} while (0)

#ifndef __CYGWIN__
static inline int my_light_gettimeofday(struct timeval *tv)
{
	return syscall((long)__NR_gettimeofday, tv, NULL);
}
#else
static inline int my_light_gettimeofday(struct timeval *tv)
{
	return gettimeofday(tv, NULL);
}
#endif

#if 1
static inline void TRACE_MSG(const char *fmt, ...) __attribute__((unused))  __attribute__((format (printf, 1, 2)));
static inline void TRACE_MSG(const char *fmt, ...)
{
	char trace_msg[512];
	va_list ap;

	/* Try to print in the allocated space. */
	va_start(ap, fmt);
	vsnprintf(trace_msg, sizeof(trace_msg), fmt, ap);
	va_end(ap);

	fputs(trace_msg, I);
}
#define TRACE(LEVEL, FORMAT, ARGS... )				\
do {								\
	if(LEVEL <= TRACE_LEVEL) {				\
		TRACE_MSG(FORMAT, ## ARGS);			\
	}							\
} while (0)
#else
#define TRACE(LEVEL, FORMAT, ARGS... )				\
do {								\
	char MSG[256];						\
								\
	if(LEVEL <= TRACE_LEVEL) {				\
		snprintf( MSG, sizeof(MSG), FORMAT, ## ARGS);	\
		fputs(MSG, I);					\
	}							\
} while (0)
#endif

#define TDUMP(lvl, str, i, buff, size) 				\
do {								\
	TRACE(lvl, str); 					\
	for(i = 0; i < size; i++) { 				\
		TRACE(lvl, "%02X", buff[i]); 			\
	} 							\
	TRACE(lvl, "\n"); 					\
} while(0)

#define TDUMPP(lvl, str, i, buff, size) 			\
do {								\
	TRACEP(lvl, str); 					\
	for(i = 0; i < size; i++) { 				\
		TRACE(lvl, "%02X", buff[i]); 			\
	} 							\
	TRACE(lvl, "\n"); 					\
} while(0)

#define TDUMPE(lvl, str, estr, i, buff, size) 			\
do {								\
	TRACE(lvl, str); 					\
	for(i = 0; i < size; i++) { 				\
		TRACE(lvl, "%02X", buff[i]); 			\
	} 							\
	TRACE(lvl, estr); 					\
} while(0)

static inline void get_time_in_a(char *timea) __attribute__((unused));
static inline void get_time_in_a(char *timea)
{
	struct timeval	tvt;
	struct tm	tm;

	my_light_gettimeofday(&tvt);
	if(localtime_r(&tvt.tv_sec, &tm)==NULL){
		timea += snprintf(timea, (size_t)25, "localtime_r error");
	} else {
		snprintf(timea, (size_t)25, "%02d:%02d:%02d.%03lu",
			tm.tm_hour,
			tm.tm_min,
			tm.tm_sec,
		       	tvt.tv_usec/1000
		);
	}
}

static inline void get_time_in_a1(char *timea) __attribute__((unused));
static inline void get_time_in_a1(char *timea)
{
	struct timeval tvt = { };
	struct tm tm = { };
	size_t val;
	if(my_light_gettimeofday(&tvt)){
		fprintf(I,"gettimeofday error %s\n", strerror(errno));
	}
	
	timea[0] = 0;
	if(localtime_r(&tvt.tv_sec, &tm) == NULL){
		fprintf(I,"localtime error %s\n", strerror(errno));
	}

	if(!(val=strftime(timea, (size_t)9, "%T",&tm))){
		fputs("strftime error\n", I);
	}
	snprintf(timea+val, (size_t)5, ".%.3lu", tvt.tv_usec/1000);
}

static inline void pTRACE_MSG(char *tm, const char *fmt, ...) __attribute__((unused)) __attribute__((format (printf, 2, 3)));
static inline void pTRACE_MSG(char *tm, const char *fmt, ...)
{
	char trace_msg[512];
	va_list ap;

	get_time_in_a(tm);

	/* Try to print in the allocated space. */
	va_start(ap, fmt);
	vsnprintf(trace_msg, sizeof(trace_msg), fmt, ap);
	va_end(ap);

	fputs(trace_msg, I);
}

#define TRACEPNF(LEVEL, FORMAT, ARGS... )				\
do {									\
	if(LEVEL <= TRACE_LEVEL) {					\
		char tmb[25];						\
		tmb[0] = '\n'; \
		if(trace_prefix[0] == '\n') {				\
	        	pTRACE_MSG(tmb+1, "%s %s"FORMAT, tmb, trace_prefix+1, ## ARGS);	\
		} else { \
	        	pTRACE_MSG(tmb+0, "%s %s"FORMAT, tmb, trace_prefix+0, ## ARGS);	\
		} \
	};								\
} while(0)

#define TRACEP(LEVEL, FORMAT, ARGS... )				\
do {								\
	TRACEPNF(LEVEL, FORMAT, ## ARGS);			\
	if(LEVEL <= TRACE_LEVEL) {				\
		TRACE_FLUSH();					\
	};							\
} while (0)

#define TRACEP0(LEVEL, FMT, ARGS...)				\
do {								\
	char MSG[256];						\
	char *TP = trace_prefix;				\
								\
	if(LEVEL <= TRACE_LEVEL) {				\
		snprintf(MSG, sizeof(MSG),			\
		    "%s"FMT, TP, ## ARGS);			\
		fputs(MSG, I);					\
		TRACE_FLUSH();					\
	};							\
} while (0)

#define SHOW_FILES() 						\
do {								\
	TRACEP(0, "fileno(stderr)=%d\n", fileno(stderr));	\
	TRACEP(0, "fileno(stdout)=%d\n", fileno(stdout));	\
	TRACEP(0, "fileno(stdin) =%d\n", fileno(stdin));	\
} while (0)

#define PERROR(S, FORMAT, ARGS... )			\
do {							\
	snprintf(S, sizeof(S), FORMAT, ## ARGS);	\
	fprintf(stderr, "%s:%s", strerror(errno), S);	\
	fflush(stderr);					\
} while (0)

/** TRACE with prefix Error including pid
 *
 */
#define ETRACE(FORMAT, ARGS... )				\
do {								\
	TRACE_MSG(FORMAT ":%s\n", ## ARGS, strerror(errno));	\
	TRACE_FLUSH();						\
} while (0)
/** TRACE with prefix Error including decimation
 *
 */
#define TRACEPD(DECIMATOR, FORMAT, ARGS... )			\
do {								\
	static int dec = 0;					\
	if(dec >= DECIMATOR)					\
		dec = 0;					\
								\
	if(!dec)						\
		TRACEP(0, FORMAT, ## ARGS);			\
	dec++;							\
} while (0)

#define ETRACED(DECIMATOR, FORMAT, ARGS... )			\
do {								\
	static int dec = 0;					\
	if(dec >= DECIMATOR)					\
		dec = 0;					\
								\
	if(!dec)						\
		ETRACE( FORMAT, ## ARGS);			\
	dec++;							\
} while (0)


#define ETRACES(MSG, FORMAT, ARGS... )				\
do {								\
	snprintf( MSG, sizeof(MSG), FORMAT ":%s\n", ## ARGS, strerror(errno));	\
	fputs(MSG, I);						\
	TRACE_FLUSH();						\
} while (0)

/** TRACE Error with prefix
 */
#define ETRACEP(FORMAT, ARGS... )				\
do {								\
	TRACEP(0, FORMAT ":%s\n", ## ARGS, strerror(errno));	\
} while (0)

/** TRACE Error with prefix and pid
 */
#define ETRACEPP(FORMAT, ARGS... )				\
do {								\
	TRACEP( MSG, sizeof(MSG), FORMAT "(pid:%d):%s\n", ## ARGS, getpid(), strerror(errno));	\
} while (0)

#endif

/* common USER space and KERNEL space part */
#define TRACEF(LEVEL, FUNC)					\
do {								\
	if(LEVEL <= TRACE_LEVEL) {				\
		FUNC;						\
	};							\
} while (0)

#ifndef TRACE_FUNCR
#define TRACE_FUNCR(L, X, R, SS) \
do { \
	R = X;			\
	TRACE(L, #X SS, R);	\
	if(R) {			\
		return R;	\
	}			\
} while (0)
#endif

#else

/* TRACE_LEVEL <0  So No trace at all */
#define TRACE(LEVEL, FORMAT, ARGS... )		((void) 0)
#define TRACEP(LEVEL, FORMAT, ARGS... )		((void) 0)
#define TRACEPNF(LEVEL, FORMAT, ARGS... )	((void) 0)
#define TRACEF(LEVEL, FUNC)			((void) 0)
#define TRACE_FLUSH()				((void) 0)

#define PERROR(S, FORMAT, ARGS... )		((void) 0)

#define ETRACE(FORMAT, ARGS... )		((void) 0)
#define ETRACEP(FORMAT, ARGS... )		((void) 0)

#endif

#ifdef __cplusplus
}
#endif

#endif
