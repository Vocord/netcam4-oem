#ifndef __ABI_DEVICE_H__
#define __ABI_DEVICE_H__
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

#ifdef __cplusplus
extern "C" {
#endif

#if defined(DEVICE_ISOLATED)
#include <isolated/abi/p3.h>
#else
#include <abi/p3.h>
#endif

/*  */
/* Объявления констант */

/* Набор базовых кодов операций ABI */
typedef enum {
    DEVICE_ABI_STATUS_RESP       /* 0x50 Status for ACK CMD              */
     = P3_ABI_DEVICE,            /*  */
    DEVICE_ABI_START_FLOW_CMD,   /* 0x51 Request for data from data flow */
    DEVICE_ABI_STOP_FLOW_CMD,    /* 0x52 Disable data from data flow     */
    DEVICE_ABI_FRAME_RESP,       /* 0x53 Data frames from data flow      */
    DEVICE_ABI_GET_STATE_CMD,    /* 0x54 Device flows state Request      */
    DEVICE_ABI_STATE_RESP,       /* 0x55 Device flows state Response     */
    DEVICE_ABI_LAST_,            /* */
} device_abi_ops_e;

/* Набор базовых кодов типов данных ABI компонента */
typedef enum {
    DEVICE_ABI_STATE_T        /* 0xe8 Состояние Phobos 3 Device                    */
     = P3_ABI_T_DEVICE,       /* Первый ткод для Phobos 3 Device                   */
    DEVICE_ABI_REQUEST_T,     /* 0xe9 Запрос потока данных device_request_t        */
    DEVICE_ABI_REQUEST_QOS_E, /* 0xea Запрос качества обслуживания p3_qos_e        */
    DEVICE_ABI_HW_STATE_T,    /* 0xeb Состояние аппаратуры Phobos 3 Device         */
    DEVICE_CLASS_E,           /* 0xec тип hw устройства см device_hw_type_e        */
    DEVICE_CAPS_E,            /* 0xed логический тип устройства см device_type_e   */

    DEVICE_ABI_LAST,          /* Для связи и проверки...                      */
    DEVICE_ABI_FRAME_DATA_T   /* импортировано из <abi/p3.h>                  */
     = P3_ABI_FRAME_DATA_T,
} device_abi_types_e;

/*\
 * Набор значений для состояний и запроса состояния
 * функционального/пакетного API подсистемы
\*/
typedef enum {
    DEVICE_STATE_GET,        /* получить состояние подсистемы                 */
    DEVICE_STATE_RUN,        /* запустить подсистему/подсистема функционирует */
    DEVICE_STATE_STOP,       /* остановить подсистему/подсистема остановлена  */
    DEVICE_STATE_CHECK,      /* выполнить диагностику/выполняется диагностика */
    DEVICE_STATE_FAIL,       /* неустранимый сбой при работе подсистемы       */

    DEVICE_STATE_RUNNING  = DEVICE_STATE_RUN,
    DEVICE_STATE_STOPED   = DEVICE_STATE_STOP,
    DEVICE_STATE_CHECKING = DEVICE_STATE_CHECK,
    DEVICE_STATE_FAILED   = DEVICE_STATE_FAIL,
} device_state_e;

/*\
 * Набор значений для типов возможностей HW устройств
 * функционального/пакетного API подсистемы
\*/
typedef enum {
    DEVICE_HW_VIDEO_IN,     /* устройство видео ввода с независимыми настройками  */
    DEVICE_HW_VIDEO_GRP_IN, /* устройство видео ввода с групповыми настройками    */
    DEVICE_HW_VIDEO_MIX_IN, /* устройство видео ввода со смешанными настройками   */

    DEVICE_HW_AUDIO_IN,     /* устройство аудио ввода с независимыми настройками  */
    DEVICE_HW_AUDIO_GRP_IN, /* устройство аудио ввода с групповыми настройками    */
    DEVICE_HW_AUDIO_MIX_IN, /* устройство аудио ввода со смешанными настройками   */
} device_type_e;

typedef enum {
        DEVICE_TYPE_PPC405GPr_VIDEO_BRIDGE,
        DEVICE_TYPE_VB			   = DEVICE_TYPE_PPC405GPr_VIDEO_BRIDGE,
        DEVICE_TYPE_PPC405GP_D16,
        DEVICE_TYPE_PPC405GPr_D8,
	DEVICE_TYPE_GUID_PROXY,
	DEVICE_TYPE_VB_ARCHIVE,
	DEVICE_TYPE_VERELAY,
	DEVICE_TYPE_NETCAM,
	DEVICE_TYPE_AB,
} device_class_e;

/*\
 * Набор значений для флагов в HW состоянии устройства
 * функционального/пакетного API подсистемы
\*/
typedef enum {
    DEVICE_FLAG_OK              =(1<<0),     /* устройство работает штатно     */
    DEVICE_FLAG_HAS_SUBS        =(1<<1),     /* устройство имеет подустройства */
    DEVICE_FLAG_IS_FLOW         =(1<<2),     /* устройство является потоком    */
    DEVICE_FLAG_CONNECTED       =(1<<3),     /* получает данные                */
    DEVICE_FLAG_LEVEL           =(1<<4),     /* уровень устройства/потока      */
} device_flags_e;

/*\
 * Набор значений для типов событий/элементов состояний
 * устройств/потоков
 * функционального/пакетного API подсистемы
\*/
typedef enum {
    DEVICE_ST_VIDEO_IN_STATUS0,     /* устройство видео ввода статус #0 */
} device_states_e;

/*\
 * Набор значений для DEVICE_ST_VIDEO_IN_STATUS0
 * событий/элементов состояний устройств/потоков 
 * функционального/пакетного API подсистемы
\*/
typedef enum {
//    DEVICE_VIN_S0_ = (1<<0x0),     /*  */
    DEVICE_VIN_S0_PEAK_WD       = (1<<0x7),     /* Peak white detect status ?    */
    DEVICE_VIN_S0_PEAK_W1       = (1<<0x7),     /* Peak white detected           */
    DEVICE_VIN_S0_PEAK_W0       = (0<<0x7),     /* Peak white not detected       */
                                                                                
    DEVICE_VIN_S0_LINE_ALTS     = (1<<0x6),     /* Line alternating status ?     */
    DEVICE_VIN_S0_LINE_ALTS1    = (1<<0x6),     /* Line alternating              */
    DEVICE_VIN_S0_LINE_ALTS0    = (0<<0x6),     /* Line not alternating          */

    DEVICE_VIN_S0_FLD_RATE      = (1<<0x5),     /* Field rate ?                  */
    DEVICE_VIN_S0_FLD_50HZ      = (1<<0x5),     /* Field rate 50Hz               */
    DEVICE_VIN_S0_FLD_60HZ      = (0<<0x5),     /* Field rate 60Hz               */

    /* since status register #1 was last read */
    DEVICE_VIN_S0_LOST_LOCK     = (1<<0x4),     /* Lost lock detect ?            */
    DEVICE_VIN_S0_LOST_LOCK1    = (1<<0x4),     /* Lost lock detected            */
    DEVICE_VIN_S0_LOST_LOCK0    = (0<<0x4),     /* Lost lock not detected        */

    DEVICE_VIN_S0_CHROMA_LOCK   = (1<<0x3),     /* Color subcarrier lock status  */
    DEVICE_VIN_S0_CHROMA_LOCK1  = (1<<0x3),     /* Color subcarrier locked       */
    DEVICE_VIN_S0_CHROMA_LOCK0  = (0<<0x3),     /* Color subcarrier not locked   */

    DEVICE_VIN_S0_VSYNC         = (1<<0x2),     /* Vertical sync lock status     */
    DEVICE_VIN_S0_VSYNC1        = (1<<0x2),     /* Vertical sync is locked       */
    DEVICE_VIN_S0_VSYNC0        = (0<<0x2),     /* Vertical sync is not locked   */

    DEVICE_VIN_S0_HSYNC         = (1<<0x1),     /* Horizontal sync lock status   */
    DEVICE_VIN_S0_HSYNC1        = (1<<0x1),     /* Horizontal sync is locked     */
    DEVICE_VIN_S0_HSYNC0        = (0<<0x1),     /* Horizontal sync is not locked */
    
    DEVICE_VIN_S0_TV_VCR        = (1<<0x0),     /* TV/VCR status                 */
    DEVICE_VIN_S0_VCR           = (1<<0x0),     /* nonstandard number of lines   */
    DEVICE_VIN_S0_TV            = (0<<0x0),     /* standard number of lines      */
} device_vin_s0_e;

/* Определения структур данных */
/*\
 * Идентификатор или запрос на данные/метаданные/статистику фрейма
 * или групы фреймов для функционального API/пакетного ABI
\*/
#if defined(_MSC_VER) && !defined(__GNUC__)
#include <pshpack1.h>
#endif
typedef p3_frames_t device_request_t;

typedef struct {
    uint32_t            param;          /* Тип параметра состояния     */
    uint16_t            event;          /* Маска изменений в состоянии */
    uint16_t            state;          /* Маска новоего состояния     */
} __attribute__((packed)) device_event_t;

typedef struct {
    p3_id_t             flow;           /* GID потока/устройства       */

    uint16_t            count;          /* длинна состояния            */
    uint16_t            type;           /* тип устройства              */

    uint8_t             flags;          /* Флаги см enum device_flags_e */
    uint8_t             level;          /* Уровень состояния           */
    uint8_t             group0;         /* Группа                      */
    uint8_t             group1;         /* ?? Подгруппа/супергруппа ?? */

    device_event_t      events[];       /* события                     */
} __attribute__((packed)) device_hw_state_t;

typedef enum {
    P3_HIST_MAX_PRESENT		= (1<< 0),
    P3_HIST_MIN_PRESENT		= (1<< 1),
} p3_histogram_flags_e;

typedef struct {
    uint32_t	pixels;		/* Total pixels in hist */
    uint16_t	x0;		/* X0 for HROI		*/
    uint16_t	x1;		/* X1 for HROI		*/
    uint16_t	y0;		/* Y0 for HROI		*/
    uint16_t	y1;		/* Y1 for HROI		*/
    uint8_t	type;		/* histogramm type	*/
    uint8_t	flags;		/* histogramm flags	*/
    uint8_t	components;	/* 1-4			*/
    uint8_t	reserved;	/*			*/
} p3_histogram_t;

#define DEVICE_HW_STATE_SIZE(N) (sizeof(device_hw_state_t) + sizeof(device_event_t)*N)

#if defined(_MSC_VER) && !defined(__GNUC__)
#include <poppack.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
