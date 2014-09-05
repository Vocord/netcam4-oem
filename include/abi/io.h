#ifndef __ABI_IO_H__
#define __ABI_IO_H__
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


/** libabi access header
 *
 */

#if defined(WIN32) && !defined(__GNUC__)
#include <compilers/visualc/inttypes.h>
#include <compilers/visualc/netinet/in.h>
#else
#include <inttypes.h>
#include <netinet/in.h>
#include <sys/poll.h>
#endif

#include <trace.h>
#include <compiler.h>

/** Defines, limits */
#define ABI_READER_MAX_NODES	128
#define ABI_READER_MAX_NODE_IES	32
#define ABI_WRITER_MAX_NODES	32

#ifdef __cplusplus
extern "C" {
#endif

#define ABI_SUB_OPR_(X, Y) {				\
	X = Y;						\
	if(X)						\
		return X;				\
}

#define ABI_SUB_OPR(X, Y) {				\
	X = Y;						\
	if(X) {						\
		TRACEP(0, "ERR(%d): %s\n", X, #Y);	\
		return X;				\
	}						\
}

#define ABI_SUB_OP(Y) {					\
	int X;						\
	X = Y;						\
	if(X) {						\
		TRACEP(0, "ERR(%d): %s\n", X, #Y);	\
		return X;				\
	}						\
}

#if __BYTE_ORDER==__BIG_ENDIAN
#define ABI_C2_BE32(X)		((const uint32_t)(X))
#define ABI_SC2_BE32(X)		((const uint32_t)(X))
#else
#if __BYTE_ORDER==__LITTLE_ENDIAN
#define ABI_SC2_BE32__(X)	(htonl(X))
#define ABI_C2_BE32__(X)	( (((X)>>24)&0x000000ff)+ \
				  (((X)>> 8)&0x0000ff00)+ \
			       	  (((X)<< 8)&0x00ff0000)+ \
				  (((X)<<24)&0xff000000)  \
				)
#define ABI_SC2_BE32(X)		(ABI_SC2_BE32__(X))
#define ABI_C2_BE32(X)		(ABI_C2_BE32__((const uint32_t)X))
#else
#error "__BYTE_ORDER undefined, please check your includes"
#endif
#endif

/** */
#define ABI_FILL_DATA_T(V, VAR) V = (data_t) { sizeof(VAR), { .data = (uint8_t*)&VAR } };

/** */
#define ABI_KEY_FILL(X)		{sizeof(X), (const uint8_t*)&X}
#define ABI_KEY_FILL_S(X)	.key = ABI_KEY_FILL(X)
#define ABI_VAL_FILL(X)		{sizeof(X), (uint8_t*)&X}
#define ABI_VAL_FILL_S(X)	.val = ABI_VAL_FILL(X)
#define ABI_VAL_FILLS(X,Y)	{X, (uint8_t*)&Y}
#define ABI_VAL_FILLS_S(X,Y)	.val = ABI_VAL_FILL(X, Y)

/* Fill ABI const Key32 with var located in memory */
#define ABI_SET_KEY32_VAR(N,K,V) {			\
	static const uint32_t akey_##K = ABI_C2_BE32(K);\
        (N).key.data = (typeof((N).key.data))&akey_##K;	\
        (N).key.size = sizeof(akey_##K);		\
        (N).val.data = (typeof((N).val.data))&V;	\
        (N).val.size = sizeof(V);			\
	(N).action = NULL;				\
	(N).rval = NULL;				\
}

#define ABI_SET_KEY32VAL32(N,K,V) {			\
	static const uint32_t akey = ABI_C2_BE32(K);	\
	static uint32_t aval;				\
       	aval = htonl(V);				\
	(N).key.data = (typeof((N).key.data))&akey;	\
	(N).key.size = sizeof(akey);			\
	(N).val.data = (typeof((N).val.data))&aval;	\
        (N).val.size = sizeof(aval);			\
	(N).action = NULL;				\
	(N).rval = NULL;				\
}

#define ABI_SET_VKEY32VAL32(N,K,V) {			\
	static uint32_t akey; akey = ABI_C2_BE32(K);	\
	static uint32_t aval; aval = ABI_C2_BE32(V);	\
        (N).key.data = (typeof((N).key.data))&akey;	\
        (N).key.size = sizeof(akey);			\
        (N).val.data = (typeof((N).val.data))&aval;	\
        (N).val.size = sizeof(aval);			\
	(N).action = NULL;				\
	(N).rval = NULL;				\
}

