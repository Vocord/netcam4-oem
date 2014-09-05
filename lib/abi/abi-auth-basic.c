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
#define _GNU_SOURCE
#include <inttypes.h>
#include <errno.h>

#include <compiler.h>

#define TRACE_PRIVATE_PREFIX	1
#define TRACE_LEVEL		3
#include <trace.h>

#include <abi/io.h>
#include <api/auth.h>
#include <api/cm-auth.h>

typedef struct {
	uint8_t			password[64];
} temp_pwd_t;

static char			*trace_prefix	= "AUTH-CB: ";

static temp_pwd_t		accounts[16]	= { };
static int			aidx		= 0;

/* stubs */

/** Fill Password from [username] and [domain]
 *
 *  /returns zero on success
 */
static int auth_get_password_stub(
	struct p3_auth_info_s	*auth,
       	data_t			*pwd,
       	const data_t		*user,
       	const data_t		*domain
)
{
	volatile admin_auth_rtd_t	*rtd = admin_auth_get_rtd();

	if(!pwd)
		return -EINVAL;

	int			i;

	admin_auth_lock(ADMIN_AUTH_SEM_RDONLY);

	temp_pwd_t		*acopy = accounts + aidx;
	for(i=0; i<rtd->total; i++) {
		p3_volatile_cast_t	c = { .vv = rtd->user + i };
		lb_account_t		*account = c.pv;

		if(account->acl & P3_AUTH_ACL_NONE)
			continue;

		if(account->size > sizeof(account->hash))
			continue;

		if(account->login.size > sizeof(account->login.str))
			continue;

		if(account->login.size != user->size)
			continue;

		if(user->size && bcmp(account->login.str, user->data, user->size))
			continue;

		/* FIXME TODO */
		memcpy(acopy->password, account->hash, account->size);
		aidx = (aidx + 1) & ((int)ARRAY_ELEMS(accounts) - 1);

		pwd->data = &acopy->password[0];
		pwd->size = account->size;

		admin_auth_unlock();

		return 0;
	}

	admin_auth_unlock();

	pwd->data = NULL;
	pwd->size = 0;

//	static uint8_t	def_pwd[1] = "1";
//	pwd->data = def_pwd;
//	pwd->size = sizeof(def_pwd);

	return 1;
}

/*
 * Fill B.Username And B.Password
 */
static int auth_get_response_account_stub(
	p3_auth_info_t	*ainfo
)
{
	p3_auth_ctx_t *B = &ainfo->B;

	B->password = (data_t){ 1, { .data = (uint8_t*)"1" } };     /* requested password */
	B->user     = (data_t){ 5, { .data = (uint8_t*)"admin"} }; /* requested user     */

	return 0;
}

/*
 * Fill A.Username And A.Password for AUTH_REQUEST(USER).
 */
static int auth_get_request_account_stub(
	p3_auth_info_t	*ainfo
)
{
	p3_auth_ctx_t *A = &ainfo->A;

	A->password = (data_t){ 0, { .data = (uint8_t*)A->password_val } }; /* requested password */
	A->user     = (data_t){ 0, { .data = (uint8_t*)A->user_val     } }; /* requested user     */

	return 0;
}

p3_auth_cb_t	auth_ops = {
	.get_password		= &auth_get_password_stub,
	.get_response_account	= &auth_get_response_account_stub,
	.get_request_account	= &auth_get_request_account_stub,
};
