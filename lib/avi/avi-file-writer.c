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

/* libavi usage example */
//#define _GNU_SOURCE
#include <inttypes.h>
#include <sys/types.h>
#include <sys/unistd.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

#include <time.h>
#include <sys/stat.h>
#include <sys/uio.h>

#include <asm/byteorder.h>

//#include <os-helpers/signals.h>
//#include <os-helpers/fops.h>
#include "avi/fourcc.h"
#include "avi/structures.h"
#include "avi/avi.h"

#include <trace.h>

/**\
 *  Usage Case:
 *
 *  h = avi_rwh_t *avi_file_open("my.avi");
 *  avi_file_list_open(h, fccRIFF, fcc_AVI);
 *      avi_file_list_open(h, fccLIST, fcc_hdrl);
 *          avi_file_chunk_append(h,fcc_avih, &AVI_avih_data, sizeof(AVI_avih));
 *        (
 *          avi_file_list_open(h, fccLIST, fcc_strl);
 *		avi_file_chunk_append(h,fcc_strh, &AVI_strh_data, sizeof(AVI_strh));
 *		avi_file_chunk_append(h,fcc_strf, &AVI_strf_data, sizeof(AVI_strf));
 *	      [ avi_file_chunk_append(h,fcc_indx, &AVI_indx_data, sizeof(AVI_indx)); ]
 *          avi_file_list_close(h); // fcc_strl
 *        )+
 *        (
 *          avi_file_list_open(h, fccLIST, fcc_odml);
 *		avi_file_chunk_append(h,fcc_dmlh, &AVI_dmlh_data, sizeof(AVI_dmlh));
 *          avi_file_list_close(h); // fcc_strl
 *        )?
 *      avi_file_list_close(h); // fcc_hdrl
 *      avi_file_list_open(h, fccLIST, fcc_movi);
 *        ( avi_file_chunk_append(h,fcc_ix(flow), &AVI_ix(flow), size(AVI_ix(flow))); )*
 *        (
 *         (
 *	    avi_file_chunk_append(h,fcc_db(flow), &Xdb_data, size(Xdb_data)); |
 *	    avi_file_chunk_append(h,fcc_dc(flow), &Xdc_data, size(Xdc_data)); |
 *	    avi_file_chunk_append(h,fcc_wb(flow), &Xwb_data, size(Xwb_data)); |
 *	    avi_file_chunk_append(h,fcc_wb(flow), &Xpc_data, size(Xpc_data)); |
 *	    avi_file_chunk_append(h,fccJUNK, &NULL_data, pad)); |
 *	    ( )
 *	   )
 *        )+
 *        (
 *	    avi_file_chunk_append(h,fcc_strh, &AVI_strh_data, sizeof(AVI_strh)); |
 *        )?
 *      avi_file_list_close(h); // fcc_movi
 *	avi_file_chunk_append(h,fcc_idx1, &AVI_idx1_data, sizeof(AVI_idx1)*frames_count);
 *  avi_file_list_close(h); // fccRIFF
 *  avi_file_close(h)
 *
\*/

/** Creates new RIFF file handler
 *  Allocates Fill data structures for RIFF file handler
 *  
 *  \param name RIFF file path 
 *
 *  \returns 0 on Failure, !0 avi_rwh_t* on Success
\*/
avi_rwh_t *avi_file_open(char *name)
{
	avi_rwh_t *a;

	a = malloc(sizeof(avi_rwh_t));
	if(!a) {
		return NULL;
	}

	/* init structure */
	memset(a, 0, sizeof(*a));
	a->sync_len = 0x7fffffff;
	a->max_idx = sizeof(a->l)/sizeof(a->l[0]) - 1;	/* last entry reserved for partial ops */
	    
	a->fd = open(name, O_RDWR|O_CREAT|O_TRUNC, S_IRGRP|S_IWGRP|S_IRUSR|S_IWUSR|S_IWOTH|S_IROTH);

	return a;
}

/** Reopens new RIFF file handler
 *  Allocates Fill data structures for RIFF file handler
 *  
 *  \param name RIFF file path 
 *
 *  \returns 0 on Failure, !0 avi_rwh_t* on Success
\*/
avi_rwh_t *avi_file_reopen(char *name, avi_rwh_t *a)
{
	if(!a) {
		a = malloc(sizeof(avi_rwh_t));
		if(!a) {
			return NULL;
		}
	}

	/* init structure */
	memset(a, 0, sizeof(*a));
	a->sync_len = 0x7fffffff;
	a->max_idx = sizeof(a->l)/sizeof(a->l[0]) - 1;	/* last entry reserved for partial ops */
	    
	a->fd = open(name, O_RDWR|O_CREAT|O_TRUNC|O_EXCL, S_IRGRP|S_IRGRP|S_IRUSR|S_IRUSR|S_IROTH);

	return a;
}

