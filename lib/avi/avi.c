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
#include <inttypes.h>

#include <stdio.h>
#include <stdlib.h>
#include "avi/fourcc.h"
#include "avi/structures.h"
#include "avi/avi.h"

#include <sys/types.h>
#include <sys/unistd.h>

#include <asm/byteorder.h>

#include <trace.h>

static uint32_t	stream_type = 0;
static uint32_t	stream_codec = 0;

uint32_t avi_get_stype(void)
{
    return stream_type;
}

uint32_t avi_get_scodec(void)
{
    return stream_codec;
}

void avi_reset_stream_type(void)
{
	stream_type = 0;
};


void avi_trace_avih(AVI_avih *h, char *lvl)
{
	TRACE(0, "%18s %s|   MicroSecPerFrame %8"PRIu32"\n",   "", lvl, __le32_to_cpu(h->ulMicroSecPerFrame));
	TRACE(0, "%18s %s|     MaxBytesPerSec %8"PRIu32"\n",   "", lvl, __le32_to_cpu(h->ulMaxBytesPerSec));
	TRACE(0, "%18s %s| PaddingGranularity %08"PRIx32"H\n", "", lvl, __le32_to_cpu(h->ulPaddingGranularity));
	TRACE(0, "%18s %s|              Flags %08"PRIx32"\n",  "", lvl, __le32_to_cpu(h->ulFlags));
	TRACE(0, "%18s %s|        TotalFrames %8"PRIu32"\n",   "", lvl, __le32_to_cpu(h->ulTotalFrames));
	TRACE(0, "%18s %s|      InitialFrames %8"PRIu32"\n",   "", lvl, __le32_to_cpu(h->ulInitialFrames));
	TRACE(0, "%18s %s|            Streams %8"PRIu32"\n",   "", lvl, __le32_to_cpu(h->ulStreams));
	TRACE(0, "%18s %s|SuggestedBufferSize %08"PRIx32"H\n", "", lvl, __le32_to_cpu(h->ulSuggestedBufferSize));
	TRACE(0, "%18s %s|              Width %8"PRIu32"\n",   "", lvl, __le32_to_cpu(h->ulWidth));
	TRACE(0, "%18s %s|             Height %8"PRIu32"\n",   "", lvl, __le32_to_cpu(h->ulHeight));

	int i;
	for(i=0; i<sizeof(h->ulReserved)/sizeof(h->ulReserved[0]);i++) {
	      TRACE(0, "%18s %s|          Reserved[%d] %08"PRIx32"\n", "", lvl, i, __le32_to_cpu(h->ulReserved[i]));
	}

	return;
}

void avi_trace_strh(AVI_strh *h, char *lvl)
{

	TRACE(0, "%18s %s|       fccType ", "", lvl); PRN_FCC(h->fccType); TRACE(0, "\n");
	TRACE(0, "%18s %s|    fccHandler ", "", lvl); PRN_FCC(h->fccHandler); TRACE(0, "\n");

	stream_type = h->fccType.v32;
	stream_codec = h->fccHandler.v32;
	
	TRACE(0, "%18s %s|         Flags %08"PRIx32"H\n", "", lvl, __le32_to_cpu(h->ulFlags));
	TRACE(0, "%18s %s|      Priority %8"PRIu32"\n",   "", lvl, __le32_to_cpu(h->ulPriority));
	TRACE(0, "%18s %s| Initial Frame %8"PRIu32"\n",   "", lvl, __le32_to_cpu(h->ulInitialFrames));
	TRACE(0, "%18s %s|   FRate Scale %8"PRIu32"\n",   "", lvl, __le32_to_cpu(h->ulScale));
	TRACE(0, "%18s %s|   Frame  Rate %8"PRIu32"\n",   "", lvl, __le32_to_cpu(h->ulRate));
	TRACE(0, "%18s %s|  Start offset %8"PRIu32"\n",   "", lvl, __le32_to_cpu(h->ulStart));
	TRACE(0, "%18s %s|Frames counter %8"PRIu32"\n",   "", lvl, __le32_to_cpu(h->ulLength));
	TRACE(0, "%18s %s|RQ Buffer Size %08"PRIx32"H\n", "", lvl, __le32_to_cpu(h->ulSuggestedBufferSize));
	TRACE(0, "%18s %s|       Quality %8"PRIu32"\n",   "", lvl, __le32_to_cpu(h->ulQuality));
	TRACE(0, "%18s %s|Sample bl Size %08"PRIx32"H\n", "", lvl, __le32_to_cpu(h->ulSampleSize));
	TRACE(0, "%18s %s|           L T %8"PRIu16" %05"PRIu16"\n", "", lvl, __le16_to_cpu(h->ulrcFrame[0]), __le16_to_cpu(h->ulrcFrame[1]));
	TRACE(0, "%18s %s|           R B %8"PRIu16" %05"PRIu16"\n", "", lvl, __le16_to_cpu(h->ulrcFrame[2]), __le16_to_cpu(h->ulrcFrame[3]));

	return;
}

