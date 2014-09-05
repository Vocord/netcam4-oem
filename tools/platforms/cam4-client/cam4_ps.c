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


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include <linux/if_packet.h>
#include <linux/if_ether.h>   /* The L2 protocols */

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#define __USE_XOPEN
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <asm/types.h>

#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

#define TRACE_PRIVATE_PREFIX    1
#include <trace.h>
#undef  TRACE_LEVEL
#define TRACE_LEVEL 1

#define DEBUG_ME 0

#include <abi/p3.h>
#include <abi/p3_helpers.h>
#include <abi/io.h>
#include <abi/device.h>
#include <api/mcast-cl.h>
#include <os-helpers/net-sockets.h>
#include <os-helpers/pthreads.h>

#include "cam4-cmd-cl.h"

#include <sys/ipc.h>
#include <sys/shm.h>
#include <abi/lb-task-cam.h>
#include <abi/lb-task-bar.h>

#include "shared_objects.h"

#include <asm/byteorder.h>
#include <avi/avi.h>
#include <avi/fourcc.h>
#include <avi/structures.h>
#include <os-helpers/inet_macros.h>

#include "cam4_ps.h"

#if 0
static char *names[]={
	"<", /* incoming */
	"B", /* broadcast */
	"M", /* multicast */
	"P", /* promisc */
	">", /* outgoing */
};
#endif

#define NIPQUAD(addr)   ((unsigned char *)&addr)[0], \
                        ((unsigned char *)&addr)[1], \
                        ((unsigned char *)&addr)[2], \
                        ((unsigned char *)&addr)[3]

int debayer_mode	= -1;
int quad = 0;
volatile int	no_sig_exit = 1;

static char* trace_prefix = "cam4_ps: ";
static struct timeval tv;
double time_diff(struct timeval tv1, struct timeval tv2){
	unsigned int tsec = tv2.tv_sec - tv1.tv_sec;
	unsigned int tusec = tv2.tv_usec - tv1.tv_usec;
	if (tusec<0) {
		tsec--;
		tusec += 1000000;
	}
	return tsec+(double)(tusec)/1000000;

}

struct timeval time_add(struct timeval tv1, double diff){
	unsigned int tsec = tv1.tv_sec + (int)(diff);
	unsigned int tusec = tv1.tv_usec + (int)((diff - (int)(diff))*1000000);
	if (tusec>1000000) {
		tsec++;
		tusec -= 1000000;
	}
	struct timeval res;
	res.tv_sec = tsec;
	res.tv_usec = tusec;
	return res;
}

typedef struct connection_s {
	int		fd;
	char		*map;
	struct		tpacket_req req;
	struct		iovec *ring;
} connection_t;

connection_t conn;

int		fd=-1;
char		*map;
struct		tpacket_req req;
struct		iovec *ring;

static int 	part_num = 0;
uint32_t	offs_static = 0;

FILE *I;

uint8_t* shmaddr[2];
uint8_t* shmaddr3;

uint32_t shmid1 = -1;
uint32_t shmid2 = -1;
uint32_t shmid3 = -1;

debayer_api_t default_debayer_api = {};

int cam4_script_processing(cam4_rd_t* cam4_rd);

/* local func declare */
static int write_separate(cam4_rd_t *cam4_rd);
static int cam4_mcast(cam4_rd_t *cam4_rd);

#ifdef CAM4_PS_LIB
cam4_ps_cb_f cam4_ps_cb = NULL;
#endif

enum state_flags {
    f_flag = 1<<0,
    n_flag = 1<<1,
    s_flag = 1<<2,

    m1_flag = 1<<3,			/* CPU ID */
    m2_flag = 1<<4,			/* IP address */

    g1_flag = 1<<5,			/* stream on */
    g2_flag = 1<<6,			/* stream off */

    frame_done_flag = 1<<14
};

static uint8_t SENSOR[7] = "/sensor";

static void show_the_banner(void)
{
    TRACE(0,	"Client for RAW stream from CAM4 by Vocord\n"
		    "Usage: sudo ./cam4_ps [OPTIONS]\n"
		    "Output format:\n"
		    "\t-b				request RAW bar flow\n"
		    "\t-f				output file for RAW frames\n"
		    "\t-n				number of RAW frames to process\n"
		    "\t-s				save each frame in file (raw.001 raw.002). Use only with -n flag\n"
		    "\t-g				just start/stop stream\n"
		    "\t\t 1				start\n"
		    "\t\t 2				stop\n"
		    "\tC val				CAMCTL mode"
		    "Find device option:\n"
		    "\t-m				work with -v (value) flag\n"
		    "\t\t 1				CPU ID\n"
		    "\t\t 2				IP address\n"
		    "\t-v				value (IP addres, Host ID, etc). look at -m option\n"
		    "\n"
		    "\t-d				IP of the client (CAM4 Raw stream -> dst IP)\n"
		    "\n"
		    "\t-h				this banner\n"
    );
    TRACE(0,/*"Press space to dump raw data\n"
		"Press \'d\' to dump yuv data\n"
		"Press \'s\' to start recording group of frames\n"
		"Press \'f\' to finish recording group of frames\n"
		"Press \'h\' to show/hide histogram\n"
		"Press \'r\' to reinitialize stream capturing\n"
		"\n"*/
		"Command interface: commands a read from /tmp/cam4.fifo\n"
		"Availiable commands:\n"
		"EXP:num -- set exposure to \"num\"\n"
		"DUMPRAW:num -- make raw dump after \"num\" seconds\n"
		"DUMPYUV:num -- make yuv dump after \"num\" seconds\n"
		"PERIOD:num -- set period to \"num\"\n"
		"GAINS:n1 n2 n3 n4 n5 -- set gains\n"
		"VIDEO:START -- start recording avi video\n"
		"VIDEO:FINISH -- stop recording avi video\n"
		"RAWVIDEO:START -- start recording raw video\n"
		"RAWVIDEO:FINISH -- stop recording raw video\n"
		);
};

void sigproc(int sig)
{

	no_sig_exit = 0;
}

void sighand(int signo)
{
    return;
}

int write_raw_video(
	cam4_rd_t	*ctx,
	void		*src,
	int		todo
)
{
	static char name[255];
	static unsigned	idx	= 0;
	switch(ctx->raw_video_writing) {
		case VIDEO_WRITE_START:
			snprintf(name, sizeof(name), "video_%05ux%05ux%02u_%05d.vraw",
				ctx->w,
				ctx->h,
				ctx->bits,
				idx
				);
			ctx->raw_video_fd = open(name, O_CREAT | O_WRONLY, 0666);
			printf("Start recording raw video %s\n",name);
			ctx->raw_video_writing = VIDEO_WRITE_PROCESS;
			idx++;
			break;
		case VIDEO_WRITE_FINISH:
			close(ctx->raw_video_fd);
			ctx->raw_video_writing = VIDEO_WRITE_NONE;
			break;
		case VIDEO_WRITE_PROCESS:
			while (todo) {
				int res = write(ctx->raw_video_fd,src, todo);
				todo -= res;
				src += res;
			}
			break;
		case VIDEO_WRITE_NONE:
			break;
	}
	return 0;
}

int write_video(cam4_rd_t* ctx, void* src, int todo) {
	static char		name[255];

	static unsigned	idx	= 0;
	avi_rwh_t *h = ctx->avi;
	switch(ctx->video_writing) {
		case VIDEO_WRITE_START:
			snprintf(name, sizeof(name), "video_%05ux%05ux%02u_%05d.avi",
				ctx->w,
				ctx->h,
				ctx->bits,
				idx
				);
			ctx->avi = avi_file_open(name);
			h = ctx->avi;
			avi_file_list_open(h, fccRIFF, fcc_AVI);
			avi_file_list_open(h, fccLIST, fcc_hdrl);
			AVI_avih AVI_avih_data = {
				.ulMicroSecPerFrame	= __cpu_to_le32((uint32_t)125000),
				.ulMaxBytesPerSec	= 0,
				.ulPaddingGranularity	= 0,
				.ulFlags 		=  0, //__cpu_to_le32(AVIF_HASINDEX|AVIF_MUSTUSEINDEX|AVIF_ISINTERLEAVED*0),
				.ulTotalFrames		= __cpu_to_le32(100),
				.ulInitialFrames	= 0,				/* FIXME TODO */
				.ulStreams		= 1,
				.ulSuggestedBufferSize	= __cpu_to_le32(ctx->w * ctx->h * ctx->bits / 8),
				.ulWidth		= __cpu_to_le32(ctx->w),
				.ulHeight		= __cpu_to_le32(ctx->h),
				.ulReserved		= { },	/* Set to zero */
			};
			avi_file_chunk_append(h,fcc_avih, (uint8_t*)&AVI_avih_data, sizeof(AVI_avih));
			avi_file_list_open(h, fccLIST, fcc_strl);
			AVI_strh  AVI_strh_data = {
				.fccType.v32		= fcc_vids,
				.fccHandler.v32		= fccUYVY,
				.ulFlags		= 0,
				.ulPriority		= 0,
				.ulInitialFrames	= 0,
				.ulScale		= __cpu_to_le32((uint32_t)1),	/**/
				.ulRate			= __cpu_to_le32((uint32_t)8),
				.ulStart		= 0,
				.ulLength		= __cpu_to_le32(100),
				.ulSuggestedBufferSize	= __cpu_to_le32(ctx->w * ctx->h * ctx->bits / 8),
				.ulQuality		= 0,	/* */
				.ulSampleSize		= 0,
				.ulrcFrame		= {0, 0, __cpu_to_le16(ctx->w),	__cpu_to_le16(ctx->h),},  /* Coordinates x,y */
			};
			avi_file_chunk_append(h,fcc_strh, (uint8_t*)&AVI_strh_data, sizeof(AVI_strh));
			AVI_vidsB AVI_strf_data = {
				.lSize = __cpu_to_le32(sizeof(AVI_vidsB)),
				.lWidth = __cpu_to_le32(ctx->w),
				.lHeight = __cpu_to_le32(ctx->h),
				.wPlanes = __CPU_TO_LE16((uint16_t)1),
				.wBitCount = __CPU_TO_LE16((uint16_t)ctx->bits),
				.lCompression = {.v32 = fccUYVY},
				.lSizeImage = __cpu_to_le32(ctx->w * ctx->h * ctx->bits / 8),
				.wXPelsPerMeter	= 0,
				.wYPelsPerMeter	= 0,
				.lClrUsed	= 0,
				.lClrImportant	= 0,
			};
			avi_file_chunk_append(h,fcc_strf, (uint8_t*)&AVI_strf_data, sizeof(AVI_vidsB));
			avi_file_list_close(h); // strl
			avi_file_list_close(h); // hdrl
			avi_file_list_open(h, fccLIST, fcc_movi);


			ctx->video_writing = VIDEO_WRITE_PROCESS;
			break;


		case VIDEO_WRITE_FINISH:
			printf("Finish recording video %s\n",name);
			avi_file_list_close(h); // fcc_movi
			avi_file_list_close(h); // fccRIFF
			avi_file_close(h);
			ctx->video_writing = VIDEO_WRITE_NONE;
			idx++;
			break;


		case VIDEO_WRITE_PROCESS:
			avi_file_chunk_append(h,fcc_00db, (uint8_t*)src, todo);
			break;


		case VIDEO_WRITE_NONE:
			break;
	}
	return 0;

}

const int16_t coefrY=9797; // 0.299
const int16_t coefgY= 19234; // 0.587
const int16_t coefbY= 3735; // 0.114

const int16_t coefCr= 23363;// 0.713
const int16_t coefCb= 18481;// 0.564


inline int16_t mul16x16s(int16_t a,int16_t b)
{
	return (int16_t)(((int32_t)a*b) >> 15);
}

void debayerRGB_fast_mode0(uint8_t *dst, uint8_t *src, int dim_x, int dim_y,int startx,int starty,int ww,int wh)
{
	int x,y;
	int r,g,b;
	int window_width,window_height;
	int remain_size;
	if(ww && wh)   // working with window in image
	{
	    window_width= ww-2;
	    window_height= wh-2;
	    remain_size= dim_x-ww;
	    src+=(dim_x*starty+startx);
	}
	else
	{
	    window_width= dim_x-2;
	    window_height= dim_y-2;
	    remain_size=0;
	}
	uint8_t *high_left=src,*high=high_left+1,*high_right=high+1;
	uint8_t *left=src+dim_x,*center=left+1,*right=center+1;
	uint8_t *low_left=left+dim_x,*low=low_left+1,*low_right=low+1;
	uint16_t Y,Cr,Cb;
	uint8_t *out=dst;

			//for( y=1; y < dim_y - 1; y+=2 )
			y= window_height;
			do
			{
				/* green 1 */
				r = (*left + *right)>>1; 	//(d_RGB_y[x-1] + d_RGB_y[x+1]) / 2;
		    		g =  *center;			//d_RGB_y[x];
		    		b = (*low + *high)>>1;	//(d_RGB_y_low[x] + d_RGB_y_high[x]) / 2;

				//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
				Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
				//Cr= (uint8_t)(0.713*(r-Y) + 128);
				Cr= (mul16x16s(r-Y,coefCr)+128);
									/* copy data on edge */
				*out++=Cr;			//dst[j]   = CbCr[CbCr_idx];		// Cb or Cr
				*out++=Y;			//dst[j+1] = Y;		// Luma
								//j += 2;
				//for( x=1; x < dim_x - 1; x+=2 )
				x= window_width;
				do
				{
						/* green 1 */
					r = (*left++ + *right++)>>1; 	//(d_RGB_y[x-1] + d_RGB_y[x+1]) / 2;
		    			g =  *center++;			//d_RGB_y[x];
		    			b = (*low++ + *high++)>>1;	//(d_RGB_y_low[x] + d_RGB_y_high[x]) / 2;
					low_left++;low_right++;high_left++;high_right++;

					//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
					Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
					//Cr= (uint8_t)(0.713*(r-Y) + 128);
					Cr= (mul16x16s(r-Y,coefCr)+128);

					*out++=Cr;
					*out++=Y;
		    				/* red */
		    			r= *center++;							  //r =  d_RGB_y[x];
		    			g= (*low++ + *left++ + *right++ + *high++)>>2;			  //g = (d_RGB_y_low[x]   + d_RGB_y[x-1]     + d_RGB_y[x+1]      + d_RGB_y_high[x]  ) / 4;
		    			b= (*low_left++ + *low_right++ + *high_left++ + *high_right++)>>2; //b = (d_RGB_y_low[x-1] + d_RGB_y_low[x+1] + d_RGB_y_high[x-1] + d_RGB_y_high[x+1]) / 4;

					//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
					Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
					//Cb= (uint8_t)(0.564*(b-Y) + 128);
					Cb= (mul16x16s(b-Y,coefCb)+128);
					Cr= (mul16x16s(r-Y,coefCr)+128);

					*out++=Cb;
					*out++=Y;

				}
				while(x-=2);
				/* copy data on edge */
				*out++=Cr;	//dst[j+1] = Y;		// Luma
				*out++=Y;	//dst[j]   = CbCr[CbCr_idx];		// Cb or Cr
						//j += 2;


				left+=(2+remain_size);center+=(2+remain_size);right+=(2+remain_size);
				low+=(2+remain_size);high+=(2+remain_size);
				low_left+=(2+remain_size);low_right+=(2+remain_size);high_left+=(2+remain_size);high_right+=(2+remain_size);
				/* blue */
		    		r= (*low_left + *low_right + *high_left + *high_right)>>2; //r = (d_RGB_y_low[x-1] + d_RGB_y_low[x+1] + d_RGB_y_high[x-1] + d_RGB_y_high[x+1]) / 4;
		    		g= (*low + *left + *right + *high)>>2;			//g = (d_RGB_y_low[x]   + d_RGB_y[x-1]     + d_RGB_y[x+1]      + d_RGB_y_high[x]  ) / 4;
		    		b =  *center;							//d_RGB_y[x];

				//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
				Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
				//Cr= (uint8_t)(0.713*(r-Y) + 128);
				Cr= (mul16x16s(r-Y,coefCr)+128);

				/* copy data on edge */

				*out++=Cr;			//dst[j]   = CbCr[CbCr_idx];		// Cb or Cr
				*out++=Y;			//dst[j+1] = Y;		// Luma
									//j += 2;
				//for( x=1; x < dim_x - 1; x+=2 )
				x= window_width;
				do
				{
					/* blue */
		    			r= (*low_left++ + *low_right++ + *high_left++ + *high_right++)>>2; //r = (d_RGB_y_low[x-1] + d_RGB_y_low[x+1] + d_RGB_y_high[x-1] + d_RGB_y_high[x+1]) / 4;
		    			g= (*low++ + *left++ + *right++ + *high++)>>2;			//g = (d_RGB_y_low[x]   + d_RGB_y[x-1]     + d_RGB_y[x+1]      + d_RGB_y_high[x]  ) / 4;
		    			b =  *center++;							//d_RGB_y[x];

					//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
					Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
					//Cr= (uint8_t)(0.713*(r-Y) + 128);
					Cr= (mul16x16s(r-Y,coefCr)+128);


					*out++=Cr;
					*out++=Y;

		  		    /* green 2 */
		    			r= (*low++ + *high++)>>1; //r = (d_RGB_y_low[x] + d_RGB_y_high[x]) / 2;
		    			g =  *center++;		//g=d_RGB_y[x];
		    			b= (*left++ + *right++) >>1; //b = (d_RGB_y[x-1] + d_RGB_y[x+1]) / 2;
					low_left++;low_right++;high_left++;high_right++;

					//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
					Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
					//Cb= (uint8_t)(0.564*(b-Y) + 128);
					Cb= (mul16x16s(b-Y,coefCb)+128);
					Cr= (mul16x16s(r-Y,coefCr)+128);

					*out++=Cb;
					*out++=Y;
				}
				while(x-=2);
				/* copy data on edge */
				*out++=Cr;	//dst[j+1] = Y;		// Luma
				*out++=Y;	//dst[j]   = CbCr[CbCr_idx];		// Cb or Cr
						//j += 2;

				left+=(2+remain_size);center+=(2+remain_size);right+=(2+remain_size);
				low+=(2+remain_size);high+=(2+remain_size);
				low_left+=(2+remain_size);low_right+=(2+remain_size);high_left+=(2+remain_size);high_right+=(2+remain_size);

			}
			while(y-=2);
}

