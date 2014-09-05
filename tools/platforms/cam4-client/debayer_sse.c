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

#include <stdint.h>

#include <trace.h>

#include "debayer_api.h"

char* trace_prefix = "debayer_sse: ";

#define cpuid(func,ax,bx,cx,dx)\
	__asm__ __volatile__ ("cpuid":\
	"=a" (ax), "=b" (bx), "=c" (cx), "=d" (dx) : "a" (func));

void debayerRGB_mode0_sse2(uint8_t *dst, uint8_t *src, int dim_x, int dim_y,int startx,int starty,int ww,int wh);
void debayerRGB_mode1_sse2(uint8_t *dst, uint8_t *src, int dim_x, int dim_y,int startx,int starty,int ww,int wh);
void debayerRGB_mode2_sse2(uint8_t *dst, uint8_t *src, int dim_x, int dim_y,int startx,int starty,int ww,int wh);
void debayerRGB_mode3_sse2(uint8_t *dst, uint8_t *src, int dim_x, int dim_y,int startx,int starty,int ww,int wh);
void debayerRGB_mode0_mmx(uint8_t *dst, uint8_t *src, int dim_x, int dim_y,int startx,int starty,int ww,int wh);
void debayerRGB_mode1_mmx(uint8_t *dst, uint8_t *src, int dim_x, int dim_y,int startx,int starty,int ww,int wh);
void debayerRGB_mode2_mmx(uint8_t *dst, uint8_t *src, int dim_x, int dim_y,int startx,int starty,int ww,int wh);
void debayerRGB_mode3_mmx(uint8_t *dst, uint8_t *src, int dim_x, int dim_y,int startx,int starty,int ww,int wh);

static const int16_t coefrY=9797; // 0.299
static const int16_t coefgY= 19234; // 0.587
static const int16_t coefbY= 3735; // 0.114

static const int16_t coefCr= 23363;// 0.713
static const int16_t coefCb= 18481;// 0.564


static inline int16_t mul16x16s(int16_t a,int16_t b)
{
	return (int16_t)(((int32_t)a*b) >> 15);
} 

typedef  char v16qi __attribute__ ((vector_size (16))) __attribute__ ((aligned(16)));
typedef  char v8qi __attribute__ ((vector_size (8))) __attribute__ ((aligned(8)));


const v8qi v8_zero={0};
const v8qi v8_mul1={0x8b,0x4c,0x22,0x4b,0x8b,0x4c,0x22,0x4b};
const v8qi v8_mul2={0x22,0x4b,0x2f,0x1d,0x22,0x4b,0x2f,0x1d};
const v8qi v8_mul3={0x43,0x5b,0x00,0x00,0x43,0x5b,0x00,0x00};
const v8qi v8_mul4={0x00,0x00,0x31,0x48,0x00,0x00,0x31,0x48};
const v8qi v8_add1={0x80,0x00,0x00,0x00,0x80,0x00,0x00,0x00};
const v8qi v8_add2={0x00,0x00,0x80,0x00,0x00,0x00,0x80,0x00};

const v8qi v8_mul5={0x22,0x4b,0x8b,0x4c,0x22,0x4b,0x8b,0x4c};
const v8qi v8_mul6={0x2f,0x1d,0x22,0x4b,0x2f,0x1d,0x22,0x4b};
const v8qi v8_mul7={0x00,0x00,0x43,0x5b,0x00,0x00,0x43,0x5b};
const v8qi v8_mul8={0x31,0x48,0x00,0x00,0x31,0x48,0x00,0x00};
const v8qi v8_add3={0x00,0x00,0x80,0x00,0x00,0x00,0x80,0x00};
const v8qi v8_add4={0x80,0x00,0x00,0x00,0x80,0x00,0x00,0x00};


const v16qi v16_zero={0};
const v16qi v16_mul1={0x8b,0x4c,0x22,0x4b,0x8b,0x4c,0x22,0x4b,0x8b,0x4c,0x22,0x4b,0x8b,0x4c,0x22,0x4b};
const v16qi v16_mul2={0x22,0x4b,0x2f,0x1d,0x22,0x4b,0x2f,0x1d,0x22,0x4b,0x2f,0x1d,0x22,0x4b,0x2f,0x1d};
const v16qi v16_mul3={0x43,0x5b,0x00,0x00,0x43,0x5b,0x00,0x00,0x43,0x5b,0x00,0x00,0x43,0x5b,0x00,0x00};
const v16qi v16_mul4={0x00,0x00,0x31,0x48,0x00,0x00,0x31,0x48,0x00,0x00,0x31,0x48,0x00,0x00,0x31,0x48};
const v16qi v16_add1={0x80,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x80,0x00,0x00,0x00};
const v16qi v16_add2={0x00,0x00,0x80,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x80,0x00};

const v16qi v16_mul5={0x22,0x4b,0x8b,0x4c,0x22,0x4b,0x8b,0x4c,0x22,0x4b,0x8b,0x4c,0x22,0x4b,0x8b,0x4c};
const v16qi v16_mul6={0x2f,0x1d,0x22,0x4b,0x2f,0x1d,0x22,0x4b,0x2f,0x1d,0x22,0x4b,0x2f,0x1d,0x22,0x4b};
const v16qi v16_mul7={0x00,0x00,0x43,0x5b,0x00,0x00,0x43,0x5b,0x00,0x00,0x43,0x5b,0x00,0x00,0x43,0x5b};
const v16qi v16_mul8={0x31,0x48,0x00,0x00,0x31,0x48,0x00,0x00,0x31,0x48,0x00,0x00,0x31,0x48,0x00,0x00};
const v16qi v16_add3={0x00,0x00,0x80,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x80,0x00};
const v16qi v16_add4={0x80,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x80,0x00,0x00,0x00};

void debayerRGB_mode0_sse2(uint8_t *dst, uint8_t *src, int dim_x, int dim_y,int startx,int starty,int ww,int wh)
{
  /*green1    -- red*/
  /*blue -- green2*/

	int8_t *tmp_ptr,*last_r;
	int tmpi;
	int window_width,window_height;
	int remain_size;
	v16qi *v16_src,*v16_src_sec,*v16_dst,v16_tmp1,v16_tmp2,v16_tmp3,v16_tmp4;
	v16qi v16_Y,v16_RG1,v16_G2B,v16_CbCr;
	v16qi v16_YC,v16_RGC;
	v16qi v16_YC_old,v16_RGC_old;
	int x,y;
	
	if(ww && wh)   // working with window in image
	{
	    window_width= ww;
	    window_height= wh-2;
	    remain_size= dim_x-ww;
	    v16_src= (v16qi *)(src+dim_x*(wh+starty-2)-16+startx+ww);
	    v16_src_sec= (v16qi *)((unsigned long)v16_src+dim_x);	   
	    v16_dst= (v16qi *)(dst+2*ww*(wh-2)-16);
	}
	else
	{
	    window_width= dim_x;
	    window_height= dim_y-2;		
	    remain_size=0;
	    v16_src= (v16qi *)(src+dim_x*(dim_y-1)-16);
	    v16_src_sec= (v16qi *)((unsigned long)v16_src+dim_x);
	    v16_dst= (v16qi *)(dst+2*dim_x*(dim_y-2)-16);
	}

	y=window_height;
	do
	{
		x= window_width;
		v16_YC_old=v16_RGC_old=v16_zero;
		do
		{
       
		v16_G2B= __builtin_ia32_punpckhbw128 (*v16_src, v16_zero);
        	v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul2, v16_G2B); // G2 and B 
		v16_RG1= __builtin_ia32_punpckhbw128 (*v16_src_sec, v16_zero);
		v16_RGC= __builtin_ia32_pslldqi128 (v16_RG1, 112);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul1, v16_RG1); // R and G1
		v16_tmp1= __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2);
		v16_YC=   __builtin_ia32_pslldqi128 (v16_tmp1, 112);
		v16_tmp2= __builtin_ia32_psrldqi128 (v16_tmp1, 16);   
		v16_tmp2= __builtin_ia32_por128 (v16_tmp2, v16_YC_old);// Y Carry
		v16_Y=    __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2); // Y 
		v16_YC_old= v16_YC;

		v16_tmp1= __builtin_ia32_psrldqi128 (v16_RG1, 16); 		
		v16_tmp1= __builtin_ia32_por128 (v16_tmp1, v16_RGC_old);// R Carry
		v16_tmp1= __builtin_ia32_psubsw128 (v16_tmp1,v16_Y);
		v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul7, v16_tmp1); 
		v16_tmp1= __builtin_ia32_psllwi128 (v16_tmp1,1);
		v16_tmp1= __builtin_ia32_paddsw128 (v16_add3, v16_tmp1);
		v16_RGC_old=v16_RGC;

		v16_tmp2= __builtin_ia32_psrldqi128 (v16_G2B, 16);
		v16_tmp2= __builtin_ia32_psubsw128 (v16_tmp2,v16_Y);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul8, v16_tmp2);
		v16_tmp2= __builtin_ia32_psllwi128 (v16_tmp2,1); 
		v16_tmp2= __builtin_ia32_paddsw128 (v16_add4, v16_tmp2); 
		v16_CbCr= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		v16_tmp1= __builtin_ia32_punpckhwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpckhwd128 (v16_CbCr,v16_zero );
		v16_tmp3= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);
	
		v16_tmp1= __builtin_ia32_punpcklwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpcklwd128 (v16_CbCr,v16_zero );
		v16_tmp4= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);


		*v16_dst--= __builtin_ia32_packuswb128 (v16_tmp4, v16_tmp3);


		v16_G2B= __builtin_ia32_punpcklbw128 (*v16_src--, v16_zero);
        	v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul2, v16_G2B); //G2 and B 
		v16_RG1= __builtin_ia32_punpcklbw128 (*v16_src_sec--, v16_zero);
		v16_RGC= __builtin_ia32_pslldqi128 (v16_RG1, 112);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul1, v16_RG1); // R and G1
		v16_tmp1= __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2);
		v16_YC=   __builtin_ia32_pslldqi128 (v16_tmp1, 112);
		v16_tmp2= __builtin_ia32_psrldqi128 (v16_tmp1, 16);   
		v16_tmp2= __builtin_ia32_por128 (v16_tmp2, v16_YC_old);// Y Carry
		v16_Y=    __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2); // Y 
		v16_YC_old= v16_YC;

		v16_tmp1= __builtin_ia32_psrldqi128 (v16_RG1, 16); 		
		v16_tmp1= __builtin_ia32_por128 (v16_tmp1, v16_RGC_old);// R Carry
		v16_tmp1= __builtin_ia32_psubsw128 (v16_tmp1,v16_Y);
		v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul7, v16_tmp1); 
		v16_tmp1= __builtin_ia32_psllwi128 (v16_tmp1,1);
		v16_tmp1= __builtin_ia32_paddsw128 (v16_add3, v16_tmp1);
		v16_RGC_old=v16_RGC;

		v16_tmp2= __builtin_ia32_psrldqi128 (v16_G2B, 16);
		v16_tmp2= __builtin_ia32_psubsw128 (v16_tmp2,v16_Y);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul8, v16_tmp2); 
		v16_tmp2= __builtin_ia32_psllwi128 (v16_tmp2,1);
		v16_tmp2= __builtin_ia32_paddsw128 (v16_add4, v16_tmp2); 
		v16_CbCr= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		v16_tmp1= __builtin_ia32_punpckhwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpckhwd128 (v16_CbCr,v16_zero);
		v16_tmp3= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);
	
		v16_tmp1= __builtin_ia32_punpcklwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpcklwd128 (v16_CbCr,v16_zero);
		v16_tmp4= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		*v16_dst--= __builtin_ia32_packuswb128 (v16_tmp4, v16_tmp3);
		}
		while(x-=16);

		v16_src=(v16qi *)((long)v16_src-remain_size);
		v16_src_sec=(v16qi *)((long)v16_src_sec-remain_size);
		tmp_ptr= (int8_t *)(v16_dst+1);
		*(int16_t *)tmp_ptr= *(int16_t *)(tmp_ptr+2);
		tmp_ptr+=(window_width<<1);
		*(int16_t *)(tmp_ptr-2)=*(int16_t *)(tmp_ptr-4);
		last_r= (int8_t *)v16_src+14+(dim_x<<1);
		tmpi= (uint8_t)*last_r-(uint8_t)*(tmp_ptr-1);
		*(tmp_ptr-2)= (mul16x16s(tmpi,coefCr)+128);
		x= window_width;
		v16_YC_old=v16_RGC_old=v16_zero;

		do
		{
       
		v16_RG1= __builtin_ia32_punpckhbw128 (*v16_src, v16_zero);
		v16_RGC= __builtin_ia32_pslldqi128 (v16_RG1, 112);
        	v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul1, v16_RG1); // R and G1 
		v16_G2B= __builtin_ia32_punpckhbw128 (*v16_src_sec, v16_zero);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul2, v16_G2B); // G2 and B
		v16_tmp1= __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2);
		v16_YC=   __builtin_ia32_pslldqi128 (v16_tmp1, 112);
		v16_tmp2= __builtin_ia32_psrldqi128 (v16_tmp1, 16);   
		v16_tmp2= __builtin_ia32_por128 (v16_tmp2, v16_YC_old);// Y Carry
		v16_Y=    __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2); // Y 
		v16_YC_old= v16_YC;

		v16_tmp1= __builtin_ia32_psrldqi128 (v16_RG1, 16); 		
		v16_tmp1= __builtin_ia32_por128 (v16_tmp1, v16_RGC_old);// R Carry
		v16_tmp1= __builtin_ia32_psubsw128 (v16_tmp1,v16_Y);
		v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul7, v16_tmp1); 
		v16_tmp1= __builtin_ia32_psllwi128 (v16_tmp1,1);
		v16_tmp1= __builtin_ia32_paddsw128 (v16_add3, v16_tmp1);
		v16_RGC_old=v16_RGC;

		v16_tmp2= __builtin_ia32_psrldqi128 (v16_G2B, 16); 
		v16_tmp2= __builtin_ia32_psubsw128 (v16_tmp2,v16_Y);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul8, v16_tmp2);
		v16_tmp2= __builtin_ia32_psllwi128 (v16_tmp2,1); 
		v16_tmp2= __builtin_ia32_paddsw128 (v16_add4, v16_tmp2);  
		v16_CbCr= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		v16_tmp1= __builtin_ia32_punpckhwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpckhwd128 (v16_CbCr,v16_zero );
		v16_tmp3= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);
	
		v16_tmp1= __builtin_ia32_punpcklwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpcklwd128 (v16_CbCr,v16_zero );
		v16_tmp4= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);


		*v16_dst--= __builtin_ia32_packuswb128 (v16_tmp4, v16_tmp3);


		v16_RG1= __builtin_ia32_punpcklbw128 (*v16_src--, v16_zero);
		v16_RGC= __builtin_ia32_pslldqi128 (v16_RG1, 112);
        	v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul1, v16_RG1); // R and G1 
		v16_G2B= __builtin_ia32_punpcklbw128 (*v16_src_sec--, v16_zero);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul2, v16_G2B); // G2 and B
		v16_tmp1= __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2);
		v16_YC=   __builtin_ia32_pslldqi128 (v16_tmp1, 112);
		v16_tmp2= __builtin_ia32_psrldqi128 (v16_tmp1, 16);
		v16_tmp2= __builtin_ia32_por128 (v16_tmp2, v16_YC_old);// Y Carry   
		v16_Y=    __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2); // Y 
		v16_YC_old= v16_YC;

		v16_tmp1= __builtin_ia32_psrldqi128 (v16_RG1, 16); 		
		v16_tmp1= __builtin_ia32_por128 (v16_tmp1, v16_RGC_old);// R Carry
		v16_tmp1= __builtin_ia32_psubsw128 (v16_tmp1,v16_Y);
		v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul7, v16_tmp1); 
		v16_tmp1= __builtin_ia32_psllwi128 (v16_tmp1,1);
		v16_tmp1= __builtin_ia32_paddsw128 (v16_add3, v16_tmp1);
		v16_RGC_old=v16_RGC;

		v16_tmp2= __builtin_ia32_psrldqi128 (v16_G2B, 16);
		v16_tmp2= __builtin_ia32_psubsw128 (v16_tmp2,v16_Y);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul8, v16_tmp2); 
		v16_tmp2= __builtin_ia32_psllwi128 (v16_tmp2,1);
		v16_tmp2= __builtin_ia32_paddsw128 (v16_add4, v16_tmp2); 
		v16_CbCr= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		v16_tmp1= __builtin_ia32_punpckhwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpckhwd128 (v16_CbCr,v16_zero);
		v16_tmp3= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);
	
		v16_tmp1= __builtin_ia32_punpcklwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpcklwd128 (v16_CbCr,v16_zero);
		v16_tmp4= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		*v16_dst--= __builtin_ia32_packuswb128 (v16_tmp4, v16_tmp3);
		}
		while(x-=16);
		v16_src=(v16qi *)((long)v16_src-remain_size);
		v16_src_sec=(v16qi *)((long)v16_src_sec-remain_size);
		tmp_ptr= (int8_t *)(v16_dst+1);
		*(int16_t *)tmp_ptr= *(int16_t *)(tmp_ptr+2);
		tmp_ptr+=(window_width<<1);
		*(int16_t *)(tmp_ptr-2)=*(int16_t *)(tmp_ptr-4);
		last_r= (int8_t *)v16_src+14+dim_x;
		tmpi= (uint8_t)*last_r-(uint8_t)*(tmp_ptr-1);
		*(tmp_ptr-2)= (mul16x16s(tmpi,coefCr)+128);

	}
	while(y-=2);
}



