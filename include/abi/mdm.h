#ifndef __ABI_MDM_H__
#define __ABI_MDM_H__
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

#if defined(MDM_ISOLATED)
#include <isolated/abi/p3.h>
#else
#include <abi/p3.h>
#endif

/*  */
/* Объявления констант */
#define MDM_FSTAT_MAX           (2048ul) /* Макс длина frame статистики       */
                                                                           
#define MDM_FSTAT_SECONDS       (2048ul) /* frame по секундной статистики     */
#define MDM_FSTAT_MINUTES       (2048ul) /* frame по минутной статистики      */
#define MDM_FSTAT_HOURS         (2048ul) /* frame по часовой статистики       */
#define MDM_FSTAT_DAYS          (2048ul) /* frame суточной статистики         */

#define MDM_MAX_L1_LINES	1022		/* max L1 cache size */

/* Набор базовых кодов операций ABI !!!Order should be immutable */
typedef enum {
    MDM_ABI_STATUS_RESP             /*1Статус команды, не требующей данных  */
     = P3_ABI_MDM,                  /*  */

    MDM_ABI_READ_FRAME_CMD,         /*1Читать фрейм данных                  */
    MDM_ABI_READ_FRAME_RESP,        /*1Ответ на команду читать фрейм данных */
    MDM_ABI_WRITE_FRAME_CMD,        /*2Записать фрейм данных                */
    
    MDM_ABI_SCHED_REQ_CTRL_CMD,     /*2управление заданиями                 */
    
    MDM_ABI_GET_STAT_CMD,           /*2Получить статистику                  */
    MDM_ABI_GET_STAT_RESP,          /*2Ответ на команду Получить статистику */
    MDM_ABI_SET_STAT_CMD,           /*2Изменить статистику                  */

    MDM_ABI_LAST_,                  /* Для связи и проверки...              */
} mdm_abi_ops_e;

/* Набор базовых кодов типов данных ABI компонента */
typedef enum {
    MDM_ABI_STAT_DATA_T=     /* данные статистики активности                */
    P3_ABI_T_MDM,            /* первый ткод для Media Data Manager          */
    MDM_ABI_REQUEST_T,       /* запрос данных mdm_request_t                 */
    MDM_ABI_META_DATA,       /* метаданные фреймов данных                   */
    MDM_ABI_FRAME_DATA,      /* 1 фрейм данных                              */
    MDM_ABI_FRAME_ADDR,      /* Адрес фрейма данных                         */
    MDM_ABI_FRAME_OFFS,      /* Смещение фрейма данных                      */
    MDM_ABI_READ_TYPE_T,     /* Тип запроса на чтение                       */
    MDM_ABI_SCHED_CTRL_T,    /* команда для data scheduler:mdm_sched_ctrl_e */
    MDM_ABI_STAT_UNIT_T,     /* запрашиваемая гранулярность данных:filetime */
    MDM_ABI_STAT_TYPE_T,     /* Тип запроса на статистику:mdm_stat_type_e   */
    MDM_ABI_STAT_SIZE_T,     /* Размер данных статистики:uint32_t           */
    MDM_ABI_LAST,            /* Для связи и проверки...                     */
} mdm_abi_types_e;

/*\
 * Набор значений для состояний и запроса состояния
 * функционального/пакетного API подсистемы
\*/
typedef enum {
    MDM_STATE_GET,         /* получить состояние подсистемы                 */
    MDM_STATE_RW,          /* режим работы чтение/запись (без ограничений)  */
    MDM_STATE_WONLY,       /* режим работы только запись                    */
    MDM_STATE_RONLY,       /* режим работы только чтение                    */
    MDM_STATE_STOP,        /* остановить подсистему/подсистема остановлена  */
    MDM_STATE_CHECK,       /* выполнить диагностику/выполняется диагностика */
    MDM_STATE_FAIL,        /* неустранимый сбой при работе подсистемы       */
} mdm_state_e;

