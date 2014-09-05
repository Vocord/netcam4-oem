#ifndef __SHARED_OBJECTS_H_INCLUDED__
#define __SHARED_OBJECTS_H_INCLUDED__
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

typedef struct hist_s {
	uint32_t nbins;
	uint32_t hist[300];
} hist_t;

typedef struct common_s {
	uint8_t		frame_idx_done;
	uint8_t		frame_done;

	uint16_t	xvHeight;
	uint16_t	xvWidth;

	uint16_t	sensHeight;
	uint16_t	sensWidth;

	uint16_t	width;
	uint16_t	height;

	int16_t		startx;
	int16_t		starty;

	size_t		data_size;
	size_t		image_size;

	char		window_title[40];

	uint32_t	nbins;
	uint32_t	hist[299];
	uint32_t	*hist_be;

	uint32_t	stddev[4];	// 4-quadrant sigma
	uint32_t	mean[4];	// 4-quadrant mian level
	 int32_t	diff[4];	// 4-quadrant baundary gradient

	hist_t		comp_hist[4];

	uint16_t	nbars;
	uint16_t	bar_flags;
	uint16_t	bars[128];
	//params
	uint32_t	exp;
	uint32_t	period;
	double 		gains[5];
	uint32_t	reg1;
	uint32_t	reg2;

} common_t;

#define	key_yuv1	(6182)
#define key_yuv2	(6193)
#define key_common	(12348)

#endif