/** Estimate new a->size
 *  1. Aligns forward a->size by uint16_t
 *  2. clear "align position needed" flag
 *  3. seeks to a->size
 *  
 *  \param a RIFF file context 
 *  \param size a->size increment
\*/
void avi_ajust_size_and_seek(avi_rwh_t *a, uint32_t size)
{
	a->size += size;
	a->size += a->size & 1;
	a->align = 0;

	lseek(a->fd, (off_t)a->size, SEEK_SET);
}

/** Estimate new a->size
 *  1. Aligns forward a->size by uint16_t
 *  2. Estimate "align position needed" flag
 *  
 *  \param a RIFF file context 
 *  \param size a->size increment
\*/
void avi_ajust_by_size(avi_rwh_t *a, uint32_t size)
{
	a->size += size + sizeof(uint32_t)*2;
	a->align = a->size & 1;
	a->size += a->align;
}

/** Set aligned position in file if needed
 *  
 *  \param a RIFF file context 
\*/
void avi_align_pos(avi_rwh_t *a)
{
	if(!a->align)
		return;

	/* seek to end of last LIST */
	lseek(a->fd, (off_t)a->size, SEEK_SET);
	a->align = 0;
}
/** Update last data entry size
 *  1. Dumps correct size for data entry
 *
 *  2. Aligns a->size by forward to even
 *  
 *  \param a RIFF file context 
 *
 *  \returns 0 on Success, !0 on Failure
\*/
int avi_file_data_last(avi_rwh_t *a)
{
	if(!a->partial) {
		return -1;
	}

	a->partial = 0;

	avi_llist_t	*c = a->l + a->idx;	/* get & to current list */
	c->type = __cpu_to_le32((uint32_t)(a->size - c->offs - 2*sizeof(uint32_t)));

	/* TODO error handling */

	/* Dump chunk header to target at its pos */
	lseek(a->fd, (off_t)c->offs, SEEK_SET);
	write(a->fd, (char*)&c->name, 2*sizeof(uint32_t));

	/*  Seek to file end, keep it uint16_t aligned */
	avi_ajust_size_and_seek(a, 0);

	return 0;
}

/** Appends data to chunk entry in RIFF file
 *  Dumps chunk header and data to file inside last opened LIST entry or
 *  at the end of file if no one LIST currently open actual data headers
 *  write perfomed by avi_file_list_close 
 *  
 *  \param a RIFF file context 
 *  \param fcc CHUNK name fourcc
 *  \param data CHUNK data pointer
 *  \param size CHUNK data size
 *
 *  \returns 0 on Success, -1 on write error
\*/
int avi_file_data_append(avi_rwh_t *a, uint8_t *data, uint32_t size)
{
	if(!a->partial) {
		return -1;
	}

	write(a->fd, data, size);

	/* ajust file size */
	a->size += size;

	if(a->size - a->sync_pos > a->sync_len) {
		fdatasync(a->fd);
		a->sync_pos = a->size;
	}

	return 0;
}

/** Creates new data chunk entry of RIFF file for sequental write
 *  Fill data structures for new RIFF file chunk entry
 *  actual data headers write perfomed by avi_file_data_last
 *  also imlied by functions:
 *  avi_file_data_create, avi_file_list_close, avi_file_list_create,
 *  avi_file_chunk_append, avi_file_close
 *  
 *  \param a RIFF file context 
 *  \param fcc LIST section name: fccRIFF | fccRIFF
 *  \param type LIST section type
 *
 *  \returns 0 on Success, -1 if LIST nested more than 10 times
\*/
int avi_file_data_create(avi_rwh_t *a, uint32_t name)
{
	avi_file_data_last(a);			/* close data chunk */
	a->partial = 1;

	avi_llist_t	*c = a->l + a->idx;	/* get & to current list */

	/* save offs to data chunk header */
	c->offs = a->size;
	c->name = name;

	/* keep size pointing to data to write */
	a->size += 2*sizeof(uint32_t);
	lseek(a->fd, (off_t)a->size, SEEK_SET);

	return 0;
}

