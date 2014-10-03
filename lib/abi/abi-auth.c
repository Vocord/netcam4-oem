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
#include <stdlib.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef TRACE_LEVEL
#define TRACE_LEVEL		0
#endif
#define TRACE_PRIVATE_PREFIX	1
#include <trace.h>

#include <abi/io.h>
#include <abi/p3_helpers.h>
#include <api/auth.h>

#include <crypto/md5.h>

static char *trace_prefix = "AUTH: ";
/** Fill chalenge container with random data
 *
 *  /para val data container descriptor
 *  /returns non zero if failed 
 *  /returns zero on success
 */
int abi_fill_chalenge(
	data_t		*val
)
{
	uint8_t		*data = val->data;
	size_t		len   = val->size;

	int fd=open("/dev/urandom", O_RDONLY);
	if(fd>=0) {
		size_t res = (size_t)read(fd, data, len);
		close(fd);

		if(res == len)
			return 0;
	}

	srandom((unsigned int)time(NULL));
	while(len) {
		len--;
		data[len] = (uint8_t)(random()>>24);
	}
	
	return 0;
}

/** Insert AUTH INFO nodes in abi packet
 *
 *  /para abi	abi context
 *  /para pos	abi node pos to insert AUTH INFO nodes
 *  /para ainfo	auth context
 *  /returns last abi node pos
 */
static int abi_auth_info_make_abi(
	abi_context_t	*abi,
	int		pos,
	p3_auth_info_t	*ainfo
)
{
	abi_node_t	tmp = { };

	ABI_SET_KEYA_VAL(tmp, P3_USERNAME_T, ainfo->B.user);
	if(pos) {
		abi_append_subnode(abi, pos, &tmp);
	} else {
		abi_append_node(abi, &tmp);
	}

	ABI_SET_KEYA_VAR(tmp, P3_TS2_T, ainfo->ts);
	abi_append_node(abi, &tmp);

	ABI_SET_KEYA_VAL(tmp, P3_CHALLENGE2_T, ainfo->chalenge);
	abi_append_node(abi, &tmp);

	ABI_SET_KEYA_VAR(tmp, P3_TS_T, ainfo->B.ts);
	abi_append_node(abi, &tmp);

	ABI_SET_KEYA_VAL(tmp, P3_CHALLENGE_T, ainfo->B.chalenge);
	abi_append_node(abi, &tmp);

	ABI_SET_KEYA_VARA(tmp, P3_HASH_TYPE_T, ainfo->hash);
	pos = abi_append_node(abi, &tmp);

	return pos;
}

/** Shufle AUTH INFO node content according to recomenations
 *
 *  /para ti	i/o task filled with AUTH INFO node content
 *  /para pwd	password to use
 *  /returns non zero if failed 
 *  /returns zero on success
\*/
/*\
 *   Генерация Hash
 *   1. Генерируем "AUTH Info" без ["Password"]
 *   2. Кодируем его СОДЕРЖИМОЕ в ABI (без самого заголовка "AUTH Info" (ключа) и длины данных).
 *   3. Делаем XOR с "паролем" (то, что короче, зацикливается: либо пароль либо сообщение)
 *   4. Добавляем "пароль" в начало и в конец кодированного ABI
 *   5. Считаем hash-функцию и её результат добавляем к сообщению в поле "Password": 
 *   ["Password"]   REQUIRED (blob_t);
 *
\*/
int auth_info_shuffle(
	abi_io_task_t	*ti,
	data_t		*pwd
)
{
	/* move AUTH_INFO to proper offset */
	memmove(ti->data+pwd->size, ti->data, ti->todo);
	ti->todo += pwd->size;		/* set new  AUT_INFO size */

	/* place passwd [1] as AUTH_INFO HEAD */
	memcpy(ti->data, pwd->data, (size_t)pwd->size);

	/* No need to SHUFLE AUTH_INFO data */

	/* place passwd [2] as AUTH_INFO TAIL */
	memcpy(ti->data + ti->todo, pwd->data, (size_t)pwd->size);
	ti->todo += pwd->size;	/* set new AUT_INFO size */

	return 0;
}

/* Compute MD5 message digest for LEN bytes beginning at BUFFER.  The
   result is always in little endian byte order, so that a byte-wise
   output yields to the wanted ASCII representation of the message
   digest. */


/* FIXME TODO Add more hashes */
int abi_auth_info_hash(
       	p3_auth_info_t	*ainfo,
	data_t		*dst
)
{
	abi_io_task_t	*ti	  = &ainfo->task_i;
	dump_hex(ti->data, ti->todo);

	dst->size = 16;

	if(ti->todo)
		md5_buffer(ti->data, ti->todo, dst->data);
	else
		memset(dst->data, 0, dst->size);

	return 0;
}

/** Fill task with capabilities from auth context
 *
 *  /para tw	i/o task to fill
 *  /para ainfo	auth context
 *  /returns non zero if failed 
 *  /returns zero on success
 */
