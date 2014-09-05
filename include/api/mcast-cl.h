#ifndef __MCAST_CL
#define __MCAST_CL
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
#include <abi/cm.h>
#include <abi/device.h>
#include <api/cm-aca.h>
#include <abi/lb.h>
#include <os-helpers/net-sockets.h>
#include <os-helpers/pthreads.h>

#define	IFS_NUM_MCL		8
#define	CL_NUM_MCL		8

typedef struct mcast_cl_client_s {		/* P3_CLIENT_N */
    /* P3_CLIENT_N */
    p3_client_id_t	cdescr;
    p3_gid_t   		gid_cl;
    p3_conn_ipv4_t	conn_cl;
} mcast_cl_client_t;

typedef struct mcast_cl_interface_s {		/* P3_INTERFACE_N */
    uint32_t		type;
    p3_gid_t   		gid_in;
    p3_conn_ipv4_t	conn_in;
    p3_load_param_t	server_param;
    uint32_t		caps;

    uint8_t		cl_idx;			/* index inside client[] */
    mcast_cl_client_t	client[CL_NUM_MCL];	/* P3_CLIENT_N */
} mcast_cl_interface_t;

typedef struct mcast_cl_payload_s {
	data_t			hostname;
	uint8_t			hostname_data[255];

	lb_string254_t		dev_name;	/* device name */
	uint32_t		sub_model;	/* device submodel */
	uint64_t		cpu_id;
	uint8_t			mac[6];
	p3_msg_part_t		msg_id;
	p3_fmw_rev_t		fm_rev;		/* firmvare revision */
	uint32_t		ctx;		/* requester context */
	uint32_t		dev_class;

	/* P3_INTERFACE_N */
	uint8_t			ifs_idx;	/* index inside interface[] */

	mcast_cl_client_t	cl_in;		/* */
	mcast_cl_interface_t	ifs;
	mcast_cl_interface_t	interface[IFS_NUM_MCL];	

	void			*priv;		/* you can save here ptr to device depended data */

} mcast_cl_payload_t;

struct mcast_cl_api_s;
typedef int mcast_cl_cb_f(struct mcast_cl_api_s *);

typedef struct mcast_cl_api_s {
	/* device related callbacks */
	//mcast_cb_f		*init;
	//mcast_fill_ns_info_f	*fill_ns_info;
	
	mcast_cl_cb_f		*abir_custom;		/* add client keys in ABI reader */
	mcast_cl_cb_f		*release;		/* release client data */
	mcast_cl_cb_f		*find_dev;		/* find interface */

	void			*priv;			/* API Private data PTR */

	char			*mcast_addr;
	int			mcast_port;
	abi_mem_flow_t		rx_flow;
	abi_io_task_t		tx_task;		/* ACA output fifo */

	abi_context_t		*rx_abi;
	uint8_t			*rx_buf;
	size_t			rx_buf_size;

	mcast_cl_payload_t	rx_vars;

	int			top_level;
	int			interface_level;	/* P3_INTERFACE_N */
	int 			client_level;		/* P3_CLIENT_N */

	/* need this device */
	uint64_t		cpu_id;			
	struct sockaddr_in	ipv4_device;		

	/* network vars */
	struct sockaddr_in	ipv4_src;		// FIXME - checkme
	struct sockaddr_in	ipv4_dst;

	struct pollfd		pfds[1];

	/* network part */
	//io_ctrl_t		ioc;

	volatile int		*no_sig_exit;		/* FIXME - checkit */
	int			local_exit;
} mcast_cl_api_t;

/* functions */
void *process_cl_mcast(void *priv);
int mcast_cl_init(mcast_cl_api_t *mcast_cl);
void mcast_cl_destroy(mcast_cl_api_t *mcast_cl);

#endif /* __MCAST_CL */
