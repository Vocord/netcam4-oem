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

/** libabi Helpers PART
 *
 */
#define _GNU_SOURCE 1
//#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/poll.h>
#include <sys/mman.h>
#include <sys/socket.h>

#include <fcntl.h>

#ifndef TRACE_LEVEL
#define TRACE_LEVEL	0
#endif
#define TRACE_PRIVATE_PREFIX	1
#include <trace.h>

static char	*trace_prefix = "ABI_IO:";

#include <abi/p3.h>
#include <abi/io.h>
#include <api/auth.h>

#include <os-helpers/net-sockets.h>

struct chunk {
	union { /* trick to avoid warning about mix of const/non-const type on assignment */
		const void *cdata;
		void       *data;
	} data;
	size_t size;
	int *count;
};

#define chunk_array_size 10

/**
 * !! Unknown error types Threads unsafe!!!
 */
const char *p3_errnotostr(int p3_errno)
{
	static char err_range[] = "Unknown ERROR:          ";

	static const char *p3_errstr[] = {
		[P3_SUCCESS         ] = "P3_SUCCESS         ",
		[P3_HARDWARE_FAULT  ] = "P3_HARDWARE_FAULT  ",
		[P3_SOFTWARE_FAULT  ] = "P3_SOFTWARE_FAULT  ",
		[P3_MEDIA_DATA_FAULT] = "P3_MEDIA_DATA_FAULT",
		[P3_MEDIA_FAULT     ] = "P3_MEDIA_FAULT     ",
		[P3_STOPPED         ] = "P3_STOPPED         ",
		[P3_INVALID_FLOW    ] = "P3_INVALID_FLOW    ",
		[P3_INVALID_TIME    ] = "P3_INVALID_TIME    ",
		[P3_INVALID_QOS     ] = "P3_INVALID_QOS     ",
		[P3_INVALID_SCALE   ] = "P3_INVALID_SCALE   ",
		[P3_INVALID_ARGUMENT] = "P3_INVALID_ARGUMENT",
		[P3_INVALID_CONTEXT ] = "P3_INVALID_CONTEXT ",
		[P3_ALREADY_DONE    ] = "P3_ALREADY_DONE    ",
		[P3_NOT_IMPLEMENTED ] = "P3_NOT_IMPLEMENTED ",
		[P3_NO_RESOURCES    ] = "P3_NO_RESOURCES    ",
		[P3_NO_DATA         ] = "P3_NO_DATA         ",
		[P3_FAILED          ] = "P3_FAILED          ",
		[P3_TIME_TO_LOW     ] = "P3_TIME_TO_LOW     ",
		[P3_TIME_TO_BIG     ] = "P3_TIME_TO_BIG     ",
		[P3_TO_MANY_CLIENTS ] = "P3_TO_MANY_CLIENTS ",
		[P3_TRY_AJUSTED_ARGS] = "P3_TRY_AJUSTED_ARGS",
		[P3_DB_FAILED       ] = "P3_DB_FAILED       ",
		[P3_UNKNOWN_COMMAND ] = "P3_UNKNOWN_COMMAND ",
		[P3_MEDIA_FULL      ] = "P3_MEDIA_FULL      ",
//		[                   ] = "                   ",
	};
	const char *s;

	if(p3_errno<0) {
		snprintf(err_range, sizeof(err_range)-1,
		       	"MORE DATA IN QUE:%6d", -p3_errno
		);
		return err_range;
	}

	if(p3_errno>=sizeof(p3_errstr)/sizeof(char*)) {
		snprintf(err_range, sizeof(err_range)-1,
		       	"Unknown ERROR:%6d", p3_errno
		);
		return err_range;
	}

	s = p3_errstr[p3_errno];

	if(!s) {
		snprintf(err_range, sizeof(err_range)-1,
		       	"Unknown ERROR:%6d", p3_errno
		);
		s = err_range;
	}

	return s;
}

void dump_asci(volatile uint8_t *data, size_t size)
{
	unsigned long	i;

	for(i=0;i<size;i++) {
		if(!(i&0x1f)) {
			TRACE(0, "\n%08lx:", i);
		}
		int c = *data;
		c = (c > 0x1f) && (c < 0x7f) ?c :'.';
		TRACE(0, "  %c",  c);
		data++;
	}
  	TRACE(0, "\n");
}

void dump_hex(volatile uint8_t *data, size_t size)
{
	unsigned long	i;

	dump_asci(data, size);

  	TRACE(0, "len: %lu@%08lx", (unsigned long)size, (unsigned long)data);
	if(!data)
		return;

	for(i=0;i<size;i++) {
		if(!(i&0x1f)) {
			TRACE(0, "\n%08lx:", i);
		}

		TRACE(0, " %02x", *data);
		data++;
	}
  	TRACE(0, "\n");
}

static void write_and_check_res(int fd, void *data, size_t size)
{
	ssize_t res = write(fd, data, size);
	if(res<0)
		ETRACE("When dump HEX to fd");
	else
		if(res != size)
			TRACEP(0, "Write:%zd insteadof %zd", res, size);
}

void dump_hex2file(const char *name, int *fd, const char *msg, volatile uint8_t *data, size_t size)
{
	if(*fd<0) {
		*fd = open(name, O_WRONLY|O_APPEND|O_TRUNC);
	}

	if(*fd<0)
		return;

	char	sb[25+10+3*32];
	char	tm[25];
	size_t	i;
	size_t	len;

	char	*s = sb;

	get_time_in_a1(tm);
  	i = (size_t)snprintf(sb, sizeof(sb), "%s:%s len: %zu", tm, msg, size);
	write_and_check_res(*fd, sb, i);

	if(!data || !size)
		return;

	for(i=0; i<size; i++) {
		if(!(i&0x1f)) {
			write_and_check_res(*fd, sb, (size_t)(s - sb));
			s = sb;

			len = (size_t)snprintf(tm, sizeof(tm), "\n%04zx", i);

			write_and_check_res(*fd, sb, len);
		}

		s += ((size_t)snprintf(s, sizeof(sb) - (size_t)(s-sb), " %02x", (unsigned)*data));
		data++;
	}

	s += snprintf(s, sizeof(sb) - (size_t)(s-sb), "\n");
	write_and_check_res(*fd, sb, (size_t)(s-sb));
}

