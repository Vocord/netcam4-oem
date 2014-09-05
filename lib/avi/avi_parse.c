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
#define _GNU_SOURCES

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>
#include <sys/mman.h>

#include <asm/byteorder.h>

#include <avi/fourcc.h>
#include <avi/avi_parse.h>

#include "trace.h"

extern FILE *I;

#undef  TRACE_LEVEL
#define TRACE_LEVEL 0

#define READ(fd, buff, size) {	err = read(fd, buff, size); \
				if(err != size) { \
					TRACE(0, "err: %s cannot read %d bytes! err=%d errno=%d\n", __func__, size, err, errno); \
					return -1; \
				} 			\
			     };

#define WRITE(fd, buff, size) {	err = write(fd, buff, size); \
				if(err != size) { \
					TRACE(0, "err: %s cannot write %d bytes! err=%d errno=%d\n", __func__, size, err, errno); \
					return -1; \
				} 			\
			      };

#define IDX_MAX	128

void avi_trace_chunk_hdr(avip_chunk_t *chunk, int32_t off, int level)
{
	int i;
	uint32_t n[2] = {};

	n[0] = chunk->four_cc;

	for(i = 0; i < level; i++)
		TRACE(0, "\t");
		
	TRACE(0, "chunk off=%08d size=%08d four_cc=[%s]\n", off, __cpu_to_le32(chunk->size), (char*)&n[0]);
}

void avi_show_index_chunk(int fd, size_t size, int level)
{
	int i, err, rlen = 0;
	uint32_t n[2] = {};
	avip_index_t idx;

	//TRACE(0, "%s size=%d\n", __func__, size);
	
	do {
		err = read(fd, &idx, sizeof(idx));
		if(err != sizeof(idx))
			return;

		n[0] = idx.ckid;
		for(i = 0; i < level; i++) TRACE(0, "\t");
		TRACE(0, "ckid=[%s] flags=0x%08lx off=%ld len=%ld\n", (char*)&n[0], 
				(unsigned long)idx.flags, 
				(unsigned long)__cpu_to_le32(idx.off), 
				(unsigned long)__cpu_to_le32(idx.len));

		rlen += err;
	} while(rlen < size);
}

void avi_trace_list_hdr(avip_list_t *lst, int32_t off, int level)
{
	int i;
	uint32_t n[4] = {};

	n[0] = lst->list;
	n[2] = lst->four_cc;

	for(i = 0; i < level; i++)
		TRACE(0, "\t");
	
	TRACE(0, "[%s] off=%08d size=%08d four_cc=[%s]\n", (char*)&n[0], off, __cpu_to_le32(lst->size), (char*)&n[2]);
}


int avi_process_chunk(avip_proc_req_t *req)
{
	int err;
	unsigned int pos;
	avip_chunk_t chunk;

	//TRACE(0, "%s off=%d\n", __func__, off);
	lseek(req->fd, req->off, SEEK_SET);

	err = read(req->fd, &chunk, sizeof(chunk));
	if(err && err != sizeof(chunk)) {
		TRACE(req->ll, "err: cannot read next chunk! err=%d errno=%d off=%d\n", err, errno, req->off);
		req->errs++;
		return -1;
	}

	pos = sizeof(chunk) + __cpu_to_le32(chunk.size);
	//TRACE(0, "pos=%d\n", pos);

	/* check pos to be even aligned */
	if(pos & 1) {
		TRACE(req->ll, "warn: pos not even aligned! pos=%d chunk size=%d\n",
				pos, __cpu_to_le32(chunk.size));

		if(req->ll <= TRACE_LEVEL) {
			TRACE(req->ll, "chunk info:\n");
			avi_trace_chunk_hdr(&chunk, req->off, 0);
			TRACE(req->ll, "list info:\n");
			avi_trace_list_hdr(req->cur_lst, req->cur_lst_off, 0);
		}
	
		pos += pos & 1;
		req->warn++;
		TRACE(req->ll, "try to permanent fix: align pos to %d\n", pos);
	}

	if(req->show)
		avi_trace_chunk_hdr(&chunk, req->off, req->level);

	if(req->show && (chunk.four_cc == fcc_indx)) {
		avi_show_index_chunk(req->fd, __cpu_to_le32(chunk.size), req->level + 1);
	}

	if(req->cur_lst->four_cc == fcc_hdrl && chunk.four_cc == fcc_avih) {
		//TRACE(0, "avih dumped\n");
		READ(req->fd, &req->avih, sizeof(req->avih));
		goto out;
	}

	if(req->cur_lst->four_cc == fcc_strl && chunk.four_cc == fcc_strh) {
		AVI_strh t;
		READ(req->fd, &t, sizeof(t));
		if(t.fccType.v32 == fcc_vids) {
			req->strh_vids = t;
		}
		goto out;
	}

	if(req->cur_lst->four_cc == fcc_movi && chunk.four_cc == fcc_00dc) {
		req->dc_frames++;
	}

	lseek(req->fd, (off_t)(req->off + pos), SEEK_SET);
out:	
	return pos;
}

