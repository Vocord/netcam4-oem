#ifndef __API_FRAMES_H__
#define __API_FRAMES_H__
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

#ifdef __cplusplus
extern "C" {
#endif

#include <abi/device.h>

typedef struct {
	device_request_t	descr;

	uint8_t			*data;
	unsigned		size;

	uint8_t			*oob_data;
	unsigned		oob_size;

	unsigned		flow;
	unsigned long		expired_at;

	uint8_t			*audio_buf;

	unsigned long		audio_mask;	/* size mask */

	unsigned long		audio_todo;

	uint8_t			flags;		/* flow flags */
	uint8_t			subrate;	/* pll independent flow subrate */
	uint16_t		event;		/* event type/mask */

	uint32_t		serial;
	uint32_t		serial_;
	
	uint64_t		tf;		/* pll based frame rate */

	filetime		audio_t0;	/* timestamp of first Audio Sample in frame */
	filetime		audio_t1;	/* timestamp of last  Audio Sample in frame */
} field_descr_t;

#ifdef __cplusplus
}
#endif

#endif
