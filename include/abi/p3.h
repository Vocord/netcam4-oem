#ifndef __ABI_P3_H__
#define __ABI_P3_H__
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
 * WARNING:  UTF-8
 *
 *    TODO:  Make all descriptions using English.
\*/
#include <compiler.h>

#if defined(_MSC_VER) && !defined(__GNUC__)
#include <compilers/visualc/inttypes.h>
#include <compilers/visualc/netinet/in.h>
#else
#include <inttypes.h>
#include <netinet/in.h>
#endif

# if __BYTE_ORDER == __BIG_ENDIAN
/* The host byte order is the same as network byte order,
   so these functions are all just identity.  */
# define __CPU_TO_LE16(X)	__BSWAP_CONSTANT_16 (X)
# else
#  if __BYTE_ORDER == __LITTLE_ENDIAN
# define __CPU_TO_LE16(X)	(X)
#  else
#   error __BYTE_ORDER undefined 
#  endif
# endif

/* Выделение пространства кодов операций ABI по модулям */
/* Изменение данного определения без изменения версии ABI */
/* запрещено */
typedef enum {
        P3_ABI_CFM0            = 0x00,     /* first опкод Config Manager     */
        P3_ABI_CFM0_           = 0x07,     /* last опкод  Config Manager     */
        P3_ABI_LAST0           = P3_ABI_CFM0_,  /*  */
        P3_ABI_MDM             = 0x1000,   /* first опкод Media Data Manager */
        P3_ABI_MDM_            = 0x10ff,   /* last опкод  Media Data Manager */
        P3_ABI_AM              = 0x1100,   /* first опкод Attribute Manager  */
        P3_ABI_AM_             = 0x11ff,   /* last опкод  Attribute Manager  */
        P3_ABI_LB              = 0x1200,   /* first опкод Logic Manager      */
        P3_ABI_LB_             = 0x12ff,   /* last опкод  Logic Manager      */
        P3_ABI_NSM             = 0x1300,   /* first опкод NameSpace Manager  */
        P3_ABI_NSM_            = 0x13ff,   /* last опкод  NameSpace Manager  */
        P3_ABI_DEVICE1         = 0x1400,   /* first опкод Device Manager     */
        P3_ABI_DEVICE1_        = 0x14ff,   /* last опкод  Device Manager     */
        P3_ABI_CFM1            = 0x1500,   /* first опкод Config Manager     */
        P3_ABI_CFM1_           = 0x15ff,   /* last опкод  Config Manager     */
        P3_ABI_TSAPI           = 0x1600,   /* first tcode for TSAPI CC Mngr  */
        P3_ABI_TSAPI_          = 0x163f,   /* last tcode  for TSAPI CC Mngr  */
        P3_ABI_LAST1           = P3_ABI_CFM1_,  /*  */
} p3_abi_ops_e;