#if 1
typedef struct {
	uint8_t		left;
	uint8_t		center;
	uint8_t		right;
} __attribute__((packed)) bayer_trio_t;

// SENS_BAYER_RGB_PHASE_RG1BG2	= (0x1<<0)
void debayerRGB_ar_mode1(uint8_t *_dst, uint8_t *_src, int dim_x, int dim_y, int startx, int starty, int ww, int wh)
{
	int 		x, y = 0;
	int 		r, g, b;
	int 		window_width, window_height;
	uint16_t 	Y, Cr, Cb;
	uint8_t 	*out = _dst;
	uint8_t		*cur_line = _src;

	bayer_trio_t	*high, *mid, *low;

	printf("XXX\n");

	if(ww && wh) {  // working with window in image
	    window_width  = ww - 2;
	    window_height = wh - 2;
	    cur_line += (dim_x * starty + startx);
	} else {
	    window_width  = dim_x - 2;
	    window_height = dim_y - 2;
	}

	/**** main body ****/
	do {
		x = 0;

		high = (bayer_trio_t*)cur_line;
		mid  = high + dim_x;
		low  = mid + dim_x;

		do {
			r = mid->center;
			g = (high->center + mid->left   + mid->right + low->center) >> 2;
			b = (high->left   + high->right + low->left  + low->right ) >> 2;

			Y  = (mul16x16s(r,coefrY) + mul16x16s(g,coefgY) + mul16x16s(b,coefbY));
			Cr = (mul16x16s(r-Y,coefCr) + 128);
			//Cb = (mul16x16s(b-Y,coefCb) + 128);

			*out++ = Cr;
			*out++ = Y;

			if(!x) {
				/* copy data to edge */
				*out++ = Cr;
				*out++ = Y;
			}
			
			high = (typeof(high))((uint8_t*)high + 1);
			mid  = (typeof(mid))((uint8_t*)mid + 1);
			low  = (typeof(low))((uint8_t*)low + 1);

			r = (mid->left + mid->right) >> 1;
			g = mid->center;
			b = (high->center + low->center) >> 1;

			Y = (mul16x16s(r,coefrY) + mul16x16s(g,coefgY) + mul16x16s(b,coefbY));
			Cb= (mul16x16s(b-Y,coefCb)+128);
			//Cr= (mul16x16s(r-Y,coefCr)+128);

			*out++ = Cb;
			*out++ = Y;

			high = (typeof(high))((uint8_t*)high + 1);
			mid  = (typeof(mid))((uint8_t*)mid + 1);
			low  = (typeof(low))((uint8_t*)low + 1);

			x += 2;
		} while(x < window_width);

		/* copy data to edge */
		*out++ = Cb;
		*out++ = Y;

		/* advance to next line */

		cur_line += dim_x;

		high = (bayer_trio_t*)cur_line;
		mid  = high + dim_x;
		low  = mid + dim_x;

		x = 0;

		do {
			r = (high->center + low->center) >> 1;
			g = mid->center;
			b = (mid->left + mid->right) >> 1;

			Y  = (mul16x16s(r,coefrY) + mul16x16s(g,coefgY) + mul16x16s(b,coefbY));
			Cr = (mul16x16s(r-Y,coefCr) + 128);
			//Cb = (mul16x16s(b-Y,coefCb) + 128);

			*out++ = Cr;
			*out++ = Y;

			if(!x) {
				/* copy data to edge */
				*out++ = Cr;
				*out++ = Y;
			}
			
			high = (typeof(high))((uint8_t*)high + 1);
			mid  = (typeof(mid))((uint8_t*)mid + 1);
			low  = (typeof(low))((uint8_t*)low + 1);

			r = (high->left + high->right + low->left + low->right) >> 2;
			g = (mid->left + high->center + mid->right + low->center) >> 2;
			b = mid->center;

			Y = (mul16x16s(r,coefrY) + mul16x16s(g,coefgY) + mul16x16s(b,coefbY));
			Cb= (mul16x16s(b-Y,coefCb)+128);
			//Cr= (mul16x16s(r-Y,coefCr)+128);

			*out++ = Cb;
			*out++ = Y;

			high = (typeof(high))((uint8_t*)high + 1);
			mid  = (typeof(mid))((uint8_t*)mid + 1);
			low  = (typeof(low))((uint8_t*)low + 1);

			x += 2;
		} while(x < window_width);

		/* copy data to edge */
		*out++ = Cr;
		*out++ = Y;

		/* advance to next line */
		cur_line += dim_x;
		
		y += 2;
	} while(y < window_height);

}
#endif

void debayerRGB_fast_mode1(uint8_t *dst, uint8_t *src, int dim_x, int dim_y,int startx,int starty,int ww,int wh)
{
	int x,y;
	int r,g,b;
	int window_width,window_height;
	int remain_size;
	if(ww && wh)   // working with window in image
	{
	    window_width= ww-2;
	    window_height= wh-2;
	    remain_size= dim_x-ww;
	    src+=(dim_x*starty+startx);
	}
	else
	{
	    window_width= dim_x-2;
	    window_height= dim_y-2;
	    remain_size=0;
	}
	uint8_t *high_left=src,*high=high_left+1,*high_right=high+1;
	uint8_t *left=src+dim_x,*center=left+1,*right=center+1;
	uint8_t *low_left=left+dim_x,*low=low_left+1,*low_right=low+1;
	uint16_t Y,Cr,Cb;
	uint8_t *out=dst;

	//for( y=1; y < dim_y - 1; y+=2 )
	y= window_height;
	do
	{
			    /* red */
		r= *center;							  //r =  d_RGB_y[x];
		g= (*low + *left + *right + *high)>>2;			  //g = (d_RGB_y_low[x]   + d_RGB_y[x-1]     + d_RGB_y[x+1]      + d_RGB_y_high[x]  ) / 4;
		b= (*low_left + *low_right + *high_left + *high_right)>>2; //b = (d_RGB_y_low[x-1] + d_RGB_y_low[x+1] + d_RGB_y_high[x-1] + d_RGB_y_high[x+1]) / 4;

		//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
		Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
		//Cr= (uint8_t)(0.713*(r-Y) + 128);
		Cr= (mul16x16s(r-Y,coefCr)+128);

		*out++=Cr;
		*out++=Y;
		//for( x=1; x < dim_x - 1; x+=2 )
		x= window_width;
		do
		{

				/* red */
			r= *center++;							  //r =  d_RGB_y[x];
			g= (*low++ + *left++ + *right++ + *high++)>>2;			  //g = (d_RGB_y_low[x]   + d_RGB_y[x-1]     + d_RGB_y[x+1]      + d_RGB_y_high[x]  ) / 4;
			b= (*low_left++ + *low_right++ + *high_left++ + *high_right++)>>2; //b = (d_RGB_y_low[x-1] + d_RGB_y_low[x+1] + d_RGB_y_high[x-1] + d_RGB_y_high[x+1]) / 4;

			//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
			Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
			//Cr= (uint8_t)(0.713*(r-Y) + 128);
			Cr= (mul16x16s(r-Y,coefCr)+128);

			*out++=Cr;
			*out++=Y;

				/* green 1 */
			r = (*left++ + *right++)>>1; 	//(d_RGB_y[x-1] + d_RGB_y[x+1]) / 2;
			g =  *center++;			//d_RGB_y[x];
			b = (*low++ + *high++)>>1;	//(d_RGB_y_low[x] + d_RGB_y_high[x]) / 2;
			low_left++;low_right++;high_left++;high_right++;
			//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
			Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
			//Cb= (uint8_t)(0.564*(b-Y) + 128);
			Cb= (mul16x16s(b-Y,coefCb)+128);
			Cr= (mul16x16s(r-Y,coefCr)+128);

			*out++=Cb;
			*out++=Y;

		}
		while(x-=2);
		/* copy data on edge */
		*out++=Cr;	//dst[j+1] = Y;		// Luma
		*out++=Y;	//dst[j]   = CbCr[CbCr_idx];		// Cb or Cr
				//j += 2;

		left+=(2+remain_size);center+=(2+remain_size);right+=(2+remain_size);
		low+=(2+remain_size);high+=(2+remain_size);
		low_left+=(2+remain_size);low_right+=(2+remain_size);high_left+=(2+remain_size);high_right+=(2+remain_size);

		 /* green 2 */
		r= (*low + *high)>>1; //r = (d_RGB_y_low[x] + d_RGB_y_high[x]) / 2;
		g =  *center;		//g=d_RGB_y[x];
		b= (*left + *right) >>1; //b = (d_RGB_y[x-1] + d_RGB_y[x+1]) / 2;

		//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
		Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
		//Cr= (uint8_t)(0.713*(r-Y) + 128);
		Cr= (mul16x16s(r-Y,coefCr)+128);

		/* copy data on edge */

		*out++=Cr;			//dst[j]   = CbCr[CbCr_idx];		// Cb or Cr
		*out++=Y;			//dst[j+1] = Y;		// Luma
							//j += 2;
		//for( x=1; x < dim_x - 1; x+=2 )
		x= window_width;
		do
		{

			/* green 2 */
			r= (*low++ + *high++)>>1; //r = (d_RGB_y_low[x] + d_RGB_y_high[x]) / 2;
			g =  *center++;		//g=d_RGB_y[x];
			b= (*left++ + *right++) >>1; //b = (d_RGB_y[x-1] + d_RGB_y[x+1]) / 2;
			low_left++;low_right++;high_left++;high_right++;

			//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
			Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
			//Cr= (uint8_t)(0.713*(r-Y) + 128);
			Cr= (mul16x16s(r-Y,coefCr)+128);

			*out++=Cr;
			*out++=Y;
			/* blue */
			r= (*low_left++ + *low_right++ + *high_left++ + *high_right++)>>2; //r = (d_RGB_y_low[x-1] + d_RGB_y_low[x+1] + d_RGB_y_high[x-1] + d_RGB_y_high[x+1]) / 4;
			g= (*low++ + *left++ + *right++ + *high++)>>2;			//g = (d_RGB_y_low[x]   + d_RGB_y[x-1]     + d_RGB_y[x+1]      + d_RGB_y_high[x]  ) / 4;
			b =  *center++;							//d_RGB_y[x];

			//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
			Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
			//Cb= (uint8_t)(0.564*(b-Y) + 128);
			Cb= (mul16x16s(b-Y,coefCb)+128);
			Cr= (mul16x16s(r-Y,coefCr)+128);

			*out++=Cb;
			*out++=Y;
		}
		while(x-=2);
		/* copy data on edge */
		*out++=Cr;	//dst[j+1] = Y;		// Luma
		*out++=Y;	//dst[j]   = CbCr[CbCr_idx];		// Cb or Cr
				//j += 2;

		left+=(2+remain_size);center+=(2+remain_size);right+=(2+remain_size);
		low+=(2+remain_size);high+=(2+remain_size);
		low_left+=(2+remain_size);low_right+=(2+remain_size);high_left+=(2+remain_size);high_right+=(2+remain_size);

	}
	while(y-=2);
}

void debayerRGB_fast_mode2(uint8_t *dst, uint8_t *src, int dim_x, int dim_y,int startx,int starty,int ww,int wh)
{
	int x,y;
	int r,g,b;
	int window_width,window_height;
	int remain_size;
	if(ww && wh)   // working with window in image
	{
	    window_width= ww-2;
	    window_height= wh-2;
	    remain_size= dim_x-ww;
	    src+=(dim_x*starty+startx);
	}
	else
	{
	    window_width= dim_x-2;
	    window_height= dim_y-2;
	    remain_size=0;
	}
	uint8_t *high_left=src,*high=high_left+1,*high_right=high+1;
	uint8_t *left=src+dim_x,*center=left+1,*right=center+1;
	uint8_t *low_left=left+dim_x,*low=low_left+1,*low_right=low+1;
	uint16_t Y,Cr,Cb;
	uint8_t *out=dst;

			//for( y=1; y < dim_y - 1; y+=2 )
			y= window_height;
			do
			{
				/* blue */
		    		r= (*low_left + *low_right + *high_left + *high_right)>>2; //r = (d_RGB_y_low[x-1] + d_RGB_y_low[x+1] + d_RGB_y_high[x-1] + d_RGB_y_high[x+1]) / 4;
		    		g= (*low + *left + *right + *high)>>2;			//g = (d_RGB_y_low[x]   + d_RGB_y[x-1]     + d_RGB_y[x+1]      + d_RGB_y_high[x]  ) / 4;
		    		b =  *center;							//d_RGB_y[x];

				//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
				Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
				//Cr= (uint8_t)(0.713*(r-Y) + 128);
				Cr= (mul16x16s(r-Y,coefCr)+128);

				/* copy data on edge */

				*out++=Cr;			//dst[j]   = CbCr[CbCr_idx];		// Cb or Cr
				*out++=Y;			//dst[j+1] = Y;		// Luma
									//j += 2;
				//for( x=1; x < dim_x - 1; x+=2 )
				x= window_width;
				do
				{
					/* blue */
		    			r= (*low_left++ + *low_right++ + *high_left++ + *high_right++)>>2; //r = (d_RGB_y_low[x-1] + d_RGB_y_low[x+1] + d_RGB_y_high[x-1] + d_RGB_y_high[x+1]) / 4;
		    			g= (*low++ + *left++ + *right++ + *high++)>>2;			//g = (d_RGB_y_low[x]   + d_RGB_y[x-1]     + d_RGB_y[x+1]      + d_RGB_y_high[x]  ) / 4;
		    			b =  *center++;							//d_RGB_y[x];

					//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
					Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
					//Cr= (uint8_t)(0.713*(r-Y) + 128);
					Cr= (mul16x16s(r-Y,coefCr)+128);


					*out++=Cr;
					*out++=Y;

		  		    /* green 2 */
		    			r= (*low++ + *high++)>>1; //r = (d_RGB_y_low[x] + d_RGB_y_high[x]) / 2;
		    			g =  *center++;		//g=d_RGB_y[x];
		    			b= (*left++ + *right++) >>1; //b = (d_RGB_y[x-1] + d_RGB_y[x+1]) / 2;
					low_left++;low_right++;high_left++;high_right++;

					//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
					Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
					//Cb= (uint8_t)(0.564*(b-Y) + 128);
					Cb= (mul16x16s(b-Y,coefCb)+128);
					Cr= (mul16x16s(r-Y,coefCr)+128);

					*out++=Cb;
					*out++=Y;
				}
				while(x-=2);
				/* copy data on edge */
				*out++=Cr;	//dst[j+1] = Y;		// Luma
				*out++=Y;	//dst[j]   = CbCr[CbCr_idx];		// Cb or Cr
						//j += 2;

				left+=(2+remain_size);center+=(2+remain_size);right+=(2+remain_size);
				low+=(2+remain_size);high+=(2+remain_size);
				low_left+=(2+remain_size);low_right+=(2+remain_size);high_left+=(2+remain_size);high_right+=(2+remain_size);
				/* green 1 */
				r = (*left + *right)>>1; 	//(d_RGB_y[x-1] + d_RGB_y[x+1]) / 2;
		    		g =  *center;			//d_RGB_y[x];
		    		b = (*low + *high)>>1;	//(d_RGB_y_low[x] + d_RGB_y_high[x]) / 2;

				//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
				Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
				//Cr= (uint8_t)(0.713*(r-Y) + 128);
				Cr= (mul16x16s(r-Y,coefCr)+128);
									/* copy data on edge */
				*out++=Cr;			//dst[j]   = CbCr[CbCr_idx];		// Cb or Cr
				*out++=Y;			//dst[j+1] = Y;		// Luma
								//j += 2;
				//for( x=1; x < dim_x - 1; x+=2 )
				x= window_width;
				do
				{
						/* green 1 */
					r = (*left++ + *right++)>>1; 	//(d_RGB_y[x-1] + d_RGB_y[x+1]) / 2;
		    			g =  *center++;			//d_RGB_y[x];
		    			b = (*low++ + *high++)>>1;	//(d_RGB_y_low[x] + d_RGB_y_high[x]) / 2;
					low_left++;low_right++;high_left++;high_right++;

					//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
					Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
					//Cr= (uint8_t)(0.713*(r-Y) + 128);
					Cr= (mul16x16s(r-Y,coefCr)+128);

					*out++=Cr;
					*out++=Y;
		    				/* red */
		    			r= *center++;							  //r =  d_RGB_y[x];
		    			g= (*low++ + *left++ + *right++ + *high++)>>2;			  //g = (d_RGB_y_low[x]   + d_RGB_y[x-1]     + d_RGB_y[x+1]      + d_RGB_y_high[x]  ) / 4;
		    			b= (*low_left++ + *low_right++ + *high_left++ + *high_right++)>>2; //b = (d_RGB_y_low[x-1] + d_RGB_y_low[x+1] + d_RGB_y_high[x-1] + d_RGB_y_high[x+1]) / 4;

					//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
					Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
					//Cb= (uint8_t)(0.564*(b-Y) + 128);
					Cb= (mul16x16s(b-Y,coefCb)+128);
					Cr= (mul16x16s(r-Y,coefCr)+128);

					*out++=Cb;
					*out++=Y;

				}
				while(x-=2);
				/* copy data on edge */
				*out++=Cr;	//dst[j+1] = Y;		// Luma
				*out++=Y;	//dst[j]   = CbCr[CbCr_idx];		// Cb or Cr
						//j += 2;

				left+=(2+remain_size);center+=(2+remain_size);right+=(2+remain_size);
				low+=(2+remain_size);high+=(2+remain_size);
				low_left+=(2+remain_size);low_right+=(2+remain_size);high_left+=(2+remain_size);high_right+=(2+remain_size);

			}
			while(y-=2);
}

