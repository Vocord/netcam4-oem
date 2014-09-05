#ifndef __ABI_P3_LB_RULE_CAM_H__
#define __ABI_P3_LB_RULE_CAM_H__
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

/*\
 * Constants declarations
\*/
typedef enum {
        ABI_P3_CAM_SENSOR_T           = 0,    /*  */
} lb_rule_cam_type_e;

typedef enum {
        ABI_P3_CAM_SENSOR_CYPRES,	/*  */
        ABI_P3_CAM_SENSOR_MICRON,	/*  */
} lb_rule_sensor_type_e;
/*\
 *  Data structures declarations
\*/
#if defined(_MSC_VER) && !defined(__GNUC__)
#include <pshpack1.h>
#endif

/*\
 *  Simple Event Management rule
\*/

typedef enum /*pdb_enum*/{						/*flags */
	SENS_FLIP_MODE_TURN_MASK	= (0x3<<0),
	SENS_FLIP_MODE_TURN_0		= (0x0<<0)/*pdb_val*/,
	SENS_FLIP_MODE_TURN_90		= (0x1<<0),
	SENS_FLIP_MODE_TURN_180		= (0x2<<0)/*pdb_val*/,
	SENS_FLIP_MODE_TURN_270		= (0x3<<0),

	SENS_6_BIT_GAIN_SCALE		= (0x0<<2),
	SENS_7_BIT_GAIN_SCALE		= (0x1<<2),
	SENS_GAIN_SCALE_MASK	        = (0x1<<2),

	SENS_MIRROR_MASK		= (0x1<<7),
	SENS_MIRROR_NOVERTICAL		= (0x0<<7)/*pdb_val*/,
	SENS_MIRROR_VERTICAL		= (0x1<<7)/*pdb_val*/,
}/*pdb_end*/ sens_flags_e;

typedef enum /*pdb_enum*/{						/* sensor misc flags */
	SENS_RAW_FLOW08			= (0x0<<0)/*pdb_val*/,
	SENS_RAW_FLOW10			= (0x1<<0)/*pdb_val*/,
	SENS_RAW_FLOW12			= (0x2<<0)/*pdb_val*/,
	SENS_RAW_FLOW16			= (0x3<<0)/*pdb_val*/,
	SENS_RAW_MASK			= (0x3<<0),

	SENS_BITS10			= (0x0<<2)/*pdb_val*/,
	SENS_BITS12			= (0x1<<2)/*pdb_val*/,
	SENS_BITS_MASK			= (0x1<<2),
	SENS_DETECT_AUTO		= (0x0<<3)/*pdb_val*/,
	SENS_DETECT_MANUAL		= (0x1<<3)/*pdb_val*/,
	SENS_DETECT_MASK		= (0x1<<3),
	SENS_SHUTTER_ROLLING		= (0x0<<4)/*pdb_val*/,
	SENS_SHUTTER_GLOBAL		= (0x1<<4)/*pdb_val*/,
	SENS_SHUTTER_MASK		= (0x1<<4),

	SENS_FPN_MODE_SHIFT		= (5),
	SENS_FPN_DISABLE		= (0x0<<SENS_FPN_MODE_SHIFT)/*pdb_val*/,
	SENS_FPN_ENABLE			= (0x1<<SENS_FPN_MODE_SHIFT)/*pdb_val*/,
	SENS_FPN_FORCE_8BIT		= (0x2<<SENS_FPN_MODE_SHIFT)/*pdb_val*/,
	SENS_FPN_FORCE_16BIT		= (0x3<<SENS_FPN_MODE_SHIFT)/*pdb_val*/,
	SENS_FPN_MASK			= (0x3<<SENS_FPN_MODE_SHIFT),
} /*pdb_end*/sens_type_e;
/* 
 * pdb_file:typedef.
 * pdb_file:field(uint8_t,sensor,[guitype=internal]).
 * pdb_file:field(-'SENS_RAW',[alias='sensor_raw',enum=sens_type_e:'SENS_RAW']).
 * pdb_file:field(uint8_t,'flags',[guitype=internal]).
 * pdb_file:field(-'SENS_FLIP_MODE_TURN',[alias='flags_turn',enum=sens_flags_e:'SENS_FLIP_MODE_TURN']).
 * pdb_file:field(-'SENS_MIRROR',[alias='flags_vertical',enum=sens_flags_e:'SENS_MIRROR']).
 * pdb_file:field(int16_t,'gain0',[off='offsetof(lb_rule_cam_sens_t,gains)']).
 * pdb_file:field(int16_t,'gain1',[off='offsetof(lb_rule_cam_sens_t,gains)+sizeof(uint16_t)']).
 * pdb_file:field(int16_t,'gain2',[off='offsetof(lb_rule_cam_sens_t,gains)+sizeof(uint16_t)*2']).
 * pdb_file:field(int16_t,'gain3',[off='offsetof(lb_rule_cam_sens_t,gains)+sizeof(uint16_t)*3']).
 * pdb_file:field(int16_t,'gain4',[off='offsetof(lb_rule_cam_sens_t,gains)+sizeof(uint16_t)*4']).
 * pdb_file:end('lb_rule_cam_sens_t').
 */
/*pdb_parse*/
typedef struct {	/* XX */
	uint8_t		sensor;		/* sensor type			*/
	uint8_t		flags;		/* flags			*/
/*pdb_noparse*/
	int16_t		gains[5];	/* sensor gains			*/
					/* 	0 - Green1		*/
					/* 	1 - Blue  		*/
					/* 	2 - Red   		*/
					/* 	3 - Green2		*/
					/* 	4 - GLOBAL		*/
/*pdb_parse*/
} __attribute__((packed)) lb_rule_cam_sens_t;
/*pdb_noparse*/
typedef enum /*pdb_enum*/{						/* flags */
	SENS_SYNC_SPERIOD_MASK		= (0x7f),	/* sout period mask */

	SENS_SYNC_MODE_GUI_MASK		= (0x107<<0),	/* 4 bits reserved  */
	SENS_SYNC_MODE_MASK		= (0x7<<0),	/* 3 bits reserved  */
	SENS_SYNC_MODE_SENS_FREE_RUN	= (0x0<<0)/*pdb_val*/,
	SENS_SYNC_MODE_FPGA_FREE_RUN	= (0x1<<0)/*pdb_val*/,
	SENS_SYNC_MODE_EXT_SYNC		= (0x2<<0)/*pdb_val*/,
	SENS_SYNC_MODE_SW_SYNC		= (0x3<<0)/*pdb_val*/,
	SENS_SYNC_MODE_SW_SYNC_START	= (0x5<<0),
	SENS_SYNC_MODE_FPGA_SYNC_START	= (0x4<<0),
	SENS_SYNC_MODE_EXT_SYNC_CENTER	= (0x6<<0)/*pdb_val*/,
	SENS_SYNC_MODE_SENS_FREE_RUN_SK	= (0x100<<0)/*pdb_val*/,
	SENS_SYNC_MODE_FPGA_FREE_RUN_SK	= (0x101<<0)/*pdb_val*/,

	SENS_SYNC_SPERIOD_SHIFT		= (     3),	/* SOUT period unit Shift */
	SENS_SYNC_SPERIOD_UNIT_MASK	= (0x3<<0),	/* SOUT period unit       */
	SENS_SYNC_SPERIOD_UNIT_100US	= (0x0<<0),
	SENS_SYNC_SPERIOD_UNIT_1MS	= (0x1<<0),
	SENS_SYNC_SPERIOD_UNIT_10MS	= (0x2<<0),
	SENS_SYNC_SPERIOD_UNIT_100MS	= (0x3<<0),

	SENS_SYNC_SPERIOD_UNITS_MASK	= (SENS_SYNC_SPERIOD_UNIT_MASK<<SENS_SYNC_SPERIOD_SHIFT),	/* SOUT period shifted unit       */
	SENS_SYNC_SPERIOD_UNITS_100US	= (SENS_SYNC_SPERIOD_UNIT_100US<<SENS_SYNC_SPERIOD_SHIFT)/*pdb_val*/,
	SENS_SYNC_SPERIOD_UNITS_1MS	= (SENS_SYNC_SPERIOD_UNIT_1MS<<SENS_SYNC_SPERIOD_SHIFT)/*pdb_val*/,
	SENS_SYNC_SPERIOD_UNITS_10MS	= (SENS_SYNC_SPERIOD_UNIT_10MS<<SENS_SYNC_SPERIOD_SHIFT)/*pdb_val*/,
	SENS_SYNC_SPERIOD_UNITS_100MS	= (SENS_SYNC_SPERIOD_UNIT_100MS<<SENS_SYNC_SPERIOD_SHIFT)/*pdb_val*/
} /*pdb_end*/sens_sync_mode_e;

typedef enum /*pdb_enum*/{						/* flags           */
	SENS_SYNC_EXT_STROBBED_MASK	= (0x4<<0),
	SENS_SYNC_EXT_MASK		= (0x7<<0),	/* 3 bits reserved */

	SENS_SYNC_EXT_SYN0_ON		= (0x0<<0)/*pdb_val*/,
	SENS_SYNC_EXT_SYN1_ON		= (0x1<<0)/*pdb_val*/,
	SENS_SYNC_EXT_SYN2_ON		= (0x2<<0)/*pdb_val*/,
	SENS_SYNC_EXT_SYN0_STROBBED_ON	= (0x4<<0)/*pdb_val*/,
	SENS_SYNC_EXT_SYN1_STROBBED_ON	= (0x5<<0)/*pdb_val*/,
	SENS_SYNC_EXT_SYN1_GPS_PPS	= (0x6<<0)/*pdb_val*/,
} /*pdb_end*/sens_sync_ext_src_e;