#define ABI_SET_KEY32VAL(N,K,V) {			\
	static const uint32_t akey_##K = ABI_C2_BE32(K);\
        (N).key.data = (typeof((N).key.data))&akey_##K;	\
        (N).key.size = sizeof(akey_##K);		\
        (N).val = *V;					\
	(N).action = NULL;				\
	(N).rval = NULL;				\
}

#define ABI_SET_KEY32VALPS(N,K,VP,VS) {			\
	static const uint32_t akey_##K = ABI_C2_BE32(K);\
        (N).key.data = (typeof((N).key.data))&akey_##K;	\
        (N).key.size = sizeof(akey_##K);		\
        (N).val.data = (typeof((N).val.data))VP;	\
        (N).val.size = VS;				\
	(N).action = NULL;				\
	(N).rval = NULL;				\
}

#define ABI_SET_KEY32VALPcS(N,K,VP,VS) {			\
	static const uint32_t akey_##K = ABI_C2_BE32(K);\
        (N).key.data = (typeof((N).key.data))&akey_##K;	\
        (N).key.size = sizeof(akey_##K);		\
        (N).val.cdata = (typeof((N).val.cdata))VP;	\
        (N).val.size = VS;				\
	(N).action = NULL;				\
	(N).rval = NULL;				\
}


#define ABI_SET_KEY32_VARP(N,K,PTR,SIZE) {		\
	static const uint32_t akey_##K = ABI_C2_BE32(K);\
        (N).key.data = (typeof((N).key.data))&akey_##K;	\
        (N).key.size = sizeof(akey_##K);		\
        (N).val.data = (typeof((N).val.data))PTR;	\
        (N).val.size = SIZE;				\
	(N).action = NULL;				\
	(N).rval = NULL;				\
}

#define ABI_SET_KEY32VALPSW(N,K,VP,VS,W) {		\
	static const uint32_t akey_##K = ABI_C2_BE32(K);\
        (N).key.data = (typeof((N).key.data))&akey_##K;	\
        (N).key.size = sizeof(akey_##K);		\
        (N).val.data = (typeof((N).val.data))VP;	\
        (N).val.size = VS;				\
	(N).action = NULL;				\
	(N).write = W;					\
}

#define ABI_SET_KEY32NODES(N,K,D) {			\
	static const uint32_t akey = ABI_C2_BE32(K);	\
        (N).key.data = (typeof((N).key.data))&akey;	\
        (N).key.size = sizeof(akey);			\
        (N).val.data = (typeof((N).val.data))&D;	\
	(N).val.size = sizeof(D)-1;			\
	(N).action   = NULL;				\
	(N).rval     = NULL;				\
}

/* Fill ABI Node with 32bit Key + non ZT String data */
#define ABI_SET_KEY32_VARA(N, K, V) {			\
	static const uint32_t akey = ABI_C2_BE32(K);	\
        (N).key.data = (typeof((N).key.data))&akey;	\
        (N).key.size = (unsigned)sizeof(akey);		\
        (N).val.data = (uint8_t*)V;			\
	(N).val.size = strlen((const char*)V);		\
	(N).action   = NULL;				\
	(N).rval     = NULL;				\
}

/* Fill ABI Node with String key */
#define ABI_KEYA2KEY_T(K) (datak_t){			\
        .size = abi_csize_ ## K(), 			\
        .data = abi_const_ ## K() 			\
}

/* Fill ABI Node with String key */
#define ABI_SET_KEYA(N, K) {				\
        (N).key.data = abi_const_ ## K();		\
        (N).key.size = abi_csize_ ## K();		\
        (N).val.data = NULL;				\
	(N).val.size = (unsigned)0;			\
	(N).action   = NULL;				\
	(N).rval     = NULL;				\
}

/* Fill ABI Node with non ZT String key */
#define ABI_SET_KEYA_VARA(N, K, V) {			\
        (N).key.data = abi_const_ ## K();		\
        (N).key.size = abi_csize_ ## K();		\
        (N).val.data = (uint8_t*)V;			\
	(N).val.size = strlen(V);			\
	(N).action   = NULL;				\
	(N).rval     = NULL;				\
}

/* Fill ABI String Key with var located in memory */
#define ABI_SET_KEYA_VAR(N,K,V) {			\
        (N).key.data = abi_const_ ## K();		\
        (N).key.size = abi_csize_ ## K();		\
        (N).val.data = (typeof((N).val.data))&V;	\
	(N).val.size = sizeof(V);			\
	(N).action   = NULL;				\
	(N).rval     = NULL;				\
}

