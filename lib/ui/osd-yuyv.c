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

#define _GNU_SOURCE

#include <sys/mman.h>
#include <sys/file.h>
#include <stdlib.h>

#include <trace.h>

#include <compiler.h>

#include <ui/osd-yuyv.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#if 0
static void osd_char_show_(uint16_t *d, uint8_t *f, int c)
{
	int i;
	int m;

	d-=2;
	f += 16*c - 1;

	for(i=0; i<16; i++) {
		f++;
		int	v = f[0];
		for(m=0; m<8; m++, v>>=1) {
			d+=2;
			*(uint32_t*)d = CM0[v&1];
		}
		d += 720 - 16;
	}
}

void osd_font_show_(uint8_t *base, uint8_t *font)
{
	uint64_t	t = -get_cycles();
	int i;
	for(i=0; i<255; i++) {
		uint16_t	*d;
		d = (uint16_t*)base + 720*16*(i>>4) + 18*(i&0xf);
		osd_char_show_(d, font, i);
	}

	t += get_cycles();
	TRACEP(0, "%s: consume %"PRIu64" cycles\n", __func__, t);
}
#endif

typedef struct osd_ascii_line_s {
	uint16_t		sc[46];

	void			*base;

	uint32_t		reserved[6];

	uint64_t		diff;
} osd_ascii_line_t;

/* FIXME Optimization needed */
static void osd_yuyv_refresh(
	osd_ctx_t	*osd,
	int		flag
)
{	
	if(!osd || osd->flag&1 || !osd->font || !osd->base || !osd->lines)
		return;

	int			i, k, j;
	int			f_dw = osd->f_dw;
#if TARGET_BBOX
	int			f_size = osd->f_size/4;	/* Ok */
#endif

	osd_ascii_line_t	*l = osd->lines;
	for(j=0; j<17; j++, l++) {	/* Lines */
		uint64_t diff = l->diff;

		if(flag)
			diff = 0xffffffffffffllu;

		if(!diff)
			continue;

		uint16_t	*s = l->sc - 1;
		uint32_t	*d_ = l->base;

		for(k=0; k<41; k++, diff >>= 1, d_ += 8) {	/* columns */
			s++;

			if(!(diff&1))
				continue;

			uint8_t		*f = osd->font   + 16* (s[0]&0xff);
			uint32_t	*c = osd->colors + 2 * (s[0]>>8);
			uint32_t	*d = d_;
			
			for(i=0; i<16; i++) {
				f++;
				int	v = f[0];
				int	m;

				for(m=0; m<8; m++, v>>=1) {
					d ++;
#if TARGET_BBOX
					d[0] = d[f_size] = c[v&1];	/* FIXME non optimal */
#else
					d[0] = c[v&1];	/* FIXME non optimal */
#endif
				}
				d += f_dw;
			}
		}
		l->diff = 0;
	}
}

static void osd_yuyv_putc(
	osd_ctx_t	*osd
)
{
	if(osd->flag&1)
		return;

	uint8_t		*f = osd->font_;
	uint32_t	*d = osd->base_;

	int i;
	int f_dw = osd->f_dw;
#if TARGET_BBOX
	for(i=0; i<16; i++) {
		int	v = f[0];
		f++;
		
		int	m;
		for(m=0; m<8; m++, v>>=1) {
			d ++;
			d[0] = osd->voxel[v&1];
		}
		d += f_dw;
	}
#else
	for(i=0; i<16; i++) {
		int	v = f[0];
		f++;
		
		int	m;
		for(m=0; m<8; m++, v>>=1) {
			d ++;
			d[0] = osd->voxel[v&1];
		}
		d += f_dw;
		for(m=0; m<8; m++, v>>=1) {
			d ++;
			d[0] = osd->voxel[v&1];
		}
		d += f_dw;
	}
#endif
}

static void osd_yuyv_putc_xy(
	osd_ctx_t	*osd,
	int		x,
	int		y,
	int		c
)
{
	osd->font_ = osd->font + 16*c;
#if TARGET_BBOX
	osd->base_ = osd->base + osd->f_w*y + 16 * x;
#else
	osd->base_ = osd->base + osd->w*y + x;
#endif
	osd_yuyv_putc(osd);
#if TARGET_BBOX
	osd->base_ += osd->f_size;
	osd_yuyv_putc(osd);
#endif
}

 void osd_yuyv_puts_xy(
	osd_ctx_t	*osd,
	int		x,
	int		y,
	const char	*str
)
{
	const uint8_t	*s = (const void*)str;
	if(!s)
		return;
#if TARGET_BBOX
	osd->base_ = osd->base + osd->f_w*y + 16 * x;
#else
	osd->base_ = osd->base + osd->w*y + x;
#endif
	int max = 100;
	while(max && *s) {
		osd->font_ = osd->font + 16*(*s);
		osd_yuyv_putc(osd);
#if TARGET_BBOX
		osd->base_ += osd->f_size;
		osd_yuyv_putc(osd);

		osd->base_ -= osd->f_size;
#endif
		osd->base_ += 16*2;
		max--;
		s++;
	}
}