void debayerRGB_mode2_sse2(uint8_t *dst, uint8_t *src, int dim_x, int dim_y,int startx,int starty,int ww,int wh)
{
  /*blue -- green2*/
  /*green1    -- red*/

	int8_t *tmp_ptr,*last_r;
	int tmpi;
	int window_width,window_height;
	int remain_size;

	v16qi *v16_src,*v16_src_sec,*v16_dst,v16_tmp1,v16_tmp2,v16_tmp3,v16_tmp4;
	v16qi v16_Y,v16_RG1,v16_G2B,v16_CbCr;
	v16qi v16_YC,v16_RGC;
	v16qi v16_YC_old,v16_RGC_old;
	
	int x,y;

	if(ww && wh)   // working with window in image
	{
	    window_width= ww;
	    window_height= wh-2;
	    remain_size= dim_x-ww;
	    v16_src= (v16qi *)(src+dim_x*(wh+starty-2)-16+startx+ww);
	    v16_src_sec= (v16qi *)((unsigned long)v16_src+dim_x);
	    v16_dst= (v16qi *)(dst+2*ww*(wh-2)-16);	   
	}
	else
	{
	    window_width= dim_x;
	    window_height= dim_y-2;		
	    remain_size=0;
	    v16_src= (v16qi *)(src+dim_x*(dim_y-1)-16);
	    v16_src_sec= (v16qi *)((unsigned long)v16_src+dim_x);
	    v16_dst= (v16qi *)(dst+2*dim_x*(dim_y-2)-16);
	}

	y=window_height;
	do
	{
		x= window_width;
		v16_YC_old=v16_RGC_old=v16_zero;
		do
		{
       
		v16_RG1= __builtin_ia32_punpckhbw128 (*v16_src, v16_zero);
		v16_RGC= __builtin_ia32_pslldqi128 (v16_RG1, 112);
        	v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul1, v16_RG1); // R and G1 
		v16_G2B= __builtin_ia32_punpckhbw128 (*v16_src_sec, v16_zero);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul2, v16_G2B); // G2 and B
		v16_tmp1= __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2);
		v16_YC=   __builtin_ia32_pslldqi128 (v16_tmp1, 112);
		v16_tmp2= __builtin_ia32_psrldqi128 (v16_tmp1, 16);   
		v16_tmp2= __builtin_ia32_por128 (v16_tmp2, v16_YC_old);// Y Carry
		v16_Y=    __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2); // Y 
		v16_YC_old= v16_YC;

		v16_tmp1= __builtin_ia32_psrldqi128 (v16_RG1, 16); 		
		v16_tmp1= __builtin_ia32_por128 (v16_tmp1, v16_RGC_old);// R Carry
		v16_tmp1= __builtin_ia32_psubsw128 (v16_tmp1,v16_Y);
		v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul7, v16_tmp1); 
		v16_tmp1= __builtin_ia32_psllwi128 (v16_tmp1,1);
		v16_tmp1= __builtin_ia32_paddsw128 (v16_add3, v16_tmp1);
		v16_RGC_old=v16_RGC;

		v16_tmp2= __builtin_ia32_psrldqi128 (v16_G2B, 16); 
		v16_tmp2= __builtin_ia32_psubsw128 (v16_tmp2,v16_Y);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul8, v16_tmp2);
		v16_tmp2= __builtin_ia32_psllwi128 (v16_tmp2,1); 
		v16_tmp2= __builtin_ia32_paddsw128 (v16_add4, v16_tmp2);  
		v16_CbCr= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		v16_tmp1= __builtin_ia32_punpckhwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpckhwd128 (v16_CbCr,v16_zero );
		v16_tmp3= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);
	
		v16_tmp1= __builtin_ia32_punpcklwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpcklwd128 (v16_CbCr,v16_zero );
		v16_tmp4= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);


		*v16_dst--= __builtin_ia32_packuswb128 (v16_tmp4, v16_tmp3);


		v16_RG1= __builtin_ia32_punpcklbw128 (*v16_src--, v16_zero);
		v16_RGC= __builtin_ia32_pslldqi128 (v16_RG1, 112);
        	v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul1, v16_RG1); // R and G1 
		v16_G2B= __builtin_ia32_punpcklbw128 (*v16_src_sec--, v16_zero);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul2, v16_G2B); // G2 and B
		v16_tmp1= __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2);
		v16_YC=   __builtin_ia32_pslldqi128 (v16_tmp1, 112);
		v16_tmp2= __builtin_ia32_psrldqi128 (v16_tmp1, 16);
		v16_tmp2= __builtin_ia32_por128 (v16_tmp2, v16_YC_old);// Y Carry   
		v16_Y=    __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2); // Y 
		v16_YC_old= v16_YC;

		v16_tmp1= __builtin_ia32_psrldqi128 (v16_RG1, 16); 		
		v16_tmp1= __builtin_ia32_por128 (v16_tmp1, v16_RGC_old);// R Carry
		v16_tmp1= __builtin_ia32_psubsw128 (v16_tmp1,v16_Y);
		v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul7, v16_tmp1); 
		v16_tmp1= __builtin_ia32_psllwi128 (v16_tmp1,1);
		v16_tmp1= __builtin_ia32_paddsw128 (v16_add3, v16_tmp1);
		v16_RGC_old=v16_RGC;

		v16_tmp2= __builtin_ia32_psrldqi128 (v16_G2B, 16);
		v16_tmp2= __builtin_ia32_psubsw128 (v16_tmp2,v16_Y);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul8, v16_tmp2); 
		v16_tmp2= __builtin_ia32_psllwi128 (v16_tmp2,1);
		v16_tmp2= __builtin_ia32_paddsw128 (v16_add4, v16_tmp2); 
		v16_CbCr= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		v16_tmp1= __builtin_ia32_punpckhwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpckhwd128 (v16_CbCr,v16_zero);
		v16_tmp3= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);
	
		v16_tmp1= __builtin_ia32_punpcklwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpcklwd128 (v16_CbCr,v16_zero);
		v16_tmp4= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		*v16_dst--= __builtin_ia32_packuswb128 (v16_tmp4, v16_tmp3);
		}
		while(x-=16);
		
		v16_src=(v16qi *)((long)v16_src-remain_size);
		v16_src_sec=(v16qi *)((long)v16_src_sec-remain_size);
		tmp_ptr= (int8_t *)(v16_dst+1);
		*(int16_t *)tmp_ptr= *(int16_t *)(tmp_ptr+2);
		tmp_ptr+=(window_width<<1);
		*(int16_t *)(tmp_ptr-2)=*(int16_t *)(tmp_ptr-4);
		last_r= (int8_t *)v16_src+14+dim_x;
		tmpi= (uint8_t)*last_r-(uint8_t)*(tmp_ptr-1);
		*(tmp_ptr-2)= (mul16x16s(tmpi,coefCr)+128);
		x= window_width;
		v16_YC_old=v16_RGC_old=v16_zero;
		do
		{
       
		v16_G2B= __builtin_ia32_punpckhbw128 (*v16_src, v16_zero);
        	v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul2, v16_G2B); // G2 and B 
		v16_RG1= __builtin_ia32_punpckhbw128 (*v16_src_sec, v16_zero);
		v16_RGC= __builtin_ia32_pslldqi128 (v16_RG1, 112);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul1, v16_RG1); // R and G1
		v16_tmp1= __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2);
		v16_YC=   __builtin_ia32_pslldqi128 (v16_tmp1, 112);
		v16_tmp2= __builtin_ia32_psrldqi128 (v16_tmp1, 16);   
		v16_tmp2= __builtin_ia32_por128 (v16_tmp2, v16_YC_old);// Y Carry
		v16_Y=    __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2); // Y 
		v16_YC_old= v16_YC;

		v16_tmp1= __builtin_ia32_psrldqi128 (v16_RG1, 16); 		
		v16_tmp1= __builtin_ia32_por128 (v16_tmp1, v16_RGC_old);// R Carry
		v16_tmp1= __builtin_ia32_psubsw128 (v16_tmp1,v16_Y);
		v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul7, v16_tmp1); 
		v16_tmp1= __builtin_ia32_psllwi128 (v16_tmp1,1);
		v16_tmp1= __builtin_ia32_paddsw128 (v16_add3, v16_tmp1);
		v16_RGC_old=v16_RGC;

		v16_tmp2= __builtin_ia32_psrldqi128 (v16_G2B, 16);
		v16_tmp2= __builtin_ia32_psubsw128 (v16_tmp2,v16_Y);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul8, v16_tmp2);
		v16_tmp2= __builtin_ia32_psllwi128 (v16_tmp2,1); 
		v16_tmp2= __builtin_ia32_paddsw128 (v16_add4, v16_tmp2); 
		v16_CbCr= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		v16_tmp1= __builtin_ia32_punpckhwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpckhwd128 (v16_CbCr,v16_zero );
		v16_tmp3= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);
	
		v16_tmp1= __builtin_ia32_punpcklwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpcklwd128 (v16_CbCr,v16_zero );
		v16_tmp4= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);


		*v16_dst--= __builtin_ia32_packuswb128 (v16_tmp4, v16_tmp3);


		v16_G2B= __builtin_ia32_punpcklbw128 (*v16_src--, v16_zero);
        	v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul2, v16_G2B); //G2 and B 
		v16_RG1= __builtin_ia32_punpcklbw128 (*v16_src_sec--, v16_zero);
		v16_RGC= __builtin_ia32_pslldqi128 (v16_RG1, 112);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul1, v16_RG1); // R and G1
		v16_tmp1= __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2);
		v16_YC=   __builtin_ia32_pslldqi128 (v16_tmp1, 112);
		v16_tmp2= __builtin_ia32_psrldqi128 (v16_tmp1, 16);   
		v16_tmp2= __builtin_ia32_por128 (v16_tmp2, v16_YC_old);// Y Carry
		v16_Y=    __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2); // Y 
		v16_YC_old= v16_YC;

		v16_tmp1= __builtin_ia32_psrldqi128 (v16_RG1, 16); 		
		v16_tmp1= __builtin_ia32_por128 (v16_tmp1, v16_RGC_old);// R Carry
		v16_tmp1= __builtin_ia32_psubsw128 (v16_tmp1,v16_Y);
		v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul7, v16_tmp1); 
		v16_tmp1= __builtin_ia32_psllwi128 (v16_tmp1,1);
		v16_tmp1= __builtin_ia32_paddsw128 (v16_add3, v16_tmp1);
		v16_RGC_old=v16_RGC;

		v16_tmp2= __builtin_ia32_psrldqi128 (v16_G2B, 16);
		v16_tmp2= __builtin_ia32_psubsw128 (v16_tmp2,v16_Y);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul8, v16_tmp2); 
		v16_tmp2= __builtin_ia32_psllwi128 (v16_tmp2,1);
		v16_tmp2= __builtin_ia32_paddsw128 (v16_add4, v16_tmp2); 
		v16_CbCr= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		v16_tmp1= __builtin_ia32_punpckhwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpckhwd128 (v16_CbCr,v16_zero);
		v16_tmp3= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);
	
		v16_tmp1= __builtin_ia32_punpcklwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpcklwd128 (v16_CbCr,v16_zero);
		v16_tmp4= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		*v16_dst--= __builtin_ia32_packuswb128 (v16_tmp4, v16_tmp3);
		}
		while(x-=16);
		v16_src=(v16qi *)((long)v16_src-remain_size);
		v16_src_sec=(v16qi *)((long)v16_src_sec-remain_size);
		tmp_ptr= (int8_t *)(v16_dst+1);
		*(int16_t *)tmp_ptr= *(int16_t *)(tmp_ptr+2);
		tmp_ptr+=(window_width<<1);
		*(int16_t *)(tmp_ptr-2)=*(int16_t *)(tmp_ptr-4);
		last_r= (int8_t *)v16_src+14+(dim_x<<1);
		tmpi= (uint8_t)*last_r-(uint8_t)*(tmp_ptr-1);
		*(tmp_ptr-2)= (mul16x16s(tmpi,coefCr)+128);

	}
	while(y-=2);
}