/*   Определение общих кодов типов данных ABI                                */
/*   Изменение данного определения возможно только путем добавления новых    */
/* кодов типов данных в зарезервированном для этого окне 0x00-0x7f           */
/*   Изменение кодов типов данных определения без изменения версии ABI       */
/* запрещено */
typedef enum {
        /* type codes */
        P3_ABI_ID_T            = 0x00,   /* p3_id_t идентификатор устройства */
        P3_ABI_DST_ID_T        = 0x01,   /* направить медиа поток в p3_id_t  */
        P3_ABI_ASYNC_CONTEXT_T = 0x02,   /* контекст для асинхронных операций*/
        P3_ABI_STATUS_T        = 0x03,   /* p3_status_e статус команды       */
        P3_ABI_SYNC_T          = 0x04,   /* p3_sync_t для валидации ABI data */
        P3_ABI_QOS_T           = 0x05,   /* p3_qos_e для QOS обслуживания    */
        P3_ABI_FRAME_DATA_T    = 0x06,   /* данные фрейма данных             */
        P3_MSG_PART_T          = 0x07,   /* p3_msg_part_t                    */
        P3_HOST_ID_T           = 0x08,   /* uint8_t[] идентификатор host     */
        P3_HOST_NAME_T         = 0x09,   /* UTF-8 hostname                   */
        P3_SYSTEM_NAME_T       = 0x0a,   /* UTF-8 Имя системы                */
        P3_COMPONENT_E         = 0x0b,   /* p3_component_e тип агента системы*/
        P3_CONN_IPV4_T         = 0x0c,   /* Адрес протокола для IP v4        */
        P3_CONN_IPV6_T         = 0x0d,   /* Адрес протокола для IP v6        */
        P3_SERVER_PARAM_T      = 0x0e,   /* p3_load_param_t as server load   */
        P3_CLIENT_PARAM_T      = 0x0f,   /* p3_load_param_t as client load   */
        P3_INDEX_T             = 0x10,   /* uint32_t used to index something */
        P3_CLIENT_ID_T         = 0x11,   /* p3_client_id_t used as client id */
        P3_LOAD_REQUEST_T      = 0x12,   /* массив 32bit p3_resource_e loads */
        P3_MAC_T               = 0x13,   /* MAC адрес в виде uint8_t[6]      */
        P3_NS_SERVER_T         = 0x14,   /* Адрес сервера DNS UTF8 uint8_t[] */
        P3_DEVICE_NAME_T       = 0x15,   /* UTF-8 Имя устройства             */
        
        P3_ABI_TIME_T          = 0x16,   /* timestamp filetime               */
        P3_ABI_TIME_POS_T0_T   = 0x17,   /* filetime data flow request start */
        P3_ABI_TIME_POS_T1_T   = 0x18,   /* filetime data flow request stop  */
        P3_ABI_TIME_POS_TE_T   = 0x19,   /* filetime data flow request end   */
        P3_ABI_TIME_POS_RX_T   = 0x1a,   /* lseek filetime входящего потока  */
        P3_ABI_TIME_POS_TX_T   = 0x1b,   /* lseek filetime исходящего потока */

        P3_ABI_MAX_BUFF_T      = 0x1c,   /* maximum input buffer for frame   */
        P3_ABI_LEN_BUFF_T      = 0x1d,   /* data frame length                */

        P3_ABI_PLAY_RATE_T     = 0x1e,   /* sustained play rate              */
        P3_ABI_READ_RATE_T     = 0x1f,   /* requested read rate              */
        P3_ABI_SKIP_RATE_T     = 0x20,   /* requested read skip rate         */
        
        P3_ABI_FRAME_INFO_T    = 0x21,   /* frame info see p3_frame_descr_t  */
        P3_ABI_FRAME_FLAG_T    = 0x22,   /* frame info see p3_frame_descr_t  */
        P3_ABI_FRAME_SIZE_T    = 0x23,   /* frame info see p3_frame_descr_t  */
        P3_ABI_FRAME_ADDR_T    = 0x24,   /* frame addr as filetime           */
        P3_ABI_FRAME_OFFS_T    = 0x25,   /* frame offset from its addr       */

        P3_ABI_RESERVED0_T     = 0x26,   /* reserved0                        */
        P3_ABI_BAND_WIDTH_T    = 0x27,   /* allocate specified band          */
        P3_ABI_PRIORITY_T      = 0x28,   /* Priority for data flow           */

        P3_ABI_GID_T           = 0x29,   /* p3_gid_t p3 subsystem/flow GUID  */
        P3_ABI_LID_T           = 0x2a,   /* p3_lid_t p3 subsystem/FLOW LID   */
	
        P3_NS_INFO_T           = 0x2b,   /* структура p3_ns_info_t           */
        P3_ABI_FLOW_OFFS_T     = 0x2c,   /* flow offset from its container   */

	P3_ABI_PACKET_ID_T     = 0x2d,   /* packet sequence ident (uint32_t) */
	
	P3_ABI_OFFSET_GID_T    = 0x2e,	 /* Partial GID for flows offset MSB */

	P3_ABI_NODE_N_	       = 0x2f,	 /* Node for grouping param blocks   */

	// Параметры Видео Tasks
	P3_ABI_VRESOLUTION_T   = 0x30,	 /* p3_resolution_e                  */
	P3_ABI_VSKIP_T	       = 0x31,	 /* Skip before each Vframe capture  */
	P3_ABI_VPERIODU_T      = 0x32,	 /* Capture every VPERIOD usecs      */

	// Параметры заданий Tasks
	P3_ABI_TASK_ID_T       = 0x33,	 /* Task Id uint32_t                 */
	P3_ABI_TASK_TYPE_T     = 0x34,	 /* lb_task_types_e                  */
	P3_ABI_TASK_FLAGS_T    = 0x35,	 /* lb_task_state_flags_e            */

	// Параметры потоков
        P3_ABI_FLOW_FLAG_T     = 0x36,   /* flow flags from device_flags_e   */
        P3_ABI_FLOW_TYPE_T     = 0x37,   /* flow type from device_type_e     */
        P3_ABI_FLOW_STATE_T    = 0x38,   /* flow state see device_event_t    */

	// Security
	P3_SECURITY_MODE_T     = 0x39,	 /* See p3_security_mode_e */

        /* device opcodes reservation */
        P3_ABI_DEVICE          = 0x50,   /* first опкод Device Manager       */
        P3_ABI_DEVICE_         = 0x6f,   /* last опкод  Device Manager       */

        /* SUB NODES */
        P3_ABI_ITEM_N          = 0xa3,   /* Контейнер связанных элементов    */
        P3_ABI_REQUEST_N       = 0xa3,   /* Контейнер элементов запроса      */

        /* Delegated type codes */
        P3_ABI_T_CFM           = 0xc8,   /* first ткод для Config Manager    */
        P3_ABI_T_CFM_          = 0xe7,   /* last ткод  Config Manager        */
        P3_ABI_T_DEVICE        = 0xe8,   /* first ткод для Device Manager    */
        P3_ABI_T_DEVICE_       = 0xff,   /* last ткод  Device Manager        */
        P3_ABI_T_MDM           = 0x100,  /* first ткод для Media Data Manager*/
        P3_ABI_T_MDM_          = 0x1ff,  /* last ткод  Media Data Manager    */
        P3_ABI_T_AM            = 0x200,  /* first ткод для Attribute Manager */
        P3_ABI_T_AM_           = 0x2ff,  /* last ткод  Attribute Manager     */
        P3_ABI_T_LB            = 0x300,  /* first ткод для Logic Manager     */
        P3_ABI_T_LB_           = 0x3ff,  /* last ткод  Logic Manager         */
        P3_ABI_T_NSM           = 0x400,  /* first ткод для NameSpace Manager */
        P3_ABI_T_NSM_          = 0x4ff,  /* last ткод  NameSpace Manager     */
        P3_ABI_T_TSAPI         = 0x500,  /* first tcode for TSAPI CC Manager */
        P3_ABI_T_TSAPI_        = 0x53f,  /* last tcode  for TSAPI CC Manager */

        P3_ABI_T_LAST,		/*  */

        P3_ABI_N_FIRST	      = 0x2f000000, /*                               */
        P3_INTERFACE_N        = 0x2f000000, /* Контейнер интерфейсов системы */
        P3_SERVER_N           = 0x2f000001, /*- not used */
        P3_CLIENT_N           = 0x2f000000, /* Контейнер информации о клиенте*/
        P3_ABI_TASK_N         = 0x2f000004, /* Контейнер параметров заданий  */
        P3_ABI_TASK_AUDIO_N   = 0x2f000005, /* Параметры Audio заданий       */
        P3_ABI_TASK_VIDEO_N   = 0x2f000006, /* Параметры Video заданий       */
	P3_ABI_FLOW_STATE_N   = 0x2f000007, /* Параметры состояния потоков   */

        P3_ABI_N_LAST,		/*  */
} p3_abi_types_e;

/* Declare ABI String constant */
#define ABI_STRING_CONST(NAME, VAL)	\
static inline uint8_t *abi_const_ ## NAME (void) __attribute__((unused)); \
static inline uint8_t *abi_const_ ## NAME (void) { return (uint8_t*)VAL; } \
static inline size_t   abi_csize_ ## NAME (void)  __attribute__((unused));\
static inline size_t   abi_csize_ ## NAME (void) { return sizeof(VAL) - 1; }

#define ABI_GETA(NAME)	(abi_const_ ## NAME ())
#define ABI_SIZEA(NAME) (abi_csize_ ## NAME ())

ABI_STRING_CONST(P3_ABI_IF_CAPS,	"If Caps");
ABI_STRING_CONST(P3_AUTH_CAPS,		"Auth Capabilities");
ABI_STRING_CONST(P3_AUTH_REQUEST,	"AUTH Request");
ABI_STRING_CONST(P3_AUTH_RESPONSE,	"AUTH Response");
ABI_STRING_CONST(P3_AUTH_STATUS,	"Auth Status");

ABI_STRING_CONST(P3_CM_TUNNEL_START,	"CM Tunnel Start");
ABI_STRING_CONST(P3_CM_TUNNEL_STOP,	"CM Tunnel Stop");
ABI_STRING_CONST(P3_ACA_CONFIG,		"ACA Config");

ABI_STRING_CONST(P3_AUTH_CAPS_RX_T,	"/Auth Capabilities RX");
ABI_STRING_CONST(P3_AUTH_CAPS_TX_T,	"/Auth Capabilities TX");
ABI_STRING_CONST(P3_AUTH_INFO_T,	"/Auth Info");

