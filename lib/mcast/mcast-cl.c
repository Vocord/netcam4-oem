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
/*******************************************************************
 *	Maintainer: Alex Nikiforov
 ********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define TRACE_PRIVATE_PREFIX    1
#include <trace.h>
#undef  TRACE_LEVEL
#define TRACE_LEVEL 0 

#include <compiler.h>
#include <api/mcast-cl.h>

#if 0
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
#endif

/***********************************************************/
/*                  mcast part                             */
/***********************************************************/

extern FILE *I;

static char* trace_prefix = "mcast_cl: ";

static void show_device(mcast_cl_api_t	*mcast_cl)
{
	int	 		i, j;
	mcast_cl_payload_t	*rx_vars = &mcast_cl->rx_vars;

	TRACEPNF(0, "Device IP:%s\n", inet_ntoa(mcast_cl->ipv4_dst.sin_addr));

	TRACEPNF(0, "\tInterface:\n");
	for(i = 0; i < rx_vars->ifs_idx; i++ ) {
		mcast_cl_interface_t *ifs = &rx_vars->interface[i];

		TRACEPNF(0, "\t\ttype: %d\n", ifs->type);
		TRACEPNF(0, "\t\tipv4: [%s:%d]\n",
		    inet_ntoa(*(struct in_addr*)&ifs->conn_in.ip),
		    ifs->conn_in.port
		);

		TRACEPNF(0, "\t\tclient:\n");
		for(j = 0; j < ifs->cl_idx; j++ ) {
			mcast_cl_client_t	*cl = &ifs->client[j];
			TRACEPNF(0, "\t\t\tipv4 [%s:%d]\n",
			    inet_ntoa(*(struct in_addr*)&cl->conn_cl.ip),
			    cl->conn_cl.port
			);
		}

	} // for(i = 1; i < IFS_NUM_CL; i++ )
}

static int config_abi_resp(abi_context_t *abi, abi_node_t *node)
{
    abi_mem_flow_t	*f = abi->priv;
    mcast_cl_api_t	*mcast_cl = f->ctx;

    TRACEPNF(3, "CONFIG_ABI_STAT_RESP\n");

    if(mcast_cl->find_dev == NULL) {
	TRACEP(0, "[ERR] you must define find_dev() callback! Exit... \n");
	exit(-1);
    }

    /* we catch device */
    int res = mcast_cl->find_dev(mcast_cl);
    if( res < 0 )
	return 0;

    TRACEPNF(0, "Catch\n");

    show_device(mcast_cl);
    mcast_cl->local_exit = 0;

    return 0;
}

static int abi_async_context_t(abi_context_t *abi, abi_node_t *node)
{
    TRACEPNF(3, "\tP3_ABI_ASYNC_CONTEXT_T\n");
    return 0;
}

static int host_id_t(abi_context_t *abi, abi_node_t *node)
{
    abi_mem_flow_t	*f = abi->priv;
    mcast_cl_api_t	*mcast_cl = f->ctx;
    mcast_cl_payload_t	*rx_vars = &mcast_cl->rx_vars;

    rx_vars->cpu_id = filetime_to_net(&rx_vars->cpu_id);

    TRACEPNF(3, "\tP3_HOST_ID_T [%16"PRIx64"]\n", rx_vars->cpu_id);

    return 0;
}

static int device_sub_model_t(abi_context_t *abi, abi_node_t *node)
{
    TRACEPNF(3, "\tP3_DEVICE_SUB_MODEL_T\n");
    return 0;
}

static int firmware_rev_t(abi_context_t *abi, abi_node_t *node)
{
    TRACEPNF(3, "\tFRWM\n");
    return 0;
}

static int msg_part_t(abi_context_t *abi, abi_node_t *node)
{
    TRACEPNF(3, "\tMSG\n");
    return 0;
}

static int device_name_t(void *priv, uint8_t *data, size_t size, size_t tail)
{
    abi_mem_flow_t	*f = priv;
    mcast_cl_api_t	*mcast_cl = f->ctx;
    mcast_cl_payload_t	*rx_vars = &mcast_cl->rx_vars;

    char		*src = f->data + f->offs;

    rx_vars->dev_name.size = size+tail;
    memcpy(rx_vars->dev_name.str, src, rx_vars->dev_name.size);
    rx_vars->dev_name.str[size+tail] = '\0';

    TRACEPNF(3, "\tP3_DEVICE_NAME_T [%s]\n", rx_vars->dev_name.str);
    
    f->offs += size + tail;

    return 0;
}

