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
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>



#define TRACE_PRIVATE_PREFIX
#include <trace.h>
#undef  TRACE_LEVEL
#define TRACE_LEVEL 0



#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xv.h>
#include <X11/extensions/Xvlib.h>
#include <X11/extensions/XShm.h>


#include <sys/poll.h>
#include <sys/unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include <abi/p3.h>
#include <abi/p3_helpers.h>
#include <abi/io.h>
#include <abi/lb-task-bar.h>

#include <ui/osd-yuyv.h>

#include "shared_objects.h"

static char* trace_prefix = "cam4_ps_Xclient: ";
FILE* I;
extern int 	XShmQueryExtension(Display*);
extern int 	XShmGetEventBase(Display*);
extern XvImage  *XvShmCreateImage(Display*, XvPortID, int, char*, int, int, XShmSegmentInfo*);

static int show_hist = 1;
static float scale = 1.0;
static int show_cross = 0;
static int show_bars = 1;

common_t *common;

int fd = -1;
int debug = 0;
int get_params = 0;

uint8_t* shmaddr[2] = { };
uint8_t* shmaddr3;


static uint32_t CM0[] = {
     0x80808080,
//    0x80808080,
    0xffffffff,

    0x80808080,
    0xfefefefe,

    0x60606060,
    0xbbbbbbbb,

    0x10101010,
    0xfefefefe,

    0xffffffff,
    0xffffffff,

    0xffffffff,
    0xffffffff,

    0xffffffff,
    0xffffffff,
};

Bool waitForNotify( Display * dpy, XEvent *e, char *arg )
{
    TRACEPNF(0,"Ev: %d\n", e->type);
	return ( e->type == MapNotify ) && ( e->xmap.window == (Window)arg );
}


int send_command(char* cmd) {
	int nbytes = strlen(cmd);
	int res = write(fd,cmd,nbytes);
	if (res<0)
		TRACEPNF(0,"Cannot write to fifo\n");
	if (res != nbytes)
		TRACEPNF(0,"Cannot write the whole message to fifo\n");
	return 0;
}

int addCross(XvImage *yuv_image) {
	int dimx = yuv_image->width;
	int dimy = yuv_image->height;
	int x,y;
	if (show_cross & 1) {
		for (x=dimx/4;x<=dimx*3/4;x++) {
			yuv_image->data[1+2*(x+dimx*dimy/2)] = (uint8_t)255;
			yuv_image->data[2+2*(x+dimx*dimy/2)] = (uint8_t)0;;
		}
		for (y=dimy/4;y<=dimy*3/4;y++) {
			yuv_image->data[1+2*(dimx/2+dimx*y)] = (uint8_t)255;
			yuv_image->data[2+2*(dimx/2+dimx*y)] = (uint8_t)0;
		}
	}
	if (show_cross & 2) {
		for (x=dimx/4;x<=dimx*3/4;x++) {
			yuv_image->data[1+2*(x+dimx*dimy/4)] = (uint8_t)255;
			yuv_image->data[2+2*(x+dimx*dimy/4)] = (uint8_t)0;
			yuv_image->data[1+2*(x+dimx*dimy*3/4)] = (uint8_t)255;
			yuv_image->data[2+2*(x+dimx*dimy*3/4)] = (uint8_t)0;
		}
		for (y=dimy/4;y<=dimy*3/4;y++) {
			yuv_image->data[1+2*(dimx/4+dimx*y)] = (uint8_t)255;
			yuv_image->data[2+2*(dimx/4+dimx*y)] = (uint8_t)0;
			yuv_image->data[1+2*(dimx*3/4+dimx*y)] = (uint8_t)255;
			yuv_image->data[2+2*(dimx*3/4+dimx*y)] = (uint8_t)0;
		}
	}
	return 0;
}

int addBars(XvImage *yuv_image, p3_2d_id_bars_t *s,osd_ctx_t* osd)
{
	int		i;
	int		x,y;

	int		dimx = yuv_image->width;
	int		dimy = yuv_image->height;

	p3_2d_id_bar_t	*bars = s->bars;

	if (s->items > 35)
		s->items = 0;

	for (i=0; i<s->items; i++, bars++) {
		int x0 = bars->x0;
		int y0 = bars->y0;
		int x1 = bars->x1;
		int y1 = bars->y1;

		if (x0 < 0) x0 = 0;
		if (y0 < 0) y0 = 0;
		if (x1 < 0) x1 = 0;
		if (y1 < 0) y1 = 0;

		if (x0 >= dimx) x0 = dimx - 1;
		if (x1 >= dimx) x1 = dimx - 1;
		if (y0 >= dimy) y0 = dimy - 1;
		if (y1 >= dimy) y1 = dimy - 1;

		if(x0>x1) {
			x = x1; x1 = x0; x0 = x;
		}

		if(y0>y1) {
			y = y1; y1 = y0; y0 = y;
		}

		TRACEPNF(0,"BAR:%d [%08x:%02x] {%4d, %4d, %4d, %4d}\n",
		    i,
		    bars->id, bars->state,
		    x0, y0, x1, y1
		);

		//draw bar
		for (x=x0;x<=x1;x++)
			yuv_image->data[1+2*(x+dimx*y0)] = (uint8_t)255;

		for (x=x0;x<=x1;x++)
			yuv_image->data[1+2*(x+dimx*y1)] = (uint8_t)255;

		for (y=y0;y<=y1;y++)
			yuv_image->data[1+2*(x0+dimx*y)] = (uint8_t)255;

		for (y=y0;y<=y1;y++)
			yuv_image->data[1+2*(x1+dimx*y)] = (uint8_t)255;
		char id_str[50];
		sprintf(id_str,"%d",bars->id);
		if(dimy <= (y0+33))
			y0 = y0 - 33;

		osd->puts_xy(osd,x0,y0,id_str);
	}

	return 0;
}