void avi_trace_vids_mj2c(AVI_vids_mj2c *h, char *lvl)
{
        TRACE(0, "%18s %s|MJP2000 H Size %08"PRIx32"H\n", "", lvl, __le32_to_cpu(h->i.Size));
	TRACE(0, "%18s %s|InterlaceFlags %8d\n",   "", lvl, __le16_to_cpu(h->i.InterlaceFlags));
	TRACE(0, "%18s %s|   WeightsMJP2 %8d\n",   "", lvl, __le16_to_cpu(h->i.WeightsMJP2));
	TRACE(0, "%18s %s|  ColorSpaceID %8d\n",   "", lvl, __le16_to_cpu(h->i.ColorSpaceID));
	TRACE(0, "%18s %s|   NativeOrder %8d\n",   "", lvl, __le16_to_cpu(h->i.NativeOrder));
	TRACE(0, "%18s %s| BitsPerSample %8d\n",   "", lvl, __le16_to_cpu(h->i.BitsPerSample));
	TRACE(0, "%18s %s|  SampleSigned %8d\n",   "", lvl, __le16_to_cpu(h->i.SampleSigned));
	TRACE(0, "%18s %s|  VSubSampling %8d\n",   "", lvl, __le16_to_cpu(h->i.VSubSampling));
	TRACE(0, "%18s %s|       VOffset %8d\n",   "", lvl, __le16_to_cpu(h->i.VOffset));
	TRACE(0, "%18s %s|  HSubSampling %8d\n",   "", lvl, __le16_to_cpu(h->i.HSubSampling));
	TRACE(0, "%18s %s|       HOffset %8d\n",   "", lvl, __le16_to_cpu(h->i.HOffset));
	TRACE(0, "%18s %s|        Levels %8d\n",   "", lvl, __le16_to_cpu(h->i.Levels));
	TRACE(0, "%18s %s|       Process %8d\n",   "", lvl, __le16_to_cpu(h->i.Process));
	TRACE(0, "%18s %s|        Layers %8d\n",   "", lvl, __le16_to_cpu(h->i.Layers));
	TRACE(0, "%18s %s|       Quality %8d\n",   "", lvl, __le16_to_cpu(h->i.Quality));
	TRACE(0, "%18s %s|           CBR %8d\n",   "", lvl, __le16_to_cpu(h->i.CBR));
	TRACE(0, "%18s %s|       BitRate %8d\n",   "", lvl, __le16_to_cpu(h->i.BitRate));
	TRACE(0, "%18s %s|      Lossless %8d\n",   "", lvl, __le16_to_cpu(h->i.Lossless));
	TRACE(0, "%18s %s|     ExtraSize %8d\n",   "", lvl, __le16_to_cpu(h->i.ExtraSize));

	if(!__le16_to_cpu(h->i.ExtraSize)) {
		return;
	}
	
	TRACE(0, "%18s %s|       Profile %8d\n",   "", lvl, __le16_to_cpu(h->e.Profile));
	TRACE(0, "%18s %s|         VSize %8d\n",   "", lvl, __le16_to_cpu(h->e.VSize));
	TRACE(0, "%18s %s|         HSize %8d\n",   "", lvl, __le16_to_cpu(h->e.HSize));
	TRACE(0, "%18s %s|       VOrigin %8d\n",   "", lvl, __le16_to_cpu(h->e.VOrigin));
	TRACE(0, "%18s %s|       HOrigin %8d\n",   "", lvl, __le16_to_cpu(h->e.HOrigin));
	TRACE(0, "%18s %s|         VTile %8d\n",   "", lvl, __le16_to_cpu(h->e.VTile));
	TRACE(0, "%18s %s|         HTile %8d\n",   "", lvl, __le16_to_cpu(h->e.HTile));
	TRACE(0, "%18s %s|      VTOrigin %8d\n",   "", lvl, __le16_to_cpu(h->e.VTOrigin));
	TRACE(0, "%18s %s|      HTOrigin %8d\n",   "", lvl, __le16_to_cpu(h->e.HTOrigin));
	TRACE(0, "%18s %s|        UseSOP %8d\n",   "", lvl, __le16_to_cpu(h->e.UseSOP));
	TRACE(0, "%18s %s|        UseEPH %8d\n",   "", lvl, __le16_to_cpu(h->e.UseEPH));
	TRACE(0, "%18s %s|         Order %8d\n",   "", lvl, __le16_to_cpu(h->e.Order));
	TRACE(0, "%18s %s|          VBlk %8d\n",   "", lvl, __le16_to_cpu(h->e.VBlk));
	TRACE(0, "%18s %s|          HBlk %8d\n",   "", lvl, __le16_to_cpu(h->e.HBlk));
	TRACE(0, "%18s %s| VAlignBlkLast %8d\n",   "", lvl, __le16_to_cpu(h->e.VAlignBlkLast));
	TRACE(0, "%18s %s| HAlignBlkLast %8d\n",   "", lvl, __le16_to_cpu(h->e.HAlignBlkLast));
	TRACE(0, "%18s %s|         Modes %8d\n",   "", lvl, __le16_to_cpu(h->e.Modes));
	TRACE(0, "%18s %s|    SizeField1 %8"PRIu32"\n",   "", lvl, __le32_to_cpu(h->e.SizeField1));
	TRACE(0, "%18s %s|    SizeField2 %8"PRIu32"\n",   "", lvl, __le32_to_cpu(h->e.SizeField2));
	TRACE(0, "%18s %s|     Reserved1 %8d\n",   "", lvl, __le16_to_cpu(h->e.Reserved1));
	TRACE(0, "%18s %s|     Reserved2 %8d\n",   "", lvl, __le16_to_cpu(h->e.Reserved2));
	TRACE(0, "%18s %s|       ICCSize %8d\n",   "", lvl, __le16_to_cpu(h->e.ICCSize));
	TRACE(0, "%18s %s|       J2CSize %8d\n",   "", lvl, __le16_to_cpu(h->e.J2CSize));
}