/** Creates new LIST entry of RIFF file
 *  Fill data structures for new RIFF file LIST entry
 *  actual data headers write perfomed by avi_file_list_close
 *  and implied by avi_file_close
 *  
 *  \param a RIFF file context 
 *  \param fcc LIST section name: fccRIFF | fccRIFF
 *  \param type LIST section type
 *
 *  \returns 0 on Success, -1 if LIST nested more than 10 times
\*/
int avi_file_list_open(avi_rwh_t *a, uint32_t fcc, uint32_t type)
{
	if(a->idx >= a->max_idx) {
		return -1;
	}

	avi_file_data_last(a);			/* close data chunk */

	avi_llist_t	*c = a->l + a->idx;	/* get & to current list */

	/* keep new LIST entry info */
	c->name = fcc;
	c->type = type;
	c->offs = a->size;

	a->idx++;

	/* write LIST sutable for MPLAYER HQ */
	avi_list_t		l;

	l.name.v32 = fcc;
	l.size     = 0;

	l.type.v32 = type;

	/* write meta */
	avi_align_pos(a);		/* Align file pos if needed */
	write(a->fd, (char*)&l, sizeof(avi_list_t));

	a->size += sizeof(avi_list_t);

	if((fcc == fccLIST) && (type == fcc_movi))	/* Save movi offset */
		a->movi_offs = a->size-4;

	return 0;
}

/** Appends JUNK data chunk to RIFF file to align next entry
 *  Writes JUNK chunk header and aligns to specified boundary
 *  
 *  \param a RIFF file context 
 *  \param alignment desired next CHUNK alignment should be power of 2
 *
 *  \returns 0 on Success, -1 on write error
\*/
int avi_file_chunk_align(avi_rwh_t *a, uint32_t alignment)
{
	avi_list_t		l;
	uint32_t	tail;
       
	avi_file_data_last(a);			/* close data chunk */

	avi_align_pos(a);		/* seek to end of last chunk/LIST */

	/* evaluate tail */
	tail = a->size & (alignment-1);
	if(!tail) {
		return 0;
	}

	/* re evaluate tail */
	tail  = (a->size+sizeof(uint32_t)*2) & (alignment-1);
	alignment -= tail;
		
	l.name.v32 = fccJUNK;
	l.size = __cpu_to_le32(alignment);

	/* TODO error handling */

	/* Dump chunk HEADER after a->size octets */
	write(a->fd, (char*)&l, sizeof(uint32_t)*2);

	/* move a->size to data end */
	avi_ajust_size_and_seek(a, sizeof(uint32_t)*2 + alignment);

	return 0;
}
/** Appends data chunk to RIFF file
 *  Dumps chunk header and data to file inside last opened LIST entry or
 *  at the end of file if no one LIST currently open actual data headers
 *  write perfomed by avi_file_list_close 
 *  
 *  \param a RIFF file context 
 *  \param fcc CHUNK name fourcc
 *  \param data CHUNK data pointer
 *  \param size CHUNK data size
 *
 *  \returns 0 on Success, -1 on write error
\*/
int avi_file_chunk_append(avi_rwh_t *a, uint32_t fcc, uint8_t *data, uint32_t size)
{
	avi_list_t	l;
      
	TRACE(8, "AVI %08"PRIx32":%c %s", size, '0' + (int)(size&1), __func__);
	TRACE(8, " CHNAME:%c%c%c%c(%08"PRIx32")", ((char*)(&fcc))[0], ((char*)(&fcc))[1], ((char*)(&fcc))[2], ((char*)(&fcc))[3], fcc);

	avi_file_data_last(a);			/* close data chunk */
	avi_align_pos(a);		/* seek to end of last chunk/LIST */
	
	l.name.v32 = fcc;
	l.size = __cpu_to_le32(size);

	struct iovec {
              void *iov_base;   /* Starting address */
              size_t iov_len;   /* Number of bytes */
        };

	struct iovec io[2] = {
	    { &l,   8 },
	    { data, size }
	};

	/* TODO error handling */

	/* Dump data to target after a->size octets */
	writev(a->fd, (void*)io, 2);	// XXX FIXME

	/*  ajust file size, keep it uint16_t aligned */
	avi_ajust_by_size(a, size); 

	return 0;
}

/** Appends fake data chunk to RIFF file
 *  Seeks chunk header and data in file inside last opened LIST entry or
 *  at the end of file if no one LIST currently open actual data headers
 *  write perfomed by avi_file_list_close 
 *  
 *  \param a RIFF file context 
 *  \param size CHUNK data size
 *
 *  \returns 0 on Success, -1 on write error
\*/
int avi_file_chunk_append_fake(avi_rwh_t *a, uint32_t size)
{
	TRACE(8, "AVI %08"PRIx32":%c %s", size, '0' + (int)(size&1), __func__);

	avi_file_data_last(a);			/* close data chunk */
	
	/*  Seek to file end, keep it uint16_t aligned */
	avi_ajust_size_and_seek(a, size + sizeof(uint32_t)*2); 
	
	return 0;
}


