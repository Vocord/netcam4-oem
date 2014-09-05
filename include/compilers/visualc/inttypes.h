#ifndef __COMPILERS_VISUALC_INTTYPES_H__
#define __COMPILERS_VISUALC_INTTYPES_H__
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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __inline__
#define __inline__ __forceinline
#endif

#ifndef __func__
#define __func__ __FUNCTION__
#endif

typedef unsigned __int8  uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;

typedef          __int8  int8_t;
typedef          __int16 int16_t;
typedef          __int32 int32_t;
typedef          __int64 int64_t;

#define S_IRUSR	_S_IREAD 
#define S_IWUSR	_S_IWRITE 
#define S_IROTH	_S_IREAD 
#define S_IWOTH	_S_IWRITE 

#ifdef __cplusplus
};
#endif
#endif
