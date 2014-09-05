#ifndef __ABI_P3_LB_TASK_BAR_H__
#define __ABI_P3_LB_TASK_BAR_H__
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
 *   Description:
 *     This file contains data types and constants declarations for P3SS ABI LB
 *     subsystem, for tasks specific to camera device menagement and settings.
\*/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(DEVICE_ISOLATED)
#include <isolated/abi/p3.h>
#include <isolated/abi/lb.h>
#else
#include <abi/p3.h>
#include <abi/lb.h>
#endif

/*\
 * Constants declarations
\*/

/*\
 *  Data structures declarations
\*/
#if defined(_MSC_VER) && !defined(__GNUC__)
#include <pshpack1.h>
#endif

/*\
 *  Simple 2d BAR container meta
\*/
typedef enum {
	P3_2D_BAR_SENSOR_DIMS,	/* p3_2d_bars_t    */
	P3_2D_BAR_SENSOR_ROI,	/* p3_2d_bars_t    */
	P3_2D_BAR_RAW_ROI,	/* p3_2d_bars_t    */
	P3_2D_BAR_JPEG_ROI,	/* p3_2d_bars_t    */
	P3_2D_BAR_HIST_ROI,	/* p3_2d_bars_t    */
	P3_2D_BAR_FACE_ROI,	/* p3_2d_bars_t    */
	P3_2D_BAR_FACE_ID_ROI,	/* p3_2d_id_bars_t */
} p3_bars_type_e;

typedef enum {
	P3_BARS_FLAG_LAST	= 0x80,	/* last bar in bar sequence */
} p3_bars_flags_e;

typedef struct {
	uint16_t	items;	/* valid bars in struct		*/
	uint8_t		type;	/* bar subtype			*/
	uint8_t		flags;	/* Reserved for flags		*/
} __attribute__((packed)) p3_bars_hdr_t;

typedef struct {
	uint16_t	x;	/* Column			*/
	uint16_t	y;	/* Line				*/
} __attribute__((packed)) point_2d_16_t;

typedef struct {
	uint16_t	w;	/* Width			*/
	uint16_t	h;	/* Heigh			*/
} __attribute__((packed)) dims_2d_16_t;

typedef struct {
	point_2d_16_t	point;
	dims_2d_16_t	dims;
} __attribute__((packed)) rect_2d_16_t;

typedef struct {
	dims_2d_16_t	parent;	/* Parent width&heigh		*/

	uint16_t	encoding;
	uint8_t		r_bits;	/* sample bits max range	*/
	uint8_t		s_bits;	/* sample bits size		*/

	rect_2d_16_t	child;	/* Child point and dims         */
} __attribute__((packed)) p3_2d_child_t;

typedef struct {
	int16_t		x0;	/* Left column			*/
	int16_t		y0;	/* Bottom line			*/
	int16_t		x1;	/* Right column			*/
	int16_t		y1;	/* Top Line			*/
} __attribute__((packed)) p3_2d_bar_t;

typedef struct {
	uint16_t	items;	/* valid 2d bars in struct	*/
	uint8_t		type;	/* 2d bar subtype		*/
	uint8_t		flags;	/* Reserved for flags		*/
	p3_2d_bar_t	bars[];
} __attribute__((packed)) p3_2d_bars_t;


typedef struct {
	int16_t		x0;	/* Left column			*/
	int16_t		y0;	/* Bottom line			*/
	int16_t		x1;	/* Right column			*/
	int16_t		y1;	/* Top Line			*/

	uint32_t	id;	/* Object ID			*/
	uint32_t	state;	/* Object State			*/
} __attribute__((packed)) p3_2d_id_bar_t;

typedef struct {
	uint16_t	items;	/* valid 2d bars in struct	*/
	uint8_t		type;	/* 2d bar subtype		*/
	uint8_t		flags;	/* Reserved for flags		*/
	p3_2d_id_bar_t	bars[];
} __attribute__((packed)) p3_2d_id_bars_t;

typedef struct {
	uint16_t	items;	/* valid 2d bars in struct	*/
	uint8_t		type;	/* 2d bar subtype		*/
	uint8_t		flags;	/* Reserved for flags		*/
	p3_2d_id_bar_t	bars[1];
} __attribute__((packed)) p3_2d_id_bars1_t;

typedef union {
	p3_bars_hdr_t	bars_hdr;
	p3_2d_bars_t	bars_2d;
	p3_2d_id_bars_t	bars_2d_id;
} p3_bars_u;
#if defined(_MSC_VER) && !defined(__GNUC__)
#include <poppack.h>
#endif

#ifdef __cplusplus
}
#endif
#endif  /* __ABI_P3_LB_TASK_BAR_H__ */