ABI_STRING_CONST(P3_ABI_FRAME_ENCRYPTED,"/encrypted-frame");
ABI_STRING_CONST(P3_ABI_SIZE_T,		"size");
ABI_STRING_CONST(P3_ABI_NONCE_T,	"nonce");
ABI_STRING_CONST(P3_ABI_XS8_T,		"xs8");

ABI_STRING_CONST(P3_TEVENT_T,		"Tevent");
ABI_STRING_CONST(P3_SEVENT_T,		"Sevent");

ABI_STRING_CONST(P3_DEVICE_SUB_MODEL_T,	"SubModel");
ABI_STRING_CONST(P3_FIRMWARE_REV_T,	"FirmwareRev");
ABI_STRING_CONST(P3_FW_MAIN_REV_T,	"FW-main");
ABI_STRING_CONST(P3_FW_RESCUE_REV_T,	"FW-rescue");

ABI_STRING_CONST(P3_HASH_TYPE_T,	"HASH");
ABI_STRING_CONST(P3_ENCRYPT_TYPE_T,	"CRYPT");
ABI_STRING_CONST(P3_SECURITY_MODE_T,	"Security Mode");
ABI_STRING_CONST(P3_USERNAME_T,		"Username");
ABI_STRING_CONST(P3_TS_T,		"Timestamp");
ABI_STRING_CONST(P3_TS2_T,		"Timestamp2");
ABI_STRING_CONST(P3_CHALLENGE_T,	"Challenge");
ABI_STRING_CONST(P3_CHALLENGE2_T,	"Challenge2");
ABI_STRING_CONST(P3_PASSWORD_T,		"Password");
ABI_STRING_CONST(P3_REASON_MSG_T, 	"Reason");

ABI_STRING_CONST(P3_FORMAT_TAG_T,	"Format tag");
ABI_STRING_CONST(P3_FORMAT_TAG_TYPE_T,	"Format tag type");
ABI_STRING_CONST(P3_FLOW_FORMAT_ID_T,	"Format id");

ABI_STRING_CONST(P3_ABI_AFLOW_GID_T,		"AGID");	/* Attached Partial GID */
ABI_STRING_CONST(P3_ABI_OOB_DATA,		"/OOB DATA");	/* Out of Band Data */

ABI_STRING_CONST(P3_ABI_STAGE_T,		"stage");	/* some stage */
ABI_STRING_CONST(P3_ABI_DATA_T,			"data");	/* some data */
ABI_STRING_CONST(P3_ABI_OFFSET32_T,		"offs");	/* some offset */

ABI_STRING_CONST(P3_ABI_UNACK_FRAMES_T,		"unack-frames");
/* Sensor related ABI http://wiki.vocord.com/spec-arch-p3ss-cm-sensor */
ABI_STRING_CONST(P3_ABI_SENSOR_N,		"/sensor");	/* Sensor Node */
ABI_STRING_CONST(P3_ABI_EXPOSITION_T,		"exposition");	/* Sensor Exposition */
ABI_STRING_CONST(P3_ABI_SENSOR_GAINS_T,		"sensor-gains");/* Sensor Gains */
ABI_STRING_CONST(P3_ABI_SENSOR_G_IDX_T,		"gains-idx");	/* Sensor Gains types */
ABI_STRING_CONST(P3_ABI_SENSOR_LUT_T,		"sensor-lut");	/* Sensor LUT */
ABI_STRING_CONST(P3_ABI_SENSOR_ROI_T,		"sensor-roi");	/* Sensor ROI */

/* Cam Ctrl data */
ABI_STRING_CONST(P3_ABI_CAMCTRL_STATE_T,	"camctrl-state");	/* CAMCTRL State */
ABI_STRING_CONST(P3_ABI_CAMCTRL_LHIST_T,	"camctrl-lhist");	/* CAMCTRL Luma Hist */

ABI_STRING_CONST(P3_ABI_2D_BARS_T,		"2d-bars");	/* 2d bars hdr p3_2d_bars_t       */
ABI_STRING_CONST(P3_ABI_RAW_IMAGE_T,		"2d-raw-child");/* 2D RAW image hdr p3_2d_child_t */
ABI_STRING_CONST(P3_ABI_RAW_VIDEO_SOURCE_T,	"raw-video-root");/* RAW source hdr video_frame_raw_hdr_t */

/* FPN related ABI http://wiki.vocord.com/spec-arch-p3ss-cm-fpn */
ABI_STRING_CONST(P3_ABI_FPN_PARAMS_T,		"fpn-params");	/* p3_fpn_params_t */
ABI_STRING_CONST(P3_ABI_FPN_STATE_T,		"fpn-state");	/* p3_fpn_state_t  */

ABI_STRING_CONST(P3_ABI_HIST_T,			"hist");	/* p3_histogram_t  */
/* I/O related settings ABI http://wiki.vocord.com/spec-arch-p3ss-cm-wire */
ABI_STRING_CONST(P3_ABI_WIRE_N,			"/wire");
ABI_STRING_CONST(P3_ABI_ROUT_STASK_T,		"r-out-stask");	/* lb_task_rout_simple_t */ 
ABI_STRING_CONST(P3_ABI_ROUT_T,			"r-out");	/* lb_task_rout_t 	 */
ABI_STRING_CONST(P3_ABI_DOUT_T,			"d-out");	/* lb_task_dout_t 	 */
ABI_STRING_CONST(P3_ABI_DIN_T,			"d-in");	/* lb_rule_din_t	 */

ABI_STRING_CONST(P3_ABI_BIT_MASK_T,		"bmsk");
ABI_STRING_CONST(P3_ABI_RESOLUTION_X_T,		"resX");
ABI_STRING_CONST(P3_ABI_RESOLUTION_Y_T,		"resY");
ABI_STRING_CONST(P3_ABI_SENSITIVITY_T,		"sensitivity");
ABI_STRING_CONST(P3_ABI_MIN_DURATION_T,		"min-duration");
ABI_STRING_CONST(P3_ABI_THRESHOLD_T,		"threshold");
ABI_STRING_CONST(P3_ABI_TEACH_SPEED_T,		"teach speed");
ABI_STRING_CONST(P3_ABI_FLAGS_T,		"flags");
ABI_STRING_CONST(P3_ABI_POST_RECORD_T,		"postrecord");
ABI_STRING_CONST(P3_ABI_PRE_RECORD_T,		"prerecord");
ABI_STRING_CONST(P3_ABI_PROFILE_T,		"profile");