void debayerRGB_fast_mode3(uint8_t *dst, uint8_t *src, int dim_x, int dim_y,int startx,int starty,int ww,int wh)
{
	int x,y;
	int r,g,b;
	int window_width,window_height;
	int remain_size;
	if(ww && wh)   // working with window in image
	{
	    window_width= ww-2;
	    window_height= wh-2;
	    remain_size= dim_x-ww;
	    src+=(dim_x*starty+startx);
	}
	else
	{
	    window_width= dim_x-2;
	    window_height= dim_y-2;
	    remain_size=0;
	}
	uint8_t *high_left=src,*high=high_left+1,*high_right=high+1;
	uint8_t *left=src+dim_x,*center=left+1,*right=center+1;
	uint8_t *low_left=left+dim_x,*low=low_left+1,*low_right=low+1;
	uint16_t Y,Cr,Cb;
	uint8_t *out=dst;

			//for( y=1; y < dim_y - 1; y+=2 )
			y= window_height;
			do
			{
		  		 /* green 2 */
		    		r= (*low + *high)>>1; //r = (d_RGB_y_low[x] + d_RGB_y_high[x]) / 2;
		    		g =  *center;		//g=d_RGB_y[x];
		    		b= (*left + *right) >>1; //b = (d_RGB_y[x-1] + d_RGB_y[x+1]) / 2;

				//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
				Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
				//Cr= (uint8_t)(0.713*(r-Y) + 128);
				Cr= (mul16x16s(r-Y,coefCr)+128);

				/* copy data on edge */

				*out++=Cr;			//dst[j]   = CbCr[CbCr_idx];		// Cb or Cr
				*out++=Y;			//dst[j+1] = Y;		// Luma
									//j += 2;
				//for( x=1; x < dim_x - 1; x+=2 )
				x= window_width;
				do
				{

		  		    	/* green 2 */
		    			r= (*low++ + *high++)>>1; //r = (d_RGB_y_low[x] + d_RGB_y_high[x]) / 2;
		    			g =  *center++;		//g=d_RGB_y[x];
		    			b= (*left++ + *right++) >>1; //b = (d_RGB_y[x-1] + d_RGB_y[x+1]) / 2;
					low_left++;low_right++;high_left++;high_right++;

					//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
					Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
					//Cr= (uint8_t)(0.713*(r-Y) + 128);
					Cr= (mul16x16s(r-Y,coefCr)+128);

					*out++=Cr;
					*out++=Y;
					/* blue */
		    			r= (*low_left++ + *low_right++ + *high_left++ + *high_right++)>>2; //r = (d_RGB_y_low[x-1] + d_RGB_y_low[x+1] + d_RGB_y_high[x-1] + d_RGB_y_high[x+1]) / 4;
		    			g= (*low++ + *left++ + *right++ + *high++)>>2;			//g = (d_RGB_y_low[x]   + d_RGB_y[x-1]     + d_RGB_y[x+1]      + d_RGB_y_high[x]  ) / 4;
		    			b =  *center++;							//d_RGB_y[x];

					//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
					Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
					//Cb= (uint8_t)(0.564*(b-Y) + 128);
					Cb= (mul16x16s(b-Y,coefCb)+128);
					Cr= (mul16x16s(r-Y,coefCr)+128);

					*out++=Cb;
					*out++=Y;
				}
				while(x-=2);
				/* copy data on edge */
				*out++=Cr;	//dst[j+1] = Y;		// Luma
				*out++=Y;	//dst[j]   = CbCr[CbCr_idx];		// Cb or Cr
						//j += 2;

				left+=(2+remain_size);center+=(2+remain_size);right+=(2+remain_size);
				low+=(2+remain_size);high+=(2+remain_size);
				low_left+=(2+remain_size);low_right+=(2+remain_size);high_left+=(2+remain_size);high_right+=(2+remain_size);
				/* red */
		    		r= *center;							  //r =  d_RGB_y[x];
		    		g= (*low + *left + *right + *high)>>2;			  //g = (d_RGB_y_low[x]   + d_RGB_y[x-1]     + d_RGB_y[x+1]      + d_RGB_y_high[x]  ) / 4;
		    		b= (*low_left + *low_right + *high_left + *high_right)>>2; //b = (d_RGB_y_low[x-1] + d_RGB_y_low[x+1] + d_RGB_y_high[x-1] + d_RGB_y_high[x+1]) / 4;

				//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
				Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
				//Cr= (uint8_t)(0.713*(r-Y) + 128);
				Cr= (mul16x16s(r-Y,coefCr)+128);

				*out++=Cr;
				*out++=Y;
				//for( x=1; x < dim_x - 1; x+=2 )
				x= window_width;
				do
				{

		    				/* red */
		    			r= *center++;							  //r =  d_RGB_y[x];
		    			g= (*low++ + *left++ + *right++ + *high++)>>2;			  //g = (d_RGB_y_low[x]   + d_RGB_y[x-1]     + d_RGB_y[x+1]      + d_RGB_y_high[x]  ) / 4;
		    			b= (*low_left++ + *low_right++ + *high_left++ + *high_right++)>>2; //b = (d_RGB_y_low[x-1] + d_RGB_y_low[x+1] + d_RGB_y_high[x-1] + d_RGB_y_high[x+1]) / 4;

					//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
					Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
					//Cr= (uint8_t)(0.713*(r-Y) + 128);
					Cr= (mul16x16s(r-Y,coefCr)+128);

					*out++=Cr;
					*out++=Y;

						/* green 1 */
					r = (*left++ + *right++)>>1; 	//(d_RGB_y[x-1] + d_RGB_y[x+1]) / 2;
		    			g =  *center++;			//d_RGB_y[x];
		    			b = (*low++ + *high++)>>1;	//(d_RGB_y_low[x] + d_RGB_y_high[x]) / 2;
					low_left++;low_right++;high_left++;high_right++;
					//Y= (uint8_t)(0.299*r + 0.587*g + 0.114*b);
					Y= (mul16x16s(r,coefrY)+mul16x16s(g,coefgY)+mul16x16s(b,coefbY));
					//Cb= (uint8_t)(0.564*(b-Y) + 128);
					Cb= (mul16x16s(b-Y,coefCb)+128);
					Cr= (mul16x16s(r-Y,coefCr)+128);

					*out++=Cb;
					*out++=Y;

				}
				while(x-=2);
				/* copy data on edge */
				*out++=Cr;	//dst[j+1] = Y;		// Luma
				*out++=Y;	//dst[j]   = CbCr[CbCr_idx];		// Cb or Cr
						//j += 2;

				left+=(2+remain_size);center+=(2+remain_size);right+=(2+remain_size);
				low+=(2+remain_size);high+=(2+remain_size);
				low_left+=(2+remain_size);low_right+=(2+remain_size);high_left+=(2+remain_size);high_right+=(2+remain_size);

			}
			while(y-=2);
}

static void BWto422(uint8_t *dst, uint8_t *src, int dim_x, int dim_y, int startx, int starty, int ww,int wh)
{

	/* and convert BW to YCbCr */
	int x,y,pos = 0;
	for (y=starty;y<wh+starty;y++)
		for (x=startx;x<ww+startx;x++)
		{
			dst[2*pos] = 0x80;
			dst[2*pos+1] = src[x+dim_x*y];
			pos++;
		} /*
	while(todo>0) {
	        dst[0] = 0x80;			// Chroma
	        dst[1] = src[0];		// Luma

	        dst[2] = 0x80;			// Chroma
	        dst[3] = src[1];		// Luma

		dst+=4;
		src+=2;

		todo-=2;
	}*/
}

/************* bayer RGB => YCbCr 4:2:2 *****************/
/* mode(left most):	0 - g1 				*/
/*			1 - r				*/
/*			2 - b				*/
/*			3 - g2				*/
/********************************************************/
// dim_x and dim_y MUST BE EVEN and greater or equal 2

static void debayerRGB_fast(uint8_t *dst, uint8_t *src, int dim_x, int dim_y, int mode, cam4_rd_t *ctx,int sse2_present,int mmx_present,int startx,int starty,int ww,int wh)
{
	if(!dim_x || !dim_y || !ww || !wh) {
		TRACE(0, "Empty DIMS: dim: (x:%d y:%d) w:%d h:%d", dim_x, dim_y, ww, wh);
		return;
	}

	if(mode < 0)
		mode = ctx->fdata_h[ctx->idx].flags;
#if 1
	if(arch_probe_fast_debayer(&ctx->d_api, dim_x, startx, ww) <= 0)
		ctx->d_api = default_debayer_api;
#else
	ctx->d_api = default_debayer_api;
#endif
	switch(mode & 0x1f) {
		case 0:
			ctx->d_api.debayerRGB_func[0](dst, src, dim_x, dim_y,startx,starty,ww,wh);
			break;
		case 1:
			ctx->d_api.debayerRGB_func[1](dst, src, dim_x, dim_y,startx,starty,ww,wh);
			break;
		case 2:
			ctx->d_api.debayerRGB_func[2](dst, src, dim_x, dim_y,startx,starty,ww,wh);
			break;
		case 3:
			ctx->d_api.debayerRGB_func[3](dst, src, dim_x, dim_y,startx,starty,ww,wh);
			break;
		case 4:
			BWto422(dst, src, dim_x, dim_y,startx,starty,ww,wh);
			break;

		default:
       			TRACE(0, "[err] Unknown encoding mode, allow 0, 1, 2, 3:%02x\n", mode);
       			exit(-1);

	}
}

#if 0
static void debayerRGB(uint8_t *dst, uint8_t *src, int dim_x, int dim_y, uint8_t mode, void *priv)
{
    int 	r, g, b;
    int		x, y;
    int		j = 0;
    uint8_t	xxor, yxor, xx, yy;
    //cam4_rd_t	*cam4_rd = priv ;

#if DEBUG_ME == 1
	/* save in r g b files*/
	int fd_r = open("component_r.raw", O_CREAT | O_WRONLY, W_OK|R_OK);
	int fd_g = open("component_g.raw", O_CREAT | O_WRONLY, W_OK|R_OK);
	int fd_b = open("component_b.raw", O_CREAT | O_WRONLY, W_OK|R_OK);
#endif

    /*                    lines :				*/
    /* d_RGB_y_low  ----------------------------------------	*/
    /* d_RGB_y      ----------------------------------------	*/
    /* d_RGB_y_high ----------------------------------------	*/
    uint8_t	*d_RGB_y, *d_RGB_y_low, *d_RGB_y_high ;

    /*			RGB => YCbCr			*/
    uint8_t	Y = 0, CbCr[2] = {};
    uint8_t	CbCr_idx = 1;


    switch(mode) {
    case 0:
	xxor = yxor = 0;
	break;
    case 1:
	xxor = 1;
	yxor = 0;
	break;
    case 2:
	xxor = 0;
	yxor = 1;
	break;
    case 3:
	xxor = yxor = 1;
	break;
    default:
       TRACE(0, "[err] Unknown encoding mode, allow 0, 1, 2, 3 \n");
       exit(-1);
    }

    /* calculate RGB and convert to YCbCr */
    for( y=1; y < dim_y - 1; y++ ) {

	d_RGB_y_low = src + (y - 1) * dim_x ;
	d_RGB_y     = src +  y      * dim_x ;
	d_RGB_y_high= src + (y + 1) * dim_x ;

	for( x=1; x < dim_x - 1; x++ ) {
	    xx = xxor ^ (x & 1) ;
	    yy = yxor ^ (y & 1) ;

TRACE(255, "xx = %d yy = %d\n", xx, yy);
TRACE(255, 	"%03d %03d %03d\n"
		"%03d %03d %03d\n"
		"%03d %03d %03d\n",
		d_RGB_y_low[x-1], d_RGB_y_low[x], d_RGB_y_low[x+1],
		d_RGB_y[x-1], d_RGB_y[x], d_RGB_y[x+1],
		d_RGB_y_high[x-1], d_RGB_y_high[x], d_RGB_y_high[x+1]
);

	    if( xx == 1 ) {
		if( yy == 1 ) {
//TRACE_LINE();
		    /* green 1 */
		    r = (d_RGB_y[x-1] + d_RGB_y[x+1]) / 2;
		    g =  d_RGB_y[x];
		    b = (d_RGB_y_low[x] + d_RGB_y_high[x]) / 2;
		} else {
//TRACE_LINE();
		    /* blue */
		    r = (d_RGB_y_low[x-1] + d_RGB_y_low[x+1] + d_RGB_y_high[x-1] + d_RGB_y_high[x+1]) / 4;
		    g = (d_RGB_y_low[x]   + d_RGB_y[x-1]     + d_RGB_y[x+1]      + d_RGB_y_high[x]  ) / 4;
		    b =  d_RGB_y[x];
		}

	    } else {
		if( yy == 1 ) {
//TRACE_LINE();
		    /* red */
		    r =  d_RGB_y[x];
		    g = (d_RGB_y_low[x]   + d_RGB_y[x-1]     + d_RGB_y[x+1]      + d_RGB_y_high[x]  ) / 4;
		    b = (d_RGB_y_low[x-1] + d_RGB_y_low[x+1] + d_RGB_y_high[x-1] + d_RGB_y_high[x+1]) / 4;
		} else {
//TRACE_LINE();
		    /* green 2 */
		    r = (d_RGB_y_low[x] + d_RGB_y_high[x]) / 2;
		    g =  d_RGB_y[x];
		    b = (d_RGB_y[x-1] + d_RGB_y[x+1]) / 2;
		}
	    }

	    //TRACEPNF(3, "Debug [%04d,%04d] =>\tR:%03d\tG:%03d\tB:%03d\n", x, y, r, g, b) ;

#if DEBUG_ME == 1
	/* save in r g b files*/
	write(fd_r, &r, 1);
	write(fd_g, &g, 1);
	write(fd_b, &b, 1);
#endif


	    Y       = (uint8_t)(0.299*r + 0.587*g + 0.114*b);
	    CbCr[0] = (uint8_t)(0.564*(b-Y) + 128);			// Cb
	    CbCr[1] = (uint8_t)(0.713*(r-Y) + 128);			// Cr

#if 1
	    if( x == 1 ) {
		/* copy data on edge */
		dst[j+1] = Y;		// Luma
		dst[j]   = CbCr[CbCr_idx];		// Cb or Cr
		j += 2;
	    }
#endif

 	    dst[j+1] = Y;		// Luma
	    //dst[j]   = 0x80;		// Cb or Cr
	    dst[j]   = CbCr[CbCr_idx];	// Cb or Cr
	    j += 2;

	    CbCr_idx ^= 1;		// Y Cb Y Cr Y Cb Y Cr = 4:2:2

#if 1
	    if( x == (dim_x-2) ) {
		/* copy data on edge */
		dst[j+1] = Y;		// Luma
		dst[j]   = CbCr[CbCr_idx];		// Cb or Cr
		j += 2;
	    }
#endif


	} /* for( x=1; x<dim_x-1; x++ ) */

    } /* for( y=1; y < dim_y-1; y++ ) */

#if DEBUG_ME == 1
   exit(-1);
#endif

}
#endif

