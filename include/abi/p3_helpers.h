#ifndef __ABI_P3_HELPERS_H__
#define __ABI_P3_HELPERS_H__
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
#if defined(DEVICE_ISOLATED)
#include <isolated/abi/p3.h>
#else
#include <abi/p3.h>
#endif
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif
//#if defined(__GNUC__)
//
#define UNIX_TIME_UTC_OF_2000_01_01     (946684799ull)
#define BUGGY_FILETIME_OF_1970_01_01    (0x019db1ded53e8000ull)
#define BUGGY_FILETIME_OF_1970_01_01_H  (27111902ull)
#define BUGGY_FILETIME_OF_1970_01_01_L  (3577643008ull)
static __inline__ uint64_t timeval2filetime(struct timeval *tv)
{
	uint64_t t;

	t =  (uint64_t)(BUGGY_FILETIME_OF_1970_01_01_H<<32ull);
	t += (uint64_t)(BUGGY_FILETIME_OF_1970_01_01_L);

	t += (uint64_t)tv->tv_sec  * 10000000ull;
	t += (uint64_t)tv->tv_usec * 10ull;

	TRACEPNF(9, "ft:%016"PRIx64" <-%10lu.%09lu\n", t, (unsigned long)tv->tv_sec, (unsigned long)tv->tv_usec);
	
	return t;
}

static __inline__ uint64_t unixtime2filetime(uint32_t time)
{
	struct timeval tv = {(long int)time, 0};
	return timeval2filetime(&tv);
}

static __inline__ uint64_t unixtime2filetime_with_respect_to_msec(uint32_t time, uint32_t msec)
{
	struct timeval tv = {(long int)time, (long int)msec * 1000};
	return timeval2filetime(&tv);
}

static __inline__ uint64_t cur2filetime(void)
{
	struct timeval tv = { };

	if(my_light_gettimeofday(&tv)) {
		ETRACE("gettimeofday error\n");
	}

	TRACEPNF(8, "%s %9lu.%06lu\n", __func__, tv.tv_sec, tv.tv_usec);

	return timeval2filetime(&tv);
}

static __inline__ uint64_t __attribute__((const))  time2filetime(time_t tm)
{
	uint64_t t;

	t = (BUGGY_FILETIME_OF_1970_01_01_H<<32ull);
	t += BUGGY_FILETIME_OF_1970_01_01_L;

	t += (uint64_t)tm * 10000000ull;

	return t;
}

static __inline__ uint64_t __attribute__((const)) filetime2msec(uint64_t ft)
{
	uint64_t t;

	ft -= (BUGGY_FILETIME_OF_1970_01_01_H<<32ull);
	ft -= (BUGGY_FILETIME_OF_1970_01_01_L);

	t = ft / 10000ull;

	return t;
}

static __inline__ time_t __attribute__((const)) filetime2time(uint64_t ft)
{
	time_t t;

	ft -= (BUGGY_FILETIME_OF_1970_01_01_H<<32ull);
	ft -= (BUGGY_FILETIME_OF_1970_01_01_L);

	t = (time_t)(ft / 10000000ull);

	return t;
}

static __inline__ filetime filetime_to_host(filetime *s)
{
	uint64_t tmp;
#if __BYTE_ORDER==__BIG_ENDIAN
        tmp = *s;
#else
#if __BYTE_ORDER==__LITTLE_ENDIAN
	tmp =  ((uint64_t)htonl((uint32_t)*s))<<32ull;
        tmp += htonl((uint32_t)((*s)>>32));
#else
#error "__BYTE_ORDER undefined, please check your includes"
#endif
#endif
        return tmp;
}

static __inline__ filetime filetime_to_net(filetime *s)
{
	return filetime_to_host(s);
}

static __inline__ filetime filetime_to_db(filetime *s)
{
	return filetime_to_host(s);
}

static __inline__ void copy_filetime_to_uint64_t(uint64_t *d, filetime *s)
{
#if __BYTE_ORDER==__BIG_ENDIAN
        *d = *s;
#else
#if __BYTE_ORDER==__LITTLE_ENDIAN
	uint64_t tmp =  ((uint64_t)htonl((uint32_t)*s))<<32ull; 
        tmp += htonl((uint32_t)((*s)>>32));
        *d = tmp;
#else
#error "__BYTE_ORDER undefined, please check your includes"
#endif
#endif
        return;
}

static __inline__ void copy_filetime_to_key(filetime *d, filetime *s)
{
	copy_filetime_to_uint64_t(d, s);

        return;
}

static __inline__ void copy_compression_to_key(
        p3_frame_descr_t *d,
       	uint32_t          compr,
       	unsigned          activity
)
{
#if __BYTE_ORDER==__BIG_ENDIAN
        *(uint32_t*)d = (compr&0x00ffffff) | (0xff000000&(activity<<24));
#else
#if __BYTE_ORDER==__LITTLE_ENDIAN
#if !defined(__ARM_ARCH_5TEJ__)
        *(uint32_t*)d = htonl(((compr<<8)&0xffffff00) | (0xff&activity));
#else
	d->activity = (uint8_t)activity;
	d->compr[0] = (compr >> 16) & 0xff;
	d->compr[1] = (compr >> 8)  & 0xff;
	d->compr[2] = (compr >> 0)  & 0xff;
#endif
#else
#error "__BYTE_ORDER undefined, please check your includes"
#endif
#endif
    return;
}

static __inline__ void tracea_ft(const char* prefix, filetime val)
{
	time_t tm = filetime2time(val);

	TRACE(0, "%s ft: %016"PRIx64" %20"PRIi64" tm: %08lx %ld %s",
		prefix,
	       	val, val,
	       	(long)tm, (long)tm,
	       	ctime(&tm)
	);
}

extern const char *p3_errnotostr(int p3_errno);
//#endif

#ifdef __cplusplus
}
#endif

#endif  /* __ABI_P3_HELPERS_H__*/
