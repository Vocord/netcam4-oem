#ifndef __API_CM_ACA_H__
#define __API_CM_ACA_H__
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

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_ISOLATED)
#include <isolated/abi/p3.h>
#include <isolated/abi/cm.h>
#else
#include <abi/p3.h>
#include <abi/cm.h>
#endif

/*  */
/* Constants declarations */


/* Data types  declarations */
#if defined(_MSC_VER) && !defined(__GNUC__)
#include <pshpack1.h>
#endif

/*\
 *
\*/

typedef struct {
	p3_conn_ipv4_t		conn;
	p3_client_id_t		cdescr;
	int			id;
} mcast_cl_descr_t;

typedef struct {
	uint32_t		type;	/*  */
	p3_conn_ipv4_t		conn;
	p3_id_t			id;

	uint32_t		caps;

	p3_load_param_t		cl_load;
	p3_load_param_t		cl_load_be;

	mcast_cl_descr_t	*cls;

	void			*m;	/* mcast_ctx_t */
	int			idx;	/* entry index */
	int			cid;	/* last client id */
} mcast_if_descr_t;

/* Functions declarations */
extern int  mcast_add_if(void *, mcast_if_descr_t *,
	int		 port,
       	uint32_t	 type,
       	uint32_t	 max
);
extern int  mcast_del_if(mcast_if_descr_t*);

extern int  mcast_add_cl(mcast_if_descr_t*, int s);
extern int  mcast_del_cl(mcast_if_descr_t*, int s);
extern int  mcast_add_cl_ip(mcast_if_descr_t*, int id, uint32_t addr, unsigned port, unsigned prot);
#ifdef __cplusplus
}
#endif

#endif
