#ifndef __ABI_P3_LB_H__
#define __ABI_P3_LB_H__
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

#if defined(DEVICE_ISOLATED)
#include <isolated/abi/p3.h>
#else
#include <abi/p3.h>
#endif

/* Helper MACROS declarations */
#define LB_DEFAULTS(X) X##_DEFAULTS
#define LB_AJUST_SETTINGS(TYPE, VAL) TYPE##_ajust_values(VAL)

/* Constants declarations */
typedef enum {
	LB_TASK_STATE_OFF		= 0,
	LB_TASK_STATE_ON		= 1,
	LB_TASK_STATE_TIMEOUT_ON	= 2,
	LB_TASK_STATE_TIMEOUT_OFF	= 3,
} lb_event_bool_state_e;

/*\
 *  Data types declarations
\*/
#if defined(_MSC_VER) && !defined(__GNUC__)
#include <pshpack1.h>
#endif

/* Event Classes */
typedef enum {
       	LB_EVENT_BOOL_T,		/* see lb_event_bool_t          */
       	LB_EVENT_TIME_T,		/* see lb_event_time_t          */
       	LB_EVENT_TASK_T,		/* see lb_event_task_t          */
} lb_event_types_e;

/* Rule Types P3_CONFIG_RULES */
typedef enum {
       	LB_RULE_BOOL_T,			/* x00 lb_rule_bool_t		*/
       	LB_RULE_TIME_T,			/* x01 lb_rule_st_time_t        */
       	LB_RULE_ACTIVITY_T,		/* x02 lb_rule_st_activity_t    */
       	LB_RULE_TASKS_T,		/* x03 lb_rule_st_task_t	*/
       	LB_RULE_POWER_T,		/* x04 lb_rule_st_power_t	*/
	LB_RULE_ALCC_T,			/* x05 lb_rule_st_alcc_t	*/
       	LB_RULE_VMASK_T,		/* x06 lb_rule_st_vmask_t	*/
       	LB_RULE_OUT_T,			/* x07 lb_rule_st_out_t		*/
       	LB_RULE_WD_OUT_T,		/* x08 lb_rule_st_wd_out_t	*/
       	LB_RULE_CLIMATE_T,		/* x09 lb_rule_st_climate_t	*/
       	LB_RULE_CAM_SENS_T,		/* x0a lb_rule_st_cam_sens_t	*/
       	LB_RULE_TV_OUT_T,		/* x0b lb_rule_st_tv_out_t	*/
       	LB_RULE_CAM_SYNC_T,		/* x0c lb_rule_st_cam_sync_t	*/
       	LB_RULE_CAM_JPEG_T,		/* x0d lb_rule_st_cam_jpeg_t	*/
       	LB_RULE_CAM_SOUT_T,		/* x0e lb_rule_st_cam_sout_t	*/
       	LB_RULE_CAM_SIN_T,		/* x0f lb_rule_st_cam_sin_t	*/
	LB_RULE_CAM_CTRL_T,		/* x10 lb_rule_st_cam_ctrl_t	*/
	LB_RULE_DC_IRIS_T,		/* x11 lb_rule_st_dc_iris_t	*/
	LB_RULE_GAIN_T,			/* x12 lb_rule_gain_t		*/
	LB_RULE_AUTO_IRIS_T,		/* x13 lb_rule_auto_iris_t	*/
	LB_RULE_HIST_ROI_T,		/* x14 lb_rule_hist_roi_t	*/
	LB_RULE_DSP_T,			/* x15 lb_rule_dsp_t		*/
	LB_RULE_IRIS_MODE_T,		/* x16 lb_rule_iris_mode_t	*/
	LB_RULE_SENS_EXPO_T,		/* x17 lb_sens_expo_params_t	*/
	LB_RULE_CAMCTRL_MODE_T,		/* x18 lb_rule_camctrl_mode_t	*/
	LB_RULE_CAMCTRL_EXPO_T,		/* x19 lb_rule_camctrl_expo_t	*/
	LB_RULE_CAMCTRL_LUT_T,		/* x1a lb_rule_camctrl_lut_t	*/
	LB_RULE_CAMCTRL_CAM_POS_T,	/* x1b lb_rule_camctrl_cam_pos_t*/
	LB_RULE_CAMCTRL_PROFILE_T,	/* x1c lb_rule_camctrl_profile_t*/
	LB_RULE_SHADOW_MASK_T,		/* x1d lb_rule_shadow_mask_t	*/
	LB_RULE_H264_PROFILE_T,		/* x1e lb_rule_h264_profile_t	*/
	LB_RULE_HEATER_T,		/* x1f lb_rule_heater_t         */
	LB_RULE_AF_VOX_T,		/* x20 lb_rule_af_vox_t         */
	LB_RULE_AF_CID_T,		/* x21 lb_rule_af_cid_t         */
	LB_RULE_AF_MFT_T,		/* x22 lb_rule_af_mft_t         */
	LB_RULE_AF_LINE_T,		/* x23 lb_rule_af_line_t        */
	LB_RULE_AF_DTMF_T,		/* x24 lb_rule_af_dtmf_t        */
	LB_RULE_AF_CLI_T,		/* x25 lb_rule_af_cli_t         */
	LB_RULE_DSP_MISC_T,		/* x26 lb_rule_af_dtmf_t        */
	LB_RULE_SKIPPER8_T,		/* x27 lb_rule_skipper8_t       */
	LB_RULE_PSKIPPER32_T,		/* x28 lb_rule_pskipper32_t     */
	LB_RULE_H264_VIDEO_MISC_T,	/* x29 lb_rule_h264_video_misc_t */
} lb_rule_types_e;

