#ifndef __MATH_BIT_OPS_H__
#define __MATH_BIT_OPS_H__
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
 * Description:
 *	various functions for bit operations on bytes or bit flow
 *
 *  Authors:
 *	Alexander V. Rostovtsev <alexr@vocord.com> (2005)
 *
\*/
#include <inttypes.h>
#include <netinet/in.h>

typedef struct bits32be_s {
	uint32_t b00	:1;
	uint32_t b01	:1;
	uint32_t b02	:1;
	uint32_t b03	:1;
	uint32_t b04	:1;
	uint32_t b05	:1;
	uint32_t b06	:1;
	uint32_t b07	:1;
	uint32_t b08	:1;
	uint32_t b09	:1;
	uint32_t b10	:1;
	uint32_t b11	:1;
	uint32_t b12	:1;
	uint32_t b13	:1;
	uint32_t b14	:1;
	uint32_t b15	:1;
	uint32_t b16	:1;
	uint32_t b17	:1;
	uint32_t b18	:1;
	uint32_t b19	:1;
	uint32_t b20	:1;
	uint32_t b21	:1;
	uint32_t b22	:1;
	uint32_t b23	:1;
	uint32_t b24	:1;
	uint32_t b25	:1;
	uint32_t b26	:1;
	uint32_t b27	:1;
	uint32_t b28	:1;
	uint32_t b29	:1;
	uint32_t b30	:1;
	uint32_t b31	:1;
} bits32be_t;

typedef struct {
  uint32_t	mask;
  char		*descr;
} mask32_description_t;

static inline unsigned count_zeros(unsigned v) __attribute__((unused));
static inline unsigned count_zeros(unsigned v)
{
	unsigned	e = 0;
	unsigned	m = 1;

	while(e<32) {
		if(v & m)
			break;

		m = m*2 +1;
		e++;
	}

	return e & 0x1f; 
}


static inline int bit_reverse8(int in) __attribute__((unused));
static inline int bit_reverse8(int in)
{
    int out;
    out  = (in>>7) & 0x01;
    out |= (in>>5) & 0x02;
    out |= (in>>3) & 0x04;
    out |= (in>>1) & 0x08;
    out |= (in<<1) & 0x10;
    out |= (in<<3) & 0x20;
    out |= (in<<5) & 0x40;
    out |= (in<<7) & 0x80;

    return out;
}

static inline unsigned bit_reverse8u(unsigned val) __attribute__((unused));
static inline unsigned bit_reverse8u(unsigned val)
{
	return 
	    ((val>>7) & 0x01) |
	    ((val>>5) & 0x02) |
	    ((val>>3) & 0x04) |
	    ((val>>1) & 0x08) |
	    ((val<<1) & 0x10) |
	    ((val<<3) & 0x20) |
	    ((val<<5) & 0x40) |
	    ((val<<7) & 0x80) ;
}

static inline unsigned bit_reverse32u(unsigned val) __attribute__((unused));
static inline unsigned bit_reverse32u(unsigned val)
{
	return
	    (bit_reverse8u(val>> 0)<<24) |
	    (bit_reverse8u(val>> 8)<<16) |
	    (bit_reverse8u(val>>16)<< 8) |
	    (bit_reverse8u(val>>24)<< 0) ;
}

#if PPC
#define BIT_INIT(O, I, S, D)	{					\
	asm volatile(							\
	    "rlwinm %0, %1, %2, %3, %4"					\
	    :"=r"(O)							\
	    :"r"(I), "n"(((S)+1)&31), "n"(D), "n"(D)			\
	);								\
}

/** Copyes bit S from I into O in bit D ie O[D] = I[S]
 *  
 *   \param I     uint_32_t source of bit S
 *   \param O     uint_32_t destination to copy bit I[S]
 *   \param S     source bit number
 *   \param D     destination bit number
 *  
\*/
#define BIT_COPY(O, I, S, D)	{					\
	asm (								\
	    "rlwimi %0, %1, %2, %3, %4"					\
	    :"+r"(O)							\
	    :"r"(I), "n"((((S)+32-(D))&31)), "n"(D), "n"(D)		\
	);								\
}

#define BITS_COPY(O, I, S, D, NUM) {					\
	asm (								\
	    "rlwimi %0, %1, %2, %3, %4"					\
	    :"+r"(O)							\
	    :"r"(I), "n"((((S)+32-(D))&31)), "n"(D), "n"(D+NUM-1)	\
	);								\
}
#else
#if CHECK_UNOPTIMIZED
#warning "BIT_COPY and BIT_INIT implemented nonoptimaly on non PPC arch"
#endif
#define BIT_INIT(O, I, S, D)	{					\
	O = (((I)>>(S)) & 1) << (D);					\
}
#define BIT_COPY(O, I, S, D)	{								\
	O = (O & ~(1<<(31-D))) | ((((I)>>(31-S)) & 1) << (31-D));	\
}

#endif

#ifdef __PPC__