/* not reenterable */
int cam4_dump_raw_frame(
	cam4_rd_t	*ctx,
	uint8_t		*src,
	size_t		todo
)
{
	int		res;
	char		name[255];
	int		fd;

	size_t		done = 0 ;
	static unsigned	idx		= 0;
	static uint32_t	fseq_old	= 0;
	uint32_t	fseq = ctx->FH.fseq;

	if(fseq - fseq_old > 100000)
		idx++;

	fseq_old = fseq;

	//frame_00_x_1376_y_1024_bitdep_12_bitform_16_expo_110_mode_dark.raw

	if (strlen(ctx->path) != 0)
		snprintf(name, sizeof(name), "%s/frame_%02u_x_%04u_y_%04u_bitdep_%02u_bitform_%02u_expo_%u_%s_%s.raw",
			ctx->path,
			ctx->img_num,
			ctx->w,
			ctx->h,
			ctx->bits,
			ctx->bits,
			ctx->expo/10,
			ctx->mode,
			ctx->pair
			//idx,
			//(unsigned long)fseq

		);
	else
		snprintf(name, sizeof(name), "frame_%05ux%05ux%02u_%03d_%08lx.raw",
			ctx->w,
			ctx->h,
			ctx->bits,
			idx,
			(unsigned long)fseq
		);




	fd = open(name, O_CREAT | O_WRONLY, 0666);
	if(fd < 0) {
		ETRACE("[err] cannot open file [%s]. errno ", name);
		return -errno;
	}

	do {
	    res = write(fd, src + done, todo);

	    if(res < 0 ) {
		    if( errno == EINTR ) {
			TRACEP(0, "warn. signal occur \n");
			continue ;
		    }

		    ETRACEP("[%s]:%d [err] cannot write data. errno:%d size:%zu",
		    	__func__, __LINE__, errno, todo);

		    return -errno;
	    }

	    todo -= res ;
	    done += res ;
	} while(todo);

	TRACEPNF(0, "DUMPED: %zu@%s\n", done, name);

	close(fd);

	return 0;
}

int cam4_dump_YCbCr_frame(
	cam4_rd_t	*ctx,
	uint8_t		*src,
	size_t		todo
)
{
	int		res;
	char		name[255];
	int		fd;

	size_t		done = 0 ;
	static unsigned	idx	= 0;

	snprintf(name, sizeof(name), "frame_YCbCr_%05ux%05ux%02u_%05d.yuv",
	    ctx->w,
	    ctx->h,
	    ctx->bits,

	    idx
	);

	fd = open(name, O_CREAT | O_WRONLY, 0666);
	if(fd < 0) {
		ETRACE("[err] cannot open file [%s]. errno ", name);
		return -errno;
	}

	do {
	    res = write(fd, src + done, todo);

	    if(res < 0 ) {
		    if( errno == EINTR ) {
			TRACEP(0, "warn. signal occur \n");
			continue ;
		    }

		    ETRACEP("[%s]:%d [err] cannot write data. errno:%d size:%zu",
		    	__func__, __LINE__, errno, todo);

		    return -errno;
	    }

	    todo -= res ;
	    done += res ;
	} while(todo);

	TRACEPNF(0, "DUMPED: %zu@%s\n", done, name);

	idx++;
	close(fd);

	return 0;
}


static void raw_off(cam4_rd_t *cam4_rd, mcast_cl_interface_t *ifs);

typedef struct Yuv_Image_s{
	uint8_t* data;
	uint32_t width;
	uint32_t height;
	uint32_t data_size;
}Yuv_Image;

int open_fifo(char* path,int mode) {
	umask(0);
	mknod(path,S_IFIFO|0666, 0);
	int fd = open(path, mode | O_NONBLOCK);
	return fd;

}

static int sensor_read_oob_hist(void *priv, uint8_t *data, size_t size, size_t tail) {
	abi_mem_flow_t* f = priv;
	cam4_rd_t* cam4_rd = f->ctx;
	common_t* c = cam4_rd->common;
	if (c == NULL) return 0;
	int nbins = tail/4;
	c->nbins = nbins;
	uint32_t* hist = f->data + f->offs;
	c->hist_be = hist;
	int i;
	for (i=0;i<nbins;i++) {

		c->hist[i] = ntohl(hist[i]);
	}
	f->offs += size+tail;
	return 0;
}

static int sensor_read_oob_state(void *priv, uint8_t *data, size_t size, size_t tail) {
	abi_mem_flow_t* f = priv;
#if 0
	cam4_rd_t* cam4_rd = f->ctx;
	CAMCTRL_StateData *d = f->data + f->offs;
	cam4_rd->state = *d;

	common_t* common = cam4_rd->common;
	common->reg1 = (d->StatData.SunAzimuthDeg);
	common->reg2 = (d->StatData.SunElevationDeg);
#endif
	f->offs += size+tail;
	return 0;
}

static int events_read(void *priv, uint8_t *data, size_t size, size_t tail) {
	abi_mem_flow_t		*f	 = priv;
	p3_tevent_str_idx_t	*gps	 = f->data + f->offs;
	p3_tevent_acc_t		*acc	 = f->data + f->offs;

	switch(ntohl(gps->type)) {
	    case P3_TEV_IMEA_GP:
		TRACE(0, "ft:%016"PRIx64" ms:%4u V:%x GPS[%3d]:%s\n",
			filetime_to_host(&gps->ts),
			ntohs(gps->idx) & 0x7fff,
			ntohs(gps->idx)>>15,
			ntohs(gps->len), gps->str
		);
		break;

	    case P3_TEV_ACC_3D:
		if(acc->valid)
			TRACE(0, "ft:%016"PRIx64" valid:%04x X:%+5d Y:%+5d Z:%+5d\n",
				filetime_to_host(&acc->ts),
				ntohs(acc->valid),
				(int16_t)((ntohs(acc->data[0]))<<4) >> 4,
				(int16_t)((ntohs(acc->data[1]))<<4) >> 4,
				(int16_t)((ntohs(acc->data[2]))<<4) >> 4
			);
		break;

	    default:
		TRACE(0, "ft:%016"PRIx64" TEV type:%08"PRIx32"\n",
			filetime_to_host(&acc->ts),
			ntohl(acc->type)
		);
	}

	f->offs += size+tail;

	return 0;
}


static int sensor_read_oob_bars(void *priv, uint8_t *data, size_t size, size_t tail) {
	abi_mem_flow_t		*f		= priv;
	cam4_rd_t		*cam4_rd	= f->ctx;
	common_t		*c		= cam4_rd->common;
	p3_2d_id_bars_t		*src		= f->data + f->offs;
	p3_2d_id_bars_t		*dst;

	f->offs += size+tail;

	if (c == NULL)
		return 0;

	if (src->type != P3_2D_BAR_FACE_ID_ROI)
		return 0;

	dst = (void*)shmaddr[c->frame_idx_done&1] + c->image_size;

	uint16_t items = htons(src->items);

	if(items > 35) {
		dst->items	= 0;
		TRACEP(0, "Invalid BARS count: %d\n", items);
		return 0;
	}

	dst->items	= items;
	dst->flags	= src->flags;

	int i;

	p3_2d_id_bar_t	*src_bar = src->bars;
	p3_2d_id_bar_t	*dst_bar = dst->bars;

	for (i=0; i<items; i++, src_bar++, dst_bar++) {
		dst_bar->x0	= htons(src_bar->x0);
		dst_bar->y0	= htons(src_bar->y0);
		dst_bar->x1	= htons(src_bar->x1);
		dst_bar->y1	= htons(src_bar->y1);

		dst_bar->id	= htonl(src_bar->id);
		dst_bar->state	= htonl(src_bar->state);
	}
	return 0;
}

void parse_oob_data(void* priv, uint8_t* data, uint32_t size) {
	//dump_hex(data, size);
	uint32_t	osize = ntohl(((uint32_t*)data)[0]) +4;

	if(size != osize) {
		TRACEPNF(0, "sz: %08"PRIx32" psz: %08"PRIx32"\n", size, osize);

		return;
	}

	cam4_rd_t		*cam4_rd	= priv;

	abi_mem_flow_t	flow = {
	    .data	= data,
	    .max_size	= size,
	    .offs	= 0,
	    .name	= "OOB",
	    .status	= 0,
	    .ctx	= cam4_rd,
	};

	static abi_context_t	*abi = NULL;
	abi = abir_open(abi, &flow,
	    abi_mem_reader,
	    abi_mem_skiper,
	    10
	);

	abi_node_t	tmp	= { };

	ABI_SET_KEYA(tmp, P3_ABI_OOB_DATA);
	int res = abi_append_node(abi, &tmp);

	ABI_SET_KEYA(tmp, P3_ABI_CAMCTRL_STATE_T);
	ABI_SET_READER(tmp, sensor_read_oob_state);
	abi_append_subnode(abi, res, &tmp);

	ABI_SET_KEYA(tmp, P3_ABI_CAMCTRL_LHIST_T);
	ABI_SET_READER(tmp, sensor_read_oob_hist);
	abi_append_node(abi, &tmp);

	ABI_SET_KEYA(tmp, P3_ABI_2D_BARS_T);
	ABI_SET_READER(tmp, sensor_read_oob_bars);
	abi_append_node(abi, &tmp);

	ABI_SET_KEYA(tmp, P3_TEVENT_T);
	ABI_SET_READER(tmp, events_read);
	abi_append_node(abi, &tmp);

	res = abir_read(abi,1);
	((uint32_t*)data)[0] = 0;	/* be a bit paranoidal */
}

/*
 * calculate components hists
 */

void calc_raw_hist(
	uint8_t		*img,
	common_t	*common
)
{
	int	x;
	int	y;
	uint8_t	val;

	memset(common->comp_hist, 0, sizeof(common->comp_hist));

	for (x=0; x<4; x++)
		common->comp_hist[x].nbins = 256;
	if(quad==0){
	for (x=0; x<common->sensWidth; x++)
		for (y=0;y<common->sensHeight;y++) {
			val  = img[x+y*common->sensWidth];

			common->comp_hist[(x&1) + 2*(y&1)].hist[val]++;
		}
	}else{ //use only one quadrand
	for (x=0; x<(common->sensWidth)/2; x++)
		for (y=0;y<(common->sensHeight)/2;y++) {
			val  = img[x+y*common->sensWidth];

			common->comp_hist[(x&1) + 2*(y&1)].hist[val]++;
		}

	}
}

void draw_camctl_stat(
	cam4_rd_t 	*rd,
	common_t	*common
)
{
	int	x;
	int	y;

	if(rd->flipped_img_size != common->sensWidth * common->sensHeight) {
		rd->flipped_img_size	= common->sensWidth * common->sensHeight;
		rd->flipped_img		= realloc(rd->flipped_img, rd->flipped_img_size);
	}

	for (y=0; y<common->sensHeight; y++)
		for (x=0; x<common->sensWidth; x++)
			rd->flipped_img[x+y*common->sensWidth] = rd->img[x+(common->sensHeight-y-1)*common->sensWidth];
#if 0
	int err = CAMCTRL_ParseAndDrawStateData(rd->flipped_img,
	    common->sensWidth, common->sensHeight,
	    common->sensWidth,
	    NULL,
	    NULL,
	    &(rd->state),
	    sizeof(CAMCTRL_StateData),
	    common->hist_be, common->nbins * sizeof(uint32_t),
	    NULL,
	    0,
	    3,
	    rd->camctl_mode
	);

	if(err)
	    ;
#endif
	for (y=0; y<common->sensHeight; y++)
		for (x=0; x<common->sensWidth; x++)
			rd->img[x+y*common->sensWidth] = rd->flipped_img[x+(common->sensHeight-y-1)*common->sensWidth];
}

// Расчет статистики по кадру.
// На текущий момент есть несущественная ошибка -
// для расчета стандартного отклонения используется предыдущее значение матожидания.
// В противовес достигается снижение числа итераций в два раза
void calc_mean_dev(
    uint16_t *img16,    // image base adress
    uint16_t sensWidth,
    uint16_t x0,        // dimensions
    uint16_t x1,
    uint16_t y0,
    uint16_t y1,
    uint32_t *mean_out, // для расчета девиации должен содержать предыдущее значение матожидания
    uint32_t *dev_out
    )
{
    unsigned long dev=0, mean=0, size=0;
    uint32_t x, y, v;

    for (y=y0; y<y1; y++)
        for (x=x0; x<x1; x+=2){
            if(y&0x1)   v = img16[x+1+y*sensWidth];
            else        v = img16[x+  y*sensWidth];

            mean += v;
            v   -= *mean_out;
            dev += v*v;
            size++;
        }

    *mean_out = (uint32_t)(mean/size);
    *dev_out  = (uint32_t)(dev/size);
}