/* Task Types P3_CONFIG_TASKS */
typedef enum {
       	LB_TASK_VIDEO_SIMPLE_T,		/*  0 */
       	LB_TASK_AUDIO_SIMPLE_T,		/*  1 */
       	LB_TASK_UART_SETTINGS_T,	/*  2 lb_task_st_uart_t         */
       	LB_TASK_RATE_SETTINGS_T,	/*  3 lb_task_st_rate_t         */
       	LB_TASK_COMPR_SETTINGS_T,	/*  4 lb_task_st_compr_t        */
       	LB_TASK_PTZ_T,			/*  5 lb_task_st_ptz_t          */
       	LB_TASK_CFG_AIN_T,		/*  6 lb_task_st_cfg_ain_t      */
       	LB_TASK_CFG_AOUT_T,		/*  7 lb_task_st_cfg_aout_t     */
       	LB_TASK_CFG_APOWER_T,		/*  8 lb_task_st_cfg_apower_t   */
       	LB_TASK_CFG_DEV_T,		/*  9 lb_task_st_cfg_dev_t      */
       	LB_TASK_CFG_MON_T,		/*  a lb_task_st_mon_t          */
       	LB_TASK_CFG_DIM_T,		/*  b lb_task_st_cfg_dim_t      */
       	LB_TASK_DEV_AIN_T,		/*  c lb_task_st_dev_ain_t      */
       	LB_TASK_CAM_LUT_T,		/*  d lb_task_st_lut_picewise1_t */
	LB_TASK_CAM_ROI_T,		/*  e lb_task_st_roi_t		*/
	LB_TASK_ROUT_T,			/*  f lb_task_st_rout_t 	*/
	LB_TASK_ROUT_SIMPLE_T,		/* 10 lb_task_st_rout_simple_t 	*/
} lb_task_types_e;

