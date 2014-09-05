#ifndef __ABI_CM_H__
#define __ABI_CM_H__
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

#if defined(CONFIG_ISOLATED)
#include <isolated/abi/p3.h>
#else
#include <abi/p3.h>
#endif

/*  */
/* Объявления констант */

/* Набор базовых кодов операций ABI */
typedef enum {
    CONFIG_ABI_GET_STAT_CMD      /* команда запроса конфигурации            */
     = P3_ABI_CFM0,              /*                                         */
    CONFIG_ABI_STAT_RESP,        /* Ответ на CONFIG_ABI_GET_STAT_CMD        */
    CONFIG_ABI_CONFIGURE_CMD,    /* команда установки конфигурации host     */

    P3_CM_RES0                   /* 1500 reserved 0                         */
     = P3_ABI_CFM1,              /* 1500                                    */
    P3_CM_RES1,                  /* 1501 reserved 1                         */
    P3_CM_STATUS_RESP,           /* 1502 статус выполнения команды          */
    P3_CM_GET_VALS_CMD,          /* 1503 команда запроса данных из CM       */
    P3_CM_DEL_VAL_CMD,           /* 1504 команда удаления данных из CM      */
    P3_CM_UPDATE_VAL_CMD,        /* команда изменения/установку данных в CM */
    P3_CM_APPEND_VAL_CMD,        /* 1506 команда дополнения данных в CM     */

    P3_CM_VALS_RESP,             /* ответ на команду запроса данных из CM   */
    
    P3_CM_ABI_LAST_,             /* Для связи и проверки...                 */
} config_abi_ops_e;

/* Набор базовых кодов типов данных ABI компонента                          */
typedef enum {
    CONFIG_ABI_IPV4_NET_T        /* IP v4 network configuration             */
     = P3_ABI_T_CFM,             /* 0xc8 Первый ткод для Phobos 3 CM        */
    CONFIG_ABI_IPV6_NET_T,       /* IP v6 network configuration             */
    P3_ABI_KEY_T,		/* 0xca */
    P3_ABI_KEY_HI_T,		/* 0xcb */
    P3_ABI_KEY_MASK_T,		/* 0xcc */
    P3_ABI_KEY_ATTR_T,		/* 0xcd */
    P3_ABI_VAL_T,		/* 0xce */
    P3_CM_REQUEST_N,		/* 0xcf */
    P3_CM_REQUEST_T,
    P3_CM_PERIOD_T,
    CONFIG_ABI_LAST_T,     /* Для связи и проверки...                       */
} config_abi_types_e;

/* Набор базовых классов хранимых данных ABI */
typedef enum {
        P3_CONFIG_SCHEDULE        = 0x01, /* Schedule Settings              */
        P3_CONFIG_AUTH            = 0x02, /* Authentication Settings        */
        P3_CONFIG_EVENTS          = 0x03, /* Event handlers Settings        */
        P3_CONFIG_TASKS           = 0x04, /* Task Settings                  */
        P3_CONFIG_NAME            = 0x05, /* Names (devices, channels, etc) */
        P3_CONFIG_RULES           = 0x06, /* Rules Settings                 */
        P3_CONFIG_NET             = 0x07, /* Network Configuration          */
        P3_CONFIG_SST             = 0x08, /* Secure storage                 */
        P3_CONFIG_LOCALIZATION    = 0x09, /* Localization Settings          */
} p3_abi_storage_data_type_e;

/*\
 * Информация о конфигурации сетевого интерфейса
\*/
typedef struct {
        uint32_t        ip;                     /* IPv4 address             */
        uint32_t        mask;                   /* IPv4 netmask             */
        uint32_t        ip_broadcast;           /* IPv4 broadcast address   */
        uint32_t        ip_gw;                  /* IPv4 gateway             */
} __attribute__((packed)) p3_net_cfg_ipv4_t;

typedef struct {
        uint8_t         ip[16];                 /* IPv6 address             */
        uint8_t         mask[16];               /* IPv6 netmask             */
        uint8_t         ip_broadcast[16];       /* IPv6 broadcast address   */
        uint8_t         ip_gw[16];              /* IPv6 gateway             */
} __attribute__((packed)) p3_net_cfg_ipv6_t;

typedef enum {
        P3_CM_EVENT_DEL_OK              = 1<<0x03, /* Event on append       */
        P3_CM_EVENT_APPEND_OK           = 1<<0x04, /* Event on append       */
        P3_CM_EVENT_CHANGE_OK           = 1<<0x05, /* Event on change       */
        P3_CM_EVENT_READ_OK             = 1<<0x06, /* Event on read         */
        P3_CM_EVENT_PERIODIC_OK         = 1<<0x07, /*?Periodic Event        */
        P3_CM_EVENT_PERIODIC_STATE_OK   = 1<<0x08, /*?Periodic state Event  */
        P3_CM_EVENT_INCOMING_DATA_OK    = 1<<0x09, /*?Incoming data Event   */
#if 1
        P3_CM_KEY_EXIST                 = 1<<0x10, /* Key/Keys exists       */
        P3_CM_KEY_DEL_OK                = 1<<0x11, /* Удаление разрешено    */
        P3_CM_KEY_APPEND_OK             = 1<<0x12, /* Дополнение разрешено  */
        P3_CM_KEY_UPDATE_OK             = 1<<0x13, /* Изменение разрешено   */
#endif
} p3_cm_item_attr_e;

typedef enum {
        P3_CM_EVENT_DEL                 = 1<<0x03, /* Event on delete       */
        P3_CM_EVENT_APPEND              = 1<<0x04, /* Event on append       */
        P3_CM_EVENT_CHANGE              = 1<<0x05, /* Event on change       */
        P3_CM_EVENT_READ                = 1<<0x06, /* Event on read         */
        P3_CM_EVENT_PERIODIC            = 1<<0x07, /*?Periodic Event        */
        P3_CM_EVENT_PERIODIC_STATE      = 1<<0x08, /*?Periodic state Event  */
        P3_CM_EVENT_INCOMING_DATA       = 1<<0x09, /*?Incoming data Event   */

        P3_CM_KEY_SCAN                  = 1<<0x1d, /*?Request for keys      */
        P3_CM_KEY_BLOCKING_IO           = 1<<0x1e, /*?Use Blocking I/O      */
        P3_CM_KEY_CLEAR_IO_BUFF         = 1<<0x1f, /*?Clear io before access*/
} p3_cm_request_item_e;

/* Определения структур данных */
#if defined(_MSC_VER) && !defined(__GNUC__)
#include <pshpack1.h>
#endif

/*\
 *
\*/

#if defined(_MSC_VER) && !defined(__GNUC__)
#include <poppack.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
