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

#include <fcntl.h>

#define TRACE_PRIVATE_PREFIX    1
#include <trace.h>
#undef  TRACE_LEVEL
#define TRACE_LEVEL 0 
#include <math/swap.h>
#include "cam4-cmd-cl.h"

#include <abi/lb-task-bar.h>

static char* trace_prefix = "CMD-CL:\t";

int  mcast_add_if(
	void		 *ctx,
       	mcast_if_descr_t *i,
	int		 port,
       	uint32_t	 type,
       	uint32_t	 max
)
{
	return 0;
}

int  mcast_del_if(mcast_if_descr_t *i)
{
	return 0;
}

int  mcast_del_cl(mcast_if_descr_t *i, int s)
{
	return 0;
}

int  mcast_add_cl(mcast_if_descr_t *i, int s)
{
	return 0;
}

static void cam4_dump_jpeg_file(
	cam4_cmd_cl_t		*cmd
)
{
	char		name[50];
	static int	id = 0;
	int		fd, res;

	snprintf(name, sizeof(name), "./out/img%016"PRIx64".%010u.jpeg",
		filetime_to_host(&cmd->in_seq64_be),
		id
	);
	fd = open(name, O_WRONLY|O_CREAT, 0660);

	if(fd <0) {
		ETRACE("when open dump file %s:", name);
		return ;
	}
	
	res = write(fd, cmd->in_data, cmd->in_size);
	if(res <0) {
		ETRACE("when write dump file:");
	}

	if(res < cmd->in_size) 
		TRACEPNF(0, "Partial data dumped: %d instead of %d:\n", res, cmd->in_size);

	close(fd);

	id ++;
}

static void cam4_dump_face_file(
	cam4_cmd_cl_t		*cmd
)
{
	char		name[150];
	static int	id = 0;
	int		fd, res;

	if(!cmd->in_size)
		return ;

	snprintf(name, sizeof(name), "./out/img%010u_%04dx%04d_%02dbits.raw",
	    id,
	    cmd->im_w, cmd->im_h,
	    cmd->im_s_bits
	);

	fd = open(name, O_WRONLY|O_CREAT, 0660);

	if(fd <0) {
		ETRACE("when open dump file %s:", name);
		return ;
	}
	
	res = write(fd, cmd->in_data, cmd->in_size);
	if(res <0) {
		ETRACE("when write dump file:");
	}

	if(res < cmd->in_size) 
		TRACEPNF(0, "Partial data dumped: %d instead of %d:\n", res, cmd->in_size);

	close(fd);

	id ++;
}


static int cam4_on_frame(abi_context_t *abi, abi_node_t *node)
{
	abi_mem_flow_t		*f		= abi->priv;

	cam4_cmd_cl_t		*cmd		= (cam4_cmd_cl_t *)f->ctx;

	TRACEPNF(0, "%-7d@[%016"PRIx64"-%016"PRIx64") seq:%016"PRIx64" c:%08x f:%02x bars:%08x\n",
		cmd->in_size,
		cmd->in_t0,
		cmd->in_t1,
		filetime_to_host(&cmd->in_seq64_be),
		cmd->in_framev,
		cmd->in_id.gid.val[23],
		cmd->bars_info
	);

	if(cmd->wmode != 1)
		return 0;

	switch (cmd->in_id.gid.val[23]) {
	    case CAM4_JPEG_IFS:
		cam4_dump_jpeg_file(cmd);
	    	break;

	    case CAM4_FACE_DEV_IFS:
		cam4_dump_face_file(cmd);
	    	break;
	}
	return 0;
}

static int cam4_read_meta(void *priv, uint8_t *data, size_t size, size_t tail)
{
	abi_mem_flow_t		*f		= priv;
	cam4_cmd_cl_t		*cmd		= (cam4_cmd_cl_t *)f->ctx;

	p3_frames_t		*meta_be	= f->data + f->offs;
   
   	size += tail;
	f->offs += size;

	if(size < sizeof(p3_frames_t)) {
		TRACEPNF(0, "Invalid format_tag\n");
		return 0;
	}

	cmd->in_t0	= filetime_to_host(&meta_be->t0);
	cmd->in_t1	= filetime_to_host(&meta_be->t1);

	cmd->in_size	= ntohl(meta_be->size);
	cmd->in_framev	= ntohl(meta_be->framev);

	return 0;
}

static int cam4_read_data(void *priv, uint8_t *data, size_t size, size_t tail)
{
	abi_mem_flow_t	*f = priv;
	cam4_cmd_cl_t		*cmd		= (cam4_cmd_cl_t *)f->ctx;

//	TRACEPNF(3, "CKb: [%3lu+%3lu]\n", (unsigned long)size, (unsigned long)tail);
	size += tail;

	cmd->in_data	= f->data + f->offs;
	cmd->in_size	= size;

	f->offs 	+= size;

	return 0;
}

