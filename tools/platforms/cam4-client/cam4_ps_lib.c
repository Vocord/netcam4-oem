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
#include <sys/time.h>
#define __USE_XOPEN
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <asm/types.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

#define TRACE_PRIVATE_PREFIX    1
#include <trace.h>
#undef  TRACE_LEVEL
#define TRACE_LEVEL 1

#include <abi/p3.h>
#include <abi/p3_helpers.h>
#include <abi/io.h>
#include <abi/device.h>
#include <api/mcast-cl.h>
#include <os-helpers/net-sockets.h>
#include <os-helpers/pthreads.h>



#include "cam4_ps.h"

int own_cb(cam4_rd_t *rd)
{
	printf("have frame!\n");

	return 0;
}

int main(void) 
{
	int err;

	char *argv[10] = {"cam4_ps", "-m", "2", "-v", "192.168.5.226", "-d", "192.168.0.50", "-M", "-C", "3" };
	int argc = 10;

	cam4_ps_set_cb(own_cb);
	
	err = cam4_ps_start(argc, argv);

	return 0;
}