typedef enum /*pdb_enum*/{						/* Period ms */
	GPS_SYNC_1HZ			= (0)	/*pdb_val*/,		/* 1000 */
	GPS_SYNC_2HZ			= (1)	/*pdb_val*/,		/*  500 */
	GPS_SYNC_4HZ			= (2)	/*pdb_val*/,		/*  250 */
	GPS_SYNC_5HZ			= (3)	/*pdb_val*/,		/*  200 */
	GPS_SYNC_8HZ			= (4)	/*pdb_val*/,		/*  125 */
	GPS_SYNC_10HZ			= (5)	/*pdb_val*/,		/*  100 */
	GPS_SYNC_12_5HZ			= (6)	/*pdb_val*/,		/*!  80 */
	GPS_SYNC_13_33HZ		= (7)	/*pdb_val*/,		/*!  75 */
	GPS_SYNC_15_625HZ		= (8)	/*pdb_val*/,		/*!  64 */
	GPS_SYNC_16_66HZ		= (9)	/*pdb_val*/,		/*!  60 */
	GPS_SYNC_20HZ			= (10)	/*pdb_val*/,		/*   50 */
	GPS_SYNC_25HZ			= (11)	/*pdb_val*/,		/*   40 */
	GPS_SYNC_40HZ			= (12)	/*pdb_val*/,		/*   25 */
	GPS_SYNC_50HZ			= (13)	/*pdb_val*/,		/*   20 */
	GPS_SYNC_MAX,
	GPS_SYNC_100HZ			= (14)	,			/*   10 */
} /*pdb_end*/gps_sync_freq_e;
#define SENS_SYNC_STROBE(V, N) ((V>>SENS_SYNC_STROBE_SIN##N##_SHIFT) & SENS_SYNC_STROBE_MASK)
typedef enum /*pdb_enum*/ {						/* flags */
	SENS_SYNC_STROBE_SIN1_SHIFT	= (0),
	SENS_SYNC_STROBE_SIN2_SHIFT	= (4),

	SENS_SYNC_STROBE_MASK		= (0x7<<0),
	SENS_SYNC_STROBE_OFF		= (0x0<<0),
	SENS_SYNC_STROBE_SIN1		= (0x1<<0),
	SENS_SYNC_STROBE_SIN2		= (0x2<<0),
	SENS_SYNC_STROBE_SIN3		= (0x3<<0),
	SENS_SYNC_STROBE_SOUT1		= (0x4<<0),
	SENS_SYNC_STROBE_SOUT2		= (0x5<<0),
	SENS_SYNC_STROBE_SOUT3		= (0x6<<0),

	SENS_SYNC_STROBE1_MASK	= (SENS_SYNC_STROBE_MASK<<SENS_SYNC_STROBE_SIN1_SHIFT),
	SENS_SYNC_STROBE1_OFF	= (SENS_SYNC_STROBE_OFF<<SENS_SYNC_STROBE_SIN1_SHIFT)/*pdb_val*/,
	SENS_SYNC_STROBE1_SIN1	= (SENS_SYNC_STROBE_SIN1<<SENS_SYNC_STROBE_SIN1_SHIFT)/*pdb_val*/,
	SENS_SYNC_STROBE1_SIN2	= (SENS_SYNC_STROBE_SIN2<<SENS_SYNC_STROBE_SIN1_SHIFT)/*pdb_val*/,
	SENS_SYNC_STROBE1_SIN3	= (SENS_SYNC_STROBE_SIN3<<SENS_SYNC_STROBE_SIN1_SHIFT)/*pdb_val*/,
	SENS_SYNC_STROBE1_SOUT1	= (SENS_SYNC_STROBE_SOUT1<<SENS_SYNC_STROBE_SIN1_SHIFT)/*pdb_val*/,
	SENS_SYNC_STROBE1_SOUT2	= (SENS_SYNC_STROBE_SOUT2<<SENS_SYNC_STROBE_SIN1_SHIFT)/*pdb_val*/,
	SENS_SYNC_STROBE1_SOUT3	= (SENS_SYNC_STROBE_SOUT3<<SENS_SYNC_STROBE_SIN1_SHIFT)/*pdb_val*/,

	SENS_SYNC_STROBE2_MASK	= (SENS_SYNC_STROBE_MASK<<SENS_SYNC_STROBE_SIN2_SHIFT),
	SENS_SYNC_STROBE2_OFF	= (SENS_SYNC_STROBE_OFF<<SENS_SYNC_STROBE_SIN2_SHIFT)/*pdb_val*/,
	SENS_SYNC_STROBE2_SIN1	= (SENS_SYNC_STROBE_SIN1<<SENS_SYNC_STROBE_SIN2_SHIFT)/*pdb_val*/,
	SENS_SYNC_STROBE2_SIN2	= (SENS_SYNC_STROBE_SIN2<<SENS_SYNC_STROBE_SIN2_SHIFT)/*pdb_val*/,
	SENS_SYNC_STROBE2_SIN3	= (SENS_SYNC_STROBE_SIN3<<SENS_SYNC_STROBE_SIN2_SHIFT)/*pdb_val*/,
	SENS_SYNC_STROBE2_SOUT1	= (SENS_SYNC_STROBE_SOUT1<<SENS_SYNC_STROBE_SIN2_SHIFT)/*pdb_val*/,
	SENS_SYNC_STROBE2_SOUT2	= (SENS_SYNC_STROBE_SOUT2<<SENS_SYNC_STROBE_SIN2_SHIFT)/*pdb_val*/,
	SENS_SYNC_STROBE2_SOUT3	= (SENS_SYNC_STROBE_SOUT3<<SENS_SYNC_STROBE_SIN2_SHIFT)/*pdb_val*/,

	SENS_SYNC_HAS_STROBE_PARAMS	= (0x7<<0),
} /*pdb_end*/sens_sync_strobe_src_e;

typedef enum {
	SENS_MISC_DRAW_GPS_DATA		= (1 << 0),
	SENS_MISC_GPS_AS_TIME_REF	= (1 << 1),	/* sync Timestamps with GPS */
	SENS_MISC_MISC_MASK		= 0x03
} sens_sync_misc_flags_e;

/*\
 *  pdb_file:typedef.
 *  pdb_file:field(uint16_t,mode,[alias=mode_flags,guitype=internal]).
 *  pdb_file:field((-'SENS_SYNC_MODE_GUI'),[guitype=selectext,alias=mode,html_addclass=video_sync_bigselectstl,html_updjs='\"video_sync_modedisplay(this);\"',enum=sens_sync_mode_e:'SENS_SYNC_MODE']).
 *  pdb_file:field((-'SENS_SYNC_SPERIOD_UNITS'),[alias=sout_timer_units,enum=sens_sync_mode_e:'SENS_SYNC_SPERIOD_UNITS',nmsrc=sngt:'html_empty_str']).
 *  pdb_file:field(@ext_src,[unused=modeext0,guitype=selectext,html_updjs='\"video_sync_modedisplay(this);\"',enum=sens_sync_ext_src_e]).
 *  pdb_file:field(uint8_t,strobe_src,[guitype=intermal]).
 *  pdb_file:field((-'SENS_SYNC_STROBE1'),[unused=modeext1,alias=strobe_src1,enum=sens_sync_strobe_src_e:'SENS_SYNC_STROBE1',enum_iterf=video_sync_strobe_src1_iterf]).
 *  pdb_file:field((-'SENS_SYNC_STROBE2'),[unused=modeext2,alias=strobe_src2,enum=sens_sync_strobe_src_e:'SENS_SYNC_STROBE2',enum_iterf=video_sync_strobe_src2_iterf]).
 *  pdb_file:field(@period,[unused=modetimer,units=usec,html_updjs='\"video_sync_periodf();\"',html_add='\" maxlength=\\\"8\\\" onfocus=\\\"video_sync_periodfocused=true;\\\"  onblur=\\\"video_sync_periodfocused=false;\\\"\"']).
 *  pdb_file:field(@sout_timer,[nmsrc=sngt:'html_empty_str']).
 *  pdb_file:field(@exp,[units=usec]).
 *  pdb_file:field(@gps_freq,[unused=modeextg,units=fps,enum=gps_sync_freq_e]).
 *  pdb_file:field(uint8_t,misc,[guitype=internal]).
 *  pdb_file:field((+'SENS_MISC_DRAW_GPS_DATA'),[unused=modeextcg,alias=misc_gps_data,guitype=checkbox]).
 *  pdb_file:field((+'SENS_MISC_GPS_AS_TIME_REF'),[unused=modeextcg,alias=misc_timeref,guitype=checkbox]).
 *  pdb_file:end('lb_rule_cam_sync_t').
\*/
/*pdb_parse*/
typedef struct {
	uint16_t	mode;		/* sync_mode			   */

	uint8_t		ext_src;	/* Ext syn source		   */
	uint8_t		strobe_src;	/* SINs strobe mode		   */

	uint32_t	exp;		/* Exposition usecs		   */
	uint32_t	period;		/* Shot period delta usecs	   */

	uint8_t		sout_timer;	/* 7bit	SOUT timer in tu from mode */
	uint8_t		gps_freq;	/* gps_sync_freq_e		   */

	uint8_t		misc;		/* misc flags 			   */
/*pdb_noparse*/
	uint8_t		pad;		/* pad				   */
/*pdb_parse*/
} __attribute__((packed)) lb_rule_cam_sync_t;

/*pdb_noparse*/
typedef enum /*pdb_enum*/{					/* colour_mode */
	SENS_COLOR_MODE_MASK		= (0x7<<0),
	SENS_COLOR_MODE_BW		= (0x0<<0)/*pdb_val*/,
	SENS_COLOR_MODE_BAYER_RGB	= (0x1<<0)/*pdb_val*/,
} /*pdb_end*/sens_jpeg_colour_mode_e;

typedef enum /*pdb_enum*/{				/* bayer_phase */
	SENS_BAYER_PHASE_MASK		= (0xf<<0),
	SENS_BAYER_RGB_PHASE_G1RG2B	= (0x0<<0)/*pdb_val*/,
	SENS_BAYER_RGB_PHASE_RG1BG2	= (0x1<<0)/*pdb_val*/,
	SENS_BAYER_RGB_PHASE_G1BG2R	= (0x2<<0)/*pdb_val*/,
	SENS_BAYER_RGB_PHASE_BG1RG2	= (0x3<<0)/*pdb_val*/,
} /*pdb_end*/sens_jpeg_bayer_phase_e;

typedef enum /*pdb_enum*/{				/* bayer_phase */
	SENS_CMODE_VMIRROR_FLIP		= (0x01<<7),
	SENS_CMODE_ORIENTATION_SHIFT	= (5),
	SENS_CMODE_ORIENTATION_MASK	= (0x07<<5),
	SENS_CMODE_MASK			= (0x1f<<0),
	SENS_CMODE_BAYER_G1RG2B		= (0x00<<0)/*pdb_val*/,
	SENS_CMODE_BAYER_RG1BG2		= (0x01<<0)/*pdb_val*/,
	SENS_CMODE_BAYER_G1BG2R		= (0x02<<0)/*pdb_val*/,
	SENS_CMODE_BAYER_BG1RG2		= (0x03<<0)/*pdb_val*/,
	SENS_CMODE_BW			= (0x04<<0)/*pdb_val*/,
} /*pdb_end*/sensor_cmode_e;
/*\
 *  State
 *
 *  pdb_file:typedef.
 *  pdb_file:field(@'pad8_3',[guitype=internal]).
 *  pdb_file:field(uint8_t,'colour_mode',[guitype=internal]).
 *  pdb_file:field(-'SENS_COLOR_MODE',[alias='color_mode_jpeg',enum=sens_jpeg_colour_mode_e]).
 *  pdb_file:field(uint8_t,'bayer_phase',[guitype=internal]).
 *  pdb_file:field(-'SENS_BAYER_PHASE',[alias=bayer_phase_jpeg,enum=sens_jpeg_bayer_phase_e]).
 *  pdb_file:end('lb_rule_cam_jpeg_t').
\*/
/*pdb_parse*/