typedef enum {
       	LB_TASK_OLD		= (0<<0x00),	/* old params is valid  */
       	LB_TASK_NEW		= (1<<0x00),	/* params changed       */

       	LB_TASK_STOPPED		= (0<<0x01),	/* task is stopped      */
       	LB_TASK_STARTED		= (1<<0x01),	/* task is active       */

       	LB_TASK_IDLE		= (0<<0x02),	/* task has no session  */
       	LB_TASK_RUNNING		= (1<<0x02),	/* task has session     */
                                                                       
       	LB_TASK_INVALID		= (0<<0x03),	/* task is invalid      */
       	LB_TASK_VALID		= (1<<0x03),	/* task is valid        */
                                                                       
       	LB_TASK_TODO  		= (1<<0x04),	/* task to be done      */
                                                                       
       	LB_TASK_DETACH 		= (1<<0x05),	/* task to be detached  */
                                                                       
	LB_TASK_GMASK		= (0xff<<0x08),	/* task group mask      */
	LB_TASK_GRP_M0		= (1<<0x08),	/* task grp0 use mask   */
	LB_TASK_GRP_M1		= (1<<0x09),	/* task grp1 use mask   */
	LB_TASK_GRP_M2		= (1<<0x0a),	/* task grp2 use mask   */
	LB_TASK_GRP_M3		= (1<<0x0b),	/* task grp3 use mask   */
	LB_TASK_GRP_M4		= (1<<0x0c),	/* task grp4 use mask   */
	LB_TASK_GRP_M5		= (1<<0x0d),	/* task grp5 use mask   */
	LB_TASK_GRP_M6		= (1<<0x0e),	/* task grp6 use mask   */
	LB_TASK_GRP_M7		= (1<<0x0f),	/* task grp7 use mask   */
} lb_task_state_flags_e;

typedef enum {
	LB_TASK_ID_EMPTY	= 0xfffffffful,
	LB_TASK_ID_OFF		= 0x80000000ul,
	LB_TASK_ID_DETACHED	= 0x40000000ul,	/* detached task group  */
	LB_TASK_ID_MASK		= 0x3ffffffful,
} lb_task_id_e;

typedef enum {
	LB_RULE_ACTIVITY_ON		= (0x01<< 0),
	LB_RULE_ACTIVITY_OFF		= (0x00<< 0),

	LB_RULE_ACTIVE_YES		= (0x01<< 1),
	LB_RULE_ACTIVE_NO		= (0x00<< 1),

	LB_RULE_PRE_ACTIVE_YES		= (0x01<< 2),
	LB_RULE_PRE_ACTIVE_NO		= (0x00<< 2),

	LB_RULE_ACTIVE_SINGLE		= (0x01<< 3),
	LB_RULE_LOD_EVENT		= (0x01<< 4),
} lb_activity_rule_mode_e;

#define LB_TASK_CHANGED(X)	((((X)->flags)>>0)&1)
#define LB_TASK_ATASK(X)	((((X)->flags)>>1)&1)
#define LB_TASK_SESSION(X)	((((X)->flags)>>2)&1)

#define LB_TASK_SET_RUNNING(X)	((X)->flags |=  LB_TASK_RUNNING)
#define LB_TASK_SET_IDLE(X)	((X)->flags &= ~LB_TASK_RUNNING)

#define LB_TASK_SET_VALID(X)	((X)->flags |=  LB_TASK_VALID)
#define LB_TASK_SET_INVALID(X)	((X)->flags &= ~LB_TASK_VALID)

#define LB_TASK_START(X)	((X)->flags |=  LB_TASK_STARTED)
#define LB_TASK_STOP(X)		((X)->flags &= ~LB_TASK_STARTED)

#define LB_TASK_IS_ACTIVE(X)	((X)->flags & LB_TASK_STARTED)

#define LB_TASK_IS_RUNNING(X)	((X)->flags & LB_TASK_RUNNING)

#define LB_TASK_IS_VALID(X)	((X)->flags & LB_TASK_VALID)
#define LB_TASK_IS_INVALID(X)	(!LB_TASK_IS_VALID(X))

#define LB_TASK_IS_TODO(X)	((X)->flags & LB_TASK_TODO)

typedef struct {
       	uint32_t	type;
       	uint32_t	len;
       	int64_t		*time;
} __attribute__((packed)) lb_event_hdr_t;

typedef struct {
	lb_event_hdr_t	hdr;
} __attribute__((packed)) lb_event_time_t;

typedef struct {
	lb_event_hdr_t	hdr;
       	uint8_t		data[];
} __attribute__((packed)) lb_event_any_t;

typedef struct {
	filetime	start;		/* task  start time */
	filetime	finish;		/* task finish time */

	uint16_t	state;		/* task state       */
	uint16_t	priority;	/* task priority    */

	uint32_t	task;		/* task todo id     */

	p3_id_t		*dev;		/* device/flow id   */
} __attribute__((packed)) lb_event_task_t;

