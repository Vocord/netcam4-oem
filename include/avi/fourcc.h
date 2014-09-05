#ifndef _FOURCC_H
#define _FOURCC_H
/*\
 * Copyright (C) 2001 Petros Tsantoulis <ptsant@otenet.gr>
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
#include <endian.h>

/* We need these to ensure proper file structure regardless of endian-ness */
#ifndef FOURCC

#if __BYTE_ORDER==__LITTLE_ENDIAN
#define FOURCC(ch0, ch1, ch2, ch3)		\
        (((uint32_t)(uint8_t)(ch0) <<  0 ) |	\
	 ((uint32_t)(uint8_t)(ch1) <<  8 ) |	\
         ((uint32_t)(uint8_t)(ch2) << 16 ) |	\
       	 ((uint32_t)(uint8_t)(ch3) << 24 ) )
#else
#define FOURCC(ch0, ch1, ch2, ch3)		\
        (((uint32_t)(uint8_t)(ch0) << 24 ) |	\
	 ((uint32_t)(uint8_t)(ch1) << 16 ) |	\
         ((uint32_t)(uint8_t)(ch2) <<  8 ) |	\
       	 ((uint32_t)(uint8_t)(ch3) <<  0 ) )
#endif
#endif

#ifndef TWOCC
#if __BYTE_ORDER==__LITTLE_ENDIAN
#define TWOCC( ch0, ch1 ) \
	(((uint32_t)(uint8_t)(ch0)<<0) | \
	 ((uint32_t)(uint8_t)(ch1)<<8))
#else
#define TWOCC( ch0, ch1 ) \
	(((uint32_t)(uint8_t)(ch0)<<8) | \
	 ((uint32_t)(uint8_t)(ch1)<<0))
#endif
#endif

#if defined(__CYGWIN__)
#if __BYTE_ORDER==__LITTLE_ENDIAN
#ifndef __cpu_to_le16
#define __cpu_to_le16(X)	(X)
#endif
#ifndef __le16_to_cpu
#define __le16_to_cpu(X)	(X)
#endif

#ifndef __cpu_to_le32
#define __cpu_to_le32(X)	(X)
#endif
#ifndef __le32_to_cpu
#define __le32_to_cpu(X)	(X)
#endif
#else
#error provide support for current ARCH
#endif
#endif

#if 0
#ifndef COMBINETWO2CC
#define COMBINETWO2CC( uint16_1, uint16_2 ) \
	(((uint32_t)(uint16_t)(uint16_1) << 0) | \
	 ((uint32_t)(uint16_t)(uint16_2) <<16))
#endif
#endif

/* AVI1.0 */
#define fccRIFF		FOURCC('R','I','F','F')
#define fccLIST		FOURCC('L','I','S','T')
#define fccJUNK		FOURCC('J','U','N','K')

#define  fcc_movi	FOURCC('m','o','v','i')
#define  fcc_vids	FOURCC('v','i','d','s')
#define  fcc_auds	FOURCC('a','u','d','s')
#define  fcc_txts	FOURCC('t','x','t','s')  

#define  fcc_AVI	FOURCC('A','V','I',' ')
#define  fcc_avih	FOURCC('a','v','i','h')

#define  fcc_rec	FOURCC('r','e','c',' ')
#define  fcc_strh	FOURCC('s','t','r','h')
#define  fcc_strf	FOURCC('s','t','r','f')
#define  fcc_strl	FOURCC('s','t','r','l')

#define  fcc_hdrl	FOURCC('h','d','r','l')
#define  fcc_strd	FOURCC('s','t','r','d')
#define  fcc_strn	FOURCC('s','t','r','n')
#define  fcc_idx1	FOURCC('i','d','x','1')

#define fcc_00db	FOURCC('0','0','d','b')
#define fcc_01db	FOURCC('0','1','d','b')
#define fcc_00dc	FOURCC('0','0','d','c')
#define fcc_01wb	FOURCC('0','1','w','b')

#define fcc_db		TWOCC('d','b')
#define fcc_dc		TWOCC('d','c')
#define fcc_pc		TWOCC('p','c')
#define fcc_wb		TWOCC('w','b')

/* ODML AKA AVI2.0 Extensions */
#define  fcc_indx	FOURCC('i','n','d','x')
#define  fcc_ix00	FOURCC('i','x','0','0')
#define  fcc_odml	FOURCC('o','d','m','l')
#define  fcc_dmlh	FOURCC('d','m','l','h')
#define  fcc_AVIX	FOURCC('A','V','I','X')
    
#define  fcc_stub	FOURCC('s','t','u','b')

