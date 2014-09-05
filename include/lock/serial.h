#ifndef __LOCK_SERIAL_H__
#define __LOCK_SERIAL_H__
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
 * Header file for serial locking functions and macros
 *
 * WARNING Do not foget to use "volatile" subtype specifier,
 * when using for IPC/ITC/RPC
 *
\*/
#include <inttypes.h>
#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	uint8_t		vcur;
	uint8_t		vnew;
} __attribute__((packed)) serial8_t;

typedef volatile struct {
	uint8_t		vcur;
	uint8_t		vnew;
} __attribute__((packed)) vserial8_t;

typedef struct {
	uint16_t	vcur;
	uint16_t	vnew;
} __attribute__((packed)) serial16_t;

typedef struct {
	uint32_t	vcur;
	uint32_t	vnew;
} __attribute__((packed)) serial32_t;

typedef struct {
	uint64_t	vcur;
	uint64_t	vnew;
} __attribute__((packed)) serial64_t;

typedef struct {
	uint32_t	vcur;
	uint32_t	vold;

	uint32_t	serial;
} __attribute__((packed)) state32_t;

typedef struct {
	uint32_t	serial;
	uint32_t	state;
	uint64_t	ft;
} __attribute__((packed)) state32_ft_t;

typedef struct {
	uint32_t	last;
	uint32_t	mark;
} __attribute__((packed)) wm_delta32_t;


static inline int check_serial8(volatile serial8_t *serial) __attribute__((unused));
static inline int check_serial8(volatile serial8_t *serial)
{
	uint_fast8_t vnew = serial->vnew; 	/* atomize new value */
	if(likely(serial->vcur == vnew))
		return 0;

	serial->vcur = vnew;
	return 1;
}

static inline int check_serial16(volatile serial16_t *serial) __attribute__((unused));
static inline int check_serial16(volatile serial16_t *serial)
{
	uint_fast16_t vnew = serial->vnew; 	/* atomize new value */
	if(likely(serial->vcur == vnew))
		return 0;

	serial->vcur = (uint16_t)vnew;
	return 1;
}

static inline int check_serial32(volatile serial32_t *serial) __attribute__((unused));
static inline int check_serial32(volatile serial32_t *serial)
{
	uint_fast32_t vnew = serial->vnew; 	/* atomize new value */
	if(likely(serial->vcur == vnew))
		return 0;

	serial->vcur = vnew;
	return 1;
}
static inline int check_serial32_(volatile serial32_t *serial) __attribute__((unused));
static inline int check_serial32_(volatile serial32_t *serial)
{
	uint_fast32_t vnew = serial->vnew; 	/* atomize new value */
	if(likely(serial->vcur == vnew))
		return 0;

	return 1;
}


static inline int check_serial8_(volatile serial8_t *serial) __attribute__((unused));
static inline int check_serial8_(volatile serial8_t *serial)
{
	if(likely(serial->vcur == serial->vnew))
		return 0;

	return 1;
}

#ifdef __cplusplus
}
#endif

#endif /* __LOCK_SERIAL_H__ */