static void* cam4_rd_process_real(void *priv)
{

	TRACE(0, "\n");
	int	mmx_present  = 0;
	int	sse2_present = 0;

	common_t	*common;
	cam4_rd_t 	*cam4_rd = priv ;
	Yuv_Image	yuv_image[2];

	int shmid = shmget(key_common, sizeof(common_t), IPC_CREAT | 0666);
	shmid1 = shmid;
	if(shmid <0) {
		ETRACE("Cant:shmget(key_common:%08x), %zd, ...)", key_common, sizeof(common_t));
		return NULL;
	}

	common = shmat(shmid, NULL, 0);

	if((intptr_t)common ==-1) {
		ETRACE("Cant:shmat(key_common:%08x), %zd, ...)", key_common, sizeof(common_t));
		return NULL;
	}

	cam4_rd->common = common;
	shmaddr3 = (uint8_t*)common;

	common->image_size = cam4_rd->FH.x_dim * cam4_rd->FH.y_dim * 2;
	common->data_size  = common->image_size + 2*4096;

	yuv_image[0].width	= cam4_rd->FH.x_dim;
	yuv_image[0].height	= cam4_rd->FH.y_dim;
	yuv_image[0].data_size  = common->image_size;

	shmid = shmget(key_yuv1, common->data_size, IPC_CREAT | 0666);
	shmid2 = shmid;
	if(shmid <0) {
		ETRACE("Cant:shmget(key_yuv1:%08x), %zd, ...)", key_yuv1, common->data_size);
		return NULL;
	}

	yuv_image[0].data = shmat(shmid,NULL,0);

	if((intptr_t)yuv_image[0].data ==-1) {
		ETRACE("Cant:shmat(key_yuv1:%08x), %zd, ...)", key_yuv1, common->data_size);
		return NULL;
	}
	shmaddr[0] = yuv_image[0].data;

	yuv_image[1].width	= cam4_rd->FH.x_dim;
	yuv_image[1].height	= cam4_rd->FH.y_dim;
	yuv_image[1].data_size  = common->image_size;
	shmid = shmget(key_yuv2, common->data_size, IPC_CREAT | 0666);
	shmid3 = shmid;
	if(shmid <0) {
		ETRACE("Cant:shmget(key_yuv2:%08x), %zd, ...)", key_yuv2, common->data_size);
		return NULL;
	}

	yuv_image[1].data = shmat(shmid,NULL,0);

	if((intptr_t)yuv_image[1].data ==-1) {
		ETRACE("Cant:shmat(key_yuv2:%08x), %zd, ...)", key_yuv2, common->data_size);
		return NULL;
	}
	TRACEPNF(0, "KEY1=%08x\n", key_common);
	TRACEPNF(0, "KEY2=%08x\n", key_yuv1);
	TRACEPNF(0, "KEY3=%08x\n", key_yuv2);

	shmaddr[1] = yuv_image[1].data;

	common->nbins		= 0;
	common->sensWidth	= yuv_image[0].width;
	common->sensHeight	= yuv_image[0].height;
	common->width		= common->sensWidth;
	common->height		= common->sensHeight;
	common->startx		= 0;
	common->starty		= 0;
	snprintf(common->window_title, sizeof(common->window_title) -1, "%s %s", cam4_rd->device_name, cam4_rd->camera_ip_str);
	common->window_title[sizeof(common->window_title) -1] = 0;

	TRACEP(0, "W:%4"PRIi32" H: %4"PRIi32" w:%4"PRIi32" h: %4"PRIi32"\n",
		common->sensWidth,
		common->sensHeight,
		yuv_image[0].width,
		yuv_image[0].height
	);

	//check_mmx_sse2(&sse2_present,&mmx_present);

	cam4_rd->start_mode |= frame_done_flag;
	int rc;
	uint16_t *img16;
	img16=(uint16_t*)malloc(common->image_size);

	while (no_sig_exit) {
		rc = sleep(3) ;
		if(!rc || !(errno == EINTR) )
			continue;

		int j = cam4_rd->idx ^ 1 ;
		cam4_rd->img = cam4_rd->buff_fd[j];

        cam4_script_processing(cam4_rd);

		if (cam4_rd->dumpraw_time.tv_sec != 0 && (cam4_rd->dumpraw_time.tv_sec < tv.tv_sec || (cam4_rd->dumpraw_time.tv_sec == tv.tv_sec && cam4_rd->dumpraw_time.tv_usec < tv.tv_usec))) {
			cam4_rd->dumpraw_time.tv_sec = 0;
			cam4_dump_raw_frame(cam4_rd, cam4_rd->img, cam4_rd->FH.fsize & 0xfffffff);
		}
		/* TEST OOB */

		if ((cam4_rd->FH.osize & 0xfffffff) > 0) {
			uint8_t* oob_data = cam4_rd->img + (cam4_rd->FH.fsize & 0xfffffff);
			parse_oob_data(cam4_rd, oob_data, cam4_rd->FH.osize & 0xfffffff);
		}

		common->frame_idx_done = j;

		cam4_rd_do_LUT(img16, cam4_rd->img, cam4_rd->FH.fsize);

		write_raw_video(cam4_rd, (uint8_t*)cam4_rd->img, cam4_rd->FH.fsize & 0xfffffff);

		calc_raw_hist(cam4_rd->img, common);

		//calculate components hists
		int x,y;

		// calculate mean and dev statistic - upper left
		unsigned long	size;
		int16_t		v, v1;


		calc_mean_dev(img16,common->sensWidth,64,                 common->sensWidth/2, 64,                  common->sensHeight/2, &(common->mean[0]),&(common->stddev[0]));
		calc_mean_dev(img16,common->sensWidth,common->sensWidth/2,common->sensWidth-64,64,                  common->sensHeight/2, &(common->mean[1]),&(common->stddev[1]));
		calc_mean_dev(img16,common->sensWidth,64,                 common->sensWidth/2, common->sensHeight/2,common->sensHeight-64,&(common->mean[2]),&(common->stddev[2]));
		calc_mean_dev(img16,common->sensWidth,common->sensWidth/2,common->sensWidth-64,common->sensHeight/2,common->sensHeight-64,&(common->mean[3]),&(common->stddev[3]));


		// calculate difference between quadrants (upper left) - (upper right)
		int32_t d=0;
		size =0;
		for (y=64;y<((common->sensHeight)/2);y++){
		    	v = img16[(common->sensWidth)/2-10+y*common->sensWidth];
		    	v1= img16[(common->sensWidth)/2+10+y*common->sensWidth];
			d += v1-v;
			size++;
			}
		common->diff[0] = d/((int32_t)size);
//		TRACEP(0, "R: diff = %d \n",common->diff[0]);

		// calculate difference between quadrants (lower left) - (lower right)
		d = 0;
		size =0;
		for (y=((common->sensHeight)/2);y<((common->sensHeight)-64);y++){
		    	v = img16[(common->sensWidth)/2-10+y*common->sensWidth];
		    	v1= img16[(common->sensWidth)/2+10+y*common->sensWidth];
			d += v1-v;
			size++;
			}
		common->diff[1] = d/((int32_t)size);


		// calculate difference between quadrants (upper left) - (lower left)
		d = 0;
		size =0;
		for (x=64;x<((common->sensWidth)/2);x++){
		    	v = img16[ x + ((common->sensHeight)/2 - 10)*common->sensWidth];
		    	v1= img16[ x + ((common->sensHeight)/2 + 10)*common->sensWidth];
			d += v1-v;
			size++;
			}
		common->diff[2] = d/((int32_t)size);

		// calculate difference between quadrants (upper right) - (lower right)
		d = 0;
		size =0;
		for (x=((common->sensWidth)/2);x<((common->sensWidth)-64);x++){
		    	v = img16[ x + ((common->sensHeight)/2 - 10)*common->sensWidth];
		    	v1= img16[ x + ((common->sensHeight)/2 + 10)*common->sensWidth];
			d += v1-v;
			size++;
			}
		common->diff[3] = d/((int32_t)size);

#ifdef CAM4_PS_LIB
		if(cam4_ps_cb)
			cam4_ps_cb(cam4_rd);
#endif

		//draw state data
		if(cam4_rd->camctl_mode)
			draw_camctl_stat(cam4_rd, common);

		debayerRGB_fast((uint8_t *)yuv_image[j].data,	// dst
			    cam4_rd->img,			// src
			    common->sensWidth,			// dim_x
			    common->sensHeight,			// dim_y
			    debayer_mode,
			    cam4_rd,
			    sse2_present,
			    mmx_present,
			    common->startx-common->startx%16,	// startx
			    common->starty-common->starty%16,	// starty
			    common->width,			// ww
			    common->height			// wh
		);

		common->frame_done = 1;
		gettimeofday(&tv,NULL);

		if (cam4_rd->dumpyuv_time.tv_sec != 0 && (cam4_rd->dumpyuv_time.tv_sec < tv.tv_sec || (cam4_rd->dumpyuv_time.tv_sec == tv.tv_sec && cam4_rd->dumpyuv_time.tv_usec < tv.tv_usec))) {
			cam4_rd->dumpyuv_time.tv_sec = 0;
			cam4_dump_YCbCr_frame(cam4_rd,(uint8_t *)yuv_image[j].data, common->width * common->height * 2);
		}
		write_video(cam4_rd, (uint8_t*)yuv_image[j].data, common->width * common->height * 2);
		cam4_rd->start_mode |= frame_done_flag;
	}

	free((void*)img16);

	if(cam4_rd->flipped_img)
		free(cam4_rd->flipped_img);

	return NULL;
}

static void *cam4_rd_process(void *priv)
{
	cam4_rd_process_real(priv);
	no_sig_exit = 0;

	return NULL;
}

static int write_frame(cam4_rd_t *cam4_rd)
{
    int		res;
    uint32_t	write_size = cam4_rd->FH.fsize & ~(0xf<<28);
    uint32_t	todo = write_size, done = 0 ;
    uint8_t	idx ;

    idx = cam4_rd->idx ^ 1 ;

TRACEPNF(0, "[line:%d] w:%d h:%d | todo = %d\n", __LINE__, cam4_rd->FH.y_dim, cam4_rd->FH.x_dim, todo);

    do {
    	res = write(cam4_rd->fd, cam4_rd->buff_fd[idx] + done, todo);

	if(res < 0 ) {

		if( errno == EINTR ) {
		    TRACEP(0, "warn. signal occur \n");
		    continue ;
		}

		ETRACEP("[%s]: [err] cannot write data. errno: ", __func__);
		TRACEP(0, "%d res = %d write_size = %d\n", __LINE__, res, todo);
		exit(-1) ;
	}
	todo -= res ;
	done += res ;

    } while(todo) ;

    cam4_rd->start_mode |= frame_done_flag;

    TRACEP(0, "[line:%d] res = %d write_size = %d done = %d\n", __LINE__, res, todo, done);
    return 0;
}

static int write_separate(cam4_rd_t *cam4_rd)
{
    char	frame_f[255] = {};
    int		res;

    sprintf(frame_f, "frame_%05d.raw", cam4_rd->frame_idx) ;

    cam4_rd->fd = open(frame_f, O_CREAT | O_WRONLY, 0660);
    if(cam4_rd->fd < 0) {
	ETRACE("[err] cannot open file [%s]. errno ", frame_f);
	exit(-1);
    }

    res = write_frame(cam4_rd);
    if( res < 0 ) {
	TRACE(0, "[err] cannot write [%s] file. \n", frame_f);
	exit(-1);
    }

    return 0;
}

void* cam4_rd_dump_buf(void *priv)
{
    cam4_rd_t 	*cam4_rd = priv ;
    int		rc ;

    cam4_rd->start_mode |= frame_done_flag;

    do {
	rc = sleep(3) ;

	if( (rc != 0) && (errno == EINTR) ) {

	    cam4_rd->write_cb(cam4_rd);
	    if( cam4_rd->frame_idx == (cam4_rd->frame_num -1) ) {
		TRACE(0, "[%d] frames succsessfully wrote \n", cam4_rd->frame_num);
		//exit(-1);
		break;
	    }

	    cam4_rd->frame_idx++;

	}

    } while(1) ;

    no_sig_exit = 0;

    return NULL;
}


static inline int cam4_rd_read_fh(void *ptr, cam4_rd_t *cam4_rd, unsigned len)
{
	static unsigned lag = 0;
	part_num++;
	video_frame_raw_hdr_t *FH  = &cam4_rd->FH;
	video_frame_raw_hdr_t *src = ptr;

	FH->lid   = ntohl(src->lid);
	FH->fseq  = ntohl(src->fseq);
	FH->gid   = src->gid;
	FH->ts    = filetime_to_host(&src->ts);
	FH->x_dim = ntohs(src->x_dim);
	FH->y_dim = ntohs(src->y_dim);
	FH->fsize = ntohl(src->fsize);

	if(len >= sizeof(video_frame_raw_hdr_t))
		FH->osize = ntohl(src->osize);
	else
		FH->osize = 0;

	if(offs_static != cam4_rd->fh_size) {
		TRACEPNF(0, "WARN: Tail missed:%08x@%08x [%08x]\n", cam4_rd->fh_size - offs_static, offs_static, cam4_rd->fh_size);
	}

	cam4_rd->fh_size	= (FH->fsize&0xfffffff) + (FH->osize&0xfffffff);
	cam4_rd->bits		= 8+2*(FH->fsize>>28);

	/* keep last frame data header */
	if(cam4_rd->pFD)
		memcpy(cam4_rd->fdata_h+cam4_rd->idx, cam4_rd->pFD, sizeof(*cam4_rd->pFD));
	else
		cam4_rd->fdata_h[cam4_rd->idx].flags = 0x4;

	TRACEPNF(0, "LID:%08x SEQ: %08x SZ:%8d@%016"PRIx64" %2dbit cm:%02x %5d x %5d oob:%5d lag:%d\n",
	    FH->lid,
	    FH->fseq,
	    cam4_rd->fh_size,
	    FH->ts,
	    cam4_rd->bits,
	    cam4_rd->fdata_h[cam4_rd->idx].flags,
	    FH->x_dim,
	    FH->y_dim,
	    (FH->osize&0xfffffff),
	    lag
	);

	if((FH->lid &0xff) == 0x09)
		part_num--;

	if(FH->fseq !=  part_num) {
	    TRACEPNF(0, "skipped real id = %08x, local id = %08x. Adjust....\n", FH->fseq, part_num) ;
	    part_num = FH->fseq ;
	}

	uint32_t todo = cam4_rd->fh_size;

	cam4_rd->done = 0;

	/* first FH - allocate space */
	if(cam4_rd->buff_fd[0] == NULL ) {
		cam4_rd->used_buf_space = todo * 16 / (8+2*(FH->fsize>>28));

		cam4_rd->buff_img   = malloc(todo * 8 / (8+2*(FH->fsize>>28)));
		if(posix_memalign((void **)&(cam4_rd->buff_fd[0]),16,cam4_rd->used_buf_space)
			|| posix_memalign((void **)&(cam4_rd->buff_fd[1]),16,cam4_rd->used_buf_space))
		{
		    ETRACEP("[%s] [err] cannot allocate space for frame. errno: ", __func__);
		    exit(-1);
		}

		TRACEPNF(0, "[%s] Allocated [%d] bytes\n", __func__, cam4_rd->used_buf_space);


		/* make signal handler */
		int rc;
		struct sigaction        actions = { };

		sigemptyset(&actions.sa_mask);
		actions.sa_flags = 0;
		actions.sa_handler = sighand;

		rc = sigaction(SIGRTMIN ,&actions, NULL);
		if( rc < 0 ) {
		    ETRACEP("Cannot create signal handler. errno ") ;
		    exit(-1) ;
		}
		TRACEPNF(0, "Activate signal handler \n");

		cam4_rd->idx = 0 ;
		int res;

		if( cam4_rd->start_mode & (f_flag | s_flag)) {

		    TRACE_LINE();

		    if( cam4_rd->start_mode & f_flag ) {
			/* need to prepare output file */
			cam4_rd->fd = open(cam4_rd->f_name, O_CREAT | O_WRONLY, W_OK|R_OK);
			if(cam4_rd->fd < 0) {
			    ETRACE("[err] cannot open file [%s]. errno ", cam4_rd->f_name);
			    exit(-1);
			}
		    }
		    CREATE_THREAD(res, cam4_rd_dump_buf, cam4_rd, cam4_rd->dump_thread);

		} else {
		    CREATE_THREAD(res, cam4_rd_process, cam4_rd, cam4_rd->dump_thread);
		}

		if( res < 0 ) {
		    ETRACE("Thread error. errno ");
		    exit(-1);
		}

		TRACEP(0, "Thread started \n");

		dump_hex((uint8_t *)ptr, sizeof(video_frame_raw_hdr_t));

		return 0;
	} else {
		if (cam4_rd->clear_buff)
			memset(cam4_rd->buff_fd[cam4_rd->idx ^ 1], 0, cam4_rd->used_buf_space);
	}

	/* new packet > old buff = reallocate */
	if( cam4_rd->used_buf_space < todo ) {
	    TRACEP(0, "[%s] we need more space: alloc:%08x req:%08x\n", __func__, cam4_rd->used_buf_space, todo);

	    TRACEP(0, "FH: \n");
	    dump_hex((uint8_t *)ptr, sizeof(video_frame_raw_hdr_t));
	    exit(-1);
	}

	/* put frame into output (file, screen, etc) */
	if( cam4_rd->start_mode & frame_done_flag ) {
	    /* old frame already wrote */
	    cam4_rd->start_mode &= ~(frame_done_flag);
	    cam4_rd->idx ^= 1;
	    pthread_kill(cam4_rd->dump_thread, SIGRTMIN);

	    cam4_rd->w = FH->x_dim;
            cam4_rd->h = FH->y_dim;

	    lag = 0;
	} else {
	    lag++;
	}

	return 0;
}

/* copy data into local buffer */
static inline int cam4_rd_read_fd(void *ptr, cam4_rd_t *cam4_rd)
{
	video_frame_raw_t		*pFD = ptr;
	video_frame_raw_hdr_t		*pFH = &cam4_rd->FH;

	cam4_rd->pFD = pFD;
	pFD->size = ntohs(pFD->size);
	pFD->offs = ntohl(pFD->offs);
	pFD->offs = pFD->offs & ~(0xf<<28);
	if(pFD->fseq !=  (pFH->fseq & 0xff)) {
		TRACEPNF(0, "WARN: FH MISSED %04x@%06x FD not belong to FH. FH->fseq: %08x FD->fseq: %04x delta: %d %u.%u.%u.%u -> %u.%u.%u.%u\n",
		    pFD->size,
		    pFD->offs,
		    pFH->fseq & 0xff,
		    pFD->fseq,
		    (pFD->fseq-pFH->fseq)& 0xff,
		    INET2DIG2(&cam4_rd->iph->ip_src),
		    INET2DIG2(&cam4_rd->iph->ip_dst)
		);

		pFH->fseq += (pFD->fseq-pFH->fseq)& 0xff;
	}

	if(offs_static != pFD->offs) {
	    TRACEPNF(0, "\tWARN DT:%08x should be:%08x received:%08x\n", pFD->offs - offs_static, offs_static, pFD->offs) ;
	}

	if(cam4_rd->fh_size < pFD->offs + pFD->size) {
//	    	TRACEP(0, "\tINVALID DATA\n");
		return 0;
	}//  .,rflyz-vbybr

	offs_static = pFD->offs + pFD->size;

	memcpy(cam4_rd->buff_fd[cam4_rd->idx] + pFD->offs, pFD->data, pFD->size);

	cam4_rd->done += pFD->size;

	return 0;
}

static inline void parse_packet(uint8_t *data, struct tpacket_hdr *h, cam4_rd_t	*cam4_rd)
{
    uint32_t	lid = (uint32_t)(*data);
    lid = htonl(lid) ;

    switch(lid&LID_TYPE) {
	case LID_FD:

		if( cam4_rd->buff_fd[0] == NULL ) {
		    return;
		}

		cam4_rd_read_fd(data, cam4_rd);
		break;

	case LID_FH:
		cam4_rd_read_fh(data, cam4_rd, h->tp_snaplen - h->tp_mac - sizeof(struct ip));

		offs_static = 0;
		break;

	default:
	    TRACEPNF(0, "This is not a FD or a FH header. dump first 128 octets :/ \n");
    }

}