/*
int addHist(XvImage *yuv_image) {
	int num_bins = 256;
	int* hist = (int*)malloc(num_bins*sizeof(int));
	memset(hist, 0, num_bins*sizeof(int));
	int i;
	int x,y;
	int maxval = 0;
	int dimx = yuv_image->width;
	//    CrYCbY format
	int size = yuv_image->width * yuv_image->height;
	for (i=0;i<size;i++) {
		uint8_t val = yuv_image->data[2*i+1];    // Y
		uint32_t bin = val*num_bins/256;
		hist[bin]++;
	}

	for (i=0;i<num_bins;i++)
		if (hist[i]>maxval)
			maxval = hist[i];
	//histogram ready

	int hist_width = num_bins*2;
	int hist_height = yuv_image->height * 0.2;
	int hist_offx = yuv_image->width * 0.1;
	int hist_offy = yuv_image->height * 0.1;

	//drawing histogram
	for (x=hist_offx;x<hist_offx+hist_width;x++) {
		uint32_t bin = (x-hist_offx)*num_bins/hist_width;
		int y_bound = hist_offy+hist_height - (hist_height)*hist[bin]*scale/maxval;
		if (y_bound < hist_offy)
			y_bound = hist_offy;
		for (y=hist_height+hist_offy;y>y_bound;y--) {
			yuv_image->data[1+2*(x+dimx*y)] = (uint8_t)255;
			yuv_image->data[2*(x+dimx*y)] = (uint8_t)255;
		}
	}

	//drawing boundaries
	for (x=hist_offx;x<=hist_offx+hist_width;x++)
		yuv_image->data[1+2*(x+dimx*hist_offy)] ^= 0xFF;
	for (x=hist_offx;x<=hist_offx+hist_width;x++)
		yuv_image->data[1+2*(x+dimx*(hist_offy+hist_height))] ^= 0xFF;

	for (y=hist_offy;y<=hist_offy+hist_height;y++)
		yuv_image->data[1+2*(hist_offx+dimx*y)] ^= 0xFF;
	for (y=hist_offy;y<=hist_offy+hist_height;y++)
		yuv_image->data[1+2*(hist_offx+hist_width+dimx*y)] ^= 0xFF;
	//drawing grid
	for (i=1;i<10;i++)
		for (x=hist_offx;x<=hist_offx+hist_width;x++)
			yuv_image->data[1+2*(x+dimx*(hist_offy+hist_height*i/10))] ^= 0xFF;
	for (i=1;i<16;i++)
		for (y=hist_offy;y<=hist_offy+hist_height;y++)
			yuv_image->data[1+2*(hist_offx+hist_width*i/16+dimx*y)] ^= 0xFF;
	//drawing text



	free(hist);
	return 0;
}*/

int add_comp_hist(XvImage *yuv_image,common_t* common)
{
////////////////////////////////////////////////////
	int i,j;
	int x,y;
	int maxval = 0;
	int dimx = yuv_image->width;
	for (j=0;j<4;j++) {
		int num_bins = common->comp_hist[j].nbins;
		uint32_t* hist = common->comp_hist[j].hist;

		for (i=0;i<num_bins;i++)
			if (hist[i]>maxval)
				maxval = hist[i];
	}

	static int X[]={ 20, 600,  20, 600 };
	static int Y[]={ 20,  20, 300, 300 };

	for (j=0;j<4;j++) {
		int num_bins = common->comp_hist[j].nbins;
		uint32_t* hist = common->comp_hist[j].hist;

		//histogram ready

		int hist_width	= num_bins*2;
		int hist_height = yuv_image->height * 0.2;

		int hist_offx = yuv_image->width * (0.04+j*0.21);
		int hist_offy = yuv_image->height * 0.1;

		hist_offx	= X[j];
		hist_offy	= Y[j];

		//drawing histogram
		for (x=hist_offx;x<hist_offx+hist_width;x++) {
			uint32_t bin = (x-hist_offx)*num_bins/hist_width;
			int y_bound = hist_offy+hist_height - (hist_height)*hist[bin]*scale/maxval;
			if (y_bound < hist_offy)
				y_bound = hist_offy;
			for (y=hist_height+hist_offy;y>y_bound;y--) {
				yuv_image->data[1+2*(x+dimx*y)] = (uint8_t)200;
				yuv_image->data[2*(x+dimx*y)] = (uint8_t)100;
			}
		}

		//drawing boundaries
		for (x=hist_offx;x<=hist_offx+hist_width;x++)
			yuv_image->data[1+2*(x+dimx*hist_offy)] ^= 0xFF;
		for (x=hist_offx;x<=hist_offx+hist_width;x++)
			yuv_image->data[1+2*(x+dimx*(hist_offy+hist_height))] ^= 0xFF;

		for (y=hist_offy;y<=hist_offy+hist_height;y++)
			yuv_image->data[1+2*(hist_offx+dimx*y)] ^= 0xFF;
		for (y=hist_offy;y<=hist_offy+hist_height;y++)
			yuv_image->data[1+2*(hist_offx+hist_width+dimx*y)] ^= 0xFF;
		//drawing grid
		for (i=1;i<10;i++)
			for (x=hist_offx;x<=hist_offx+hist_width;x++)
				yuv_image->data[1+2*(x+dimx*(hist_offy+hist_height*i/10))] ^= 0xFF;
		for (i=1;i<16;i++)
			for (y=hist_offy;y<=hist_offy+hist_height;y++)
				yuv_image->data[1+2*(hist_offx+hist_width*i/16+dimx*y)] ^= 0xFF;

	}
	return 0;
}


