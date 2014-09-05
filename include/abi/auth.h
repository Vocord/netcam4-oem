#ifndef __ABI_AUTH_H__
#define __ABI_AUTH_H__
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
 *         Brief: AUTH ABI declarations.
 *
 *   Description:
 *     Declaratins of data constants and structures for P3SS AUTH ABI
 *
 *   Dependencies: <abi/p3.h>
\*/
#include <abi/p3.h>

#if defined(_MSC_VER) && !defined(__GNUC__)
#include <compilers/visualc/align1.h>
#endif

#if defined(_MSC_VER) && !defined(__GNUC__)
#include <compilers/visualc/alignx.h>
#endif

typedef enum {
	P3_AUTH_MODE_SESSIONS	= (1<< 0),	/* Do Session authentication */
	P3_AUTH_MODE_HASH_PWD	= (1<< 1),
	P3_AUTH_MODE_HASH_USER	= (1<< 2),
	P3_AUTH_MODE_CMD_DATA	= (1<< 3),	/* Do Encrypt CMD DATA messages */
} p3_if_attrs_e;

typedef struct {
	uint32_t	rand0;
	uint32_t	rand1;

	uint64_t	serial;
} __attribute__((packed)) nonce128_t;
#endif  /* __ABI_AUTH_H__ */