typedef struct {
	uint8_t		compression;	/* JPEG compression		   */
	uint8_t		bayer_phase;	/* Bayer phase			   */
	uint8_t		colour_mode;	/* Colour mode			   */
	uint8_t		pad8_3;		/* pad				   */
} __attribute__((packed)) lb_rule_cam_jpeg_t;
/*pdb_noparse*/
/* duration Unit */
#define DURATION_UNIT_MASK (0x3)	
#define DURATION_UNIT_1US (0x0)
#define DURATION_UNIT_10US (0x1)
#define DURATION_UNIT_100US  (0x2)
#define DURATION_UNIT_1MS (0x3)

#define POLARITY_NEGATIVE (0x0)
#define POLARITY_POSITIVE (0x1)
#define POLARITY_MASK (0x1)

#define SENS_SIN_DELAYS_UNIT_SHIFT 0
#define SENS_SIN_DELAY0_UNIT_SHIFT 2
#define SENS_SIN_DELAY1_UNIT_SHIFT 4
#define SENS_SIN_POLARITY_SHIFT 6
typedef enum /*pdb_enum*/{						/* sIN flags */
	SENS_SIN_DELAYS_UNIT_MASK	= (0x3<<0),	/* Strobe delay Unit */
	SENS_SIN_DELAYS_UNIT_1US	= (0x0<<0)/*pdb_val*/,
	SENS_SIN_DELAYS_UNIT_10US	= (0x1<<0)/*pdb_val*/,
	SENS_SIN_DELAYS_UNIT_100US	= (0x2<<0)/*pdb_val*/,
	SENS_SIN_DELAYS_UNIT_1MS	= (0x3<<0)/*pdb_val*/,

	SENS_SIN_DELAY0_UNIT_MASK	= (0x3<<2),	/* OFF delay Unit */
	SENS_SIN_DELAY0_UNIT_1US	= (0x0<<2)/*pdb_val*/,
	SENS_SIN_DELAY0_UNIT_10US	= (0x1<<2)/*pdb_val*/,
	SENS_SIN_DELAY0_UNIT_100US	= (0x2<<2)/*pdb_val*/,
	SENS_SIN_DELAY0_UNIT_1MS	= (0x3<<2)/*pdb_val*/,

	SENS_SIN_DELAY1_UNIT_MASK	= (0x3<<4),	/* ON  delay Unit */
	SENS_SIN_DELAY1_UNIT_1US	= (0x0<<4)/*pdb_val*/,
	SENS_SIN_DELAY1_UNIT_10US	= (0x1<<4)/*pdb_val*/,
	SENS_SIN_DELAY1_UNIT_100US	= (0x2<<4)/*pdb_val*/,
	SENS_SIN_DELAY1_UNIT_1MS	= (0x3<<4)/*pdb_val*/,

	SENS_SIN_POLARITY_MASK		= (0x1<<6),	/* Active Polarity */
	SENS_SIN_POLARITY_NEGATIVE	= (0x0<<6)/*pdb_val*/,
	SENS_SIN_POLARITY_POSITIVE	= (0x1<<6)/*pdb_val*/,

	SENS_SIN_MIN_DURATION_MASK	= (0x3ff),
	SENS_SIN_DELAY_MASK		= (0x07f),
} /*pdb_end*/sens_sin_flags_e;

typedef enum {
	WIRE_UNIT_MASK	= (0x3<<0),
	WIRE_UNIT_1US	= (0x0<<0),
	WIRE_UNIT_10US	= (0x1<<0),
	WIRE_UNIT_100US	= (0x2<<0),
	WIRE_UNIT_1MS	= (0x3<<0),
} wire_flags_e;

/*
 * pdb_file:typedef.
 * pdb_file:field(uint8_t,flags,[guitype=internal]).
 * pdb_file:field(-'SENS_SIN_DELAY0_UNIT',[nmsrc=sngt:'html_empty_str',alias='delay0_units',enum=sens_sin_flags_e:'SENS_SIN_DELAY0_UNIT']).
 * pdb_file:field(-'SENS_SIN_DELAY1_UNIT',[nmsrc=sngt:'html_empty_str',alias='delay1_units',enum=sens_sin_flags_e:'SENS_SIN_DELAY1_UNIT']).
 * pdb_file:field(-'SENS_SIN_DELAYS_UNIT',[nmsrc=sngt:'html_empty_str',alias='delays_units',enum=sens_sin_flags_e:'SENS_SIN_DELAYS_UNIT']).
 * pdb_file:field(-'SENS_SIN_POLARITY',[alias='polarity',enum=sens_sin_flags_e:'SENS_SIN_POLARITY']).
 * pdb_file:field(@min_duration,[units=usec]).
 * pdb_file:field(@delay0,[nmsrc=sngt:'html_empty_str']).
 * pdb_file:field(@delay1,[nmsrc=sngt:'html_empty_str']).
 * pdb_file:field(@delays,[nmsrc=sngt:'html_empty_str']).
 * pdb_file:end('lb_rule_cam_sin_t').
 */
/*pdb_parse*/
typedef struct {
	uint8_t		flags;		/* flags			   */
	uint8_t		delays;		/* 7bit Stb delay in tu from flags */
	uint8_t		delay0;		/* 7bit Off delay in tu from flags */
	uint8_t		delay1;		/* 7bit On  dealy in tu from flags */
	uint16_t	min_duration;	/*10bit	us			   */
} __attribute__((packed)) lb_rule_cam_sin_t;
/*pdb_noparse*/

typedef struct {
	uint16_t	flags;		/* flags			 */
	uint16_t	min_duration;	/* debounce filter us		 */
	uint32_t	delays;		/* us Stb delay in tu from flags */
	uint32_t	delay0;		/* us Off delay in tu from flags */
	uint32_t	delay1;		/* us On  dealy in tu from flags */
} __attribute__((packed)) lb_rule_din_t;

#define SENS_SOUT_DURATION_UNIT_SHIFT 0
#define SENS_SOUT_POLARITY_SHIFT 6
typedef enum /*pdb_enum*/{						/* sOUT flags */
	SENS_SOUT_DURATION_MASK		= (0x7f),	/* duration mask */
	SENS_SOUT_DURATION_UNIT_MASK	= (0x3<<0),	/* duration Unit */
	SENS_SOUT_DURATION_UNIT_1US	= (0x0<<0)/*pdb_val*/,
	SENS_SOUT_DURATION_UNIT_10US	= (0x1<<0)/*pdb_val*/,
	SENS_SOUT_DURATION_UNIT_100US	= (0x2<<0)/*pdb_val*/,
	SENS_SOUT_DURATION_UNIT_1MS	= (0x3<<0)/*pdb_val*/,

	SENS_SOUT_MODE_MASK		= (0x7<<2),	/* Operation Mode */
	SENS_SOUT_MODE_OUT0		= (0x0<<2)/*pdb_val*/,
	SENS_SOUT_MODE_OUT1		= (0x1<<2)/*pdb_val*/,
	SENS_SOUT_MODE_SW		= (0x2<<2)/*pdb_val*/,
	SENS_SOUT_MODE_SENSOR_STROBE	= (0x3<<2)/*pdb_val*/,
	SENS_SOUT_MODE_TIMER		= (0x4<<2)/*pdb_val*/,

	SENS_SOUT_POLARITY_MASK		= (0x1<<6),	/* Active Polarity */
	SENS_SOUT_POLARITY_NEGATIVE	= (0x0<<6)/*pdb_val*/,
	SENS_SOUT_POLARITY_POSITIVE	= (0x1<<6)/*pdb_val*/,
} /*pdb_end*/sens_sout_flags_e;
/*
 * pdb_file:typedef.
 * pdb_file:field(uint8_t,flags,[guitype=internal]).
 * pdb_file:field(-'SENS_SOUT_MODE',[html_updjs='\"sout_modedisplay(this);\"',alias='mode',enum=sens_sout_flags_e:'SENS_SOUT_MODE']).
 * pdb_file:field(-'SENS_SOUT_DURATION_UNIT',[unused=modenoconst,nmsrc=sngt:'html_empty_str',alias='duration_units',enum=sens_sout_flags_e:'SENS_SOUT_DURATION_UNIT']).
 * pdb_file:field(-'SENS_SOUT_POLARITY',[unused=modenoconst,alias='polarity',enum=sens_sout_flags_e:'SENS_SOUT_POLARITY']).
 * pdb_file:field(@duration,[unused=modenoconst,nmsrc=sngt:'html_empty_str']).
 * pdb_file:end('lb_rule_cam_sout_t').
 */
/*pdb_parse*/
typedef struct {
	uint8_t		flags;		/* flags			   */
	uint8_t		duration;	/* 7bit	in tu from flags	   */
} __attribute__((packed)) lb_rule_cam_sout_t;
/*pdb_noparse*/

typedef struct {
	uint8_t			flags;		/* Polarity See
						   SENS_SOUT_POLARITY_xxx  */
	uint8_t			mode;		/* Output/latch source see
						   SENS_SOUT_MODE_xxx	   */
	uint8_t			pad2;
	uint8_t			pad3;
	uint32_t		duration;	/* pulse duration us 	   */
} __attribute__((packed)) lb_task_dout_t;

/*\
 *  Cam Management rule storage
\*/

typedef struct {
	uint8_t			family;	/* type P3_CONFIG_RULES            */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_RULE_CAM_SENS_T         */
	uint8_t			id;	/* ID sensor id                    */

	lb_rule_cam_sens_t	settings;
} __attribute__((packed)) lb_rule_st_cam_sens_t;

typedef struct {
	uint8_t			family;	/* type P3_CONFIG_RULES            */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_RULE_CAM_SYNC_T         */
	uint8_t			id;	/* ID sensor id                    */

	lb_rule_cam_sync_t	settings;
} __attribute__((packed)) lb_rule_st_cam_sync_t;

typedef struct {
	uint8_t			family;	/* type P3_CONFIG_RULES            */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_RULE_CAM_JPEG_T         */
	uint8_t			id;	/* ID sensor id                    */

	lb_rule_cam_jpeg_t	settings;
} __attribute__((packed)) lb_rule_st_cam_jpeg_t;

typedef struct {
	uint8_t			family;	/* type P3_CONFIG_RULES            */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_RULE_CAM_SIN_T          */
	uint8_t			id;	/* ID sensor id                    */

	lb_rule_cam_sin_t	settings;
} __attribute__((packed)) lb_rule_st_cam_sin_t;