int add_hist_from_oob(XvImage *yuv_image,int num_bins,uint32_t* hist) {
	int i;
	int x,y;
	int maxval = 0;
	int dimx = yuv_image->width;

	for (i=0;i<num_bins;i++)
		if (hist[i]>maxval)
			maxval = hist[i];
	//histogram ready

	int hist_width = num_bins*2;
	int hist_height = yuv_image->height * 0.2;
	int hist_offx = yuv_image->width * 0.1;
	int hist_offy = yuv_image->height * 0.1;

	//drawing histogram
	for (x=hist_offx;x<hist_offx+hist_width;x++) {
		uint32_t bin = (x-hist_offx)*num_bins/hist_width;
		int y_bound = hist_offy+hist_height - (hist_height)*hist[bin]*scale/maxval;
		if (y_bound < hist_offy)
			y_bound = hist_offy;
		for (y=hist_height+hist_offy;y>y_bound;y--) {
			yuv_image->data[1+2*(x+dimx*y)] = (uint8_t)200;
			yuv_image->data[2*(x+dimx*y)] = (uint8_t)100;
		}
	}

	//drawing boundaries
	for (x=hist_offx;x<=hist_offx+hist_width;x++)
		yuv_image->data[1+2*(x+dimx*hist_offy)] ^= 0xFF;
	for (x=hist_offx;x<=hist_offx+hist_width;x++)
		yuv_image->data[1+2*(x+dimx*(hist_offy+hist_height))] ^= 0xFF;

	for (y=hist_offy;y<=hist_offy+hist_height;y++)
		yuv_image->data[1+2*(hist_offx+dimx*y)] ^= 0xFF;
	for (y=hist_offy;y<=hist_offy+hist_height;y++)
		yuv_image->data[1+2*(hist_offx+hist_width+dimx*y)] ^= 0xFF;
	//drawing grid
	for (i=1;i<10;i++)
		for (x=hist_offx;x<=hist_offx+hist_width;x++)
			yuv_image->data[1+2*(x+dimx*(hist_offy+hist_height*i/10))] ^= 0xFF;
	for (i=1;i<16;i++)
		for (y=hist_offy;y<=hist_offy+hist_height;y++)
			yuv_image->data[1+2*(hist_offx+hist_width*i/16+dimx*y)] ^= 0xFF;


	return 0;
}

int add_subwindow(XvImage* yuv_image, int startx, int starty, int totalx, int totaly) {
	int x,y;
	startx -= startx%16;
	starty -= starty%16;
	for (x=-300; x<-100;x++) {
		yuv_image->data[(yuv_image->height-300)*yuv_image->width*2+x*2+1] = (uint8_t) 255;
		yuv_image->data[(yuv_image->height-100)*yuv_image->width*2+x*2+1] = (uint8_t) 255;
	}
	for (y=-300; y<-100;y++) {
		yuv_image->data[(yuv_image->height+y)*yuv_image->width*2-100*2+1] = (uint8_t) 255;
		yuv_image->data[(yuv_image->height+y)*yuv_image->width*2-300*2+1] = (uint8_t) 255;
	}
	//printf("%d %d %d %d %d %d\n",startx,starty,yuv_image->width, yuv_image->height,totalx,totaly);
	for (x=-300+200*startx/totalx; x<-300+200*(startx + yuv_image->width)/totalx;x++) {
		yuv_image->data[(yuv_image->height-300+200*starty/totaly)*yuv_image->width*2+x*2+1] = (uint8_t) 255;
		yuv_image->data[(yuv_image->height-300+200*(starty+yuv_image->height)/totaly)*yuv_image->width*2+x*2+1] = (uint8_t) 255;
	}
	for (y=-300+200*starty/totaly; y<-300+200*(starty+yuv_image->height)/totaly;y++) {
		yuv_image->data[(yuv_image->height+y)*yuv_image->width*2+(-300+200*startx/totalx)*2+1] = (uint8_t) 255;
		yuv_image->data[(yuv_image->height+y)*yuv_image->width*2+(-300+200*(startx + yuv_image->width)/totalx)*2+1] = (uint8_t) 255;
	}
	return 0;
}

void sigproc(int);

static int	xv_port = -1;
static int	xv_offs = 0;

typedef struct {
	uint32_t	exp;
	uint32_t	period;

	double		gains[5];

	uint32_t	reg1;
	uint32_t	reg2;

	uint16_t	stddev[4];
	uint16_t	mean[4];
	int16_t		diff[4];

} cam_stat_t;