/* Declare stubs */
#ifndef __CYGWIN__
int mcast_del_cl(void *, int) __attribute__((weak));
int mcast_add_cl(void *, int) __attribute__((weak));

int mcast_del_cl(void *ifs, int port)
{
	TRACEP(0, "%s not defined\n", __func__);
	abort();
	return 0;
}

int mcast_add_cl(void *ifs, int port)
{
	TRACEP(0, "%s not defined\n", __func__);
	abort();
	return 0;
}
#else
extern int mcast_del_cl(void *, int);
extern int mcast_add_cl(void *, int);
#endif

/* mem => mem abi reader/writer */
int abi_mem_reader(void *priv, uint8_t *data, size_t size)
{
	abi_mem_flow_t	*f = priv;

	if(f->status)
		return -ENOBUFS;

	TRACEP(4, "%s:RX: %8lxH@%08lxH->%08lx offs:%08lx ",
		f->name,
	       	(unsigned long)size,
		(unsigned long)f->data + f->offs,
	       	(unsigned long)data,
		f->offs
	);

	if(f->offs + size> f->max_size) {
		TRACEP(0, "%s: ENOBUFF max:%lu %zu@%lu\n",
			f->name,
			f->max_size,
			size,
			f->offs
		);

		f->offs = 0;		/* !!! */
		f->status = -ENOBUFS;	/* !!! */
		memset(data, 0, size);

		return -ENOBUFS;
	}

	memcpy(data, f->data+ f->offs, size);
	f->offs += size;

	TRACE(5, "[%3lu]",  (unsigned long)size);

	int i;
	for(i=0; (i<80) && (i<size); i++) {
		TRACE(6, " %02x", data[i]);
	}

	TRACE(4, "\n");
//	TRACE_FLUSH();

	return (int)size;
}

int abi_mem_skip(void *priv, uint8_t *data, size_t size, size_t tail)
{
	abi_mem_flow_t	*f = priv;

	if(f->status)
		return -ENOBUFS;

	size+=tail;

	TRACEP(8, "%s:RS: %8lxH@%08lxH->%08lx offs:%08lx ",
		f->name,
	       	(unsigned long)size,
		(unsigned long)f->data + f->offs,
	       	(unsigned long)data,
		f->offs
	);

	if(f->offs + size> f->max_size) {
		TRACEP(0, "%s: ENOBUFF\n", f->name);
		f->offs = 0;		/* !!! */
		f->status = -ENOBUFS;	/* !!! */

		return -ENOBUFS;
	}

	f->offs += size;

	TRACE(9, "[%3lu]",  (unsigned long)size);

	int i;
	for(i=0; (i<80) && (i<size); i++) {
		TRACE(6, " %02x", data[i]);
	}

	TRACE(8, "\n");

	return (int)size;
}

int abi_mem_skipern(void *priv, size_t size)
{
	abi_mem_flow_t	*f = priv;

	if(f->status)
		return -ENOBUFS;

	TRACEPNF(0, "%s:SK: [%8lu]", f->name, (unsigned long)size);
	if(size<=0)
		goto exit;

	if(f->offs + size > f->max_size) {
		f->offs = 0;		/* !!! */
		f->status = -ENOBUFS;	/* !!! */

		return -ENOBUFS;
	}

	TRACE(0, " [%3lu]", (unsigned long)size);
	unsigned long i;
	for(i=0; (i<16) && (i<size); i++) {
		TRACE(0, " %02x", ((uint8_t*)f->data)[f->offs + i]);
	}

	if(size>32) {
		TRACE(6, "..");
		i = size - 16;
	}

	for( ; i<size; i++) {
		TRACE(0, " %02x", ((uint8_t*)f->data)[f->offs + i]);
	}

	f->offs+=size;

exit:
	TRACE(0, "\n");

	return 0;
}

int abi_mem_skiper(void *priv, size_t size)
{
	abi_mem_flow_t	*f = priv;

	if(f->status)
		return -ENOBUFS;

	TRACEPNF(0, "%s:SK: [%8lu]", f->name, (unsigned long)size);
	if(size<=0)
		goto exit;

	if(f->offs + size > f->max_size) {
		f->offs = 0;		/* !!! */
		f->status = -ENOBUFS;	/* !!! */

		return -ENOBUFS;
	}
#if TRACE_LEVEL > 8
	TRACE(5, " [%3lu]", (unsigned long)size);
	size_t i;
	for(i=0; (i<16) && (i<size); i++) {
		TRACE(6, " %02x", ((uint8_t*)f->data)[f->offs + i]);
	}

	if(size>32) {
		TRACE(6, "..");
		i = size - 16;
	}

	for( ; i<size; i++) {
		TRACE(6, " %02x", ((uint8_t*)f->data)[f->offs + i]);
	}
#endif
	f->offs+=size;

exit:
	TRACE(0, "\n");

	return 0;
}

int abi_mem_skiperq(void *priv, size_t size)
{
	abi_mem_flow_t	*f = priv;

	if(f->status)
		return -ENOBUFS;

	if(size<=0)
		goto exit;

	if(f->offs + size> f->max_size) {
		f->offs = 0;		/* !!! */
		f->status = -ENOBUFS;	/* !!! */

		return -ENOBUFS;
	}
	f->offs+=size;

exit:

	return 0;
}

/** Fills task data buffer and todo/done vals from abiw_write
 *  !!! priv is ptr to abi_io_task_t
 */
int abi_task_write2mem(void *priv, const uint8_t *data, size_t size)
{
	abi_io_task_t	*t = priv;

	if(t->status)
		return -ENOBUFS;

	unsigned long	offs = t->todo + t->done;

	TRACEP(4, "WR: %7lxH@%08lxH->%08lx",
	       	(unsigned long)size,
	       	(unsigned long)data,
		(unsigned long)(t->data + offs)
	);

	if(offs + size> t->max) {
		TRACE(4, "\n");
		TRACEPNF(0, "WR: priv=%08lx ENOBUFS FB:%08lx\n", (unsigned long)priv, t->todo_fb);

		t->todo = t->todo_fb;	/* Set fallback value */
		t->status = -ENOBUFS;	/* !!! */

		return -ENOBUFS;
	}

#if TRACE_LEVEL > 4
	TRACE(5, " [%3lu]", (unsigned long)size);
	size_t i;
	for(i=0; (i<16) && (i<size); i++) {
		TRACE(6, " %02x", data[i]);
	}

	if(size>32) {
		TRACE(6, "..");
		i = size - 16;
	}

	for( ; i<size; i++) {
		TRACE(6, " %02x", data[i]);
	}

#endif
	TRACE(4, "\n");

	memcpy(t->data + offs, data, size);
	t->todo += size;

	return (int)size;
}

