/**
 * @file       gif.c
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
#include "gif.h"
#define CHUNK_MAX_LENGTH 255 /* < maximum length of each data-chunk contained in the RASTER DATA BLOCK */

/*=================================================================================================================*/
#pragma mark - > BIT BUFFER

typedef struct BitBuffer {
    Byte array[CHUNK_MAX_LENGTH];
    unsigned byte;
    unsigned shift;
    unsigned index;
} BitBuffer;

static void initBitBuffer(BitBuffer* buffer) {
    buffer->byte  = 0;
    buffer->shift = 0;
    buffer->index = 0;
}


static Bool fwriteCode(unsigned code, unsigned length, BitBuffer* buffer, FILE* file) {
    
    int i; for (i=0; i<length; ++i) {
        buffer->byte |= (code&0x01) << buffer->shift++;
        if (buffer->shift>7) {
            buffer->array[buffer->index++] = buffer->byte;
            buffer->byte = buffer->shift = 0;
            if (buffer->index==CHUNK_MAX_LENGTH) {
                fputc(CHUNK_MAX_LENGTH,file);
                fwrite(buffer->array, sizeof(Byte), CHUNK_MAX_LENGTH, file);
                buffer->index = 0;
            }
        }
        code >>= 1;
    }
    return TRUE;
}

static void flushBitBuffer(BitBuffer* buffer, FILE* file) {
    if (buffer->shift>0) { buffer->array[buffer->index++] = buffer->byte; }
    if (buffer->index>0) {
        fputc(buffer->index,file);
        fwrite(buffer->array, sizeof(Byte), buffer->index, file);
    }
}




/*=================================================================================================================*/
#pragma mark - > WRITTING TO FILE

static Bool fwriteInt16(unsigned value, FILE *file) {
    unsigned char data[2];
    data[0] = (unsigned char)(value   );
    data[1] = (unsigned char)(value>>8);
    return 2==fwrite(data, 1, 2, file);
}

static Bool fwriteInt8(unsigned value, FILE *file) {
    return fputc(value,file)==value;
}

/**
 * Write a palette of colors into a GIF file
 * @param bgraColors       An array of colors in format BGRA (first byte=Blue, second byte=Green, ..)
 * @param sizeInBytes      The size of `bgraColors` in bytes
 * @param numberOfColors   Number of color of the palette to write
 * @param file             The output file where the palette will be written
 */
static Bool fwritePaletteBGRA(const Byte* bgraColors, int sizeInBytes, int numberOfColors, FILE *file) {
    int i; const Byte *last, *bgra; Byte rgb[3];
    assert( bgraColors!=NULL && sizeInBytes>0 );
    assert( numberOfColors>0 );
    assert( file!=NULL );
    
    bgra = bgraColors;
    last = &bgra[sizeInBytes-4];
    for (i=0; i<numberOfColors; ++i) {
        if (bgra<=last) { rgb[0]=bgra[2]; rgb[1]=bgra[1]; rgb[2]=bgra[0]; bgra+=4; }
        else            { rgb[0]=rgb[1]=rgb[2]=0; }
        if ( 3!=fwrite(rgb, 1, 3, file) ) { return FALSE; }
    }
    return TRUE;
}


/*=================================================================================================================*/
#pragma mark - > WRITTING GIF ELEMENTS TO FILE

/**
 * Writes the GIF header
 * @param width           The width of the image in pixels
 * @param height          The height of the image in pixels
 * @param bitsPerPixel    The number of bits for each pixel (valid values: 1 or 8)
 * @param colorTable      An array of RGBA elements that maps values in the pixel-data to rgb colors
 */
static Bool fwriteHeader(int         width,
                         int         height,
                         int         bitsPerPixel,
                         const void* colorTable,
                         int         colorTableSize,
                         FILE*       file)
{
    const int bitsPerComponent    = bitsPerPixel==1 ? 5 : 8;
    const int numberOfColors      = 1<<bitsPerPixel;
    const int useGlobalColorTable = 1; /* use GLOBAL color table */
    const int backgroundColor     = 0; /* background = index 0 */
    const int aspectRatio         = 0; /* No aspect ratio info is given */
    int flags;
    assert( width>0 && height>0 );
    assert( bitsPerPixel==1 || bitsPerPixel==8 );
    flags = useGlobalColorTable<<7 | (bitsPerComponent-1)<<4 | (bitsPerPixel-1);
    
    /* write signature */
    fputs("GIF89a",file);
    /* write screen descriptor */
    fwriteInt16(width          , file);
    fwriteInt16(height         , file);
    fwriteInt8 (flags          , file);
    fwriteInt8 (backgroundColor, file);
    fwriteInt8 (aspectRatio    , file);
    /* write color table */
    fwritePaletteBGRA(colorTable, colorTableSize, numberOfColors, file);
    return TRUE;
}