void redraw_params(
	Display		*dpy,
	Window		winfo,
	GC		gcinfo,
	cam_stat_t	*stat
)
{
	char st[50];
	sprintf(st,"Exposure %d mks", stat->exp);
	XDrawString(dpy, winfo, gcinfo, 20, 30, st, strlen(st));

	sprintf(st,"Period %d mks", stat->period);
	XDrawString(dpy, winfo, gcinfo, 20, 50, st, strlen(st));

	int	y = 70;
	int	i;

	for (i=0; i<ARRAY_ELEMS(stat->gains); i++, y+=20) {
		sprintf(st,"Gains[%d] %lf", i, stat->gains[i]);
		XDrawString(dpy, winfo, gcinfo, 20, y, st, strlen(st));
	}

	sprintf(st,"Reg1 0x%08x", stat->reg1);
	XDrawString(dpy, winfo, gcinfo, 20, 170, st, strlen(st));

	sprintf(st,"Reg2 0x%08x", stat->reg2);
	XDrawString(dpy, winfo, gcinfo, 20, 190, st, strlen(st));


	uint16_t dx = 10, dy = 220, w=180, h=120;
	XDrawRectangle(dpy, winfo, gcinfo, dx, dy, w, h);

	XDrawLine(dpy, winfo, gcinfo, dx+w/2, dy, dx+w/2, dy+h);
	XDrawLine(dpy, winfo, gcinfo, dx, dy+h/2, dx+w , dy+h/2);

	sprintf(st,"Mean: %d", stat->mean[0]);
	XDrawString(dpy, winfo, gcinfo, dx+15    , dy+25    , st, strlen(st));
	sprintf(st,"Mean: %d",stat->mean[1]);
	XDrawString(dpy, winfo, gcinfo, dx+15+w/2, dy+25    , st, strlen(st));
	sprintf(st,"Mean: %d",stat->mean[2]);
	XDrawString(dpy, winfo, gcinfo, dx+15    , dy+25+h/2, st, strlen(st));
	sprintf(st,"Mean: %d",stat->mean[3]);
	XDrawString(dpy, winfo, gcinfo, dx+15+w/2, dy+25+h/2, st, strlen(st));
	sprintf(st,"Dev : %d", stat->stddev[0]);
	XDrawString(dpy, winfo, gcinfo, dx+15    , dy+45    , st, strlen(st));
	sprintf(st,"Dev : %d",stat->stddev[1]);
	XDrawString(dpy, winfo, gcinfo, dx+15+w/2, dy+45    , st, strlen(st));
	sprintf(st,"Dev : %d",stat->stddev[2]);
	XDrawString(dpy, winfo, gcinfo, dx+15    , dy+45+h/2, st, strlen(st));
	sprintf(st,"Dev : %d",stat->stddev[3]);
	XDrawString(dpy, winfo, gcinfo, dx+15+w/2, dy+45+h/2, st, strlen(st));

	dx = 200; dy = 220, w=180, h=120;
	XDrawRectangle(dpy, winfo, gcinfo, dx, dy, w, h);

	XDrawLine(dpy, winfo, gcinfo, dx, dy, dx+w, dy+h);
	XDrawLine(dpy, winfo, gcinfo, dx, dy+h, dx+w , dy);

    sprintf(st,"Diff: %d", stat->diff[0]);
	XDrawString(dpy, winfo, gcinfo, dx-30  +w/2, dy-15+  h*2/7,st, strlen(st));
	sprintf(st,"Diff: %d",stat->diff[1]);
	XDrawString(dpy, winfo, gcinfo, dx-30  +w/2, dy+10+  h*6/7, st, strlen(st));
	sprintf(st,"Diff: %d",stat->diff[2]);
	XDrawString(dpy, winfo, gcinfo, dx-40  +w*2/7,dy+5  +h/2, st, strlen(st));
	sprintf(st,"Diff: %d",stat->diff[3]);
	XDrawString(dpy, winfo, gcinfo, dx-40  +w*6/7,dy+5  +h/2, st, strlen(st));


	// try to draw hist
	//{
        //int x = 410, y=10;
        //XDrawLine(dpy, winfo, gcinfo, 0, 0, 100, 100);
	//}

}

void draw_params(
	Display		*dpy,
	Window		winfo,
	GC		gcinfo,
	int		screen,
	common_t	*common
)
{

	static cam_stat_t last = {};
#if 0
	for (i=0;i<256;i++) {
		XSetForeground(dpy, gc, i<<24);
		XDrawRectangle(dpy, window, gc, 200+i, 400+i, 514-2*i, 514-2*i);
	}
#endif
	XSetForeground(dpy, gcinfo, BlackPixel(dpy, screen));
	redraw_params(dpy, winfo, gcinfo, &last);

	last.exp	= common->exp/10;
	last.period	= common->period/10;


	int		i;
	for (i=0; i<ARRAY_ELEMS(last.gains); i++)
		last.gains[i]	= common->gains[i];

	last.reg1	= common->reg1;
	last.reg2	= common->reg2;

	for (i=0;i<4;i++){
	    last.mean[i] 	= common->mean[i];
	    last.stddev[i] 	= common->stddev[i];
		last.diff[i] 	= common->diff[i];
	}


	XSetForeground(dpy, gcinfo, WhitePixel(dpy, screen));
	redraw_params(dpy, winfo, gcinfo, &last);

#if 0
	XSetForeground(dpy, gc, WhitePixel(dpy, screen));
	XDrawRectangle(dpy, window, gc, 20+0, 190+0, 200-1, 20-0);
	XDrawRectangle(dpy, window, gc, 20+2, 190+2, 200-4, 20-4);
	XSetForeground(dpy, gc, BlackPixel(dpy, screen));
	XDrawRectangle(dpy, window, gc, 20+1, 190+1, 200-2, 20-2);
#endif
}