static void osd_yuyv_puts_xyc(
	osd_ctx_t	*osd,
	int		x,
	int		y,
	const char	*str,
	int		color
)
{
    	if(!str)
		str = "(null)";

	if(!osd || !osd->lines || (y>17) || (y<0) || !str[0])
		return;

	osd_ascii_line_t	*l = (osd_ascii_line_t*)osd->lines + y;

	uint16_t		*d = (uint16_t*)(l->sc-1) + x;
	const uint8_t		*s = (const void*)str;

	uint64_t		diff=0;
	uint16_t		c = color << 8;

	y=x;
	while (s[0] && (x<ARRAY_ELEMS(l->sc))) {
		diff >>=1;
		d++;

		if(d[0] != (c + s[0]))
			diff |= 1ull<<63;


		d[0] = c + s[0];

		s++;
		x++;
	}

	l->diff |= diff>>(64-x);
}

#if 0
static int osd_read_root(
	osd_ctx_t	*osd,
	const char	*name,
	long		foffs
)
{
	int fd = open(name, O_RDONLY);
	if(fd<0) {
		ETRACE("while open: %s", name);
		return 1;
	}

	lseek(fd, foffs, SEEK_SET);

	read(fd, osd->base, (size_t)osd->w*osd->h*2);
	close(fd);

	return 0;
}
#endif

static int osd_read_root_m4y(
	osd_ctx_t	*osd,
	const char	*name,
	long		foffs
)
{
	int fd = open(name, O_RDONLY);
	if(fd<0) {
		ETRACE("while open: %s", name);
		return 1;
	}


	loff_t off;

//	loff_t fsize = lseek(fd, (off_t)0, SEEK_END);
	size_t	dsize = 720*576*2;

//	dsize = fsize - foffs;

	size_t	page_size = getpagesize();
	off = foffs & (~(page_size-1));
	size_t msize = dsize + foffs - off;
#if defined(_POSIX_MAPPED_FILES) || defined(__CYGWIN__)
#if defined(_POSIX_SYNCHRONIZED_IO) || defined(__CYGWIN__)

	uint8_t *src1 = mmap(NULL, msize, PROT_READ, MAP_SHARED, fd, (off_t)off);
	if((long)src1 == -1) {
		ETRACE("%s: @mmap", __func__);

		_exit(0);

		return 1;    
	}

	uint8_t *src = src1 + (foffs - off);
#endif
#endif

	uint32_t	*dst = (typeof(dst))osd->base;
	int x,y;
	uint32_t	tmp;

	uint8_t 	*srcU = src +osd->w*osd->h;
	uint8_t 	*srcV = srcU+osd->w*osd->h/2;

	for(y = 0; y<osd->h; y++)
		for(x = 0; x<osd->w; x+=2,dst++,src+=2,srcU++, srcV++) {
		    	tmp = (src[0]<<16) + (src[1]<<0)+(srcU[0]<<24) + (srcV[0]<<8);
			*dst = tmp;
		}

	munmap(src1, msize);
	close(fd);

	return 0;
}

void osd_yuyv_init(
	osd_ctx_t	*osd,
	void		*base,
	void		*font,
	void		*palette,

	int		w,
	int		h
)
{
	osd->data	= base;
#if TARGET_BBOX
	base += 64 + 22*w - 4;
#endif
	osd->base	= base;
	osd->font	= font - 1;

	osd->w		= w;
	osd->h		= h;

	osd->f_w	= w * 16;
	osd->f_dw	= w/2-8;	/* in sizeof(uint32_t) */
	osd->f_dh	= w * 16;

	osd->f_size	= w*h;		/* field size */

	osd->colors	= palette;
	osd->voxel	= osd->colors;

	osd->putc_xy	= osd_yuyv_putc_xy;
	osd->puts_xy	= osd_yuyv_puts_xy;
	osd->puts_xyc	= osd_yuyv_puts_xyc;
	osd->refresh	= osd_yuyv_refresh;
	osd->read	= osd_read_root_m4y;

	if(!osd->lines) {
		osd->lines = malloc(18*sizeof(osd_ascii_line_t));

		if(!osd->lines)
			return;

		/* Fill screen with defaults */
		osd_ascii_line_t	*l = osd->lines;

		int i, j;
		for(j=0; j<17; j++, l++) {
			l->base = base;
			base += osd->f_dh*2;

			l->diff = 0xffffffffffffllu;
			for(i=0; i<ARRAY_ELEMS(l->sc); i++) {
				l->sc[i] = ' ';
			}
		}
	}
	/* Restore old screen */
	osd->refresh(osd, 1);
}

