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

/** libabi Writer PART
 *
 */

#include <inttypes.h>
#include <stdlib.h>
#include <netinet/in.h>

#ifndef TRACE_LEVEL
#define TRACE_LEVEL	0
#endif
#include <trace.h>

#include <abi/io.h>

abi_context_t *abiw_open(abi_context_t *abi, void *priv, abi_iow_f *write, size_t max)
{
	if(!abi) {
		abi = (abi_context_t*)malloc(sizeof(abi_context_t));

		if(!abi) {	/* check if malloc failed */
			return abi;
		}

		abi->nodes = NULL;
	}

	if(!abi->nodes) {
		/* Alloc Nodes */
		abi->nodes = (abi_node_t*)malloc((size_t)max * sizeof(abi_node_t));
		if(!abi->nodes) {	/* check if malloc failed */
			free(abi);

			return NULL;
		}

		/* Init state */
		abi->max = max;
		abi->last  = 0;
	}

	/* Init state */
	abi->size  = 0;
	abi->priv  = priv;
	abi->write = write;
	
	return abi;
}

void abiw_reopen(abi_context_t *abi)
{
	abi->last=0;
	
	return;
}

void abi_close(abi_context_t *abi)
{
	if(abi) {
		if(abi->nodes) {
			free(abi->nodes);
		}
		free(abi);
	}
	
	return;
}

/** Insert node after node "idx"
 *
 *  /para abi ABI helper functions context
 *  /para node node container to apend in data packet
 *  /para idx node to insers subnode into
 *  /returns -ABI_ERANGE on invalid value of idx 
 *  /returns -ABI_EFULL if to many nodes in data packet
 *  /returns idx of new node on success
 */
int abi_insert_node(abi_context_t *abi, int idx, abi_node_t *node)
{
	int16_t prev = -1;
	
	if(abi->last >= abi->max) {
		return -ABI_EFULL;
	}

	/* check bounds */
	if(idx < 0) {
		return -ABI_ERANGE;
	}
       
	if(idx >= abi->last) {
		return -ABI_ERANGE;
	}

	/* insert data */
	prev = abi->nodes[idx].neig;
	abi->nodes[idx].neig = (int16_t)abi->last;	/* neig points to new */

	abi->nodes[abi->last] = *node;
	
	abi->nodes[abi->last].next = -1;
	abi->nodes[abi->last].neig = prev;
		
	TRACE(2, "new node inserted @%d\n", abi->last);
	abi->last++;
	
	return abi->last-1;
}

/** Append node after last appended one
 *
 *  /para abi ABI helper functions context
 *  /para node node container to apend in data packet
 *  /para idx node to insers subnode into
 *  /returns -ABI_ERANGE on invalid value of idx 
 *  /returns -ABI_EFULL if to many nodes in data packet
 *  /returns idx of new node on success
 */
int abi_append_node(abi_context_t *abi, abi_node_t *node)
{
	if(abi->last >= abi->max) {
		return -ABI_EFULL;
	}
	
	abi->nodes[abi->last] = *node;
	
	abi->nodes[abi->last].next = -1;
	abi->nodes[abi->last].neig = -1;

	if(abi->last>0) {
		abi->nodes[abi->last-1].neig = (int16_t)abi->last;
	}
		
	TRACE(2, "new node appended @%d\n", abi->last);
	abi->last++;
	
	return abi->last-1;
}

/** Append Subnode into node "idx"
 *
 *  Caller should take care about correct idx parameter
 *  /para abi ABI helper functions context
 *  /para idx node to insers subnode into
 *  /para node node container to apend in data packet
 *  /returns -ABI_ERANGE on invalid value of idx 
 *  /returns -ABI_EFULL if to many nodes in data packet
 *  /returns idx of new node on success
 */
int abi_append_subnode(abi_context_t *abi, int idx, abi_node_t *node)
{
	if(abi->last >= abi->max) {
		return -ABI_EFULL;
	}
	
	/* Check bpunds */
	if(idx>=abi->last) {
		return -ABI_ERANGE;
	}

	if(idx<0) {
		idx = abi->last-1;
		if(idx < 0) {
			return -ABI_ERANGE;
		}
	}
#if 0
	if(abi->nodes[idx].next>=0) {	/* Already exists */
		return -2;
	}
#endif
	int16_t	prev = abi->nodes[idx].next;

	abi->nodes[idx].next = (int16_t)abi->last;

	abi->nodes[abi->last] = *node;
	
	abi->nodes[abi->last].next = prev;
	abi->nodes[abi->last].neig = -1;
		
	abi->last++;
	return abi->last-1;
}

/** returns size header
 *
 */
uint32_t abiw_eval_ssize(uint32_t size, uint32_t *res)
{
	uint32_t t = (size>0x007f? 2U:1U) + (size>0x3fff? 2U:0U);
	
	switch(t) {
	    case 1:
		size = 0x00000000|(size<<24);
		break;
	    case 2:
		size = 0x80000000|(size<<16);
		break;
	    case 4:
		size = 0xc0000000|(size<< 0);
		break;
	}

	*res = htonl(size);	
	
	return t;
}

/** Calculate PACKET size starting from node "idx"
 *
 *  Caller should take care about correct idx parameter
 *  /para abi ABI helper functions context
 *  /para idx nodoe index to proces from
 */

