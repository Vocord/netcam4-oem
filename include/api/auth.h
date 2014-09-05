#ifndef __API_AUTH_H__
#define __API_AUTH_H__
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
 *   Description:
 *     Contains declarations of common API of P3SS Auth Component
 *
\*/
#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_ISOLATED)
#include <isolated/abi/p3.h>
#include <isolated/abi/io.h>
#include <isolated/abi/auth.h>
#else
#include <abi/p3.h>
#include <abi/io.h>
#include <abi/auth.h>
#endif

/*  */
/* Объявления макро констант */
#define CHALENGE_MAX	(32)
#define PASSWORD_MAX	(32)

/* Объявления констант */
typedef enum {
	P3_AUTH_DONE_POSITIVE	= (1U<<0),
	P3_AUTH_DONE_NEGATIVE	= (1U<<1),
	P3_AUTH_INFO_PRESENT	= (1U<<2),
	P3_AUTH_HASH_PRESENT	= (1U<<3),
} p3_auth_info_e;


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
struct p3_auth_info_s;
typedef struct {
	filetime	ts;				/* timestamp BE */

	uint8_t		chalenge_val[CHALENGE_MAX];	/* chalenge */
	uint8_t		password_val[PASSWORD_MAX];	/* password */
	uint8_t		user_val[128];			/* username */
	uint8_t		hash_val[128];			/* hash result */

	data_t		chalenge;
	data_t		password;
	data_t		hash;
	data_t		user;

	uint32_t	mode_rx;		/*+ AUTH_CAPS sec mode rx */
	uint32_t	mode_tx;		/*+ AUTH_CAPS sec mode tx */
} p3_auth_ctx_t;

typedef int	auth_info_f(struct p3_auth_info_s *);
typedef int	auth_get_pwd_f(
	struct p3_auth_info_s	*,
       	data_t			*pwd,
       	const data_t		*user,
       	const data_t		*domain
);

typedef struct p3_auth_info_s {
	p3_auth_ctx_t	A;		/* Alice TxRequest RxResposce */
	p3_auth_ctx_t	B;		/* Bob   RxRequest TxResponse */

	uint8_t		chalenge_val[CHALENGE_MAX];	/* for TxResponse */
	uint8_t		temp_val[128];

	data_t		chalenge;	/* for TxResponse */
	
	auth_get_pwd_f	*get_password;	/* get password by uname */
	auth_info_f	*get_response_account;	/* get pwd and user for auth response */
	auth_info_f	*get_request_account;	/* get user for auth request */

	char		*hash;		/* Alice hash */

	uint32_t	flags;		/* auth mode */
	uint32_t	state;
	uint32_t	status;

	filetime	ts;		/* current timestamp BE */

	abi_context_t	*abi;

	abi_mem_flow_t	flow_rx;
	abi_mem_flow_t	ainfo_flow;
	abi_io_task_t	task_i;		/* "/AUTH Info" for/from AUTH Response */

	abi_io_task_t	*task_tx;	/* for TX data */
	abi_context_t	*abi_rx;	/* abi for parsing RX data */

	abi_io_tasks_t	*io_task;	/* I/O task */
	void		*priv;		/* virtualziation context */
} p3_auth_info_t;

typedef struct p3_auth_cb_s {
	auth_get_pwd_f	*get_password;		/* get password by uname */
	auth_info_f	*get_response_account;	/* get pwd and user for auth response */
	auth_info_f	*get_request_account;	/* get user for auth request */
} p3_auth_cb_t;

static inline void auth_set_get_password(
	p3_auth_info_t	*ainfo,
	auth_get_pwd_f	*get_password
)
{
	ainfo->get_password = get_password;
}

static inline void auth_set_get_request_account(
	p3_auth_info_t	*ainfo,
	auth_info_f	*get_request_account
)
{
	ainfo->get_request_account = get_request_account;
}

static inline void auth_set_get_response_account(
	p3_auth_info_t	*ainfo,
	auth_info_f	*get_response_account
)
{
	ainfo->get_response_account = get_response_account;
}

/* Определения функций */
extern int abi_fill_chalenge(data_t *val);

extern int auth_fill_caps(p3_auth_info_t *ainfo);
extern int auth_fill_request(p3_auth_info_t *ainfo);
extern int auth_fill_response(p3_auth_info_t *ainfo);
extern int auth_fill_status(p3_auth_info_t *ainfo);

/* process incoming data functions */
extern int auth_process(p3_auth_info_t *ainfo, abi_mem_flow_t *flow);

/* Initialization/Release funcs */
extern int auth_abir_init(p3_auth_info_t *ainfo);
extern int auth_init(
	p3_auth_info_t	*ainfo,
       	abi_io_task_t	*task_tx,
	p3_auth_cb_t	*ifc
);
extern int auth_release(p3_auth_info_t *ainfo);

extern int task_auth_init(abi_io_tasks_t *task, uint32_t flags);
extern int task_auth_release(abi_io_tasks_t *task, uint32_t flags);


/* inlines with ext references */
static inline int ioc_auth_init(
	io_ctrl_t	*ioc,
       	unsigned	flags,
	p3_auth_cb_t	*ops
)
{
	ioc->auth_flags = flags;

	if(!(flags & P3_AUTH_MODE_SESSIONS))
		return 0;

	ioc->auth_init		= &task_auth_init;
	ioc->auth_release	= &task_auth_release;

	ioc->auth_ifc		= ops;
	
	return 0;
}

static inline int ioc_auth_rx(
	io_ctrl_t	*ioc,
       	const char	*name
)
{
	if(ioc->rx_task && (ioc->rx_task->flags & ABI_IO_AUTHA_REQUIRED)) {
		int res;

	 	p3_auth_info_t *ainfo = ioc->rx_task->auth;

		abi_mem_flow_t	*f = &ainfo->flow_rx;

		f->data     = ioc->rx_flow.data;
		f->max_size = ioc->rx_flow.max_size;
		f->offs     = 0;
		f->status   = 0;
		f->name	    = name;
		
		res = abir_read(ainfo->abi_rx, 0);		/* parse data */
		if(res) {
			TRACEPNF(0, "%s abir_read res: %d errno: %d\n",
			   ioc->rx_flow.name, res, errno
			);

			return -errno;
		}

		abi_mem2fd_ioc_send(ioc->rx_task, NULL, 0ul);

		if(ainfo->state & P3_AUTH_DONE_NEGATIVE)
			abi_kill_cur_task(ioc);

		if(ainfo->state & P3_AUTH_DONE_POSITIVE)
			ioc->rx_task->expires = 0;

		return 0;
	} else {
		return 1;
	}
}

extern p3_auth_cb_t	auth_ops;
extern void auth_set_client_credentionals(char *user, char *pass);

#ifdef __cplusplus
}
#endif

#endif