static void raw_off(cam4_rd_t *cam4_rd, mcast_cl_interface_t *ifs)
{
	TRACEPNF(0, "Raw off\n");
	cam4_cmd_cl_process(&cam4_rd->cam4_cl, 1 /* stop */, cam4_rd->flow_id, 0);
}

static int raw_on(cam4_rd_t *cam4_rd, mcast_cl_interface_t *ifs)
{
	TRACEPNF(0, "Raw on\n");
    cam4_cmd_cl_t	*cam4_cl = &cam4_rd->cam4_cl;
    p3_conn_ipv4_t	*conn_mcast = &ifs->conn_in;
    p3_conn_ipv4_t	*conn_raw = &cam4_cl->conn_ipv4;

    /* copy parameters from mcast response */
    *conn_raw = *conn_mcast;
    //inet_aton("192.168.0.165", (struct in_addr *)&cam4_cl->conn_cl.ip);

    return (cam4_cmd_cl_process(cam4_cl, 0 /* start */,  cam4_rd->flow_id, 0));
}

#if 0
int cam4_set_lut(abi_io_tasks_t* ts, uint8_t lid, uint8_t type, uint16_t max_in, uint16_t max_out) {
	abi_io_task_t* tw= &ts->writer;
	abi_context_t	*abiw = tw->abi;
	int res;
	abiw_reopen(abiw);
	abi_node_t	tmp	= { };

	p3_lut_attrs_t lut;
	lut->lid = ntohs(lid);
	lut->type = ntohs(type);
	lut->flags = P3_LUT_SCALE_AUTO;
	lut->max_in = ntohs(max_in);
	lut->max_out = ntohs(max_out);


	ABI_SET_KEY32(tmp,P3_CM_UPDATE_VAL_CMD);
	res = abi_append_node(abiw,&tmp);

	ABI_SET_KEY32(tmp,P3_ABI_ITEM_N);
	res = abi_append_subnode(abiw,res,&tmp);

	ABI_SET_KEY32_VARA(tmp,P3_ABI_KEY_T, "/sensor");
	abi_append_subnode(abiw,res,&tmp);

	ABI_SET_KEY32(tmp,P3_ABI_VAL_T);
	res = abi_append_node(abiw,&tmp);

	ABI_SET_KEYA(tmp,P3_ABI_TASK_N);
	res = abi_append_subnode(abiw,res,&tmp);

	ABI_SET_KEYA(tmp,P3_ABI_SENSOR_N);
	res = abi_append_subnode(abiw,res,&tmp);

	ABI_SET_KEYA_VAR(tmp,P3_ABI_SENSOR_LUT_T,lut);
	abi_append_subnode(abiw,res,&tmp);

	TRACEPNF(0,"Set LUT max_in=%d max_out=%d \n",max_in, max_out);
	abiw_write_task(tw, (int)1);
	abi_task_send(ts, NULL, 0);

	return 0;
}
#endif


int cam4_get_params(abi_io_tasks_t* ts) {
	abi_io_task_t* tw= &ts->writer;
	abi_context_t	*abiw = tw->abi;
	int res;
	abiw_reopen(abiw);
	abi_node_t	tmp	= { };

	ABI_SET_KEY32(tmp,P3_CM_GET_VALS_CMD);
	res = abi_append_node(abiw,&tmp);

	ABI_SET_KEY32(tmp,P3_ABI_ITEM_N);
	res = abi_append_subnode(abiw,res,&tmp);

	ABI_SET_KEY32_VARA(tmp,P3_ABI_KEY_T, "/sensor");
	abi_append_subnode(abiw,res,&tmp);

	ABI_SET_KEY32(tmp,P3_ABI_VAL_T);
	res = abi_append_node(abiw,&tmp);

	ABI_SET_KEYA(tmp,P3_ABI_TASK_N);
	res = abi_append_subnode(abiw,res,&tmp);

	ABI_SET_KEYA(tmp,P3_ABI_SENSOR_N);
	res = abi_append_subnode(abiw,res,&tmp);

	abiw_write_task(tw, (int)1);
	abi_task_send(ts, NULL, 0);

	return 0;
}



#if 0

[P3_CM_UPDATE_VAL_CMD] = {
    [P3_ABI_ASYNC_CONTEXT_T] = uint32_t(xxx);
    [P3_ABI_GID_T] = p3_gid_t(yyy);
    [P3_ABI_ITEM_N] = {
	[P3_ABI_KEY_T] = ascinz("/wire");	// NON ZERO terminated !!!
	[P3_ABI_VAL_T] = {
	    [P3_ABI_RULE_N] = {
		[P3_ABI_WIRE_N] = {
		    [P3_ABI_OFFSET_GID_T] = uint8_t(2);	// Sub device 2
		    [P3_ABI_ROUT_T]    	= lb_task_rout_t {
			.mode   = ABI_P3_RELAY_SIMPLE|ABI_P3_RELAY_LEVCON,	/*  */
			.state  = ABI_P3_RELAY_OFF
		    };
		}
		[P3_ABI_WIRE_N] = {
		    [P3_ABI_OFFSET_GID_T] = uint8_t(1);	// Sub device 1
		    [P3_ABI_DIN_T]    	= lb_task_din_t {
			.flags = 		/* flags			 */
			.min_duration =		/* us debounce filter		 */
			.delays =		/* us Stb delay in tu from flags */
			.delay0 =		/* us Off delay in tu from flags */
			.delay1 =		/* us On  dealy in tu from flags */
		    };
		}
	    };
	};
    };
};

#endif


int cam4_set_expo(int exp,abi_io_tasks_t* ts) {

	abi_io_task_t* tw= &ts->writer;
	uint64_t exp1 = exp;
	uint64_t exp2 = (uint64_t)(filetime_to_net(&exp1));
	abi_context_t	*abiw = tw->abi;
	int res;
	abiw_reopen(abiw);
	abi_node_t	tmp	= { };
	/*
	[P3_CM_UPDATE_VAL_CMD] = {
		[P3_ABI_ASYNC_CONTEXT_T] = uint32_t(xxx);
		[P3_ABI_GID_T] = p3_gid_t(yyy);
		[P3_ABI_ITEM_N] = {
			[P3_ABI_KEY_T] = ascinz("/sensor");	// NON ZERO terminated !!!
			[P3_ABI_VAL_T] = {
				[P3_ABI_TASK_N] = {
					[P3_ABI_SENSOR_N] = {
						[P3_ABI_EXPOSITION_T]= uint64_t(10);	// 1us

						[P3_ABI_PERIOD_T]    = uint64_t(1000000);	// 100ms
					}
				}
			}
		}
	};*/
	ABI_SET_KEY32(tmp,P3_CM_UPDATE_VAL_CMD);
	res = abi_append_node(abiw,&tmp);

	ABI_SET_KEY32(tmp,P3_ABI_ITEM_N);
	res = abi_append_subnode(abiw,res,&tmp);

	ABI_SET_KEY32_VARA(tmp,P3_ABI_KEY_T, "/sensor");
	abi_append_subnode(abiw,res,&tmp);

	ABI_SET_KEY32(tmp,P3_ABI_VAL_T);
	res = abi_append_node(abiw,&tmp);

	ABI_SET_KEYA(tmp,P3_ABI_TASK_N);
	res = abi_append_subnode(abiw,res,&tmp);

	ABI_SET_KEYA(tmp,P3_ABI_SENSOR_N);
	res = abi_append_subnode(abiw,res,&tmp);

	ABI_SET_KEYA_VAR(tmp,P3_ABI_EXPOSITION_T,exp2);
	abi_append_subnode(abiw,res,&tmp);

	TRACEPNF(0,"Set exposure %d\n",(int)exp);
	abiw_write_task(tw, (int)1);
	abi_task_send(ts, NULL, 0);

	return 0;
}


int cam4_set_period(int per,abi_io_tasks_t* ts) {

	abi_io_task_t* tw= &ts->writer;
	uint64_t per1 = per;
	uint64_t per2 = (uint64_t)(filetime_to_net(&per1));
	abi_context_t	*abiw = tw->abi;
	int res;
	abiw_reopen(abiw);
	abi_node_t	tmp	= { };
	/*
	[P3_CM_UPDATE_VAL_CMD] = {
		[P3_ABI_ASYNC_CONTEXT_T] = uint32_t(xxx);
		[P3_ABI_GID_T] = p3_gid_t(yyy);
		[P3_ABI_ITEM_N] = {
			[P3_ABI_KEY_T] = ascinz("/sensor");	// NON ZERO terminated !!!
			[P3_ABI_VAL_T] = {
				[P3_ABI_TASK_N] = {
					[P3_ABI_SENSOR_N] = {
						[P3_ABI_EXPOSITION_T]= uint64_t(10);	// 1us

						[P3_ABI_TIME_PERIOD_T]    = uint64_t(1000000);	// 100ms
					}
				}
			}
		}
	};*/
	ABI_SET_KEY32(tmp,P3_CM_UPDATE_VAL_CMD);
	res = abi_append_node(abiw,&tmp);

	ABI_SET_KEY32(tmp,P3_ABI_ITEM_N);
	res = abi_append_subnode(abiw,res,&tmp);

	ABI_SET_KEY32_VARA(tmp,P3_ABI_KEY_T, "/sensor");
	abi_append_subnode(abiw,res,&tmp);

	ABI_SET_KEY32(tmp,P3_ABI_VAL_T);
	res = abi_append_node(abiw,&tmp);

	ABI_SET_KEYA(tmp,P3_ABI_TASK_N);
	res = abi_append_subnode(abiw,res,&tmp);

	ABI_SET_KEYA(tmp,P3_ABI_SENSOR_N);
	res = abi_append_subnode(abiw,res,&tmp);

	ABI_SET_KEYA_VAR(tmp,P3_ABI_TIME_PERIOD_T,per2);
	abi_append_subnode(abiw,res,&tmp);

	TRACEPNF(0,"Set period %d\n",(int)per);
	abiw_write_task(tw, (int)1);
	abi_task_send(ts, NULL, 0);

	return 0;
}

int cam4_set_gains(float* gains,abi_io_tasks_t* ts) {

	abi_io_task_t* tw= &ts->writer;
	fp16x16_t gains_n[5];
	int i;
	for (i=0;i<5;i++) {
		gains_n[i] = htonl((uint32_t)(gains[i]*65536));
	}
	abi_context_t	*abiw = tw->abi;
	int res;
	abiw_reopen(abiw);
	abi_node_t	tmp	= { };
	/*
	[P3_CM_UPDATE_VAL_CMD] = {
		[P3_ABI_ASYNC_CONTEXT_T] = uint32_t(xxx);
		[P3_ABI_GID_T] = p3_gid_t(yyy);
		[P3_ABI_ITEM_N] = {
			[P3_ABI_KEY_T] = ascinz("/sensor");	// NON ZERO terminated !!!
			[P3_ABI_VAL_T] = {
				[P3_ABI_TASK_N] = {
					[P3_ABI_SENSOR_N] = {
						[P3_ABI_EXPOSITION_T]= uint64_t(10);	// 1us
						[P3_ABI_SENSOR_GAINS_T]	OPTIONAL ARRAY (fp16x16_t);	// "sensor-gains"
						[P3_ABI_TIME_PERIOD_T]    = uint64_t(1000000);	// 100ms
					}
				}
			}
		}
	};*/
	ABI_SET_KEY32(tmp,P3_CM_UPDATE_VAL_CMD);
	res = abi_append_node(abiw,&tmp);

	ABI_SET_KEY32(tmp,P3_ABI_ITEM_N);
	res = abi_append_subnode(abiw,res,&tmp);

	ABI_SET_KEY32_VARA(tmp,P3_ABI_KEY_T,"/sensor");
	abi_append_subnode(abiw,res,&tmp);

	ABI_SET_KEY32(tmp,P3_ABI_VAL_T);
	res = abi_append_node(abiw,&tmp);

	ABI_SET_KEYA(tmp,P3_ABI_TASK_N);
	res = abi_append_subnode(abiw,res,&tmp);

	ABI_SET_KEYA(tmp,P3_ABI_SENSOR_N);
	res = abi_append_subnode(abiw,res,&tmp);

	ABI_SET_KEYA_VAR(tmp,P3_ABI_SENSOR_GAINS_T,gains_n);
	abi_append_subnode(abiw,res,&tmp);


	TRACEPNF(0,"Set gains %f %f %f %f %f\n",gains[0],gains[1],gains[2],gains[3],gains[4]);
	abiw_write_task(tw, (int)1);
	abi_task_send(ts, NULL, 0);

	return 0;
}

int cam4_set_tunnel (abi_io_tasks_t* ts, int start, char* prefix) {
	abi_io_task_t* tw= &ts->writer;

	abi_context_t	*abiw = tw->abi;
	int res;
	abiw_reopen(abiw);
	abi_node_t	tmp	= { };

	if (start) {
		ABI_SET_KEYA(tmp,P3_CM_TUNNEL_START);
	} else {
		ABI_SET_KEYA(tmp,P3_CM_TUNNEL_STOP);
	}
	res = abi_append_node(abiw,&tmp);

	ABI_SET_KEY32(tmp,P3_ABI_ITEM_N);
	res = abi_append_subnode(abiw,res,&tmp);

	if (strncmp(prefix, "/sensor", 7) == 0)
		ABI_SET_KEY32_VARA(tmp,P3_ABI_KEY_T,"/sensor");
	if (strncmp(prefix, "/dev/uart/0", 11) == 0)
		ABI_SET_KEY32_VARA(tmp,P3_ABI_KEY_T,"/dev/uart/0");
	if (strncmp(prefix, "/dev/uart/1", 11) == 0)
		ABI_SET_KEY32_VARA(tmp,P3_ABI_KEY_T,"/dev/uart/1");
	abi_append_subnode(abiw,res,&tmp);

	ABI_SET_KEY32(tmp,P3_ABI_VAL_T);
	res = abi_append_node(abiw,&tmp);

	if (start)
		TRACEPNF(0,"Tunnel started\n");
	else
		TRACEPNF(0,"Tunnel stopped\n");

	abiw_write_task(tw, (int)1);
	abi_task_send(ts, NULL, 0);
	return 0;
}

int send_uart(abi_io_tasks_t* ts, char* buf) {
	abi_io_task_t* tw= &ts->writer;

	abi_context_t	*abiw = tw->abi;
	int res;
	abiw_reopen(abiw);
	abi_node_t	tmp	= { };

	ABI_SET_KEY32(tmp,P3_CM_APPEND_VAL_CMD);
	res = abi_append_node(abiw,&tmp);

	ABI_SET_KEY32(tmp,P3_ABI_ITEM_N);
	res = abi_append_subnode(abiw,res,&tmp);

	ABI_SET_KEY32_VARA(tmp,P3_ABI_KEY_T,"/dev/uart/1");
	abi_append_subnode(abiw,res,&tmp);

	ABI_SET_KEY32_VARA(tmp,P3_ABI_VAL_T, buf);
	res = abi_append_node(abiw,&tmp);

	abiw_write_task(tw, (int)1);
	abi_task_send(ts, NULL, 0);
	return 0;
}


int cam4_reinit(cam4_rd_t* cam4_rd){
	cam4_rd->mcast_reinit = 1;
	return 0;
}



enum sens_state{
    EMVA_WAIT,
	EMVA_START,
	EMVA_DUMP_A,
	EMVA_DUMP_B,
	EMVA_NEXT_STEP,
	EMVA_NOP,
} sens_state;


typedef struct sens_test_s
{
  enum sens_state state;

  int default_period;
  int default_expo;
  int default_step;
  int default_steps;

  int default_skip_frames;

  char temp[255];

} sens_test_t;
  sens_test_t sens_test =
   {
   EMVA_NOP,
   1000000,// period, 100 ms
   1500,   // start expo
   10000,  // step, dT = 1 ms
   20,     // steps count
   3,      // img skip before dump
   };

// инициализация теста сенсора
// осуществляется следующим набором команд:
// BLACK - поиск уровня черного
//         выключение освещения
//         период - 100 мс
//         экспозиция - 300 мкс
//         LUT х15
// SIGMA - оценка уровня сигмы
//         выключение освещения
//         период - 100 мс
//         экспозиция - 300 мкс
//
//         включение освещения
//         период - 100 мс
//         экспозиция - 300 мкс
//         расчет, возврат

