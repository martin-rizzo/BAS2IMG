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
#include <assert.h>
#include <stdlib.h>
#include "bmp.h"
#define getInt16(ptr) (ptr[0] | ptr[1]<<8); ptr+=2;
#define getInt32(ptr) (ptr[0] | ptr[1]<<8 | ptr[2]<<16 | ptr[3]<<24); ptr+=4;
#define getScanlineSize(width,bitsPerPixel) ((width)*(bitsPerPixel)+31)/32 * 4
#define FileHeaderSize    14
#define BmpInfoHeaderSize 40
#define XPixelsPerMeter   2834
#define YPixelsPerMeter   2834



/*=================================================================================================================*/
#pragma mark - > BITMAP HEADER

void initBmpHeader(BmpHeader* header,
                   int        width,
                   int        height,
                   int        bitsPerPixel)
{
    const int scanlineSize   = getScanlineSize(width, bitsPerPixel);
    const int colorTableSize = 4 * (1<<bitsPerPixel);
    const int pixelDataSize  = scanlineSize * height;
    assert( width  >  0 );
    assert( height != 0 );
    assert( bitsPerPixel==1 || bitsPerPixel==4 || bitsPerPixel==8 );
    
    header->fileType        = 0x4D42;
    header->pixelDataOffset = FileHeaderSize + BmpInfoHeaderSize + colorTableSize;
    header->fileSize        = header->pixelDataOffset + pixelDataSize;
    header->headerSize      = BmpInfoHeaderSize;
    header->imageWidth      = width;
    header->imageHeight     = height;
    header->planes          = 1;
    header->bitsPerPixel    = bitsPerPixel;
    header->pixelDataSize   = pixelDataSize;
    header->compression     = FALSE;
    header->totalColors     = 0;
    header->importantColors = 0;
    header->scanlineSize    = scanlineSize;
}


/*=================================================================================================================*/
#pragma mark - > WRITTING TO FILE

static int fwriteInt16(unsigned value, FILE *file) {
    unsigned char data[2];
    data[0] = (unsigned char)(value   );
    data[1] = (unsigned char)(value>>8);
    return 2==fwrite(data, 1, 2, file);
}

static int fwriteInt32(unsigned value, FILE *file) {
    unsigned char data[4];
    data[0] = (unsigned char)(value    );
    data[1] = (unsigned char)(value>> 8);
    data[2] = (unsigned char)(value>>16);
    data[3] = (unsigned char)(value>>24);
    return 4==fwrite(data, 1, 4, file);
}

static int fwriteData(const void* data, long dataSize, FILE *file) {
    return dataSize==(long)fwrite(data, 1, dataSize, file);
}

static int fwriteBmpHeader(const BmpHeader *header, FILE *file) {
    return
    fwriteInt16(header->fileType       , file) &&
    fwriteInt32(header->fileSize       , file) &&
    fwriteInt32(           0           , file) &&
    fwriteInt32(header->pixelDataOffset, file) &&
    fwriteInt32(header->headerSize     , file) &&
    fwriteInt32(header->imageWidth     , file) &&
    fwriteInt32(header->imageHeight    , file) &&
    fwriteInt16(header->planes         , file) &&
    fwriteInt16(header->bitsPerPixel   , file) &&
    fwriteInt32(header->compression    , file) &&
    fwriteInt32(header->pixelDataSize  , file) &&
    fwriteInt32(XPixelsPerMeter        , file) &&
    fwriteInt32(YPixelsPerMeter        , file) &&
    fwriteInt32(header->totalColors    , file) &&
    fwriteInt32(header->importantColors, file);
}


/*=================================================================================================================*/
#pragma mark - > PUBLIC FUNCTIONS

/**
 * Returns the exact number of bytes from one line of pixels to the next
 *
 * The BMP format needs the scanline's length to be a multiple of 4, this
 * function calculates that value based on the bmp width (in pixels) and
 * the number of bits of each pixel.
 * @param width         The width of the image in pixels
 * @param bitsPerPixel  The number of bits for each pixel
 */
