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

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include <ui/osd.h>

#include <trace.h>
#include <math/bit_ops.h>

void osd_str_showxyc(
	osd_ctx_t	*osd,
	int		x,
	int		y,
	const char	*str,
	int		color
)
{
	osd->voxel = osd->colors + color*2;
	osd->puts_xy(osd, x, y, str);
	osd->voxel = osd->colors;
}

void osd_font_show(osd_ctx_t *osd)
{
	int i;
	for(i=0; i<255; i++) {
		osd->putc_xy(osd, i&0xf, i>>4, i);
	}
}

void osd_font_read(char *name, uint8_t *font)
{
	int i;
	int fd = open(name, O_RDONLY);
	if(fd<0) {
		ETRACE("while open: %s", name);
		return ;
	}

	i = read(fd, font, 4096);
	close(fd);

	if(i!=4096) {
		ETRACE("read(\"%s\", 4096) returns %d", name, i);
		return ;
	}

	int t, d=0;
	for(i=0; i<4096; i++) {
		t = font[i];
		d = 0;
		BIT_COPY(d, t, 24, 31);
		BIT_COPY(d, t, 25, 30);
		BIT_COPY(d, t, 26, 29);
		BIT_COPY(d, t, 27, 28);
                                   
		BIT_COPY(d, t, 28, 27);
		BIT_COPY(d, t, 29, 26);
		BIT_COPY(d, t, 30, 25);
		BIT_COPY(d, t, 31, 24);

		font[i] = d;
	}
}
/*\
 *	UI Helpers
\*/