ABI_STRING_CONST(P3_ABI_REQUEST_ID_T,		"FLOW");
ABI_STRING_CONST(P3_ABI_TIME_MIN_T,		"tMIN");
ABI_STRING_CONST(P3_ABI_TIME_MAX_T,		"tMAX");
ABI_STRING_CONST(P3_ABI_TIME_NEW_T0_T,		"tNT0");
ABI_STRING_CONST(P3_ABI_TIME_NEW_T1_T,		"tNT1");
ABI_STRING_CONST(P3_ABI_TIME_PREV_T,		"TPRV");
ABI_STRING_CONST(P3_ABI_TIME_PERIOD_T,		"period");
ABI_STRING_CONST(P3_ABI_SEQ64_T,		"seq64");	/* 64bit Sequence number */

ABI_STRING_CONST(P3_ABI_NODE_N,			"/");
ABI_STRING_CONST(P3_ABI_TASK_N,			"/task");	/* Task Node */
ABI_STRING_CONST(P3_ABI_RULE_N,			"/rule");	/* RULE Node */
ABI_STRING_CONST(P3_ABI_RELAY_N,		"/relay");	/* RELAY Node */
ABI_STRING_CONST(P3_ABI_FPN_N,			"/fpn");	/* FPN Node */
ABI_STRING_CONST(P3_ABI_FPN_CONFIG_N,		"/fpn/config");	/* FPN Config Node */

ABI_STRING_CONST(P3_ABI_CONFIG_N,		"/config");	/* Config Sub Node */
ABI_STRING_CONST(P3_ABI_FW_CONFIG_N,		"/fw-config");	/* FW Config Sub Node */

ABI_STRING_CONST(P3_ABI_FACE_DETECTOR_N,	"/fd");
ABI_STRING_CONST(P3_ABI_TF_IN_T,		"tf-in");
ABI_STRING_CONST(P3_ABI_TF_OUT_T,		"tf-out");
ABI_STRING_CONST(P3_ABI_DIM_MIN_T,		"dim-min");
ABI_STRING_CONST(P3_ABI_DIM_MAX_T,		"dim-max");
ABI_STRING_CONST(P3_ABI_FD_CELL_T,		"fd-cell");
ABI_STRING_CONST(P3_ABI_FD_FLAGS_T,		"fd-flags");
ABI_STRING_CONST(P3_ABI_FD_FANGLE_T,		"fd-face-angle");
ABI_STRING_CONST(P3_ABI_SCALE16_16_T,		"scale16:16");


ABI_STRING_CONST(P3_ABI_SENSOR_SMODE_T,		"sensor-sync-mode");
ABI_STRING_CONST(P3_ABI_SENSOR_SSRC_T,		"sensor-sync-src");

/* VIDEO related */
ABI_STRING_CONST(P3_ABI_VIDEO_N,		"/video");
ABI_STRING_CONST(P3_ABI_H264_N,			"/h.264");
ABI_STRING_CONST(P3_ABI_BRIGHTNESS_T,		"brightness");	/* uint16_t */
ABI_STRING_CONST(P3_ABI_CONTRAST_T,		"contrast");	/* uint16_t */
ABI_STRING_CONST(P3_ABI_JPEG_N,			"/jpeg");
ABI_STRING_CONST(P3_ABI_QUALITY_T,		"quality");	/* uint8_t */
ABI_STRING_CONST(P3_ABI_RAW_N,			"/raw");
ABI_STRING_CONST(P3_ABI_BIT_DEPTH_T,		"bit-depth");	/* uint8_t */

/* CamCtrl related */
ABI_STRING_CONST(P3_ABI_CAMCTRL_N,		"/camctrl");

ABI_STRING_CONST(P3_ABI_CAMCTRL_OPTS_N,		"/camctrl_opts");
ABI_STRING_CONST(P3_ABI_CAMCTRL_PROFILE_ID_T,	"cc_profile_id"); /* uint32_t */
ABI_STRING_CONST(P3_ABI_CAMCTRL_EXPO_MODE_T,	"cc_expo_mode");  /* uint16_t */
ABI_STRING_CONST(P3_ABI_CAMCTRL_OPTS_T,		"cc_opts");       /* uint32_t */
ABI_STRING_CONST(P3_ABI_CAMCTRL_EXPO_TARGET_OBJ_SIZE_T, "cc_expo_target_obj_size"); /* uint16_t */
ABI_STRING_CONST(P3_ABI_CAMCTRL_PROFILE_DATA_T, "cc_profile_data_t"); /* blob_t */

ABI_STRING_CONST(P3_ABI_IRIS_OPTS_N,		"/iris_opts");
ABI_STRING_CONST(P3_ABI_IRIS_MODE_T,		"iris_mode");     /* uint16_t */
ABI_STRING_CONST(P3_ABI_IRIS_DRIVE_T,		"iris_drive");    /* uint16_t */

/* AUDIO related */
ABI_STRING_CONST(P3_ABI_AUDIO_N,		"/audio");
ABI_STRING_CONST(P3_ABI_AUDIO_IN_T,		"a-in");
ABI_STRING_CONST(P3_ABI_AF_VOX_ENERGY_T,	"eVOX");

ABI_STRING_CONST(P3_ABI_AF_VOX_T,		"af-vox");
ABI_STRING_CONST(P3_ABI_AF_CID_T,		"af-cid");
ABI_STRING_CONST(P3_ABI_AF_CLI_T,		"af-cli");
ABI_STRING_CONST(P3_ABI_AF_DTMF_T,		"af-dtmf");
ABI_STRING_CONST(P3_ABI_AF_LINE_T,		"af-line");
ABI_STRING_CONST(P3_ABI_AF_LINE_LEVEL_T,	"af-line-dc");

/* VERELAY related ABI http://wiki.vocord.com/spec-arch-p3ss-cm-verelay */
ABI_STRING_CONST(P3_ABI_PROFILE_N,		"profile");
ABI_STRING_CONST(P3_ABI_WD_NAME_T,		"wd-name");
ABI_STRING_CONST(P3_ABI_WD_OUT_T,		"wd-out");
ABI_STRING_CONST(P3_ABI_WD_OUT_STATE_T,		"wd-out-state");
ABI_STRING_CONST(P3_ABI_CLIMATE_T,		"climate");
ABI_STRING_CONST(P3_ABI_CLIMATE_STATE_T,	"climate-state");

/* ENUMERATOR related ABI http://wiki.vocord.com/spec-arch-enum */
ABI_STRING_CONST(P3_ABI_ENUMERATOR_N,		"/enum");
ABI_STRING_CONST(P3_ABI_ENUM_MODE_T,		"enum-mode");
ABI_STRING_CONST(P3_ABI_ENUM_VAL_T,		"enum-val");

ABI_STRING_CONST(P3_ABI_DST_FLOWS_N,		"fTX");
ABI_STRING_CONST(P3_ABI_SRC_FLOWS_N,		"fRX");
ABI_STRING_CONST(P3_ABI_IDX_T,			"idx");
ABI_STRING_CONST(P3_ABI_SIZE_MAX_T,		"sz-max");