int read_xv_buf(common_t* common)
{
	XvImage *yuv_image_ar[2];
	XvImage *yuv_image;


//	int		adaptor;
	int		attributes, formats;
	unsigned	encodings, _du, _w, _h;
	int		i, j, ret, p, _di;

	XvAdaptorInfo		*ai;
	XvEncodingInfo	*ei;
	XvAttribute		*at;
	XvImageFormatValues	*fo;


	unsigned int		p_version, p_release,
			    p_request_base, p_event_base, p_error_base;
	unsigned int	p_num_adaptors;

	Display		*dpy;
	Window		window;
	Window		winfo;
	Window		_dw;	/* Root link */
//	XSizeHints		hint;
	XSetWindowAttributes	xswa;
	XVisualInfo		vinfo;
	int			screen;
	unsigned long		mask;
	XEvent		event;
	GC			gc;
	GC			gcinfo;
//	XFontSet	fontset;

	/* for shm */
	int 			shmem_flag = 0;
	XShmSegmentInfo	yuv_shminfo1 = {}, yuv_shminfo2 ={};
//	int			CompletionType;

	TRACE(3,"starting up video testapp...\n\n");

//	adaptor = -1;

	dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
	    TRACE(3,"Cannot open Display.\n");
	    exit (-1);
	}

	/**--------------------------------- XV ------------------------------------*/
	TRACE(3,"beginning to parse the Xvideo extension...\n\n");

	/** query and print Xvideo properties */
	ret = XvQueryExtension(dpy, &p_version, &p_release, &p_request_base,
			     &p_event_base, &p_error_base);

	if (ret != Success) {
	    if (ret == XvBadExtension)
		TRACE(3,"XvBadExtension returned at XvQueryExtension.\n");
	    else if (ret == XvBadAlloc)
		TRACE(3,"XvBadAlloc returned at XvQueryExtension.\n");
	    else
	    TRACE(3,"other error happened at XvQueryExtension.\n");
	}

	TRACE(3,"========================================\n");
	TRACE(3,"XvQueryExtension returned the following:\n");
	TRACE(3,"p_version      : %u\n", p_version);
	TRACE(3,"p_release      : %u\n", p_release);
	TRACE(3,"p_request_base : %u\n", p_request_base);
	TRACE(3,"p_event_base   : %u\n", p_event_base);
	TRACE(3,"p_error_base   : %u\n", p_error_base);
	TRACE(3,"========================================\n");

	ret = XvQueryAdaptors(dpy, DefaultRootWindow(dpy),
			    &p_num_adaptors, &ai);

	if (ret != Success) {
	    if (ret == XvBadExtension)
		TRACE(3,"XvBadExtension returned at XvQueryExtension.\n");
	    else if (ret == XvBadAlloc)
		TRACE(3,"XvBadAlloc returned at XvQueryExtension.\n");
	    else
		TRACE(3,"other error happaned at XvQueryAdaptors.\n");
	}

	TRACE(3,"=======================================\n");
	TRACE(3,"XvQueryAdaptors returned the following:\n");
	TRACE(3,"%d adaptors available.\n", p_num_adaptors);
	for (i = 0; i < p_num_adaptors; i++) {
	    TRACE(3,	" name:        %s\n"
			    " type:        %s%s%s%s%s\n"
			    " ports:       %ld\n"
			    " first port:  %ld\n",
			    ai[i].name,
			    (ai[i].type & XvInputMask)	? "input | "	: "",
			    (ai[i].type & XvOutputMask)	? "output | "	: "",
			    (ai[i].type & XvVideoMask)	? "video | "	: "",
			    (ai[i].type & XvStillMask)	? "still | "	: "",
			    (ai[i].type & XvImageMask)	? "image | "	: "",
			    ai[i].num_ports,
			    ai[i].base_id
		    );

//	    TRACE(3,"adaptor %d ; format list:\n", i);

	    for (j = 0; j < ai[i].num_formats; j++) {
		TRACE(3," depth=%d, visual=%ld\n",
		ai[i].formats[j].depth,
		ai[i].formats[j].visual_id);
	    }

	    for (p = ai[i].base_id; p < ai[i].base_id+ai[i].num_ports; p++) {

		TRACE(3," encoding list for port %d\n", p);
		if (XvQueryEncodings(dpy, p, &encodings, &ei) != Success) {
		    TRACE(3,"XvQueryEncodings failed.\n");
		    continue;
		}

		for (j = 0; j < encodings; j++) {
		    TRACE(3,"  id=%ld, name=%s, size=%ldx%ld, numerator=%d, denominator=%d\n",
				ei[j].encoding_id, ei[j].name, ei[j].width, ei[j].height,
				ei[j].rate.numerator, ei[j].rate.denominator);
			if (debug) TRACE(0,"%ldx%ld\n",ei[j].width, ei[j].height);
		}
		if (debug) return 0;
		common->xvWidth = ei[0].width;
		common->xvHeight = ei[0].height;
		//common->xvWidth = 800;
		//common->xvHeight = 600;
		if (common->xvWidth < common->sensWidth)
			common->width = common->xvWidth;
		else
			common->width = common->sensWidth;
		if (common->xvHeight < common->sensHeight)
			common->height = common->xvHeight;
		else
			common->height = common->sensHeight;
		XvFreeEncodingInfo(ei);

		at = XvQueryPortAttributes(dpy, p, &attributes);

		for (j = 0; j < attributes; j++) {
			TRACE(3,	"  attr: %15s %cget %cset { %+6i %+6i }%s",
					at[j].name,
					(at[j].flags & XvGettable) ?'+' :'-',
					(at[j].flags & XvSettable) ?'+' :'-',
					at[j].min_value, at[j].max_value,
					at[j].flags & XvGettable ?" " :"\n"
			);

			if(at[j].flags & XvGettable) {
				int val;
				val = 0;
				Atom attr = XInternAtom(dpy, at[j].name, False);
				XvGetPortAttribute(dpy, p, attr, &val);
				TRACE(3, "%d\n", val);
			}
		}

		if (at)
		    XFree(at);

		TRACE(3," image format list for port %d\n", p);
		fo = XvListImageFormats(dpy, p, &formats);

		for (j = 0; j < formats; j++) {
			TRACE(3,"  0x%x (%4.4s) %s\n",
			    fo[j].id,
			    (char *)&fo[j].id,
			    (fo[j].format == XvPacked) ? "packed" : "planar");
		}

		if (fo)
		    XFree(fo);
	    }

	    TRACE(3,"\n");
	}

	if (p_num_adaptors > 0) {

		if (xv_port == -1)
			xv_port = ai[0].base_id;
		XvFreeAdaptorInfo(ai);

	}

	TRACEPNF(0,"Xv port = %d\n",xv_port);

    screen = DefaultScreen(dpy);

    /* find best display */
    if (XMatchVisualInfo(dpy, screen, 24, TrueColor, &vinfo)) {
	TRACE(3," found 24bit TrueColor\n");
    } else if (XMatchVisualInfo(dpy, screen, 16, TrueColor, &vinfo)) {
	TRACE(3," found 16bit TrueColor\n");
    } else if (XMatchVisualInfo(dpy, screen, 15, TrueColor, &vinfo)) {
	TRACE(3," found 15bit TrueColor\n");
    } else if (XMatchVisualInfo(dpy, screen, 8, PseudoColor, &vinfo)) {
	TRACE(3," found 8bit PseudoColor\n");
    } else if (XMatchVisualInfo(dpy, screen, 8, GrayScale, &vinfo)) {
	TRACE(3," found 8bit GrayScale\n");
    } else if (XMatchVisualInfo(dpy, screen, 8, StaticGray, &vinfo)) {
	TRACE(3," found 8bit StaticGray\n");
    } else if (XMatchVisualInfo(dpy, screen, 1, StaticGray, &vinfo)) {
	TRACE(3," found 1bit StaticGray\n");
    } else {
	TRACE(3,"requires 16 bit display\n");
	exit (-1);
    }