typedef struct {
	uint8_t			family;	/* type P3_CONFIG_RULES            */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_RULE_CAM_SOUT_T         */
	uint8_t			id;	/* ID sensor id                    */

	lb_rule_cam_sout_t	settings;
} __attribute__((packed)) lb_rule_st_cam_sout_t;

typedef enum {
	CAM4_LUT_TARGET_NONE	= 0,
	CAM4_LUT_TARGET_RAW	= 1,
	CAM4_LUT_TARGET_JPEG	= 2,

	CAM4_LUT_TARGET_MAX	= 3,
	CAM4_LUT_TARGET_MASK	= 3,
} lut_target_e;

#if 0
typedef enum {
	CAMCTRL_OPTION_AUTO_EXPOSURE		= 0x0001,
	CAMCTRL_OPTION_AUTO_LUT			= 0x0002,
	CAMCTRL_OPTION_AUTO_GAIN		= 0x0004,
	CAMCTRL_OPTION_AUTO_OFFSET		= 0x0008,
	CAMCTRL_OPTION_AUTO_STRIPE_COMPENS	= 0x0010,
	CAMCTRL_OPTION_EXPOSURE_50HZ_SYNCHRO	= 0x0020,
	CAMCTRL_OPTION_AUTO_IRIS		= 0x0040,
	CAMCTRL_OPTION_AUTO_FOCUS		= 0x0080
} cam_ctrl_modes_e;
#endif
#define CAMCTRL_OPTION_SYNCR0_MASK (CAMCTRL_OPTION_EXPOSURE_50HZ_SYNCHRO|CAMCTRL_OPTION_EXPOSURE_60HZ_SYNCHRO|CAMCTRL_OPTION_EXPOSURE_FULL_PERIOD_SYNCHRO)

typedef struct {
	uint8_t			expo_level_p;
	uint8_t			expo_ovb_p;	
} __attribute__((packed)) lb_rule_cam_ctrl_expo_t;

typedef enum /*pdb_enum*/{
	CAM4_GAIN_METHOD_GREY_NORM = 0/*pdb_val*/
} /*pdb_end*/cam4_gain_method_e;

typedef enum {
	CAM4_GAIN_MODE_NORMAL = 0
} cam4_gain_mode_e;

typedef enum /*pdb_enum*/{
 	CAM4_GAIN_ROI_UNIFORM = 0/*pdb_val*/,
 	CAM4_GAIN_ROI_CENTER3X3 = 1/*pdb_val*/
} /*pdb_end*/cam4_gain_roi_type_e;

typedef enum {
	CAM4_HIST_ROI_SENSOR	= 0,
	CAM4_HIST_ROI_USER	= 1
} cam4_hist_roi_mode_e;

typedef enum {
	CAM4_ROI_ID_HIST	= 0,
	CAM4_ROI_ID_RAW		= 1,
	CAM4_ROI_ID_JPEG	= 2,
	CAM4_ROI_ID_SHADOW	= 3,
	CAM4_ROI_ID_SENS	= 4,
	CAM4_ROI_ID_HIST_SIMPLE	= 5,
	CAM4_ROI_ID_MAX		= 5
} cam4_roi_id_e;

typedef enum /*pdb_enum*/{
	CAM4_ROI_SPEC_SENSOR	= 0/*pdb_val*/,
	CAM4_ROI_USER	 	= 1/*pdb_val*/,
	CAM4_ROI_FULL_FRAME	= 2/*pdb_val*/,
	CAM4_ROI_AS_RAW		= 3/*pdb_val*/,
	CAM4_ROI_AS_JPEG	= 4/*pdb_val*/ /* WTF? */,
	CAM4_ROI_USER_PERCENT	= 5/*pdb_val*/,
	CAM4_ROI_MODE_MAX	= 5,
	CAM4_ROI_SIMPLE_FLAG	= 6,
	CAM4_ROI_RO_FLAG        = 7,
	CAM4_ROI_APPLIED_FLAG	= 8
} /*pdb_end*/cam4_roi_mode_e;

typedef enum {
	CAM4_ROI_RAW_SPEC_SENSOR = CAM4_ROI_SPEC_SENSOR,
	CAM4_ROI_RAW_USER	 = CAM4_ROI_USER,
	CAM4_ROI_RAW_FULL_FRAME	 = CAM4_ROI_FULL_FRAME
} cam4_roi_raw_mode_e;

typedef enum {
	CAM4_ROI_JPEG_SPEC_SENSOR = CAM4_ROI_SPEC_SENSOR,
	CAM4_ROI_JPEG_USER	  = CAM4_ROI_USER,
	CAM4_ROI_JPEG_FULL_FRAME  = CAM4_ROI_FULL_FRAME,
	CAM4_ROI_JPEG_AS_RAW	  = CAM4_ROI_AS_RAW
} cam4_roi_jpeg_mode_e;

typedef enum {
	CAM4_ROI_HIST_SPEC_SENSOR  = CAM4_ROI_SPEC_SENSOR,
	CAM4_ROI_HIST_AS_RAW	   = CAM4_ROI_AS_RAW,
	CAM4_ROI_HIST_USER	   = CAM4_ROI_USER,
	CAM4_ROI_HIST_USER_PERCENT = CAM4_ROI_USER_PERCENT
} cam4_roi_hist_mode_e;

typedef enum /*pdb_enum*/{
	CAM4_ALL_ROI_MODE_AUTO_ROT_DISABLE = (1 << 0)/*pdb_val*/,
	CAM4_ALL_ROI_MODE_SIMPLE	   = (1 << 1)/*pdb_val*/,
	CAM4_ALL_ROI_MODE_MASK	   	   = 0x03,

	CAM4_ALL_ROI_TURN_ON_AUTO_ROT_DIS  = (1 << 2),
} /*pdb_end*/cam4_all_roi_mode_e;
/*  pdb_file:typedef.
 *  pdb_file:field(@id,[guitype=internal]).
 *  pdb_file:field(@x0,[html_addclass=video_sens_entrypxi_stl,nmsrc=sngt:'html_empty_str',units=px]).
 *  pdb_file:field(@x1,[html_addclass=video_sens_entrypxi_stl,nmsrc=sngt:'\"&mdash;\"',units=px]).
 *  pdb_file:field(@y0,[html_addclass=video_sens_entrypxi_stl,nmsrc=sngt:'html_empty_str',units=px]).
 *  pdb_file:field(@y1,[html_addclass=video_sens_entrypxi_stl,nmsrc=sngt:'\"&mdash;\"',units=px]).
 *  pdb_file:field(uint16_t,x0_persent,[format=frac256,off='offsetof(lb_rule_roi_t,x0)',html_addclass=video_sens_entrypersenti_stl,nmsrc=sngt:'html_empty_str',units=persent]).
 *  pdb_file:field(uint16_t,x1_persent,[format=frac256,off='offsetof(lb_rule_roi_t,x1)',html_addclass=video_sens_entrypersenti_stl,nmsrc=sngt:'\"&mdash;\"',units=persent]).
 *  pdb_file:field(uint16_t,y0_persent,[format=frac256,off='offsetof(lb_rule_roi_t,y0)',html_addclass=video_sens_entrypersenti_stl,nmsrc=sngt:'html_empty_str',units=persent]).
 *  pdb_file:field(uint16_t,y1_persent,[format=frac256,off='offsetof(lb_rule_roi_t,y1)',html_addclass=video_sens_entrypersenti_stl,nmsrc=sngt:'\"&mdash;\"',units=persent]).
 *  pdb_file:field(@mode,[nmsrc=sngt:'html_empty_str',enum=cam4_roi_mode_e,enum_iterf=video_sens_roimodeiterf,html_updjs='"exproi_display(this);"']).
 *  pdb_file:end('lb_rule_roi_t',[idx=[nsensor,nroi]]).
 * */
/*pdb_parse*/
typedef struct {
	uint16_t		x0;
	uint16_t		x1;
	uint16_t		y0;
	uint16_t		y1;
	uint8_t			mode;
	uint8_t			id;
/*pdb_noparse*/
	uint8_t			res[2];
/*pdb_parse*/
} __attribute__((packed)) lb_rule_roi_t; /* 12 */
/*  pdb_file:typedef.
 *  pdb_file:field(uint8_t,rmode,[guitype=internal]).
 *  pdb_file:field(+'CAM4_ALL_ROI_MODE_SIMPLE',[alias=rmode_advanced,format=chk1,guitype=checkboxext,html_updjs='\"mon_update_roi();\"']).
 *  pdb_file:field(+'CAM4_ALL_ROI_MODE_AUTO_ROT_DISABLE',[alias=rmode_dis]).
 *  pdb_file:end('lb_rule_all_roi_t',[idx=[nsensor]]).
 */
typedef struct {
	uint8_t			cnt;
	uint8_t			rmode;
/*pdb_noparse*/
	lb_rule_roi_t		roi[6]; /* 72 */
/*pdb_parse*/
} __attribute__((packed)) lb_rule_all_roi_t; /* 74 */

/*pdb_noparse*/
typedef struct {
	uint8_t			family;	/* type P3_CONFIG_RULES            */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_RULE_CAM_SYNC_T         */
	uint8_t			id;	/* ID sensor id                    */

	lb_rule_all_roi_t	settings;
} __attribute__((packed)) lb_rule_st_all_roi_t;

#define lb_rule_all_roi_t_DEFAULTS ((lb_rule_all_roi_t){})

typedef enum {
	CAM4_IRISVAL_EXTERNAL 		= 0,
	CAM4_IRISVAL_INTERNAL		= 1,
} cam4_auto_iris_level_type_e;

typedef enum /*pdb_enum*/{
	CAM4_IRIS_MODE_MANUAL		= 0/*pdb_val*/,
	CAM4_IRIS_MODE_AUTO		= 1/*pdb_val*/,
	CAM4_IRIS_MODE_OPEN 		= 2/*pdb_val*/,
	CAM4_IRIS_MODE_CLOSE		= 3/*pdb_val*/,
	CAM4_IRIS_MODE_PARTIAL_OPEN	= 4,
	CAM4_IRIS_MODE_MAX
} /*pdb_end*/cam4_iris_mode_e;

typedef enum {
	CAM4_IRIS_CAP_MANUAL		= (1 << CAM4_IRIS_MODE_MANUAL),
	CAM4_IRIS_CAP_AUTO		= (1 << CAM4_IRIS_MODE_AUTO),
	CAM4_IRIS_CAP_OPEN		= (1 << CAM4_IRIS_MODE_OPEN),
	CAM4_IRIS_CAP_CLOSE		= (1 << CAM4_IRIS_MODE_CLOSE),
	CAM4_IRIS_CAP_PARTIAL_OPEN	= (1 << CAM4_IRIS_MODE_PARTIAL_OPEN)
} cam4_iris_drive_caps_e;