/**
 * Writes the GIF image descriptor to the specified file
 * @param width            The width of the image in pixels
 * @param height           The height of the image in pixels
 * @param bitsPerPixel     The number of bits for each pixel (valid values: 1 or 8)
 * @param file             The output file where the descriptor will be stored
 */
static Bool fwriteImageDescriptor(int   width,
                                  int   height,
                                  int   bitsPerPixel,
                                  FILE* file)
{
    const int useLocalColorTable = 0; /* not use local color table */
    const int interlace          = 0; /* image is NOT interlaced   */
    const int sorted             = 0; /* color table is NOT sorted */
    int fields;
    assert( width>0 && height>0 );
    assert( bitsPerPixel==1 || bitsPerPixel==8 );
    fields = useLocalColorTable<<7 | interlace<<6 | sorted<<5 | (bitsPerPixel-1);

    /* write image descriptor */
    fwriteInt8 ( 0x2C , file); /* image separator */
    fwriteInt16(  0   , file); /* image left position */
    fwriteInt16(  0   , file); /* image top position */
    fwriteInt16(width , file); /* image width */
    fwriteInt16(height, file); /* image height */
    fwriteInt8 (fields, file); /* packed fields */
    return TRUE;
}

/**
 * Writes the pixel data of a GIF image using LZW compression
 * @param width           The width of the image in pixels
 * @param height          The height of the image in pixels
 * @param scanlineSize    The number of bytes from one line of pixels to the next (negative = upside-down image)
 * @param bitsPerPixel    The number of bits for each pixel (valid values: 1 or 8)
 * @param pixelData       An array of values describing each pixel of the image
 * @param pixelDataSize   The size of `pixelData` in number of BYTES
 * @param file            The output file where the image will be written
 */
static Bool fwriteLzwImage(int         width,
                           int         height,
                           int         scanlineSize,
                           int         bitsPerPixel,
                           const void* pixelData,
                           int         pixelDataSize,
                           FILE*       file)
{
    int x,y; Bool upsideDown=FALSE;
    const unsigned initialCodeSize  = (bitsPerPixel>2) ? bitsPerPixel : 2;
    const unsigned clearCode        = 1 << initialCodeSize;
    const unsigned endOfInformation = clearCode+1;
    unsigned codeSize = initialCodeSize+1;
  /*unsigned code     = -1; */
    unsigned nextCode;

    BitBuffer buffer;
    const Byte *pixels, *scanline;
    
    assert( width>0 && height>0 );
    assert( bitsPerPixel==1 || /* bitsPerPixel==4 ||*/ bitsPerPixel==8 );
    
    /* handle "upside-down" images */
    if ( scanlineSize<0 ) { scanlineSize=-scanlineSize; upsideDown=TRUE; }
    assert( scanlineSize>=(width*bitsPerPixel/8) );
    
    initBitBuffer(&buffer);
    fputc(initialCodeSize,file);
    
    pixels = (const Byte*)pixelData;
    for (y=0; y<height; ++y) {
        scanline = &pixels[ scanlineSize * (upsideDown ? (height-y-1) : y) ];
        for (x=0; x<width; ++x) {
            
            /* get pixel color at position x,y */
            switch (bitsPerPixel) {
                default:
                case 8: nextCode = scanline[x]; break;
              /*case 4: nextCode = scanline[x/2]>>(4*(~x&1)) & 0x0F; break;*/
                case 1: nextCode = scanline[x/8]>>(~x&7)     & 0x01; break;
            }
            /* writting with no compression */
            fwriteCode( nextCode, codeSize,  &buffer,file);
            fwriteCode(clearCode, codeSize,  &buffer,file);
            
        }
    }
    
    /* fwriteCode( code       , codeSize        ,  &buffer,file); */
    /* fwriteCode( clearCode  , codeSize        ,  &buffer,file); */
    fwriteCode( endOfInformation, initialCodeSize,  &buffer,file);
    flushBitBuffer(&buffer, file);
    fputc(0, file);
    return TRUE;
}


/*=================================================================================================================*/
#pragma mark - > PUBLIC FUNCTIONS

/**
 * Writes an image to file using the GIF format
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
Bool fwriteGif(int         width,
               int         height,
               int         scanlineSize,
               int         bitsPerPixel,
               const void* colorTable,
               int         colorTableSize,
               const void* pixelData,
               int         pixelDataSize,
               FILE*       file)
{
    assert( width>0 && height>0 );
    assert( scanlineSize!=0 );
    assert( bitsPerPixel==1 || bitsPerPixel==8 );
    assert( colorTable!=NULL && colorTableSize>0 );
    assert( pixelData!=NULL && pixelDataSize>0 );
    assert( file!=NULL );
    
    fwriteHeader(width, height, bitsPerPixel, colorTable, colorTableSize, file);
    fwriteImageDescriptor(width, height, bitsPerPixel, file);
    fwriteLzwImage(width, height, scanlineSize, bitsPerPixel, pixelData, pixelDataSize, file);
    return TRUE;
}