static int abi_http_task_add_chunk(abi_io_task_t *t, const uint8_t *data, size_t size)
{
	/* append data if any */
        if(data) {
		union {
			const struct jpeg_buffs* cb;
			struct jpeg_buffs* b;
		} trick = { .cb = (const struct jpeg_buffs*)data };
		if(t->chain[t->chain_pidx].data.cdata != 0) { /* current slot not empty */
			++t->chain_pidx;
			if(t->chain_pidx == chunk_array_size) { /* out of last slot */
				t->chain_pidx = 0; /* let's try first slot */
			}
			if(t->chain[t->chain_pidx].data.cdata != 0) {
				/* data will be lost */
				/* no space for new chunk, set put index to get index */
				t->chain_pidx = t->chain_gidx; /* current get index will be free first */
				TRACEP(0, "%s:%d TRACE no space for data is %p %lu %p %zd\n", __FILE__ , __LINE__, t->data, t->max, data, size );
				return -1;
			}
		}
		t->chain[t->chain_pidx].data.cdata = trick.cb->buff.cbuff /* data */;
		t->chain[t->chain_pidx].size = size;
		t->chain[t->chain_pidx].count = &trick.b->count;
	} else {
		TRACEPNF(0, "WARNING:%s:Data dropped:%lu@0x%08lx\n",
			__func__,
			(unsigned long)size,
		       	(unsigned long)data
		);
	}
	if(t->todo == 0) {
                /* single slot with data */
		t->chain_gidx = t->chain_pidx;
		t->data = t->chain[t->chain_gidx].data.data;
		t->todo = t->chain[t->chain_gidx].size;
		t->done = 0;
		if (t->chain[t->chain_gidx].count && t->todo != 0) {
			++*t->chain[t->chain_gidx].count;
		}
	}
	return 0;
}

/* abi IOC helpers */

void abi_dump_to_fd(void *d, size_t size, const char *name)
{
	int fd = open(name, O_WRONLY|O_CREAT, S_IWOTH|S_IROTH);
	ssize_t res = write(fd, d, size);
	if(res<0)
		ETRACE("When dump data to fd");
	else
		if(res != size)
			TRACEP(0, "Write:%zd insteadof %zd", res, size);

	close(fd);
}

void abi_release_task(abi_io_task_t	*t)
{
	if(t->chain) {
		TRACEP(9, "free(chain:%08lx)\n", (unsigned long)t->chain);
		free(t->chain);
		/* t->data = NULL; */
	}
	t->data = t->flag & ABI_IO_INT_BUFF ?t->data :NULL;
	if(t->data) {
		TRACEP(9, "free(data:%08lx)\n", (unsigned long)t->data);
		free(t->data);
	}

	if(t->abi) {
		TRACEP(9, "abi_close(%08lx)\n", (unsigned long)t->abi);
		abi_close(t->abi);
	}

	if(t->mm_fd>=0) {	/* Release IN fd if active one */
		if(t->mm_offs && t->mm_size) {
			munmap(t->mm_offs, t->mm_size);
		}

		close(t->mm_fd);
	}

	/* be a bit paranoidal */
	memset(t, 0, sizeof(*t));
	t->mm_fd = -1;
}
#if 0
int abi_preinit_task(abi_io_task_t *t, int s, abi_task_io_f *op, size_t size)
{
	t->max  = size;
	t->op   = op;

	TRACEP(9, "malloc((size_t)t->max:%08lx) => %08lx", t->max, (unsigned long)t->data);
	t->data = malloc((size_t)t->max);
	t->chain = 0;
	if(!t->data) {
		return -errno;
	}

	t->fd = s;

	t->todo = 0;
	t->done = 0;

	t->mm_fd = -1;			/* fd IN not active */
	t->flag	|= ABI_IO_INT_BUFF;	/* allow free on shutdown */

	return 0;
}
#endif

int abi_preinit_taskr(
	abi_io_task_t	*t,
	int		s,
	abi_task_ior_f	*opr,
	size_t		size,
	size_t		todo
)
{
	t->max  = size;
	t->opr   = opr;
	t->fd = s;

	if(!size) {
		return 0;
	}

	TRACEP(9, "malloc((size_t)t->max:%08lx) => %08lx", t->max, (unsigned long)t->data);
	t->data = malloc((size_t)t->max);
	t->chain = 0;
	if(!t->data) {
		return -errno;
	}

	/* init reader state */
	t->state  = ABI_READ_INITIAL;
	t->todo   = todo;	/* read chunk len */
	t->done   = 0;

	t->mm_fd = -1;			/* fd IN not active */
	t->flag	|= ABI_IO_INT_BUFF;	/* allow free on shutdown */

	return 0;
}

int abi_preinit_taskw(
	abi_io_task_t	*t,
	int		s,
	abi_task_iow_f	*opw,
	size_t		size
)
{
	t->max  = size;
	t->opw   = opw;

	TRACEP(9, "malloc((size_t)t->max:%08lx) => %08lx", t->max, (unsigned long)t->data);
	t->data = malloc((size_t)t->max);
	t->chain = 0;
	if(!t->data) {
		return -errno;
	}

	t->fd = s;

	t->todo = 0;
	t->done = 0;

	t->mm_fd = -1;	/* fd IN not active */

	t->flag	|= ABI_IO_EVENT_SENDER|ABI_IO_INT_BUFF;	/* able to send events */

	/* alloc writer ABI */
	t->abi = abiw_open(NULL,
	    t,
	    abi_task_write2mem,
	    50
	);

	return 0;
}