typedef enum /*pdb_enum*/{
	CAM4_DRIVE_TYPE_DC_DRIVE	= 0/*pdb_val*/,
	CAM4_DRIVE_TYPE_P_IRIS		= 1
} /*pdb_end*/cam4_iris_drive_type_e;

typedef struct {
	uint32_t		caps;
} __attribute__((packed)) lb_rule_drive_type_caps_t;
/*
 *  pdb_file:typedef.
 *  pdb_file:field(@'expo_low',[units=usec,nmsrc=s:'\"autoexp_expo_lowhigh\"',html_addclass=autoexp_entryexpi_stl]).
 *  pdb_file:field(@'expo_high',[units=usec,nmsrc=sngt:'\"&mdash;\"',html_addclass=autoexp_entryexpi_stl]).
 *  pdb_file:field(@'res0',[guitype=internal]).
 *  pdb_file:end('lb_rule_auto_iris_t').
*/
/*pdb_parse*/ 
typedef struct {
	uint8_t			iris_mode;	/* cam4_iris_mode_e 	 */
	uint8_t			level_type;
	uint8_t			drive_type; 	/* reserved */
	uint8_t			res0;
	uint32_t		expo_low; 	/* AUTO_IRIS expo_low    */
	uint32_t		expo_high;	/* 	     & expo_high */
/*pdb_noparse*/ 
	uint32_t		res1[15];
/*pdb_parse*/ 
} __attribute__((packed)) lb_rule_auto_iris_t;
/*pdb_noparse*/ 

typedef struct {
	uint8_t			family;	/* type P3_CONFIG_RULES            */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_RULE_CAM_SYNC_T         */
	uint8_t			id;	/* ID sensor id                    */

	lb_rule_auto_iris_t	settings;
} __attribute__((packed)) lb_rule_st_auto_iris_t;

#define lb_rule_auto_iris_t_DEFAULTS ((lb_rule_auto_iris_t){		\
	.iris_mode		= 0,					\
	.expo_low		= 1000,					\
	.expo_high		= 10000,				\
	.level_type		= CAM4_IRISVAL_INTERNAL			\
})
/*
 * pdb_file:typedef.
 * pdb_file:field(@iris_mode,[guitype=selectext,enum=cam4_iris_mode_e,html_updjs='\"autoexp_exposure_display(this)\"']).
 * pdb_file:field(@drive_type,[enum=cam4_iris_drive_type_e]).
 * pdb_file:field(@res,[guitype=internal]).
 * pdb_file:end('lb_rule_iris_mode_t').
*/
/*pdb_parse*/ 
typedef struct {
	uint8_t			iris_mode;	/* cam4_iris_mode_e 	   */
	uint8_t			drive_type;	/* cam4_iris_drive_type_e  */
	uint8_t			part_step;	/* partial step		   */
	uint8_t			res0;
} __attribute__((packed)) lb_rule_iris_mode_t;
/*pdb_noparse*/ 

typedef struct {
	uint8_t			family;	/* type P3_CONFIG_RULES            */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_RULE_CAM_SYNC_T         */
	uint8_t			id;	/* ID sensor id                    */

	lb_rule_iris_mode_t	settings;
} __attribute__((packed)) lb_rule_st_iris_mode_t;

#define lb_rule_iris_mode_t_DEFAULTS ((lb_rule_iris_mode_t){		\
	.iris_mode		= CAM4_IRIS_MODE_MANUAL,		\
})
/*
 *  pdb_file:typedef.
 *  pdb_file:field(@'method',[nmsrc=sngt:html_empty_str,enum=cam4_gain_method_e]).
 *  pdb_file:field(@'mode',[guitype=internal]).
 *  pdb_file:field(@'roi_type',[enum=cam4_gain_roi_type_e]).
 *  pdb_file:field(@'res',[guitype=internal]).
 *  pdb_file:field(@'adaptation_time_sec',[html_addclass=autoexp_entry2_stl,nmsrc=sngt:html_empty_str,units=sec]).
 *  pdb_file:field(@'mono_reaction_time_sec',[html_addclass=autoexp_entry2_stl,units=sec]).
 *  pdb_file:end('lb_rule_gain_t').
*/
/*pdb_parse*/
typedef struct {
	uint8_t			method;
	uint8_t			mode;
	uint8_t			roi_type;
	uint8_t			res;
	uint16_t		adaptation_time_sec;
	uint16_t		mono_reaction_time_sec;
/*pdb_noparse*/
	uint8_t			res0[3];
	uint32_t		res1[7];
/*pdb_parse*/
} __attribute__((packed)) lb_rule_gain_t;
/*pdb_noparse*/
typedef struct {
	uint8_t			family;	/* type P3_CONFIG_RULES            */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_RULE_CAM_SYNC_T         */
	uint8_t			id;	/* ID sensor id                    */

	lb_rule_gain_t		settings;
} __attribute__((packed)) lb_rule_st_gain_t;

#define lb_rule_gain_t_DEFAULTS ((lb_rule_gain_t){			\
	.method			= CAM4_GAIN_METHOD_GREY_NORM,		\
	.mode			= CAM4_GAIN_MODE_NORMAL,		\
	.roi_type		= CAM4_GAIN_ROI_UNIFORM, 		\
	.adaptation_time_sec	= 5,					\
	.mono_reaction_time_sec	= 0,					\
})

#if 0
typedef enum /*pdb_enum*/
{
 CAMCTRL_EXPO_METHOD_MEDAVG/*pdb_val*/,
 CAMCTRL_EXPO_METHOD_MED/*pdb_val*/,
 CAMCTRL_EXPO_METHOD_AVG/*pdb_val*/,
 CAMCTRL_EXPO_METHOD_DICHOTOMY_MED,
 CAMCTRL_EXPO_METHOD_DICHOTOMY_AVG,
 CAMCTRL_EXPO_METHOD_OVERLIGHT_MIN/*pdb_val*/,
 CAMCTRL_EXPO_METHOD_MAXITEM
}/*pdb_end*/camctrl_eexpo_method;
#endif
/* -------------- New CamCtrl style ------------------ */
/*
 *  pdb_file:typedef.
 *  pdb_file:field(@'method',[enum=camctrl_eexpo_method]).
 *  pdb_file:field(@'asym_adapt_time',[units=sec,html_addclass=autoexp_entry4_stl]).
 *  pdb_file:field(@'expo_lvl_p',[nmsrc=sngt:'html_empty_str',units=persent,html_addclass=autoexp_entryp_stl]).
 *  pdb_file:field(@'expo_ovb_p',[units=persent,html_addclass=autoexp_entryplr_stl]).
 *  pdb_file:field(@'res',[guitype=internal]).
 *  pdb_file:end('lb_rule_camctrl_expo_mode_t',[idx=[nsensor,nexpomode]]).
*/
/*pdb_parse*/
typedef struct {
	uint16_t		method;
	int16_t			asym_adapt_time;
	uint8_t			expo_lvl_p;
	uint8_t			expo_ovb_p;
/*pdb_noparse*/
	uint8_t			res[6];
/*pdb_parse*/
} __attribute__((packed)) lb_rule_camctrl_expo_mode_t; /* 12 */
/*pdb_noparse*/
/*
 *  pdb_file:typedef.
 *  pdb_file:field(@'cam_angle_deg',[format=su12_20,units=deg]).
 *  pdb_file:field(@'sun_elevation_angle_deg',[format=su12_20,units=deg]).
 *  pdb_file:field(@'res',[guitype=internal]).
 *  pdb_file:end('lb_rule_camctrl_emode_condparam_t').
*/
/*pdb_parse*/
typedef struct {
	int32_t			cam_angle_deg;
	int32_t			sun_elevation_angle_deg;
	uint16_t		cond_th;
	uint16_t		res;
} __attribute__((packed)) lb_rule_camctrl_emode_condparam_t; /* 10 */
/*pdb_noparse*/
typedef struct {
	lb_rule_camctrl_emode_condparam_t 	start;
	lb_rule_camctrl_emode_condparam_t 	finish;
} __attribute__((packed)) lb_rule_camctrl_expo_switch_t; /* 20 */
#if 0
typedef enum /*pdb_enum*/
{
 CAMCTRL_EXPO_MODE_AUTO/*pdb_val*/,
 CAMCTRL_EXPO_MODE_NORMAL/*pdb_val*/,
 CAMCTRL_EXPO_MODE_BLACK/*pdb_val*/,
 CAMCTRL_EXPO_MODE_WHITE/*pdb_val*/,
 CAMCTRL_EXPO_MODE_CALIB,
 CAMCTRL_EXPO_MODE_MAXITEM
}/*pdb_end*/camctrl_eexpo_mode;
#endif
/*
 *  pdb_file:typedef.
 *  pdb_file:field(@'res0',[guitype=internal]).
 *  pdb_file:field(@'res1',[guitype=internal]).
 *  pdb_file:field(@'expo_mode',[nmsrc=sngt:html_empty_str,enum=camctrl_eexpo_mode,html_updjs='\"autoexp_exposure_display();\"']).
 *  pdb_file:field(@'expo_switch_type',[guitype=internal]).
 *  pdb_file:field(@'expo_molp',[guitype=internal,html_addclass=autoexp_entryp_stl]).
 *  pdb_file:field(@'scn_delay',[nmsrc=sngt:html_empty_str,units=sec,html_addclass=autoexp_entryp_stl]).
 *  pdb_file:field(@'expo_max',[units=usec,nmsrc=sngt:'\"&mdash;\"',html_addclass=autoexp_entryexpi_stl]).
 *  pdb_file:field(@'expo_min',[units=usec,nmsrc=s:'\"autoexp_expo_rngminmax\"',html_addclass=autoexp_entryexpi_stl]).
 *  pdb_file:field(@'expo_low',[guitype=internal]).
 *  pdb_file:field(@'expo_high',[guitype=internal]).
 *  pdb_file:field(@'expo_target_obj_size',[units=px,unused='t__',format=dec0,html_addclass=autoexp_entryexpi_stl]).
 *  pdb_file:field(@'iris_delay_msec',[guitype=internal]).
 *  pdb_file:field(@'last_profile_id',[guitype=stdvalue,enum=camctrl_eprofile_type]).
 *  pdb_file:end('lb_rule_camctrl_expo_t').
*/
/*pdb_parse*/
typedef struct {
	uint32_t		expo_max;
	uint32_t		expo_min;
	uint8_t			scn_delay;
	uint8_t			res0;
	uint8_t			expo_mode;	/* normal / black / white	*/
	uint8_t			res1;
	uint16_t		expo_switch_type; /* INDOOR / TRAFFIC / other	*/
	uint16_t		expo_molp;	/* maximum possible lum offset  */
/*pdb_noparse*/
	lb_rule_camctrl_expo_mode_t	expo_mode_params[4];
/*pdb_parse*/
	uint32_t		expo_low;	/* PAY ATTENTION! ONLY FOR PROFILE, NOT ACTUAL DATA */
	uint32_t		expo_high;	/* PAY ATTENTION! ONLY FOR PROFILE, NOT ACTUAL DATA */
	uint16_t		iris_delay_msec;
	uint8_t			last_profile_id;
	uint16_t		expo_target_obj_size;
/*pdb_noparse*/
	uint8_t			res2[2];
	lb_rule_camctrl_expo_switch_t	expo_mode_switch[4];
	uint32_t		res3[16];
/*pdb_parse*/
}  __attribute__((packed)) lb_rule_camctrl_expo_t;
/*pdb_noparse*/

