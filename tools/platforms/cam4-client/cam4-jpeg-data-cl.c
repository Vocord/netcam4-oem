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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define TRACE_PRIVATE_PREFIX    1
#include <trace.h>
#undef  TRACE_LEVEL
#define TRACE_LEVEL 0 

#include <abi/p3.h>
#include <abi/p3_helpers.h>
#include <abi/io.h>
#include <abi/device.h>
#include <api/mcast-cl.h>
#include <os-helpers/net-sockets.h>
#include <os-helpers/pthreads.h>

#include "cam4-cmd-cl.h"

FILE *I;

static char* trace_prefix = "cam4-jpeg-data-cl: ";

volatile int	no_sig_exit = 1;

enum state_flags {
    f_flag = 1<<0,
    n_flag = 1<<1,
    s_flag = 1<<2,

    m1_flag = 1<<3,			/* CPU ID */
    m2_flag = 1<<4,			/* IP address */

    g1_flag = 1<<5,			/* stream on */
    g2_flag = 1<<6,			/* stream off */

    frame_done_flag = 1<<14
};

typedef struct cam4_jpeg_cl_s {
	int			start_mode;
	int			flow;

	mcast_cl_api_t		mcast_cl;
	pthread_t		mcast_cl_thread;

	cam4_cmd_cl_t		cam4_cl;	 /* cam4 cmd context */
	mcast_cl_interface_t	ifs;
} cam4_jpeg_cl_t;

void sigproc(int sig)
{	
	no_sig_exit = 0;
}


static void show_the_banner(void)
{
    TRACE(0,	"Client for JPEG stream from CAM4 by Vocord\n"
		    "Usage: sudo ./cam4_ps [OPTIONS]\n"
		    "Output format:\n"
		    "\t-g				just start/stop stream\n"
		    "\t\t 1				start\n"
		    "\t\t 2				stop\n"
		    "Find device option:\n"
		    "\t-m				work with -v (value) flag\n"
		    "\t\t 1				CPU ID\n"
		    "\t\t 2				IP address\n"
		    "\t-v					value (IP addres, Host ID, etc). look at -m option\n"
		    "\t-f flow				1 - RAW(ip), 2- JPEG(tcp) 4- FACES(ip) 5- FACES(tcp)\n"
		    "\t-w 1				Do dump incoming data"
		    "\t-h				this banner\n"
    );
};

static int cam4_mcast(cam4_jpeg_cl_t *cam4)
{
    int			res;
    void		*thread_exit = 0;
    mcast_cl_api_t	*mcast_cl = &cam4->mcast_cl;

    TRACEPNF(0, "[%s]\n", __func__);

     if( (res = mcast_cl_init(mcast_cl)) < 0)
	 return -1;

   /* get the actual information */
    CREATE_THREAD(res, process_cl_mcast, mcast_cl, cam4->mcast_cl_thread);

    THREAD_JOIN(res, cam4->mcast_cl_thread, thread_exit)

    /* clear buffers */
    mcast_cl_destroy(mcast_cl);

    return res;
}

static int jpeg_on(cam4_jpeg_cl_t *cam4_rd, mcast_cl_interface_t *ifs)
{
    cam4_cmd_cl_t	*cam4_cl = &cam4_rd->cam4_cl;
    p3_conn_ipv4_t	*conn_mcast = &ifs->conn_in;
    p3_conn_ipv4_t	*conn_raw = &cam4_cl->conn_ipv4;

    /* copy parameters from mcast response */
    *conn_raw = *conn_mcast;

    return(cam4_cmd_cl_process(cam4_cl, 0 /* start */, cam4_rd->flow, 1));
}