/* MJ2P/QT */
#define  fcc_jpg2000	FOURCC('j','P',' ',' ')
#define  fcc_mjp2	FOURCC('m','j','p','2')
#define  fcc_jpeg	FOURCC('j','p','e','g')
#define  fcc_sowt	FOURCC('s','o','w','t')
#define  fcc_moof	FOURCC('m','o','o','f')
#define  fcc_mfhd	FOURCC('m','f','h','d')
#define  fcc_jp2c	FOURCC('j','p','2','c')
#define  fcc_mvex	FOURCC('m','v','e','x')
#define  fcc_trex	FOURCC('t','r','e','x')
#define  fcc_ftyp	FOURCC('f','t','y','p')
#define  fcc_mdat	FOURCC('m','d','a','t')
#define  fcc_moov	FOURCC('m','o','o','v')
#define  fcc_mvhd	FOURCC('m','v','h','d')
#define  fcc_trak	FOURCC('t','r','a','k')
#define  fcc_tkhd	FOURCC('t','k','h','d')
#define  fcc_mdia	FOURCC('m','d','i','a')
#define  fcc_mdhd	FOURCC('m','d','h','d')
#define  fcc_hdlr	FOURCC('h','d','l','r')
#define  fcc_vide	FOURCC('v','i','d','e')
#define  fcc_mhlr	FOURCC('m','h','l','r')
#define  fcc_soun	FOURCC('s','o','u','n')
#define  fcc_minf	FOURCC('m','i','n','f')
#define  fcc_vmhd	FOURCC('v','m','h','d')
#define  fcc_smhd	FOURCC('s','m','h','d')
#define  fcc_dinf	FOURCC('d','i','n','f')
#define  fcc_stbl	FOURCC('s','t','b','l')
#define  fcc_dref	FOURCC('d','r','e','f')
#define  fcc_url	FOURCC('u','r','l',' ')
#define  fcc_stsz	FOURCC('s','t','s','z')
#define  fcc_stsc	FOURCC('s','t','s','c')
#define  fcc_stts	FOURCC('s','t','t','s')
#define  fcc_stsd	FOURCC('s','t','s','d')
#define  fcc_stco	FOURCC('s','t','c','o')
#define  fcc_jp2h	FOURCC('j','p','2','h')
#define  fcc_fiel	FOURCC('f','i','e','l')
#define  fcc_jsub	FOURCC('j','s','u','b')
#define  fcc_ihdr	FOURCC('i','h','d','r')
#define  fcc_colr	FOURCC('c','o','l','r')
#define  fcc_free	FOURCC('f','r','e','e')
#define  fcc_skip	FOURCC('s','k','i','p')

/* QT */
#define  fcc_qt		FOURCC('q','t',' ',' ')

/* wave codecs */
#define WAVE_FORMAT_PCM		(1)
/*      divx codecs     */
#define fccDIVX FOURCC('D','I','V','X')
#define fccDIV3 FOURCC('D', 'I', 'V', '3')
#define fccDIV4 FOURCC('D', 'I', 'V', '4')
#define fccdiv3 FOURCC('d', 'i', 'v', '3')
#define fccdiv4 FOURCC('d', 'i', 'v', '4')
#define fccMP41 FOURCC('M', 'P', '4', '1')
#define fccMP43 FOURCC('M', 'P', '4', '3')
/*      old ms mpeg-4 codecs */
#define fccMP42 FOURCC('M', 'P', '4', '2')
#define fccmp42 FOURCC('m', 'p', '4', '2')
#define fccmp43 FOURCC('m', 'p', '4', '3')
#define fccmpg4 FOURCC('m', 'p', 'g', '4')
#define fccMPG4 FOURCC('M', 'P', 'G', '4')
/*      windows media codecs */
#define fccWMV1 FOURCC('W', 'M', 'V', '1')
#define fccwmv1 FOURCC('w', 'm', 'v', '1')
#define fccWMV2 FOURCC('W', 'M', 'V', '2')
#define fccwmv2 FOURCC('w', 'm', 'v', '2')
/*      other codecs    */
#define fccIV32 FOURCC('I', 'V', '3', '2')
#define fccIV41 FOURCC('I', 'V', '4', '1')
#define fccIV50 FOURCC('I', 'V', '5', '0')
#define fccI263 FOURCC('I', '2', '6', '3')
#define fccCVID FOURCC('c', 'v', 'i', 'd')
#define fccVCR2 FOURCC('V', 'C', 'R', '2')
#define fcc_strf_MJ2C FOURCC('M', 'J', '2', 'C')
#define fcc_strf_mj2c FOURCC('m', 'j', '2', 'c')
#define fccMJPG FOURCC('M', 'J', 'P', 'G')
#define fccYUV  FOURCC('Y', 'U', 'V', ' ')
#define fccYUY2 FOURCC('Y', 'U', 'Y', '2')
#define fccYV12 FOURCC('Y', 'V', '1', '2')/* Planar mode: Y + V + U  (3 planes) */
#define fccIYUV FOURCC('I', 'Y', 'U', 'V')/* Planar mode: Y + U + V  (3 planes) */
#define fccUYVY FOURCC('U', 'Y', 'V', 'Y')/* Packed mode: U0+Y0+V0+Y1 (1 plane) */
#define fccYVYU FOURCC('Y', 'V', 'Y', 'U')/* Packed mode: Y0+V0+Y1+U0 (1 plane) */

#endif