int auth_fill_caps(
	p3_auth_info_t	*ainfo
)
{
	abi_io_task_t *tw	= ainfo->task_tx;
	TRACE_LINE();

	int		res;
	abi_node_t	tmp  = { };
	abi_context_t	*abi = tw->abi;

	ainfo->state = 0;	/* FIXME Reset auth status */
	
	abiw_reopen(abi);

/*\
 *  Согласование возможностей
 *  [P3_AUTH_CAPS]
 *  {
 *      [P3_ABI_ASYNC_CONTEXT_T]       OPTIONAL (uint32_t);
 *  
 *      [P3_AUTH_CAPS_TX_T] {
 *          [P3_HASH_TYPE_T]           MULTIPLE (string_t);
 *          [P3_ENCRYPT_TYPE_T]        MULTIPLE (string_t);
 *  
 *          [P3_SECURITY_MODE_T]       REQUIRED (p3_security_mode_e);	// "Security Mode"	
 *      }
 *  
 *      [P3_AUTH_CAPS_RX_T] {						// = "/AUTH Capabilities RX"
 *          [P3_HASH_TYPE_T]           MULTIPLE (string_t);		// = "HASH"
 *          [P3_ENCRYPT_TYPE_T]        MULTIPLE (string_t);		// = "CRYPT"
 *  
 *          [P3_SECURITY_MODE_T]       REQUIRED (p3_security_mode_e);	// "Security Mode"	
 *      }
 *  
 *      [P3_ABI_STATUS_T]      OPTIONAL (p3_status_e);
 *  }
\*/
	ABI_SET_KEYA(tmp, P3_AUTH_CAPS);
	res = abi_append_node(abi, &tmp);

	ABI_SET_KEYA(tmp, P3_AUTH_CAPS_TX_T);
	res = abi_append_subnode(abi, res, &tmp);

	ABI_SET_KEYA_VARA(tmp, P3_HASH_TYPE_T, ainfo->hash);
	abi_append_subnode(abi, res, &tmp);

	ABI_SET_KEY32_VAR(tmp, P3_SECURITY_MODE_T, ainfo->A.mode_tx);
	abi_append_node(abi, &tmp);

	ABI_SET_KEYA(tmp, P3_AUTH_CAPS_RX_T);
	res = abi_insert_node(abi, res, &tmp);

	ABI_SET_KEYA_VARA(tmp, P3_HASH_TYPE_T, ainfo->hash);
	abi_append_subnode(abi, res, &tmp);

	ABI_SET_KEY32_VAR(tmp, P3_SECURITY_MODE_T, ainfo->A.mode_rx);
	abi_append_node(abi, &tmp);

	res = abiw_write_task(tw, 1);

	return res;
}

/** Fill task with AUTH_REQUEST from auth context
 *
 *  /para tw	i/o task to fill
 *  /para ainfo	auth context
 *  /returns non zero if failed 
 *  /returns zero on success
 */
int auth_fill_request(
	p3_auth_info_t	*ainfo
)
{
	abi_io_task_t *tw	= ainfo->task_tx;

	TRACE_LINE();

	int		res;
	abi_node_t	tmp = { };
	abi_context_t	*abi = tw->abi;
	
	abiw_reopen(abi);

	TRACE_LINE();

	/* fill requested chalenge  */
	abi_fill_chalenge(&ainfo->A.chalenge);
	
	(*ainfo->get_request_account)(ainfo);	/* get request user:password */

/*\
 *   Запрос на авторизацию
 *   [P3_AUTH_REQUEST]				// "AUTH Request"
 *   {
 *       [P3_ABI_ASYNC_CONTEXT_T] OPTIONAL (uint32_t);
 *   
 *       [P3_AUTH_INFO_T] {				// "/AUTH Info" block to be hashed + extra info
 *           [P3_USERNAME_T]   OPTIONAL (string_t);	// "Username" OPTIONAL!
 *           [P3_TS_T]	       REQUIRED (uint64_t);	// "Timestamp"
 *           [P3_CHALLENGE_T]  REQUIRED (blob_t);	// "Challenge" not less than 32 bytes of "random" data
 *   
 *           // any extra fields
 *           ...
 *       }
 *       [P3_HASH_TYPE_T]      MULTIPLE (string_t);	// "HASH" hash to use; none -> use any
 *       [P3_ABI_STATUS_T]     OPTIONAL (p3_status_e);
 *   }
 *   
\*/
	ABI_SET_KEYA(tmp, P3_AUTH_REQUEST);
	res = abi_append_node(abi, &tmp);

	ABI_SET_KEYA(tmp, P3_AUTH_INFO_T);
	res = abi_append_subnode(abi, res, &tmp);

	ABI_SET_KEYA_VAR(tmp, P3_TS_T, ainfo->A.ts);
	abi_append_subnode(abi, res, &tmp);

	ABI_SET_KEYA_VAL(tmp, P3_CHALLENGE_T, ainfo->A.chalenge);
	abi_append_node(abi, &tmp);

	if(ainfo->A.user.size) {	/* request the particular user */
		ABI_SET_KEYA_VAL(tmp, P3_USERNAME_T, ainfo->A.user);
		abi_append_node(abi, &tmp);
	}
#if 0
	ABI_SET_KEYA_VARA(tmp, P3_HASH_TYPE_T, ainfo->hash);
	res = abi_insert_node(abi, res, &tmp);
#endif
	TRACEF(9, abiw_dump(abi));

	TRACE_LINE();

	ainfo->A.ts = cur2filetime();

	res = abiw_write_task(tw, 1);

	return 0;
}