/* Fill ABI Key_t with var located in memory */
#define ABI_SET_KEYT_VAR(N,K,V) {			\
	(N).key = (K);					\
	(N).val.data = (typeof((N).val.data))&V;	\
	(N).val.size = sizeof(V);			\
	(N).action   = NULL;				\
	(N).rval     = NULL;				\
}

/* Fill ABI Key_t with var indexed */
#define ABI_SET_KEYT_VI(N,K,V,I) {			\
	(N).key      = (K);				\
	(N).val      = (V);				\
	(N).val.data += (N).val.size *(I);		\
	(N).action   = NULL;				\
	(N).rval     = NULL;				\
}


/* Fill ABI String Key with VAL descriptor */
#define ABI_SET_KEYA_VAL(N,K,V) {			\
        (N).key.data = abi_const_ ## K();		\
        (N).key.size = abi_csize_ ## K();		\
        (N).val      = V;				\
	(N).action   = NULL;				\
	(N).rval     = NULL;				\
}

/* Fill ABI String Key with ptr to blob */
#define ABI_SET_KEYA_VARP(N,K, PTR, SIZE) {		\
        (N).key.data = abi_const_ ## K();		\
        (N).key.size = abi_csize_ ## K();		\
        (N).val.data = (typeof((N).val.data))(PTR);	\
	(N).val.size = (SIZE);				\
	(N).action   = NULL;				\
	(N).rval     = NULL;				\
}

#define ABI_SET_KEY32(N,K) {			\
	static const uint32_t akey = ABI_C2_BE32(K);    \
        (N).key.data = (typeof((N).key.data))&akey;	\
        (N).key.size = sizeof(akey);			\
        (N).val.data = NULL;				\
	(N).val.size = (unsigned)0;			\
	(N).action   = NULL;				\
	(N).rval     = NULL;				\
}

/* Set Action for ABI Node */
#define ABI_SET_ACTION(N,ACTION) {			\
	(N).action    = ACTION;				\
}

/* Set Reader for ABI Node */
#define ABI_SET_READER(N,READER) {			\
	(N).rval     = READER;				\
}

/* Set Writer for ABI Node */
#define ABI_SET_WRITER(N,WRITER) {			\
	(N).write     = WRITER;				\
}

/* Compare AKEY with key located in memory */
#define ABI_CMP_KEYA(K, V) ({					\
	size_t sz = (V)->size;					\
	sz = sz > abi_csize_ ## K() ? abi_csize_ ## K() :sz;	\
        memcmp((V)->data, abi_const_ ## K(), sz) ;		\
})

#define ARRAY2DATA_T(A)  (data_t){ sizeof(A[0]), { (void*)(A + 0) } }

/** Error codes & op status */
typedef enum {
        ABI_OK		= 0,
        ABI_ERANGE,		/* INDEX not present: Invalid Range */
        ABI_EFULL,		/* TO MANY nodes                    */
        ABI_EZERO_NODES,	/* NO NODES to process              */
        ABI_ELEVEL,		/* NODE LEVEL too deep              */
        ABI_ENO_MATCH,		/* No one key match ABI PACKET      */
        ABI_EPARSE,		/* Data parse ERROR                 */
        ABI_EOID,		/* End of input data                */
        ABI_EIO,		/* I/O errror                       */
} abi_op_status_e;

typedef enum {
        ABI_IO_EVENT_USER_MASK	= (0x1ffff),	/* user io events state mask*/

        ABI_IO_INT_BUFF		= (1<<0x1b),	/* internal buffer used     */
        ABI_IO_EVENT_SELF	= (1<<0x1c),	/* alow to send self events */
        ABI_IO_EVENT_SENDER	= (1<<0x1d),	/*?task is event sender     */
        ABI_IO_EVENT_HANDLER	= (1<<0x1e),	/*?task is event handler    */
} abi_io_ftask_e;

typedef enum {
        ABI_WRITE_LENGTH	= 0,
        ABI_WRITE_PACKET,
} abi_write_stage_e;

typedef enum {
        ABI_READ_INITIAL	= 0,	/* reader in initial state          */
        ABI_READ_HEADER		= 0,	/* reader is reading packet header  */
        ABI_READ_LENGTH		= 0,	/* reader is reading ABI packet len */
        ABI_READ_PACKET,		/* reader is reading ABI packet data*/
	
	ABI_READ_SKIP_ALL,		/* reader skips all incoming data   */
} abi_read_stage_e;