typedef struct {
	uint16_t		cc_ver;
	uint16_t		cc_subver;
	uint16_t		cc_parver;
	uint16_t		res;
	uint32_t		ts;
	lb_string30_t		name;
	uint32_t		cc_len;
} __attribute__((packed)) lb_user_profile_blob_hdr_t;

typedef struct { /* XXX - OLD version */
	uint32_t		expo_max;
	uint32_t		expo_min;
	uint8_t			scn_delay;
	uint8_t			res0;
	uint8_t			expo_mode;	/* normal / black / white	*/
	uint8_t			res1;
	uint16_t		expo_switch_type; /* INDOOR / TRAFFIC / other	*/
	uint16_t		expo_molp;	/* maximum possible lum offset  */
	lb_rule_camctrl_expo_mode_t	expo_mode_params[4];
	uint32_t		expo_low;
	uint32_t		expo_high;
	uint16_t		iris_delay_msec;
	uint8_t			last_profile_id;
	uint8_t			res2[21];
}  __attribute__((packed)) lb_rule_camctrl_expo_old_t;

typedef struct {
	uint8_t			family;	/* type P3_CONFIG_RULES            */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_RULE_CAM_SYNC_T         */
	uint8_t			id;	/* ID sensor id                    */

	lb_rule_camctrl_expo_t	settings;
} __attribute__((packed)) lb_rule_st_camctrl_expo_t;
#if 0
typedef /*pdb_enum*/enum
{
 CAMCTRL_LUT_METHOD_LINEAR_1X/*pdb_val*/,
 CAMCTRL_LUT_METHOD_CONTRAST_ADAPTIVE_LINEAR/*pdb_val*/,
 CAMCTRL_LUT_METHOD_CONTRAST_EQUALIZATION/*pdb_val*/,
 CAMCTRL_LUT_METHOD_LIGHT_CORRECTION/*pdb_val*/,
 CAMCTRL_LUT_METHOD_MAXITEM
}/*pdb_end*/camctrl_elut_method;
#endif
/*
 * pdb_file:typedef.
 * pdb_file:field(@lut_method,[nmsrc=sngt:html_empty_str,html_addclass='\"\\\" style=\\\"width:30ex;\"',enum=camctrl_elut_method]).
 * pdb_file:field(uint8_t,lut_target,[guitype=internal]).
 * pdb_file:field(+'CAM4_LUT_TARGET_JPEG',[alias='lut_target_jpeg',guitype=checkboxext,html_updjs='\"autoexp_lut_display()\"']).
 * pdb_file:field(+'CAM4_LUT_TARGET_RAW',[alias='lut_target_raw',guitype=checkboxext,html_updjs='\"autoexp_lut_display()\"']).
 * pdb_file:field(@contrast_lvl_p,[nmsrc=s:'\"autoexp_lut_clp\"',units=persent,html_addclass=autoexp_entryplr_stl]).
 * pdb_file:field(@average_lvl_p,[nmsrc=sngt:html_empty_str,units=persent,html_addclass=autoexp_entryp_stl]).
 * pdb_file:field(@max_gain_p,[nmsrc=s:'\"autoexp_lut_mgp1\"',units=persent,html_addclass=autoexp_entryplr_stl]).
 * pdb_file:field(@lut_scn_delay,[units=sec,html_addclass='autoexp_entry2_stl']).
 * pdb_file:end('lb_rule_camctrl_lut_t').
 * */

/*pdb_parse*/
typedef struct {
	uint8_t			lut_method;
	uint8_t			lut_target;	/* apply LUT for JPEG / RAW	*/
	uint16_t		contrast_lvl_p;	/* contrast level percent	*/
	uint16_t		average_lvl_p;	/* average normal percent	*/
	uint16_t		max_gain_p;	/* max gain percent		*/
	uint16_t		lut_scn_delay;
/*pdb_noparse*/
	uint8_t			res[10];
/*pdb_parse*/
} __attribute__((packed)) lb_rule_camctrl_lut_t;
/*pdb_noparse*/
typedef struct {
	uint8_t			family;	/* type P3_CONFIG_RULES            */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_RULE_CAM_SYNC_T         */
	uint8_t			id;	/* ID sensor id                    */

	lb_rule_camctrl_lut_t	settings;
} __attribute__((packed)) lb_rule_st_camctrl_lut_t;

typedef struct {
	lb_rule_camctrl_expo_t		expo_param;
	lb_rule_camctrl_lut_t		lut_param;
	lb_rule_gain_t			gain_param;
	lb_rule_camctrl_expo_switch_t	__no_expo_mode_switch[3];
	uint8_t				cur_profile_id;
} __attribute__((packed)) lb_rule_camctrl_profile_t;

typedef struct {
	uint8_t			family;	/* type P3_CONFIG_RULES            */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_RULE_CAMCTRL_PROFILE_T  */
	uint8_t			id;	/* ID sensor id                    */

	lb_rule_camctrl_profile_t settings;
} __attribute__((packed)) lb_rule_st_camctrl_profile_t;

typedef enum {
	CAM4_CAMCTRL_MODE_MASK = 0xffff
} cam4_camctrl_mode_mask_e;
#if 0
typedef enum /*pdb_enum*/
{
 CAMCTRL_PROFILE_NORMAL/*pdb_val*/,     /* specifies param for universal applications  */
 CAMCTRL_PROFILE_FAST/*pdb_val*/,       /* specifies param for low delay applications  */
 CAMCTRL_PROFILE_TRAFFIC/*pdb_val*/,    /* specifies param for traffic applications    */
CAMCTRL_PROFILE_TRAFFIC_SURVEY/*pdb_val*/,
CAMCTRL_PROFILE_TRAFFIC_GATE/*pdb_val*/,

 CAMCTRL_PROFILE_INDOOR/*pdb_val*/,     /* specifies param for indoor applications     */
 CAMCTRL_PROFILE_OUTDOOR/*pdb_val*/,    /* specifies param for outdoor applications    */
 CAMCTRL_PROFILE_ENTRANCE/*pdb_val*/,   /* specifies param for entrance (object backlight) applications  */
 CAMCTRL_PROFILE_FACE_K/*pdb_val*/,     /* specifies param for NetCam K-series applications  */
 CAMCTRL_PROFILE_USER/*pdb_val*/,       /* specifies param for user   applications     */
 CAMCTRL_PROFILE_MAXITEM
} /*pdb_end*/ camctrl_eprofile_type;
#endif
/*
 * pdb_file:typedef.
 * pdb_file:field(uint8_t,profile,[enum=camctrl_eprofile_type,off=0,guitype=selectext,html_updjs='\"autoexp_profile_display()\"',nmsrc=sngt:html_empty_str]).
 * pdb_file:end('lb_rule_camctrl_profile_t').
 * */
/*
 * pdb_file:typedef.
 * pdb_file:field(uint16_t,mode,[guitype=internal]).
 * pdb_file:field(+'CAMCTRL_OPTION_AUTO_GAIN',[alias=mode_gain,guitype=checkboxext,html_updjs='\"autoexp_gain_display()\"',unused=autoexp_gain__]).
 * pdb_file:field(+'CAMCTRL_OPTION_AUTO_EXPOSURE',[alias=mode_exposure,guitype=checkboxext,html_updjs='\"autoexp_exposure_display()\"']).
 * pdb_file:field(-'CAMCTRL_OPTION_SYNCR0',[alias=mode_syncr0,html_updjs='\"autoexp_exposure_display()\"',enum=[s:'\"CAMCTRL_OPTION_EXPOSURE_SYNCHRO_NONE\"'=v:0,s:'\"CAMCTRL_OPTION_EXPOSURE_50HZ_SYNCHRO\"'=v:'CAMCTRL_OPTION_EXPOSURE_50HZ_SYNCHRO',s:'\"CAMCTRL_OPTION_EXPOSURE_60HZ_SYNCHRO\"'=v:'CAMCTRL_OPTION_EXPOSURE_60HZ_SYNCHRO',s:'\"CAMCTRL_OPTION_EXPOSURE_50HZ_SYNCHRO_FULL\"'=v:'CAMCTRL_OPTION_EXPOSURE_50HZ_SYNCHRO|CAMCTRL_OPTION_EXPOSURE_FULL_PERIOD_SYNCHRO',s:'\"CAMCTRL_OPTION_EXPOSURE_60HZ_SYNCHRO_FULL\"'=v:'CAMCTRL_OPTION_EXPOSURE_60HZ_SYNCHRO|CAMCTRL_OPTION_EXPOSURE_FULL_PERIOD_SYNCHRO']]).
 * pdb_file:end('lb_rule_camctrl_mode_t').
 */
/*pdb_parse*/
typedef struct {
	uint16_t		mode;
/*pdb_noparse*/
	uint8_t			res[2];
/*pdb_parse*/
} __attribute__((packed)) lb_rule_camctrl_mode_t; /* 4 */
/*pdb_noparse*/
typedef struct {
	uint8_t			family;	/* type P3_CONFIG_RULES            */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_RULE_CAM_SYNC_T         */
	uint8_t			id;	/* ID sensor id                    */

	lb_rule_camctrl_mode_t	settings;
} __attribute__((packed)) lb_rule_st_camctrl_mode_t;

