#ifndef __MATH_SWAP_FUNCTIONS__
#define __MATH_SWAP_FUNCTIONS__
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
 * Header file for aligment functions and macros
\*/
#include <endian.h>
#include <asm/byteorder.h>

#ifdef CPU_TO_LE64
#warning CPU_TO_LE64 alredy defined somewere!
#undef	CPU_TO_LE64
#endif

#ifdef CPU_TO_LE32
#warning CPU_TO_LE32 alredy defined somewere!
#undef	CPU_TO_LE32
#endif

#ifdef CPU_TO_LE16
#warning CPU_TO_LE16 alredy defined somewere!
#undef	CPU_TO_LE16
#endif

#define CPU_CONST_TO_LE16(V)  ((0xff&((V)>>8)) + (0xff00&((V)<<8)))
#define CPU_CONST_TO_LE32(V)  ( \
	(0x000000ff&((V)>>24))+ \
	(0x0000ff00&((V)>> 8))+ \
	(0x00ff0000&((V)<< 8))+ \
	(0xff000000&((V)<<24)))

#define LE16_TO_CPU(a) CPU_TO_LE16(a)
#define LE32_TO_CPU(a) CPU_TO_LE32(a)
#define LE64_TO_CPU(a) CPU_TO_LE64(a)

#if __BYTE_ORDER==__BIG_ENDIAN

#ifdef __linux__

#define CPU_TO_LE16(a) (__builtin_constant_p(a) \
				? CPU_CONST_TO_LE16(a) \
				: __cpu_to_le16(a))
#define CPU_TO_LE32(a) (__builtin_constant_p(a) \
				? CPU_CONST_TO_LE32(a) \
				: __cpu_to_le32(a))
#define CPU_TO_LE64(a) (__builtin_constant_p(a) \
				? CPU_CONST_TO_LE64(a) \
				: __cpu_to_le64(a))

#else
#define CPU_TO_LE16(V)  CPU_CONST_TO_LE16(V)
#define CPU_TO_LE32(V)  CPU_CONST_TO_LE32(V)
#define CPU_TO_LE64(V)  CPU_CONST_TO_LE64(V)
#endif

#elif __BYTE_ORDER==__LITTLE_ENDIAN

#define CPU_TO_LE16(V)  (V)
#define CPU_TO_LE32(V)  (V)

#else
#error "__BYTE_ORDER undefined, please check your includes"
#endif

#endif
