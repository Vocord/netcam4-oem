#ifndef __AVI_STRUCTURES__
#define __AVI_STRUCTURES__
/*\
 * Header file for standard AVI structures
 *
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
#define __GNU_
#include <stdint.h>

typedef struct {
    union {
    	uint32_t	v32;
	uint8_t		v8[4];
    } name;

    uint32_t	size;
    union {
	uint32_t	v32;
	uint8_t		v8[4];
    } type;
    uint8_t	data[0];
} avi_list_t;

typedef struct {
	loff_t		offs;

    	uint32_t	name;
    	uint32_t	type;
} __attribute__((packed)) avi_llist_t;

typedef struct {
	avi_llist_t	l[10];		/* Nested LIST descriptors */

	int		partial;	/* current nesting level */
	int		idx;		/* current nesting level */
	int		max_idx;	/* max nesting level */
	int		fd;		/* RIFF file descriptor */

	loff_t		size;		/* Last data entry offset EQ avi size */
	loff_t		sync_pos;	/* Last data sync pos size */

	uint32_t	movi_offs;	/* last 'movi' section offset */
	
	int		sync_len;	/* Max unsynced delta */
	int		align;
} avi_rwh_t;


/* avih */
typedef struct _MainAVIHeader {
	uint32_t	ulMicroSecPerFrame;
	uint32_t	ulMaxBytesPerSec;
	uint32_t	ulPaddingGranularity;
	uint32_t	ulFlags;
	uint32_t	ulTotalFrames;
	uint32_t	ulInitialFrames;
	uint32_t	ulStreams;
	uint32_t	ulSuggestedBufferSize;
	uint32_t	ulWidth;
	uint32_t	ulHeight;
	uint32_t	ulReserved[4];	/* Set to zero */
} AVI_avih, MainAVIHeader;


#define AVISF_DISABLED 0x0000001   /* These values are not known for sure */
#define AVISF_VIDEO_PALCHANGES 0x0001000 

typedef union {
	uint8_t		v8[4];
	uint32_t	v32;
} fcc_u;

/* strh */
typedef struct _AVIStreamHeader {
  	fcc_u		fccType;
	fcc_u		fccHandler;
	uint32_t	ulFlags;
	uint32_t	ulPriority;
	uint32_t	ulInitialFrames;
	uint32_t	ulScale;
	uint32_t	ulRate;
	uint32_t	ulStart;
	uint32_t	ulLength;
	uint32_t	ulSuggestedBufferSize;
	uint32_t	ulQuality; /* Between 0 - 10000, -1 means use default */
	uint32_t	ulSampleSize;
	uint16_t	ulrcFrame[4];  /* Coordinates x,y */
} AVI_strh, AVIStreamHeader;


/* strf chunk : WAVEFORMATEX, PCMWAVERFORMAT, BITMAPINFO */

/* NOTE : documentation refers to Width, Height and X/Y Pixels/meter as LONG */
/* This is assumed to be a signed 32 bit int */

typedef struct _BITMAPINFOHEADER {
	uint32_t	lSize;
	int32_t		lWidth; /* Width, in pixels */
	int32_t		lHeight; /* Height, in pixels */
	uint16_t	wPlanes;
	uint16_t	wBitCount;
  	fcc_u		lCompression;
	uint32_t	lSizeImage; /* Size in bytes, of the image, can be 0 for uncompressed RGB */
	int32_t		wXPelsPerMeter;
	int32_t		wYPelsPerMeter;
	uint32_t	lClrUsed;
	uint32_t	lClrImportant;
} AVI_vidsB, BITMAPINFOHEADER;

typedef struct _RGBQUAD {
	uint8_t		rgbBlue;
	uint8_t		rgbGreen;
	uint8_t		rgbRed;
	uint8_t		rgbReserved; /* alpha channel, propably */
} RGBQUAD;

typedef struct _BITMAPINFO {
	BITMAPINFOHEADER	bmiHeader;
	RGBQUAD			bmiColors[1];  /* I won't use it. */
} AVI_vids, BITMAPINFO;

typedef struct _WAVEFORMATEX {
	uint16_t	wFormatTag;
	uint16_t	wChannels;
	uint32_t	lSamplesPerSec;
	uint32_t	lAvgBytesPerSec;
	uint16_t	wBlockAlign;
	uint16_t	wBitsPerSample;
	uint16_t	wSize; /* Size in bytes of the surplus header,
			   depending on the format! */
} AVI_auds, WAVEFORMATEX;

