/**
 * @file       bmp.c
 * @date       Feb 18, 2020
 * @author     Martin Rizzo | <martinrizzo@gmail.com>
 * @copyright  Copyright (c) 2020 Martin Rizzo.
 *             This project is released under the MIT License.
 * -------------------------------------------------------------------------
 *  BAS2IMG - The "source code to image" converter for BASIC language
 * -------------------------------------------------------------------------
 *  Copyright (c) 2020 Martin Rizzo
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * -------------------------------------------------------------------------
 */
#include <stdlib.h>
#include <assert.h>
#include "bmp.h"
#define getInt16(ptr) (ptr[0] | ptr[1]<<8); ptr+=2;
#define getInt32(ptr) (ptr[0] | ptr[1]<<8 | ptr[2]<<16 | ptr[3]<<24); ptr+=4;
typedef int Bool; enum { FALSE=0, TRUE }; /* < Boolean */

int loadBitmapHeader(BitmapHeader *bmp, const void* data, long dataSize) {
    const char *ptr;
    assert( bmp!=NULL && data!=NULL );
    
    ptr = (const char*)data;
    if (dataSize<54)                   { return FALSE; }
    if (ptr[0]!=0x42 || ptr[1]!=0x4D ) { return FALSE; }
    bmp->fileType        = getInt16(ptr);
    bmp->fileSize        = getInt32(ptr);
    ptr+=4; /* skip reserved values */
    bmp->pixelDataOffset = getInt32(ptr);
    bmp->headerSize      = getInt32(ptr);
    bmp->imageWidth      = getInt32(ptr);
    bmp->imageHeight     = getInt32(ptr);
    bmp->planes          = getInt16(ptr);
    bmp->bitsPerPixel    = getInt16(ptr);
    bmp->compression     = getInt32(ptr);
    ptr+=12; /* skip imageSize, xPixelsPerMeter & yPixelsPerMeter */
    bmp->totalColors     = getInt32(ptr);
    bmp->importantColors = getInt32(ptr);
    bmp->scanlineSize    = ((bmp->imageWidth*bmp->bitsPerPixel-1)/32+1)*4;
    /*
    DLOG(("fileType: %d", bmp.fileType));
    DLOG(("fileSize: %d", bmp.fileSize));
    DLOG(("pixelDataOffset: %d", bmp.pixelDataOffset));
    DLOG(("headerSize: %d", bmp.headerSize));
    DLOG(("imageWidth: %d", bmp.imageWidth));
    DLOG(("imageHeight: %d", bmp.imageHeight));
    DLOG(("planes: %d", bmp.planes));
    DLOG(("bitsPerPixel: %d", bmp.bitsPerPixel));
    DLOG(("compression: %d", bmp.compression));
    DLOG(("totalColors: %d", bmp.totalColors));
    DLOG(("importantColors: %d", bmp.importantColors));
    DLOG(("scanlineSize: %d", bmp.scanlineSize));
    */
    return TRUE;
}