/** Close last LIST entry
 *  Dumps all meta of last opened LIST
 *  
 *  \param a RIFF file context 
 *
 *  \returns 0 on Success, !0 on Failure
\*/
int avi_file_list_close(avi_rwh_t *a)
{
	if(a->idx <= 0) {
		return -1;
	}

	avi_file_data_last(a);			/* close data chunk */

	/* TODO error handling */
	a->idx--;

	avi_llist_t	*c = a->l + a->idx;	/* get & to current list */
	avi_list_t		l;

	l.name.v32 = c->name;
	l.size = __cpu_to_le32((uint32_t)(a->size - c->offs - 2*sizeof(uint32_t)));

	l.type.v32 = c->type;

	/* write meta */	
	lseek(a->fd, (off_t)c->offs, SEEK_SET);
	write(a->fd, (char*)&l, sizeof(avi_list_t));

	/* seek to end of last LIST */
	lseek(a->fd, (off_t)a->size, SEEK_SET);
	a->align = 0;
	
	return 0;
}

/** Close RIFF file
 *  1. Dumps all meta of non closed LISTs
 *
 *  2. Closes RIFF file descriptor
 *
 *  3. Free context 
 *  
 *  \param a RIFF file context 
 *
 *  \returns 0 on Success, !0 on Failure
\*/
int avi_file_close(avi_rwh_t *a)
{
	avi_file_data_last(a);			/* close data chunk */

	while(a->idx) {
	    avi_file_list_close(a);
	}

	close(a->fd);
	free(a);
	    
	return 0;
}

/** Close RIFF file
 *  1. Dumps all meta of non closed LISTs
 *  2. Closes RIFF file descriptor
 *
 *  \param a RIFF file context 
 *
 *  \returns 0 on Success, !0 on Failure
\*/
int avi_file_pre_close(avi_rwh_t *a)
{
	avi_file_data_last(a);			/* close data chunk */

	while(a->idx) {
	    avi_file_list_close(a);
	}

	close(a->fd);

	return 0;
}

/** Write noninterlaced audio frame to AVI file
 *  1. Dumps frame data to avi file
 *  2. Modify index cache
 *
 *  \param avi AVI file context 
 *  \param d pointer to audio frame data 
 *  \param size audio frame size 
 *
 *  \returns 0 on Success, 1 on Index overflow
\*/
int avif_write_audio(avi_hdlr_t *avi, uint8_t *d, unsigned size)
{
	if(avi->idx->cb >= avi->max_frames) {
		return 1;
	}

	AVI_idx1c	*idx = avi->idx->index + avi->idx->cb;
	avi_rwh_t	*w   = avi->w;

	/* Init index entry */
	idx->dwChunkId = fcc_01wb;
	idx->dwFlags   = __cpu_to_le32(AVIIF_KEYFRAME);

	/* Offset from RIFF:"AVI "/`movi` start */
	idx->dwOffset  = __cpu_to_le32((uint32_t)w->size - w->movi_offs);
	idx->dwSize    = size;

	avi->idx->cb++;
	avi->idx->fcc = 0;	/* Set next field num to write */
             
	/* Write video data */ 
        avi_file_chunk_append(w, fcc_01wb, d, size);

	return 0;
}


/******************************************************
 *
 * ODML AVI 2.0 part
 *
 *****************************************************/

/* ODML implementation		*/
int avif_odml_write_2fields(
	avi_hdlr_t *avi,
       	uint8_t *d0,
       	unsigned size0,
       	uint8_t *d1,
       	unsigned size1
)
{
	unsigned size = size0 + size1;
	avi_rwh_t	*w   = avi->w;
#if 0
	if(avi->idx->cb >= avi->max_frames) {
		return 1;
	}

	
	AVI_idx1c	*idx = avi->idx->index + avi->idx->cb;

	/* Init index entry */
	idx->dwChunkId = fcc_00dc;
	idx->dwFlags   = __cpu_to_le32(AVIIF_KEYFRAME);

	/* Offset from RIFF:"AVI "/`movi` start */
	idx->dwOffset  = __cpu_to_le32((uint32_t)w->size - w->movi_offs);
	idx->dwSize    = size;

	avi->idx->cb++;
	avi->idx->fcc = 0;	/* Set next field num to write */
#endif 

	/* Write video data */ 
	avi_list_t	l;

	l.name.v32 = fcc_00dc;
	l.size     = __cpu_to_le32(size);

	struct iovec {
              void *iov_base;   /* Starting address */
              size_t iov_len;   /* Number of bytes */
        };

	struct iovec io[3] = {
	    { &l, 8 },
	    { d0, size0 },
	    { d1, size1 }
	};

	avi_align_pos(w);		/* Align file pos if needed */

	ssize_t	res = writev(w->fd, (void*)io, 3);	// XXX FIXME
	if(res != (8 + size0 + size1) ) {
		if(res == -1) {
		}
	}
		
	/*  ajust file size, keep it uint16_t aligned */
	avi_ajust_by_size(w, size); 

	return 0;
}
/* END OF ODML implementation	*/