static int cam4_read_skip(void *priv, uint8_t *data, size_t size, size_t tail)
{
	abi_mem_flow_t	*f = priv;

    //    TRACEPNF(3, "CKb: [%3lu+%3lu]\n", (unsigned long)size, (unsigned long)tail);
	
	f->offs += size + tail;

	return 0;
}

static int cam4_read_format_tag(void *priv, uint8_t *data, size_t size, size_t tail)
{
	abi_mem_flow_t		*f = priv;
	cam4_cmd_cl_t		*cmd = (cam4_cmd_cl_t *)f->ctx;
	BITMAPINFOHEADER	*h = f->data + f->offs;
   
   	size += tail;
	f->offs += size;

	if(size < sizeof(BITMAPINFOHEADER)) {
		TRACEPNF(0, "Invalid format_tag\n");
		return 0;
	}

	cmd->in_w = LE32_TO_CPU(h->lWidth);
	cmd->in_h = LE32_TO_CPU(h->lHeight);

	return 0;
}

static int cam4_read_image_info(void *priv, uint8_t *data, size_t size, size_t tail)
{
	abi_mem_flow_t		*f    = priv;
	cam4_cmd_cl_t		*cmd  = (cam4_cmd_cl_t *)f->ctx;

	p3_2d_child_t		*im = f->data + f->offs;
   
   	size += tail;
	f->offs += size;

	if(size) {
		cmd->im_w		= ntohs(im->child.dims.w);
		cmd->im_h		= ntohs(im->child.dims.h);

		cmd->im_r_bits		= im->r_bits;
		cmd->im_s_bits		= im->s_bits;
	} else {
		cmd->im_w		= 0;
		cmd->im_h		= 0;

		cmd->im_r_bits		= 0;
		cmd->im_s_bits		= 0;
	}

	return 0;
}

static int cam4_read_2d_bars(void *priv, uint8_t *data, size_t size, size_t tail)
{
	abi_mem_flow_t		*f    = priv;
	cam4_cmd_cl_t		*cmd  = (cam4_cmd_cl_t *)f->ctx;

	p3_2d_id_bars_t		*bars = f->data + f->offs;
   
   	size += tail;
	f->offs += size;

	cmd->bars_info = ntohs(bars->items) | (bars->flags<<16)| (bars->type<<24);

	return 0;
}