int avi_process_list(avip_proc_req_t *req)
{
	uint32_t h;
	int err, pos = 0;
	avip_list_t cur_lst;
	avip_proc_req_t r = *req;

	lseek(req->fd, req->off, SEEK_SET);

	//TRACE(0, "%s level=%d off=%d\n", __func__, r.level, req->off);

	err = read(req->fd, &cur_lst, sizeof(cur_lst));
	if(err && err != sizeof(cur_lst)) {
		TRACE(0, "err: cannot read next lst! err=%d errno=%d off=%d\n", err, errno, req->off);
		return -1;
	}
	pos += err;

	//TRACE(0, "list=%08lx pos=%d\n", cur_lst.list, pos);
	if(req->show) {
		avi_trace_list_hdr(&cur_lst, req->off, r.level);
	}
	
	if(req->req_lst) {
		if(cur_lst.list == req->req_lst->list && cur_lst.four_cc == req->req_lst->four_cc) {
			r.req_lst_off = req->off;
		}
	}
	
	/* check size to be correct */
	if(!cur_lst.size || ((req->off + __cpu_to_le32(cur_lst.size)) > req->avi_size)) {
		TRACE(req->ll, "err: wrong lst size=%d detected!\n", __cpu_to_le32(cur_lst.size));
		if(req->ll <= TRACE_LEVEL) {
			TRACE(req->ll, "list info:\n");
			avi_trace_list_hdr(&cur_lst, req->off, 0);
		}
		req->errs++;
		goto out;
	}
	
	r.level++;
	r.cur_lst = &cur_lst;
	r.cur_lst_off = req->off;

	while(pos < __cpu_to_le32(cur_lst.size)) {
		err = read(req->fd, &h, sizeof(uint32_t));
		if(err != sizeof(uint32_t)) {
			TRACE(req->ll, "err: cannot get next elem head! err=%d errno=%d off=%d pos=%d level=%d\n",
					err, errno, req->off, pos, r.level);
			req->errs++;
			break;
		}
	
		r.off = req->off + pos;
		
		if(h == fccLIST) {
			/* there is LIST ahead */
			err = avi_process_list(&r);
			if(err < 0)
				return -1;
			pos += err;
		} else {
			/* show chunk */
			err = avi_process_chunk(&r);
			if(err < 0) {
				TRACE(req->ll, "warn: due to bad chunk try seek to end of list\n");
				pos = sizeof(cur_lst) + __cpu_to_le32(cur_lst.size);
				lseek(req->fd, req->off + pos, SEEK_SET);
				r.warn++;
				goto out;
			}
			pos += err;
		}
	}

	if(pos & 1) {
		TRACE(req->ll, "warn: pos not even aligned! pos=%d list size=%d\n",
				pos, __cpu_to_le32(cur_lst.size));

		if(req->ll <= TRACE_LEVEL) {
			TRACE(req->ll, "list info:\n");
			avi_trace_list_hdr(&cur_lst, req->off, 0);
		}
	
		pos += pos & 1;
		TRACE(req->ll, "try to permanent fix: align pos to %d\n", pos);
	}

out:
	req->warn 		= r.warn;
	req->req_lst_off 	= r.req_lst_off;
	req->avih 		= r.avih;
	req->strh_vids 		= r.strh_vids;
	req->dc_frames 		= r.dc_frames;

	//TRACE(0, "pos=%d fall back to level=%d\n", pos, level);

	return pos;
}