//    CompletionType = -1;
#if 0
    hint.x = 1;
    hint.y = 1;
    hint.width = common->sensWidth;
    hint.height = common->sensHeight;
    hint.flags = PPosition | PSize;
#endif
    xswa.colormap =  XCreateColormap(dpy, DefaultRootWindow(dpy), vinfo.visual, AllocNone);
    xswa.event_mask = StructureNotifyMask | ExposureMask;
    xswa.background_pixel = 0;
    xswa.border_pixel = 0;

    mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

	window = XCreateWindow(dpy, DefaultRootWindow(dpy),
			     0, 0,
			     common->width,
			     common->height,
			     0, vinfo.depth,
			     InputOutput,
			     vinfo.visual,
			     mask, &xswa);

	winfo = XCreateWindow(dpy, DefaultRootWindow(dpy),
			     0, 0,
			     400,
			     1000,
			     0, vinfo.depth,
			     InputOutput,
			     vinfo.visual,
			     mask, &xswa
	);

	//sprintf(cam4_rd->window_title, "ip:%s  dev_name:%s ",cam4_rd->camera_ip_str,cam4_rd->device_name);
    XStoreName(dpy, winfo, "Stat");
    XStoreName(dpy, window, common->window_title);

    XSetIconName(dpy, window, "cam4_stream");
    XSetIconName(dpy, winfo, "cam4 stat");

    XSelectInput(dpy, window, StructureNotifyMask|KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask|Button1MotionMask);

    /** Map window */
    XMapWindow(dpy, window);
    XMapWindow(dpy, winfo);
    XRaiseWindow(dpy, winfo);

    TRACE(0,"MAP WINDOW\n");
    /** Wait for map. */
    XIfEvent( dpy, &event, waitForNotify, (char *)window );

    if (XShmQueryExtension(dpy))
	shmem_flag = 1;

    if (!shmem_flag) {
	TRACE(3,"no shmem available.\n");
	exit (-1);
    }

//    if (shmem_flag==1)
//	CompletionType = XShmGetEventBase(dpy) + ShmCompletion;


    gc     = XCreateGC(dpy, window, 0, 0);
    gcinfo = XCreateGC(dpy, winfo,  0, 0);