typedef struct {
	p3_id_t		*dev;		/* device/flow id   */

	uint32_t	len;		/* rule len in bytes */
	void		*rules;
	
} __attribute__((packed)) lb_rules_t;

typedef struct {
	uint8_t		type;		/* task type id     */
	uint8_t		group;		/* group id         */
} __attribute__((packed)) lb_task_st_hdr_t;

typedef struct {
	uint32_t		rate;	/* average data rate */
} __attribute__((packed)) lb_task_rate_options_t;

typedef struct {
	lb_task_st_hdr_t	hdr;
	lb_task_rate_options_t	options;
} __attribute__((packed)) lb_task_st_rate_t;

typedef struct {
	uint32_t		rate;
} __attribute__((packed)) lb_task_rate_t;

typedef struct {
	uint8_t			family;	/* type P3_CONFIG_TASKS            */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_TASK_RATE_SETTINGS_T    */
	uint8_t			id;	/* ID sensor id                    */

	lb_task_rate_t		settings;
} __attribute__((packed)) lb_task_st_rate_t_new;

typedef struct {
	uint32_t		compr;	/* average data compr */
} __attribute__((packed)) lb_task_compr_options_t;

typedef struct {
	lb_task_st_hdr_t	hdr;
	lb_task_compr_options_t	options;
} __attribute__((packed)) lb_task_st_compr_t;

typedef struct {
	uint32_t		compression;
} __attribute__((packed)) lb_task_compr_t;

typedef struct {
	uint8_t			family;	/* type P3_CONFIG_TASKS            */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_TASK_COMPR_SETTINGS_T   */
	uint8_t			id;	/* ID sensor id                    */

	lb_task_compr_t		settings;
} __attribute__((packed)) lb_task_st_compr_t_new;


typedef struct {
	uint32_t		flags;
} __attribute__((packed)) lb_task_cfg_mon_t;

typedef struct {
	uint8_t			family;	/* type P3_CONFIG_TASKS            */
	uint8_t			size;	/* BLOB size                       */
	uint8_t			type;	/* type LB_TASK_CFG_MON_T          */
	uint8_t			id;	/* ID group id                     */

	lb_task_cfg_mon_t	settings;
} __attribute__((packed)) lb_task_st_cfg_mon_t;

typedef struct {
	uint8_t		type;		/* type                            */
	uint8_t		size;		/* BLOB size                       */
	uint8_t		data[0];	/* data                            */
} __attribute__((packed)) lb_st_atom_t;

typedef struct {
	uint8_t		family;		/* type P3_CONFIG_NET              */
	uint8_t		idx;		/* index inside type               */
	uint8_t		type;		/* type P3_CONFIG_NET_IPV4         */
	uint8_t		str[30];	/* Stat Send period                */
} __attribute__((packed)) lb_st_string_idx_t;

typedef struct {
	uint8_t		family;		/* type P3_CONFIG_NET              */
	uint8_t		size;		/* BLOB size                       */
	uint8_t		type;		/* type P3_CONFIG_NET_IPV4         */
	uint8_t		str[254];	/* Stat Send period                */
} __attribute__((packed)) lb_st_string_t;

typedef struct {
	uint8_t		size;
	uint8_t		str[30];
	uint8_t		zero;
} __attribute__((packed)) lb_string30_t;

typedef struct {
	uint8_t		size;
	uint8_t		str[254];
	uint8_t		zero;
} __attribute__((packed)) lb_string254_t;

typedef struct {
	uint8_t		family;		/* type P3_CONFIG_NET              */
	uint8_t		size;		/* BLOB size                       */
	uint8_t		type;		/* sub type                        */
} __attribute__((packed)) lb_st_bool_t;

typedef struct {
	uint8_t		family;		/* type P3_CONFIG_NET              */
	uint8_t		size;		/* BLOB size EQ 1                  */
	uint8_t		type;		/* type of flag                    */
	uint8_t		flag;		/* flag value                      */
} __attribute__((packed)) lb_st_flag8_t;

#if defined(_MSC_VER) && !defined(__GNUC__)
#include <poppack.h>
#endif

#ifdef __cplusplus
}
#endif
#endif  /* __ABI_P3_LB_H__*/