static int host_name_t(void *priv, uint8_t *data, size_t size, size_t tail)
{
    abi_mem_flow_t	*f = priv;
    mcast_cl_api_t	*mcast_cl = f->ctx;
    mcast_cl_payload_t	*rx_vars = &mcast_cl->rx_vars;

    char		*src = f->data + f->offs;

    memcpy(rx_vars->hostname_data, src, size+tail);
    rx_vars->hostname_data[size+tail] = '\0';

    TRACEPNF(3, "\tP3_HOST_NAME_T [%s]\n", rx_vars->hostname_data);

    f->offs += size + tail;
	
    return 0;
}

static int mcast_check_bin(void *priv, uint8_t *data, size_t size, size_t tail)
{
    abi_mem_flow_t	*f = priv;

    TRACEPNF(3, "CKb: [%3lu+%3lu]\n", (unsigned long)size, (unsigned long)tail);
    
    f->offs += size + tail;

    return 0;
}

#if 0
static int mcast_check_str(void *priv, uint8_t *data, size_t size, size_t tail)
{
    abi_mem_flow_t	*f = priv;
    char		*src = f->data + f->offs;
    //mcast_cl_api_t	*mcast_cl = f->ctx;

    char tmp[255] = { };
    memcpy(tmp, src, size);
    tmp[size] = '\0';

    //TRACE(3, "size [%d] tail [%d]\n", size, tail);
    //dump_hex(src, size);

    TRACEPNF(3, "\tCKs: [%3lu+%3lu] [%s]\n", (unsigned long)size, (unsigned long)tail, tmp);

    f->offs += size + tail;
	
    return 0;
}
#endif
static int abi_if_caps(abi_context_t *abi, abi_node_t *node)
{
    TRACEPNF(3, "\t\tP3_ABI_IF_CAPS\n");
    return 0;
}

static int abi_gid_t_in(abi_context_t *abi, abi_node_t *node)
{
    TRACEPNF(3, "\t\tP3_ABI_GID_T\n");
    return 0;
}

static int interface_n(abi_context_t *abi, abi_node_t *node)
{
	abi_mem_flow_t		*f		= abi->priv;
	mcast_cl_api_t		*mcast_cl	= f->ctx;
	mcast_cl_payload_t	*rx_vars	= &mcast_cl->rx_vars;
	mcast_cl_interface_t	*ifs		= &rx_vars->ifs;

	if(rx_vars->ifs_idx >= IFS_NUM_MCL) {
		TRACEPNF(0, "[WARN] too much interfaces on device (%d)\n", rx_vars->ifs_idx);
		rx_vars->ifs_idx++;
		return 0;
	} 

	if(ifs->cl_idx >= CL_NUM_MCL)
		ifs->cl_idx = CL_NUM_MCL;

	/* save current ifs & increase idx inside array */
	rx_vars->interface[rx_vars->ifs_idx] = *ifs;
	rx_vars->ifs_idx++;

	ifs->cl_idx = 0;

	TRACEPNF(3, "\tP3_INTERFACE_N\n");

	return 0;
}

static int server_param_t(abi_context_t *abi, abi_node_t *node)
{
    TRACEPNF(3, "\t\tP3_SERVER_PARAM_T\n");
    return 0;
}

static int client_n(abi_context_t *abi, abi_node_t *node)
{
	abi_mem_flow_t		*f		= abi->priv;
	mcast_cl_api_t		*mcast_cl	= f->ctx;
	mcast_cl_payload_t	*rx_vars	= &mcast_cl->rx_vars;
	mcast_cl_interface_t	*ifs		= &rx_vars->ifs;

	if(ifs->cl_idx >= CL_NUM_MCL) {
		TRACEPNF(0, "[WARN] too much client on interface (%d)\n", ifs->cl_idx);
		ifs->cl_idx++;

		return 0;
	} 

	/* save client */
	ifs->client[ifs->cl_idx] = rx_vars->cl_in;
	ifs->cl_idx++;

	TRACEPNF(3, "\t\tP3_CLIENT_N\n");

	return 0;
}

static int abi_gid_t_cl(abi_context_t *abi, abi_node_t *node)
{
    TRACEPNF(3, "\t\t\tP3_ABI_GID_T\n");
    return 0;
}

