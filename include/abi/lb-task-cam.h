#ifndef __ABI_P3_LB_TASK_CAM_H__
#define __ABI_P3_LB_TASK_CAM_H__
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

#include <lock/serial.h>
#include <abi/lb-rule-cam.h>

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
 *  Simple Event Management rule
\*/
typedef enum {
	P3_CAM_RELAY_GROUPS = 1,
	P3_CAM_RELAY_G0_CNT = 3,
} p3_cam_limits_e;

typedef enum {
	P3_CAM_LUT_RAW,
	P3_CAM_LUT_JPEG,
	P3_CAM_LUT_DSP,

	P3_CAM_LUT_MAX = 3,			/* XXX Should Be last entry */
} p3_cam_lut_id_e;

typedef enum /*pdb_enum*/{
	P3_LUT_DEFAULT_LINEAR	= 0/*pdb_val*/,	/*			*/
	P3_LUT_PIECEWISE_LINEAR	= 1/*pdb_val*/,	/*			*/
	P3_LUT_TRUE_LUT		= 2,	/* 			*/
} /*pdb_end*/p3_lut_type_e;

typedef enum /*pdb_enum*/{
	P3_LUT_SCALE_NOAUTO	= (0<<0)/*pdb_val*/,	/* NOAUTO SCALE LUT	*/
	P3_LUT_SCALE_AUTO	= (1<<0)/*pdb_val*/,	/* AUTO SCALE LUT	*/
	P3_LUT_SCALE_MASK	= (1<<0),	/* SCALE MASK		*/
} /*pdb_end*/p3_lut_flags_e;
/*
 * pdb_file:typedef.
 * pdb_file:field(@lid,[guitype=internal]).
 * pdb_file:field(@type,[enum=p3_lut_type_e,guitype=selectext,html_updjs='\"lut_displayupdate();\"']).
 * pdb_file:field(uint8_t,flags,[guitype=internal]).
 * pdb_file:field(-'P3_LUT_SCALE',[alias=flags_scale,enum=p3_lut_flags_e,guitype=selectext,html_updjs='\"lut_displayupdate();\"']).
 * pdb_file:field(@param,[guitype=internal]).
 * pdb_file:field(@cell_size,[guitype=internal]).
 * pdb_file:field(@cell_count,[guitype=internal]).
 * pdb_file:field(@max_in,[unused=rng,locdescr=s:'\"in lut_rng\"',nmsrc=sngt:'\"<span id=\\\"lut_maxin_annot\\\">&nbsp;</span>\"',html_updjs='\"lut_displayupdate();\"']).
 * pdb_file:field(@max_out,[unused=rng,locdescr=s:'\"in lut_rng\"',nmsrc=sngt:'\"<span id=\\\"lut_maxout_annot\\\">&nbsp;</span>\"',html_updjs='\"lut_displayupdate();\"']).
 * pdb_file:end('p3_lut_attrs_t').
 * pdb_file:typedef.
 * pdb_file:field(@x0,[nmsrc=sngt:html_empty_str,html_addclass=lut_coord_stl,html_updjs='\"lut_displayupdate();\"']).
 * pdb_file:field(@y0,[nmsrc=sngt:html_empty_str,html_addclass=lut_coord_stl,html_updjs='\"lut_displayupdate();\"']).
 * pdb_file:field(@k,[nmsrc=sngt:html_empty_str,html_addclass=lut_coord_stl,html_updjs='\"lut_displayupdate();\"']).
 * pdb_file:field(@scale,[guitype=internal]).
 * pdb_file:end('p3_lut_picewise1_t',[idx=[nlutcoord]]).
 * */
/*pdb_parse*/
typedef struct {
	uint8_t		lid;		/* LUT id			*/
	uint8_t		type;		/* LUT type			*/
	uint16_t	param;		/* type dependent parameter     */

	uint16_t	max_in;		/* max incoming value for LUT  	*/
	uint16_t	max_out;	/* max outgouing value for LUT	*/

	uint8_t		flags;		/* lut flags	 		*/
	uint8_t		cell_size;	/* lut cell  size in octets	*/
	uint16_t	cell_count;	/* lut cells count		*/
} __attribute__((packed)) p3_lut_attrs_t;
typedef struct {
	uint16_t	y0;		/* Y start                      */
	uint16_t	x0;		/* X start			*/
	uint16_t	k;		/* tg(ALPHA)		  	*/
	uint16_t	scale;		/* k=1			  	*/
} p3_lut_picewise1_t;
/*pdb_noparse*/

typedef struct {
	p3_lut_attrs_t		hdr;
	uint8_t			data[0];
} __attribute__((packed)) p3_lut_any_t;

typedef struct {
	p3_lut_attrs_t		hdr;
	p3_lut_picewise1_t	data[0];
}  __attribute__((packed)) lb_task_lut_picewise1_t;

typedef struct {
	uint8_t			family;	/* type P3_CONFIG_TASKS            */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_TASK_CAM_LUT_T          */
	uint8_t			id;	/* ID sensor id                    */

	lb_task_lut_picewise1_t	settings;
} __attribute__((packed)) lb_task_st_lut_picewise1_t;

typedef struct {
	uint16_t		id;		/* cam4_roi_mode_e */

	uint16_t		col_first;	/* x0 in parent */
	uint16_t		col_last;	/* x1 in parent */
	uint16_t		row_first;	/* y0 in parent */
	uint16_t		row_last;	/* y1 in parent */

	uint8_t			mode;		/* cam4_roi_mode_e */
	uint8_t			res0;
} __attribute__((packed)) lb_task_roi_t;

typedef struct {
	uint8_t			family;	/* type P3_CONFIG_TASK             */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_TASK_CAM_ROI_T          */
	uint8_t			id;	/* ID sensor id                    */

	lb_task_roi_t		settings;
} __attribute__((packed)) lb_task_st_roi_t;

typedef enum {
	CAM4_SENSOR_CAP_COLOR	= (1 << 0),
	CAM4_SENSOR_CAP_MASK	= 0x01
} lb_task_sensor_caps_e;

typedef struct {
	uint64_t		caps;	/* sensor capability flags	   */
} __attribute__((packed)) lb_task_sensor_caps_t;

#define lb_task_lut_picewise1_t_DEFAULTS ((lb_task_lut_picewise1_t){ 	\
    .hdr		= {						\
	.type		=	P3_LUT_DEFAULT_LINEAR,			\
    },								\
})

#if defined(_MSC_VER) && !defined(__GNUC__)
#include <poppack.h>
#endif

#ifdef __cplusplus
}
#endif
#endif  /* __ABI_P3_LB_TASK_CAM_H__ */