void debayerRGB_mode1_sse2(uint8_t *dst, uint8_t *src, int dim_x, int dim_y,int startx,int starty,int ww,int wh)
{
  /*red -- green1*/
  /*green2    -- blue*/
	int8_t *tmp_ptr,*last_r;
	int tmpi;
	int window_width,window_height;
	int remain_size;

	v16qi *v16_src,*v16_src_sec,*v16_dst,v16_tmp1,v16_tmp2,v16_tmp3,v16_tmp4;
	v16qi v16_Y,v16_RG1,v16_G2B,v16_CbCr;
	v16qi v16_YC;
	v16qi v16_YC_old;
	
	int x,y;

	if(ww && wh)   // working with window in image
	{
	    window_width= ww;
	    window_height= wh-2;
	    remain_size= dim_x-ww;
	    v16_src= (v16qi *)(src+dim_x*(wh+starty-2)-16+startx+ww);
	    v16_src_sec= (v16qi *)((unsigned long)v16_src+dim_x);
	    v16_dst= (v16qi *)(dst+2*ww*(wh-2)-16);	   
	}
	else
	{
	    window_width= dim_x;
	    window_height= dim_y-2;		
	    remain_size=0;
	    v16_src= (v16qi *)(src+dim_x*(dim_y-1)-16);
	    v16_src_sec= (v16qi *)((unsigned long)v16_src+dim_x);
	    v16_dst= (v16qi *)(dst+2*dim_x*(dim_y-2)-16);
	}

	y=window_height;
	do
	{
		x= window_width;
		v16_YC_old=v16_zero;
		do
		{
       
		v16_G2B= __builtin_ia32_punpckhbw128 (*v16_src, v16_zero);
        	v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul6, v16_G2B); // G2 and B 
		v16_RG1= __builtin_ia32_punpckhbw128 (*v16_src_sec, v16_zero);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul5, v16_RG1); // R and G1
		v16_tmp1= __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2);
		v16_YC=   __builtin_ia32_pslldqi128 (v16_tmp1, 112);
		v16_tmp2= __builtin_ia32_psrldqi128 (v16_tmp1, 16);   
		v16_tmp2= __builtin_ia32_por128 (v16_tmp2, v16_YC_old);// Y Carry
		v16_Y=    __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2); // Y 
		v16_YC_old= v16_YC;

		v16_tmp1= __builtin_ia32_psubsw128 (v16_RG1,v16_Y);
		v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul7, v16_tmp1); 
		v16_tmp1= __builtin_ia32_psllwi128 (v16_tmp1,1);
		v16_tmp1= __builtin_ia32_paddsw128 (v16_add3, v16_tmp1);
		

		v16_tmp2= __builtin_ia32_psubsw128 (v16_G2B,v16_Y);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul8, v16_tmp2);
		v16_tmp2= __builtin_ia32_psllwi128 (v16_tmp2,1); 
		v16_tmp2= __builtin_ia32_paddsw128 (v16_add4, v16_tmp2); 
		v16_CbCr= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		v16_tmp1= __builtin_ia32_punpckhwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpckhwd128 (v16_CbCr,v16_zero );
		v16_tmp3= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);
	
		v16_tmp1= __builtin_ia32_punpcklwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpcklwd128 (v16_CbCr,v16_zero );
		v16_tmp4= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);


		*v16_dst--= __builtin_ia32_packuswb128 (v16_tmp4, v16_tmp3);


		v16_G2B= __builtin_ia32_punpcklbw128 (*v16_src--, v16_zero);
        	v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul6, v16_G2B); //G2 and B 
		v16_RG1= __builtin_ia32_punpcklbw128 (*v16_src_sec--, v16_zero);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul5, v16_RG1); // R and G1
		v16_tmp1= __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2);
		v16_YC=   __builtin_ia32_pslldqi128 (v16_tmp1, 112);
		v16_tmp2= __builtin_ia32_psrldqi128 (v16_tmp1, 16);   
		v16_tmp2= __builtin_ia32_por128 (v16_tmp2, v16_YC_old);// Y Carry
		v16_Y=    __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2); // Y 
		v16_YC_old= v16_YC;

		v16_tmp1= __builtin_ia32_psubsw128 (v16_RG1,v16_Y);
		v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul7, v16_tmp1); 
		v16_tmp1= __builtin_ia32_psllwi128 (v16_tmp1,1);
		v16_tmp1= __builtin_ia32_paddsw128 (v16_add3, v16_tmp1);

		v16_tmp2= __builtin_ia32_psubsw128 (v16_G2B,v16_Y);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul8, v16_tmp2); 
		v16_tmp2= __builtin_ia32_psllwi128 (v16_tmp2,1);
		v16_tmp2= __builtin_ia32_paddsw128 (v16_add4, v16_tmp2); 
		v16_CbCr= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		v16_tmp1= __builtin_ia32_punpckhwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpckhwd128 (v16_CbCr,v16_zero);
		v16_tmp3= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);
	
		v16_tmp1= __builtin_ia32_punpcklwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpcklwd128 (v16_CbCr,v16_zero);
		v16_tmp4= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		*v16_dst--= __builtin_ia32_packuswb128 (v16_tmp4, v16_tmp3);
		}
		while(x-=16);

		v16_src=(v16qi *)((long)v16_src-remain_size);
		v16_src_sec=(v16qi *)((long)v16_src_sec-remain_size);
		tmp_ptr= (int8_t *)(v16_dst+1);
		*(int16_t *)tmp_ptr= *(int16_t *)(tmp_ptr+2);
		tmp_ptr+=(window_width<<1);
		*(int16_t *)(tmp_ptr-2)=*(int16_t *)(tmp_ptr-4);
		last_r= (int8_t *)v16_src+15+(dim_x<<1);
		tmpi= (uint8_t)*last_r-(uint8_t)*(tmp_ptr-1);
		*(tmp_ptr-2)= (mul16x16s(tmpi,coefCr)+128);
		x= window_width;
		v16_YC_old=v16_zero;
		do
		{
       
		v16_RG1= __builtin_ia32_punpckhbw128 (*v16_src, v16_zero);
        	v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul5, v16_RG1); // R and G1 
		v16_G2B= __builtin_ia32_punpckhbw128 (*v16_src_sec, v16_zero);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul6, v16_G2B); // G2 and B
		v16_tmp1= __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2);
		v16_YC=   __builtin_ia32_pslldqi128 (v16_tmp1, 112);
		v16_tmp2= __builtin_ia32_psrldqi128 (v16_tmp1, 16);   
		v16_tmp2= __builtin_ia32_por128 (v16_tmp2, v16_YC_old);// Y Carry
		v16_Y=    __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2); // Y 
		v16_YC_old= v16_YC;

		v16_tmp1= __builtin_ia32_psubsw128 (v16_RG1,v16_Y);
		v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul7, v16_tmp1); 
		v16_tmp1= __builtin_ia32_psllwi128 (v16_tmp1,1);
		v16_tmp1= __builtin_ia32_paddsw128 (v16_add3, v16_tmp1);
	

		v16_tmp2= __builtin_ia32_psubsw128 (v16_G2B,v16_Y);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul8, v16_tmp2);
		v16_tmp2= __builtin_ia32_psllwi128 (v16_tmp2,1); 
		v16_tmp2= __builtin_ia32_paddsw128 (v16_add4, v16_tmp2);  
		v16_CbCr= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		v16_tmp1= __builtin_ia32_punpckhwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpckhwd128 (v16_CbCr,v16_zero );
		v16_tmp3= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);
	
		v16_tmp1= __builtin_ia32_punpcklwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpcklwd128 (v16_CbCr,v16_zero );
		v16_tmp4= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);


		*v16_dst--= __builtin_ia32_packuswb128 (v16_tmp4, v16_tmp3);


		v16_RG1= __builtin_ia32_punpcklbw128 (*v16_src--, v16_zero);
        	v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul5, v16_RG1); // R and G1 
		v16_G2B= __builtin_ia32_punpcklbw128 (*v16_src_sec--, v16_zero);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul6, v16_G2B); // G2 and B
		v16_tmp1= __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2);
		v16_YC=   __builtin_ia32_pslldqi128 (v16_tmp1, 112);
		v16_tmp2= __builtin_ia32_psrldqi128 (v16_tmp1, 16);
		v16_tmp2= __builtin_ia32_por128 (v16_tmp2, v16_YC_old);// Y Carry   
		v16_Y=    __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2); // Y 
		v16_YC_old= v16_YC;

		v16_tmp1= __builtin_ia32_psubsw128 (v16_RG1,v16_Y);
		v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul7, v16_tmp1); 
		v16_tmp1= __builtin_ia32_psllwi128 (v16_tmp1,1);
		v16_tmp1= __builtin_ia32_paddsw128 (v16_add3, v16_tmp1);

		v16_tmp2= __builtin_ia32_psubsw128 (v16_G2B,v16_Y);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul8, v16_tmp2); 
		v16_tmp2= __builtin_ia32_psllwi128 (v16_tmp2,1);
		v16_tmp2= __builtin_ia32_paddsw128 (v16_add4, v16_tmp2); 
		v16_CbCr= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		v16_tmp1= __builtin_ia32_punpckhwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpckhwd128 (v16_CbCr,v16_zero);
		v16_tmp3= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);
	
		v16_tmp1= __builtin_ia32_punpcklwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpcklwd128 (v16_CbCr,v16_zero);
		v16_tmp4= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		*v16_dst--= __builtin_ia32_packuswb128 (v16_tmp4, v16_tmp3);
		}
		while(x-=16);
		v16_src=(v16qi *)((long)v16_src-remain_size);
		v16_src_sec=(v16qi *)((long)v16_src_sec-remain_size);
		tmp_ptr= (int8_t *)(v16_dst+1);
		*(int16_t *)tmp_ptr= *(int16_t *)(tmp_ptr+2);
		tmp_ptr+=(window_width<<1);
		*(int16_t *)(tmp_ptr-2)=*(int16_t *)(tmp_ptr-4);
		last_r= (int8_t *)v16_src+15+dim_x;
		tmpi= (uint8_t)*last_r-(uint8_t)*(tmp_ptr-1);
		*(tmp_ptr-2)= (mul16x16s(tmpi,coefCr)+128);

	}
	while(y-=2);
}