static int device_class(abi_context_t *abi, abi_node_t *node)
{
    abi_mem_flow_t	*f = abi->priv;
    mcast_cl_api_t	*mcast_cl = f->ctx;
    mcast_cl_payload_t	*rx_vars = &mcast_cl->rx_vars;
    
    rx_vars->dev_class = htonl(rx_vars->dev_class);

    switch(rx_vars->dev_class) {
    case 0: TRACEPNF(3, "\tDEVICE_CLASS_E [DEVICE_TYPE_PPC405GPr_VIDEO_BRIDGE / DEVICE_TYPE_VB]\n"); break;
    case 1: TRACEPNF(3, "\tDEVICE_CLASS_E [DEVICE_TYPE_PPC405GP_D16]\n"); break;
    case 2: TRACEPNF(3, "\tDEVICE_CLASS_E [DEVICE_TYPE_PPC405GPr_D8]\n"); break;

    case 3: TRACEPNF(3, "\tDEVICE_CLASS_E [DEVICE_TYPE_GUID_PROXY]\n"); break;
    case 4: TRACEPNF(3, "\tDEVICE_CLASS_E [DEVICE_TYPE_VB_ARCHIVE]\n"); break;
    case 5: TRACEPNF(3, "\tDEVICE_CLASS_E [DEVICE_TYPE_VERELAY]\n"); break;
    case 6: TRACEPNF(3, "\tDEVICE_CLASS_E [DEVICE_TYPE_NETCAM]\n"); break;

    default: TRACEPNF(3,"\tDEVICE_CLASS_E [UNKNOWN]\n"); break;
    }

    return 0;
}

static int component_e(
	abi_context_t *abi,
	abi_node_t *node
)
{
	abi_mem_flow_t		*f		= abi->priv;
	mcast_cl_api_t		*mcast_cl	= f->ctx;
	mcast_cl_payload_t	*rx_vars	= &mcast_cl->rx_vars;
	mcast_cl_interface_t	*ifs		= &rx_vars->ifs;

	static char *p3_components[] = {
	    "[P3_MDM_DATA]",
	    "[P3_MDM_CMD]",
	    "[P3_MDM_BOTH]",
	    "[P3_DEVICE_DATA]",
	    "[P3_DEVICE_CMD]",
	    "[P3_DEVICE_BOTH]",
	    "[P3_CM]"
	};
	
	ifs->type = htonl(ifs->type);

	if(ifs->type < ARRAY_ELEMS(p3_components))
		TRACEPNF(3,"\t\tP3_COMPONENT_E [%s]\n", p3_components[ifs->type]);
	else
		TRACEPNF(3,"\t\tP3_COMPONENT_E [%08x]\n", ifs->type);

	return 0;
}

static int conn_ipv4_t_in(abi_context_t *abi, abi_node_t *node)
{
	abi_mem_flow_t		*f		= abi->priv;
	mcast_cl_api_t		*mcast_cl	= f->ctx;
	mcast_cl_payload_t	*rx_vars	= &mcast_cl->rx_vars;
	mcast_cl_interface_t	*ifs		= &rx_vars->ifs;
    
	ifs->conn_in.port = ntohs(ifs->conn_in.port);

	TRACEPNF(3, "\t\tP3_CONN_IPV4_T [%s:%d]\n",
	    inet_ntoa(*(struct in_addr*)&ifs->conn_in.ip),
	    ifs->conn_in.port
	);

	return 0;
}

static int conn_ipv4_t_cl(abi_context_t *abi, abi_node_t *node)
{
	abi_mem_flow_t		*f	 	= abi->priv;
	mcast_cl_api_t		*mcast_cl	= f->ctx;
	mcast_cl_payload_t	*rx_vars 	= &mcast_cl->rx_vars;
	mcast_cl_client_t	*cl	 	= &rx_vars->cl_in;
    
	cl->conn_cl.port = ntohs(cl->conn_cl.port);

	TRACEPNF(3, "\t\t\tP3_CONN_IPV4_T [%s:%d]\n",
	    inet_ntoa(*(struct in_addr*)&cl->conn_cl.ip),
	    cl->conn_cl.port
	);

	return 0;
}