/* FIXME Набор значений для кода возврата функционального/пакетного API      */
typedef enum {
        P3_SUCCESS,           /* успешное выполнение команды                 */
        P3_HARDWARE_FAULT,    /* неустранимый HW сбой при выполнении команды */
        P3_SOFTWARE_FAULT,    /* сбой SW компонента при выполнении команды   */
        P3_MEDIA_DATA_FAULT,  /* логическое повреждение данных на носителе   */
        P3_MEDIA_FAULT,       /* неустранимый аппаратный сбой носителя       */
        P3_STOPPED,           /* подсистема остановлена                      */
        P3_INVALID_FLOW,      /* неправильный p3_id_t                        */
        P3_INVALID_TIME,      /* неправильное время                          */
        P3_INVALID_QOS,       /* 8 неправильный QOS                          */
        P3_INVALID_SCALE,     /* неправильный Scale                          */
        P3_INVALID_ARGUMENT,  /* неправильные аргументы                      */
        P3_INVALID_CONTEXT,   /* неправильный контекст                       */
        P3_ALREADY_DONE,      /* операция уже была осуществлена              */
        P3_NOT_IMPLEMENTED,   /* неподдерживаемая команда                    */
        P3_NO_RESOURCES,      /* недостаточно ресуров чтобы выполнить команду*/
        P3_NO_DATA,           /* запрос не вернул никаких данных             */
        P3_FAILED,            /* 16 невозможно выполнить команду             */
        P3_TIME_TO_LOW,       /* слишком маленькое значение времени          */
        P3_TIME_TO_BIG,       /* слишком большоеое значение времени          */
        P3_TO_MANY_CLIENTS,   /* достигнуто максимальное число клиентов      */
        P3_TRY_AJUSTED_ARGS,  /* необходимо повторить (arguments are fixed)  */
        P3_DB_FAILED,         /* невозможно выполнить команду поиска в DB    */
	P3_UNKNOWN_COMMAND,   /* неподдерживаемая/неизвестная команда        */
        P3_MEDIA_FULL,        /* на носителе нет места                       */
        P3_UNSUPPORTED_KEY,   /* Unsupported Key Value                       */
} p3_status_e;

/* FIXME Набор значений для обозначения типа ABI компонента системы Phobos   */
typedef enum {
        P3_MDM_DATA,
        P3_MDM_CMD,
        P3_MDM_BOTH,		/* MDM DATA and CMD interface		     */

        P3_DEVICE_DATA,
        P3_DEVICE_CMD,
        P3_DEVICE_BOTH,		/* DEV DATA and CMD interface		     */
        
        P3_CM,			/* CM  DATA and CMD interface		     */

	P3_DEVICE_HTTP,		/* DEV HTTP DATA and CMD interface 	     */
	P3_DEVICE_RAW_IP	/* DEV RAW IP DATA interface	 	     */
} p3_component_e;

typedef enum {
	P3_SEC_NONE,
	P3_SEC_SIGNED,
	P3_SEC_ENCRIPTED,
} p3_security_mode_e;

/*\
 *  Набор базовых кодов типов качества обслуживания
 * (способа отбрасывания фреймов данных)
\*/
typedef enum {
        P3_QOS_RT_ACTUAL_DATA,        /* Наиболее актуальные данные         */
        P3_QOS_RT_JITTER_BUFFERED,    /* Устранять неравномерности data ts  */
        P3_QOS_RT_OLDEST_DATA,        /* Наименее актуальные данные         */

        P3_QOS_RT_VARIABLE_BAND,      /* Режим переменного bitrate данных   */
        
        P3_QOS_RT_USE_SKIP_RATE,      /* timestamp меняется не монотонно    */

        /* Неприменимо к real time подсистемам (игнорируется ими)           */
        P3_QOS_NRT_RELIABLE_DELIVERY, /* Режим надежной доставки            */
        P3_QOS_NRT_USE_SKIP_RATE,     /* Режим надежной доставки M из N     */
        
        P3_QOS_ENOT_IMPLEMENTED = -2, /* Режим QOS не реализован            */
        P3_QOS_EFAILED =-1,           /* Не возможно установить режим QOS   */
} p3_qos_e;

typedef enum {
        P3_ABI_SYNC_MARK,           /* Маркирующий ключ                     */
        P3_ABI_SYNC_CSUM,           /* контрольная сумма: xor всех октетов не
                                       включая ключи с сигнатурой совпадающей
                                       с сигнатурой данного ключа.  Начальное
                                       значение .val, промежуточное значение
                                       должно совпадать с полем .val check
                                       ключа */
        P3_ABI_SYNC_TYPE       = 0x000000ff,    /* Маска типа sync ключа     */
        P3_ABI_SYNC_KEY_T_MASK = 0xC0000000,    /* Маска подтипа sync ключа  */
        P3_ABI_SYNC_KEY_CHECKO = 0x00000000,    /* Ключ с не обязательным
                                       START отсутствие START генерирует
                                       warning */
        P3_ABI_SYNC_KEY_CHECKM = 0x40000000,    /* Ключ с обязательным START
                                       отсутствие START генерирует abort()   */
        P3_ABI_SYNC_KEY_STARTO = 0x80000000,    /* Ключ с опциональным
                                       подтверждением отсутствие генерирует
                                       warning     */
        P3_ABI_SYNC_KEY_STARTM = 0xC0000000,    /* Ключ с обязательным
                                       подтверждением отсутствие подтверждения
                                       на томже уровне генерирует abort()    */
} p3_abi_sync_e;

/* Набор констант для p3_lid_t.id см определение p3_lid_t ниже */
typedef enum {
        P3_ID_DYNAMIC       =0x80000000,     /* маска динамического ресурса  */
        P3_ID_RESOLVE_FAILED=0xfffffffe,     /* lid ресурса не существует    */
        P3_ID_NEED_RESOLVE  =0xffffffff,     /* lid ресурса не указан        */
} p3_lid_e;

typedef enum {              /* промежуток агрегации/per sample               */
        MA_STAT_FILL_RANGE, /* записать в [t0,t1) range доступной статистики */
        MA_STAT_SECOND,     /* запрос статистики агрегированной за 1 секунду */
        MA_STAT_MINUTE,     /* запрос статистики агрегированной за 1 минуту  */
        MA_STAT_HOUR,       /* запрос статистики агрегированной за 1 час     */
        MA_STAT_DAY,        /* запрос статистики агрегированной за 1 день    */
        MA_STAT_BEST_FIT,   /* агрегирование BEST FIT/per sample             */
} p3_stat_scale_e;