/*\
 *
 *  biSize
 *    Specifies the size of the structure, in bytes. This size does not include the color table or the masks mentioned in the biClrUsed member. See the Remarks section for more information. 
 *  
 *  biWidth
 *    Specifies the width of the bitmap, in pixels. 
 *  
 *  biHeight
 *    Specifies the height of the bitmap, in pixels. If biHeight is positive, the bitmap is a bottom-up DIB and its origin is the lower-left corner. If biHeight is negative, the bitmap is a top-down DIB and its origin is the upper-left corner.
 *
 *    If biHeight is negative, indicating a top-down DIB, biCompression must be either BI_RGB or BI_BITFIELDS. Top-down DIBs cannot be compressed. 
 *  
 *  biPlanes
 *    Specifies the number of planes for the target device. This value must be set to 1. 
 *  
 *  biBitCount
 *    Specifies the number of bits per pixel. The biBitCount member of the BITMAPINFOHEADER structure determines the number of bits that define each pixel and the maximum number of colors in the bitmap. This member must be one of the following values.
 *    Value 	Description
 *     1 	The bitmap is monochrome, and the bmiColors member contains two entries. Each bit in the bitmap array represents a pixel. If the bit is clear, the pixel is displayed with the color of the first entry in the bmiColors table; if the bit is set, the pixel has the color of the second entry in the table.
 *     2 	The bitmap has four possible color values.
 *     4 	The bitmap has a maximum of 16 colors, and the bmiColors member contains up to 16 entries. Each pixel in the bitmap is represented by a 4-bit index into the color table. For example, if the first byte in the bitmap is 0x1F, the byte represents two pixels. The first pixel contains the color in the second table entry, and the second pixel contains the color in the sixteenth table entry.
 *     8 	The bitmap has a maximum of 256 colors, and the bmiColors member contains up to 256 entries. In this case, each byte in the array represents a single pixel.
 *    16 	The bitmap has a maximum of 2^16 colors. If the biCompression member of the BITMAPINFOHEADER is BI_RGB, the bmiColors member is NULL. Each WORD in the bitmap array represents a single pixel. The relative intensities of red, green, and blue are represented with 5 bits for each color component. The value for blue is in the least significant 5 bits, followed by 5 bits each for green and red. The most significant bit is not used. The bmiColors color table is used for optimizing colors used on palette-based devices, and must contain the number of entries specified by the biClrUsed member of the BITMAPINFOHEADER.
 *    24 	The bitmap has a maximum of 2^24 colors, and the bmiColors member is NULL. Each 3-byte triplet in the bitmap array represents the relative intensities of blue, green, and red, respectively, for a pixel. The bmiColors color table is used for optimizing colors used on palette-based devices, and must contain the number of entries specified by the biClrUsed member of the BITMAPINFOHEADER.
 *    32 	The bitmap has a maximum of 2^32 colors. If the biCompression member of the BITMAPINFOHEADER is BI_RGB, the bmiColors member is NULL. Each DWORD in the bitmap array represents the relative intensities of blue, green, and red, respectively, for a pixel. The high byte in each DWORD is not used. The bmiColors color table is used for optimizing colors used on palette-based devices, and must contain the number of entries specified by the biClrUsed member of the BITMAPINFOHEADER.
 *
 *    If the biCompression member of the BITMAPINFOHEADER is BI_BITFIELDS, the bmiColors member contains three DWORD color masks that specify the red, green, and blue components, respectively, of each pixel. Each DWORD in the bitmap array represents a single pixel.
 *  
 *  biCompression
 *    Specifies the type of compression for a compressed bottom-up bitmap (top-down DIBs cannot be compressed). This member can be the one of the following values.
 *    Value 	Description
 *    BI_RGB 	An uncompressed format.
 *    BI_BITFIELDS 	Specifies that the bitmap is not compressed and that the color table consists of three DWORD color masks that specify the red, green, and blue components, respectively, of each pixel. This is valid when used with 16- and 32-bpp bitmaps. This value is valid in Windows CE versions 2.0 and later.
 *  
 *  biSizeImage
 *    Specifies the size, in bytes, of the image. This may be set to zero for BI_RGB
 *   bitmaps. 
 *  
 *  biXPelsPerMeter
 *    Specifies the horizontal resolution, in pixels per meter, of the target device
 *   for the bitmap. An application can use this value to select a bitmap from a
 *   resource group that best matches the characteristics of the current device. 
 *  
 *  biYPelsPerMeter
 *    Specifies the vertical resolution, in pixels per meter, of the target device for
 *   the bitmap 
 *  
 *  biClrUsed
 *    Specifies the number of color indexes in the color table that are actually used by the bitmap. If this value is zero, the bitmap uses the maximum number of colors corresponding to the value of the biBitCount member for the compression mode specified by biCompression.
 *
 *    If biClrUsed is nonzero and the biBitCount member is less than 16, the biClrUsed member specifies the actual number of colors the graphics engine or device driver accesses. If biBitCount is 16 or greater, the biClrUsed member specifies the size of the color table used to optimize performance of the system color palettes. If biBitCount equals 16 or 32, the optimal color palette starts immediately following the three DWORD masks.
 *
 *    If the bitmap is a packed bitmap (a bitmap in which the bitmap array immediately follows the BITMAPINFO header and is referenced by a single pointer), the biClrUsed member must be either zero or the actual size of the color table. 
 *  
 *  biClrImportant
 *    Specifies the number of color indexes required for displaying the bitmap. If this value is zero, all colors are required. 
 *
\*/
void avi_trace_vids(AVI_vids *h, char *lvl)
{
	AVI_vidsB *b =(typeof(b))h;

	TRACE(0, "%18s %s|         lSize %8"PRIu32"\n",   "", lvl, __le32_to_cpu(b->lSize));
	TRACE(0, "%18s %s|        lWidth %8"PRIu32"\n",   "", lvl, __le32_to_cpu(b->lWidth));
	TRACE(0, "%18s %s|       lHeight %8"PRIu32"\n",   "", lvl, __le32_to_cpu(b->lHeight));
	TRACE(0, "%18s %s|       wPlanes %5"PRIu16"\n",   "", lvl, __le16_to_cpu(b->wPlanes));
	TRACE(0, "%18s %s|     wBitCount %5"PRIu16"\n",   "", lvl, __le16_to_cpu(b->wBitCount));
	TRACE(0, "%18s %s|  lCompression ", "", lvl); PRN_FCC(b->lCompression); TRACE(0, "\n");
	TRACE(0, "%18s %s|    lSizeImage %8"PRIu32"\n",   "", lvl, __le32_to_cpu(b->lSizeImage    ));
	TRACE(0, "%18s %s|wXPelsPerMeter %8"PRIu32"\n",   "", lvl, __le32_to_cpu(b->wXPelsPerMeter));
	TRACE(0, "%18s %s|wYPelsPerMeter %8"PRIu32"\n",   "", lvl, __le32_to_cpu(b->wYPelsPerMeter));
	TRACE(0, "%18s %s|      lClrUsed %8"PRIu32"\n",   "", lvl, __le32_to_cpu(b->lClrUsed      ));
	TRACE(0, "%18s %s| lClrImportant %8"PRIu32"\n",   "", lvl, __le32_to_cpu(b->lClrImportant ));

        switch(stream_codec) {
	    case  fcc_strf_mj2c:
	    case  fcc_strf_MJ2C:
		avi_trace_vids_mj2c((AVI_vids_mj2c*)(b+1), lvl-2);
	}
}
/*\
 *  wFormatTag
 *     Waveform-audio format type. Format tags are registered with Microsoft Corporation
 *    for many compression algorithms. A complete list of format tags can be found in the
 *    Mmreg.h header file.
 *
 *  nChannels
 *     Number of channels in the waveform-audio data. Monaural data uses one channel and
 *    stereo data uses two channels.
 *
 *  nSamplesPerSec
 *     Sample rate, in samples per second (Hertz), that each channel should be played or
 *    recorded. If wFormatTag is WAVE_FORMAT_PCM, then common values for nSamplesPerSec
 *    are 8.0 kHz, 11.025 kHz, 22.05 kHz, and 44.1 kHz. For non-PCM formats, this member
 *    must be computed according to the manufacturer's specification of the format tag.
 *
 *  nAvgBytesPerSec
 *     Required average data-transfer rate, in bytes per second, for the format tag.
 *    If wFormatTag is WAVE_FORMAT_PCM, nAvgBytesPerSec should be equal to the product
 *    of nSamplesPerSec and nBlockAlign. For non-PCM formats, this member must be
 *    computed according to the manufacturer's specification of the format tag.
 *
 *     Playback and record software can estimate buffer sizes by using the
 *    nAvgBytesPerSec member.
 *
 *  nBlockAlign
 *     Block alignment, in bytes. The block alignment is the minimum atomic unit of data
 *    for the wFormatTag format type. If wFormatTag is WAVE_FORMAT_PCM, nBlockAlign
 *    should be equal to the product of nChannels and wBitsPerSample divided by 8 (bits
 *    per byte). For non-PCM formats, this member must be computed according to the
 *    manufacturer's specification of the format tag.
 *
 *     Playback and record software must process a multiple of nBlockAlign bytes of data
 *    at a time. Data written and read from a device must always start at the beginning
 *    of a block. For example, it is illegal to start playback of PCM data in the middle
 *    of a sample (that is, on a non-block-aligned boundary). 
 *
 *  wBitsPerSample
 *     Bits per sample for the wFormatTag format type. If wFormatTag is WAVE_FORMAT_PCM,
 *    then wBitsPerSample should be equal to 8 or 16. For non-PCM formats, this member
 *    must be set according to the manufacturer's specification of the format tag. Some
 *    compression schemes cannot define a value for wBitsPerSample, so this member can
 *    be zero.
 *     
 *  cbSize
 *     Size, in bytes, of extra format information appended to the end of the
 *    WAVEFORMATEX structure. This information can be used by non-PCM formats to store
 *    extra attributes for the wFormatTag. If no extra information is required by the
 *    wFormatTag, this member must be set to zero. For WAVE_FORMAT_PCM formats only,
 *    this member is ignored. 
\*/
void avi_trace_auds(AVI_auds *h, char *lvl)
{
	TRACE(0, "%18s %s|    wFormatTag %8d\n",   "", lvl, __le16_to_cpu(h->wFormatTag));
	TRACE(0, "%18s %s|     wChannels %8d\n",   "", lvl, __le16_to_cpu(h->wChannels));
	TRACE(0, "%18s %s|lSamplesPerSec %8"PRIu32"\n",   "", lvl, __le32_to_cpu(h->lSamplesPerSec));
	TRACE(0, "%18s %s|AvgBytesPerSec %8"PRIu32"\n",   "", lvl, __le32_to_cpu(h->lAvgBytesPerSec));
	TRACE(0, "%18s %s|   wBlockAlign %8d\n",   "", lvl, __le16_to_cpu(h->wBlockAlign));
	TRACE(0, "%18s %s|wBitsPerSample %8d\n",   "", lvl, __le16_to_cpu(h->wBitsPerSample));
	TRACE(0, "%18s %s|         wSize %8d\n",   "", lvl, __le16_to_cpu(h->wSize));

	return;
}

void avi_trace_txts(AVI_txts *h, char *lvl)
{

	return;
}

void avi_trace_strf(void *h, char *lvl)
{

	switch(stream_type) {
	    case fcc_vids:
		return avi_trace_vids((AVI_vids*)h, lvl);
	    case fcc_auds:
		return avi_trace_auds((AVI_auds*)h, lvl);
	    case fcc_txts:
		return avi_trace_txts((AVI_txts*)h, lvl);
	}
	return;
}

/*static*/ void avi_trace_(void *h, char *lvl)
{

	return;
}