int cam4_script_processing(cam4_rd_t* cam4_rd)
{

    switch (sens_test.state)
    {
        case EMVA_START:
            TRACEPNF(0,"Trying to run EMVA test\n");

            cam4_set_period(sens_test.default_period,cam4_rd->ts);

            cam4_set_expo(sens_test.default_expo,cam4_rd->ts);
            cam4_rd->expo = sens_test.default_expo; // store value

            //sprintf(sens_test.temp, "EMVA_%s", cam4_rd->frame_idx) ;
            //mkdir("EMVA")

            sprintf(cam4_rd->path, "/home/dump/emva");
            //sprintf(cam4_rd->mode, "TEST");

            cam4_rd->img_num = 0;

            sens_test.state = EMVA_WAIT;
        break;

        case EMVA_WAIT:
            TRACEPNF(0,"EMVA wait %d frames to continue\n", sens_test.default_skip_frames );
            if ( sens_test.default_skip_frames-- == 0){
                sens_test.state = EMVA_DUMP_A;
            }
        break;

        case EMVA_DUMP_A: // for EMVA test recommended 2 identical imgs
            TRACEPNF(0,"EMVA dump img A\n");
            sprintf(cam4_rd->pair, "A");
            gettimeofday(&tv,NULL);
            cam4_rd->dumpraw_time = time_add(tv,-1);
            sens_test.state = EMVA_DUMP_B;

        break;

        case EMVA_DUMP_B:
            TRACEPNF(0,"EMVA dump img B\n");
            sprintf(cam4_rd->pair, "B");
            gettimeofday(&tv,NULL);
            cam4_rd->dumpraw_time = time_add(tv,-1);
            sens_test.state = EMVA_NEXT_STEP;


            if (cam4_rd->img_num++ >= sens_test.default_steps){
                TRACEPNF(0,"EMVA complete!\n");
                sens_test.state = EMVA_NOP;
            }

        break;

        case EMVA_NEXT_STEP:

            TRACEPNF(0,"EMVA prepare to the next step. Expo %d, Max %d \n", (sens_test.default_step)*(cam4_rd->img_num), sens_test.default_steps);
            cam4_set_expo((sens_test.default_step)*(cam4_rd->img_num),cam4_rd->ts);
            cam4_rd->expo = (sens_test.default_step)*(cam4_rd->img_num); // store value

            sens_test.default_skip_frames = 1;

            sens_test.state = EMVA_WAIT;
        break;

        default:
        case EMVA_NOP:
            //TRACEPNF(0,"EMVA do nothing\n");
            break;
    }

    return 0;
}



int parse_command(FILE* fdstream,cam4_rd_t* cam4_rd) {
	char buf[300];
	char* s = fgets(buf, sizeof(buf), fdstream);
	if (s == NULL)
		return -1;

	TRACEPNF(5,"Command: %s\n", buf);
	if (strncmp(buf,"NOP:",4) == 0) {
		return 0;
	}
	if (strncmp(buf,"GET:",4) == 0) {
		cam4_get_params(cam4_rd->ts);
		return 0;
	}
	if (strncmp(buf,"EXP:",4) == 0) {
		int exp = 0;
		sscanf((char*)(buf+4),"%d",&exp);
		cam4_set_expo(exp,cam4_rd->ts);
		cam4_rd->expo = exp; // store setted value
		return 0;
	}
	if (strncmp(buf,"DUMPRAW:",8) == 0) {
		double timeout = 0;
		sscanf((char*)(buf+8),"%lf",&timeout);
		gettimeofday(&tv,NULL);
		cam4_rd->dumpraw_time = time_add(tv,timeout);
		return 0;
	}
	if (strncmp(buf,"DUMPYUV:",8) == 0) {
		double timeout = 0;
		sscanf((char*)(buf+8),"%lf",&timeout);
		gettimeofday(&tv,NULL);
		cam4_rd->dumpyuv_time = time_add(tv,timeout);
		return 0;
	}
	if (strncmp(buf,"PERIOD:",7) == 0) {
		int per = 0;
		sscanf((char*)(buf+7),"%d",&per);
		cam4_set_period(per,cam4_rd->ts);
		return 0;
	}
	if (strncmp(buf,"GAINS:",6) == 0) {
		float gains[5];
		sscanf((char*)(buf+6),"%f %f %f %f %f",&gains[0],&gains[1],&gains[2],&gains[3],&gains[4]);
		cam4_set_gains(gains,cam4_rd->ts);
		return 0;
	}
	if (strncmp(buf,"VIDEO:",6) == 0) {
		if (strncmp(buf+6,"START",5) == 0) {
				if (cam4_rd->video_writing == VIDEO_WRITE_NONE)
					cam4_rd->video_writing = VIDEO_WRITE_START;
		} else if (strncmp(buf+6,"FINISH",6) == 0) {
			if (cam4_rd->video_writing == VIDEO_WRITE_PROCESS)
				cam4_rd->video_writing = VIDEO_WRITE_FINISH;
			}
		return 0;
	}
	if (strncmp(buf,"RAWVIDEO:",9) == 0) {
		if (strncmp(buf+9,"START",5) == 0) {
				if (cam4_rd->raw_video_writing == VIDEO_WRITE_NONE)
					cam4_rd->raw_video_writing = VIDEO_WRITE_START;
		} else if (strncmp(buf+9,"FINISH",6) == 0) {
			if (cam4_rd->raw_video_writing == VIDEO_WRITE_PROCESS)
				cam4_rd->raw_video_writing = VIDEO_WRITE_FINISH;
			}
		return 0;
	}
	if (strncmp(buf,"REINIT:",7) == 0) {
		cam4_reinit(cam4_rd);
		return 0;
	}
	if (strncmp(buf,"XVIMAGE:",8) == 0) {
		if (strncmp(buf+8,"WIDTH:",6) == 0) {
			sscanf(buf+14,"%d",&(cam4_rd->xvWidth));
		}
		if (strncmp(buf+8,"HEIGHT:",7) == 0) {
			sscanf(buf+15,"%d",&(cam4_rd->xvHeight));
		}
	}
	if (strncmp(buf,"TUNNEL:",7) == 0) {
		if (strncmp(buf+7, "START:", 6) == 0) {
			cam4_set_tunnel(cam4_rd->ts, 1, buf+13);
		}
		if (strncmp(buf+7, "STOP:", 5) == 0) {
			cam4_set_tunnel(cam4_rd->ts, 0, buf+12);
		}
	}

	if (strncmp(buf,"UART:",5) == 0) {
		send_uart(cam4_rd->ts, buf+5);
	}

	if (strncmp(buf,"TEST:",5) == 0) {
        //send_sout(buf+5);
		//  cam4_init_test(cam4_rd, buf+5);
		sens_test.state = EMVA_START;

	}



#if 0
	if (strncmp(buf,"LUTRAW:",7) == 0) {
		if (strncmp(buf+7,"LIN:",4) == 0) {
			int max_in, max_out;
			sscanf((char*)(buf+11),"%d %d",&max_in,&max_out);
			cam4_set_lut(cam4_rd->ts, P3_CAM_LUT_RAW,P3_LUT_DEFAULT_LINEAR,max_in,max_out);
		}
		return 0;
	}
#endif
	if (strncmp(buf,"PATH:",5) == 0) {
		sscanf(buf+5,"%s",cam4_rd->path);
		TRACEP(0, "CMD: \"PATH\"Received FROM FIFO: %s\n", cam4_rd->path);
		}

	if (strncmp(buf,"IMGNUM:",7) == 0) {
		sscanf((char*)(buf+7),"%d",&(cam4_rd->img_num));
		TRACEP(0, "CMD: \"IMGNUM\"Received FROM FIFO: %d\n", cam4_rd->img_num);
	}

	if (strncmp(buf,"MODE:",5) == 0) {
		sscanf(buf+5,"%s",cam4_rd->mode);
		TRACEP(0, "CMD: \"MODE\"Received FROM FIFO: %s\n", cam4_rd->mode);
		}



	if (strncmp(buf,"STOP:",5) == 0) {
		TRACEP(0, "CMD: \"STOP\"Received FROM FIFO\n");
		no_sig_exit = 0;
	}

	return 0;
}

static int read_exposition(void *priv, uint8_t *data, size_t size, size_t tail)
{
	abi_mem_flow_t* f = priv;
	cam4_rd_t* cam4_rd = f->ctx;
	common_t* c = cam4_rd->common;
	uint64_t exp = (uint64_t)(filetime_to_net(f->data+ f->offs));
	//printf("Exposition %d\n",(uint32_t)exp);
	c->exp = exp;
	f->offs += size+tail;
	return 0;
}

static int read_period(void *priv, uint8_t *data, size_t size, size_t tail) {
	abi_mem_flow_t* f = priv;
	cam4_rd_t* cam4_rd = f->ctx;
	common_t* c = cam4_rd->common;
	uint64_t per = (uint64_t)(filetime_to_net(f->data+ f->offs));
	//printf("Period %d\n",(uint32_t)per);
	c->period = per;
	f->offs += size+tail;
	return 0;
}

static int read_gains(void *priv, uint8_t *data, size_t size, size_t tail) {
	abi_mem_flow_t* f = priv;
	cam4_rd_t* cam4_rd = f->ctx;
	common_t* c = cam4_rd->common;
	double gains[5];
	int i;
	uint16_t *d = f->data + f->offs;
	for (i=0;i<5;i++,d+=2) {
		uint16_t g1 = (d[0]);
		uint16_t g2 = (d[1]);
		gains[i] = g2+g1/65536.;
		//printf("Gains[%d] %lf\n",i,gains[i]);
		c->gains[i] = gains[i];
	}
	f->offs += size+tail;
	return 0;
}

static int read_lut(void *priv, uint8_t *data, size_t size, size_t tail) {
	abi_mem_flow_t	*f	 = priv;
//	cam4_rd_t	*cam4_rd = f->ctx;
//	common_t	*c	 = cam4_rd->common;
//	uint16_t	*d	 = f->data + f->offs;

	f->offs += size + tail;

	return 0;
}

int cam4_prepare_ioc_abir(
	cam4_rd_t	*cam4_rd,
	io_ctrl_t	*ioc
)
{
	abi_context_t *abi = ioc->rx_abi = abir_open(
	    NULL,
	    &ioc->rx_flow,
	    abi_mem_reader,
	    abi_mem_skiperq,
	    40
	);
	ioc->rx_flow.ctx = cam4_rd;
	if(!abi)
		return -P3_FAILED;

	abi_node_t	tmp	= { };
	int res = 0;

	ABI_SET_KEY32(tmp,P3_CM_UPDATE_VAL_CMD);
	res = abi_append_node(abi,&tmp);

	ABI_SET_KEY32(tmp,P3_ABI_ITEM_N);
	res = abi_append_subnode(abi,res,&tmp);

	ABI_SET_KEY32_VAR(tmp, P3_ABI_ID_T, cam4_rd->id);
	abi_append_node(abi, &tmp);

	ABI_SET_KEY32_VAR(tmp, P3_ABI_GID_T, cam4_rd->id.gid);
	abi_append_node(abi, &tmp);

	ABI_SET_KEY32_VARA(tmp, P3_ABI_KEY_T, SENSOR);
	abi_append_subnode(abi, res, &tmp);

	ABI_SET_KEY32(tmp, P3_ABI_VAL_T);
	res = abi_append_node(abi, &tmp);

	ABI_SET_KEYA(tmp, P3_ABI_TASK_N);
	res = abi_append_subnode(abi, res, &tmp);

	ABI_SET_KEYA(tmp, P3_ABI_SENSOR_N);
	res = abi_append_subnode(abi, res, &tmp);

	ABI_SET_KEYA(tmp, P3_ABI_EXPOSITION_T);
	ABI_SET_READER(tmp, read_exposition);
	res = abi_append_subnode(abi, res, &tmp);

	ABI_SET_KEYA(tmp, P3_ABI_TIME_PERIOD_T);
	ABI_SET_READER(tmp, read_period);
	res = abi_append_node(abi, &tmp);

	ABI_SET_KEYA(tmp, P3_ABI_SENSOR_GAINS_T);
	ABI_SET_READER(tmp, read_gains);
	res = abi_append_node(abi, &tmp);

	ABI_SET_KEYA(tmp, P3_ABI_SENSOR_LUT_T);
	ABI_SET_READER(tmp, read_lut);
	res = abi_append_node(abi, &tmp);

	return 0;
}

void* process_cm_ifs(void*priv) {
	struct pollfd pfd[1];
	cam4_rd_t* cam4_rd = priv;
	int fd = open_fifo("/tmp/cam4.fifo", O_RDONLY);
	FILE* fdstream = fdopen(fd,"r");
	int s_in;
	struct sockaddr_in cl_addr;
	if (fd<0) {
		ETRACE("ERROR cannot open fifo");
		return NULL;
	}
	io_ctrl_t	*ioc=(io_ctrl_t*)malloc(sizeof(io_ctrl_t));
	abi_init_ioc(
	    ioc,
	    1,
	    cam4_rd,
	    NULL,			/* interfaces handler- NULL for client*/
	    65536,			/* rx buff size */
	    65536,			/* tx buff size */
	    trace_prefix		/* subsystem name */
	);
	p3_conn_ipv4_t	*conn_raw = &(cam4_rd->cm_ifs->conn_in);

	s_in = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    cl_addr.sin_family = PF_INET;
    cl_addr.sin_addr = *((struct in_addr *)&conn_raw->ip);
    cl_addr.sin_port = htons(conn_raw->port);
    TRACEP(0, "Command interface [%s:%d]\n", inet_ntoa(cl_addr.sin_addr), ntohs(cl_addr.sin_port));

    int res = connect(s_in, (struct sockaddr*)&cl_addr, (socklen_t)sizeof(cl_addr));
    if(res) {
	    ETRACEP("[err] cannot connect to %s:%d. errno ", \
			    inet_ntoa(cl_addr.sin_addr), \
			    ntohs(cl_addr.sin_port) );
	    return NULL;
    }
    TRACEPNF(0, "connected to cm dev\n");

    res = abi_ioc_add_socket(ioc, s_in);
    if(res < 0) {
	    TRACE(0, "Cannot add socket into ioc structure");
	    return NULL;
    }

    cam4_rd->ts = ioc->tasks + res;

    pfd[0].fd = fd;
	pfd[0].events = POLLIN|POLLERR;
	pfd[0].revents = 0;

	cam4_prepare_ioc_abir(cam4_rd, ioc);
	while (1) {
		//process incoming packet from CM interface
		abi_io_iterator(ioc, 10, 0);

		//process incoming commands from fifo
		int res = poll(pfd,1,0);
		if (res<0) {
			TRACEPNF(0,"ERROR poll()\n");
			return NULL;
		}
		if (res == 0)
			continue;
		// res > 0
		if (pfd[0].revents & POLLIN) {
			parse_command(fdstream,cam4_rd);

		}
		/*
		if (pfd[0].revents & POLLHUP) {
			shutdown(s_in, SHUT_RDWR);
			close(s_in);

			break;
		}*/
	}

	return NULL;
}


int find_ifs(mcast_cl_api_t *mcast_cl)
{
    int			i, res;
    mcast_cl_payload_t	*rx_vars = &mcast_cl->rx_vars;
    cam4_rd_t 		*cam4_rd = (cam4_rd_t *)mcast_cl->priv;

    res = memcmp(&mcast_cl->ipv4_device.sin_addr, &mcast_cl->ipv4_dst.sin_addr, sizeof(typeof(mcast_cl->ipv4_dst.sin_addr)));
    if( (mcast_cl->cpu_id != rx_vars->cpu_id) && (res != 0) )
		return -1;

	cam4_rd->device_name = (char*)rx_vars->dev_name.str;
	TRACEPNF(0,"Device name %s\n",rx_vars->dev_name.str);
	//cam4_rd->device_name_size = rx_vars->dev_name.size;
    /* find properly ifs */

    for(i = 1; i < rx_vars->ifs_idx; i++ ) {
    	mcast_cl_interface_t *ifs = &rx_vars->interface[i];

		if( ifs->type == P3_CM && cam4_rd->first_time) {
			cam4_rd->first_time = 0;
			int res;
			TRACEPNF(0, "Found P3_CM\n");
			cam4_rd->cm_ifs = ifs;
			CREATE_THREAD(res, process_cm_ifs, cam4_rd, cam4_rd->cm_thread);

			break;
		}

    }

    for(i = 1; i < rx_vars->ifs_idx; i++ ) {
    	mcast_cl_interface_t *ifs = &rx_vars->interface[i];
		if( ifs->type == P3_DEVICE_BOTH ) {
			TRACEPNF(0, "Found P3_DEVICE_BOTH\n");
			res = raw_on(cam4_rd, ifs);
			return i;
		}

    } // for(i = 1; i < IFS_NUM_CL; i++ )

    TRACEPNF(0, "[ERR] Cannot find P3_CM interface\n");

    return -1;
}

