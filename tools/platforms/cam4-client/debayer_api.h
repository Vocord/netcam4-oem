#ifndef __DEBAYER_API_H_INCLUDED__
#define __DEBAYER_API_H_INCLUDED__
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

typedef void _debayerRGB_func(uint8_t *dst, uint8_t *src, int dim_x, int dim_y, int startx, int starty, int ww, int wh);

typedef struct {
	_debayerRGB_func *debayerRGB_func[4];
} debayer_api_t;

extern int arch_probe_fast_debayer(debayer_api_t *api, int dim_x, int startx, int ww);

#endif