void abi_mem2fd_taskw_ajust(abi_io_task_t *tw)
{
	memmove(tw->data, tw->data + tw->done, tw->todo);
	tw->done = 0;
}

/** Append new data to send buffer and send remaining data
 */
int abi_mem2fd_task_send(
	abi_io_tasks_t	*t,
	struct pollfd	*fds,
	const char	*name,
	const uint8_t	*data,
	size_t		size
)
{
	abi_io_task_t	*tw	= &t->writer;
	ssize_t		res;

	/* append data if any */
	if(data && (tw->todo + size <= tw->max)) {
		if(tw->todo + tw->done + size > tw->max)
			abi_mem2fd_taskw_ajust(tw);

		memcpy(tw->data + tw->todo + tw->done, data, size);

		tw->todo += size;
	} else {
		if(size) {
			uint8_t *p;
			p = (typeof(p))&t->party.sin_addr.s_addr;
			TRACEPNF(0, "WARNING:%s:Data dropped: %u.%u.%u.%u:%-5u:%zu@0x%08lx\n",
				name,
				p[0], p[1], p[2], p[3],
				ntohs(t->party.sin_port),
				size,
				(unsigned long)data
			);
		}
	}

	if(!tw->todo) {
		fds->events &= ~(POLLOUT|POLLWRNORM);

		if(!(t->flags & ABI_IO_FD_WRITER))
			fds->events |= (POLLIN|POLLRDNORM);

		return 0;
	}

	errno = 0;

	/* Send data */
	res = write(tw->fd, tw->data +tw->done, tw->todo);
	TRACEPNF(4, "%s:write(fd, @%08lx, %08lx) => %d\n",
		name,
	       	(unsigned long)tw->data +tw->done, (unsigned long)tw->todo,
	       	res
	);

	if(res <= 0) {
		if(errno == EAGAIN) {
			fds->events |= POLLOUT|POLLWRNORM;
			fds->events &= ~(POLLIN|POLLRDNORM);
			return 0;
		}

		ETRACE("%s:ERROR Todo:0x%08lx Done:0x%08lx. errno:", name,
	       		(unsigned long)tw->done, (unsigned long)tw->todo
		);

		if(t->ioc)
			abi_kill_task(t->ioc, t);

		return -errno;
	}

	tw->todo -= (unsigned long)res;

	if(tw->todo) {
		tw->done += (unsigned long)res;

		fds->events |= POLLOUT|POLLWRNORM;
		fds->events &= ~(POLLIN|POLLRDNORM);
		return 0;
	}

	if(t->flags & ABI_IO_FD_WRITER)
		fds->events &= ~(POLLOUT|POLLWRNORM);
	else
		fds->events |= (POLLIN|POLLRDNORM);

	tw->done = 0;	/* clear initial offset */

	return t->flags & ABI_IO_DYING_TASK;
}

int abi_preinit_http_taskw(
	abi_io_task_t	*t,
	int		s,
	abi_task_iow_f	*opw,
	size_t		size
)
{
	t->max = size;
	t->opw = opw;

	TRACEP(9, "malloc((size_t):%08lx) => %08lx", (unsigned long)sizeof(struct chunk) * chunk_array_size, (unsigned long)t->chain);
	t->data = 0;
	t->chain = malloc(sizeof(struct chunk) * chunk_array_size);
	t->chain_pidx = 0;
	t->chain_gidx = 0;
	if(!t->chain) {
		return -errno;
	}

	memset(t->chain, 0, sizeof(struct chunk) * chunk_array_size);

	t->fd = s;

	t->todo = 0;
	t->done = 0;
	t->status = 0;

	t->mm_fd = -1;	/* fd IN not active */

	t->flag	|= ABI_IO_EVENT_SENDER;	/* able to send events */

	return 0;
}

int abi_mem2fd_http_ioc_send(
	abi_io_tasks_t	*t,
	const uint8_t	*data,
	size_t		size
)
{
	io_ctrl_t	*ioc	= t->ioc;
	struct pollfd	*fds	= ioc->fds + t->fidx;
	const char	*name	= ioc->rx_flow.name;
	abi_io_task_t	*tw	= &t->writer;
	int		res;

	/* append data if any */
	if(size && (abi_http_task_add_chunk(tw, data, size) != 0)) {
		/* no space for new chunk */
		TRACE(4, "\n");
		TRACEPNF(0, "WR: ENOBUFS\n");

		tw->status = -ENOBUFS;	/* !!! */

		return -ENOBUFS;
	}

	while ( tw->todo ) { /* while fd ready for write and non-empty chunks present */
		errno = 0;

		/* Send data */
		res = write(tw->fd, tw->data +tw->done, tw->todo);
		TRACEPNF( 4, "%s:write(fd, @%08lx, %08lx) => %d\n",
			  name,
			  (unsigned long)tw->data +tw->done, (unsigned long)tw->todo,
			  res
			);

		if(res <= 0) {
			if(errno == EAGAIN) {
				fds->events |= POLLOUT|POLLWRNORM;
				fds->events &= ~(POLLIN|POLLRDNORM);
				return 0;
			}

			ETRACE("%s:ERROR Todo:0x%08lx Done:0x%08lx. errno:", name,
			       (unsigned long)tw->done, (unsigned long)tw->todo
				);

			if(t->ioc)
				abi_kill_task(t->ioc, t);

			return -errno;
		}

		tw->todo -= (unsigned long)res;

		if(tw->todo) {
			tw->done += (unsigned long)res;

			fds->events |= POLLOUT|POLLWRNORM;
			fds->events &= ~(POLLIN|POLLRDNORM);
			return 0;
		}

                /* mark current slot as free */
		tw->chain[tw->chain_gidx].data.cdata = 0;
		tw->chain[tw->chain_gidx].size = 0;
		if (tw->chain[tw->chain_gidx].count) {
			--*tw->chain[tw->chain_gidx].count;
		}

                /* find next slot with data or reach slot for next put */
		if(tw->chain_gidx != tw->chain_pidx) {
			int old_gidx = tw->chain_gidx;
			int i;
			if(tw->chain[tw->chain_pidx].data.cdata != 0) {
				/* in tw->chain_pidx is latest data */
				tw->chain_gidx = tw->chain_pidx; /* */
				/* skip all other chunks, if any */
				for (i = 0; i < chunk_array_size; ++i) {
					if(i != tw->chain_gidx && i != old_gidx) {
						tw->chain[i].data.cdata = 0;
						tw->chain[i].size = 0;
					}
				}
			} else {
				/* slot tw->chain_pidx is empty; let's check previous slot */
				if(tw->chain_pidx == 0) {
					/* previous slot is chunk_array_size - 1 */
					if(tw->chain[chunk_array_size - 1].data.cdata != 0) {
						tw->chain_gidx = chunk_array_size - 1;	
					} else {
						tw->chain_gidx = 0; /* == tw->chain_pidx */	
					}
				} else {
					if(tw->chain[tw->chain_pidx-1].data.cdata != 0) {
						tw->chain_gidx = tw->chain_pidx - 1;
					} else {
						tw->chain_gidx = tw->chain_pidx;
					}
				}
				/* skip all other chunks, if any */
				for (i = 0; i < chunk_array_size; ++i) {
					if(i != tw->chain_gidx && i != old_gidx && i != tw->chain_pidx) {
						tw->chain[i].data.cdata = 0;
						tw->chain[i].size = 0;
					}
				}
			}
		}
		tw->data = tw->chain[tw->chain_gidx].data.data; /* may be 0 here */
		tw->todo = tw->chain[tw->chain_gidx].size; /* may be 0 here */
		tw->done = 0; /* clear initial offset */
		if (tw->chain[tw->chain_gidx].count && tw->todo != 0) {
			++*tw->chain[tw->chain_gidx].count;
		}
	}

	/* nothing to do */
	if(t->flags & ABI_IO_FD_WRITER)
		fds->events &= ~(POLLOUT|POLLWRNORM);
	else
		fds->events |= (POLLIN|POLLRDNORM);

	return t->flags & ABI_IO_DYING_TASK;
}