//	char ** missing = 0;
//    int nmissing;
//    char * def = 0;
//	fontset = XCreateFontSet(dpy,"-*-fixed-*-*-*-*-13-*-*-*-*-*-*-*", &missing, &nmissing, &def);

    yuv_image_ar[0] = XvShmCreateImage(dpy, xv_port, 0x59565955, 0, common->width, common->height, &yuv_shminfo1);
    yuv_image = yuv_image_ar[0];
    yuv_shminfo1.shmid = shmget(key_yuv1, 0, 0);
    if(yuv_shminfo1.shmid<0)
	    yuv_shminfo1.shmid = shmget(key_yuv1, common->sensWidth * common->sensHeight * 2+4096, IPC_EXCL |IPC_CREAT | 0666);
    yuv_shminfo1.shmaddr = yuv_image->data = shmat(yuv_shminfo1.shmid, 0, 0);
    yuv_shminfo1.readOnly = False;
	shmaddr[0] = (uint8_t*)yuv_shminfo1.shmaddr;
    if (!XShmAttach(dpy, &yuv_shminfo1)) {
		TRACE(3,"XShmAttach failed !\n");
		exit (-1);
    }

    TRACE(0, "shmem id1 = %d \n", yuv_shminfo1.shmid );

    yuv_image_ar[1] = XvShmCreateImage(dpy, xv_port, 0x59565955, 0, common->width, common->height, &yuv_shminfo2);
    yuv_image = yuv_image_ar[1];
    yuv_shminfo2.shmid = shmget(key_yuv2, 0, 0);
    if(yuv_shminfo2.shmid < 0)
	    yuv_shminfo2.shmid = shmget(key_yuv2, common->sensWidth * common->sensHeight * 2 +4096, IPC_CREAT |IPC_EXCL| 0666);
    yuv_shminfo2.shmaddr = yuv_image->data = shmat(yuv_shminfo2.shmid, 0, 0);
    yuv_shminfo2.readOnly = False;
    shmaddr[1] = (uint8_t*)yuv_shminfo2.shmaddr;
    if (!XShmAttach(dpy, &yuv_shminfo2)) {
		TRACE(0,"XShmAttach failed !\n");
		exit (-1);
    }
    //TRACEPNF(0,"yuv image 1 ptr =============  %d\n",(uint32_t)yuv_image_ar[0]->data);
    //TRACEPNF(0,"yuv image 2 ptr =============  %d\n",(uint32_t)yuv_image_ar[1]->data);
	TRACE(0, "shmem id2 = %d \n", yuv_shminfo2.shmid );
	TRACE(0, "Shared size %d\n", yuv_image->data_size);

	yuv_image = yuv_image_ar[0];
	int show_subwindow = 0;
	int offx = 0;
	int offy = 0;

	osd_ctx_t osd[2];
	memset(&(osd[0]),0,2*sizeof(osd_ctx_t));
	uint8_t* font = (uint8_t*)malloc(4096);
	osd_font_read("koi8r-8x16",font);
	osd_yuyv_init(&(osd[0]), yuv_image_ar[0]->data, font, CM0, common->width, common->height);
	osd_yuyv_init(&(osd[1]), yuv_image_ar[1]->data, font, CM0, common->width, common->height);

	while (1) {
		while (!common->frame_done) {
			usleep(5000);
			int keycode = 0;
		/* check for events pending */
			while (XPending(dpy)) {
				XNextEvent(dpy, &event);
				if (event.type == ButtonPress) {
					if (common->sensHeight > common->xvHeight || common->sensWidth > common->xvWidth) {
						show_subwindow = 1;
						offx = event.xbutton.x;
						offy = event.xbutton.y;
					}
				}
				if (event.type == ButtonRelease) {
					show_subwindow = 0;
				}
				if (event.type == MotionNotify && show_subwindow) {
					common->startx += (event.xmotion.x-offx)*2;
					common->starty += (event.xmotion.y-offy)*2;
					offx = event.xmotion.x;
					offy = event.xmotion.y;
					if (common->startx < 0)
						common->startx = 0;
					if (common->starty < 0)
						common->starty = 0;
					if (common->startx > common->sensWidth - yuv_image->width )
						common->startx = common->sensWidth - yuv_image->width ;
					if (common->starty > common->sensHeight - yuv_image->height )
						common->starty = common->sensHeight - yuv_image->height ;
				}
				if(event.type == KeyPress) {
					keycode = event.xkey.keycode;
					if (keycode == 9) {
						sigproc(0);
					}
					if (keycode == 54) {        //c
						TRACEPNF(0, "========= Pressed %08x\n", event.xkey.keycode);
						show_cross ++;
						if (show_cross == 4)
							show_cross = 0;
					}

					if (keycode == 86) {       //up
						TRACEPNF(0, "========= Pressed %08x\n", event.xkey.keycode);
						scale *= 1.2;
					}
					if (keycode == 82) {      //down
						TRACEPNF(0, "========= Pressed %08x\n", event.xkey.keycode);
						scale /= 1.2;
					}
					if (keycode == 90) {     //end
						TRACEPNF(0, "========= Pressed %08x\n", event.xkey.keycode);
						scale = 1.0;
					}

					if (keycode == 65) {    //space
						TRACEPNF(0, "========= Pressed %08x\n", event.xkey.keycode);
						send_command("DUMPRAW:0.1");
					}



					if (keycode == 39) {   //s
						TRACEPNF(0, "========= Pressed %08x\n", event.xkey.keycode);
						send_command("VIDEO:START");
					}
					if (keycode == 40) {    //d
						TRACEPNF(0, "========= Pressed %08x\n", event.xkey.keycode);
						send_command("DUMPYUV:0.1");
					}
					if (keycode == 41) {    //f
						TRACEPNF(0, "========= Pressed %08x\n", event.xkey.keycode);
						send_command("VIDEO:FINISH");
					}
					if (keycode == 42) {    //g
						TRACEPNF(0, "========= Pressed %08x\n", event.xkey.keycode);
						send_command("TEST:BLACK"); // Do nothing
					}
					if (keycode == 43) {   //h
						TRACEPNF(0, "========= Pressed %08x\n", event.xkey.keycode);
						show_hist++;
						if (show_hist > 2)
							show_hist = 0;
					}
					if (keycode == 44) {  // j
						TRACEPNF(0, "========= Pressed %08x\n", event.xkey.keycode);
						send_command("RAWVIDEO:START");
					}
					if (keycode == 45) { // k
						TRACEPNF(0, "========= Pressed %08x\n", event.xkey.keycode);
						send_command("RAWVIDEO:FINISH");
					}

					if (keycode == 56) {   //b
						TRACEPNF(0, "========= Pressed %08x\n", event.xkey.keycode);
						show_bars ^= 0xFF;
					}


					if (keycode == 27) {    //r
						TRACEPNF(0, "========= Pressed %08x\n", event.xkey.keycode);
						send_command("REINIT:");
					}






				}
			}
		}
		if (get_params)
			send_command("GET:");
		int j = common->frame_idx_done;
		yuv_image = yuv_image_ar[j];

		TRACEPNF(90, "ptr = [%p] \n", yuv_image);
		XGetGeometry(dpy, window, &_dw, &_di, &_di, &_w, &_h, &_du, &_du);

		if (show_hist == 1)
			add_comp_hist(yuv_image,common);
		else if (show_hist == 2)
			add_hist_from_oob(yuv_image,common->nbins,common->hist);

		if (show_bars) {
			p3_2d_id_bars_t		*s;
			s = (void*)shmaddr[common->frame_idx_done & 1] + common->image_size;

			addBars(yuv_image, s, osd+j);
			s->items = 0;
		}
		if (show_cross)
			addCross(yuv_image);
		if (show_subwindow)
			add_subwindow(yuv_image, common->startx, common->starty, common->sensWidth, common->sensHeight);

		if (0 && get_params) {
			int x,y;
			int dimx = yuv_image->width;
			for (x=15;x<200;x++) {
				for (y=35;y<385;y++) {
					yuv_image->data[1+2*(x+dimx*y)] = (uint8_t)255;

				}
			}

		}
		//osd[j].refresh(osd+j,0);

		XvShmPutImage(dpy, xv_port, window, gc, yuv_image,
				xv_offs, xv_offs, yuv_image->width, yuv_image->height,
				0, 0, _w, _h, False);

		if (get_params)
			draw_params(dpy, winfo, gcinfo, screen, common);

		XSync(dpy, True);
		common->frame_done = 0 ;
	}

	return 0;
}