typedef struct _AVIINDEXENTRY {
	uint32_t	lckid; /* FOURCC of the chunk id */
	uint32_t	lFlags; /* Logical OR of the flags, please check docs */
	uint32_t	lChunkOffset; /* >Including< the header, i.e. before */
	uint32_t	lChunkLength; /* Not including the header */
} AVI_idx1c_, AVIINDEXENTRY;

typedef struct _avioldindex_entry {
	uint32_t   dwChunkId;
	uint32_t   dwFlags;
	uint32_t   dwOffset;
	uint32_t   dwSize;
} AVI_idx1c;

typedef struct _avioldindex {
	uint32_t  fcc;
	uint32_t  cb;

	AVI_idx1c index[0];
} AVI_idx1, AVIOLDINDEX;

/* odml addons */

/* bIndexType codes */
#define AVI_INDEX_OF_INDEXES		0x00	// when each entry in aIndex array points to an index chunk
#define AVI_INDEX_OF_CHUNKS		0x01	// when each entry in aIndex array points to a chunk in the file
#define AVI_INDEX_OF_TIMED_CHUNKS  	0x02
#define AVI_INDEX_OF_SUB_2FIELD    	0x03
#define AVI_INDEX_IS_DATA 		0x80	// when each entry is aIndex is really the data

/* bIndexSubtype codes for INDEX_OF_CHUNKS */
#define AVI_INDEX_SUB_DEFAULT		0x00
#define AVI_INDEX_SUB_2FIELD      	0x01	// when fields within frames are also indexed
#define AVI_INDEX_2FIELD AVI_INDEX_SUB_2FIELD	// different docs describe this in different way

typedef struct  {
	int64_t qwOffset;	/* must point to the fcc (ix##) not to the chunk */
	uint32_t dwSize;	/* size of the index chuck at qwOffset */
	uint32_t dwDuration;	/* seems that it's number of frames */
} _avisuperindex_entry;

typedef struct _avisuperindex_chunk {
	//uint32_t 	fcc;		/* 'indx' */
	//uint32_t 	cb;		/* size of this structure */
	uint16_t 	wLongsPerEntry;	/* must be 4 (size of each entry in aIndex array) */
	uint8_t	 	bIndexSubType;	/* must be 0 or AVI_INDEX_SUB_2FIELD */
	uint8_t	 	bIndexType;	/* must be AVI_INDEX_OF_INDEXES */
	uint32_t 	nEntriesInUse;	/* number of entries in aIndex array that are used  */
	uint32_t 	dwChunkId;	/* '##dc' or '##db' or '##wb', etc */
	uint32_t 	dwReserved[3]	/* must be 0 */;

	_avisuperindex_entry aIndex[0];

} AVISUPERINDEX, *PAVISUPERINDEX;

typedef struct _avistdindex_entry {
	uint32_t dwOffset;	/* qwBaseOffset + this is absolute file offset */
	uint32_t dwSize;	/* bit 31 is set if this is NOT a keyframe (does not include size of riff header)*/
} AVISTDINDEX_ENTRY;

typedef struct _avistdindex_chunk {
	//uint32_t	fcc;		/* 'indx' or 'ix##' */
	//uint32_t	cb;
	uint16_t	wLongsPerEntry; /* must be sizeof(aIndex[0])/sizeof(DWORD) */
	uint8_t		bIndexSubType;	/* must be 0 */
	uint8_t		bIndexType;	/* must be AVI_INDEX_OF_CHUNKS */
	uint32_t	nEntriesInUse;	/**/ 
	uint32_t	dwChunkId;	/* '##dc' or '##db' or '##wb' etc.. */
	int64_t		qwBaseOffset;	/* all dwOffsets in aIndex array are relative to this */
	uint32_t	dwReserved3;	/* must be 0 */
	
   	AVISTDINDEX_ENTRY aIndex[0];

} AVISTDINDEX, *PAVISTDINDEX;