#define lb_rule_camctrl_mode_t_DEFAULTS	((lb_rule_camctrl_mode_t){	\
		.mode	=	0					\
})
#if 0
typedef enum /*pdb_enum*/{
 CAMCTRL_CAM_ORNT_NORTH      = 0/*pdb_val*/,
 CAMCTRL_CAM_ORNT_NORTH_EAST = 45/*pdb_val*/,
 CAMCTRL_CAM_ORNT_EAST       = 90/*pdb_val*/,
 CAMCTRL_CAM_ORNT_SOUTH_EAST = 135/*pdb_val*/,
 CAMCTRL_CAM_ORNT_SOUTH      = 180/*pdb_val*/,
 CAMCTRL_CAM_ORNT_SOUTH_WEST = 225/*pdb_val*/,
 CAMCTRL_CAM_ORNT_WEST       = 270/*pdb_val*/,
 CAMCTRL_CAM_ORNT_NORTH_WEST = 315/*pdb_val*/,
 CAMCTRL_CAM_ORNT_NONE = 0xFFFF/*pdb_val*/,
 CAMCTRL_CAM_ORNT_MAXITEM    = 361
}/*pdb_end*/camctrl_ecam_orientation_;
#endif
#define CAMCTRL_CAM_ORNT_NONE 0xFFFFU
/*
 * pdb_file:typedef.
 * pdb_file:field(uint16_t,cam_orient,[guitype=selectext,enum=camctrl_ecam_orientation_,off=0,html_updjs='\"time_pos_display(this);\"']).
 * pdb_file:end('lb_rule_camctrl_cam_pos_sel_t').
 * pdb_file:typedef.
 * pdb_file:field(@cam_orient,[unused='orient',units=deg]).
 * pdb_file:field(uint32_t,latitude_deg,[alias=latitude_deg_val,format=u12_20,units=deg]).
 * pdb_file:field(-'CAMCTRL_CAM_POSDIR',[alias=latitude_deg_dir,enum=[s:'\"CAMCTRL_CAM_POSDIR_NORTH\"'=v:0,s:'\"CAMCTRL_CAM_POSDIR_SOUTH\"'=v:'CAMCTRL_CAM_POSDIR_MASK']]).
 * field(-'CAMCTRL_CAM_POSVAL',[alias=latitude_deg_val,format=u12_20,units=deg]).
 * pdb_file:field(uint32_t,longtitude_deg,[alias=longtitude_deg_val,format=u12_20,units=deg]).
 * pdb_file:field(-'CAMCTRL_CAM_POSDIR',[alias=longtitude_deg_dir,enum=[s:'\"CAMCTRL_CAM_POSDIR_EAST\"'=v:0,s:'\"CAMCTRL_CAM_POSDIR_WEST\"'=v:'CAMCTRL_CAM_POSDIR_MASK']]).
 * field(-'CAMCTRL_CAM_POSVAL',[alias=longtitude_deg_val,format=u12_20,units=deg]).
 * pdb_file:field(@longtitude_deg,[format=u12_20,units=deg]).
 * pdb_file:end('lb_rule_camctrl_cam_pos_t').
 */
/*pdb_parse*/
#define CAMCTRL_CAM_POSDIR_MASK 0x80000000
#define CAMCTRL_CAM_POSVAL_MASK 0x7FFFFFFF
typedef struct {
	uint16_t		cam_orient;
	uint32_t		latitude_deg;
	uint32_t		longtitude_deg;
/*pdb_noparse*/
	uint8_t			res[6];
/*pdb_parse*/
} __attribute__((packed)) lb_rule_camctrl_cam_pos_t;
/*pdb_noparse*/

typedef struct {
	uint8_t			family;	/* type P3_CONFIG_RULES            */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_RULE_CAM_SYNC_T         */
	uint8_t			id;	/* ID sensor id                    */

	lb_rule_camctrl_cam_pos_t settings;
} __attribute__((packed)) lb_rule_st_camctrl_cam_pos_t;

#define lb_rule_camctrl_cam_pos_t_DEFAULTS ((lb_rule_camctrl_cam_pos_t){ \
		.cam_orient 		= 	0,			 \
		.latitude_deg		=	0,			 \
		.longtitude_deg		=	0,			 \
})

typedef enum {
	CAM4_CC_SUNPOS_STATE_AVAIL = (1 << 0),
	CAM4_CC_IRIS_STATE_AVAIL   = (1 << 1),
	CAM4_CC_EXPO_STATE_AVAIL   = (1 << 2)
} lb_rule_camctrl_state_report_e;

typedef struct {
	uint32_t sun_cam_angle_deg;
	uint32_t sun_elevation_deg;
	uint32_t cond_th;
} __attribute__((packed)) lb_rule_camctrl_expo_mode_cond_param_t;

typedef struct {
	uint32_t set_switch_type;
	uint32_t set_mode_index;
	uint32_t target_expo_mode;
	lb_rule_camctrl_expo_mode_cond_param_t last_cond;
	lb_rule_camctrl_expo_mode_cond_param_t recom_cond;
} __attribute__((packed)) lb_rule_expo_mode_switch_state_t;

typedef struct {
	uint32_t cur_expo_mode;
	int32_t	 is_auto_expo_mode;
	lb_rule_expo_mode_switch_state_t expo_switch_state;
}  __attribute__((packed)) lb_rule_camctrl_expo_mode_state_t;

typedef struct {
	uint32_t cam_sun_deg;
	uint32_t elevation_deg;

   	uint32_t iris_cur_value;
   	uint32_t iris_target_value;
   	int32_t  iris_calib_progress;
   	int32_t  iris_state_flag;

	lb_rule_camctrl_expo_mode_state_t expo_mode_state;

	uint8_t	 avail;
	uint8_t	 res0[3];
	uint32_t res1[7];
} __attribute__((packed)) lb_rule_camctrl_state_t;


/* --------------------------------------------------- */
typedef struct {
	uint32_t		state;		/* */
	uint32_t		saved;		/* */
	uint32_t		factory;	/* */
	int32_t			read_out;	/* */
} __attribute__((packed)) lb_rule_dc_iris_state_t;

/* --------------------------------------------------- */
typedef struct {
	uint32_t		c_seq;		/* Frame sequence  */
	uint32_t		c_gs1_seq;	/* GS type 1 count */
	uint32_t		c_gs2_seq;	/* GS type 2 count */
} __attribute__((packed)) frame_sync_stat_t;

typedef struct {
	uint32_t		period;		/* in 1usec units */
	uint32_t		exposition;	/* in 1usec units */

	frame_sync_stat_t	cur;		/* current */
	uint32_t		expo_min;
	uint32_t		expo_max;
	int32_t			readout;

	uint32_t		period_min;
	uint32_t		period_max;

	uint32_t		period_syncs;	/* in 1usec units */
	uint32_t		syncs_in_period;/* period_syncs in frame period */

	uint32_t		reserved32[8];
} __attribute__((packed)) lb_rule_sync_state_t;

/* --------------------------------------------------- */
typedef enum {
	CAM4_TRACKER_SEND_PREDICTED	= 1<<0,
	CAM4_TRACKER_SEND_TRIAL		= 1<<1,
} cam4_tracker_flags_e;

typedef enum {
	CAM4_TRACKER_STATE_MASK		= 0xff,
	CAM4_TRACKER_STATE_PREDICTED	= 0x10,
	CAM4_TRACKER_STATE_TRIAL	= 0x03,
	CAM4_TRACKER_STATE_LEAVE	= 0xd0,
} cam4_tracker_state_e;
/*
 * pdb_file:typedef.
 * pdb_file:field(@dir,[enum=nc4_fd_detect_direction_e]).
 * pdb_file:field(@det_min_face_size,[units=bio_px,enum=nc4_fd_min_face_size_e]).
 * pdb_file:field(uint8_t,tflags,[guitype=internal]).
 * pdb_file:field(+'CAM4_TRACKER_SEND_PREDICTED',[alias=tflags_predicted]).
 * pdb_file:field(+'CAM4_TRACKER_SEND_TRIAL',[alias=tflags_trial]).
 * pdb_file:field(@trial_period,[nmsrc=sngt:'html_empty_str',units=ms]).
 * pdb_file:field(@leaving_period,[units=ms]).
 * pdb_file:field(@min_face_size,[nmsrc=sngt:'html_empty_str',html_add='bio_entryplr_stl',units=px]).
 * pdb_file:field(@max_face_size,[nmsrc=sngt:'\"&mdash;\"',html_add='bio_entryplr_stl',units=px]).
 * pdb_file:field(@scale,[format=frac256]).
 * pdb_file:end('lb_rule_dsp_t').
 * */

/*pdb_parse*/
typedef struct {
	uint8_t			threshold;		/* FD treshhold     */
	uint8_t			dir;			/* FD direction     */  //fd-face-angle
	uint8_t			det_min_face_size;	/* FD min obj size  */    //fd-cell
	uint8_t			tflags;			/* Tracker FLAGS    */  //fd-flags

	uint16_t		trial_period;		/* in filter size ms*/ // tf-in
	uint16_t		leaving_period;		/* out filter size  */ // tf-out

	uint16_t		scale;			/* scale area       */
	uint16_t		min_face_size;		/* FD min face size */  //dim-min

	uint16_t		max_face_size;		/* FD max face size */ //dim-max
/*pdb_noparse*/
	uint8_t			res1[18];
/*pdb_parse*/
} __attribute__((packed)) lb_rule_dsp_t;

/*pdb_noparse*/
#define lb_rule_dsp_t_DEFAULTS ((lb_rule_dsp_t){			\
	.threshold		= 2,					\
	.dir			= FACE_DETECT_DIRECTION_UP, 		\
	.det_min_face_size	= FACE_DETECT_MIN_SIZE_20x20,		\
	.min_face_size		= 0,					\
	.max_face_size		= 0,					\
	.trial_period		= 50,					\
	.leaving_period		= 1000,					\
	.scale			= 0x0180				\
})

typedef struct {
	uint8_t			family;	/* type P3_CONFIG_RULES            */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_RULE_DSP_T              */
	uint8_t			id;	/* ID sensor id                    */

	lb_rule_dsp_t		settings;
} __attribute__((packed)) lb_rule_st_dsp_t;

/* --------------------------------------------------- */

typedef enum {
	CAM4_DSP_MISC_OPT_H264_FD 	= (1 << 0), /* http streaming of 320x240 h.264    */
	CAM4_DSP_MISC_OPT_RTSP	  	= (1 << 1), /* rtsp streaming of full-frame h.264 */
	CAM4_DSP_MISC_OPT_H264		= (1 << 2), /* http streaming of full-frame h.264 */
	CAM4_DSP_MISC_OPT_RTSP_FD	= (1 << 3), /* rtsp streaming of 320x240 h.264	  */
	CAM4_DSP_MISC_OPT_FD		= (1 << 4), /* FaceDetector engine enable	  */
	CAM4_DSP_MISC_OPT_FD_FRAMES	= (1 << 5), /* allow to draw frames on FD h.264 ch*/
	CAM4_DSP_MISC_OPT_MASK	  	= 0xff
} cam4_dsp_misc_opts_e;