void debayerRGB_mode3_sse2(uint8_t *dst, uint8_t *src, int dim_x, int dim_y,int startx,int starty,int ww,int wh)
{
  /*green2 -- blue*/
  /*red    -- green1*/
	int8_t *tmp_ptr,*last_r;
	int tmpi;
	int window_width,window_height;
	int remain_size;

	v16qi *v16_src,*v16_src_sec,*v16_dst,v16_tmp1,v16_tmp2,v16_tmp3,v16_tmp4;
	v16qi v16_Y,v16_RG1,v16_G2B,v16_CbCr;
	v16qi v16_YC;
	v16qi v16_YC_old;
	
	int x,y;

	if(ww && wh)   // working with window in image
	{
	    window_width= ww;
	    window_height= wh-2;
	    remain_size= dim_x-ww;
	    v16_src= (v16qi *)(src+dim_x*(wh+starty-2)-16+startx+ww);
	    v16_src_sec= (v16qi *)((unsigned long)v16_src+dim_x);	
	    v16_dst= (v16qi *)(dst+2*ww*(wh-2)-16);   
	}
	else
	{
	    window_width= dim_x;
	    window_height= dim_y-2;		
	    remain_size=0;
	    v16_src= (v16qi *)(src+dim_x*(dim_y-1)-16);
	    v16_src_sec= (v16qi *)((unsigned long)v16_src+dim_x);
	    v16_dst= (v16qi *)(dst+2*dim_x*(dim_y-2)-16);
	}

	y=window_height;
	do
	{
		x= window_width;
		v16_YC_old=v16_zero;
		do
		{
       
		v16_RG1= __builtin_ia32_punpckhbw128 (*v16_src, v16_zero);
        	v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul5, v16_RG1); // R and G1 
		v16_G2B= __builtin_ia32_punpckhbw128 (*v16_src_sec, v16_zero);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul6, v16_G2B); // G2 and B
		v16_tmp1= __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2);
		v16_YC=   __builtin_ia32_pslldqi128 (v16_tmp1, 112);
		v16_tmp2= __builtin_ia32_psrldqi128 (v16_tmp1, 16);   
		v16_tmp2= __builtin_ia32_por128 (v16_tmp2, v16_YC_old);// Y Carry
		v16_Y=    __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2); // Y 
		v16_YC_old= v16_YC;

		v16_tmp1= __builtin_ia32_psubsw128 (v16_RG1,v16_Y);
		v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul7, v16_tmp1); 
		v16_tmp1= __builtin_ia32_psllwi128 (v16_tmp1,1);
		v16_tmp1= __builtin_ia32_paddsw128 (v16_add3, v16_tmp1);
	

		v16_tmp2= __builtin_ia32_psubsw128 (v16_G2B,v16_Y);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul8, v16_tmp2);
		v16_tmp2= __builtin_ia32_psllwi128 (v16_tmp2,1); 
		v16_tmp2= __builtin_ia32_paddsw128 (v16_add4, v16_tmp2);  
		v16_CbCr= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		v16_tmp1= __builtin_ia32_punpckhwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpckhwd128 (v16_CbCr,v16_zero );
		v16_tmp3= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);
	
		v16_tmp1= __builtin_ia32_punpcklwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpcklwd128 (v16_CbCr,v16_zero );
		v16_tmp4= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);


		*v16_dst--= __builtin_ia32_packuswb128 (v16_tmp4, v16_tmp3);


		v16_RG1= __builtin_ia32_punpcklbw128 (*v16_src--, v16_zero);
        	v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul5, v16_RG1); // R and G1 
		v16_G2B= __builtin_ia32_punpcklbw128 (*v16_src_sec--, v16_zero);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul6, v16_G2B); // G2 and B
		v16_tmp1= __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2);
		v16_YC=   __builtin_ia32_pslldqi128 (v16_tmp1, 112);
		v16_tmp2= __builtin_ia32_psrldqi128 (v16_tmp1, 16);
		v16_tmp2= __builtin_ia32_por128 (v16_tmp2, v16_YC_old);// Y Carry   
		v16_Y=    __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2); // Y 
		v16_YC_old= v16_YC;

		v16_tmp1= __builtin_ia32_psubsw128 (v16_RG1,v16_Y);
		v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul7, v16_tmp1); 
		v16_tmp1= __builtin_ia32_psllwi128 (v16_tmp1,1);
		v16_tmp1= __builtin_ia32_paddsw128 (v16_add3, v16_tmp1);

		v16_tmp2= __builtin_ia32_psubsw128 (v16_G2B,v16_Y);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul8, v16_tmp2); 
		v16_tmp2= __builtin_ia32_psllwi128 (v16_tmp2,1);
		v16_tmp2= __builtin_ia32_paddsw128 (v16_add4, v16_tmp2); 
		v16_CbCr= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		v16_tmp1= __builtin_ia32_punpckhwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpckhwd128 (v16_CbCr,v16_zero);
		v16_tmp3= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);
	
		v16_tmp1= __builtin_ia32_punpcklwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpcklwd128 (v16_CbCr,v16_zero);
		v16_tmp4= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		*v16_dst--= __builtin_ia32_packuswb128 (v16_tmp4, v16_tmp3);
		}
		while(x-=16);
		
		v16_src=(v16qi *)((long)v16_src-remain_size);
		v16_src_sec=(v16qi *)((long)v16_src_sec-remain_size);
		tmp_ptr= (int8_t *)(v16_dst+1);
		*(int16_t *)tmp_ptr= *(int16_t *)(tmp_ptr+2);
		tmp_ptr+=(window_width<<1);
		*(int16_t *)(tmp_ptr-2)=*(int16_t *)(tmp_ptr-4);
		last_r= (int8_t *)v16_src+15+dim_x;
		tmpi= (uint8_t)*last_r-(uint8_t)*(tmp_ptr-1);
		*(tmp_ptr-2)= (mul16x16s(tmpi,coefCr)+128);
		x= window_width;
		v16_YC_old=v16_zero;
		do
		{
       
		v16_G2B= __builtin_ia32_punpckhbw128 (*v16_src, v16_zero);
        	v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul6, v16_G2B); // G2 and B 
		v16_RG1= __builtin_ia32_punpckhbw128 (*v16_src_sec, v16_zero);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul5, v16_RG1); // R and G1
		v16_tmp1= __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2);
		v16_YC=   __builtin_ia32_pslldqi128 (v16_tmp1, 112);
		v16_tmp2= __builtin_ia32_psrldqi128 (v16_tmp1, 16);   
		v16_tmp2= __builtin_ia32_por128 (v16_tmp2, v16_YC_old);// Y Carry
		v16_Y=    __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2); // Y 
		v16_YC_old= v16_YC;

		v16_tmp1= __builtin_ia32_psubsw128 (v16_RG1,v16_Y);
		v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul7, v16_tmp1); 
		v16_tmp1= __builtin_ia32_psllwi128 (v16_tmp1,1);
		v16_tmp1= __builtin_ia32_paddsw128 (v16_add3, v16_tmp1);
		

		v16_tmp2= __builtin_ia32_psubsw128 (v16_G2B,v16_Y);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul8, v16_tmp2);
		v16_tmp2= __builtin_ia32_psllwi128 (v16_tmp2,1); 
		v16_tmp2= __builtin_ia32_paddsw128 (v16_add4, v16_tmp2); 
		v16_CbCr= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		v16_tmp1= __builtin_ia32_punpckhwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpckhwd128 (v16_CbCr,v16_zero );
		v16_tmp3= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);
	
		v16_tmp1= __builtin_ia32_punpcklwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpcklwd128 (v16_CbCr,v16_zero );
		v16_tmp4= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);


		*v16_dst--= __builtin_ia32_packuswb128 (v16_tmp4, v16_tmp3);


		v16_G2B= __builtin_ia32_punpcklbw128 (*v16_src--, v16_zero);
        	v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul6, v16_G2B); //G2 and B 
		v16_RG1= __builtin_ia32_punpcklbw128 (*v16_src_sec--, v16_zero);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul5, v16_RG1); // R and G1
		v16_tmp1= __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2);
		v16_YC=   __builtin_ia32_pslldqi128 (v16_tmp1, 112);
		v16_tmp2= __builtin_ia32_psrldqi128 (v16_tmp1, 16);   
		v16_tmp2= __builtin_ia32_por128 (v16_tmp2, v16_YC_old);// Y Carry
		v16_Y=    __builtin_ia32_paddsw128 (v16_tmp1, v16_tmp2); // Y 
		v16_YC_old= v16_YC;

		v16_tmp1= __builtin_ia32_psubsw128 (v16_RG1,v16_Y);
		v16_tmp1= __builtin_ia32_pmulhw128 (v16_mul7, v16_tmp1); 
		v16_tmp1= __builtin_ia32_psllwi128 (v16_tmp1,1);
		v16_tmp1= __builtin_ia32_paddsw128 (v16_add3, v16_tmp1);

		v16_tmp2= __builtin_ia32_psubsw128 (v16_G2B,v16_Y);
		v16_tmp2= __builtin_ia32_pmulhw128 (v16_mul8, v16_tmp2); 
		v16_tmp2= __builtin_ia32_psllwi128 (v16_tmp2,1);
		v16_tmp2= __builtin_ia32_paddsw128 (v16_add4, v16_tmp2); 
		v16_CbCr= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		v16_tmp1= __builtin_ia32_punpckhwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpckhwd128 (v16_CbCr,v16_zero);
		v16_tmp3= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);
	
		v16_tmp1= __builtin_ia32_punpcklwd128 (v16_zero,v16_Y);
		v16_tmp2= __builtin_ia32_punpcklwd128 (v16_CbCr,v16_zero);
		v16_tmp4= __builtin_ia32_por128 (v16_tmp1, v16_tmp2);

		*v16_dst--= __builtin_ia32_packuswb128 (v16_tmp4, v16_tmp3);
		}
		while(x-=16);
		v16_src=(v16qi *)((long)v16_src-remain_size);
		v16_src_sec=(v16qi *)((long)v16_src_sec-remain_size);
		tmp_ptr= (int8_t *)(v16_dst+1);
		*(int16_t *)tmp_ptr= *(int16_t *)(tmp_ptr+2);
		tmp_ptr+=(window_width<<1);
		*(int16_t *)(tmp_ptr-2)=*(int16_t *)(tmp_ptr-4);
		last_r= (int8_t *)v16_src+15+(dim_x<<1);
		tmpi= (uint8_t)*last_r-(uint8_t)*(tmp_ptr-1);
		*(tmp_ptr-2)= (mul16x16s(tmpi,coefCr)+128);

	}
	while(y-=2);
}
void debayerRGB_mode0_mmx(uint8_t *dst, uint8_t *src, int dim_x, int dim_y,int startx,int starty,int ww,int wh)
{
  /*green1    -- red*/
  /*blue -- green2*/

	int8_t *tmp_ptr,*last_r;
	int tmpi;
	int window_width,window_height;
	int remain_size;

	v8qi *v8_src,*v8_src_sec,*v8_dst,v8_tmp1,v8_tmp2,v8_tmp3,v8_tmp4;
	v8qi v8_Y,v8_RG1,v8_G2B,v8_CbCr;
	v8qi v8_YC,v8_RGC;
	v8qi v8_YC_old,v8_RGC_old;

	int x,y;

	if(ww && wh)   // working with window in image
	{
	    window_width= ww;
	    window_height= wh-2;
	    remain_size= dim_x-ww;
	    v8_src= (v8qi *)(src+dim_x*(wh+starty-2)-8+startx+ww);
	    v8_src_sec= (v8qi *)((unsigned long)v8_src+dim_x);	
	    v8_dst= (v8qi *)(dst+2*ww*(wh-2)-8);   
	}
	else
	{
	    window_width= dim_x;
	    window_height= dim_y-2;		
	    remain_size=0;
	    v8_src= (v8qi *)(src+dim_x*(dim_y-1)-8);
	    v8_src_sec= (v8qi *)((unsigned long)v8_src+dim_x);
	    v8_dst= (v8qi *)(dst+2*dim_x*(dim_y-2)-8);
	}

	y=window_height;
	do
	{
		x= window_width;
		v8_YC_old=v8_RGC_old=v8_zero;
		do
		{
       
		v8_G2B= __builtin_ia32_punpckhbw (*v8_src, v8_zero);
        	v8_tmp1= __builtin_ia32_pmulhw (v8_mul2, v8_G2B); // G2 and B 
		v8_RG1= __builtin_ia32_punpckhbw (*v8_src_sec, v8_zero);

		v8_RGC= __builtin_ia32_psllqi (v8_RG1, 48);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul1, v8_RG1); // R and G1
		v8_tmp1= __builtin_ia32_paddsw (v8_tmp1, v8_tmp2);
		v8_YC=   __builtin_ia32_psllqi (v8_tmp1, 48);
		v8_tmp2= __builtin_ia32_psrlqi (v8_tmp1, 16);   
		v8_tmp2= __builtin_ia32_por (v8_tmp2, v8_YC_old);// Y Carry
		v8_Y=    __builtin_ia32_paddsw (v8_tmp1, v8_tmp2); // Y 
		v8_YC_old= v8_YC;

		v8_tmp1= __builtin_ia32_psrlqi (v8_RG1, 16); 		
		v8_tmp1= __builtin_ia32_por (v8_tmp1, v8_RGC_old);// R Carry
		v8_tmp1= __builtin_ia32_psubsw (v8_tmp1,v8_Y);
		v8_tmp1= __builtin_ia32_pmulhw (v8_mul7, v8_tmp1); 
		v8_tmp1= __builtin_ia32_psllwi (v8_tmp1,1);
		v8_tmp1= __builtin_ia32_paddsw (v8_add3, v8_tmp1);
		v8_RGC_old=v8_RGC;

		v8_tmp2= __builtin_ia32_psrlqi (v8_G2B, 16);
		v8_tmp2= __builtin_ia32_psubsw (v8_tmp2,v8_Y);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul8, v8_tmp2);
		v8_tmp2= __builtin_ia32_psllwi (v8_tmp2,1); 
		v8_tmp2= __builtin_ia32_paddsw (v8_add4, v8_tmp2); 
		v8_CbCr= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		v8_tmp1= __builtin_ia32_punpckhwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpckhwd (v8_CbCr,v8_zero );
		v8_tmp3= __builtin_ia32_por (v8_tmp1, v8_tmp2);
	
		v8_tmp1= __builtin_ia32_punpcklwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpcklwd (v8_CbCr,v8_zero );
		v8_tmp4= __builtin_ia32_por (v8_tmp1, v8_tmp2);


		*v8_dst--= __builtin_ia32_packuswb (v8_tmp4, v8_tmp3);


		v8_G2B= __builtin_ia32_punpcklbw (*v8_src--, v8_zero);
        	v8_tmp1= __builtin_ia32_pmulhw (v8_mul2, v8_G2B); //G2 and B 
		v8_RG1= __builtin_ia32_punpcklbw (*v8_src_sec--, v8_zero);
		v8_RGC= __builtin_ia32_psllqi (v8_RG1, 48);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul1, v8_RG1); // R and G1
		v8_tmp1= __builtin_ia32_paddsw (v8_tmp1, v8_tmp2);
		v8_YC=   __builtin_ia32_psllqi (v8_tmp1, 48);
		v8_tmp2= __builtin_ia32_psrlqi (v8_tmp1, 16);   
		v8_tmp2= __builtin_ia32_por (v8_tmp2, v8_YC_old);// Y Carry
		v8_Y=    __builtin_ia32_paddsw (v8_tmp1, v8_tmp2); // Y 
		v8_YC_old= v8_YC;

		v8_tmp1= __builtin_ia32_psrlqi (v8_RG1, 16); 		
		v8_tmp1= __builtin_ia32_por (v8_tmp1, v8_RGC_old);// R Carry
		v8_tmp1= __builtin_ia32_psubsw (v8_tmp1,v8_Y);
		v8_tmp1= __builtin_ia32_pmulhw (v8_mul7, v8_tmp1); 
		v8_tmp1= __builtin_ia32_psllwi (v8_tmp1,1);
		v8_tmp1= __builtin_ia32_paddsw (v8_add3, v8_tmp1);
		v8_RGC_old=v8_RGC;

		v8_tmp2= __builtin_ia32_psrlqi (v8_G2B, 16);
		v8_tmp2= __builtin_ia32_psubsw (v8_tmp2,v8_Y);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul8, v8_tmp2); 
		v8_tmp2= __builtin_ia32_psllwi (v8_tmp2,1);
		v8_tmp2= __builtin_ia32_paddsw (v8_add4, v8_tmp2); 
		v8_CbCr= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		v8_tmp1= __builtin_ia32_punpckhwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpckhwd (v8_CbCr,v8_zero);
		v8_tmp3= __builtin_ia32_por (v8_tmp1, v8_tmp2);
	
		v8_tmp1= __builtin_ia32_punpcklwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpcklwd (v8_CbCr,v8_zero);
		v8_tmp4= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		*v8_dst--= __builtin_ia32_packuswb (v8_tmp4, v8_tmp3);
		}
		while(x-=8);
		
		v8_src=(v8qi *)((long)v8_src-remain_size);
		v8_src_sec=(v8qi *)((long)v8_src_sec-remain_size);		
		tmp_ptr= (int8_t *)(v8_dst+1);
		*(int16_t *)tmp_ptr= *(int16_t *)(tmp_ptr+2);
		tmp_ptr+=(window_width<<1);
		*(int16_t *)(tmp_ptr-2)=*(int16_t *)(tmp_ptr-4);
		last_r= (int8_t *)v8_src+6+(dim_x<<1);
		tmpi= (uint8_t)*last_r-(uint8_t)*(tmp_ptr-1);
		*(tmp_ptr-2)= (mul16x16s(tmpi,coefCr)+128);
		x= window_width;
		v8_YC_old=v8_RGC_old=v8_zero;

		do
		{
       
		v8_RG1= __builtin_ia32_punpckhbw (*v8_src, v8_zero);
		v8_RGC= __builtin_ia32_psllqi (v8_RG1, 48);
        	v8_tmp1= __builtin_ia32_pmulhw (v8_mul1, v8_RG1); // R and G1 
		v8_G2B= __builtin_ia32_punpckhbw (*v8_src_sec, v8_zero);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul2, v8_G2B); // G2 and B
		v8_tmp1= __builtin_ia32_paddsw (v8_tmp1, v8_tmp2);
		v8_YC=   __builtin_ia32_psllqi (v8_tmp1, 48);
		v8_tmp2= __builtin_ia32_psrlqi (v8_tmp1, 16);   
		v8_tmp2= __builtin_ia32_por (v8_tmp2, v8_YC_old);// Y Carry
		v8_Y=    __builtin_ia32_paddsw (v8_tmp1, v8_tmp2); // Y 
		v8_YC_old= v8_YC;

		v8_tmp1= __builtin_ia32_psrlqi (v8_RG1, 16); 		
		v8_tmp1= __builtin_ia32_por (v8_tmp1, v8_RGC_old);// R Carry
		v8_tmp1= __builtin_ia32_psubsw (v8_tmp1,v8_Y);
		v8_tmp1= __builtin_ia32_pmulhw (v8_mul7, v8_tmp1); 
		v8_tmp1= __builtin_ia32_psllwi (v8_tmp1,1);
		v8_tmp1= __builtin_ia32_paddsw (v8_add3, v8_tmp1);
		v8_RGC_old=v8_RGC;

		v8_tmp2= __builtin_ia32_psrlqi (v8_G2B, 16); 
		v8_tmp2= __builtin_ia32_psubsw (v8_tmp2,v8_Y);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul8, v8_tmp2);
		v8_tmp2= __builtin_ia32_psllwi (v8_tmp2,1); 
		v8_tmp2= __builtin_ia32_paddsw (v8_add4, v8_tmp2);  
		v8_CbCr= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		v8_tmp1= __builtin_ia32_punpckhwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpckhwd (v8_CbCr,v8_zero );
		v8_tmp3= __builtin_ia32_por (v8_tmp1, v8_tmp2);
	
		v8_tmp1= __builtin_ia32_punpcklwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpcklwd (v8_CbCr,v8_zero );
		v8_tmp4= __builtin_ia32_por (v8_tmp1, v8_tmp2);


		*v8_dst--= __builtin_ia32_packuswb (v8_tmp4, v8_tmp3);


		v8_RG1= __builtin_ia32_punpcklbw (*v8_src--, v8_zero);
		v8_RGC= __builtin_ia32_psllqi (v8_RG1, 48);
        	v8_tmp1= __builtin_ia32_pmulhw (v8_mul1, v8_RG1); // R and G1 
		v8_G2B= __builtin_ia32_punpcklbw (*v8_src_sec--, v8_zero);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul2, v8_G2B); // G2 and B
		v8_tmp1= __builtin_ia32_paddsw (v8_tmp1, v8_tmp2);
		v8_YC=   __builtin_ia32_psllqi (v8_tmp1, 48);
		v8_tmp2= __builtin_ia32_psrlqi (v8_tmp1, 16);
		v8_tmp2= __builtin_ia32_por (v8_tmp2, v8_YC_old);// Y Carry   
		v8_Y=    __builtin_ia32_paddsw (v8_tmp1, v8_tmp2); // Y 
		v8_YC_old= v8_YC;

		v8_tmp1= __builtin_ia32_psrlqi (v8_RG1, 16); 		
		v8_tmp1= __builtin_ia32_por (v8_tmp1, v8_RGC_old);// R Carry
		v8_tmp1= __builtin_ia32_psubsw (v8_tmp1,v8_Y);
		v8_tmp1= __builtin_ia32_pmulhw (v8_mul7, v8_tmp1); 
		v8_tmp1= __builtin_ia32_psllwi (v8_tmp1,1);
		v8_tmp1= __builtin_ia32_paddsw (v8_add3, v8_tmp1);
		v8_RGC_old=v8_RGC;

		v8_tmp2= __builtin_ia32_psrlqi (v8_G2B, 16);
		v8_tmp2= __builtin_ia32_psubsw (v8_tmp2,v8_Y);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul8, v8_tmp2); 
		v8_tmp2= __builtin_ia32_psllwi (v8_tmp2,1);
		v8_tmp2= __builtin_ia32_paddsw (v8_add4, v8_tmp2); 
		v8_CbCr= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		v8_tmp1= __builtin_ia32_punpckhwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpckhwd (v8_CbCr,v8_zero);
		v8_tmp3= __builtin_ia32_por (v8_tmp1, v8_tmp2);
	
		v8_tmp1= __builtin_ia32_punpcklwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpcklwd (v8_CbCr,v8_zero);
		v8_tmp4= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		*v8_dst--= __builtin_ia32_packuswb (v8_tmp4, v8_tmp3);
		}
		while(x-=8);

		v8_src=(v8qi *)((long)v8_src-remain_size);
		v8_src_sec=(v8qi *)((long)v8_src_sec-remain_size);
		tmp_ptr= (int8_t *)(v8_dst+1);
		*(int16_t *)tmp_ptr= *(int16_t *)(tmp_ptr+2);
		tmp_ptr+=(window_width<<1);
		*(int16_t *)(tmp_ptr-2)=*(int16_t *)(tmp_ptr-4);
		last_r= (int8_t *)v8_src+6+dim_x;
		tmpi= (uint8_t)*last_r-(uint8_t)*(tmp_ptr-1);
		*(tmp_ptr-2)= (mul16x16s(tmpi,coefCr)+128);

	}
	while(y-=2);
}

