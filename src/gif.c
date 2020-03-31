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

static Bool fwriteRGB(const Byte* rgb, FILE *file) {
    return 3==fwrite(rgb, 1, 3, file);
}


/*=================================================================================================================*/
#pragma mark - > WRITTING GIF ELEMENTS TO FILE

/**
 * Writes the GIF header
 * @param width           The width of the image in pixels
 * @param height          The height of the image in pixels
 * @param numberOfColors  The number of colors of the image (valid values: 2 or 256)
 * @param colorTable      An array of RGBA elements that maps values in the pixel-data to rgb colors
 */
static Bool fwriteHeader(int         width,
                         int         height,
                         int         numberOfColors,
                         const void *colorTable,
                         int         colorTableSize,
                         FILE       *file)
{
    static Byte blackRGB[] = { 0,0,0,0 };
    const int bitsPerPixel        = numberOfColors==2 ? 1 : 8;
    const int bitsPerComponent    = numberOfColors==2 ? 5 : 8;
    const int useGlobalColorTable = 1; /* use GLOBAL color table */
    const int backgroundColor     = 0; /* background = index 0 */
    const int aspectRatio         = 0; /* No aspect ratio info is given */
    int flags,i; const Byte *rgb, *last;
    assert( width>0 && height>0 );
    assert( numberOfColors==2 || numberOfColors==256 );
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
    rgb  = colorTable;
    last = &rgb[colorTableSize-4];
    for (i=0; i<numberOfColors; ++i) {
        if (rgb<=last) { fwriteRGB(rgb,file); rgb+=4; }
        else           { fwriteRGB(blackRGB,file);    }
    }
    return TRUE;
}

/**
 * Writes the GIF image descriptor to the specified file
 * @param width            The width of the image in pixels
 * @param height           The height of the image in pixels
 * @param numberOfColors   The number of colors of the image (valid values: 2 or 256)
 * @param file             The output file where the descriptor will be stored
 */
static Bool fwriteImageDescriptor(int   width,
                                  int   height,
                                  int   numberOfColors,
                                  FILE *file)
{
    const int useLocalColorTable = 0; /* not use local color table */
    const int interlace          = 0; /* image is NOT interlaced   */
    const int sorted             = 0; /* color table is NOT sorted */
    const int bitsPerPixel = numberOfColors==2 ? 1 : 8;
    int fields;
    assert( width>0 && height>0 );
    assert( numberOfColors==2 || numberOfColors==256 );
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


/*=================================================================================================================*/
#pragma mark - > PUBLIC FUNCTIONS

/**
 * Writes an image to file using the GIF format
 * @param width           The width of the image in pixels
 * @param height          The height of the image in pixels
 * @param scanlineSize    The number of bytes from one line of pixels to the next
 * @param numberOfColors  The number of colors of the image (valid values: 2 or 256)
 * @param colorTable      An array of RGBA elements (32bits) that maps the values in the pixel-data to rgb colors
 * @param colorTableSize  The size of `colorTable` in number of BYTES
 * @param pixelData       An array of values describing each pixel of the image
 * @param pixelDataSize   The size of `pixelData` in number of BYTES
 */
Bool fwriteGif(int width, int height, int scanlineSize, int numberOfColors,
               const void *colorTable, int colorTableSize,
               const void *pixelData,  int pixelDataSize,
               FILE *file)
{
    assert( width >0 );
    assert( height>0 );
    assert( numberOfColors==2 || numberOfColors==256 );
    assert( file!=NULL );
    
    fwriteHeader(width, height, numberOfColors, colorTable, colorTableSize, file);
    fwriteImageDescriptor(width, height, numberOfColors, file);
    fwriteLzwImage(width, height, numberOfColors, pixelData, pixelDataSize, file);

    return TRUE;
}