/**
 *
\*/
/*\
 *  TYPE device_class_e = uint32_t;
 *  TYPE device_ltype_e = uint32_t;
 *  TYPE p3_component_e = uint32_t;
 *
 *  [CONFIG_ABI_STAT_RESP] {
 *      [P3_MSG_PART_T]     REQUIRED (p3_msg_part_t);
 *      [DEVICE_CLASS_E]    REQUIRED (device_class_e);
 *      [P3_HOST_ID_T]      REQUIRED (blob_t);
 *      [P3_MAC_T]          REQUIRED (blob_t);
 *      [P3_SYSTEM_NAME_T]  OPTIONAL (blob_t);
 *      [P3_HOST_NAME_T]    OPTIONAL (blob_t);
 *      [P3_DEVICE_NAME_T]  OPTIONAL (blob_t);
 *	[P3_DEVICE_SUB_MODEL_T] OPTIONAL (uint32_t);
 *	[P3_FIRMWARE_REV_T] OPTIONAL (p3_fmw_rev_t);
 *
 *      [P3_INTERFACE_N] ?OPTIONAL? MULTIPLE {
 *          [P3_ABI_GID_T]          REQUIRED MULTIPLE
 *      			    (p3_id_t);
 *          [P3_COMPONENT_E]        REQUIRED MULTIPLE (p3_component_e);
 *          [P3_CONN_IPV4_T]        OPTIONAL	      (p3_conn_ipv4_t);
 *          [P3_CONN_IPV6_T]        OPTIONAL          (p3_conn_ipv6_t);
 *          [P3_SERVER_PARAM_T]     REQUIRED ARRAY    (p3_load_param_t);
 *          [DEVICE_CAPS_E]         OPTIONAL ARRAY    (device_ltype_e);
 *
 *          [P3_CLIENT_N]  OPTIONAL MULTIPLE {
 *      	[P3_CLIENT_ID_T]    REQUIRED (p3_client_id_t);
 *      	[P3_ABI_GID_T]      OPTIONAL (p3_id_t);
 *      	[P3_CONN_IPV4_T]    OPTIONAL (p3_conn_ipv4_t);
 *      	[P3_CONN_IPV6_T]    OPTIONAL (p3_conn_ipv6_t);
 *      	[P3_CLIENT_PARAM_T] OPTIONAL ARRAY (p3_load_param_t);
 *          };
 *      };
 *  };
 *
\*/