void debayerRGB_mode2_mmx(uint8_t *dst, uint8_t *src, int dim_x, int dim_y,int startx,int starty,int ww,int wh)
{
  /*blue -- green2*/
   /*green1    -- red*/

	int8_t *tmp_ptr,*last_r;
	int tmpi;
	int window_width,window_height;
	int remain_size;

	v8qi *v8_src,*v8_src_sec,*v8_dst,v8_tmp1,v8_tmp2,v8_tmp3,v8_tmp4;
	v8qi v8_Y,v8_RG1,v8_G2B,v8_CbCr;
	v8qi v8_YC,v8_RGC;
	v8qi v8_YC_old,v8_RGC_old;

	int x,y;
	if(ww && wh)   // working with window in image
	{
	    window_width= ww;
	    window_height= wh-2;
	    remain_size= dim_x-ww;
	    v8_src= (v8qi *)(src+dim_x*(wh+starty-2)-8+startx+ww);
	    v8_src_sec= (v8qi *)((unsigned long)v8_src+dim_x);	
	    v8_dst= (v8qi *)(dst+2*ww*(wh-2)-8);   
	}
	else
	{
	    window_width= dim_x;
	    window_height= dim_y-2;		
	    remain_size=0;
	    v8_src= (v8qi *)(src+dim_x*(dim_y-1)-8);
	    v8_src_sec= (v8qi *)((unsigned long)v8_src+dim_x);
	    v8_dst= (v8qi *)(dst+2*dim_x*(dim_y-2)-8);
	}
	y=window_height;
	do
	{
		x= window_width;
		v8_YC_old=v8_RGC_old=v8_zero;
		do
		{
       
		v8_RG1= __builtin_ia32_punpckhbw (*v8_src, v8_zero);
		v8_RGC= __builtin_ia32_psllqi (v8_RG1, 48);
        	v8_tmp1= __builtin_ia32_pmulhw (v8_mul1, v8_RG1); // R and G1 
		v8_G2B= __builtin_ia32_punpckhbw (*v8_src_sec, v8_zero);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul2, v8_G2B); // G2 and B
		v8_tmp1= __builtin_ia32_paddsw (v8_tmp1, v8_tmp2);
		v8_YC=   __builtin_ia32_psllqi (v8_tmp1, 48);
		v8_tmp2= __builtin_ia32_psrlqi (v8_tmp1, 16);   
		v8_tmp2= __builtin_ia32_por (v8_tmp2, v8_YC_old);// Y Carry
		v8_Y=    __builtin_ia32_paddsw (v8_tmp1, v8_tmp2); // Y 
		v8_YC_old= v8_YC;

		v8_tmp1= __builtin_ia32_psrlqi (v8_RG1, 16); 		
		v8_tmp1= __builtin_ia32_por (v8_tmp1, v8_RGC_old);// R Carry
		v8_tmp1= __builtin_ia32_psubsw (v8_tmp1,v8_Y);
		v8_tmp1= __builtin_ia32_pmulhw (v8_mul7, v8_tmp1); 
		v8_tmp1= __builtin_ia32_psllwi (v8_tmp1,1);
		v8_tmp1= __builtin_ia32_paddsw (v8_add3, v8_tmp1);
		v8_RGC_old=v8_RGC;

		v8_tmp2= __builtin_ia32_psrlqi (v8_G2B, 16); 
		v8_tmp2= __builtin_ia32_psubsw (v8_tmp2,v8_Y);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul8, v8_tmp2);
		v8_tmp2= __builtin_ia32_psllwi (v8_tmp2,1); 
		v8_tmp2= __builtin_ia32_paddsw (v8_add4, v8_tmp2);  
		v8_CbCr= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		v8_tmp1= __builtin_ia32_punpckhwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpckhwd (v8_CbCr,v8_zero );
		v8_tmp3= __builtin_ia32_por (v8_tmp1, v8_tmp2);
	
		v8_tmp1= __builtin_ia32_punpcklwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpcklwd (v8_CbCr,v8_zero );
		v8_tmp4= __builtin_ia32_por (v8_tmp1, v8_tmp2);


		*v8_dst--= __builtin_ia32_packuswb (v8_tmp4, v8_tmp3);


		v8_RG1= __builtin_ia32_punpcklbw (*v8_src--, v8_zero);
		v8_RGC= __builtin_ia32_psllqi (v8_RG1, 48);
        	v8_tmp1= __builtin_ia32_pmulhw (v8_mul1, v8_RG1); // R and G1 
		v8_G2B= __builtin_ia32_punpcklbw (*v8_src_sec--, v8_zero);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul2, v8_G2B); // G2 and B
		v8_tmp1= __builtin_ia32_paddsw (v8_tmp1, v8_tmp2);
		v8_YC=   __builtin_ia32_psllqi (v8_tmp1, 48);
		v8_tmp2= __builtin_ia32_psrlqi (v8_tmp1, 16);
		v8_tmp2= __builtin_ia32_por (v8_tmp2, v8_YC_old);// Y Carry   
		v8_Y=    __builtin_ia32_paddsw (v8_tmp1, v8_tmp2); // Y 
		v8_YC_old= v8_YC;

		v8_tmp1= __builtin_ia32_psrlqi (v8_RG1, 16); 		
		v8_tmp1= __builtin_ia32_por (v8_tmp1, v8_RGC_old);// R Carry
		v8_tmp1= __builtin_ia32_psubsw (v8_tmp1,v8_Y);
		v8_tmp1= __builtin_ia32_pmulhw (v8_mul7, v8_tmp1); 
		v8_tmp1= __builtin_ia32_psllwi (v8_tmp1,1);
		v8_tmp1= __builtin_ia32_paddsw (v8_add3, v8_tmp1);
		v8_RGC_old=v8_RGC;

		v8_tmp2= __builtin_ia32_psrlqi (v8_G2B, 16);
		v8_tmp2= __builtin_ia32_psubsw (v8_tmp2,v8_Y);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul8, v8_tmp2); 
		v8_tmp2= __builtin_ia32_psllwi (v8_tmp2,1);
		v8_tmp2= __builtin_ia32_paddsw (v8_add4, v8_tmp2); 
		v8_CbCr= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		v8_tmp1= __builtin_ia32_punpckhwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpckhwd (v8_CbCr,v8_zero);
		v8_tmp3= __builtin_ia32_por (v8_tmp1, v8_tmp2);
	
		v8_tmp1= __builtin_ia32_punpcklwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpcklwd (v8_CbCr,v8_zero);
		v8_tmp4= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		*v8_dst--= __builtin_ia32_packuswb (v8_tmp4, v8_tmp3);
		}
		while(x-=8);
		
		v8_src=(v8qi *)((long)v8_src-remain_size);
		v8_src_sec=(v8qi *)((long)v8_src_sec-remain_size);
		tmp_ptr= (int8_t *)(v8_dst+1);
		*(int16_t *)tmp_ptr= *(int16_t *)(tmp_ptr+2);
		tmp_ptr+=(window_width<<1);
		*(int16_t *)(tmp_ptr-2)=*(int16_t *)(tmp_ptr-4);
		last_r= (int8_t *)v8_src+6+dim_x;
		tmpi= (uint8_t)*last_r-(uint8_t)*(tmp_ptr-1);
		*(tmp_ptr-2)= (mul16x16s(tmpi,coefCr)+128);
		x= window_width;
		v8_YC_old=v8_RGC_old=v8_zero;
		do
		{
       
		v8_G2B= __builtin_ia32_punpckhbw (*v8_src, v8_zero);
        	v8_tmp1= __builtin_ia32_pmulhw (v8_mul2, v8_G2B); // G2 and B 
		v8_RG1= __builtin_ia32_punpckhbw (*v8_src_sec, v8_zero);

		v8_RGC= __builtin_ia32_psllqi (v8_RG1, 48);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul1, v8_RG1); // R and G1
		v8_tmp1= __builtin_ia32_paddsw (v8_tmp1, v8_tmp2);
		v8_YC=   __builtin_ia32_psllqi (v8_tmp1, 48);
		v8_tmp2= __builtin_ia32_psrlqi (v8_tmp1, 16);   
		v8_tmp2= __builtin_ia32_por (v8_tmp2, v8_YC_old);// Y Carry
		v8_Y=    __builtin_ia32_paddsw (v8_tmp1, v8_tmp2); // Y 
		v8_YC_old= v8_YC;

		v8_tmp1= __builtin_ia32_psrlqi (v8_RG1, 16); 		
		v8_tmp1= __builtin_ia32_por (v8_tmp1, v8_RGC_old);// R Carry
		v8_tmp1= __builtin_ia32_psubsw (v8_tmp1,v8_Y);
		v8_tmp1= __builtin_ia32_pmulhw (v8_mul7, v8_tmp1); 
		v8_tmp1= __builtin_ia32_psllwi (v8_tmp1,1);
		v8_tmp1= __builtin_ia32_paddsw (v8_add3, v8_tmp1);
		v8_RGC_old=v8_RGC;

		v8_tmp2= __builtin_ia32_psrlqi (v8_G2B, 16);
		v8_tmp2= __builtin_ia32_psubsw (v8_tmp2,v8_Y);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul8, v8_tmp2);
		v8_tmp2= __builtin_ia32_psllwi (v8_tmp2,1); 
		v8_tmp2= __builtin_ia32_paddsw (v8_add4, v8_tmp2); 
		v8_CbCr= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		v8_tmp1= __builtin_ia32_punpckhwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpckhwd (v8_CbCr,v8_zero );
		v8_tmp3= __builtin_ia32_por (v8_tmp1, v8_tmp2);
	
		v8_tmp1= __builtin_ia32_punpcklwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpcklwd (v8_CbCr,v8_zero );
		v8_tmp4= __builtin_ia32_por (v8_tmp1, v8_tmp2);


		*v8_dst--= __builtin_ia32_packuswb (v8_tmp4, v8_tmp3);


		v8_G2B= __builtin_ia32_punpcklbw (*v8_src--, v8_zero);
        	v8_tmp1= __builtin_ia32_pmulhw (v8_mul2, v8_G2B); //G2 and B 
		v8_RG1= __builtin_ia32_punpcklbw (*v8_src_sec--, v8_zero);
		v8_RGC= __builtin_ia32_psllqi (v8_RG1, 48);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul1, v8_RG1); // R and G1
		v8_tmp1= __builtin_ia32_paddsw (v8_tmp1, v8_tmp2);
		v8_YC=   __builtin_ia32_psllqi (v8_tmp1, 48);
		v8_tmp2= __builtin_ia32_psrlqi (v8_tmp1, 16);   
		v8_tmp2= __builtin_ia32_por (v8_tmp2, v8_YC_old);// Y Carry
		v8_Y=    __builtin_ia32_paddsw (v8_tmp1, v8_tmp2); // Y 
		v8_YC_old= v8_YC;

		v8_tmp1= __builtin_ia32_psrlqi (v8_RG1, 16); 		
		v8_tmp1= __builtin_ia32_por (v8_tmp1, v8_RGC_old);// R Carry
		v8_tmp1= __builtin_ia32_psubsw (v8_tmp1,v8_Y);
		v8_tmp1= __builtin_ia32_pmulhw (v8_mul7, v8_tmp1); 
		v8_tmp1= __builtin_ia32_psllwi (v8_tmp1,1);
		v8_tmp1= __builtin_ia32_paddsw (v8_add3, v8_tmp1);
		v8_RGC_old=v8_RGC;

		v8_tmp2= __builtin_ia32_psrlqi (v8_G2B, 16);
		v8_tmp2= __builtin_ia32_psubsw (v8_tmp2,v8_Y);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul8, v8_tmp2); 
		v8_tmp2= __builtin_ia32_psllwi (v8_tmp2,1);
		v8_tmp2= __builtin_ia32_paddsw (v8_add4, v8_tmp2); 
		v8_CbCr= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		v8_tmp1= __builtin_ia32_punpckhwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpckhwd (v8_CbCr,v8_zero);
		v8_tmp3= __builtin_ia32_por (v8_tmp1, v8_tmp2);
	
		v8_tmp1= __builtin_ia32_punpcklwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpcklwd (v8_CbCr,v8_zero);
		v8_tmp4= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		*v8_dst--= __builtin_ia32_packuswb (v8_tmp4, v8_tmp3);
		}
		while(x-=8);

		v8_src=(v8qi *)((long)v8_src-remain_size);
		v8_src_sec=(v8qi *)((long)v8_src_sec-remain_size);
		tmp_ptr= (int8_t *)(v8_dst+1);
		*(int16_t *)tmp_ptr= *(int16_t *)(tmp_ptr+2);
		tmp_ptr+=(window_width<<1);
		*(int16_t *)(tmp_ptr-2)=*(int16_t *)(tmp_ptr-4);
		last_r= (int8_t *)v8_src+6+(dim_x<<1);
		tmpi= (uint8_t)*last_r-(uint8_t)*(tmp_ptr-1);
		*(tmp_ptr-2)= (mul16x16s(tmpi,coefCr)+128);

	}
	while(y-=2);
}

