#ifndef __NET_CAP_FILES_H__
#define __NET_CAP_FILES_H__
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

#include <sys/time.h>

typedef struct {
	int32_t tv_sec;
	int32_t tv_usec;
} pcap_timeval_t;

typedef struct {
	pcap_timeval_t		tv_last;
	uint32_t		len;
} __attribute__((packed)) header_tcpdump1_t;

typedef struct {
	header_tcpdump1_t	header_own; 
	uint32_t		len;
} __attribute__((packed)) tcpdump_packet_header_t;

#endif // !__NET_CAP_FILES_H__
