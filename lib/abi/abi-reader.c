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

/** libabi Reader PART
 *
 */

#include <inttypes.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#ifndef TRACE_LEVEL
#define TRACE_LEVEL	0
#endif
#include <trace.h>

#include <abi/io.h>

/** Receives size of data_t
 * Returns size of data_t container using MSB of firsct size val octet
 */
static __inline__ uint32_t abir_get_ssize(
        abi_context_t *abir,
        uint32_t      *ssize
)
{
        abir_ssize_t    d;

        abir->read(abir->priv, &d.s8.v0, 1ul);

        switch(d.s8.v0>>6) {
            case 0:
            case 1:
                d.v = d.s8.v0;
                *ssize = 1;
                break;

            case 2:
                abir->read(abir->priv, &d.s8.v1, 1ul);
                d.v = ntohs(d.s16.v0) & 0x3fff;
                *ssize = 2;
                break;

            case 3:
            default:
                abir->read(abir->priv, &d.s8.v1, 3ul);
                d.v = ntohl(d.v)  & 0x3fffffff;
                *ssize = 4;
                break;
        }

        return d.v;
}

abi_context_t *abir_open(
	abi_context_t	*abi, 
	void		*priv, 
	abi_io_f	*read, 
	abir_skiper_f	*skip, 
	size_t		max
)
{
	if(!abi) {
		abi = (abi_context_t*)malloc(sizeof(abi_context_t));
		if(!abi) {	/* check if malloc failed */
			return abi;
		}

		/* Alloc Nodes */
		abi->nodes = (abi_node_t*)malloc(max * sizeof(abi_node_t));
		if(!abi->nodes) {	/* check if malloc failed */
			free(abi);

			return NULL;
		}

		/* Init state */
		abi->max   = max;
		abi->last  = 0;
	}

	/* Init state */
	abi->priv  = priv;
	abi->read  = read;
	abi->skip  = skip;
	
	return abi;
}

#if 0
void abir_reopen(abi_context_t *abi)
{
	abi->last=0;
	
	return;
}
#endif
/** scans packet id keys
 * Assumes that all nodes linked with node->match has
 * size wich EQ to todo
 * */
int abir_scan_key_part(
	abi_context_t	*abi,
	uint8_t		*key,	/* key part data	*/
	unsigned	pos,	/* key part offset	*/
	unsigned	plen	/* key part len		*/
)
{
	int		idx  = abi->match;
	int		prev = abi->match;

	int		res;
	
	abi_node_t	*node;

	while(idx>=0) {
		node = &abi->nodes[idx];

		res = memcmp(node->key.data+pos, key, (size_t)plen);

		if(likely(res)) {
			int_least16_t next = node->match;

			if(unlikely(idx == abi->match)) {	/* if first */
				abi->match = next;
			} else {
				abi->nodes[prev].match = next;
			}

			idx = next;
			prev = next;
			TRACE(8, "+i=%3d p=%3d m=%3d\n", idx, prev, abi->match);	
			continue;
		} 

		prev = idx;
		idx  = node->match;
		TRACE(8, "-i=%3d p=%3d m=%3d\n", idx, prev, abi->match);	
	}

	return abi->match;
}

/** 1. Make list of nodes which match requested size
 *  2. Append wildcard node if any to matched list
 */
static int abir_link_match_size_branch(abi_context_t *abi, int idx, uint32_t todo)
{
	int		prev;
	abi_node_t	*node;

	/* init prev & match */
	prev		= -1;
	abi->match	= -1;

	/* create list */
	while(idx>=0) {
		node = &abi->nodes[idx];

		if(node->key.size == todo) {
			if(prev >= 0) {
				abi->nodes[prev].match = (int16_t)idx;
			} else {
				abi->match = idx;
			}

			prev = idx;
		}

		idx  = node->neig;
	}

	if(abi->match>=0) {
		abi->nodes[prev].match = -1;
	}
	
	return abi->match;
}

/* Key cache should contain the integer number of cache lines */
#define ABIR_KEY_CACHE_SIZE	(0x20)