void debayerRGB_mode1_mmx(uint8_t *dst, uint8_t *src, int dim_x, int dim_y,int startx,int starty,int ww,int wh)
{
  /*red -- green1*/
  /*green2    -- blue*/
	int8_t *tmp_ptr,*last_r;
	int tmpi;
	int window_width,window_height;
	int remain_size;

	v8qi *v8_src,*v8_src_sec,*v8_dst,v8_tmp1,v8_tmp2,v8_tmp3,v8_tmp4;
	v8qi v8_Y,v8_RG1,v8_G2B,v8_CbCr;
	v8qi v8_YC;
	v8qi v8_YC_old;
	
	int x,y;
	
	if(ww && wh)   // working with window in image
	{
	    window_width= ww;
	    window_height= wh-2;
	    remain_size= dim_x-ww;
	    v8_src= (v8qi *)(src+dim_x*(wh+starty-2)-8+startx+ww);
	    v8_src_sec= (v8qi *)((unsigned long)v8_src+dim_x);	
	    v8_dst= (v8qi *)(dst+2*ww*(wh-2)-8);   
	}
	else
	{
	    window_width= dim_x;
	    window_height= dim_y-2;		
	    remain_size=0;
	    v8_src= (v8qi *)(src+dim_x*(dim_y-1)-8);
	    v8_src_sec= (v8qi *)((unsigned long)v8_src+dim_x);
	    v8_dst= (v8qi *)(dst+2*dim_x*(dim_y-2)-8);
	}

	y=window_height;
	do
	{
		x= window_width;
		v8_YC_old=v8_zero;
		do
		{
       
		v8_G2B= __builtin_ia32_punpckhbw (*v8_src, v8_zero);
        	v8_tmp1= __builtin_ia32_pmulhw (v8_mul6, v8_G2B); // G2 and B 
		v8_RG1= __builtin_ia32_punpckhbw (*v8_src_sec, v8_zero);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul5, v8_RG1); // R and G1
		v8_tmp1= __builtin_ia32_paddsw (v8_tmp1, v8_tmp2);
		v8_YC=   __builtin_ia32_psllqi (v8_tmp1, 48);
		v8_tmp2= __builtin_ia32_psrlqi (v8_tmp1, 16);   
		v8_tmp2= __builtin_ia32_por (v8_tmp2, v8_YC_old);// Y Carry
		v8_Y=    __builtin_ia32_paddsw (v8_tmp1, v8_tmp2); // Y 
		v8_YC_old= v8_YC;

		v8_tmp1= __builtin_ia32_psubsw (v8_RG1,v8_Y);
		v8_tmp1= __builtin_ia32_pmulhw (v8_mul7, v8_tmp1); 
		v8_tmp1= __builtin_ia32_psllwi (v8_tmp1,1);
		v8_tmp1= __builtin_ia32_paddsw (v8_add3, v8_tmp1);
		

		v8_tmp2= __builtin_ia32_psubsw (v8_G2B,v8_Y);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul8, v8_tmp2);
		v8_tmp2= __builtin_ia32_psllwi (v8_tmp2,1); 
		v8_tmp2= __builtin_ia32_paddsw (v8_add4, v8_tmp2); 
		v8_CbCr= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		v8_tmp1= __builtin_ia32_punpckhwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpckhwd (v8_CbCr,v8_zero );
		v8_tmp3= __builtin_ia32_por (v8_tmp1, v8_tmp2);
	
		v8_tmp1= __builtin_ia32_punpcklwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpcklwd (v8_CbCr,v8_zero );
		v8_tmp4= __builtin_ia32_por (v8_tmp1, v8_tmp2);


		*v8_dst--= __builtin_ia32_packuswb (v8_tmp4, v8_tmp3);


		v8_G2B= __builtin_ia32_punpcklbw (*v8_src--, v8_zero);
        	v8_tmp1= __builtin_ia32_pmulhw (v8_mul6, v8_G2B); //G2 and B 
		v8_RG1= __builtin_ia32_punpcklbw (*v8_src_sec--, v8_zero);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul5, v8_RG1); // R and G1
		v8_tmp1= __builtin_ia32_paddsw (v8_tmp1, v8_tmp2);
		v8_YC=   __builtin_ia32_psllqi (v8_tmp1, 48);
		v8_tmp2= __builtin_ia32_psrlqi (v8_tmp1, 16);   
		v8_tmp2= __builtin_ia32_por (v8_tmp2, v8_YC_old);// Y Carry
		v8_Y=    __builtin_ia32_paddsw (v8_tmp1, v8_tmp2); // Y 
		v8_YC_old= v8_YC;

		v8_tmp1= __builtin_ia32_psubsw (v8_RG1,v8_Y);
		v8_tmp1= __builtin_ia32_pmulhw (v8_mul7, v8_tmp1); 
		v8_tmp1= __builtin_ia32_psllwi (v8_tmp1,1);
		v8_tmp1= __builtin_ia32_paddsw (v8_add3, v8_tmp1);

		v8_tmp2= __builtin_ia32_psubsw (v8_G2B,v8_Y);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul8, v8_tmp2); 
		v8_tmp2= __builtin_ia32_psllwi (v8_tmp2,1);
		v8_tmp2= __builtin_ia32_paddsw (v8_add4, v8_tmp2); 
		v8_CbCr= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		v8_tmp1= __builtin_ia32_punpckhwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpckhwd (v8_CbCr,v8_zero);
		v8_tmp3= __builtin_ia32_por (v8_tmp1, v8_tmp2);
	
		v8_tmp1= __builtin_ia32_punpcklwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpcklwd (v8_CbCr,v8_zero);
		v8_tmp4= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		*v8_dst--= __builtin_ia32_packuswb (v8_tmp4, v8_tmp3);
		}
		while(x-=8);

		v8_src=(v8qi *)((long)v8_src-remain_size);
		v8_src_sec=(v8qi *)((long)v8_src_sec-remain_size);
		tmp_ptr= (int8_t *)(v8_dst+1);
		*(int16_t *)tmp_ptr= *(int16_t *)(tmp_ptr+2);
		tmp_ptr+=(window_width<<1);
		*(int16_t *)(tmp_ptr-2)=*(int16_t *)(tmp_ptr-4);
		last_r= (int8_t *)v8_src+7+(dim_x<<1);
		tmpi= (uint8_t)*last_r-(uint8_t)*(tmp_ptr-1);
		*(tmp_ptr-2)= (mul16x16s(tmpi,coefCr)+128);
		x= window_width;
		v8_YC_old=v8_zero;
		do
		{
       
		v8_RG1= __builtin_ia32_punpckhbw (*v8_src, v8_zero);
        	v8_tmp1= __builtin_ia32_pmulhw (v8_mul5, v8_RG1); // R and G1 
		v8_G2B= __builtin_ia32_punpckhbw (*v8_src_sec, v8_zero);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul6, v8_G2B); // G2 and B
		v8_tmp1= __builtin_ia32_paddsw (v8_tmp1, v8_tmp2);
		v8_YC=   __builtin_ia32_psllqi (v8_tmp1, 48);
		v8_tmp2= __builtin_ia32_psrlqi (v8_tmp1, 16);   
		v8_tmp2= __builtin_ia32_por (v8_tmp2, v8_YC_old);// Y Carry
		v8_Y=    __builtin_ia32_paddsw (v8_tmp1, v8_tmp2); // Y 
		v8_YC_old= v8_YC;

		v8_tmp1= __builtin_ia32_psubsw (v8_RG1,v8_Y);
		v8_tmp1= __builtin_ia32_pmulhw (v8_mul7, v8_tmp1); 
		v8_tmp1= __builtin_ia32_psllwi (v8_tmp1,1);
		v8_tmp1= __builtin_ia32_paddsw (v8_add3, v8_tmp1);
	

		v8_tmp2= __builtin_ia32_psubsw (v8_G2B,v8_Y);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul8, v8_tmp2);
		v8_tmp2= __builtin_ia32_psllwi (v8_tmp2,1); 
		v8_tmp2= __builtin_ia32_paddsw (v8_add4, v8_tmp2);  
		v8_CbCr= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		v8_tmp1= __builtin_ia32_punpckhwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpckhwd (v8_CbCr,v8_zero );
		v8_tmp3= __builtin_ia32_por (v8_tmp1, v8_tmp2);
	
		v8_tmp1= __builtin_ia32_punpcklwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpcklwd (v8_CbCr,v8_zero );
		v8_tmp4= __builtin_ia32_por (v8_tmp1, v8_tmp2);


		*v8_dst--= __builtin_ia32_packuswb (v8_tmp4, v8_tmp3);


		v8_RG1= __builtin_ia32_punpcklbw (*v8_src--, v8_zero);
        	v8_tmp1= __builtin_ia32_pmulhw (v8_mul5, v8_RG1); // R and G1 
		v8_G2B= __builtin_ia32_punpcklbw (*v8_src_sec--, v8_zero);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul6, v8_G2B); // G2 and B
		v8_tmp1= __builtin_ia32_paddsw (v8_tmp1, v8_tmp2);
		v8_YC=   __builtin_ia32_psllqi (v8_tmp1, 48);
		v8_tmp2= __builtin_ia32_psrlqi (v8_tmp1, 16);
		v8_tmp2= __builtin_ia32_por (v8_tmp2, v8_YC_old);// Y Carry   
		v8_Y=    __builtin_ia32_paddsw (v8_tmp1, v8_tmp2); // Y 
		v8_YC_old= v8_YC;

		v8_tmp1= __builtin_ia32_psubsw (v8_RG1,v8_Y);
		v8_tmp1= __builtin_ia32_pmulhw (v8_mul7, v8_tmp1); 
		v8_tmp1= __builtin_ia32_psllwi (v8_tmp1,1);
		v8_tmp1= __builtin_ia32_paddsw (v8_add3, v8_tmp1);

		v8_tmp2= __builtin_ia32_psubsw (v8_G2B,v8_Y);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul8, v8_tmp2); 
		v8_tmp2= __builtin_ia32_psllwi (v8_tmp2,1);
		v8_tmp2= __builtin_ia32_paddsw (v8_add4, v8_tmp2); 
		v8_CbCr= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		v8_tmp1= __builtin_ia32_punpckhwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpckhwd (v8_CbCr,v8_zero);
		v8_tmp3= __builtin_ia32_por (v8_tmp1, v8_tmp2);
	
		v8_tmp1= __builtin_ia32_punpcklwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpcklwd (v8_CbCr,v8_zero);
		v8_tmp4= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		*v8_dst--= __builtin_ia32_packuswb (v8_tmp4, v8_tmp3);
		}
		while(x-=8);
		v8_src=(v8qi *)((long)v8_src-remain_size);
		v8_src_sec=(v8qi *)((long)v8_src_sec-remain_size);
		tmp_ptr= (int8_t *)(v8_dst+1);
		*(int16_t *)tmp_ptr= *(int16_t *)(tmp_ptr+2);
		tmp_ptr+=(window_width<<1);
		*(int16_t *)(tmp_ptr-2)=*(int16_t *)(tmp_ptr-4);
		last_r= (int8_t *)v8_src+7+dim_x;
		tmpi= (uint8_t)*last_r-(uint8_t)*(tmp_ptr-1);
		*(tmp_ptr-2)= (mul16x16s(tmpi,coefCr)+128);

	}
	while(y-=2);
}