static int cam4_cl_abir_init(cam4_cmd_cl_t *cmd)
{
	int		res, oob_node;
	io_ctrl_t	*ioc	= &cmd->ioc;

	ioc->rx_flow.ctx	= cmd;		/* save ctx */

	TRACEPNF(0, "[%s()]\n", __func__);

	abi_context_t *abi = cmd->ioc.rx_abi = abir_open(
	    NULL,
	    &ioc->rx_flow,
	    abi_mem_reader,
	    abi_mem_skiper,
	    40
	);

	if(!abi)
		return -P3_FAILED;
	
	abi_node_t	tmp	= { };

	ABI_SET_KEY32(tmp, DEVICE_ABI_FRAME_RESP);
	ABI_SET_ACTION(tmp, cam4_on_frame);
	res = abi_append_node(abi, &tmp);

	ABI_SET_KEY32_VAR(tmp, P3_ABI_ID_T, cmd->in_id);
	abi_append_subnode(abi, res, &tmp);

	ABI_SET_KEY32_VAR(tmp, P3_ABI_GID_T, cmd->in_id.gid);
	abi_append_node(abi, &tmp);

	ABI_SET_KEY32(tmp, DEVICE_ABI_REQUEST_T);
	ABI_SET_READER(tmp, cam4_read_meta);
	abi_append_node(abi, &tmp);

	ABI_SET_KEY32(tmp, DEVICE_ABI_FRAME_DATA_T);
	ABI_SET_READER(tmp, cam4_read_data);
	abi_append_node(abi, &tmp);

	abi_append_node(abi, &tmp);
	ABI_SET_KEYA(tmp, P3_ABI_OOB_DATA);
	oob_node = abi_append_node(abi, &tmp);

	ABI_SET_KEY32(tmp, P3_ABI_REQUEST_N);
	res = abi_append_node(abi, &tmp);

	ABI_SET_KEYA(tmp, P3_ABI_2D_BARS_T)
	ABI_SET_READER(tmp, cam4_read_2d_bars);
	abi_append_subnode(abi, oob_node, &tmp);

	ABI_SET_KEYA(tmp, P3_ABI_RAW_IMAGE_T);
	ABI_SET_READER(tmp, cam4_read_image_info);
	abi_append_node(abi, &tmp);

	ABI_SET_KEYA(tmp, P3_ABI_RAW_VIDEO_SOURCE_T);	/* video_frame_raw_hdr_t */
	ABI_SET_READER(tmp, cam4_read_skip);
	abi_append_node(abi, &tmp);

	ABI_SET_KEYA(tmp, P3_TEVENT_T);			/* GP events */
	ABI_SET_READER(tmp, cam4_read_skip);
	abi_append_node(abi, &tmp);

	ABI_SET_KEYA(tmp, P3_ABI_CAMCTRL_STATE_T);	/* CAMCTRL state */
	ABI_SET_READER(tmp, cam4_read_skip);
	abi_append_node(abi, &tmp);

	ABI_SET_KEYA(tmp, P3_ABI_CAMCTRL_LHIST_T);	/* CAMCTRL hist */
	ABI_SET_READER(tmp, cam4_read_skip);
	abi_append_node(abi, &tmp);

	ABI_SET_KEYA_VAR(tmp, P3_FLOW_FORMAT_ID_T, cmd->in_fmt_id_be);
	abi_append_subnode(abi, res, &tmp);

	ABI_SET_KEY32_VAR(tmp, P3_ABI_TIME_POS_T0_T, cmd->in_t0_be);
	abi_append_node(abi, &tmp);

	ABI_SET_KEY32_VAR(tmp, P3_ABI_TIME_POS_T1_T, cmd->in_t1_be);
	abi_append_node(abi, &tmp);

	ABI_SET_KEYA_VAR(tmp, P3_ABI_SEQ64_T, cmd->in_seq64_be);
	abi_append_node(abi, &tmp);

	ABI_SET_KEYA_VAR(tmp, P3_FORMAT_TAG_TYPE_T, cmd->in_tag_type_be);
	abi_append_node(abi, &tmp);

	ABI_SET_KEYA(tmp, P3_FORMAT_TAG_T);
	ABI_SET_READER(tmp, cam4_read_format_tag);
	abi_append_node(abi, &tmp);

	return 0;
}


int cam4_init_abiw_stop(cam4_cmd_cl_t *cam4_rc, abi_context_t *abiw)
{
	int 		rnode;
/*\
TYPE p3_qos_e = uint32_t;

[DEVICE_ABI_STOP_FLOW_CMD] {
}
*/
	abi_node_t	tmp	= { };

	TRACEPNF(0, "[%s]\n", __func__);
	
	ABI_SET_KEY32(tmp, DEVICE_ABI_STOP_FLOW_CMD);
	rnode = abi_append_node(abiw, &tmp);
	
	ABI_SET_KEY32_VAR(tmp, P3_ABI_ASYNC_CONTEXT_T, cam4_rc->as_ctx);
	abi_append_subnode(abiw, rnode, &tmp);

	ABI_SET_KEY32_VAR(tmp, P3_ABI_ID_T, cam4_rc->cam4_id);
	abi_append_node(abiw, &tmp);

	return 0;
}

