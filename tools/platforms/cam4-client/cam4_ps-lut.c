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

#include <arpa/inet.h>
#include "cam4_ps-lut.h"

/*http://wiki.vocord.com/spec-fmt-video#5_raw_raw_curve_ */
/*********************************************************/
/*  Reduce bits from 16 to 8	 			 */
/*********************************************************/
/*							 */
/* y = ax + b 						 */
/* a - curve_s	   [0..4096] (curve_s<16 - a<1 		 */
/*		             8 bit integer part		 */
/*		             4  bit float part		 */
/* b - curve_shift [0..4096]				 */
/*							 */
/* we have 3 sectors in graph				 */
/* curve_th - offset form origin			 */
/* of each part of the graph				 */
/*							 */
/*********************************************************/
static inline uint_fast8_t lut(
	uint_fast16_t v,
	uint_fast16_t curve_s0,
	uint_fast16_t curve_s1,
	uint_fast16_t curve_s2,
	uint_fast16_t curve_th0,
	uint_fast16_t curve_th1,
	uint_fast16_t curve_th2,
	uint_fast16_t curve_shift0,
	uint_fast16_t curve_shift1,
	uint_fast16_t curve_shift2
)
{
	if(v < curve_th0 )
		return  (((curve_s0 * v) + curve_shift0) >> 4);

	if (v < curve_th1)
		return (((curve_s1 * v) + curve_shift1) >> 4);

	return (((curve_s2 * v) + curve_shift2) >> 4);
}

static inline void unpack_8(
	uint16_t *s16,
	uint32_t *s,
	size_t	 size)
{
    	uint8_t *d; int i;
	d = (uint8_t*)s;
	for(i=0;i<size;i++)
    		s16[i] = d[i];
}

static inline void LUT_10_to_8(
	uint16_t *d16,
	uint32_t *s,
	size_t	 size,
	uint16_t curve_s0,
	uint16_t curve_s1,
	uint16_t curve_s2,
	uint16_t curve_th0,
	uint16_t curve_th1,
	uint16_t curve_th2,
	uint16_t curve_shift0,
	uint16_t curve_shift1,
	uint16_t curve_shift2
)
{
	uint32_t	v0, v1, v2, v3, v4, v;
	uint8_t		*d = (uint8_t *)s;

	while(size>=20) {
		size -= 20;

		v0 = ntohl(s[0]);
		v1 = ntohl(s[1]);
		v2 = ntohl(s[2]);
		v3 = ntohl(s[3]);
		v4 = ntohl(s[4]);

		d16[0] = v = v0>>22;
		d[0] = lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);

		d16[1] = v = (v0>>12) & 0x3ff;
		d[1] = lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);

		d16[2] = v = (v0>> 2) & 0x3ff;
		d[2] = lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);

		d16[3] = v = ((v0<<8) & 0x3ff) | (v1>>24);
		d[3] = lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);

		d16[4] = v = (v1>>14) & 0x3ff;
		d[4] = lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);

		d16[5] = v = (v1>>4) & 0x3ff;
		d[5] = lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);

		d16[6] = v = ((v1<<6) & 0x3ff) | (v2>>26);
		d[6]= lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);

		d16[7] = v = (v2>>16) & 0x3ff;
		d[7] = lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);

		d16[8] = v = (v2>>6) & 0x3ff;
		d[8] = lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);

		d16[9] = v = ((v2<<4) & 0x3ff) | (v3>>28);
		d[9] = lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);
		d16[10] = v = (v3>>18) & 0x3ff;
		d[10] = lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);

		d16[11] = v = (v3>>8) & 0x3ff;
		d[11] = lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);

		d16[12] = v = ((v3<<2) & 0x3ff) | (v4>>30);
		d[12]= lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);

		d16[13] = v = (v4>>20) & 0x3ff;
		d[13] = lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);

		d16[14] = v = (v4>>10) & 0x3ff;
		d[14] = lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);

		d16[15] = v = (v4>>0) & 0x3ff;
		d[15] = lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);
		d16+=16;
		d+=16;
		s+=5;
	}
}

