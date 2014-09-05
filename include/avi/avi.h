#ifndef __AVI_FUNCTIONS__
#define __AVI_FUNCTIONS__
/*\
 * Header file for standard AVI structures
 *
 * Copyright (C) 2000-2005 Vocord Global Telecommunications <info@vocord.com>
 *
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111, USA.
\*/

#include <avi/structures.h>
#include <api/frames.h>
#include <abi/mdm.h>

struct avi_hdlr_s;
typedef struct avi_hdlr_s {
	avi_rwh_t	*w;
	AVI_idx1	*idx;

	AVISUPERINDEX	*six_odml;
	AVISTDINDEX	*fix_odml;

	uint32_t	max_frames;
	uint32_t	max_size;

	uint8_t		*lfield_data;
	uint32_t	lfield_size;

	int		(*write_f0)(struct avi_hdlr_s*, uint8_t*, unsigned);
	int		(*write_f1)(struct avi_hdlr_s*, uint8_t*, unsigned);

	int		fd;	/* idx fd */
	unsigned	subrate; /* pll independent video frame rate */

	uint64_t	t0;	/* data start */
	uint64_t	t1;	/* data end   */
	uint64_t	tc;	/* data to close */
	uint64_t	tf;	/* frame period */
	
	uint64_t	audio_th;	/* audio th */
	
	uint64_t	flow;	/* local flow id */
	uint64_t	aflow;	/* audio flow id */
	
	loff_t		tag;	/* AVI data entry offset tag */

	int		year;	/* create date */
	int8_t		month;	/* create date */
	int8_t		day;	/* create date */
	int8_t		fflag;	/* flow flag */
	int8_t		reserved;	/* reserved */
} avi_hdlr_t;

/* Avi Trace Functions */
extern void avi_trace_avih(AVI_avih *, char *);
extern void avi_trace_strh(AVI_strh *, char *);
extern void avi_trace_vids_mj2c(AVI_vids_mj2c *, char *);
extern void avi_trace_vids(AVI_vids *, char *);
extern void avi_trace_auds(AVI_auds *, char *);
extern void avi_trace_txts(AVI_txts *, char *);
extern void avi_trace_strf(void *, char*);
extern void avi_reset_stream_type(void);
extern uint32_t avi_get_stype(void);
extern uint32_t avi_get_scodec(void);

/* Avi File Write functions */
extern avi_rwh_t *avi_file_open(char *name);
extern avi_rwh_t *avi_file_reopen(char *name, avi_rwh_t*);
extern int avi_file_data_last(avi_rwh_t *a);
extern int avi_file_data_append(avi_rwh_t *a, uint8_t *data, uint32_t size);
extern int avi_file_data_create(avi_rwh_t *a, uint32_t name);
extern int avi_file_list_open(avi_rwh_t *a, uint32_t fcc, uint32_t type);
extern int avi_file_chunk_align(avi_rwh_t *a, uint32_t alignment);
extern int avi_file_chunk_append(avi_rwh_t *a, uint32_t fcc, uint8_t *data, uint32_t size);
extern int avi_file_chunk_append_fake(avi_rwh_t *a, uint32_t size);
extern int avi_file_list_close(avi_rwh_t *a);
extern int avi_file_pre_close(avi_rwh_t *a);
extern int avi_file_close(avi_rwh_t *a);

/* special func */
extern void avi_ajust_by_size(avi_rwh_t *a, uint32_t size);
extern void avi_align_pos(avi_rwh_t *a);

/* avi data write helper functions */
extern int avif_write_audio(avi_hdlr_t *avi, uint8_t *d, unsigned size);
/* non interlaced fields */
extern int avif_write_field(avi_hdlr_t *avi, field_descr_t *f, mdm_request_t *meta);
extern int avif_write_field_oob(avi_hdlr_t *avi, field_descr_t	*f, mdm_request_t *meta);
/* interlaced fields */
extern int avif_write_field0(avi_hdlr_t *avi, uint8_t *d, unsigned size);
extern int avif_write_field1(avi_hdlr_t *avi, uint8_t *d, unsigned size);
extern int avif_write_2fields(avi_hdlr_t *avi, field_descr_t *f, mdm_request_t *meta);
extern int avif_write_2fields_oob(avi_hdlr_t *avi, field_descr_t *f, mdm_request_t *meta);
/* Avi Dump Functions */    
#define PRN_FCC(X) TRACE(0, "    %c%c%c%c %08"PRIx32"", X.v8[0], X.v8[1], X.v8[2], X.v8[3], X.v32 );
#endif