typedef enum {
	ABI_REQ_MASK 		= (1U<<0x18)-1, /* request type mask        */

	ABI_REQ_CTX		= (1U<<0x18), /* request context present    */
	ABI_DATA_SKIP		= (1U<<0x19), /* skip reply sent (filtered) */
	ABI_DATA_ASYNC		= (1U<<0x1a), /* received async request     */
	ABI_ID_GID		= (1U<<0x1b), /* p3_gid present             */
	ABI_ID_DGID		= (2U<<0x1b), /* p3_gid present DYNO        */
	ABI_ID_ID		= (3U<<0x1b), /* p3_id present              */
	ABI_ID_REQ		= (1U<<0x1d), /* request id present         */

	ABI_DATA_RESP		= (1U<<0x1e), /* data responce ready        */
	ABI_DATA_REQ		= (1U<<0x1f), /* data request received      */
} abi_processing_flags_e;

typedef enum {
        ABI_IO_FD_READER	= (1<<0x00),	/* task for fd              */
        ABI_IO_TASK_SOCKET	= (1<<0x01),	/* task for socket          */
        ABI_IO_LISTEN_SOCKET	= (1<<0x02),	/* task for listen socket   */
        ABI_IO_FD_WRITER	= (1<<0x03),	/* task for blocked write   */

        ABI_IO_TASK_TYPES	=   ABI_IO_FD_READER	| 
				    ABI_IO_TASK_SOCKET	|
				    ABI_IO_LISTEN_SOCKET|
				    ABI_IO_FD_WRITER,

        ABI_IO_TASK_CLOSE_FD	=   ABI_IO_FD_READER	| 
				    ABI_IO_TASK_SOCKET	|
				    ABI_IO_FD_WRITER,


        ABI_IO_LIMITED_TASK	= (1<<0x1b),	/* task for limited iops    */
        ABI_IO_DYING_TASK	= (1<<0x1c),	/* task should be killed    */
        ABI_IO_AUTHA_REQUIRED	= (1<<0x1d),	/* AUTH A required          */
        ABI_IO_AUTHB_REQUIRED	= (1<<0x1e),	/* AUTH B required          */
        ABI_IO_TASK_ACTIVE	= (1<<0x1f),	/* task used                */
} abi_task_flags_e;

/** forward decls */
struct pollfd;
struct abi_io_tasks_s;

typedef struct abi_context_s	abi_context_t;
typedef struct abi_node_s	abi_node_t;
struct io_ctrl_s;

/** data types decls */
typedef struct {
        size_t		size;
	union {
        	uint32_t	*u32;
        	int32_t		*i32;
        	uint16_t	*u16;
        	int16_t		*i16;
        	uint8_t		*u8;
        	int8_t		*i8;
        	uint8_t		*data;
		const uint8_t	*cdata;
		char		*c;
		const char	*cc;
	} ;
} data_t;

typedef struct {
        size_t		size;
        const uint8_t	*data;
} datak_t;

struct jpeg_buffs {
	union {
		const uint8_t *cbuff;
		uint8_t *buff;
	} buff;
	size_t	buff_size;
	size_t	size;
	int     count;
} __attribute__((packed)) ;

/** exported functions decls */
typedef int abi_iow_f(void *, const uint8_t *data, size_t size);
typedef int abi_io_f(void*, uint8_t *data, size_t size);

typedef int abi_task_ior_f(struct abi_io_tasks_s*, uint8_t *data, size_t size);
typedef int abi_task_iow_f(struct abi_io_tasks_s*, const uint8_t *data, size_t size);
typedef int abi_auth_cb_f(struct abi_io_tasks_s*, uint32_t event);
typedef int abi_io_event_f(struct abi_io_tasks_s*, int event);
typedef int abi_io_cb_f(struct io_ctrl_s *, int);

typedef int abir_skiper_f(void *, size_t size);
typedef int abir_nreader_f(void *, uint8_t *data, size_t size, size_t tail);
typedef int abir_node_cb_f(abi_context_t *abi, abi_node_t *node);
typedef int abir_inode_cb_f(abi_context_t *abi, int branch);