int abi_http_task_shutdown(struct abi_io_tasks_s *tasks, int event)
{
	abi_io_task_t *tw = &tasks->writer;

	if (tw && tw->todo != 0 && tw->chain != 0 && tw->chain[tw->chain_gidx].count != 0) {
		--*tw->chain[tw->chain_gidx].count;
	}

	return 0;
}

void abi_close_socket(io_ctrl_t *ioc, int fd_del_idx, int line)
{
	abi_io_tasks_t	*tasks	= ioc->tasks;
	abi_io_tasks_t	*tsk2del= tasks + tasks[fd_del_idx].tidx;	/* get task ref */

	abi_trace_ioc_state(ioc, line-1);

	if(!(tsk2del->flags & ABI_IO_TASK_CLOSE_FD))	/* FIXME TODO */
		goto done;

	if(fd_del_idx >= ioc->cnt) {
		TRACEPNF(0, "%s:WARNING: socket/fd task already released ===========\n", ioc->rx_flow.name);
		goto done;
	}

	if(tsk2del->flags & ABI_IO_TASK_SOCKET) {
		/* release auth info */
		if(ioc->auth_release)
			(*ioc->auth_release)(tsk2del, ioc->auth_flags);

		TRACEPNF(0, "%s: fidx:%d tidx: %zd\n", __func__, fd_del_idx, (size_t)(tsk2del - tasks));
		mcast_del_cl(ioc->ifs, ioc->fds[fd_del_idx].fd);	/* Release clint info */

		if(shutdown(ioc->fds[fd_del_idx].fd, SHUT_RDWR))
			ETRACE("while shutdown socket\n");
	}

	int	event = ioc->fds[fd_del_idx].revents;
	close(ioc->fds[fd_del_idx].fd);

	if(tsk2del->shutdown) {
		tsk2del->shutdown(tsk2del, event);
		tsk2del->shutdown = NULL;		/* release callback */
	}

	abi_release_task(&tsk2del->reader); /* free reader buffs */
	abi_release_task(&tsk2del->writer); /* free writer buffs */

	tsk2del->flags = 0;		/* clear task flags */

	ioc->cnt--;
	nfds_t fd_last_idx = ioc->cnt;

	/* Replace FD to del by last FD from ioc->fds */
	memmove(ioc->fds +fd_del_idx, ioc->fds +fd_last_idx, sizeof(ioc->fds[0]));
	memset(ioc->fds +fd_last_idx, 0, sizeof(ioc->fds[0]));

	/* ajust fidx & tidx for last_fd and its task */
	tasks[fd_del_idx].tidx		= tasks[fd_last_idx].tidx;

	int tsk_fd_last_idx		= tasks[fd_del_idx].tidx;
	tasks[tsk_fd_last_idx].fidx	= (int16_t)fd_del_idx;

	TRACEPNF(0, "%s: fd/socket task released =====================\n", ioc->rx_flow.name);
done:
	abi_trace_ioc_state(ioc, line);
}

int abi_ioc_recv_header(abi_io_tasks_t *t)
{
	abi_io_task_t	*tr = &t->reader;
	ssize_t		res;

	res = read(tr->fd, tr->data+tr->done, (size_t)tr->todo);
	if(res <= 0) {
		if(errno == EAGAIN) {
			return 0;
		}

		if(errno) {
			ETRACE("%s:Error when data len receiving:%d",
			    t->ioc->rx_flow.name, errno
			);
		}

		return -1;
	}

	/* update buffer info */
	tr->todo-=(unsigned long)res;
	tr->done+=(unsigned long)res;

	if(tr->todo) {		/* len is not received completely */
		TRACEP(0, "%s:Data len todo: %08lx res:%ld\n",
		   t->ioc->rx_flow.name, tr->todo, (long)res
		);
		return 0;
	}

	tr->state = ABI_READ_PACKET;
	tr->done  = 0;

	return 0;
}