int avi_simple_check(avip_proc_req_t *req)
{
	int err;
	avip_list_t lst;

	if(!req->avi_size) {
		req->avi_size = lseek(req->fd, 0, SEEK_END);
	}

	lseek(req->fd, 0, SEEK_SET);
	
	err = read(req->fd, &lst, sizeof(lst));
	if(err != sizeof(lst)) {
		TRACE(0, "[%s] err need [%d] bytes, but read [%d] \n", __func__, sizeof(lst), err);
		if(err <= 0) {
			ETRACE("[err] read less than need or EOF. errno: ");
		}

		return -1;
	}

	if( (lst.list != fccRIFF) && (lst.four_cc == fcc_AVI) ) {
		uint8_t 	*fcc = (uint8_t *)&lst.four_cc;
		uint8_t 	*list = (uint8_t *)&lst.list;

		TRACE(0, "[err] expect RIFF-list, but in file the LIST is [%c%c%c%c] and the FOURCC is [%c%c%c%c]\n", 
			list[0], list[1], list[2], list[3],
			fcc[0], fcc[1], fcc[2], fcc[3]
			);	

		return -1;
	}

	if( (__cpu_to_le32(lst.size) + 8) != req->avi_size ) {
		TRACE(0, "[err] The AVI file is damaged. Cant seek to the end of RIFF. Actual size[%d], but in RIFF[%d]\n", \
			__cpu_to_le32(lst.size) + 8, req->avi_size);
		return -1;
	}

	return 0;
}

int avi_check(avip_proc_req_t *req)
{
	int err;
	avip_list_t lst;

	if(!req->avi_size) {
		req->avi_size = lseek(req->fd, 0, SEEK_END);
	}

	lseek(req->fd, 0, SEEK_SET);

	do {
		err = read(req->fd, &lst, sizeof(lst));
		if(err != sizeof(lst)) {
			if(err) {
				TRACE(0, "%s err: #1 cannot read next lst! [todo=%d done=%d] off=0x%x. errno: %s\n",
					__func__, sizeof(lst), err, req->off, strerror(errno));

				req->errs++;
				goto out;
			} else {
				/* EOF */
				break;
			}
		}

		if(lst.list == fccRIFF) { 
			TRACE(0, "RIFF found off=0x%x\n", req->off);
			req->off = lseek(req->fd, 0, SEEK_CUR) - sizeof(lst);
			avi_process_list(req);
		} else if(lst.list == fccLIST) {
			TRACE(0, "LIST found off=0x%x\n", req->off);
			req->off = lseek(req->fd, 0, SEEK_CUR) - sizeof(lst);
			avi_process_list(req);
		} else {
			TRACE(90, "[err] neither RIFF nor LIST, %c%c%c%c\n",
				*(((uint8_t *)&lst.list) + 0),
				*(((uint8_t *)&lst.list) + 1),
				*(((uint8_t *)&lst.list) + 2),
				*(((uint8_t *)&lst.list) + 3)
			);

			//exit(-1);
		}
	
	} while(err > 0);

out:
	/* hmm i dont know - need check it */
	req->off = 0;

	TRACE(90, "req->req_lst_off = [0x%x]\n", req->req_lst_off);

	return 0;
}