/** Fill task with AUTH_RESPONSE from auth context
 *
 *  /para tw	i/o task to fill
 *  /para ainfo	auth context
 *  /returns non zero if failed 
 *  /returns zero on success
 */
int auth_fill_response(
       	p3_auth_info_t	*ainfo
)
{
	abi_io_task_t *tw	= ainfo->task_tx;

	TRACE_LINE();

	abi_io_task_t	*ti	  = &ainfo->task_i;
	abi_context_t	*abi	  = tw->abi;
	
	int		res, next;
	abi_node_t	tmp = { };

	abiw_reopen(ti->abi);

	(*ainfo->get_response_account)(ainfo);	/* get response user:password */
	abi_fill_chalenge(&ainfo->chalenge);	/* get response chalenge */
	TRACEF(0, abi_show_data_t_as_str("TX-PASS", &ainfo->B.password));

	/* Prepare AUTH_INFO HASH */
	ti->done = 0;
	ti->todo = 0;
	ti->status = 0;

	abi_auth_info_make_abi(ti->abi, 0, ainfo);

	abiw_write(ti->abi, 0);

	auth_info_shuffle(ti, &ainfo->B.password);	/* shuffle AUTH INFO */
	abi_auth_info_hash(ainfo, &ainfo->B.hash);

	abiw_reopen(abi);
/*\
 *   Ответ на авторизацию/идентификацию
 *   [P3_AUTH_RESPONSE]				// "AUTH Response"
 *   {
 *       [P3_ABI_ASYNC_CONTEXT_T] OPTIONAL (uint32_t);
 *   
 *       [P3_AUTH_INFO_T] {				// "/AUTH Info"
 *           [P3_USERNAME_T]   OPTIONAL (string_t);	// "Username" (username may be changed/added by client)
 *           [P3_TS_T]	  REQUIRED (uint64_t);	// "Timestamp" must be equal to one from Request
 *           [P3_CHALLENGE_T]  REQUIRED (blob_t);	// "Challenge" must be equal to one from Request
 *   
 *           // any extra fields supplied in Request
 *           ...
 *   
 *           // any extra additional fields
 *           [P3_TS2_T]	  REQUIRED (uint64_t);	// "Timestamp2" OWN timestamp
 *           [P3_CHALLENGE2_T] REQUIRED (blob_t);	// "Challenge2" not less than 32 bytes of OWN "random" data
 *           [P3_HASH_TYPE_T]  REQUIRED (string_t);	// "HASH" used hash
 *       }
 *       [P3_PASSWORD_T]       REQUIRED (blob_t);	// "Password" Hash сгенерированный как описано ниже
 *       [P3_ABI_STATUS_T]     OPTIONAL (p3_status_e);
 *   }
 *
\*/
	ABI_SET_KEYA(tmp, P3_AUTH_RESPONSE);
	res = next = abi_append_node(abi, &tmp);

	ABI_SET_KEYA(tmp, P3_AUTH_INFO_T);
	res = abi_append_subnode(abi, res, &tmp);

	abi_auth_info_make_abi(abi, res, ainfo);

	ABI_SET_KEYA_VAL(tmp, P3_PASSWORD_T, ainfo->B.hash);
	abi_insert_node(abi, res, &tmp);

	res = abiw_write_task(tw, 1);

	TRACE_LINE();

	return 0;
}

/** Fill task with auth status
 *
 *  /para tw	i/o task to fill
 *  /para ainfo	auth context
 *  /returns non zero if failed 
 *  /returns zero on success
 */
int auth_fill_status(
	p3_auth_info_t	*ainfo
)
{
	abi_io_task_t *tw	= ainfo->task_tx;

	TRACE_LINE();

	int		res;
	abi_node_t	tmp  = { };
	abi_context_t	*abi = tw->abi;

	if(ainfo->state & P3_AUTH_DONE_POSITIVE) {
		ainfo->status = P3_SUCCESS;
	} else {
		ainfo->status = (uint32_t)-P3_FAILED;

		abi_kill_task(ainfo->io_task->ioc, ainfo->io_task);
	}

	abiw_reopen(abi);

/*\
 *   Статус Авторизации/идентификации
 *
 *   [AUTH_STATUS]			// "AUTH Status"
 *   {
 *       [P3_ABI_ASYNC_CONTEXT_T] OPTIONAL (uint32_t);
 *       ["Reason"]         OPTIONAL (string_t);	// Textual description
 *       [P3_ABI_STATUS_T]  REQUIRED (p3_status_e);	// hresult
 *       // когда договоримся о способе подписи и/или шифровки, сюда добавятся новые поля
 *   }
 *
\*/
	ABI_SET_KEYA(tmp, P3_AUTH_STATUS);
	res = abi_append_node(abi, &tmp);

	ABI_SET_KEY32_VAR(tmp, P3_ABI_STATUS_T, ainfo->status);
	res = abi_append_subnode(abi, res, &tmp);

//	ABI_SET_KEYA_VARA(tmp, P3_REASON_MSG_T, "X3");	/* UTF-8 ? UTF-16LE? */
//	abi_append_node(abi, &tmp);

	res = abiw_write_task(tw, 1);

	return res;
}