int abi_ioc_recv(abi_io_tasks_t *t, uint8_t *data, size_t size)
{
	io_ctrl_t	*ioc	= t->ioc;
	abi_io_task_t	*tr	= &t->reader;

	ssize_t		res;

	errno = 0;		/* clear errno */

	if(tr->state == ABI_READ_LENGTH) {	/* is in read len state? */
		res = abi_ioc_recv_header(t);
		if(res)
			return res;

		if(tr->state == ABI_READ_LENGTH) /* return if incomplete */
			return 0;

		/* Len received */
		tr->todo	 = ntohl(*(uint32_t*)tr->data);
		if(tr->todo > tr->max) {
			TRACEP(0, "%s:Invalid data len: %08lx maximum allowed: %08lx\n",
			    t->ioc->rx_flow.name, tr->todo, tr->max
			);
			return -1;
		}
	}

	res = read(tr->fd, tr->data+tr->done, tr->todo);
	if(res <= 0) {
		if(errno == EAGAIN) {
			return 0;
		}

		if(errno) {
			ETRACE("%s:Error when data receiving:%d", ioc->rx_flow.name, errno);
		}

		return -1;
	}

	tr->done += (unsigned long)res;
	tr->todo -= (unsigned long)res;

	if(tr->todo) {
		TRACEPNF(4, "%s:Incomplete data, remains: %lu\n", ioc->rx_flow.name, tr->todo);

		return 0;
	}

	TRACEP(9, "%s:Read: %lu octets\n", ioc->rx_flow.name, tr->done);
	TRACEF(10, dump_hex(tr->data, tr->done));
//	TRACEF(9, abi_dump_to_fd(tr->data, tr->done, "/media/cm.log"));

	/* Init input flow  */
	abi_mem_flow_t	*f = &ioc->rx_flow;

	f->data     = tr->data;
	f->max_size = tr->done;
	f->offs     = 0;
	f->status   = 0;

//	TRACEF(0, append_to_file("/media/mdm.in", tr->data, tr->done));

	/* change task state */
	tr->state = ABI_READ_LENGTH;
	tr->done  = 0;
	tr->todo  = 4;

	ioc->rx_task = t;	/* keep link to last receiver task */

	if(!ioc->rx_cb) {
		return 0;
	}

	/* work on OOB_CMDS */
	int	i = t - ioc->tasks;

	return ioc->rx_cb(ioc, i);
}

/** Connect To client by ADDR and PORT and add connected socket to ioc
 *
\*/
int abi_connect_inet_socket(io_ctrl_t *ioc, const char *name, int port)
{
	struct sockaddr_in	in_addr = {};

	if(inet_ipv4_addr_setup(name, &in_addr, port))
	    	return -1;

	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(connect(fd, (struct sockaddr*)&in_addr, (socklen_t)sizeof(in_addr))) {
		ETRACE("Cant connect to: %s:%d", name, port);
		return -1;
	}

	return abi_ioc_add_socket(ioc, fd);
}

/** Helper for adding IOC tasks
 *
\*/
int abi_ioc_add_fd(
	io_ctrl_t	*ioc,
	abi_task_ior_f	*opr,
	int		flags,
	int		fd,
	size_t 		param
)
{
	if(ioc->max <= ioc->cnt) {
		return -EUSERS;
	}

	abi_io_tasks_t	*task = abi_get_free_task(ioc);
	if(!task) {
		TRACEP(0, "ERR:No free task, but should be!!!");

		return -EUSERS;		/* To many users */
	}

	int	tidx = task - ioc->tasks;

	/* create links and maps  */
	task->ioc			= ioc;			/* save backward link to ioc */
	task->fidx			= (int16_t)ioc->cnt;	/* map from task to fds */
	ioc->tasks[ioc->cnt].tidx	= (int16_t)tidx;	/* map from fds to task */

	/* setup fds */
	ioc->fds[ioc->cnt].fd		= fd;
	ioc->fds[ioc->cnt].events	= POLLIN|POLLRDNORM;
	ioc->fds[ioc->cnt].revents	= 0;

	task->reader.opr	= NULL;
	task->writer.opw	= NULL;

	if(fcntl(fd, F_SETFL, (long)O_NONBLOCK)) {
		ETRACE("fcntl(s, F_SETFL, O_NONBLOCK): ");
		return -errno;
	}

	task->flags	 = flags|ABI_IO_TASK_ACTIVE;	/* XXX for AUTH */
	if(ioc->party_size) {
		task->party = ioc->party;
		task->party_size = ioc->party_size;

		ioc->party_size = 0;
	}

	ioc->cnt++;

	switch(flags & ABI_IO_TASK_TYPES) {
	    case ABI_IO_FD_WRITER:
		ABI_SUB_OP(abi_preinit_taskw(&task->writer, fd, abi_mem2fd_ioc_send, param));
		ioc->fds[ioc->cnt].events	= 0;
		break;

	    case ABI_IO_FD_READER:
	    case ABI_IO_LISTEN_SOCKET:
		ABI_SUB_OP(abi_preinit_taskr(&task->reader, fd, opr, param, param));
		break;

	    case ABI_IO_TASK_SOCKET:
		/* set socket options */
		net_sock_keepalive(fd, 4, 10);

		if(!ioc->accept || (*ioc->accept)(task, (int)param)) {
			/* init reader task */
			ABI_SUB_OP(abi_preinit_taskr(&task->reader, fd, abi_ioc_recv, ioc->buff_rx, (size_t)4));

			/* init writer task */
			ABI_SUB_OP(abi_preinit_taskw(&task->writer, fd, abi_mem2fd_ioc_send, ioc->buff_tx));
		}

		/* init task auth info XXX task should fully operate before this */
		if(ioc->auth_init)
			(*ioc->auth_init)(task, ioc->auth_flags);
		break;
	}

	return tidx;
}

/** Add Reader fd to IOC
 *
\*/
int abi_ioc_add_fd_writer(
	io_ctrl_t	*ioc,
	abi_task_ior_f	*opr,
	int		fd,
	size_t 		buff_tx
)
{
	TRACEPNF(0, "==%s add %s handler:%5d %s max:%lu cur:%lu\n",
		ioc->rx_flow.name,
		"fd",
		fd,
		(ioc->max > ioc->cnt) ?"accepted" :"rejected",
		(unsigned long)ioc->max,
	       	(unsigned long)ioc->cnt
	);

	return abi_ioc_add_fd(ioc, opr, ABI_IO_FD_WRITER, fd, buff_tx);
}

