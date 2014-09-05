#ifndef __CAM4_PS_H__
#define __CAM4_PS_H__
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

#include <inttypes.h>

#include <abi/ip-video-raw.h>
#include "cam4-cmd-cl.h"
#include "shared_objects.h"

#include "debayer_api.h"
#include "cam4_ps-lut.h"

enum video_write{
	VIDEO_WRITE_START,
	VIDEO_WRITE_PROCESS,
	VIDEO_WRITE_FINISH,
	VIDEO_WRITE_NONE
} video_write;

struct cam4_rd_s;

typedef int write_cb_f(struct cam4_rd_s *);
typedef struct cam4_rd_s {
	uint16_t			start_mode;

	char				f_name[256];

	uint16_t			frame_num;
	uint16_t			frame_idx;
	write_cb_f			*write_cb;
	uint8_t				*img;
	uint8_t				*buff_img;
	uint8_t				*buff_fd[2];
	video_frame_raw_t		fdata_h[2];
	uint8_t 			clear_buff;
	uint32_t			used_buf_space;
	int				fd;

	/* buffer */
	uint8_t				buff_fh[65536];
	video_frame_raw_hdr_t		FH;
	video_frame_raw_t		*pFD;

	uint32_t			done;

	pthread_t			dump_thread;
	uint8_t				idx;


	/* mcast stuff */
	mcast_cl_api_t			mcast_cl;
	pthread_t			mcast_cl_thread;
	pthread_t 			cm_thread;

	/* cam4 cmd interface */
	cam4_cmd_cl_t			cam4_cl;
	mcast_cl_interface_t		*cm_ifs;
	mcast_cl_interface_t		*raw_ifs;

	abi_io_tasks_t			*ts;

	uint32_t			fh_size;
	unsigned			bits;
	unsigned			w;
	unsigned			h;

	char*				camera_ip_str;
	char				window_title[64];
	char* 				device_name;
	int 				device_name_size;

	enum video_write		video_writing;
	enum video_write    		raw_video_writing;

	int				mcast_reinit;
	int				first_time;
	struct timeval			last_time;

	struct timeval			dumpraw_time;
	struct timeval			dumpyuv_time;
	int				xvHeight;
	int				xvWidth;

	common_t*			common;

	int				raw_video_fd;
	avi_rwh_t			*avi;

	uint8_t				flow_id;

	uint8_t				pad1;
	uint8_t				pad2;
	uint8_t				pad3;

	p3_id_t				id;

	uint32_t			camctl_mode;

	uint8_t*			flipped_img;
	size_t				flipped_img_size;

	uint32_t			state_size;
	uint8_t				disable_mcast;
	uint32_t			ifs_port;
	struct	ip			*iph;

	// for EMVA tests define correct path
	char 				path[256];
	char 				mode[256];
	char                		pair[16];
	uint32_t			img_num;
	uint32_t 		    	expo;

	debayer_api_t			d_api;
} cam4_rd_t;

typedef int (*cam4_ps_cb_f)(cam4_rd_t *hdr);

extern int cam4_ps_start(int argc, char **argv);
extern void cam4_ps_stop(void);
extern void cam4_ps_set_cb(void *func);

#endif