/*\
 *
 *   AUTH ABI reader parser and helpers
 *
\*/

/* Service routines */


/* Node callbacks */

/** Process AUTH_CAPS Node
 *
 *  /para abi	ABI contextl
 *  /para node	ABI node
 *
 *  /returns non zero if failed 
 *  /returns zero on success
\*/
static int auth_abi_on_auth_caps(
	abi_context_t *abi,
       	abi_node_t *node
)
{
	abi_mem_flow_t	*rx    = abi->priv;
       	p3_auth_info_t	*ainfo = rx->ctx; 
	
	TRACE_LINEL(5);
	auth_fill_request(ainfo);

	return 0;
}

/** Process AUTH_REQUEST Node
 *
 *  /para abi	ABI contextl
 *  /para node	ABI node
 *
 *  /returns non zero if failed 
 *  /returns zero on success
\*/
static int auth_abi_on_auth_request(
	abi_context_t *abi,
       	abi_node_t *node
)
{
	abi_mem_flow_t	*rx    = abi->priv;
       	p3_auth_info_t	*ainfo = rx->ctx; 

	if(!ainfo)
		return 1;

	TRACE_LINE();
	auth_fill_response(ainfo);

	return 0;
}

/** Process AUTH_RESP Node
 *
 *  /para abi	ABI contextl
 *  /para node	ABI node
 *
 *  /returns non zero if failed 
 *  /returns zero on success
\*/
static int auth_abi_on_auth_response(
	abi_context_t *abi,
       	abi_node_t *node
)
{
	abi_mem_flow_t	*rx	= abi->priv;
       	p3_auth_info_t	*ainfo	= rx->ctx; 
	abi_io_task_t	*ti	= &ainfo->task_i;

	TRACE_LINE();
	if(!ainfo)
		return 1;
	
	if(ainfo->state & P3_AUTH_DONE_NEGATIVE) {
		TRACEPNF(0, "P3_AUTH_INFO Checks failed\n");
		goto negative;
	}

	auth_info_shuffle(ti, &ainfo->A.password);	/* shuffle AUTH INFO */

	/* calculate hash for data FIXME TODO */
	abi_auth_info_hash(ainfo, &ainfo->B.hash);

	TRACE_LINE();

	if(
	    (ainfo->B.hash.size != ainfo->A.hash.size) ||
	    bcmp(ainfo->B.hash.data, ainfo->A.hash.data, (size_t)ainfo->B.hash.size)
	) {
		TRACEPNF(0, "P3_AUTH_INFO hash not matched len rcv:%zu our:%zu\n",
			ainfo->B.hash.size, ainfo->A.hash.size
		);
		goto negative;
	} else {
		ainfo->state	      |= P3_AUTH_DONE_POSITIVE;
		if(ainfo->io_task) {
			ainfo->io_task->flags &= ~ABI_IO_AUTHA_REQUIRED;
		}

		TRACEPNF(0, "P3_AUTH_DONE_POSITIVE\n");
	}

	auth_fill_status(ainfo);

	return 0;

negative:
	ainfo->state |= P3_AUTH_DONE_NEGATIVE;
	TRACEPNF(0, "P3_AUTH_DONE_NEGATIVE\n");

	auth_fill_status(ainfo);
	
	return 0;
}

/** Process AUTH_CAPS_RX Node
 *
 *  /para abi	ABI contextl
 *  /para node	ABI node
 *
 *  /returns non zero if failed 
 *  /returns zero on success
\*/
static int abi_on_auth_caps_rx_t(
	abi_context_t *abi,
       	abi_node_t *node
)
{
	abi_mem_flow_t	*rx    = abi->priv;
       	p3_auth_info_t	*ainfo = rx->ctx; 

	TRACE_LINE();
	if(!ainfo)
		return 1;

	return 0;
}

/** Process AUTH_CAPS_TX Node
 *
 *  /para abi	ABI contextl
 *  /para node	ABI node
 *
 *  /returns non zero if failed 
 *  /returns zero on success
\*/
static int abi_on_auth_caps_tx_t(
	abi_context_t *abi,
       	abi_node_t *node
)
{
	abi_mem_flow_t	*rx    = abi->priv;
       	p3_auth_info_t	*ainfo = rx->ctx; 

	TRACE_LINE();

	if(!ainfo)
		return 1;

	return 0;
}

/* Value readers calbacks */

/** Pre process hash type
 *  
 *  Read HASH type. Postprocessing managed by:
 *    abi_on_auth_caps_tx_t(...)
 *    abi_on_auth_caps_rx_t(...)
 *  callbacks.
 * 
 *  /para priv	ABI context priv
 *  /para data	destination data container ptr
 *  /para size	destination data container size
 *  /para tail	overrun of destination data container size
 *
 *  /returns consumed input octets count 
\*/
static int abir_read_hash_t(void *priv, uint8_t *data, size_t size, size_t tail)
{
	if(tail)
		return abi_mem_skiper(priv, size+tail);

	return abi_mem_reader(priv, data, size);
}