typedef struct {	/* iteration task io support */
        uint64_t	dhash;	/* date hash request data  */

	unsigned	state;	/* task statemachine state */
        abi_io_ftask_e	flag;	/* task dir groups and flags */

	union {
	    abi_task_ior_f	*opr;	/* task i/o handler  */
	    abi_task_iow_f	*opw;	/* task i/o handler  */
	} ;
	abi_context_t	*abi;	/* task abi          */

        void		*data;	/* rx buffer addr    */

        int		status;	/* buffer status     */

        unsigned long	todo_fb;/* tail data size fall back */
        unsigned long	todo;	/* tail data size    */
        unsigned long   done;	/* last data offset  */
        unsigned long	max;	/* max buffer space  */

        int		fd;	/* i/o descriptor    */

        int		mm_fd;	/* mmbuf  i/o descriptor */
        uint8_t		*mm_offs; /* mmbuff data ptr     */
        unsigned long	mm_size;/* mmbuff window size    */
	off_t		mm_fd_size;

        void		*priv;	/* task private data */

	struct chunk    *chain;
	int             chain_pidx;
	int             chain_gidx;
} abi_io_task_t;

typedef struct sockaddr sockaddr_t;
typedef struct sockaddr_in sockaddr_in_t;

typedef struct abi_io_tasks_s {
        abi_io_task_t		reader;
        abi_io_task_t		writer;

        abi_io_event_f		*poll;
        abi_io_event_f		*shutdown;
        abi_io_event_f		*multiple;	/* cb for multiple send task
						   in the group (round-robin,
						   etc) */

	int32_t			flags;		/* task OP mode & flags */
	uint32_t		flags_auth;	/* authentication flags */

	int16_t			fidx;		/* task idx to fd map */
	int16_t			tidx;		/* fd idx to task map */

	struct io_ctrl_s	*ioc;
	void			*priv;		/* tasks private data	*/
	void			*auth;		/* auth private data	*/

	uint32_t		tmout;		/* session timeout	*/
        uint64_t		expires;	/* session expire time	*/

	sockaddr_in_t		party;		/* session party addr	*/
	socklen_t		party_size;	/* party addr size      */
} abi_io_tasks_t;

/*
 * Struct for in memory data hamdling for abi i/o
 */
typedef struct {
	void		*ctx;	/* flow private context        */
	const char	*name;	/* flow name for debug traces  */

	void		*data;	/* flow data ptr               */
	unsigned long	offs;	/* current data offset         */
	
	unsigned long	max_size; /* flow data buffer size     */

	int		status;	/* flow status                 */
} abi_mem_flow_t;

/*
 * Struct for hamdling of the set of i/o streams
 */
typedef struct io_ctrl_s {
	struct pollfd	*fds;		/* pollfds array               */
	abi_io_tasks_t	*tasks;		/* tasks array                 */
                                                                      
	nfds_t		cnt;		/* i/o tasks active            */
	nfds_t		max;		/* i/o tasks free              */
	
	void		*priv;		/* per ioc private data        */
	void		*ifs;		/* ifs ctx for accounting      */

	abi_auth_cb_f	*auth_init;	/* task auth init callback     */
	abi_auth_cb_f	*auth_release;	/* task auth release callback  */
	void		*auth_ifc;	/* auth custom interface stubs */
	uint32_t	auth_flags;	/* authentication flags        */
	
	abi_io_event_f	*accept;	/* task i/o creator callback   */
	abi_io_event_f	*reject;	/* task i/o rejecter callback  */

	int		plugin_state;	/* iterator plugin state       */
	void		*plugin_ctx;	/* iterator plugin context     */
	abi_io_cb_f	*plugin_cb;	/* iterator plugin callback    */

	abi_io_cb_f	*rx_cb;		/* incomming request callback  */

	abi_context_t	*rx_abi;	/* requests process ABI parser */
	abi_mem_flow_t	rx_flow;	/* requests process flow       */

	abi_io_tasks_t	*rx_task;	/* request or accept task      */

	uint32_t	rx_flags;	/* request flags               */
	uint32_t	tx_flags;	/* responce flags              */

	size_t		req_ctx_len;	/* size of request ctx if any  */

	size_t		buff_rx;	/* size of rx buf to allocate  */
	size_t		buff_tx;	/* size of tx buf to allocate  */

	uint64_t	*tm;		/* time value                  */

	sockaddr_in_t	party;		/* accepted session party addr */
	socklen_t	party_size;	/* accepted party addr size    */
} io_ctrl_t;

struct abi_node_s {
        datak_t		key;	/* Key content			*/
        data_t		val;	/* Value content		*/

        int16_t		next;	/* Next level node in val	*/
        int16_t		neig;	/* Neighbour node		*/

        uint32_t	size;	/* node + subnodes size		*/

        /* Read ABI part */
        int16_t		level;	/* to control deep level	*/
        int16_t		match;	/* to control key match		*/

        void		*priv;	/* private data for rval/action */