int cam4_init_abiw_start(cam4_cmd_cl_t *cam4_rc, abi_context_t *abiw)
{
	int 		rnode, req, dst;
	abi_node_t	tmp	= { };

	TRACEPNF(0, "[%s]\n", __func__);

	cam4_rc->as_ctx = 0;

/*\
TYPE p3_qos_e = uint32_t;

[DEVICE_ABI_START_FLOW_CMD] {
    [P3_ASYNC_CONTEXT]		OPTIONAL (uint32_t);

    CHOICE REQUIRED {
	[P3_ABI_ID_T]		        (p3_id_t);
	[P3_ABI_GID_T]		        (p3_gid_t);
	[P3_ABI_OFFSET_GID_T]		(blob_t);		// TODO
    }

    [P3_ABI_SRC_FLOWS_N]	MULTIPLE {			// TODO
	[P3_ABI_GID_T]		OPTIONAL (blob_t);
	[P3_ABI_OFFSET_GID_T]	OPTIONAL (blob_t);
	[P3_ABI_MASK_T]		OPTIONAL (blob_t);	
    };

    [P3_ABI_QOS_T]		OPTIONAL (p3_qos_e);		// TODO

    [P3_ABI_DST_FLOWS_N]	MULTIPLE {			
	[P3_ABI_GID_T]		OPTIONAL (p3_gid_t);		// TODO
	[P3_ABI_OFFSET_GID_T]	OPTIONAL (blob_t);		// TODO
	[P3_CONN_IPV4_T]	OPTIONAL (p3_conn_ipv4_t);
    };

    [P3_ABI_REQUEST_N]		REQUIRED {
	// Request Bounds
	[P3_ABI_TIME_POS_T0_T]	OPTIONAL (filetime);		// TODO
	[P3_ABI_TIME_POS_T1_T]	OPTIONAL (filetime);		// TODO

	// Filters
	[P3_ABI_FRAME_INFO_T]	OPTIONAL (uint32_t);		// TODO
	[P3_ABI_MAX_BUFF_T]	REQUIRED (uint32_t);

	// Rate control
	CHOICE {
	    [P3_ABI_READ_RATE_T] OPTIONAL (fp16x16_t);
	    [P3_ABI_SKIP_RATE_T] OPTIONAL (fq16x16_t);
	}
    };
};
\*/

	ABI_SET_KEY32(tmp, DEVICE_ABI_START_FLOW_CMD);
	rnode = abi_append_node(abiw, &tmp);

	ABI_SET_KEY32_VAR(tmp, P3_ABI_ASYNC_CONTEXT_T, cam4_rc->as_ctx);
	abi_append_subnode(abiw, rnode, &tmp);

	ABI_SET_KEY32_VAR(tmp, P3_ABI_ID_T, cam4_rc->cam4_id);
	abi_append_node(abiw, &tmp);

	ABI_SET_KEYA(tmp, P3_ABI_DST_FLOWS_N);
	dst = abi_append_node(abiw, &tmp);

	ABI_SET_KEY32(tmp, P3_ABI_REQUEST_N);
	req = abi_append_node(abiw, &tmp);

	ABI_SET_KEY32_VAR(tmp, P3_CONN_IPV4_T, cam4_rc->conn_cl);	
	abi_append_subnode(abiw, dst, &tmp);
	
	ABI_SET_KEY32_VAR(tmp, P3_ABI_MAX_BUFF_T, cam4_rc->max_buff);
	abi_append_subnode(abiw, req, &tmp);
#if 0
	static uint32_t count;
	count = htonl(100);
	ABI_SET_KEYA_VAR(tmp, P3_ABI_UNACK_FRAMES_T, count);
	abi_append_node(abiw, &tmp);
#endif
	/* dump abiw packet */
	if( TRACE_LEVEL > 2 )
		abiw_dump(abiw);

	return 0;
}

static void cam4_cmd_iterator(
	cam4_cmd_cl_t		*cmd
)
{
	do {
		abi_io_iterator(&cmd->ioc, 10, 10);
	} while(cmd->no_sig_exit[0]);
}

int cam4_cmd_cl_process(
	cam4_cmd_cl_t		*cmd,
	uint8_t			mode,
	uint8_t			flow,
	int			flag
)
{
	int			res;
	int			task;

	p3_conn_ipv4_t		*conn = &cmd->conn_ipv4;

	TRACEPNF(0, "[%s()]\n", __func__);
	
	io_ctrl_t	*ioc = &cmd->ioc;

	if(!ioc->rx_task) {
		cmd->ioc.rx_flow.ctx = cmd;

		ABI_SUB_OP( abi_init_ioc(
		    ioc,
		    1,
		    cmd,
		    NULL,			/* interfaces handler- NULL for client */
		    1<<21,			/* rx buff size */
		    4096,			/* tx buff size */
		    trace_prefix		/* subsystem name */
		));

		cam4_cl_abir_init(cmd);

		TRACEP(0, "Try to connect to CMD interface @%s:%d\n",
			inet_ntoa(((struct in_addr*)&conn->ip)[0]),
			conn->port
		);
		
		task = abi_connect_inet_socket(
			ioc,
			inet_ntoa(((struct in_addr*)&conn->ip)[0]),
			conn->port
		);

		if(task < 0)
			return -1;

		/* init writer ABI */
		ioc->rx_task = ioc->tasks + task;
	}

	TRACEPNF(0, "Task ID %08"PRIxPTR"\n", (intptr_t)ioc->rx_task);

	abi_io_task_t 	*tw	= &ioc->rx_task->writer;

	abi_writer_task_reset(tw);

	if(mode) /* stop flow */
		res = cam4_init_abiw_stop(cmd, tw->abi);
	else /* start flow */
		res = cam4_init_abiw_start(cmd, tw->abi);

	/* FIXME - replace with CAM4_RAW_IFS constant */
	cmd->cam4_id.gid.val[23] = flow;

	if(res < 0) {
	    TRACEP(0, "[ERR] Cannot create CMD packet\n");
	    return -1;
	}

	abiw_write_task(tw, (int)1);
	abi_task_send(ioc->rx_task, NULL, 0);

	if(flag)
		cam4_cmd_iterator(cmd);

	return 0;
}