/* промежуток агрегации/per sample */
typedef p3_stat_scale_e mdm_stat_scale_e; 

/* Набор базовых кодов типов чтения данных */
typedef enum {
    MDM_READ_DATA    =(1<< 0), /* Запрос на данные                          */
    MDM_READ_META    =(1<< 1), /* Запрос на мета данные фрейма              */
    MDM_READ_LEFT    =(0<< 2), /* Запрос на данные левый интервал           */
    MDM_READ_RIGHT   =(1<< 2), /* Запрос на данных правый интервал          */
    MDM_READ_NUM_MASK=(1<< 3), /* Маска типа запроса данных                 */
    MDM_READ_SINGLE  =(0<< 3), /* Запрос на один сэмпл данных               */
    MDM_READ_MULTIPLE=(1<< 3), /* Запрос на несколько сэмплов данных        */
    MDM_READ_REALTIME=(1<< 4), /* Запрос на данные для RealTime playing     */
    MDM_READ_LOOP    =(1<< 5), /* Запрос на циклическое чтение данных       */
    MDM_READ_SETUP   =(1<< 6), /* Запрос с активизацией по команде PLAY     */

    MDM_READ_META_MSK=(7<< 9), /* Маска запроса метаданных                  */ 
    MDM_READ_META_CL1=(7<< 9), /* Запрос метаданных в L1 RAW формате        */ 
    MDM_READ_META_T0 =(1<< 9), /* Запрос метаданных T0 из L1                */
} mdm_read_type_e;

/* Набор базовых кодов типов чтения данных */
typedef enum {
    MDM_STAT_ACTIVITY=(1<< 0), /* Запрос на данные активности               */
    MDM_STAT_MASK    =(1<< 1), /* Запрос на маску наличия чанков данных     */
    MDM_STAT_LEFT    =(0<< 2), /* Запрос на данные левый интервал           */
    MDM_STAT_RIGHT   =(1<< 2), /* Запрос на данных правый интервал          */
} mdm_stat_type_e;

/*\
 * Набор значений для состояний, изменения и запроса состояния системы mdm
 * функционального/пакетного API подсистемы
\*/
typedef enum {
    MDM_SCHED_REQ_PAUSE,    /* Приостановить выдачу данных по запросу       */
    MDM_SCHED_REQ_RESUME,   /* Продолжить выдачу данных по запросу          */
    MDM_SCHED_REQ_CANCEL,   /* Прервать и удалить запрос                    */
    MDM_SCHED_REQ_GET_LIST, /* Получить список текущих запросов             */
} mdm_sched_ctrl_e;

/* Набор базовых кодов флагов meta данных */
typedef enum {
    MDM_META_DCHUNK_START=(1<< 0),       /* создан новый data chunk         */
    MDM_META_DCHUNK_STOP =(1<< 1),       /* завершен data chunk             */
    MDM_META_RECORD_START=(1<< 2),       /* Создана новая запись            */
    MDM_META_RECORD_STOP =(1<< 3),       /* Запись завершена                */
    MDM_META_EVENT       =(1<< 4),       /* Произошло событие(see OOB_DATA) */
    MDM_META_OOB         =(1<< 5),       /* OOB Data present                */

    MDM_META_USER_MASK   =		 /* User events mask                */
    			   MDM_META_RECORD_START|
    			   MDM_META_RECORD_STOP|
			   MDM_META_EVENT,
} mdm_meta_flags_e;

typedef struct {
#if __BYTE_ORDER == __LITTLE_ENDIAN
	uint32_t	len:24;
	uint32_t	flags:8;
#elif __BYTE_ORDER == __BIG_ENDIAN
	uint32_t	flags:8;
	uint32_t	len:24;
#else
#error  "Please fix <bits/endian.h>"
#endif
} __attribute__((packed))  mdm_flen_t;