int getBmpScanlineSize2(int width, int bitsPerPixel) {
    assert( width>0 && bitsPerPixel>0 );
    return getScanlineSize(width, bitsPerPixel);
}

/**
 * Fills the BmpHeader structure with data that was read from a file
 * @param header      The BmpHeader structure to fill
 * @param data        A buffer containing data that was read from a BMP file
 * @param dataSize    The size of `data` in number of BYTES
 */
Bool fillBmpHeader(BmpHeader *header, const void* data, long dataSize) {
    const Byte *ptr;
    assert( header!=NULL && data!=NULL && dataSize>0 );
    
    ptr = (Byte*)data;
    if (dataSize<54)                   { return FALSE; }
    if (ptr[0]!=0x42 || ptr[1]!=0x4D ) { return FALSE; }
    header->fileType        = getInt16(ptr);
    header->fileSize        = getInt32(ptr);
    ptr+=4; /* skip reserved values */
    header->pixelDataOffset = getInt32(ptr);
    header->headerSize      = getInt32(ptr);
    header->imageWidth      = getInt32(ptr);
    header->imageHeight     = getInt32(ptr);
    header->planes          = getInt16(ptr);
    header->bitsPerPixel    = getInt16(ptr);
    header->compression     = getInt32(ptr);
    header->pixelDataSize   = getInt32(ptr);
    ptr+=8; /* skip xPixelsPerMeter & yPixelsPerMeter */
    header->totalColors     = getInt32(ptr);
    header->importantColors = getInt32(ptr);
    header->scanlineSize    = getScanlineSize(header->imageWidth, header->bitsPerPixel);
    return TRUE;
}


/*
Bool fwriteBmp(const BmpHeader *header,
               const void      *colorTable, int colorTableSize,
               const void      *pixelData , int pixelDataSize,
               FILE *file) {
    
    return
    fwriteBmpHeader(header, file)                &&
    fwriteData(colorTable, colorTableSize, file) &&
    fwriteData(pixelData,  pixelDataSize,  file);
}
*/


/**
 * Writes an image to file using the BMP format
 * @param width           The width of the image in pixels
 * @param height          The height of the image in pixels
 * @param scanlineSize    The number of bytes from one line of pixels to the next (negative = upside-down image)
 * @param bitsPerPixel    The number of bits for each pixel (valid values: 1 or 8)
 * @param colorTable      An array of RGBA elements (32bits) that maps the values in the pixel-data to rgb colors
 * @param colorTableSize  The size of `colorTable` in number of BYTES
 * @param pixelData       An array of values describing each pixel of the image
 * @param pixelDataSize   The size of `pixelData` in number of BYTES
 * @param file            The output file where the image will be written
 */
Bool fwriteBmp(int         width,
               int         height,
               int         scanlineSize,
               int         bitsPerPixel,
               const void* colorTable,
               int         colorTableSize,
               const void* pixelData,
               int         pixelDataSize,
               FILE*       file)
{
    BmpHeader header;
    assert( width>0 && height>0 );
    assert( scanlineSize==getBmpScanlineSize2(width,bitsPerPixel) );
    assert( bitsPerPixel==1 || bitsPerPixel==8 );
    assert( colorTable!=NULL && colorTableSize>0 );
    assert( pixelData!=NULL && pixelDataSize>0 );
    assert( file!=NULL );
    
    /* handle upside-down image */
    if (scanlineSize<0) {
        height       = -height;
        scanlineSize = -scanlineSize;
    }
    /* generate bmp-header and check if provided `scanlineSize` matches with BMP format */
    initBmpHeader(&header, width, height, bitsPerPixel);
    assert( header.scanlineSize==scanlineSize );
    
    /* write data using bmp format */
    return
        fwriteBmpHeader(&header, file)               &&
        fwriteData(colorTable, colorTableSize, file) &&
        fwriteData(pixelData,  pixelDataSize,  file);
}