/* Определения структур */
#if defined(_MSC_VER) && !defined(__GNUC__)
#include <compilers/visualc/align1.h>
#endif
/* идентификатор ресурса {потока, записи, устройства...} уникальный локально */
/* на host, который будет передавать даннные по запросу, должен высылаться   */
/* назад если поле .p3_id_t.lid.id в запросе равно 0xffff'ffff, чтобы в      */
/* последующих запросах не производить поиск адресуемого объекта в global    */
/* namespace                                                                 */
typedef struct  p3_lid_s {
        uint32_t   id;       /*  4 octets локально уникальный id ресурса     */
        uint16_t   id_src;   /*  2 octets id транспортной сессии получателя  */
        uint16_t   id_dst;   /*  2 octets id транспортной сессии отправителя */
} __attribute__((packed)) p3_lid_t;

/*\
 * глобальный в рамках системы идентификатор ресурса {потока, записи,
 * устройства ...} !высылается всегда!
\*/
typedef struct p3_fid_s {
        uint32_t   dev_type;  /*  4 octets host type                         */
        uint8_t    cpu_id[8]; /*  8 octets host CPU ID                       */
        uint8_t    flow[12];  /* 12 octets flow/device ID                    */
} __attribute__((packed)) p3_fid_t;

typedef struct p3_guid_s {
        uint32_t   type;      /*  4 octets device ID for guid proxy          */
        uint32_t   udef;      /*  4 octets user defined                      */
        uint8_t    guid[16];   /* 16 octets GUID                             */
} __attribute__((packed)) p3_guid_t;

typedef struct p3_did_s {
        uint8_t    dev_id[12]; /* 12 octets device ID                        */
        uint8_t    flow[12];   /* 12 octets flow/device ID                   */
} __attribute__((packed)) p3_did_t;

typedef union p3_gid_s {
        uint8_t    val[24];   /* 24 octets dns RR GID unic в рамках системы  */
        uint32_t   val32[6];  /* 24 octets dns RR GID unic в рамках системы  */
	p3_fid_t   fid;       /* 24 octets flow ID unical в рамках системы   */
	p3_did_t   did;       /* 24 octets device ID unical в рамках системы */
	p3_guid_t  guid;      /* 24 octets proxy GUID unical в рамках системы*/
} __attribute__((packed)) p3_gid_t;

/* global в рамках системы идентификатор ресурса {потока, записи,устройства} */
typedef struct p3_id_s {
        p3_lid_t   lid;       /*  8 octets ID ресурса уникальный локально    */
        p3_gid_t   gid;       /* 24 octets dns RR GID unic в рамках системы  */
} __attribute__((packed)) p3_id_t;

/*\
 *  val_t для ключа P3_ABI_SYNC_T, используется в целях валидации и отладки
 *  пакетов ABI. размер ограничен P3_ABI_SYNC_MAX
\*/
typedef struct p3_sync_s {
        uint32_t   type;        /* тип синхронизатора см p3_abi_sync_e       */
        uint32_t   val;         /* значение синхронизатора                   */
        uint8_t    sign[0];     /* synch SIGN len up to P3_ABI_SYNC_MAX-8    */
} __attribute__((packed)) p3_abi_sync_t;

#define P3_ABI_SYNC_MAX         (80)

#define P3_FT_ONE_USECOND       (          10ULL) /* 1000 ns in 100ns chunks */
#define P3_FT_ONE_MSECOND       (       10000ULL) /* 1000 us in 100ns chunks */
#define P3_FT_ONE_SECOND        (    10000000ULL) /* 1 sec   in 100ns chunks */
#define P3_FT_ONE_MINUTE        (   600000000ULL) /* 1 minue in 100ns chunks */
#define P3_FT_ONE_HOUR          ( 36000000000ULL) /* 1 hour  in 100ns chunks */
#define P3_FT_ONE_DAY           (864000000000ULL) /* 1 day   in 100ns chunks */

#define P3_FT_MAX               (0xffffffffffffffffull) /* max filetime value */
#define P3_FT_MIN               (0x0000000000000000ull) /* min filetime value */

typedef uint64_t        filetime;
typedef struct p3_sevent_s {
	uint32_t	type;		// See declaration of p3_events_e

	uint8_t		data[];		// Event data
} __attribute__((packed)) p3_sevent_t;

typedef struct p3_sevent_mask_s {
	uint32_t	type;		// See declaration of p3_events_e

	uint32_t	idx;		// Event mask offset
	uint32_t	mask;		// Event mask
} __attribute__((packed)) p3_sevent_mask_t;

typedef struct p3_sevent_mask16_s {
	uint32_t	type;		// See declaration of p3_events_e

	uint16_t	idx;		// Event mask offset
	uint16_t	mask;		// Event mask
} __attribute__((packed)) p3_sevent_mask16_t;

typedef struct p3_tevent_s {
	filetime	ts;      	// Timestamp (filetime)
	uint32_t	type;    	// See declaration of p3_events_e

	uint8_t		data[];  	// Event data
} __attribute__((packed)) p3_tevent_t;

typedef struct p3_tevent_sesion_s {
	filetime	ts;		// Timestamp (filetime)
	uint32_t	type;		// See declaration of p3_events_e

	uint32_t	seq;		// Session sequental ID
} __attribute__((packed)) p3_tevent_session_t;

typedef struct p3_tevent_ex_fee_s {
	filetime	ts;		// Timestamp (filetime)
	uint32_t	type;		// See declaration of p3_events_e

	uint16_t	fee;		// Exam FEE
	uint16_t	total;		// Exam FEE total
} __attribute__((packed)) p3_tevent_ex_fee_t;

typedef struct p3_probe_state_s {
	uint16_t	idx;		// Detector idx
	uint16_t	state;		// Detector state
	uint32_t	serial;		// Serial

	filetime	t0;		// Timestamp (filetime)
	filetime	t1;		// Timestamp (filetime)

	uint32_t	dt0;		// Prerecord  ft>>20
	uint32_t	dt1;		// Postrecord ft>>20
} __attribute__((packed)) p3_probe_state_t;

typedef struct p3_tevent_md_s {
	filetime	ts;		// Timestamp (filetime)
	uint32_t	type;		// See declaration of p3_events_e
	uint32_t	reserved;	//

	p3_probe_state_t	states[];
} __attribute__((packed)) p3_tevent_md_t;

typedef struct p3_enum_state_s {
	uint16_t	idx;		// idx
	uint16_t	state;		// State
	uint32_t	serial;		// Serial

	filetime	t0;		// Timestamp (filetime)
	filetime	t1;		// Timestamp (filetime)
} __attribute__((packed)) p3_enum_state_t;

typedef struct p3_tevent_enum_s {
	filetime	ts;		// Timestamp (filetime)
	uint32_t	type;		// See declaration of p3_events_e
	uint32_t	reserved;	//

	p3_enum_state_t	states[];
} __attribute__((packed)) p3_tevent_enum_t;