static void mcast_cl_abir_init(mcast_cl_api_t *mcast_cl)
{
	mcast_cl->rx_flow.ctx = mcast_cl;		/* save ctx */
	mcast_cl->rx_flow.name = trace_prefix;
	mcast_cl->rx_abi = abir_open(NULL,
		&mcast_cl->rx_flow,
		abi_mem_reader,
		abi_mem_skiperq,
		22
	);

	mcast_cl_payload_t		*rx_vars = &mcast_cl->rx_vars;
	mcast_cl_interface_t		*ifs	 = &rx_vars->ifs;
	mcast_cl_client_t		*cl	 = &rx_vars->cl_in;
	
	abi_context_t			*abi	 = mcast_cl->rx_abi;
	abi_node_t			tmp	 = { };
	int				res, res0, res1;

	ABI_SET_KEY32(tmp, CONFIG_ABI_STAT_RESP);
	ABI_SET_ACTION(tmp, config_abi_resp);
	res = abi_append_node(abi, &tmp);
       
	ABI_SET_KEYA_VAR(tmp, P3_DEVICE_SUB_MODEL_T, rx_vars->sub_model);
	ABI_SET_ACTION(tmp, device_sub_model_t);
	res = abi_append_subnode(abi, res, &tmp);

	mcast_cl->top_level = res;

	ABI_SET_KEY32VAL(tmp, P3_HOST_NAME_T, &rx_vars->hostname);
	ABI_SET_READER(tmp, host_name_t);
	abi_append_node(abi, &tmp);

	ABI_SET_KEY32VALPS(tmp, P3_DEVICE_NAME_T, rx_vars->dev_name.str, rx_vars->dev_name.size);
	ABI_SET_READER(tmp, device_name_t);
	abi_append_node(abi, &tmp);
	    
	ABI_SET_KEY32_VAR(tmp, P3_HOST_ID_T, rx_vars->cpu_id);
	//ABI_SET_READER(tmp, mcast_check_bin);
	ABI_SET_ACTION(tmp, host_id_t);
	abi_append_node(abi, &tmp);

	ABI_SET_KEY32_VAR(tmp, P3_MAC_T, rx_vars->mac);
	ABI_SET_READER(tmp, mcast_check_bin);
	abi_append_node(abi, &tmp);

	ABI_SET_KEY32_VAR(tmp, P3_ABI_ASYNC_CONTEXT_T, rx_vars->ctx);
	ABI_SET_ACTION(tmp, abi_async_context_t);
	abi_append_node(abi, &tmp);

	ABI_SET_KEYA_VAR(tmp, P3_FIRMWARE_REV_T, rx_vars->fm_rev);
	ABI_SET_ACTION(tmp, firmware_rev_t);
	abi_append_node(abi, &tmp);
	
	ABI_SET_KEY32_VAR(tmp, P3_MSG_PART_T, rx_vars->msg_id);
	ABI_SET_ACTION(tmp, msg_part_t);
	abi_append_node(abi, &tmp);

	ABI_SET_KEY32_VAR(tmp, DEVICE_CLASS_E, rx_vars->dev_class);
	ABI_SET_ACTION(tmp, device_class);
	abi_append_node(abi, &tmp);

	/********************************************/   
	/*          P3_INTERFACE_N     		    */
	/********************************************/   
	ABI_SET_KEY32(tmp, P3_INTERFACE_N);
	ABI_SET_ACTION(tmp, interface_n);
	res0 = abi_append_node(abi, &tmp);

	ABI_SET_KEY32_VAR(tmp, P3_COMPONENT_E, ifs->type);
	ABI_SET_ACTION(tmp, component_e);
	res0 = abi_append_subnode(abi, res0, &tmp);

	mcast_cl->interface_level = res0;

	ABI_SET_KEY32_VAR(tmp, P3_SERVER_PARAM_T, ifs->server_param);
	ABI_SET_ACTION(tmp, server_param_t);
	abi_append_node(abi, &tmp);
	
	ABI_SET_KEY32_VAR(tmp, P3_ABI_GID_T, ifs->gid_in);
	ABI_SET_ACTION(tmp, abi_gid_t_in);
	abi_append_node(abi, &tmp);
	
	ABI_SET_KEY32_VAR(tmp, P3_CONN_IPV4_T, ifs->conn_in);
	ABI_SET_ACTION(tmp, conn_ipv4_t_in);
	abi_append_node(abi, &tmp);
	    
	ABI_SET_KEYA_VAR(tmp, P3_ABI_IF_CAPS, ifs->caps);
	ABI_SET_ACTION(tmp, abi_if_caps);
	abi_append_node(abi, &tmp);

	/********************************************/   
	/*          P3_CLIENT_N     		    */
	/********************************************/   
	ABI_SET_KEY32(tmp, P3_CLIENT_N);
	ABI_SET_ACTION(tmp, client_n);
	res1 = abi_append_node(abi, &tmp);

	ABI_SET_KEY32_VAR(tmp, P3_CLIENT_ID_T, cl->cdescr);
	res1 = abi_append_subnode(abi, res1, &tmp);

	mcast_cl->client_level = res1;

	ABI_SET_KEY32_VAR(tmp, P3_CONN_IPV4_T, cl->conn_cl);
	ABI_SET_ACTION(tmp, conn_ipv4_t_cl);
	abi_append_node(abi, &tmp);

	ABI_SET_KEY32_VAR(tmp, P3_ABI_GID_T, cl->gid_cl);
	ABI_SET_ACTION(tmp, abi_gid_t_cl);
	abi_append_node(abi, &tmp);

	/* add client custom keys */
	if(mcast_cl->abir_custom)
		mcast_cl->abir_custom(mcast_cl);

	TRACEF(9, abir_dump(abi));
}

static int mcast_cl_read(mcast_cl_api_t *mcast_cl)
{
	ssize_t		res;
	abi_mem_flow_t	*f	= &mcast_cl->rx_flow;
	socklen_t	slen	= sizeof(mcast_cl->ipv4_dst);

	res = recvfrom(
		mcast_cl->pfds[0].fd,
		    mcast_cl->rx_buf,
		    mcast_cl->rx_buf_size,
		    0,						/* flags */
		    (struct sockaddr*)&mcast_cl->ipv4_dst,
		    &slen
		);

	if(res < 0) {
		ETRACEP("Error when mcast data receiving. errno ");
		return -1;
	}
       
	TRACEPNF(3, "[%s] recvfrom() = %zd need = %zd\n", __func__, res, mcast_cl->rx_buf_size);

	/* Init input flow  */
	f->data     = mcast_cl->rx_buf;
	f->max_size = (long unsigned int)res;
	f->offs     = 0;
	f->status   = 0;

	TRACEPNF(99, "OOB data received from %s:%d [%lu] bytes\n",
	    inet_ntoa(mcast_cl->ipv4_dst.sin_addr),
	    htons(mcast_cl->ipv4_dst.sin_port),
	    f->max_size
	);

	/* clean current vars */
	memset(&mcast_cl->rx_vars, 0, sizeof(mcast_cl->rx_vars));

	 /* extract data from ABI */
	res = abir_read(mcast_cl->rx_abi, 0);

	return 0;
}

