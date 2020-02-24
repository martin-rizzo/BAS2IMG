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
#include "types.h"

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

int getBmpScanlineSize(int width, int numberOfColors);

Bool setBmpHeader(BmpHeader *header, int width, int height, int numberOfColors);

Bool extractBmpHeader(BmpHeader *header, const void* data, long dataSize);

Bool fwriteBmp(const BmpHeader *header,
               const void      *colorTable, int colorTableSize,
               const void      *pixelData , int pixelDataSize,
               FILE *file);

#endif /* bas2img_bmp_h */
