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
 *       Author: alexr, mostly based on <linux/compiler.h> 
 *    Copyright: (c) 2000 Vocord Telecommunications Ltd
 *      License: LGPL
 *   
 *
 *  tab-size=8
 *
 *  $Id: trace.h,v 1.8 2005/03/05 12:22:31 alexr Exp $
 *
\*/
#ifndef __COMPILER_H__
#define __COMPILER_H__ 1

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
#ifndef __ASSEMBLY__
#ifndef _weak_alias
#define _weak_alias(name, aliasname) \
  extern __typeof (name) aliasname __attribute__ ((weak, alias (#name)));
#endif

#if __GNUC__ > 3
//# include <compiler-gcc+.h>	/* catch-all for GCC 4, 5, etc. */

#ifndef __KERNEL__
//#define inline			__inline__ __attribute__((always_inline))
#define __inline__		__inline__ __attribute__((always_inline))
#define __inline		__inline__ __attribute__((always_inline))
#endif

#ifndef __deprecated
#define __deprecated		__attribute__((deprecated))
#endif

#ifndef __attribute_used__ 
#define __attribute_used__	__attribute__((__used__))
#endif
    
#ifndef __attribute_pure__
#define __attribute_pure__	__attribute__((pure))
#endif

#elif __GNUC__ == 3
//# include <compiler-gcc3.h>	/* catch-all for GCC 3.x.x */
#ifndef __attribute_pure__
#define __attribute_pure__	__attribute__((pure))
#endif

#if __GNUC_MINOR__ > 0
#ifndef __deprecated
# define __deprecated	__attribute__((deprecated))
#endif
#endif

#if __GNUC_MINOR__ >= 1
#ifndef inline
# define inline         __inline__ __attribute__((always_inline))
# define __inline__     __inline__ __attribute__((always_inline))
# define __inline       __inline__ __attribute__((always_inline))
#endif
#endif

#if  __GNUC_MINOR__ >= 3
#ifndef __attribute_used__ 
#define __attribute_used__	__attribute__((__used__))
#endif
#else
#ifndef __attribute_used__ 
#define __attribute_used__	__attribute__((__unused__))
#endif
#endif /* __GNUC_MINOR__ >= 3 */
#endif

#elif __GNUC__ == 2
# error Sorry, your compiler is too old please use GCC 3+ compilers.
#else
# error Sorry, your compiler is too old/not recognized.
#endif
#define __COMPILER_DEFINED__	1
#endif

/* Intel compiler defines __GNUC__. So we will overwrite implementations
 * coming from above header files here
 */
#ifdef __INTEL_COMPILER
# error Sorry, INTEL ECC compiler currently not supported.
//# include <linux/compiler-intel.h>
#ifdef __ECC

/* Some compiler specific definitions are overwritten here
 * for Intel ECC compiler
 */

//#include <asm/intrinsics.h>

/* Intel ECC compiler doesn't support gcc specific asm stmts.
 * It uses intrinsics to do the equivalent things.
 */
#undef barrier
#undef RELOC_HIDE

#define barrier() __memory_barrier()

#define RELOC_HIDE(ptr, off)					\
  ({ unsigned long __ptr;					\
     __ptr = (unsigned long) (ptr);				\
    (typeof(ptr)) (__ptr + (off)); })

#endif
#define __COMPILER_DEFINED__	2
#endif

/*
 * Generic compiler-dependent macros required for kernel
 * build go below this comment. Actual compiler/compiler version
 * specific implementations come from the above header files
 */

#ifndef __COMPILER_DEFINED__
#if defined(_MSC_VER) && !defined(__GNUC__)
#endif

#ifndef likely
#define likely(x)		(x)
#endif

#ifndef unlikely
#define unlikely(x)		(x)
#endif

#define __attribute__(x)
      
#else
#ifndef likely
#define likely(x)	__builtin_expect((long)!!(x), 1l)
#endif

#ifndef unlikely
#define unlikely(x)	__builtin_expect((long)!!(x), 0l)
#endif
      
#endif
/*
 * Allow us to mark functions as 'deprecated' and have gcc emit a nice
 * warning for each use, in hopes of speeding the functions removal.
 * Usage is:
 * 		int __deprecated foo(void)
 */
#ifndef __deprecated
# define __deprecated		/* unimplemented */
#endif

/*
 * Allow us to avoid 'defined but not used' warnings on functions and data,
 * as well as force them to be emitted to the assembly file.
 *
 * H
 * As of gcc 3.3, static functions that are not marked with attribute((used))
 * may be elided from the assembly file.  As of gcc 3.3, static data not so
 * marked will not be elided, but this may change in a future gcc version.
 *
 * In prior versions of gcc, such functions and data would be emitted, but
 * would be warned about except with attribute((unused)).
 */
#ifndef __attribute_used__
# define __attribute_used__	/* unimplemented */
#endif

/*
 * From the GCC manual:
 *
 * Many functions have no effects except the return value and their
 * return value depends only on the parameters and/or global
 * variables.  Such a function can be subject to common subexpression
 * elimination and loop optimization just as an arithmetic operator
 * would be.
 * [...]
 */
#ifndef __attribute_pure__
# define __attribute_pure__	/* unimplemented */
#endif

/* Optimization barrier */
#ifndef barrier
# define barrier() __memory_barrier()
#endif

#ifndef RELOC_HIDE
# define RELOC_HIDE(ptr, off)					\
  ({ unsigned long __ptr;					\
     __ptr = (unsigned long) (ptr);				\
    (typeof(ptr)) (__ptr + (off)); })
#endif

#ifdef __cplusplus
}
#endif

#define ARRAY_ELEMS(X)	(sizeof(X)/sizeof(X[0]))

#ifndef offsetof
#define offsetof(TYPE, MEMB) ((size_t) &((TYPE *)0)->MEMB)
#endif

#endif