/** Read requested user name from AUTH_REQUEST
 *  
 *  Read requested user name. Postprocessing managed by:
 *  auth_abi_on_auth_request(...) callback.
 * 
 *  /para priv	ABI context priv
 *  /para data	destination data container ptr
 *  /para size	destination data container size
 *  /para tail	overrun of destination data container size
 *
 *  /returns consumed input octets count 
\*/
static int auth_abir_read_user_b(void *priv, uint8_t *data, size_t size, size_t tail)
{
	abi_mem_flow_t	*rx    = priv;
       	p3_auth_info_t	*ainfo = rx->ctx; 

	TRACE_LINE();

	if(tail)
		return abi_mem_skiper(priv, size);

	abi_mem_reader(priv, data, size);

	ainfo->B.user.size = size;
	ainfo->B.user.data = data;

	return 0;
}

/** Read requested chalenge
 *  
 *  /para priv	ABI context priv
 *  /para data	destination data container ptr
 *  /para size	destination data container size
 *  /para tail	overrun of destination data container size
 *
 *  /returns consumed input octets count 
\*/
static int auth_abir_read_chalenge_b(void *priv, uint8_t *data, size_t size, size_t tail)
{
	abi_mem_flow_t	*rx    = priv;
       	p3_auth_info_t	*ainfo = rx->ctx; 

	TRACE_LINE();

	abi_mem_reader(priv, data, size);
	if(tail)
		abi_mem_skiper(priv, tail);

	ainfo->B.chalenge.size = size;
	ainfo->B.chalenge.data = data;

	return 0;
}

/** Check Hash type in AUTH_RESPONSE FIXME
 *  
 *  /para priv	ABI context priv
 *  /para data	destination data container ptr
 *  /para size	destination data container size
 *  /para tail	overrun of destination data container size
 *
 *  /returns consumed input octets count 
\*/
static int auth_abir_check_hash_type(void *priv, uint8_t *data, size_t size, size_t tail)
{
	abi_mem_flow_t	*f	 = priv;
	p3_auth_info_t	*ainfo	 = f->ctx;

	abir_show_as_str("hash-avl", data, size, tail);
	abir_show_as_str("hash  RX", f->data + f->offs, size, tail);
	if(tail || bcmp(f->data + f->offs, data, size)) {
		ainfo->state |= P3_AUTH_DONE_NEGATIVE;
		TRACEPNF(0, "String Does Not Match\n");
	}

	f->offs += size + tail;

	return 0;
}

/** Check User Name in  AUTH_RESPONSE
 *  
 *  /para priv	ABI context priv
 *  /para data	destination data container ptr
 *  /para size	destination data container size
 *  /para tail	overrun of destination data container size
 *
 *  /returns consumed input octets count 
\*/
static int auth_abir_check_name_a(void *priv, uint8_t *data, size_t size, size_t tail)
{
	abi_mem_flow_t	*f	 = priv;
	p3_auth_info_t	*ainfo	 = f->ctx;

	abir_show_as_str("user-responce", f->data + f->offs, size, tail);
	abir_show_as_str("user--request", data, size, tail);

	if(size) {
		if(
		    tail ||
		    size != ainfo->A.user.size ||
		    bcmp(f->data + f->offs, data, size)
		) {
			ainfo->state |= P3_AUTH_DONE_NEGATIVE;
			TRACEPNF(0, "User Not Matched\n");
			goto done;
		}

		ainfo->A.user.size = size;
	} else { /* User name provided in Responce */
		size_t len = sizeof(ainfo->A.user_val);

		len = tail <  len ?tail :len;
		ainfo->A.user.size = len;
		ainfo->A.user.data = ainfo->A.user_val;

		memcpy(ainfo->A.user_val, f->data + f->offs, len);
	}


	/* We get password by username only if user present */
	int res = (*ainfo->get_password)(ainfo, &ainfo->A.password, &ainfo->A.user, NULL);
	if(res) {
		ainfo->state |= P3_AUTH_DONE_NEGATIVE;
		TRACEPNF(0, "User Not Found\n");
	}
	abi_show_data_t_as_str("PASS", &ainfo->A.password);
done:
	f->offs += size + tail;

	return 0;
}

/** Check blob in AUTH_RESPONSE
 *  If blob is differ than saved then FAIL auth 
 *  /para priv	ABI context priv
 *  /para data	destination data container ptr
 *  /para size	destination data container size
 *  /para tail	overrun of destination data container size
 *
 *  /returns consumed input octets count 
\*/
static int abi_auth_check_blob(void *priv, uint8_t *data, size_t size, size_t tail)
{
	abi_mem_flow_t	*f	 = priv;
	p3_auth_info_t	*ainfo	 = f->ctx;

	TRACEPNF(0, "CKb: [%3lu+%3lu]\n", (unsigned long)size, (unsigned long)tail);
	if(tail || bcmp(f->data + f->offs, data, size)) {
		ainfo->state |= P3_AUTH_DONE_NEGATIVE;
		TRACEPNF(0, "Challange or ts not matched\n");
	}
	
	f->offs += size + tail;

	return 0;
}

