#ifndef __P3_ABI_CAM_H__
#define __P3_ABI_CAM_H__
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
 *     This file contains data types and constants declarations for P3SS ABI LB
 *     subsystem, for tasks specific to camera device menagement and settings.
\*/

#ifdef __cplusplus
extern "C" {
#endif

/*\
 * Constants declarations
\*/
/* The CAM4 interfaces source 							*/
/* http://wiki.vocord.com/tiki-index.php?page=spec-arch-p3ss-ns-doc#NETCAM	*/
typedef enum {
	CAM4_CMD_IFS		= 0, 	/*  CMD			DEV interface		*/
	CAM4_RAW_IFS		= 1,	/*  Video		RAW interface		*/
	CAM4_RAW_IF0		= 1,	/*  Video		RAW interface		*/
	CAM4_JPEG_IFS		= 2,	/*  JPEG		DEV interface		*/
	CAM4_JPEG_IF0		= 2,	/*  JPEG		DEV interface		*/
	CAM4_MPJG_IFS		= 3, 	/*  JPEG-MULTIPART	HTTP interface		*/
	CAM4_MPJG_IF0		= 3, 	/*  JPEG-MULTIPART	HTTP interface		*/
	CAM4_FACE_IFS		= 4, 	/*  Boxes		RAW interface		*/
	CAM4_FACE_IF0		= 4, 	/*  Boxes		RAW interface		*/
	CAM4_FACE_DEV_IFS	= 5, 	/*  Boxes		DEV interface		*/
	CAM4_FACE_DEV_IF0	= 5, 	/*  Boxes		DEV interface		*/
	CAM4_AUDIO_IFS		= 6,	/*  Audio		DEV interface		*/

	CAM4_RAW_IF1		= 9,	/*  Video		RAW interface		*/
	CAM4_JPEG_IF1		= 10,	/*  JPEG		DEV interface		*/
	CAM4_MPJG_IF1		= 11, 	/*  JPEG-MULTIPART	HTTP interface		*/
	CAM4_FACE_IF1		= 12, 	/*  Boxes		RAW interface		*/
	CAM4_FACE_DEV_IF1	= 13,	/*  Boxes		DEV interface		*/
	
	/* RESERVED 	*/
	/* 6-8	Video  	*/
	/* 9-12 Audio 	*/
} cam4_data_source_e;
/*\
 *  Data structures declarations
\*/
#if defined(_MSC_VER) && !defined(__GNUC__)
#include <pshpack1.h>
#endif


#if defined(_MSC_VER) && !defined(__GNUC__)
#include <poppack.h>
#endif

#ifdef __cplusplus
}
#endif
#endif  /* __P3_ABI_CAM_H__ */