        union {
                abi_iow_f	*write;	/* called to write val */
                abir_nreader_f	*rval;	/* called to post val to user   */
        } ;
        abir_node_cb_f	*action;/* called to deliver IE to user */
};

struct abi_context_s {
        abi_node_t	*nodes;
        
        int		last;	/* index of first unused node	*/
        int		level;	/* to control max deep level	*/
        size_t		max;	/* to control max node idx	*/
        int		match;	/* to control match list	*/
        
        uint32_t	size;	/* total PACKET size		*/

        void		*priv;	/* private data for read/write  */
        
        abi_iow_f	*write;	/* block data writer		*/
        abi_io_f	*read;	/* block data reader		*/

        abir_skiper_f	*skip;	/* block data skiper		*/

        uint8_t		key[16];/* key data container           */
        
        int		status;	/* ext cmd status		*/
        int		_errno;	/* ext cmd errno		*/
	int		error;	/* reader error			*/
};

/** takes in account header size (ie ssize)
 * Calculates size of size container
 */
static __inline__ uint32_t abiw_ajust_size(uint32_t size)
{
        uint32_t d;

        d  = size>0x007fU? 2U:1U;
        d += size>0x3fffU? 2U:0U;

        return size + d;
}

typedef union {
        uint32_t	v;
        struct {
                uint16_t	v0;
                uint16_t	v1;
        } s16;
        struct {
                uint8_t		v0;
                uint8_t		v1;
                uint8_t		v2;
                uint8_t		v3;
        } s8;
} abir_ssize_t;

static __inline__ void abi_set_keyd_action(
        abi_node_t     *node,
        const uint8_t  *data,
        abir_node_cb_f *action
)
{
        node->key.data = data;
        node->action = action;
}

static __inline__ void abi_set_key32node(
        abi_node_t     *node,
        const uint32_t *key,
        uint8_t        *data,
        size_t		size
)
{
        node->key.data = (typeof(node->key.data))key;
        node->key.size = sizeof(*key);
	
        node->val.data = data;
	node->val.size = size;
}

static __inline__ void abi_trace_task(abi_io_task_t  *t)
{
	if(t) {
		TRACE(0, "%08lx|%3d|%05lx|%5lu|%5lu|%05x|%08lx|%08lx|",
			(unsigned long)t->data,
			t->fd,
			t->max,
			t->todo,
			t->done,
			t->state,
			(unsigned long)t->opr,
			(unsigned long)t->abi
		);
	} else {
		TRACE(0, "data ptr| fd|  max| todo| done|state|     op|     abi|");
	}
}

static __inline__ abi_io_tasks_t *abi_get_free_task(io_ctrl_t *ioc)
{
	int		i;

	for(i=0; i<ioc->max; i++) {
		if(!(ioc->tasks[i].flags & ABI_IO_TASK_ACTIVE)) {
			return ioc->tasks + i;
		}
	}

	return NULL;
}

extern void abi_mem2fd_taskw_ajust(abi_io_task_t *tw);
extern int abi_mem2fd_task_send(
	abi_io_tasks_t	*t,
	struct pollfd	*fds,
	const char	*name,
	const uint8_t	*data,
	size_t		size
);

extern int abi_mem2fd_http_ioc_send(abi_io_tasks_t *t, const uint8_t *data, size_t size);

/** Append new data to send buffer and send remaining data
 */
static __inline__ int abi_mem2fd_ioc_send(abi_io_tasks_t *t, const uint8_t *data, size_t size)
{
	io_ctrl_t	*ioc	= t->ioc;
	struct pollfd	*fds	= ioc->fds + t->fidx;
	const char	*name	= ioc->rx_flow.name;

	return abi_mem2fd_task_send(t, fds, name, data, size);
}

static __inline__ int abi_mem2fd_ioc_send_completely(abi_io_tasks_t *t, 
								const uint8_t *data, size_t size)
{
	io_ctrl_t	*ioc	= t->ioc;
	struct pollfd	*fds	= ioc->fds + t->fidx;
	const char	*name	= ioc->rx_flow.name;
	abi_io_task_t	*tw	= &t->writer;

	if(tw->todo + size > tw->max)
		return -ENOMEM;

	return abi_mem2fd_task_send(t, fds, name, data, size);
}

static __inline__ int abi_task_send(abi_io_tasks_t *t, const uint8_t *data, size_t size)
{
	if(!(t->flags & ABI_IO_LIMITED_TASK))
		return (*t->writer.opw)(t, data, size);

	if(size) {	/* send new data atomic chunk if limit alows */
		if(t->reader.dhash) {
			t->reader.dhash--;
			if(!t->reader.dhash)	/* DIE after atomic send have completed */
				t->flags |= ABI_IO_DYING_TASK;

			return (*t->writer.opw)(t, data, size);
		}

		return 0;	/* do non send new data atomic chunk */
	}

	return (*t->writer.opw)(t, data, size);
}