typedef struct p3_tevent_str_idx_s {
	filetime	ts;		// Timestamp (filetime)
	uint32_t	type;		// See declaration of p3_events_e
	uint16_t	idx;		//
	uint16_t	len;		//

	uint8_t		str[];
} __attribute__((packed)) p3_tevent_str_idx_t;

typedef struct p3_tevent_str16_idx_s {
	filetime	ts;		// Timestamp (filetime)
	uint32_t	type;		// See declaration of p3_events_e
	uint16_t	idx;		//
	uint16_t	len;		//

	uint8_t		str[16];
} __attribute__((packed)) p3_tevent_str16_idx_t;

typedef struct p3_tevent_str_s {
	filetime	ts;		// Timestamp (filetime)
	uint32_t	type;		// See declaration of p3_events_e
	uint32_t	len;		//

	uint8_t		str[];
} __attribute__((packed)) p3_tevent_str_t;

typedef p3_tevent_str_t p3_tevent_imea_t;

typedef struct p3_tevent_sn_s {
	filetime	ts;		// Timestamp (filetime)
	uint32_t	type;		// See declaration of p3_events_e
	uint32_t	reserved;	//

	p3_probe_state_t	states[];
} __attribute__((packed)) p3_tevent_sn_t;

typedef struct p3_tevent_ph_st_s {
	filetime	ts;		// Timestamp (filetime)
	uint32_t	type;		// P3_TEV_PF_STATE
	uint16_t	idx;		// line local idx
	uint16_t	state;		// See declaration of p3_ph_st_e
} __attribute__((packed)) p3_tevent_ph_st_t;

typedef struct {
	filetime	ts;		/* Timestamp (filetime)	*/
	uint32_t	type;		/* P3_TEV_ACC_3D	*/

	uint16_t	subtype;	/*			*/
	uint16_t	valid;		/* valid samples	*/
	int16_t		data[];		/* data samples		*/
} __attribute__((packed)) p3_tevent_acc_t;


typedef enum {
	P3_TEV_SESSION_PREV	= 0x0000, /* last    session start */
	P3_TEV_SESSION_THIS	= 0x0001, /* current session start */
	P3_TEV_SESSION_NEXT	= 0x0002, /* next    session start */

	P3_TEV_EX_FEE_PREV	= 0x0003, /* last    exam fee */
	P3_TEV_EX_FEE_THIS	= 0x0004, /* current exam fee */
	P3_TEV_EX_FEE_NEXT	= 0x0005, /*?next    exam fee */

	P3_TEV_LO_SIMPLE_THIS	= 0x0006, /* current LO event       */
	P3_TEV_MD_SIMPLE_THIS	= 0x0007, /* current MD event       */
	P3_TEV_VS_SIMPLE	= 0x0008, /* current Video Status   */

	P3_TEV_IMEA_GP		= 0x0009, /* current GPS msg p3_tevent_str_t	*/

	P3_TEV_VQ_SIMPLE	= 0x000a, /* current Video Quality 		*/
	P3_TEV_SN_SIMPLE_THIS	= 0x000b, /* current SeNsor event start		*/

	P3_TEV_AF_CID		= 0x000c, /* CALLER ID msg  p3_tevent_str_idx_t	*/
	P3_TEV_AF_MFT		= 0x000d, /* MFC tones p3_tevent_str_idx_t	*/
	P3_TEV_AF_PULSE		= 0x000e, /* PULSE dial msg p3_tevent_str_idx_t	*/
	P3_TEV_PF_STATE		= 0x000f, /* Phone If state p3_tevent_ph_st_t	*/
	P3_TEV_AF_CLI		= 0x0010, /* CL line id msg  p3_tevent_str_idx_t*/
	P3_TEV_AF_DTMF		= 0x0011, /* DTMF tones msg  p3_tevent_str_idx_t*/

	P3_TEV_ACC_3D		= 0x0012, /* 3D Accelerator  p3_tevent_acc_t	*/

	P3_SEV_AF_VOX_STATUS	= 0x1000, /* AF VOX  status p3_sevent_mask16_s	*/
	P3_SEV_PF_LINE_STATUS 	= 0x1001, /* PF LINE status p3_sevent_mask16_s	*/
	P3_SEV_PF_RING_STATUS 	= 0x1002, /* PF RING status p3_sevent_mask16_s	*/
	P3_SEV_PF_HOOK_STATUS 	= 0x1003, /* PF HOOK status p3_sevent_mask16_s	*/
}  p3_events_e;

/*\
 *      Fixed point types
\*/

typedef uint32_t fp16x16_t;		/* fixed point 16.16 unsigned */
typedef int32_t	 fpi16x16_t;		/* fixed point 16.16 integer  */
typedef struct {			/* rational 16.0/16.0 */
	uint16_t	nom;		/* nominator */
	uint16_t	denom;		/* denominator */
} fq16x16_t;
/*\
 * Обязательный параметр для multicast нотификаций. В нотификациях которые
 * не помещаются в 1 UDP пакет служит для обеспечения атомарности
 * передаваемых данных 
\*/
typedef struct {
        filetime        ts;             /* timestamp of sampled object value */
        uint16_t        id;             /* P3 ABI packet fragment            */
        uint16_t        max;            /* max P3 ABI packet fragments       */
} __attribute__((packed)) p3_msg_part_t;

/*\
 * Информация о транспортном адресе и протоколе клиента или сервера
\*/
typedef struct {
        uint32_t        ip;             /* IPv4 address */
        uint16_t        port;           /* IPv4 port */
        uint8_t         tproto;         /* тип транспортного протокола */
        uint8_t         aproto;         /* тип протокола приложения    */
} __attribute__((packed)) p3_conn_ipv4_t;

/*\
 * Информация о загруженности ресурса см p3_resiurces_e
\*/
typedef struct {
        uint32_t        rtype;          /* Тип ресурса см p3_resource_e      */
        uint32_t        max;            /* MAX допустимое количество ресурса */
        uint32_t        cur;            /* Currently used resource amount    */
        uint32_t        stat[3];        /* статистика использования ресурса  */
} __attribute__((packed)) p3_load_param_t;

/*\
 * Информация о ревизии прошивки
\*/
typedef struct {
	uint8_t		device;
	uint16_t	major;
	uint8_t		minor;
	filetime	time;
} __attribute__((packed)) p3_fmw_rev_t;