typedef struct _avifieldindex_chunk {
	//uint32_t	fcc;		/* 'ix##' */
	//uint32_t	cb;
	uint16_t	wLongsPerEntry;	/* must be 3 (size of each entry in aIndex array) */
	uint8_t		bIndexSubType;  /* AVI_INDEX_2FIELD */
	uint8_t		bIndexType; 	/* AVI_INDEX_OF_CHUNKS */
	uint32_t	nEntriesInUse;	/* same as in superindex */
	uint32_t	dwChunkId; 	/* '##dc' or '##db' */
	uint32_t	qwBaseOffset; 	/* offsets in aIndex array are relative to this */
	uint32_t	dwReserved[3]; 	/* must be 0 */

	struct _avifieldindex_entry {
		uint32_t dwOffset;
		uint32_t dwSize;	/* size of all fields (bit 31 set for NON-keyframes) */
		uint32_t dwOffsetField2;/* offset to second field */
	} aIndex[0];

} AVIFIELDINDEX, * PAVIFIELDINDEX;

typedef struct {
	uint32_t dwTotalFrames;		/* indicates the real size of the AVI file in frames */
} ODMLExtendedAVIHeader;


/* end of odml addons */

#define AVIIF_LIST      0x00000001
#define AVIIF_KEYFRAME  0x00000010
#define AVIIF_FIRSTPART 0x00000020
#define AVIIF_LASTPART  0x00000040
#define AVIIF_MIDPART   (AVIIF_LASTPART | AVIFF_FIRSTPART)
#define AVIIF_NOTIME    0x00000100
#define AVIIF_COMPUSE   0x0fff0000

#define AVIIF_KNOWN_FLAGS 0x0fff0171

/* flags for use in <ulFlags> in AVIFileHdr */
#define AVIF_HASINDEX           0x00000010      /*  Index at end of file? */
#define AVIF_MUSTUSEINDEX       0x00000020
#define AVIF_ISINTERLEAVED      0x00000100
#define AVIF_TRUSTCKTYPE        0x00000800      /* Use CKType to find key frames? */
#define AVIF_WASCAPTUREFILE     0x00010000
#define AVIF_COPYRIGHTED        0x00020000

typedef struct BIN_FILE_HEADERtag {
        uint32_t dwTag;
        uint32_t dwExtra;
} BIN_FILE_HEADER, *PBIN_FILE_HEADER;

typedef struct BIN_IMAGE_HEADERtag {
        uint32_t dwTag;
        uint32_t dwSize;
} BIN_IMAGE_HEADER, *PBIN_IMAGE_HEADER;

#if 0        
// ADV2 struct
typedef struct ADV2tag {
        HANDLE hFile; // File handle
        ADV202_COMPRESSED_FIELD_HEADER FieldHdr; // ADV2 format description
        BOOL bOld;
        BOOL bBin;
        BIN_FILE_HEADER BinFileHdr;
        BIN_IMAGE_HEADER BinImgHdr;
        uint8_t cFmt;
        int nWidth;
        int nHeight;
        BOOL bPPT;
        BOOL bPLT;
        BOOL bSOP;
        BOOL bEPH;
} ADV2, *PADV2;
#endif

/* MORGAN Codec JPEG2000INFOHEADER2 */
typedef struct {
	uint32_t 	Size;		//  sizeof(JPEG2000INFOHEADER2) + sizes of ICC profile and J2C Header (default=sizeof(JPEG2000INFOHEADER2))
	uint16_t 	InterlaceFlags;	//  Interlace flags (see Interlace Flags, default=0).
	uint16_t 	WeightsMJP2;	//  MJP2 weights (1=yes, 0=no, default=1).
	uint16_t 	ColorSpaceID;	//  Color space ID (see Color Space IDs, default: see JPEG2000INFOHEADER2 Initialization functions.).
	uint16_t 	NativeOrder;	//  Native order (0 = little-endian, 1 = big-endian, default=0).
	uint16_t 	BitsPerSample;	//  Bits per sample (default=8).
	uint16_t 	SampleSigned;	//  Sample signed (1=yes, 0=no, default=0).
	uint16_t 	VSubSampling;	//  V subsampling (default: see JPEG2000INFOHEADER2 Initialization functions.).
	uint16_t 	VOffset;	//  V offset (default=0).
	uint16_t 	HSubSampling;	//  H subsampling (default: see JPEG2000INFOHEADER2 Initialization functions.).
	uint16_t 	HOffset;	//  H offset (default=0).
	uint16_t 	Levels;		//  Number of wavelet decomposition levels, or s_es. May not exceed 32. (default=5).
	uint16_t 	Process;	//  Wavelet kernels (0=5/3, 1=9/7, default=0 for RGB, 1 for YUV).
	uint16_t 	Layers;		//  Number of quality layers. May not exceed 16384 (default=1).
	uint16_t 	Quality;	//  Quality (from 0 to 2400 in % of 1 bit per pixel, q=100 => ~1bpp, q=1000 => ~10bpp, q=2400 => ~24bpp possible for Digital Cinema see Compliance points (Cpoint) for MJPEG2000).
	uint16_t 	CBR;		//  Constant bit-rate (1=yes, 0=no, default=0), 0 means VBR.
	uint16_t 	BitRate;	//  Average bit-rate expressed in KB/sec with 1KB=1024 bytes, 0 means not defined (default=0).
	uint16_t 	Lossless;	//  Lossless (1=yes, 0=no, default=0).
	uint16_t 	ExtraSize;	//  sizeof(JPEG2000EXTRAHEADER) + sizes of ICC profile and J2C Header (default=sizeof(JPEG2000EXTRAHEADER))
} morgan_jpeg2000infoheader2;