int find_ifs(mcast_cl_api_t *mcast_cl)
{
    static int		fake_exit = 0;
    int			i, res;
    mcast_cl_payload_t	*rx_vars = &mcast_cl->rx_vars;
    cam4_jpeg_cl_t 	*cam4 = (cam4_jpeg_cl_t *)mcast_cl->priv;

    res = memcmp(&mcast_cl->ipv4_device.sin_addr, &mcast_cl->ipv4_dst.sin_addr, sizeof(typeof(mcast_cl->ipv4_dst.sin_addr)));
    if( (mcast_cl->cpu_id != rx_vars->cpu_id) && (res != 0) )
	return -1;

    /* find properly ifs */
    for(i = 1; i < rx_vars->ifs_idx; i++ ) {
    	mcast_cl_interface_t *ifs = &rx_vars->interface[i];

	if( ifs->type == P3_DEVICE_BOTH ) {
	    TRACEPNF(0, "Found P3_DEV component\n");
	    cam4->ifs = *ifs;
	    mcast_cl->no_sig_exit = &fake_exit;

	    return i;
	}

    } // for(i = 1; i < IFS_NUM_CL; i++ )

    TRACEPNF(0, "[ERR] Cannot find P3_CM interface\n");

    return -1;
}

int main (int argc, char **argv) 
{
	int 		i, k;
	char		val_str[255] = "";

	cam4_jpeg_cl_t	cam4 = {
		.mcast_cl	= {
			.no_sig_exit	= &no_sig_exit,
			.mcast_addr	= "224.0.1.20",
			.mcast_port	= 4521,
			.priv	   	= &cam4,
			.find_dev	= &find_ifs,
		},

		.cam4_cl	= {
			.wmode		= 0,
		},

		.flow		= CAM4_JPEG_IFS,
		.cam4_cl	= {
		    .no_sig_exit	= &no_sig_exit,
		},
	};

	I = stdout ;

	signal(SIGINT, sigproc);

	/* FIXME - add bayer phase */
	/* parse parameters */	
	while ((i = getopt(argc, argv, "w:d:f:g:hm:n:sv:")) != -1) {

	    TRACE(3, "mode %c\n", i);

	    switch (i) {
	    case 'g':
	    	/* file presented */
		k = atoi(optarg);
		switch(k) {
		    case 1: cam4.start_mode |= g1_flag; break;
		    case 2: cam4.start_mode |= g2_flag; break;
		    default: TRACEP(0, "=========== [ERR] unknow -m option.\n"); show_the_banner(); return -1;
		}
		break;

	    case 'w':
	    	/* write mode */
		cam4.cam4_cl.wmode = atoi(optarg);
		break;

	    case 'f':
	    	/* flow */
		cam4.flow = atoi(optarg);
		break;

	    case 'm':
	    	/* file presented */
		k = atoi(optarg);
		switch(k) {
		    case 1: cam4.start_mode |= m1_flag; break;
		    case 2: cam4.start_mode |= m2_flag; break;
		    default: TRACEP(0, "=========== [ERR] unknow -g option.\n"); show_the_banner(); return -1;
		}
		break;

	    case 'v':
	    	/* value for search */
		strncpy(val_str, optarg, sizeof(val_str));
		break;

	    case 'h':
	    default:
	    	show_the_banner();
		return 0;
	    }
	};
	
	/* check masks */
	if( cam4.start_mode & m1_flag) {
	    k = sscanf(val_str, "%"PRIx64, &cam4.mcast_cl.cpu_id);

	    if(k != 1) { 
		TRACEP(0, "=========== [ERR] Wrong -v option.\n");
		show_the_banner();
		return -1;
	    }
	} else if (cam4.start_mode & m2_flag) {
	    k = inet_aton(val_str, &cam4.mcast_cl.ipv4_device.sin_addr);

	    if(k == 0) { 
		TRACEP(0, "=========== [ERR] Wrong -v option.\n");
		show_the_banner();
		return -1;
	    }

	};

	/* MCAST prepare */
	i = cam4_mcast(&cam4);
	if( i < 0 )
	    return -1;

	if(cam4.ifs.type == P3_DEVICE_BOTH)
		jpeg_on(&cam4, &cam4.ifs);

	return 0;
};
