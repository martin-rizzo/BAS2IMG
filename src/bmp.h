/**
 * @file       bmp.h
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
#ifndef bas2img_bmp_h
#define bas2img_bmp_h
#include <stdio.h>
#include "globals.h"

typedef struct BmpHeader {
    unsigned fileType;
    unsigned fileSize;
    unsigned pixelDataOffset;
    unsigned headerSize;
    unsigned imageWidth;
    unsigned imageHeight;
    unsigned planes;
    unsigned bitsPerPixel;
    unsigned compression;
    unsigned pixelDataSize;
    unsigned totalColors;
    unsigned importantColors;
    unsigned scanlineSize;
} BmpHeader;

/**
 * Returns the exact number of bytes from one line of pixels to the next
 *
 * The BMP format needs the scanline's length to be a multiple of 4, this
 * function calculates that value based on the bmp width (in pixels) and
 * the number of bits of each pixel.
 * @param width         The width of the image in pixels
 * @param bitsPerPixel  The number of bits for each pixel
 */
int getBmpScanlineSize2(int width, int bitsPerPixel);

/**
 * Fills the BmpHeader structure with data that was read from a file
 * @param header      The BmpHeader structure to fill
 * @param data        A buffer containing data that was read from a BMP file
 * @param dataSize    The size of `data` in number of BYTES
 */
Bool fillBmpHeader(BmpHeader *header, const void* data, long dataSize);

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
               FILE*       file);

#endif /* bas2img_bmp_h */