/* process subnodes */
static uint32_t abir_process_node(
	abi_context_t	*abi,
	int		branch
)
{
       	uint32_t	tail;
       	uint32_t	todo;
       	uint32_t	done;
       	uint32_t	ksize;
       	uint32_t	vsize;

	abi_node_t	*node;

	TRACE(4, "%s:%d branch:%d\n", __func__, __LINE__, branch);

	todo = abir_get_ssize(abi, &ksize);	/* get KEY size */
	ksize += todo;

	/* create size match linked list */
	TRACEF(7, abir_dump(abi));
	abir_link_match_size_branch(abi, branch, todo);
	TRACEF(7, abir_dump(abi));

	done = 0;

	/* search for KEY */
	while((todo>done) && (abi->match>=0)) {
		uint8_t		key[ABIR_KEY_CACHE_SIZE];	/* key container */

		tail = todo - done;
		tail = (ABIR_KEY_CACHE_SIZE>tail) ?tail : ABIR_KEY_CACHE_SIZE;

		if(abi->read(abi->priv, key, (size_t)tail) < 0) {
			abi->error = 1;
			return todo;
		}

		abir_scan_key_part(abi, key, done, tail);
		TRACEF(7, abir_dump(abi));
		
		done += tail;
	}

	if(abi->match<0) {
		/* skip rest KEY value */
		if(todo != done) {
			if(abi->skip(abi->priv, (size_t)todo - (size_t)done) < 0) {
				abi->error = 1;
				return todo;
			}
		}

		/* skip VAL value */
		todo = abir_get_ssize(abi, &vsize);	/* get val size */
		vsize += todo;				/* ajust val size */

		TRACE(4, "%s:%d ret:%"PRIi32"\n", __func__, __LINE__, ksize + vsize);
		if(abi->skip(abi->priv, (size_t)todo) < 0) {
			abi->error = 1;
			return  ksize + vsize;
		}

		return ksize + vsize;			/* consumed data */
	}

	node = &abi->nodes[abi->match];

	/* scan key attached value */
	todo = abir_get_ssize(abi, &vsize);	/* get VAL size */
	vsize += todo;				/* ajust val size */

//	abir_dump(abi);

	tail = (todo > node->val.size) ?todo - node->val.size :0;

	node->size = todo;	/* Save remote size */

	/* If node has subnodes process them */
	if(node->next>=0) {
		done = 0;

		/* process node subnodes recursively */
		while(todo>done) {
			done += abir_process_node(abi, node->next);
			TRACE(4, "%s:%d todo:%"PRIu32" done:%"PRIu32"\n", __func__, __LINE__, todo, done);

			if(abi->error)
				return todo;
		}

		goto node_done;
	}
	
	todo -= tail;	/* calculate container size */

	/* decode one level deep branch structure */
	if(node->rval) {
	    	void *priv;
	       
		priv = node->priv ?node->priv :abi->priv;
		if(node->rval(priv, node->val.data, (size_t)todo, (size_t)tail) < 0) {
			abi->error = 1;
			return ksize + vsize;
		}
	} else {
		if(abi->read(abi->priv, node->val.data, (size_t)todo) < 0) {
			abi->error = 1;
			return ksize + vsize;
		}
		
		if(tail) {	/* read data tail */ 
			if(abi->skip(abi->priv, (size_t)tail) < 0) {
				abi->error = 1;
				return ksize + vsize;
			}
		}
	}

node_done:
	if(node->action) {
		node->action(abi, node);
	}

	TRACE(4, "%s:%d ret:%"PRIu32"\n", __func__, __LINE__, ksize + vsize);
	return ksize + vsize;
}

/** Read & parse PACKET
 *
 *  Caller should take care about correct input nodes structure
 *  /para flag
 *  /para idx
 */
int abir_read_size(abi_context_t *abi, uint32_t	size)
{
	uint32_t	done = 0;	/* processed size */

	if(!abi->last) {
		return -ABI_EZERO_NODES;
	}

	abi->error = 0;		/* clear error */

	abi->size = size;	/* keep total size */

	while(abi->size > done) {
		done += abir_process_node(abi, 0);
		if(abi->error)
			return -ABI_EIO;
	}
	
	if(abi->size != done) {	/* check total size */
		return -ABI_EPARSE;
	}

	if(abi->error)
		return -ABI_EIO;

	return -ABI_OK;
}

/** Read & parse PACKET
 *
 *  Caller should take care about correct input nodes structure
 *  /para flag
 *  /para idx
 */
int abir_read(abi_context_t *abi, int flag)
{
	uint32_t	done = 0;	/* processed size */
	uint32_t	size = 0;

	if(!abi->last) {
		return -ABI_EZERO_NODES;
	}

	if(flag) {
		abi->status = abi->read(abi->priv, (uint8_t*)&size, sizeof(size));
		abi->size   = ntohl(size);        /* keep total size */

		if(abi->status<=0) {
			return -ABI_EOID;
		}

		return abir_read_size(abi, abi->size);
	}
	
	abi->error = 0;	/* clear error */

	done = abir_process_node(abi, 0);
	abi->size = done;

	if(abi->error)
		return -ABI_EIO;

	return -ABI_OK;
}

int abir_dump(abi_context_t *abi)
{
	abi_node_t	*node;
	int		i;
	
	TRACE(0, "%s: Nodes: %2d Size: %4"PRIx32" %5"PRIu32" match: %d\n", __func__,
		abi->last,
		abi->size,
		abi->size,
		abi->match
	);

	TRACE(0, "%s: id next neig match level ksize vsize key\n",
		    __func__);
	for(i=0; i<abi->last; i++) {
		int j;

		node = &abi->nodes[i];

		TRACE(0, "%s: %2d %4d %4d %5d %5d %5zd %5zd [",
		    __func__,
		    i,
		    node->next,
		    node->neig,
		    node->match,
		    node->level,
		    node->key.size,
		    node->val.size
		);

		for(j=0; (j<40) && (j<node->key.size); j++) {
			TRACE(0, " %02x", node->key.data[j]);
		}
		    
		TRACE(0, " ]\n");
	}

	return 0;
}