void sigproc(int sig) {
	if (fd>=0) {
		close(fd);
		fd = -1;
	}
	common->xvHeight = common->xvWidth = 0;
	shmdt(shmaddr[0]);
	shmdt(shmaddr[1]);
	shmdt(shmaddr3);
	exit(-1);
}

int show_help(void) {
	TRACE(0,"Press space to dump raw frame\n"
		"Press \'d\' to dump yuv frame\n"
		"Press \'j\' to start recording group of raw frames\n"
		"Press \'k\' to finish recording group of raw frames\n"
		"Press \'h\' to show/hide histogram\n"
		"Press \'r\' to reinitialize stream capturing\n"
		"Press \'s\' to start recording group of yuv frames\n"
		"Press \'f\' to finish recording group of yuv frames\n"
		"Press \'up\' to increase histogram scale\n"
		"Press \'down\' to decrease histogram scale\n"
		"Press \'end\' to reset histogram scale\n"
		"Press \'c\' to show cross\n"
		"\n");
	return 0;
}

int main(int argc, char** argv) {
	int i;

	I = stdout;
	fd = open("/tmp/cam4.fifo", O_WRONLY | O_NONBLOCK,0x666);
	signal(SIGINT, sigproc);
	while ((i = getopt(argc, argv, "p:t:h:g")) != -1) {
		switch(i){
			case 'h':
				show_help();
				return 0;
			case 'p':
	    	/* xv port */
				xv_port = strtol(optarg, (char **)NULL, 0);
				break;
			case 't':
				debug = 1;
				break;
			case 'g':
				get_params = 1;
				break;
		}
	}
	int shmid = shmget(key_common,sizeof(common_t),IPC_CREAT | 0666);
	printf("%d\n", shmid);
	common = shmat(shmid,NULL,0);
	shmaddr3 = (uint8_t*)common;
	if (!common) {
		TRACEPNF(0,"shmat failed\n");
		return 0;
	}
	TRACEPNF(0,"common shmem id = %d\n",shmid);
	while (!common->frame_done)
		sleep(1);

	TRACEPNF(0,"Width = %d  Height = %d\n",common->width,common->height);
	read_xv_buf(common);
	return 0;
}