/** Read hash from AUTH_RESPONSE
 *  Reads hash value and updates hash descriptor. 
 *  /para priv	ABI context priv
 *  /para data	destination data container ptr
 *  /para size	destination data container size
 *  /para tail	overrun of destination data container size
 *
 *  /returns consumed input octets count 
\*/
static int auth_abir_read_hash(void *priv, uint8_t *data, size_t size, size_t tail)
{
	abi_mem_flow_t	*rx    = priv;
       	p3_auth_info_t	*ainfo = rx->ctx; 

	if(!tail) {
		abi_mem_reader(priv, data, size);
	} else {
		TRACEPNF(0, "%s [%zd+%zd] Hash size not matched\n", __func__, size, tail);
		
		ainfo->state     |= P3_AUTH_DONE_NEGATIVE;
		return abi_mem_skiper(priv, size+tail);
	}

	ainfo->state     |= P3_AUTH_HASH_PRESENT;

	ainfo->A.hash.data = data;
	ainfo->A.hash.size = size;

	return 0;
}

int abi_on_auth_info_t(void *priv, uint8_t *data, size_t size, size_t tail)
{
	abi_mem_flow_t	*rx    = priv;
       	p3_auth_info_t	*ainfo = rx->ctx; 

	TRACE_LINE();

	if(!tail) {
		abi_mem_reader(priv, data, size);
	} else {
		return abi_mem_skiper(priv, size+tail);
	}

	ainfo->state     |= P3_AUTH_INFO_PRESENT;

	ainfo->state     &=~(unsigned)(P3_AUTH_DONE_POSITIVE|P3_AUTH_DONE_NEGATIVE);

	abi_mem_flow_t	*f = &ainfo->ainfo_flow;
	f->data		= data;
	f->offs		= 0;
	f->max_size	= size;
	f->status	= 0;

	ainfo->task_i.done = 0;
	ainfo->task_i.todo = size;	/* keep size of ainfo */

	return 	abir_read_size(ainfo->abi, (uint32_t)size);
}

/** Init ainfo AUTH_INFO NODE parser
 *
 *  /para ainfo	auth context
 */
static void abi_init_check_auth_info(
       	p3_auth_info_t	*ainfo
)
{
	abi_node_t	tmp = { };
	abi_mem_flow_t	*flow = &ainfo->ainfo_flow;

	abi_context_t	*abi = abir_open(NULL, flow,
	    abi_mem_reader, abi_mem_skiper,
	    8
	);

	ainfo->abi		= abi;

	flow->ctx		= ainfo;
	flow->name		= "AINFO_CHK";
	    
	ABI_SET_KEYA_VAL(tmp, P3_USERNAME_T, ainfo->A.user);
	ABI_SET_READER(tmp, auth_abir_check_name_a);
	abi_append_node(abi, &tmp);

	ABI_SET_KEYA_VAR(tmp, P3_TS_T, ainfo->A.ts);
	ABI_SET_READER(tmp, abi_auth_check_blob);
	abi_append_node(abi, &tmp);

	ABI_SET_KEYA_VAL(tmp, P3_CHALLENGE_T, ainfo->A.chalenge);
	ABI_SET_READER(tmp, abi_auth_check_blob);
	abi_append_node(abi, &tmp);
#if 0
	ABI_SET_KEYA_VAR(tmp, P3_TS2_T, ainfo->B.ts);
	abi_append_node(abi, &tmp);

	ABI_SET_KEYA_VAL(tmp, P3_CHALLENGE2_T, ainfo->B.chalenge);
	abi_append_node(abi, &tmp);
#endif
	ABI_SET_KEYA_VARA(tmp, P3_HASH_TYPE_T, ainfo->hash);
	ABI_SET_READER(tmp, auth_abir_check_hash_type);
	abi_append_node(abi, &tmp);

	return;
}

/** Init ainfo abir parser
 *
 *  /para tw	i/o task to fill
 *  /para ainfo	auth context
 *  /returns non zero if failed 
 *  /returns zero on success
 */