/** Add Reader fd to IOC
 *
\*/
int abi_ioc_add_fd_reader(
	io_ctrl_t	*ioc,
	abi_task_ior_f	*opr,
	int		fd,
	size_t 		buff_rx
)
{
	TRACEPNF(0, "==%s add %s handler:%5d %s max:%lu cur:%lu\n",
		ioc->rx_flow.name,
		"fd",
		fd,
		(ioc->max > ioc->cnt) ?"accepted" :"rejected",
		(unsigned long)ioc->max,
	       	(unsigned long)ioc->cnt
	);

	int res = abi_ioc_add_fd(ioc, opr, ABI_IO_FD_READER, fd, buff_rx);
	if(res<0) {
		close(fd);
	}

	return res;
}

/** Add socket to IOC
 *
\*/
int abi_ioc_add_socket(
	io_ctrl_t		*ioc,
	int			s_in
)
{
	TRACEPNF(0, "==%s add %s handler:%5d %s max:%lu cur:%lu\n",
		ioc->rx_flow.name,
		"socket",
		s_in,
		(ioc->max > ioc->cnt) ?"accepted" :"rejected",
		(unsigned long)ioc->max,
	       	(unsigned long)ioc->cnt
	);

	int res = abi_ioc_add_fd(ioc, NULL, ABI_IO_TASK_SOCKET, s_in, 0);
	if(res>=0) {
		mcast_add_cl(ioc->ifs, s_in);		/* announce client info */
	}

	return res;
}

/** Accept incoming connection on listening socket
 *
\*/
int abi_ioc_accept_client(abi_io_tasks_t *tasks, uint8_t *data, size_t s)
{
	int			s_in;
	io_ctrl_t		*ioc  = tasks->ioc;

	abi_trace_ioc_state(ioc, __LINE__);

	ioc->party_size = sizeof(ioc->party);
	s_in = accept(tasks->reader.fd, &ioc->party, &ioc->party_size);
	if(s_in < 0) {
		ETRACEP("%s:%s:==Accept failed:Error code %d", ioc->rx_flow.name, __func__, s_in);

		return -errno;
	};

	int res = abi_ioc_add_fd(
		ioc,
		NULL,
		ABI_IO_TASK_SOCKET, s_in,
		(size_t)tasks->reader.flag
	);

	uint8_t *t;
	t = (typeof(t))&ioc->party.sin_addr.s_addr;
	TRACEP(0, "==%s CONNECTION from: %u.%u.%u.%u:%-5u %s max:%lu cur:%lu\n", ioc->rx_flow.name,
		t[0], t[1], t[2], t[3],
		ntohs(ioc->party.sin_port),
		(res>=0) ?"accepted" :"rejected",
		(unsigned long)ioc->max,
	       	(unsigned long)ioc->cnt
	);

	if(res>=0) {
		mcast_add_cl(ioc->ifs, s_in);		/* announce client info */
	} else {
		if(ioc->reject)
			(ioc->reject)(tasks, s_in);

		shutdown(s_in, SHUT_RDWR);
		close(s_in);
	}

	return 0;
}

/** Append Listen socket to IOC
 *
\*/
int abi_ioc_set_listen_sock(io_ctrl_t *ioc, int s)
{
	TRACE_LINE();

	/* init socket listener hook */
	int res = abi_ioc_add_fd(
		ioc,
		abi_ioc_accept_client,
		ABI_IO_LISTEN_SOCKET, s,
		0
	);
	if(res>=0) {
		ioc->tasks[res].reader.data  = ioc->priv;
	}

	return res;
}

/** Init I/O Controller ctx
 *
\*/
int abi_init_ioc(
	io_ctrl_t	*ioc,
       	unsigned	max_agents,
	void		*priv,
	void		*ifs,		/* interfaces handler */
	size_t		buff_rx,	/* rx buff size */
	size_t		buff_tx,	/* tx buff size */
	char		*name		/* subsystem name */
)
{
	memset(ioc, 0, sizeof(*ioc));	/* clear ioc structure */

	ioc->ifs = ifs;			/* set interfaces handler ctx ptr */

	/* allocate pollfds */
	ioc->fds   = calloc((size_t)max_agents, sizeof(struct pollfd));
	if(!ioc->fds)
		return -ENOMEM;

	/* allocate tasks */
	ioc->tasks = calloc((size_t)max_agents, sizeof(abi_io_tasks_t));
	if(!ioc->tasks)
		return -ENOMEM;

	/* init limits */
	ioc->max	   = max_agents;
	ioc->priv	   = priv;
	ioc->rx_flow.name  = name;

	ioc->buff_rx	   = buff_rx;
	ioc->buff_tx	   = buff_tx;

	/* Setup default incomming data processing function */
	ioc->rx_cb = &abi_rx_cb_stub_ex;

    	return 0;
}

int abi_free_ioc(io_ctrl_t *ioc)
{
	free(ioc->fds);
	free(ioc->tasks);

	return 0;
}

void abi_trace_ioc_state(io_ctrl_t *ioc, int line)
{
	int		i;

	TRACE(0, "@%4d_%4ld_%12s_______POLL|_____TASK|", line, (long)ioc->cnt, ioc->rx_flow.name);
	TRACE(0, "________________________________________________WRITER|");
	TRACE(0, "________________________________________________READER|");
	TRACE(0, "\n");
	TRACE_FLUSH();

	TRACE(0, "tsk| fd|  events|  rvents|flag    |fidx|tidx|");

	TRACE(0, "task buf| fd|max H| todo| done|state|      op|     abi|");
	TRACE(0, "task buf| fd|max H| todo| done|state|      op|     abi|");
	TRACE(0, "\n");
	TRACE_FLUSH();

	for(i=0; i<ioc->max; i++) {
		abi_io_tasks_t *ts = &ioc->tasks[i];

		TRACE(0, "%3d|%3d|%08x|%08x|%08"PRIx32"|%4d|%4d|",
			i,
			ioc->fds[i].fd,
			ioc->fds[i].events,
			ioc->fds[i].revents,
			ts->flags,
			ts->fidx, ts->tidx
		);

		TRACEF(0, abi_trace_task(&ts->writer));
		TRACEF(0, abi_trace_task(&ts->reader));

		TRACE(0, "\n");
		TRACE_FLUSH();
	}
}
int abi_io_on_timeout(io_ctrl_t *ioc)
{
	if(!ioc->tm)
		return 0;

	int		i;
	abi_io_tasks_t	*t;

	uint64_t	tm = *ioc->tm;

rescan:
	for(i=0; i<ioc->cnt; i++) {
		t = &ioc->tasks[ioc->tasks[i].tidx];

		if(!t->expires)
			continue;

		if(tm>t->expires) {
			abi_close_socket(ioc, i, __LINE__);
			goto rescan;
		}
	}

	return 0;
}