/*
 * pdb_file:typedef.
 * pdb_file:field(@wd_mode,[enum=nc4_dsp_wd_mode_e]).
 * pdb_file:field(uint8_t,opts,[guitype=internal]).
 * pdb_file:field(+'CAM4_DSP_MISC_OPT_H264_FD',[alias=opts_h264_fd]).
 * pdb_file:field(+'CAM4_DSP_MISC_OPT_RTSP',[alias=opts_rtsp]).
 * pdb_file:field(+'CAM4_DSP_MISC_OPT_H264',[alias=opts_h264]).
 * pdb_file:field(+'CAM4_DSP_MISC_OPT_RTSP_FD',[alias=opts_rtsp_fd]).
 * pdb_file:field(+'CAM4_DSP_MISC_OPT_FD',[alias=opts_fd]).
 * pdb_file:field(+'CAM4_DSP_MISC_OPT_FD_FRAMES',[alias=opts_fd_frames]).
 * pdb_file:end('lb_rule_dsp_misc_t').
 * */
/*pdb_parse*/
typedef struct {
	uint32_t		wd_mode;		/* DSP WD mode */
	uint8_t			opts;
	uint8_t			magic;
/*pdb_noparse*/
	uint8_t			res[2];
/*pdb_parse*/
} __attribute__((packed)) lb_rule_dsp_misc_t;
/*pdb_noparse*/

typedef struct {
	uint8_t			family;	/* type P3_CONFIG_RULES            */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_RULE_DSP_T              */
	uint8_t			id;	/* ID sensor id                    */

	lb_rule_dsp_misc_t	settings;
} __attribute__((packed)) lb_rule_st_dsp_misc_t;

#define lb_rule_dsp_misc_t_DEFAULTS ((lb_rule_dsp_misc_t){		\
	.wd_mode		= NC4_DSP_WD_OFF,			\
	.opts			= CAM4_DSP_MISC_OPT_H264 | CAM4_DSP_MISC_OPT_FD, \
})

/* --------------------------------------------------- */

typedef enum {
	CAM4_SHADOW_MASK_NOT_SET = 0,
	CAM4_SHADOW_MASK_SET	 = 1,
} cam4_shadow_mask_e;

typedef enum {
	CAM4_RAW_SHADOW_MASK_SPEC_SENSOR = 0,
	CAM4_RAW_SHADOW_MASK_USER	 = 1,
	CAM4_RAW_SHADOW_MASK_FULL_FRAME	 = 2,
} cam4_raw_shadow_mask_e;

typedef enum {
	CAM4_JPEG_SHADOW_MASK_SPEC_SENSOR = CAM4_RAW_SHADOW_MASK_SPEC_SENSOR,
	CAM4_JPEG_SHADOW_MASK_USER	  = CAM4_RAW_SHADOW_MASK_USER,
	CAM4_JPEG_SHADOW_MASK_FULL_FRAME  = CAM4_RAW_SHADOW_MASK_FULL_FRAME,
	CAM4_JPEG_SHADOW_MASK_AS_RAW	  = 3
} cam4_jpeg_shadow_mask_e;

typedef struct {
	uint8_t			mode;
	uint8_t			w;
	uint8_t			h;
	uint8_t			jmode;
	uint8_t			jw;
	uint8_t			jh;
	uint8_t			res[2];
} __attribute__((packed)) lb_rule_shadow_mask_t;

typedef struct {
	uint8_t			family;	/* type P3_CONFIG_RULES            */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_RULE_CAM_SHADOW_MASK_T  */
	uint8_t			id;	/* ID sensor id                    */

	lb_rule_shadow_mask_t	settings;
} __attribute__((packed)) lb_rule_st_shadow_mask_t;

#define lb_rule_shadow_mask_t_DEFAULTS ((lb_rule_shadow_mask_t){	\
	.mode			= CAM4_SHADOW_MASK_NOT_SET,		\
})

/* --------------------------------------------------- */

typedef enum/*pdb_enum*/ {
	CAM4_DC_IRIS_DIRECT_CONTROL		= 0/*pdb_val*/,
	CAM4_DC_IRIS_ADAPTATION_CONTROL		= 1/*pdb_val*/,
} /*pdb_end*/cam4_dc_iris_mode_e;

typedef enum /*pdb_enum*/ {
	CAM4_DC_IRIS_SPEED_LO_X1		= 0/*pdb_val*/,
	CAM4_DC_IRIS_SPEED_LO_X4		= 1/*pdb_val*/,
	CAM4_DC_IRIS_SPEED_HI_X8		= 2/*pdb_val*/,
	CAM4_DC_IRIS_SPEED_HI_X16		= 3/*pdb_val*/,
} /*pdb_end*/cam4_dc_iris_speed_e;

typedef enum {
	CAM4_P_IRIS_1_4,
	CAM4_P_IRIS_1_6,
	CAM4_P_IRIS_1_8,
	CAM4_P_IRIS_2_0,
	CAM4_P_IRIS_2_8,
	CAM4_P_IRIS_3_2,
	CAM4_P_IRIS_4_0,
	CAM4_P_IRIS_5_6,
	CAM4_P_IRIS_8_0,
	CAM4_P_IRIS_11_0,
	CAM4_P_IRIS_16_0,
	CAM4_P_IRIS_32_0
} cam4_p_iris_numbers_e;

typedef enum /*pdb_enum*/{
	CAM4_DC_IRIS_STATE_OPEN			= 0/*pdb_val*/,
	CAM4_DC_IRIS_STATE_CLOSE		= 1/*pdb_val*/,
	CAM4_DC_IRIS_STATE_PARTIAL_OPEN		= 2,
	CAM4_DC_IRIS_STATE_DISABLED		= 3,
	CAM4_DC_IRIS_STATE_MASK			= 3,
	CAM4_P_IRIS_1_4SH	=(CAM4_P_IRIS_1_4<<4)/*pdb_val*/,
	CAM4_P_IRIS_1_6SH	=(CAM4_P_IRIS_1_6<<4)/*pdb_val*/,
	CAM4_P_IRIS_1_8SH	=(CAM4_P_IRIS_1_8<<4)/*pdb_val*/,
	CAM4_P_IRIS_2_0SH	=(CAM4_P_IRIS_2_0<<4)/*pdb_val*/,
	CAM4_P_IRIS_2_8SH	=(CAM4_P_IRIS_2_8<<4)/*pdb_val*/,
	CAM4_P_IRIS_3_2SH	=(CAM4_P_IRIS_3_2<<4)/*pdb_val*/,
	CAM4_P_IRIS_4_0SH	=(CAM4_P_IRIS_4_0<<4)/*pdb_val*/,
	CAM4_P_IRIS_5_6SH	=(CAM4_P_IRIS_5_6<<4)/*pdb_val*/,
	CAM4_P_IRIS_8_0SH	=(CAM4_P_IRIS_8_0<<4)/*pdb_val*/,
	CAM4_P_IRIS_11_0SH	=(CAM4_P_IRIS_11_0<<4)/*pdb_val*/,
	CAM4_P_IRIS_16_0SH	=(CAM4_P_IRIS_16_0<<4)/*pdb_val*/,
	CAM4_P_IRIS_32_0SH	=(CAM4_P_IRIS_32_0<<4)/*pdb_val*/,
	CAM4_P_IRIS_MASK			= 0xF0
} /*pdb_end*/cam4_dc_iris_state_e;

#define CAM4_P_IRIS_LEVEL(dir_state) 		((dir_state & 0xf0) >> 4)
#define CAM4_DC_IRIS_DIR_STATE(dir_state)	(dir_state & 0x0f)

/* 
 * pdb_file:typedef.
 * pdb_file:field(@mode,[unused=expmanual,html_updjs='"iris_display(this)"',enum=cam4_dc_iris_mode_e]).
 * pdb_file:field(uint8_t,dir_state,[alias=dir_state_all,guitype=internal]).
 * pdb_file:field(-'CAM4_DC_IRIS_STATE',[alias=dir_state,unused=manual,enum=cam4_dc_iris_state_e:'CAM4_DC_IRIS_STATE']).
 * pdb_file:field(-'CAM4_P_IRIS',[alias=dir_statep,unused=manualp,enum=cam4_dc_iris_state_e:'CAM4_P_IRIS']).
 * pdb_file:field(@t_lvl,[unused=auto]).
 * pdb_file:field(@t_acc,[unused=auto]).
 * pdb_file:field(@dc_speed,[unused=auto,enum=cam4_dc_iris_speed_e]).
 * pdb_file:field(@pwm_dump,[guitype=internal]).
 * pdb_file:end('lb_rule_dc_iris_t').
 */
/*pdb_parse*/
typedef struct {
	uint8_t			mode;
	uint8_t			t_lvl;
	uint8_t			t_acc;
	uint8_t			dc_speed;
	uint8_t			pwm_dump;	/* value for pwm_dump 				*/
	uint8_t			dir_state;	/* high 4-bit = cam4_p_iris_numbers_e,
						   low - state to set in direct control mode 	*/
} __attribute__((packed)) lb_rule_dc_iris_t;
/*pdb_noparse*/
typedef struct {
	uint8_t			family;	/* type P3_CONFIG_RULES            */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_RULE_CAM_SYNC_T         */
	uint8_t			id;	/* ID sensor id                    */

	lb_rule_dc_iris_t	settings;
} __attribute__((packed)) lb_rule_st_dc_iris_t;

#define lb_rule_dc_iris_t_DEFAULTS ((lb_rule_dc_iris_t){ 		\
	.mode		=	0,					\
	.t_lvl		=	0x80,					\
	.t_acc		=	0x04,					\
	.dc_speed	=	0x03,					\
	.pwm_dump	=	0x8d,					\
	.dir_state	=	0,					\
})

#define lb_rule_cam_sin_t_DEFAULTS	((lb_rule_cam_sin_t){		\
})

#define lb_rule_cam_sout_t_DEFAULTS	((lb_rule_cam_sout_t){		\
})

#define lb_rule_cam_sens_t_DEFAULTS	((lb_rule_cam_sens_t){		\
	.flags		= 0,		/* TURN_0		  */	\
	.gains		= { 0x14, 0x14, 0x14, 0x14 }		\
})

#define lb_rule_cam_sync_t_DEFAULTS	((lb_rule_cam_sync_t){		\
	.mode		= 0,		/* FREE RUN		  */	\
	.exp		= 40000,	/* Exposition             */	\
	.period		= 0,		/* Period Delta           */	\
})

#define lb_rule_cam_jpeg_t_DEFAULTS	((lb_rule_cam_jpeg_t){		\
	.compression	= 8,		/* Compression level	  */	\
	.colour_mode	= 1,		/* BAYER_RGB		  */	\
})

#if defined(_MSC_VER) && !defined(__GNUC__)
#include <poppack.h>
#endif

#ifdef __cplusplus
}
#endif
#endif  /* __ABI_P3_LB_RULE_CAM_H__ */