/* MORGAN Codec JPEG2000EXTRAHEADER */
typedef struct {
	uint16_t 	Profile;	//  Restricted profile to which the code-stream must conform, 0 to 2, 0=most restrictive, 2=places no restrictions (default=2).
	uint16_t 	VSize;		//  Image size: vertical dimension (default=0).
	uint16_t 	HSize;		//  Image size: horizontal dimension (default=0).
	uint16_t 	VOrigin;	//  Image origin on canvas: vertical coordinate (default=0).
	uint16_t 	HOrigin;	//  Image origin on canvas: horizontal coordinate (default=0).
	uint16_t 	VTile;		//  Tile partition size: vertical dimension (default=0).
	uint16_t 	HTile;		//  Tile partition size: horizontal dimension (default=0).
	uint16_t 	VTOrigin;	//  Tile origin on canvas: vertical coordinate (default=0).
	uint16_t 	HTOrigin;	//  Tile origin on canvas: horizontal coordinate (default=0).
	uint16_t 	UseSOP;		//  Include SOP markers (i.e., resync markers) (1=yes, 0=no, default=0).
	uint16_t 	UseEPH;		//  Include EPH markers (marker end of each packet header) (1=yes, 0=no, default=0).
	uint16_t 	Order;		//  Default progression order (see Progression Order, default=0=LRCP).
	uint16_t 	VBlk;		//  Nominal code-block dimensions: Vertical coordinate (default=64; 0 means default). Must be powers of 2, no less than 4 and no greater than 1024.
	uint16_t 	HBlk;		//  Nominal code-block dimensions: Horizontal coordinate (default=64; 0 means default). Must be powers of 2, no less than 4 and no greater than 1024.
	uint16_t 	VAlignBlkLast;	//  Last sample of VBlk aligned at a multiple of the block dimension (a power of 2) (1=yes, 0=no, default=0).
	uint16_t 	HAlignBlkLast;	//  Last sample of HBlk aligned at a multiple of the block dimension (a power of 2) (1=yes, 0=no, default=0).
	uint16_t 	Modes;		//  Block coder mode switches (see Block Coder Mode Switches, default=0).
	uint32_t 	SizeField1;	//  Size in bytes of the Field 1 codestream.
	uint32_t 	SizeField2;	//  Size in bytes of the Field 2 codestream.
	uint16_t 	Reserved1;	//  Reserved for future use, must be 0.
	uint16_t 	Reserved2;	//  Reserved for future use, must be 0.
	uint16_t 	ICCSize;	//  embedded ICC profile size (default=0).
	uint16_t 	J2CSize;	//  embedded J2C Main Header and Tile-parts Headers size (default=0). 
} morgan_jpeg2000extraheader;

typedef struct {
	morgan_jpeg2000infoheader2 i;
	morgan_jpeg2000extraheader e;
} AVI_vids_mj2c;

typedef struct {
    AVI_vidsB			b;
    morgan_jpeg2000infoheader2	i;
} AVI_strf_mj2c_small;

/* TODO */
typedef char *AVI_txts;
#endif