void debayerRGB_mode3_mmx(uint8_t *dst, uint8_t *src, int dim_x, int dim_y,int startx,int starty,int ww,int wh)
{
  /*green2    -- blue*/
  /*red -- green1*/
	int8_t *tmp_ptr,*last_r;
	int tmpi;
	int window_width,window_height;
	int remain_size;

	v8qi *v8_src,*v8_src_sec,*v8_dst,v8_tmp1,v8_tmp2,v8_tmp3,v8_tmp4;
	v8qi v8_Y,v8_RG1,v8_G2B,v8_CbCr;
	v8qi v8_YC;
	v8qi v8_YC_old;
	
	int x,y;

	if(ww && wh)   // working with window in image
	{
	    window_width= ww;
	    window_height= wh-2;
	    remain_size= dim_x-ww;
	    v8_src= (v8qi *)(src+dim_x*(wh+starty-2)-8+startx+ww);
	    v8_src_sec= (v8qi *)((unsigned long)v8_src+dim_x);
	    v8_dst= (v8qi *)(dst+2*ww*(wh-2)-8);	   
	}
	else
	{
	    window_width= dim_x;
	    window_height= dim_y-2;		
	    remain_size=0;
	    v8_src= (v8qi *)(src+dim_x*(dim_y-1)-8);
	    v8_src_sec= (v8qi *)((unsigned long)v8_src+dim_x);
	    v8_dst= (v8qi *)(dst+2*dim_x*(dim_y-2)-8);
	}

	y=window_height;
	do
	{
		x= window_width;
		v8_YC_old=v8_zero;

		do
		{
       
		v8_RG1= __builtin_ia32_punpckhbw (*v8_src, v8_zero);
        	v8_tmp1= __builtin_ia32_pmulhw (v8_mul5, v8_RG1); // R and G1 
		v8_G2B= __builtin_ia32_punpckhbw (*v8_src_sec, v8_zero);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul6, v8_G2B); // G2 and B
		v8_tmp1= __builtin_ia32_paddsw (v8_tmp1, v8_tmp2);
		v8_YC=   __builtin_ia32_psllqi (v8_tmp1, 48);
		v8_tmp2= __builtin_ia32_psrlqi (v8_tmp1, 16);   
		v8_tmp2= __builtin_ia32_por (v8_tmp2, v8_YC_old);// Y Carry
		v8_Y=    __builtin_ia32_paddsw (v8_tmp1, v8_tmp2); // Y 
		v8_YC_old= v8_YC;

		v8_tmp1= __builtin_ia32_psubsw (v8_RG1,v8_Y);
		v8_tmp1= __builtin_ia32_pmulhw (v8_mul7, v8_tmp1); 
		v8_tmp1= __builtin_ia32_psllwi (v8_tmp1,1);
		v8_tmp1= __builtin_ia32_paddsw (v8_add3, v8_tmp1);
	

		v8_tmp2= __builtin_ia32_psubsw (v8_G2B,v8_Y);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul8, v8_tmp2);
		v8_tmp2= __builtin_ia32_psllwi (v8_tmp2,1); 
		v8_tmp2= __builtin_ia32_paddsw (v8_add4, v8_tmp2);  
		v8_CbCr= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		v8_tmp1= __builtin_ia32_punpckhwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpckhwd (v8_CbCr,v8_zero );
		v8_tmp3= __builtin_ia32_por (v8_tmp1, v8_tmp2);
	
		v8_tmp1= __builtin_ia32_punpcklwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpcklwd (v8_CbCr,v8_zero );
		v8_tmp4= __builtin_ia32_por (v8_tmp1, v8_tmp2);


		*v8_dst--= __builtin_ia32_packuswb (v8_tmp4, v8_tmp3);


		v8_RG1= __builtin_ia32_punpcklbw (*v8_src--, v8_zero);
        	v8_tmp1= __builtin_ia32_pmulhw (v8_mul5, v8_RG1); // R and G1 
		v8_G2B= __builtin_ia32_punpcklbw (*v8_src_sec--, v8_zero);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul6, v8_G2B); // G2 and B
		v8_tmp1= __builtin_ia32_paddsw (v8_tmp1, v8_tmp2);
		v8_YC=   __builtin_ia32_psllqi (v8_tmp1, 48);
		v8_tmp2= __builtin_ia32_psrlqi (v8_tmp1, 16);
		v8_tmp2= __builtin_ia32_por (v8_tmp2, v8_YC_old);// Y Carry   
		v8_Y=    __builtin_ia32_paddsw (v8_tmp1, v8_tmp2); // Y 
		v8_YC_old= v8_YC;

		v8_tmp1= __builtin_ia32_psubsw (v8_RG1,v8_Y);
		v8_tmp1= __builtin_ia32_pmulhw (v8_mul7, v8_tmp1); 
		v8_tmp1= __builtin_ia32_psllwi (v8_tmp1,1);
		v8_tmp1= __builtin_ia32_paddsw (v8_add3, v8_tmp1);

		v8_tmp2= __builtin_ia32_psubsw (v8_G2B,v8_Y);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul8, v8_tmp2); 
		v8_tmp2= __builtin_ia32_psllwi (v8_tmp2,1);
		v8_tmp2= __builtin_ia32_paddsw (v8_add4, v8_tmp2); 
		v8_CbCr= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		v8_tmp1= __builtin_ia32_punpckhwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpckhwd (v8_CbCr,v8_zero);
		v8_tmp3= __builtin_ia32_por (v8_tmp1, v8_tmp2);
	
		v8_tmp1= __builtin_ia32_punpcklwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpcklwd (v8_CbCr,v8_zero);
		v8_tmp4= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		*v8_dst--= __builtin_ia32_packuswb (v8_tmp4, v8_tmp3);
		}
		while(x-=8);
		
		v8_src=(v8qi *)((long)v8_src-remain_size);
		v8_src_sec=(v8qi *)((long)v8_src_sec-remain_size);
		tmp_ptr= (int8_t *)(v8_dst+1);
		*(int16_t *)tmp_ptr= *(int16_t *)(tmp_ptr+2);
		tmp_ptr+=(window_width<<1);
		*(int16_t *)(tmp_ptr-2)=*(int16_t *)(tmp_ptr-4);
		last_r= (int8_t *)v8_src+7+dim_x;
		tmpi= (uint8_t)*last_r-(uint8_t)*(tmp_ptr-1);
		*(tmp_ptr-2)= (mul16x16s(tmpi,coefCr)+128);
		x= window_width;
		v8_YC_old=v8_zero;
		do
		{
       
		v8_G2B= __builtin_ia32_punpckhbw (*v8_src, v8_zero);
        	v8_tmp1= __builtin_ia32_pmulhw (v8_mul6, v8_G2B); // G2 and B 
		v8_RG1= __builtin_ia32_punpckhbw (*v8_src_sec, v8_zero);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul5, v8_RG1); // R and G1
		v8_tmp1= __builtin_ia32_paddsw (v8_tmp1, v8_tmp2);
		v8_YC=   __builtin_ia32_psllqi (v8_tmp1, 48);
		v8_tmp2= __builtin_ia32_psrlqi (v8_tmp1, 16);   
		v8_tmp2= __builtin_ia32_por (v8_tmp2, v8_YC_old);// Y Carry
		v8_Y=    __builtin_ia32_paddsw (v8_tmp1, v8_tmp2); // Y 
		v8_YC_old= v8_YC;

		v8_tmp1= __builtin_ia32_psubsw (v8_RG1,v8_Y);
		v8_tmp1= __builtin_ia32_pmulhw (v8_mul7, v8_tmp1); 
		v8_tmp1= __builtin_ia32_psllwi (v8_tmp1,1);
		v8_tmp1= __builtin_ia32_paddsw (v8_add3, v8_tmp1);
		

		v8_tmp2= __builtin_ia32_psubsw (v8_G2B,v8_Y);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul8, v8_tmp2);
		v8_tmp2= __builtin_ia32_psllwi (v8_tmp2,1); 
		v8_tmp2= __builtin_ia32_paddsw (v8_add4, v8_tmp2); 
		v8_CbCr= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		v8_tmp1= __builtin_ia32_punpckhwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpckhwd (v8_CbCr,v8_zero );
		v8_tmp3= __builtin_ia32_por (v8_tmp1, v8_tmp2);
	
		v8_tmp1= __builtin_ia32_punpcklwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpcklwd (v8_CbCr,v8_zero );
		v8_tmp4= __builtin_ia32_por (v8_tmp1, v8_tmp2);


		*v8_dst--= __builtin_ia32_packuswb (v8_tmp4, v8_tmp3);


		v8_G2B= __builtin_ia32_punpcklbw (*v8_src--, v8_zero);
        	v8_tmp1= __builtin_ia32_pmulhw (v8_mul6, v8_G2B); //G2 and B 
		v8_RG1= __builtin_ia32_punpcklbw (*v8_src_sec--, v8_zero);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul5, v8_RG1); // R and G1
		v8_tmp1= __builtin_ia32_paddsw (v8_tmp1, v8_tmp2);
		v8_YC=   __builtin_ia32_psllqi (v8_tmp1, 48);
		v8_tmp2= __builtin_ia32_psrlqi (v8_tmp1, 16);   
		v8_tmp2= __builtin_ia32_por (v8_tmp2, v8_YC_old);// Y Carry
		v8_Y=    __builtin_ia32_paddsw (v8_tmp1, v8_tmp2); // Y 
		v8_YC_old= v8_YC;

		v8_tmp1= __builtin_ia32_psubsw (v8_RG1,v8_Y);
		v8_tmp1= __builtin_ia32_pmulhw (v8_mul7, v8_tmp1); 
		v8_tmp1= __builtin_ia32_psllwi (v8_tmp1,1);
		v8_tmp1= __builtin_ia32_paddsw (v8_add3, v8_tmp1);

		v8_tmp2= __builtin_ia32_psubsw (v8_G2B,v8_Y);
		v8_tmp2= __builtin_ia32_pmulhw (v8_mul8, v8_tmp2); 
		v8_tmp2= __builtin_ia32_psllwi (v8_tmp2,1);
		v8_tmp2= __builtin_ia32_paddsw (v8_add4, v8_tmp2); 
		v8_CbCr= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		v8_tmp1= __builtin_ia32_punpckhwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpckhwd (v8_CbCr,v8_zero);
		v8_tmp3= __builtin_ia32_por (v8_tmp1, v8_tmp2);
	
		v8_tmp1= __builtin_ia32_punpcklwd (v8_zero,v8_Y);
		v8_tmp2= __builtin_ia32_punpcklwd (v8_CbCr,v8_zero);
		v8_tmp4= __builtin_ia32_por (v8_tmp1, v8_tmp2);

		*v8_dst--= __builtin_ia32_packuswb (v8_tmp4, v8_tmp3);
		}
		while(x-=8);

		v8_src=(v8qi *)((long)v8_src-remain_size);
		v8_src_sec=(v8qi *)((long)v8_src_sec-remain_size);
		tmp_ptr= (int8_t *)(v8_dst+1);
		*(int16_t *)tmp_ptr= *(int16_t *)(tmp_ptr+2);
		tmp_ptr+=(window_width<<1);
		*(int16_t *)(tmp_ptr-2)=*(int16_t *)(tmp_ptr-4);
		last_r= (int8_t *)v8_src+7+(dim_x<<1);
		tmpi= (uint8_t)*last_r-(uint8_t)*(tmp_ptr-1);
		*(tmp_ptr-2)= (mul16x16s(tmpi,coefCr)+128);

	}
	while(y-=2);
}