int auth_abir_init(
       	p3_auth_info_t	*ainfo
)
{
	int		res, next;
	abi_context_t	*abi;
	abi_node_t	tmp = { };

	ainfo->flow_rx.ctx = ainfo;	/* save priv ctx */

	abi_init_check_auth_info(ainfo);
	
	abi = abir_open(NULL, &ainfo->flow_rx, abi_mem_reader, abi_mem_skiper, 40);
	ainfo->abi_rx = abi;

/*\
 *  Согласование возможностей
 *  [P3_AUTH_CAPS]
 *  {
 *      [P3_ABI_ASYNC_CONTEXT_T]       OPTIONAL (uint32_t);
 *
 *      [P3_AUTH_CAPS_TX_T] {
 *          [P3_HASH_TYPE_T]           MULTIPLE (string_t);
 *          [P3_ENCRYPT_TYPE_T]        MULTIPLE (string_t);
 *
 *          [P3_SECURITY_MODE_T]       REQUIRED (p3_security_mode_e);	// "Security Mode"	
 *      }
 *
 *      [P3_AUTH_CAPS_RX_T] {						// = "/AUTH Capabilities RX"
 *          [P3_HASH_TYPE_T]           MULTIPLE (string_t);		// = "HASH"
 *          [P3_ENCRYPT_TYPE_T]        MULTIPLE (string_t);		// = "CRYPT"
 *
 *          [P3_SECURITY_MODE_T]       REQUIRED (p3_security_mode_e);	// "Security Mode"	
 *      }
 *
 *      [P3_ABI_STATUS_T]      OPTIONAL (p3_status_e);
 *  }
\*/
	ABI_SET_KEYA(tmp, P3_AUTH_CAPS);
	ABI_SET_ACTION(tmp, auth_abi_on_auth_caps);
	res = next = abi_append_node(abi, &tmp);

	ABI_SET_KEYA(tmp, P3_AUTH_CAPS_TX_T);
	ABI_SET_ACTION(tmp, abi_on_auth_caps_tx_t);
	res = abi_append_subnode(abi, res, &tmp);

	ABI_SET_KEYA_VAR(tmp, P3_HASH_TYPE_T, ainfo->temp_val);
	ABI_SET_READER(tmp, abir_read_hash_t);
	abi_append_subnode(abi, res, &tmp);

	ABI_SET_KEY32_VAR(tmp, P3_SECURITY_MODE_T, ainfo->B.mode_tx);
	abi_append_node(abi, &tmp);

	ABI_SET_KEYA(tmp, P3_AUTH_CAPS_RX_T);
	ABI_SET_ACTION(tmp, abi_on_auth_caps_rx_t);
	res = abi_insert_node(abi, res, &tmp);

	ABI_SET_KEYA_VAR(tmp, P3_HASH_TYPE_T, ainfo->temp_val);
	ABI_SET_READER(tmp, abir_read_hash_t);
	abi_append_subnode(abi, res, &tmp);

	ABI_SET_KEY32_VAR(tmp, P3_SECURITY_MODE_T, ainfo->B.mode_rx);
	abi_append_node(abi, &tmp);

/*\
 *   Запрос на авторизацию
 *   [P3_AUTH_REQUEST]				// "AUTH Request"
 *   {
 *       [P3_ABI_ASYNC_CONTEXT_T] OPTIONAL (uint32_t);
 *   
 *       [P3_AUTH_INFO_T] {				// "/AUTH Info" block to be hashed + extra info
 *           [P3_USERNAME_T]   OPTIONAL (string_t);	// "Username" OPTIONAL!
 *           [P3_TS_T]	       REQUIRED (uint64_t);	// "Timestamp"
 *           [P3_CHALLENGE_T]  REQUIRED (blob_t);	// "Challenge" not less than 32 bytes of "random" data
 *   
 *           // any extra fields
 *           ...
 *       }
 *       [P3_ABI_STATUS_T]     OPTIONAL (p3_status_e);
 *   }
 *   
\*/

	ABI_SET_KEYA(tmp, P3_AUTH_REQUEST);
	ABI_SET_ACTION(tmp, auth_abi_on_auth_request);
	res = next = abi_insert_node(abi, next, &tmp);

	ABI_SET_KEYA(tmp, P3_AUTH_INFO_T);
	res = abi_append_subnode(abi, res, &tmp);

	ABI_SET_KEYA_VAR(tmp, P3_USERNAME_T, ainfo->B.user_val);
	ABI_SET_READER(tmp, auth_abir_read_user_b);
	abi_append_subnode(abi, res, &tmp);

	ABI_SET_KEYA_VAR(tmp, P3_TS_T, ainfo->B.ts);
	abi_append_node(abi, &tmp);

	ABI_SET_KEYA_VAR(tmp, P3_CHALLENGE_T, ainfo->B.chalenge_val);
	ABI_SET_READER(tmp, auth_abir_read_chalenge_b);
	abi_append_node(abi, &tmp);

/*\
 *   Ответ на авторизацию/идентификацию
 *   [P3_AUTH_RESPONSE]				// "AUTH Response"
 *   {
 *       [P3_ABI_ASYNC_CONTEXT_T] OPTIONAL (uint32_t);
 *   
 *       [P3_AUTH_INFO_T] {				// "/AUTH Info"
 *           [P3_USERNAME_T]   OPTIONAL (string_t);	// "Username" (username may be changed/added by client)
 *           [P3_TS_T]	  REQUIRED (uint64_t);	// "Timestamp" must be equal to one from Request
 *           [P3_CHALLENGE_T]  REQUIRED (blob_t);	// "Challenge" must be equal to one from Request
 *   
 *           // any extra fields supplied in Request
 *           ...
 *   
 *           // any extra additional fields
 *           [P3_TS2_T]	  REQUIRED (uint64_t);	// "Timestamp2" OWN timestamp
 *           [P3_CHALLENGE2_T] REQUIRED (blob_t);	// "Challenge2" not less than 32 bytes of OWN "random" data
 *           [P3_HASH_TYPE_T]  REQUIRED (string_t);	// "HASH" used hash
 *       }
 *       [P3_PASSWORD_T]       REQUIRED (blob_t);	// "Password" Hash сгенерированный как описано ниже
 *       [P3_ABI_STATUS_T]     OPTIONAL (p3_status_e);
 *   }
 *
\*/
	ABI_SET_KEYA(tmp, P3_AUTH_RESPONSE);
	ABI_SET_ACTION(tmp, auth_abi_on_auth_response);
	res = next = abi_insert_node(abi, next, &tmp);

	ABI_SET_KEYA_VARP(tmp, P3_AUTH_INFO_T, ainfo->task_i.data, ainfo->task_i.max);
	ABI_SET_READER(tmp, abi_on_auth_info_t);
	res = abi_append_subnode(abi, res, &tmp);

	ABI_SET_KEYA_VAR(tmp, P3_PASSWORD_T, ainfo->A.hash_val);
	ABI_SET_READER(tmp, auth_abir_read_hash);
	abi_append_node(abi, &tmp);
	
	TRACE_LINEL(1);
	TRACEF(9, abiw_dump(abi));

	return res;
}

