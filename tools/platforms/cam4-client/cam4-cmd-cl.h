#ifndef	__CAM4_CMD_CL_
#define	__CAM4_CMD_CL_
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

#include <abi/p3.h>
#include <abi/p3_helpers.h>
#include <abi/io.h>
#include <abi/device.h>
#include <api/mcast-cl.h>
#include <os-helpers/net-sockets.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <api/cm-aca.h>

#include <avi/structures.h>
#include <abi/cam.h>

typedef struct cam4_rc_auth_s {
	data_t		login;
	data_t		password;
	uint8_t		auth_mode;
} cam4_rc_auth_t;

typedef struct cam4_cmd_cl_s {
	io_ctrl_t		ioc;

	uint32_t		as_ctx;
	//p3_gid_t		cam4_gid;
	p3_id_t			cam4_id;
	p3_conn_ipv4_t		conn_ipv4;		/* ipv4 of a netcam4 */
	p3_conn_ipv4_t		conn_cl;		/* ipv4 of a netcam4 */
	uint32_t		max_buff;

	struct in_addr 		mcast_addr;		/* addr of a RAW stream client */

	volatile int		*no_sig_exit;		/* exit flag */

	p3_id_t			in_id;

	uint32_t		in_fmt_id_be;		/* BE format id */
	uint32_t		in_tag_type_be;		/* BE format tag type */

	filetime		in_t0;
	filetime		in_t1;

	filetime		in_t0_be;
	filetime		in_t1_be;

	uint64_t		in_seq64_be;

	uint32_t		in_framev;		/* data subtype	*/
	uint32_t		in_size;		/* data size	*/
	uint8_t			*in_data;		/* data ptr	*/

	uint32_t		in_w;			/* frame width	*/
	uint32_t		in_h;			/* frame height	*/

	int			wmode;			/* write mode   */
	uint32_t		bars_info;		/* objects outlined */

	uint8_t			im_r_bits;
	uint8_t			im_s_bits;
	uint16_t		pad0;

	uint16_t		im_w;
	uint16_t		im_h;
} cam4_cmd_cl_t;

/* func defenition */
extern int cam4_cmd_cl_process(
	cam4_cmd_cl_t		*cam4_rc,
	uint8_t			mode,
	uint8_t			flow,
	int			flag
);
#endif	/* __CAM4_CMD_CL_ */