static inline void LUT_12_to_8(
	uint16_t *d16,
	uint32_t *s,
	size_t	 size,
	uint16_t curve_s0,	uint16_t curve_s1,	uint16_t curve_s2,
	uint16_t curve_th0,	uint16_t curve_th1,	uint16_t curve_th2,
	uint16_t curve_shift0,	uint16_t curve_shift1,	uint16_t curve_shift2
)
{
	uint32_t	v0, v1, v2;
	uint8_t		*d = (uint8_t *)s;
	uint16_t 	v;
	while(size >= 12) {
		size -= 12;

		v0 = ntohl(s[0]);
		v1 = ntohl(s[1]);
		v2 = ntohl(s[2]);

		d16[0] = v = v0>>20;
		d[0] = lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);

		d16[1] = v = (v0>>8) & 0xfff;
		d[1] = lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);

		d16[2] = v = ((v0<<4) & 0xfff) | (v1>>28);
		d[2] = lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);

		d16[3] = v = (v1>>16) & 0xfff;
		d[3] = lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);

		d16[4] = v = (v1>>4) & 0xfff;
		d[4] = lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);

		d16[5] = v = ((v1<<8) & 0xfff) | (v2>>24);
		d[5]= lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);
		d16[6] = v = (v2>>12) & 0xfff;
		d[6] = lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);
		d16[7] = v = (v2>>0) & 0xfff;
		d[7] = lut(v,
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);

		d+=8;
		d16+=8;
		s+=3;
	}
}

static inline void LUT_16_to_8(
	uint16_t *d16,
	uint16_t *s,
	size_t	 size,
	uint16_t curve_s0,	uint16_t curve_s1,	uint16_t curve_s2,
	uint16_t curve_th0,	uint16_t curve_th1,	uint16_t curve_th2,
	uint16_t curve_shift0,	uint16_t curve_shift1,	uint16_t curve_shift2
)
{
	uint8_t		*d = (uint8_t *)s;

	while(size>0) {
	    	d16[0] = ntohs(s[0]);
		d[0] = lut(d16[0],
		    curve_s0, curve_s1, curve_s2,
		    curve_th0, curve_th1, curve_th2,
		    curve_shift0, curve_shift1, curve_shift2
		);

		s++;
		d++;
		d16++;
		size -= 2;
	}
}

void cam4_rd_do_LUT(
       uint16_t        *raw16,
       void            *img,
       uint32_t        fsize
)
{
	size_t	 size = fsize & 0xfffffff;
	switch((fsize>>28) & 7) {
	    case 0:	/* 8 */
		unpack_8(raw16,(uint32_t *)img, size);
		break;

	    case 1:	/* 10 */
		LUT_10_to_8(raw16, (uint32_t *)img, size,
			    4,		// curve_s0
			    0,		// curve_s1
			    0,		// curve_s2

			    1024,	// curve_th0
			    0,		// curve_th1
			    0,		// curve_th2

			    0,		// curve_shift0
			    0,		// curve_shift1
			    0 		// curve_shift2
		);
		break;

	    case 2:	/* 12 */
		LUT_12_to_8(raw16, (uint32_t *)img, size,
			    1,		// curve_s0
			    0,		// curve_s1
			    0,		// curve_s2

			    4096,	// curve_th0
			    0,		// curve_th1
			    0,		// curve_th2

			    0,		// curve_shift0
			    0,		// curve_shift1
			    0 		// curve_shift2
		);
		break;

	    case 3:	/* 14 */
		break;

	    case 4:	/* 16 */
		LUT_16_to_8(raw16, (uint16_t *)img, size,
			    1,		// curve_s0
			    0,		// curve_s1
			    0,		// curve_s2

			    65535,	// curve_th0
			    0,		// curve_th1
			    0,		// curve_th2

			    0,		// curve_shift0
			    0,		// curve_shift1
			    0 		// curve_shift2
		);
		break;
	}
}