int abiw_get_node_sizes(abi_context_t *abi, int idx)
{
	abi_node_t	*node;

	node = &abi->nodes[idx];
	TRACE(3, "[%2d]-NS: %08"PRIx32"h %8"PRIu32" TS: %08"PRIx32"h %8"PRIu32"\n",
	    idx,
	    node->size, node->size,
	    abi->size, abi->size
	);

	do {
		node = &abi->nodes[idx];

		abi->size += abiw_ajust_size(node->key.size);

		/* if has subnodes */
		if(node->next>=0) {	/* subnode exists */
			node->size = -abi->size;	/* init node val size */
		
			abiw_get_node_sizes(abi, node->next);

			node->size += abi->size;	/* calculate node val size */
			abi->size += abiw_ajust_size(node->size) - node->size;
		} else {	/* no subnode */
			/* save val size */
			abi->size += abiw_ajust_size(node->val.size);
		}

		TRACE(2, "[%2d] NS: %08"PRIx32"h %8"PRIi32" TS: %08"PRIx32"h %8"PRIi32"\n",
		    idx,
		    node->size, node->size,
		    abi->size, abi->size
		);
		idx = node->neig;
	} while(idx>=0);

	return 0;
}

int abiw_write_recursive(abi_context_t *abi, int idx)
{
	abi_node_t	*node;

	uint32_t	tlen;
	uint32_t	stlen;

	do {
		node = &abi->nodes[idx];

		/* write key size */
		stlen = abiw_eval_ssize(node->key.size, &tlen);
		abi->write(abi->priv, (uint8_t*)&tlen, (size_t)stlen);
		
		/* write key data */
		abi->write(abi->priv, node->key.data, (size_t)node->key.size);

		if(node->next>=0) {
			/* write node val size */
			stlen = abiw_eval_ssize(node->size, &tlen);
			abi->write(abi->priv, (uint8_t*)&tlen, (size_t)stlen);
		
			/* write node val data */
			abiw_write_recursive(abi, node->next);
		} else {
			/* write data */
			stlen = abiw_eval_ssize(node->val.size, &tlen);
			abi->write(abi->priv, (uint8_t*)&tlen, (size_t)stlen);

			if(node->write) {
				void *priv = node->priv ?node->priv :abi->priv;

				node->write(priv, node->val.data, (size_t)node->val.size);
			} else {
				abi->write(abi->priv, node->val.data, (size_t)node->val.size);
			}
		}

		idx = node->neig;
	} while(idx>=0);

	return 0;
}

/** Write ABI packet to task
 *
 *  /para abi ABI task helper context
 *  /para flag whether write packet size before data packet
 *  /returns non zero if failed 
 *  /returns zero on success
 */
int abiw_write_task(abi_io_task_t *tw, int flag)
{
	tw->status = 0;
	tw->todo   = 0;
	tw->done   = 0;

	return abiw_write(tw->abi, flag);
}


/** Calc size for ABI packet
 *
 *  /para abi ABI helper functions context
 *  /para size pointer so save ABI packet data size
 *  /para flag whether write packet size before data packet
 *  /returns non zero if failed 
 *  /returns zero on success
 */
int abiw_write_pre(abi_context_t *abi, uint32_t *size, int flag)
{
	if(!abi->last) {
		return -ABI_EZERO_NODES;
	}
	
	abi->size = 0;

	abiw_get_node_sizes(abi, 0);

	if(!size)
		return 0;

	if(flag)
	    *size = abi->size + sizeof(uint32_t);
	else
	    *size = abi->size; 

	return 0;
}

/** Do Write ABI packet
 *
 *  /para abi ABI helper functions context
 *  /para flag whether write packet size before data packet
 *  /returns non zero if failed 
 *  /returns zero on success
 */
int abiw_write_do(abi_context_t *abi, int flag)
{
	if(flag) {
		uint32_t	tlen;

		/* write key size */
		tlen = htonl(abi->size);
		abi->write(abi->priv, (uint8_t*)&tlen, sizeof(tlen));
	}

	abiw_write_recursive(abi, 0);
	return 0;
}

/** Write ABI packet
 *
 *  /para abi ABI helper functions context
 *  /para flag whether write packet size before data packet
 *  /returns non zero if failed 
 *  /returns zero on success
 */
int abiw_write(abi_context_t *abi, int flag)
{
	if(!abi->last) {
		return -ABI_EZERO_NODES;
	}
	
	abi->size = 0;

	abiw_get_node_sizes(abi, 0);

	if(flag) {
		uint32_t	tlen;

		/* write key size */
		tlen = htonl(abi->size);
		abi->write(abi->priv, (uint8_t*)&tlen, sizeof(tlen));
	}

	abiw_write_recursive(abi, 0);

	return 0;
}

/** Dump node structure info
 *
 *  /para abi ABI helper functions context
 *  /returns non zero if failed 
 *  /returns zero on success
 */
int abiw_dump(abi_context_t *abi)
{
	abi_node_t	*node;
	int		i;
	
	TRACE(0, "%s: Nodes: %2d Size: %4"PRIx32" %5"PRIu32" match: %d\n", __func__,
		abi->last,
		abi->size,
		abi->size,
		abi->match
	);

	TRACE(0, "%s: id next neig match level ksize nsizeh|    vsize    vbuff|key\n",
		__func__
	);

	for(i=0; i<abi->last; i++) {
		int j;

		node = &abi->nodes[i];

		TRACE(0, "%s: %2d %4d %4d %5d %5d %5zd %5"PRIx32"h|%8zxh@%08lx|",
		        __func__,
		        i,
		        node->next,
		        node->neig,
		        node->match,
		        node->level,
		        node->key.size,
		        node->size,
		       	node->val.size,
		       	(unsigned long)node->val.data
		);

		for(j=0; (j<40) && (j<node->key.size); j++) {
			TRACE(0, " %02x", node->key.data[j]);
		}
		    
		TRACE(0, "|\n");
	}

	return 0;
}