static void check_mmx_sse2(int *sse2_present, int *mmx_present) {
#ifdef __x86_64__
	*sse2_present	= 1;
	*mmx_present	= 1;
#else
	int tmp1=0,tmp2=0;
	__asm__ ("pushfl;\
		  popl %0;\
		  movl $0x200000,%1;\
		  xorl %0,%1;\
		  pushl %1;\
		  popfl;\
		  pushfl;\
		  popl %1;\
		  xorl %1,%0":"=r"(tmp1), "=r"(tmp2));

	if(tmp1&0x200000)  {/*cpuid present*/
	   /* mmx and sse2 present control */
		int a,b,c,d;
		cpuid(1,a,b,c,d);
		if(d&0x800000)
			*mmx_present=1;
		else
			*mmx_present=0;
		if(d&0x4000000)
			*sse2_present=1;
		else
			*sse2_present=0;

	}
	else
		*sse2_present=*mmx_present=0;
#endif

	if (*sse2_present)
		TRACEPNF(0, "CPU: SSE2 present\n");

	if (*mmx_present)
		TRACEPNF(0, "CPU: MMX present\n");
}

int arch_probe_fast_debayer(debayer_api_t *api, int dim_x, int startx, int ww)
{
	static int early_probe = -1;
	int sse2_present, mmx_present;
	static debayer_api_t _api = {};

	if((dim_x & 0xf) || (startx & 0xf) || (ww & 0xf))
		return -1;
	
	if(early_probe == -1) {
		check_mmx_sse2(&sse2_present, &mmx_present);

		early_probe = 1;

		if(sse2_present) {
			_api.debayerRGB_func[0] = &debayerRGB_mode0_sse2;
			_api.debayerRGB_func[1] = &debayerRGB_mode1_sse2;
			_api.debayerRGB_func[2] = &debayerRGB_mode2_sse2;
			_api.debayerRGB_func[3] = &debayerRGB_mode3_sse2;

			TRACEP(0, "%s: SSE2 selected\n", __func__);
			
			early_probe = 1;

			goto out;
		}

		if(mmx_present) {
			_api.debayerRGB_func[0] = &debayerRGB_mode0_mmx;
			_api.debayerRGB_func[1] = &debayerRGB_mode1_mmx;
			_api.debayerRGB_func[2] = &debayerRGB_mode2_mmx;
			_api.debayerRGB_func[3] = &debayerRGB_mode3_mmx;

			TRACEP(0, "%s: MMX selected\n", __func__);

			early_probe = 2;

			goto out;
		}

		TRACEP(0, "%s: no optimization engine found\n", __func__);

		early_probe = 0;
	}

out:
	if(early_probe > 0)
		*api = _api;

	return early_probe;
}