int avi_idx_restore(avip_proc_req_t *req)
{
	int err, ipos = 0, iflush_cnt = 0;
	unsigned long pos;
	int32_t		chunk_size;
	uint32_t	chunk_offs;
	avip_list_t *riffh, *movih;
	avip_index_t idx[IDX_MAX];
	avip_chunk_t ch, idx_ch = {fcc_idx1, 0};
	uint8_t *avi_mm;
       
	lseek(req->fd, 0, SEEK_SET);
	avi_mm	= mmap(0, req->avi_size, PROT_READ|PROT_WRITE, MAP_SHARED, req->fd, 0);
	//TRACE(0, "avi_mm=%08lx\n", avi_mm);

	if(avi_mm == (void*)-1) {
		TRACE(0, "%s err: cannot mmap avi file! errno=%d\n", __func__, errno);
		return -1;
	}

	if(!req->req_lst_off) {
		TRACE(0, "%s err: no 'movi' list found, restore isn't possible\n", __func__);
		goto out_err;
	}

	riffh = (avip_list_t*)avi_mm;

	movih = (avip_list_t*)(avi_mm + req->req_lst_off);


	if(movih->four_cc != fcc_movi) {
		TRACE(0, "%s(): [err] cannot find 'movi' section\n", __func__);
		goto out_err;
	}

	/* Stupid LIST, 4 octets - fourcc size */
	movih->size = 4;
	chunk_offs = 4;

	if(req->show)
		TRACE(0, "try to close 'RIFF' and 'movi' list\n");
	
	/* number of dc frames will be re-counted */
	req->dc_frames = 0;
	
	/* seek to first `movi` list chunk */
	lseek(req->fd, (int32_t)(req->req_lst_off + sizeof(*movih)), SEEK_SET);

	pos = req->req_lst_off + sizeof(*movih);

TRACE(90, "start pos=%lx\n", pos);

	while(pos < req->avi_size)
	{
		READ(req->fd, &ch, sizeof(ch));

		/* round to 2 */
		chunk_size = __cpu_to_le32(ch.size);
		chunk_size += chunk_size & 1;

#if 0
TRACE(0, "ch.size=%x pos=%lx\n", chunk_size, pos);
TRACE(0, "\tline:%d chunk, %x%x%x%x\n",
	__LINE__,
	*(((uint8_t *)&ch.four_cc) + 0),
	*(((uint8_t *)&ch.four_cc) + 1),
	*(((uint8_t *)&ch.four_cc) + 2),
	*(((uint8_t *)&ch.four_cc) + 3)
);
#endif
		
		pos += (sizeof(ch) + chunk_size);

		if(pos > req->avi_size) {
			TRACE(0, "%s err: pos=%ld exeeds avi size=%d! last chunk size=%d\n", __func__, pos, req->avi_size, chunk_size);
			return -1;
		}

		if(ipos >= IDX_MAX) {
			/* flush indexes part */
			lseek(req->fd, (int32_t)(req->avi_size + sizeof(idx_ch) + iflush_cnt * IDX_MAX * sizeof(avip_index_t)), SEEK_SET);
			WRITE(req->fd, &idx[0], IDX_MAX * sizeof(avip_index_t));
			iflush_cnt++;
			ipos = 0;
			lseek(req->fd, (int32_t)(pos - chunk_size), SEEK_SET);
		}
	
		/* we dont need any JUNK in the idx1 section */
		if(ch.four_cc != fccJUNK) {
		    idx[ipos].ckid 	= ch.four_cc;
		    idx[ipos].off  	= __cpu_to_le32(chunk_offs);
		    idx[ipos].len	= ch.size;
		    idx[ipos].flags 	= __cpu_to_le32(AVIIF_KEYFRAME);

		    if(ch.four_cc == fcc_00dc) {
			    req->dc_frames++;
		    }
			    
		    ipos++;
		}
		
		chunk_offs += (sizeof(ch) + chunk_size);

		lseek(req->fd, chunk_size, SEEK_CUR);
		movih->size += sizeof(ch) + chunk_size;

		//TRACE(0, "movih->size=%ld\n", movih->size);
	}

	/* flush last indexes part */
	if(ipos > 0) {
		ipos--;
		lseek(req->fd, (int32_t)(req->avi_size + sizeof(idx_ch) + iflush_cnt * IDX_MAX * sizeof(avip_index_t)), SEEK_SET);
		WRITE(req->fd, &idx[0], ipos * sizeof(avip_index_t));	
	}
	
	/* calculate idx1 chunk size */ //& write it in file */
	idx_ch.size = (ipos + iflush_cnt * IDX_MAX) * sizeof(avip_index_t);

	/* riffh time need to be calculated _here_, too */
	/* convert it all to proper endian */
	riffh->size = __cpu_to_le32(req->req_lst_off + movih->size + sizeof(idx_ch) + idx_ch.size);
	idx_ch.size = __cpu_to_le32(idx_ch.size);
	movih->size = __cpu_to_le32(movih->size);

	/* write idx1 to file */	
	lseek(req->fd, (int32_t)(req->avi_size), SEEK_SET);
	WRITE(req->fd, &idx_ch, sizeof(idx_ch));

	munmap(avi_mm, req->avi_size);
	return 0;
out_err:
	munmap(avi_mm, req->avi_size);
	return -1;
}


