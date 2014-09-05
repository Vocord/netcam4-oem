#ifndef __ABI_IP_VIDEO_RAW_H__
#define __ABI_IP_VIDEO_RAW_H__
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
 *     subsystem, for tasks specific to camera device data aquiring.
\*/

#include <abi/p3.h>

enum lid_type {
	LID_FD	 = 0<<31,
	LID_FH	 = 1<<31,
	LID_TYPE = 1<<31,
};

enum sample_format {
	sf_8bit		= 0<<28, 	/*  8bit */
	sf_10bit	= 1<<28, 	/* 10bit */
	sf_12bit	= 2<<28, 	/* 12bit */
	sf_14bit	= 3<<28,	/* 14bit */
	sf_16bit	= 4<<28,	/* 16bit */
};

typedef struct {
	uint32_t	lid;		/* lid[31] Packet Type		*/
					/*   =0 - FD packet		*/
					/* lid[30:0] Local Flow ID	*/
	uint8_t		flags;		/* Frame flags               	*/
					/* [4:0] colour mode            */
					/*   =0 BAYER_G1RG2B		*/
					/*   =1 BAYER_RG1BG2		*/
					/*   =2 BAYER_G1BG2R		*/
					/*   =3 BAYER_BG1RG2		*/
					/*   =4 BW			*/
					/* [6:5] camera orientation     */
					/*   =0 -   0			*/
					/*   =1 -  90			*/
					/*   =2 - 180			*/
					/*   =3 - 270			*/
					/* [7] Mirror flip		*/
					/*   =0 - off			*/
					/*   =1 - on			*/
	uint8_t		fseq;		/* Frame sequence Low  8 bits	*/
	uint16_t	size;		/* size of data[]		*/
	uint16_t	x_dim;		/* Frame width			*/
	uint16_t	y_dim;		/* Frame height			*/
	uint32_t	offs;		/* offs[27:0] Frame Data offset */
					/* offs[31:28] Sample format    */
					/*   =0 -  8bit			*/
					/*   =1 - 10bit			*/
					/*   =2 - 12bit			*/
					/*   =3 - 14bit			*/
					/*   =4 - 16bit			*/
	uint8_t		data[];
}  __attribute__((packed)) video_frame_raw_t;

typedef struct {
	uint32_t	lid;		/* x00 lid[31] Packet Type	*/
					/* x00  =1 - FH packet		*/
					/* x00 lid[30:0] Local Flow ID	*/
	uint32_t	fseq;		/* x04 Frame sequence 32 bits	*/
	p3_gid_t	gid;		/* x08 Frame GID		*/
	uint64_t	ts;		/* x20 Time stamp		*/
	uint16_t	x_dim;		/* x28 Frame width		*/
	uint16_t	y_dim;		/* x2a Frame height		*/
	uint32_t	fsize;		/* x2c */
					/* fsize[27:0] Frame Data size  */
					/* fsize[31:28] Sample format   */
					/*   =0 -  8bit			*/
					/*   =1 - 10bit			*/
					/*   =2 - 12bit			*/
					/*   =3 - 14bit			*/
					/*   =4 - 16bit			*/
	uint32_t	osize;		/* x30 */
					/* osize[27:0] F OOB Data size  */
					/* osize[31:28] Sensor bits	*/
					/*   =0 -  8bit			*/
					/*   =1 - 10bit			*/
					/*   =2 - 12bit			*/
					/*   =3 - 14bit			*/
					/*   =4 - 16bit			*/
	uint8_t		abi[];		/* x34 ABI packed data		*/
}  __attribute__((packed)) video_frame_raw_hdr_t;

#endif /* __ABI_IP_VIDEO_RAW_H__ */