typedef struct {
	uint32_t	ts;			/* frame time offset */
	uint32_t	offs;			/* frame data offset */

	union {					/* frame data size */
		mdm_flen_t	flens;
		uint32_t	flenv;
		uint8_t		flen8[4];
	} ;

	union {					/* frame info */
		p3_frame_descr_t	info;
		uint32_t		infov;
	} ;
} __attribute__((packed)) mdm_l1_chunk;

typedef struct {		/* attantion this struct should be in sync with mdm_l1_t */
	uint64_t	t0;
	uint64_t	t1;
	uint64_t	th;
	uint64_t	reserved;

	mdm_l1_chunk	cache[];
} __attribute__((packed)) _mdm_l1_t;

typedef struct {		/* attantion this struct should be in sync with _mdm_l1_t */
	filetime	t0;	/* oldest data */
	filetime	t1;	/* youngest data */
	filetime	th;
	uint64_t	reserved;

	mdm_l1_chunk	cache[MDM_MAX_L1_LINES+2];

	int		idx;	/* current or max cache index */
	int		sidx;	/* last cache search */
} __attribute__((packed)) mdm_l1_t;

typedef struct {		/* attantion this struct should be in sync with _mdm_l1_t */
	uint16_t		idx;	/* current or max cache index */
	uint16_t		sidx;	/* last cache search */
} __attribute__((packed)) mdm_l1_resp_t;

/*\
 *  MDM ABI ASCII Constants
\*/
ABI_STRING_CONST(MDM_ABI_CL1_HEAD_T,	"MDM CL1H");	/* _mdm_l1_t         */
ABI_STRING_CONST(MDM_ABI_CL1_DATA_T,	"MDM CL1D");	/* mdm_l1_chunk []   */
ABI_STRING_CONST(MDM_ABI_CL1_RESP_T,	"MDM CL1R");	/* mdm_l1_resp_t     */

/* Определения структур */
#if defined(WIN32) && !defined(__GNUC__)
#include <compilers/visualc/align1.h>
#endif
/*\
 * Идентификатор или запрос на данные/метаданные/статистику фрейма
 * или групы фреймов для функционального API/пакетного ABI
\*/
typedef struct mdm_request_s {
        filetime       te;        /* te-finish, ts-start точка выполненного  */
        filetime       ts;        /* запроса или данных в наличии            */

        filetime       tn;        /*  предпологаемый ts следующего фрейма    */
        filetime       tu;        /*  requested granularity scale            */
	
        uint64_t       th;        /* th data hash                            */
	
        union {                      /* (4 octets) "контейнер" параметра 1   */
        	p3_stat_scale_e scale;    /* requested granularity scale     */

                p3_frame_descr_t frame;  /* описатель данных фрейма          */
                uint32_t        framev;  /* описатель данных фрейма as val   */
        } ;
        uint32_t        priority; /* Запрашиваемый приоретет запроса         */
        uint32_t        size;     /* размер данных буфера для запроса        */
        uint32_t        max_size; /* MAX размер буфера для запроса           */

	/* CL1 var */
/* ====> DON'T TOUCH THIS - ORDER IS IMPORTANT!!!! <====== */
        filetime	t1;        /* t0-start, t1-finish точка запрашиваемо- */
        filetime	t0;        /* го или заполняемого интервала времени   */
	mdm_l1_chunk	l1[2];
	uint32_t	l1_index;
/* ======================================================= */

} __attribute__((packed)) mdm_request_t;

/*\
 * Идентификатор блока данных пренадлежащих потоку
\*/
typedef struct mdm_record_id_s {
    filetime       t0;        /*( 8 octets) t0-начальная, t1-конечная       */
    filetime       t1;        /*( 8 octets) точка интервала времени         */

    p3_id_t        gid; /* Идентификатор записи */
} __attribute__((packed)) mdm_record_id_t;

#if defined(WIN32) && !defined(__GNUC__)
#include <compilers/visualc/alignx.h>
#endif
#ifdef __cplusplus
}
#endif

#endif