static __inline__ void abi_kill_task(io_ctrl_t *ioc, abi_io_tasks_t *t)
{
	if(!ioc || !t)
		return;

	t->flags |= ABI_IO_DYING_TASK;

	int fidx = t->fidx;
	if((fidx<0) || (fidx>=ioc->cnt))
		return;

 	/* mark for deletion inside io_iterator */
	ioc->fds[fidx].revents |= POLLERR;
}

static __inline__ void abi_kill_cur_task(io_ctrl_t *ioc)
{
	abi_kill_task(ioc, ioc->rx_task);
}

/* ABI task support API */
extern int abi_init_ioc(
	io_ctrl_t	*,
       	unsigned	max_agents,
       	void		*,
       	void		*ifs,
	size_t		buff_rx,	/* rx buff size */
	size_t		buff_tx,	/* tx buff size */
       	char		*name		/* subsystem stream name */
);
extern int abi_free_ioc(io_ctrl_t *);

/* ABI common API */
extern void abi_close(abi_context_t *abi);
extern int abi_insert_node(abi_context_t *abi, int idx, abi_node_t *node);
extern int abi_append_node(abi_context_t *abi, abi_node_t *node);
extern int abi_append_subnode(abi_context_t *abi, int idx, abi_node_t *node);

/* ABI writer API */
extern abi_context_t *abiw_open(
        abi_context_t   *abi,
        void            *priv,
        abi_iow_f       *write,
        size_t          max
);
extern void abiw_reopen(abi_context_t *abi);
extern uint32_t abiw_eval_ssize(uint32_t size, uint32_t *res);
extern int abiw_get_node_sizes(abi_context_t *abi, int idx);
extern int abiw_write_task(abi_io_task_t *tw, int flag);
extern int abiw_write(abi_context_t *abi, int flag);
extern int abiw_write_pre(abi_context_t*, uint32_t *size, int flag);
extern int abiw_write_do(abi_context_t *abi, int flag);

extern int abiw_dump(abi_context_t *abi);

/* May be became private soon */
int abiw_write_recursive(abi_context_t *abi, int idx);

/* ABI reader ABI */
extern abi_context_t *abir_open(
        abi_context_t   *abi, 
        void            *priv, 
        abi_io_f        *read, 
        abir_skiper_f   *skip, 
        size_t          max
);
//extern void abir_reopen(abi_context_t *abi);
extern int abir_read_size(abi_context_t *abi, uint32_t size);
extern int abir_read(abi_context_t *abi, int flag);
extern int abir_dump(abi_context_t *abi);

/* utils */
extern void dump_asci(volatile uint8_t *, size_t size);
extern void dump_hex(volatile uint8_t *, size_t size);
extern void dump_hex2file(
	const char		*name,
	int			*fd,
	const char		*msg,
	volatile uint8_t	*data,
	size_t			size
);
extern void abi_dump_to_fd(void *, size_t, const char *name);

extern void abi_release_task(abi_io_task_t*);

//extern int  abi_preinit_task(abi_io_task_t*, int s, abi_task_io_f*, size_t size);
extern int  abi_preinit_taskr(abi_io_task_t*, int s, abi_task_ior_f*, size_t size, size_t todo);
extern int  abi_preinit_taskw(abi_io_task_t*, int s, abi_task_iow_f*, size_t size);
extern int  abi_preinit_http_taskw(abi_io_task_t*, int s, abi_task_iow_f*, size_t size);
extern int abi_task_write2mem(void *priv, const uint8_t *data, size_t size);
extern int abi_http_task_write2mem(void *priv, const uint8_t *data, size_t size);

#if 0
extern int  abi_mem2fd_ioc_send(
	abi_io_tasks_t	*t,
       	uint8_t		*data,
       	size_t		size
);
#endif
extern int abi_connect_inet_socket(io_ctrl_t *, const char *name, int port);
extern int abi_ioc_add_socket(io_ctrl_t *ioc, int s);
extern int abi_ioc_add_fd_writer(
	io_ctrl_t	*ioc,
	abi_task_ior_f	*opr,
	int		fd,
	size_t 		buff_tx
);
extern int abi_ioc_add_fd_reader(
	io_ctrl_t	*ioc,
	abi_task_ior_f	*opr,
	int		s,
	size_t 		buff_rx
);
extern void abi_close_socket(io_ctrl_t *, int fd_del_idx, int line);
extern void abi_trace_ioc_state(io_ctrl_t	*ioc, int line);	/* trace ioc state */