/*\
 * Идентификаторы ресурсов и состояний для p3_load_param_t.rtype
\*/
typedef enum {
        P3_RC_CLIENTS,			/* Plugged clients statistic  ??? */
        P3_RC_TRANSPORT_R,
        P3_RC_TRANSPORT_W,
        P3_RC_CPU,
        P3_RC_HDD_R,
        P3_RC_HDD_W,

        P3_RC_VIDEO_CH_MASK,		/* Available dev video flows mask */
        P3_RC_AUDIO_CH_MASK,		/* Available dev audio flows mask */
        P3_RC_DATA_CH_MASK,
        P3_RC_VIDEO_CH_COUNT,		/* Maximum available video inputs */
        P3_RC_VIDEO_CH_PLUGGED_MASK,	/* Inputs plugged to videosource  */
        P3_RC_VIDEO_CH_WITH_DATA_MASK,	/* Inputs with nonzero video data */
        P3_RC_PROBES_MASK,		/* Probes state mask              */
        P3_RC_MOTION_DETECTORS_MASK,	/* Motion detectors state mask    */
        P3_RC_FT_STAGE,			/* FT stage MSK(use) Uptime(free) */
        P3_RC_PROBES_COUNT,		/* Probes count mask              */
        P3_RC_MOTION_DETECTORS_COUNT,	/* Motion detectors cnt & offset  */
        P3_RC_MDM_STAT_COUNT,		/* MDM statfs avl use & dev num   */

        P3_RC_LO_DETECTORS_MASK,	/* Lost Objects |   state mask    */
        P3_RC_LO_DETECTORS_COUNT,	/*  detectors   |   cnt & offset  */

        P3_RC_RAW_VIDEO_CH_MASK,	/* Available raw video flows mask */
        P3_RC_RAW_OBJECTS_CH_MASK,	/* Available raw faces flows mask */
        P3_RC_DEV_OBJECTS_CH_MASK,	/* Available dev faces flows mask */

        P3_RC_AUDIO_DECIMATED_CH_MASK,	/* Decimated audio flows mask     */
        P3_RC_LINE_DC_CH_MASK,		/* Line DC level flows mask       */
        P3_RC_LINE_RAW_CH_MASK,		/* Line RAW flows mask            */
} p3_resource_e;
/*\
 * Server clients Information
\*/
typedef struct {
        filetime        ts;             /* timestamp подключения к серверу   */
        uint32_t        id;             /* порядковый номер соединеня        */
        uint32_t        load;           /* ?? общая загрузка сервера         */
} __attribute__((packed)) p3_client_id_t;

typedef struct p3_frame_descr_s {   /* (4 octets) описатель данных фрейма    */
        uint8_t        activity;    /*( 1 octet)  уровень активности         */
        uint8_t        compr[3];    /*( 3 octets) тип компрессии             */
} __attribute__((packed)) p3_frame_descr_t;

typedef enum {
	P3_VRES_FULL	=0,
	P3_VRES_HFULL	=1,
} p3_vresolution_e;
/*\
 * Идентификатор или запрос на данные/метаданные/статистику фрейма
 * или групы фреймов для функционального API/пакетного ABI
\*/
typedef struct p3_frames_s {
        filetime       t0;        /* t0-start, t1-finish точка запрашиваемо- */
        filetime       t1;        /* го или заполняемого интервала времени   */
        union {                   /* (8 octets) "контейнер" параметра 0      */
                uint64_t        param0; /* параметр 0                        */
                uint32_t        offs[2]; /* смещение данных и времени frame  */
                uint32_t        p0a2[2];/* параметр 0 в виде 2x4 октета      */
                uint8_t         p0a8[8];/* параметр 0 в виде 8ми октетов     */
                filetime        te;     /* finish точка выполненного запроса */
                filetime        ts;     /* start точка выполненного запроса  */
        } ;
        union {                      /* (4 octets) "контейнер" параметра 1   */
                uint32_t        param1;  /* параметр 1                       */
                uint32_t        priority;/* Запрашиваемый приоретет запроса  */
                p3_frame_descr_t frame;  /* описатель данных фрейма          */
                uint32_t        framev;  /* описатель данных фрейма as val   */
                p3_stat_scale_e scale;   /* requested granularity scale      */
        } ;
        union {                       /* (4 octets) "контейнер" параметра 2  */
                uint32_t        param2;   /* параметр 2                      */
                uint32_t        size;     /* размер буфера для запроса       */
                uint32_t        max_size; /* MAX размер буфера для запроса   */
        } ;
} __attribute__((packed)) p3_frames_t;

/*\
 *      NS usage information exchange containers
\*/
typedef struct p3_resource_s {
        uint32_t                type;     /* resource type see p3_resource_e */
        uint32_t                avl;      /* available resources mask        */
        uint32_t                used;     /* used resources mask             */
} __attribute__((packed)) p3_resource_t;

typedef struct p3_ns_info_s {
        uint32_t                offset;   /* offset from base interface GID  */
        p3_resource_t           resource; /* resource descriptor             */
} __attribute__((packed)) p3_ns_info_t;

/*\
 *      Enumerator information exchange containers and constants
\*/
typedef enum {
        P3_ABI_ENUM_SIMPLE  = 0x00000000, /* offset & count                  */
        P3_ABI_ENUM_MASK    = 0x00000001, /* offset & mask                   */

        P3_ABI_ENUM_NS_MASK = 0xf0000000, /* Name Space Type mask            */
        P3_ABI_ENUM_GLOBAL  = 0x40000000, /*                vs GID           */
        P3_ABI_ENUM_LOCAL   = 0x80000000, /*                vs LOCAL NS      */
        P3_ABI_ENUM_PRIVATE = 0x00000000, /*                vs PRIVATE NS    */
} p3_abi_enum_e;

typedef struct {
        uint32_t                offset;   /* offset from base interface GID  */
        uint32_t                number;   /* resource count or mask          */
} __attribute__((packed)) p3_abi_enum_t;

/*\
 *      volatile <=> nonvolatile pointers cast helper
 *      !! DANGERUS to USE. You NEED completely understand all side effects !!
\*/
typedef union {
        volatile void           *vv;    /* pointer to volatile void         */
        void                    *pv;    /* pointer to non volatile void     */
        volatile uint8_t        *vu8;   /* pointer to volatile uint8_t      */
        char                    *pc;    /* pointer to non volatile  char    */
        uint8_t                 *pi8;   /* pointer to non volatile  int8_t  */
        uint8_t                 *pu8;   /* pointer to non volatile uint8_t  */
        volatile uint16_t       *vu16;  /* pointer to volatile uint16_t     */
        uint16_t                *pu16;  /* pointer to non volatile uint16_t */
        volatile uint32_t       *vu32;  /* pointer to volatile uint32_t     */
        uint32_t                *pu32;  /* pointer to non volatile uint32_t */
} p3_volatile_cast_t;

#define VOLATILE_CAST(TYPE, VAR, VARV)	\
{					\
	union {				\
		TYPE		nv;	\
		volatile TYPE	v;	\
	} cnv = { .v = VARV };		\
					\
	VAR = cnv.nv;			\
}

#if defined(_MSC_VER) && !defined(__GNUC__)
#include <compilers/visualc/alignx.h>
#endif

#endif  /* __ABI_P3_H__*/