#define LOADU32(S,V)	asm          ("\tlwzu %1, 0x4(%0)\n" :"+b"(S),"=r"(V))
#define LOADU32V(S,V)	asm volatile ("\tlwzu %1, 0x4(%0)\n" :"+b"(S),"=r"(V))
#define LOADU16(S,V)	asm          ("\tlhzu %1, 0x2(%0)\n" :"+b"(S),"=r"(V))
#define LOADU16V(S,V)	asm volatile ("\tlhzu %1, 0x2(%0)\n" :"+b"(S),"=r"(V))
#define LOADU8(S,V)	asm          ("\tlbzu %1, 0x1(%0)\n" :"+b"(S),"=r"(V))
#define LOADU8V(S,V)	asm volatile ("\tlbzu %1, 0x1(%0)\n" :"+b"(S),"=r"(V))

#define STOREU32(D,V)	asm          ("\tstwu %1, 4+%0\n" : "+o"(*D) : "r"(V))
#define STOREU32V(D,V)	asm volatile ("\tstwu %1, 4+%0\n" : "+o"(*D) : "r"(V))
#define STOREU16(D,V)	asm          ("\tsthu %1, 2+%0\n" : "+o"(*D) : "r"(V))
#define STOREU16V(D,V)	asm volatile ("\tsthu %1, 2+%0\n" : "+o"(*D) : "r"(V))
#define STOREU8(D,V)	asm          ("\tstbu %1, 1+%0\n" : "+o"(*D) : "r"(V))
#define STOREU8V(D,V)	asm volatile ("\tstbu %1, 1+%0\n" : "+o"(*D) : "r"(V))

#else

#define LOADU32(S,V)	{ S++;  V = *S; }
#define LOADU16(S,V)	{ S++;  V = *S; }
#define LOADU8(S,V)	{ S++;  V = *S; }

#define STOREU32(D,V)	{ D++; *D = V; }
#define STOREU16(D,V)	{ D++; *D = V; }
#define STOREU8(D,V)	{ D++; *D = V; }

#define LOADU32V(S,V)	LOADU32(S,V)
#define LOADU16V(S,V)	LOADU16(S,V)
#define LOADU8V(S,V)	LOADU8(S,V)

#define STOREU32V(D,V)	STOREU32(D,V)
#define STOREU16V(D,V)	STOREU16(D,V)
#define STOREU8V(D,V)	STOREU8(D,V) 

#endif

#if 0
#ifdef __GCC__
#else
#define ___swab16(x) \
({ \
	__u16 __x = (x); \
	((__u16)( \
		(((__u16)(__x) & (__u16)0x00ffU) << 8) | \
		(((__u16)(__x) & (__u16)0xff00U) >> 8) )); \
})
#define __CPU_TO_LE16(X)	\
	(__builtin_constant_p((__u32)(x)) ? \
	 ___swab32((x)) : \
	 __fswab32((x)))
#endif
#endif

/** Returns no less then 25bits specified by size, at offs bits
 *  offset from data buffer pointer d
 *  
 *  Assumes that:
 *   1. (offs+size) < 32
 *   2. d[3+offs>>3] is present memory
 *   
 *   \param d     pointer to data buffer
 *   \param offs offset in bits from data bufer start to desired value
 *   \param size size of desired value in bits 
 *  
\*/
static inline uint32_t get_bits25_32(uint8_t *d, unsigned int offs, unsigned int size) __attribute__((unused));
static inline uint32_t get_bits25_32(uint8_t *d, unsigned int offs, unsigned int size)
{
	uint32_t	v = ntohl(*(uint32_t*)(d + (offs>>3)));

	return ((v<<(offs&7))>>(32-size));
}

/** Sets no less then 25bits specified by size, at offs bits
 *  offset from data buffer pointer d
 *  
 *  Assumes that:
 *   1. (offs+size) < 32
 *   2. d[3+offs>>3] is present memory
 *   
 *   \param d     pointer to data buffer
 *   \param offs offset in bits from data bufer start to desired value
 *   \param size size of desired value in bits 
 *   \param val   size bits to be set right aligned, bits [31:size] should be
 *  
\*/
static inline void set_bits25_32(uint8_t *d, unsigned int offs, unsigned int size, uint32_t val) __attribute__((unused));
static inline void set_bits25_32(uint8_t *d, unsigned int offs, unsigned int size, uint32_t val)
{
	uint32_t	v;

	/* */
	d += (offs>>3);
	offs &= 7;
	
	v = ntohl(*(uint32_t*)d);

	val |= ((v>>(32-offs))<<(size));
	v = (val <<(32-offs-size)) | ((v<<(offs+size))>>(offs+size));

	*(uint32_t*)d = htonl(v);
	return;
}

static inline void bintoa(char *s, uint32_t val, int i) __attribute__((unused));

static inline void bintoa(char *s, uint32_t val, int i)
{
	i--;
	for(; i>=0; i--) {
		s[i] = ((val & (1U<<i)) && 1) + '0';
	}
}
#endif
/* __MATH_BIT_OPS_H__ */