extern int abi_ioc_set_listen_sock(io_ctrl_t *ioc, int s);
extern int abi_ioc_append2all(
	io_ctrl_t*,
	int		flag,
	void		*data,
	size_t		size
);

extern int abi_ioc_recv_header(abi_io_tasks_t *t);

extern int abi_io_iterator(io_ctrl_t *ioc, int poll_timeout, unsigned reject_timeout);

extern int abi_mem_reader(void *, uint8_t *data, size_t size);
extern int abi_mem_skiper(void *, size_t size);
extern int abi_mem_skipern(void *, size_t size);
extern int abi_mem_skiperq(void *, size_t size);

/* Silent skip mem reader */
extern int abi_mem_skip(void *, uint8_t *data, size_t size, size_t tail);

extern int abi_rx_cb_stub_ex(io_ctrl_t	*ioc, int i);

/* dump functions */
extern void abi_show_data_t_as_str(char *prefix, data_t*);
extern void abir_show_as_str(
	char		*prefix,
       	uint8_t		*data,
       	size_t		size,
       	size_t		tail
);

#if ABI_IO_ITERATOR_PLUGIN
static inline int abi_io_iterator_plugin(
	io_ctrl_t	*ioc,
       	int		poll_timeout,
       	unsigned	reject_timeout
) __attribute__((unused));
static inline int abi_io_iterator_plugin(
	io_ctrl_t	*ioc,
       	int		poll_timeout,
       	unsigned	reject_timeout
)
{
	int	res = 0;
	if(ioc->plugin_cb) {
		res = (*ioc->plugin_cb)(ioc, ioc->plugin_state);
	}

	if(!res)
		return abi_io_iterator(ioc, poll_timeout, reject_timeout);

	return res;
}
#endif

static inline int abi_ioc_event_data_append2all(
	abi_io_tasks_t *t,
	int flag,
	void *data,
	size_t size
)
{
	return abi_ioc_append2all(t->ioc, flag, data, size);
}

static inline void abi_init_ioc_plugin(
	io_ctrl_t	*ioc,
	abi_io_cb_f	*cb,
	void		*ctx,
	int		state
) __attribute__((unused));
static inline void abi_init_ioc_plugin(
	io_ctrl_t	*ioc,
	abi_io_cb_f	*cb,
	void		*ctx,
	int		state
)
{
	ioc->plugin_cb		= cb;
	ioc->plugin_ctx		= ctx;
	ioc->plugin_state	= state;
}

static inline void abi_writer_task_reset(
	abi_io_task_t	*tw
) __attribute__((unused));
static inline void abi_writer_task_reset(
	abi_io_task_t	*tw
)
{
	abiw_reopen(tw->abi);

	/* clear Writer task buffer */
	tw->done   = 0;
	tw->todo   = 0;
	tw->status = 0;
}

static inline void abi_writer_task_append(
	abi_io_task_t	*tw
) __attribute__((unused));
static inline void abi_writer_task_append(
	abi_io_task_t	*tw
)
{
	abiw_reopen(tw->abi);

	/* clear Writer task buffer */
	tw->todo_fb = tw->todo;
	tw->status = 0;
}


/* Append event from task */
static __inline__ int abi_ioc_event_append2all(abi_io_tasks_t *t, int flag) __attribute__((unused));
static __inline__ int abi_ioc_event_append2all(abi_io_tasks_t *t, int flag)
{
	abi_io_task_t	*sw = &t->writer;

	abi_ioc_event_data_append2all(t, flag, sw->data, sw->todo);

	return 0;
}

/** Remove io task from IOC by abi_io_tasks_t*
 * Should be used outside abi_io_iterator() ! 
 */
static inline void __attribute__((unused)) abi_ioc_release_task (abi_io_tasks_t *t, int line)
{
	int		i, fd;
	io_ctrl_t	*ioc = t->ioc;

	if(t->flags & ABI_IO_FD_WRITER)
		fd = t->writer.fd;
	else
		fd = t->reader.fd;

	for(i=0; i<ioc->cnt; i++) {
		if(fd == ioc->fds[i].fd) {
			return abi_close_socket(t->ioc, i, line);
		}
	}

	TRACEP(0, "Task Not Found: %8p\n", t);
}

#ifdef __cplusplus
}
#endif

#endif