void mcast_cl_do_request(mcast_cl_api_t *m)
{
	if(!m->ipv4_device.sin_addr.s_addr)
		return;

	abi_io_task_t	*tw  = &m->tx_task;
	ssize_t		res;

	/* PING dest */
	abi_preinit_taskw(
		tw,
		m->pfds[0].fd,
		NULL,
		1024	/* buff size */
	);

	abi_writer_task_reset(tw);

	abi_node_t	tmp = { };

	ABI_SET_KEY32(tmp, CONFIG_ABI_GET_STAT_CMD);
	res = abi_append_node(tw->abi, &tmp);
	abiw_write(tw->abi, 0);			/* prepare entair PACKET to send */

	m->ipv4_device.sin_port = htons(4521);
	res = sendto(tw->fd, tw->data, tw->todo, 0, (struct sockaddr*)&m->ipv4_device, (socklen_t)sizeof(m->ipv4_device));
	if(res != tw->todo) {
		ETRACE("When send mcast");
	}

	TRACEPNF(0, "TEST\n");
}

static int cam4_mcast(cam4_rd_t *cam4_rd)
{
    int			res;
    void		*thread_exit = 0;
    mcast_cl_api_t	*mcast_cl = &cam4_rd->mcast_cl;

    TRACEPNF(0, "[%s]\n", __func__);

     if( (res=mcast_cl_init(mcast_cl)) < 0)
	 return -1;

    //mcast_cl->cpu_id = 0xa25d00505a84070eull;		// linux devision
    //mcast_cl->cpu_id = 0x9ea2ad02bc534713ull;		// yuris

   /* get the actual information */
    CREATE_THREAD(res, process_cl_mcast, mcast_cl, cam4_rd->mcast_cl_thread);

    THREAD_JOIN(res, cam4_rd->mcast_cl_thread, thread_exit);

    /* clear buffers */
    mcast_cl_destroy(mcast_cl);
    return res;
}

int create_connection(connection_t* conn) {
	if ( (conn->fd=socket(PF_PACKET, SOCK_DGRAM, 0))<0 ) {
		perror("socket()");
		return 1;
	}

	/* Setup the fd for mmap() ring buffer */
	conn->req.tp_block_size	= 8192;
	conn->req.tp_frame_size	= 8192;
	conn->req.tp_block_nr		= 2000;
	conn->req.tp_frame_nr		= 1*2000;

	if ( (setsockopt(conn->fd,
		SOL_PACKET,
		PACKET_RX_RING,
		(char *)&(conn->req),
		sizeof(conn->req))) != 0 ) {
		perror("setsockopt()");
		close(conn->fd);
		return 1;
	};

	/* mmap() the sucker */
	conn->map=mmap(NULL,
		conn->req.tp_block_size * conn->req.tp_block_nr,
		PROT_READ|PROT_WRITE|PROT_EXEC, MAP_SHARED, conn->fd, 0);

	if ( conn->map==MAP_FAILED ) {
		perror("mmap()");
		close(conn->fd);
		return 1;
	}

	/* Setup our ringbuffer */
	conn->ring=malloc(conn->req.tp_frame_nr * sizeof(struct iovec));
	int i;
	for(i=0; i<conn->req.tp_frame_nr; i++) {
		conn->ring[i].iov_base=(void *)((long)conn->map)+(i*conn->req.tp_frame_size);
		conn->ring[i].iov_len=conn->req.tp_frame_size;
	}

// ++++++++++++++++++++++++++++++++++++++++++
	/* bind the packet socket */
	struct sockaddr_ll	addr = {
	    .sll_family		= AF_PACKET,
	    .sll_protocol	= htons(ETH_P_IP),
	    .sll_ifindex	= 0,	/* eth0 FIXME - need detect ifs ID via ioctl(). 0 - any ifs */
	    .sll_hatype		= 0,
	    .sll_pkttype	= 0,
	    .sll_halen		= 0,
	};

	if(bind(conn->fd, (struct sockaddr *)&addr, sizeof(addr)) ) {
		munmap(conn->map, conn->req.tp_block_size * conn->req.tp_block_nr);
		perror("bind()");
		close(conn->fd);
		return 1;
	}
	return 0;
}
int connection_free(connection_t* conn) {

	struct tpacket_stats st;
	socklen_t len=sizeof(st);

	if (!getsockopt(conn->fd,SOL_PACKET,PACKET_STATISTICS,(char *)&st,&len))
		TRACE(0, "recieved %u packets, dropped %u\n",
			st.tp_packets, st.tp_drops);

	if (conn->map)
		munmap(conn->map, conn->req.tp_block_size * conn->req.tp_block_nr);

	if ( conn->fd>=0 ) {
		close(conn->fd);
		conn->fd = -1;
	}

	if (conn->ring) {
		free(conn->ring);
		conn->ring = NULL;
	}
	return 0;
}

int cam4_ps_main(int argc, char **argv)
{
	struct pollfd 		pfd[2];
	int 			i,k;
	char			val_str[255] = "";
	char			dst_str[255] = "";

	cam4_rd_t	cam4_rd = {
		.mcast_cl.no_sig_exit	= &no_sig_exit,
		.mcast_cl.mcast_addr	= "224.0.1.20",
		.mcast_cl.mcast_port	= 4521,
		.mcast_cl.priv		= &cam4_rd,
		.mcast_cl.find_dev	= &find_ifs,

		.flow_id		= CAM4_RAW_IFS,


		.video_writing		= VIDEO_WRITE_NONE,
		.raw_video_writing	= VIDEO_WRITE_NONE,

		.mcast_reinit		= 0,
		.first_time		= 1,

		.dumpraw_time.tv_sec	= 0,
		.dumpyuv_time.tv_sec	= 0,

		.xvWidth		= 0,
		.xvHeight		= 0,

		.common			= NULL,

		.clear_buff		= 0,
		.disable_mcast		= 0,
	};

	default_debayer_api.debayerRGB_func[0] = debayerRGB_fast_mode0;
	default_debayer_api.debayerRGB_func[1] = debayerRGB_ar_mode1; //debayerRGB_fast_mode1;
	default_debayer_api.debayerRGB_func[2] = debayerRGB_fast_mode2;
	default_debayer_api.debayerRGB_func[3] = debayerRGB_fast_mode3;

	cam4_cmd_cl_t	*cam4_cl = &cam4_rd.cam4_cl;

	cam4_cl->no_sig_exit = &no_sig_exit;
	I = stdout ;

	signal(SIGINT, sigproc);

	/* FIXME - add bayer phase */
	/* parse parameters */
	while ((i = getopt(argc, argv, "ZC:bD:d:f:g:hm:n:sv:zMp:q")) != -1) {
		TRACE(3, "mode %c\n", i);

		switch (i) {
		    case 'g':
			/* file presented */
			k = atoi(optarg);
			switch(k) {
			case 1: cam4_rd.start_mode |= g1_flag; break;
			case 2: cam4_rd.start_mode |= g2_flag; break;
			default: TRACEP(0, "=========== [ERR] unknow -m option.\n"); show_the_banner(); return -1;
			}
			break;

		    case 'n':
			/* numer of files */
			cam4_rd.start_mode |= n_flag;
			cam4_rd.frame_num = atoi(optarg);
			if(cam4_rd.frame_num < 1) {
			    TRACE(0, "n must be more than 1 \n") ;
			    return 0;
			}
			TRACE(0, "Need to process [%d] frames \n", cam4_rd.frame_num);
			break;

		    case 'b':
			/* request face bars */
			cam4_rd.flow_id = CAM4_FACE_IFS;
			break;

		    case 's':
			/* single file */
			cam4_rd.start_mode |= s_flag ;
			cam4_rd.write_cb = &write_separate;
			break;

		    case 'f':
			/* file presented */
			cam4_rd.start_mode |= f_flag ;
			memcpy(cam4_rd.f_name, optarg, sizeof(cam4_rd.f_name));
			cam4_rd.write_cb = &write_frame;
			break;
			case 'z':
				/*clear buffer*/
			cam4_rd.clear_buff = 1;
			break;
		    case 'm':
			/* file presented */
			k = atoi(optarg);
			switch(k) {
			case 1: cam4_rd.start_mode |= m1_flag; break;
			case 2: cam4_rd.start_mode |= m2_flag; break;
			default: TRACEP(0, "=========== [ERR] unknown -g option.\n"); show_the_banner(); return -1;
			}
			break;
		    case 'v':
			/* value for search */
			strncpy(val_str, optarg, sizeof(val_str));
			cam4_rd.camera_ip_str = optarg;
			break;

		    case 'C':
			cam4_rd.camctl_mode = strtoul(optarg, (char **)NULL, 0);
			break;

		    case 'p':
			/* ifs port */
			cam4_rd.ifs_port = strtol(optarg, (char **)NULL, 0);
			break;
		    case 'Z':
			/* disable multicast */
			cam4_rd.disable_mcast = 2;
			break;
		    case 'M':
			/* disable multicast */
			cam4_rd.disable_mcast = 1;
			break;
		    case 'D':
			/* value for search */
			debayer_mode = strtol(optarg, (char **)NULL, 0);
			break;
		    case 'd':
			/* value for search */
			strncpy(dst_str, optarg, sizeof(dst_str));
			break;
		    case 'q':
			quad = 1;
			break;

		    case 'h':
		    default:
			show_the_banner();
			return 0;
		}
	};

	if (!dst_str[0])	 {
		/* if client ip is not set in options, user chooses it manually */
		int		sock = 0;
		char		buf[1024];
		struct ifconf	ifc;

		struct ifreq	*ifr;
		int		nInterfaces;
		int		i;

		if( (sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
			perror("socket");
			return 1;
		}

		ifc.ifc_len = sizeof(buf);
		ifc.ifc_buf = buf;
		if(ioctl(sock, SIOCGIFCONF, &ifc) < 0) {
			perror("ioctl(SIOCGIFCONF)");
			return 1;
		}
		ifr = ifc.ifc_req;
		nInterfaces = ifc.ifc_len / sizeof(struct ifreq);
		for(i = 0; i < nInterfaces; i++)
		{
			struct ifreq *item = &ifr[i];

		/* Show the device name and IP address */
			printf("[%d]\t%s:\tIP %s\n",
				i,
				item->ifr_name,
				inet_ntoa(((struct sockaddr_in *)&item->ifr_addr)->sin_addr));
		}
		if (scanf("%d",&i) < 0 || i<0 || i>=nInterfaces) {
			perror("Number is not valid\n");
			return 1;
		}
		struct ifreq *item = &ifr[i];
		strcpy(dst_str, inet_ntoa(((struct sockaddr_in *)&item->ifr_addr)->sin_addr));
		close(sock);
	}

	/* check masks */
	if( cam4_rd.start_mode & m1_flag) {
	    k = sscanf(val_str, "%"PRIx64, &cam4_rd.mcast_cl.cpu_id);

	    if(k != 1) {
		TRACEP(0, "=========== [ERR] Wrong -v option.\n");
		show_the_banner();
		return -1;
	    }
	} else if (cam4_rd.start_mode & m2_flag) {
	    k = inet_aton(val_str, &cam4_rd.mcast_cl.ipv4_device.sin_addr);
	    if(k == 0) {
		TRACEP(0, "=========== [ERR] Wrong -v option.\n");
		show_the_banner();
		return -1;
	    }

	};

	if((cam4_rd.start_mode & s_flag) && (cam4_rd.start_mode & f_flag)) {
	    TRACE(0, "You cannot use -s and -f at the same time \n");
	    return 0;
	}

	if( (cam4_rd.start_mode & (s_flag | f_flag)) && ((cam4_rd.start_mode & n_flag)==0 ) ) {
	    TRACE(0, "Incorrect using of -s or -f flag. You must set how much frames you are need via -n flag. \n");
	    return 0 ;
	}

	k = inet_aton(dst_str, (struct in_addr *)&cam4_cl->conn_cl.ip);
	if(k == 0) {
	    TRACEP(0, "=========== [ERR] Wrong -d option.\n");
	    show_the_banner();
	    return -1;
	}

	k = inet_aton(cam4_rd.camera_ip_str, (struct in_addr *)&cam4_cl->conn_ipv4.ip);
	if(k == 0) {
	    TRACEP(0, "=========== [ERR] Wrong -v option.\n");
	    show_the_banner();
	    return -1;
	}


	/* let's fly */
	TRACEP(0, "Ready-Steady-GOOOOOOOOOOOOO \n") ;

	/* MCAST prepare */
	if (cam4_rd.disable_mcast) {
		cam4_rd.raw_ifs = (mcast_cl_interface_t *)malloc(sizeof(mcast_cl_interface_t));
		inet_aton(val_str, (struct in_addr *)&cam4_rd.raw_ifs->conn_in.ip);
		cam4_rd.raw_ifs->conn_in.port = 10001;
		if (cam4_rd.disable_mcast == 1)
			raw_on(&cam4_rd, cam4_rd.raw_ifs);

		cam4_rd.cm_ifs = (mcast_cl_interface_t *)malloc(sizeof(mcast_cl_interface_t));
		inet_aton(val_str, (struct in_addr *)&cam4_rd.cm_ifs->conn_in.ip);
		cam4_rd.cm_ifs->conn_in.port = 10002;
		int res;
		if (cam4_rd.disable_mcast == 1) {
			CREATE_THREAD(res, process_cm_ifs, &cam4_rd, cam4_rd.cm_thread);
		}
		i = 0;
	} else
		i = cam4_mcast(&cam4_rd);

	if( i < 0 )
	    return -1;

	if( (cam4_rd.start_mode & g1_flag) ) {
		TRACEPNF(0, "Start the stream and exit\n");
		return 0;

	} else if( (cam4_rd.start_mode & g2_flag) ) {
		TRACEPNF(0, "Stop stream and exit\n");
		goto finish;
	}

	conn.fd = -1;
	if (create_connection(&conn))
		return 0;
// ++++++++++++++++++++++++++++++++++++++++++

#if 0
	/* make logfile */
	I = fopen("cam4_ps.log", "w+");
	if( I == NULL) {
		ETRACE("[%s]: [err] cannot log_file. errno: ", __func__);
		exit(-1);
	}
#endif

	i = 0;
	gettimeofday(&(cam4_rd.last_time),NULL);
	while(no_sig_exit) {
		while(*(unsigned long*)conn.ring[i].iov_base) {
			//printf("a");
			struct tpacket_hdr *h = conn.ring[i].iov_base;
			//struct sockaddr_ll *sll=(void *)h + TPACKET_ALIGN(sizeof(*h));
			//unsigned char *bp=(unsigned char *)h + h->tp_mac;
			struct	ip *iph = (struct ip *)((unsigned char *)h + h->tp_mac) ;
			uint8_t *data = (uint8_t *)iph + sizeof(struct ip);
/*
			TRACE(90, "%u.%.6u: if%u %s %u bytes\n",
				h->tp_sec, h->tp_usec,
				addr.sll_ifindex,
				names[addr.sll_pkttype],
				h->tp_len
			);
*/
			if(
			    (iph->ip_id == htons (54321)) &&
			    (iph->ip_p  == 253)
			) {
				/* check for the protocol type */
				cam4_rd.iph = iph;
				if(
				    (!cam4_rd.disable_mcast) ||
				    (iph->ip_dst.s_addr == cam4_rd.cam4_cl.conn_cl.ip) ||
				    (iph->ip_src.s_addr == cam4_rd.cam4_cl.conn_ipv4.ip)
				) {
					parse_packet(data, h, &cam4_rd);
				}
			}
			/* tell the kernel this packet is done with */
			h->tp_status=0;
			//mb(); /* memory barrier */

			i = (i==conn.req.tp_frame_nr-1) ? 0 : i+1;
		}

		/* Sleep when nothings happening */
		pfd[0].fd=conn.fd;
		pfd[0].events=POLLIN|POLLERR;
		pfd[0].revents=0;
		poll(pfd, 1, 1000);
		if (cam4_rd.mcast_reinit) {
			cam4_rd.mcast_reinit = 0;

			TRACEPNF(0,"Reinit stream\n");
			i = cam4_mcast(&cam4_rd);
			if( i < 0 )
				return -1;
		}
	}
finish:
	raw_off(&cam4_rd, NULL);

	TRACE_FLUSH();
	connection_free(&conn);
	if (shmid1 != -1) {
		shmctl(shmid1, IPC_RMID, NULL);	/* Destroy Region */
		shmdt(shmaddr[0]);
	}

	if (shmid2 != -1) {
		shmctl(shmid2, IPC_RMID, NULL);	/* Destroy Region */
		shmdt(shmaddr[1]);
	}

	if (shmid3 != -1) {
	    memset(shmaddr3, 0, sizeof(common_t));
	    shmctl(shmid3, IPC_RMID, NULL);	/* Destroy Region */
	    shmdt(shmaddr3);
	}

	return 0;
}

#ifndef CAM4_PS_LIB
int main(int argc, char *argv[])
{
	return cam4_ps_main(argc, argv);
}
#else
int cam4_ps_start(int argc, char **argv)
{
	return cam4_ps_main(argc, argv);
}

void cam4_ps_stop(void)
{
	no_sig_exit = 0;
}

void cam4_ps_set_cb(void *func)
{
	cam4_ps_cb = func;
}
#endif /* CAM4_PS_LIB */