/* stubs */

/** Fill Password from [username] and [domain]
 *
 */
static int auth_get_password_stub(
	struct p3_auth_info_s	*auth,
       	data_t			*pwd,
       	const data_t		*user,
       	const data_t		*domain
)
{
	static uint8_t	def_pwd[1] = "1";

	pwd->data = def_pwd;
	pwd->size = sizeof(def_pwd);

	return 0;
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
	B->user     = (data_t){ 5, { .data = (uint8_t*)"admin" } }; /* requested user     */

	return 0;
}

/*
 * Fill A.Username And A.Password
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

/* service routines */


int auth_init(p3_auth_info_t *ainfo, abi_io_task_t *task_tx, p3_auth_cb_t *ifc)
{
	TRACE_LINE();
	
	ainfo->task_tx = task_tx;	/* save ptr to TX task */

	/* init ainfo and deps */
	ABI_FILL_DATA_T(ainfo->A.chalenge, ainfo->A.chalenge_val);
	ABI_FILL_DATA_T(ainfo->B.hash,	   ainfo->B.hash_val);
	ABI_FILL_DATA_T(ainfo->chalenge,   ainfo->chalenge_val);	/* temporal chalenge */

	ainfo->B.chalenge  = (data_t){ };	/* clear B.chalenge */

	ainfo->hash       = "md5",		/* supported hash */

	abi_preinit_taskw(&ainfo->task_i,  -1, NULL, (size_t)4096);

	auth_abir_init(ainfo);

	/* register stubs */
	ainfo->get_password		= &auth_get_password_stub;
	ainfo->get_response_account	= &auth_get_response_account_stub;
	ainfo->get_request_account	= &auth_get_request_account_stub;

	if(ifc) {
		if(ifc->get_password)
			ainfo->get_password = ifc->get_password;

		if(ifc->get_response_account)
			ainfo->get_response_account = ifc->get_response_account;

		if(ifc->get_request_account)
			ainfo->get_request_account = ifc->get_request_account;
	}

	return 0;
}

int auth_release(p3_auth_info_t *ainfo)
{
	abi_release_task(&ainfo->task_i);

	if(ainfo->abi_rx) {
		abi_close(ainfo->abi_rx);
		ainfo->abi_rx = NULL;
	}

	if(ainfo->abi) {
		abi_close(ainfo->abi);
		ainfo->abi = NULL;
	}

	return 0;
}

int task_auth_init(abi_io_tasks_t *task, uint32_t flags)
{
	TRACE_LINE();

	if(task->auth) {
		TRACEP(0, "%s: ERROR Already Exist\n", __func__);
		return 1;
	}

	p3_auth_info_t *ainfo = task->auth = calloc((size_t)1, sizeof(p3_auth_info_t));
	if(!ainfo) {
		ETRACE("ERROR Auth info alloc:");
		return 1;
	}

	ainfo->io_task	= task;	/* save task context ptr */
	ainfo->flags	= flags;

	task->flags |= ABI_IO_AUTHA_REQUIRED;	/* request auth of Alice */

	/* setup expire time */
	if(task->ioc->tm)
		task->expires = *task->ioc->tm + P3_FT_ONE_SECOND*4;

	int ret = auth_init(ainfo, &task->writer, task->ioc->auth_ifc);

	auth_fill_caps(ainfo);
	abi_task_send(task, NULL, 0ul);

	return ret;
}

int task_auth_release(abi_io_tasks_t *task, uint32_t flags)
{
	TRACE_LINE();
	int res;

	if(!task->auth) {
		TRACEP(0, "%s: Already Released\n", __func__);
		return 1;
	}

	/* release subobjects */
	res = auth_release(task->auth);

	/* release memory */
	free(task->auth);
	task->auth = NULL;		/* Be a bit paronoidal */
	
	return res;
}