int mcast_cl_init(mcast_cl_api_t *mcast_cl) 
{
    int		s, res;

    if(mcast_cl->rx_buf_size == 0)
	mcast_cl->rx_buf_size = 4096 ;

    if(mcast_cl->rx_buf != NULL) {
	TRACEP(0, "You cannot init mcast client again until you release it\n");
    }

    mcast_cl->rx_buf = malloc(mcast_cl->rx_buf_size);

    if(mcast_cl->rx_buf == NULL) {
	ETRACEP("Error. Cannot allocate RX buffer. errno ");
	exit(-1);
    }

    /* setup socket */
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (s == -1) {
	    ETRACE("socket creation error");
	    exit(-1);
    }

    if(mcast_cl->mcast_port == 0)
	mcast_cl->mcast_port = 4521;

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);			/* Listen on any addr */
    // sin.sin_addr.s_addr = inet_addr(mcast_cl->mcast_addr);
    sin.sin_port = htons(mcast_cl->mcast_port);

    if( (bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0 ) {
	ETRACEP("cannot bind() multicast socket. errno ");
	exit(-1);
    }

    struct ip_mreq	mreq = { };

    /* setup multicast listening */
    mreq.imr_multiaddr.s_addr = inet_addr(mcast_cl->mcast_addr);
    mreq.imr_interface.s_addr   = htonl(INADDR_ANY);

    /* become member of multicast group for receiving packets */
    res = setsockopt (s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));

    if(res == -1) {
	    ETRACEP("add into mcast group. errno ");
	    exit(-1);
    }
	
    unsigned char on_uc = 0;

    /* disable loopback to self */
    res = setsockopt (s, IPPROTO_IP, IP_MULTICAST_LOOP, 
		(void *) &on_uc, (socklen_t)sizeof (on_uc));

    if(res == -1) {
	    ETRACEP("disable mcast loopback");
    }

    mcast_cl->pfds[0].fd = s;
    mcast_cl->pfds[0].events = (POLLIN);

    mcast_cl_abir_init(mcast_cl);

    TRACEPNF(0, "MCAST init succsefully\n");

    return 0;
}

void mcast_cl_destroy(mcast_cl_api_t *mcast_cl)
{
    /* RX part */
    free(mcast_cl->rx_buf);
    mcast_cl->rx_buf = NULL;
    
    abi_close(mcast_cl->rx_abi);

    struct ip_mreq	mreq = { };
    mreq.imr_multiaddr.s_addr = inet_addr(mcast_cl->mcast_addr);
    mreq.imr_interface.s_addr   = htonl(INADDR_ANY);
    setsockopt (mcast_cl->pfds[0].fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));

    close(mcast_cl->pfds[0].fd);
    
    /* destroy client data */
    if(mcast_cl->release)
	mcast_cl->release(mcast_cl);
    
    TRACEPNF(0, "MCAST release all buffers\n");
}

void mcast_cl_do_request_stub(mcast_cl_api_t *mcast_cl)
{

}
_weak_alias(mcast_cl_do_request_stub, mcast_cl_do_request);

void *process_cl_mcast(void *priv)
{
	static int res;
	mcast_cl_api_t *mcast_cl = priv;

	if(!mcast_cl->no_sig_exit) {
		static int	dummy;
		mcast_cl->no_sig_exit = &dummy;
	}

	mcast_cl->local_exit = 1;

	mcast_cl_do_request(mcast_cl);

	do {
	    res = poll(mcast_cl->pfds, 1, 1000);

	    if( res < 0 ) {
		if( errno == EINTR )
		    continue;
	    
		ETRACEP("Error during poll. errno ");
		exit(-1);
	    }

	    if(!res)		/* timeout */
		continue;
	    
	    if(mcast_cl->pfds[0].revents & POLLIN)
		mcast_cl_read(mcast_cl);

	} while(mcast_cl->local_exit && *mcast_cl->no_sig_exit);

	res = 0;
	return &res;
}

#if 0
int main(void)
{
    I = stdout;

    mcast_cl_api_t mcast_cl = {
		.mcast_addr = "224.0.1.20",
		.mcast_port = 4521
    		};

    int res = mcast_cl_init(&mcast_cl);
    if( res < 0 ) {
	TRACE(0, "OOpps \n");
    }

    process_cl_mcast(&mcast_cl);

    return 0;
}
#endif