int abi_io_iterator(io_ctrl_t *ioc, int poll_timeout, unsigned reject_timeout)
{
	int		res;
	int		i;

	/* sleep in poll up to poll_timeout */
	res = poll(ioc->fds, ioc->cnt, poll_timeout);

	if(res<0) {
		if(errno == EINTR) {
			ETRACE("==while polling");
		} else {
			ETRACE("Terminiting due to\n");
		}
		return errno;
	}

	if(!res)	/* timeout */
		return abi_io_on_timeout(ioc);


	abi_io_tasks_t	*t;

	nfds_t max_try = ioc->cnt;
rescan:
	/* prevent cycling in rescan */
	if(!max_try)
		return errno;

	max_try--;
	for(i=0; i<ioc->cnt; i++) {
		struct pollfd	*fds = ioc->fds + i;
		t = &ioc->tasks[ioc->tasks[i].tidx];

		if((fds->revents & POLLOUT) && t->writer.opw) {
			if(abi_task_send(t, NULL, 0ul)) {
				abi_close_socket(ioc, i, __LINE__);
				goto rescan;
			}
		}

		if((fds->revents & POLLIN) && t->reader.opr) {
			if((*t->reader.opr)(t, NULL, 0ul)) {
				if(t->flags & ABI_IO_LISTEN_SOCKET) {
					usleep(reject_timeout);
					abi_trace_ioc_state(ioc, __LINE__);
					continue;
				}

				abi_close_socket(ioc, i, __LINE__);
				goto rescan;
			}

			if(t->flags & ABI_IO_LISTEN_SOCKET)
				abi_trace_ioc_state(ioc, __LINE__);
		}

		if(fds->revents & (POLLHUP|POLLERR)) {
			abi_close_socket(ioc, i, __LINE__);
			goto rescan;
		}
	}

	return errno;
}

int abi_ioc_append2all(
	io_ctrl_t	*ioc,
	int		flag,
	void		*data,
	size_t		size
)
{
	int i;
	for(i=0; i<ioc->max; i++) {
		abi_io_tasks_t	*d  = ioc->tasks+i;

		if(!(d->flags & ABI_IO_TASK_ACTIVE))
		 	continue;

		if(!(d->flags & (ABI_IO_TASK_SOCKET|ABI_IO_FD_WRITER)))
		 	continue;

		if(d->flags & ABI_IO_LISTEN_SOCKET)
		 	continue;

		if(!flag) {	/* send if for all */
			abi_task_send(d, data, size);	/* init send */
		 	continue;
		}

		/* skip if not for current target */
		if(!((int)d->writer.flag & flag))
		 	continue;

		/* skip if self events disabled */
		if(!(d->writer.flag & ABI_IO_EVENT_SELF) && (d == ioc->rx_task))
			continue;

		TRACEP(2, "[%d]\ttflags: %08x wflags:%08x flags:%08x sz:%zu\n",
			i,
			d->flags, d->writer.flag, flag,
			size
		);

		/* check if the task have multiple send callback */
		if(d->multiple != NULL) {

			int mtask = d->multiple(d, flag);
			if(mtask >= 0) {
				d = ioc->tasks + mtask;
				abi_task_send(d, data, size);	/* init send */
				/* break out from for(i=0; i<ioc->max; i++) */
				break;
			}

			continue;

		}

		abi_task_send(d, data, size);	/* init send */
	}

	return 0;
}

/** TRACE data_t as string value no more than 128 chars
 *
 *  /para prefix	dump prefix
 *  /para val		value descriptor to dump
 */
void abi_show_data_t_as_str(char* prefix, data_t *val)
{
	char s[129];
	s[128] = 0;

	snprintf(s, (size_t)val->size+1, "%s", val->data);

	TRACEPNF(0, "%s: [%3zu] \"%s\"\n", prefix, val->size, s);
}

/** TRACE abir_reader content as string value no more than 128 chars
 *
 *  /para prefix	dump prefix
 *  /para data		value ptr to dump
 *  /para size		value fit in container size
 *  /para tail		value not fit in container size
 */
void abir_show_as_str(char* prefix, uint8_t *data, size_t size, size_t tail)
{
	char s[129];
	s[128] = 0;

	if(data) {
		snprintf(s, size+1+tail, "%s", data);
	} else {
		sprintf(s, "(null)");
	}

	TRACEPNF(0, "%s: [%3zu:%3zu] \"%s\"\n", prefix, size, tail, s);
}

/** ABI XYZ interface receiver call back example
 *  1. parses data request
 *  2. process data request
 *  3. prepare to send the request result
 */
int abi_rx_cb_stub_ex(
	io_ctrl_t	*ioc,
	int		i)
{
	int res;

	TRACEPNF(2, "%s: IN: [%d] [%lu] bytes\n", ioc->rx_flow.name,
		i,
		ioc->rx_flow.max_size
	);

	ioc->rx_flags	= 0;				/* clear flags */

	res = ioc_auth_rx(ioc, ioc->rx_flow.name);
	if(res != 1) {
		return res;
	}

	if(ioc->rx_abi) {
		res = abir_read(ioc->rx_abi, 0);		/* parse data */
		if(res) {
			TRACEPNF(0, "%s abir_read res: %d errno: %d\n",
			   ioc->rx_flow.name, res, errno
			);

			return -errno;
		}
	}

	if((ioc->rx_flags & ABI_DATA_SKIP)) {		/* FIXME */
		ioc->rx_flags &= ~(typeof(ioc->rx_flags))ABI_DATA_ASYNC;
		return 0;
	}

	if(ioc->rx_flags & ABI_DATA_RESP) {
		abi_task_send(ioc->rx_task, NULL, 0ul);
	}

	return 0;
}
