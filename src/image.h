/**
 * @file       image.h
 * @date       Feb 24, 2020
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
#ifndef bas2img_image_h
#define bas2img_image_h
#include <stdio.h>
#include "types.h"


typedef struct Image {
    unsigned  width;
    unsigned  height;
    unsigned  scanlineSize;
    Byte     *colorTable;
    unsigned  colorTableSize;
    Byte     *pixelData;
    unsigned  pixelDataSize;
    
    int         curColor;  /* < current color (palette index) */
    const Font *curFont;   /* < current font (NULL = none) */
} Image;


/*=================================================================================================================*/
#pragma mark - > ALLOCATION / DEALLOCATION

/**
 * Allocates a new image with a specific size
 * @param width         The width of the image
 * @param height        The height of the image
 */
Image * allocImage(int width, int height);

/**
 * Deallocate an image previously allocated with 'allocImage(..)'
 */
void freeImage(Image *image);


/*=================================================================================================================*/
#pragma mark - > DRAWING


void setPaletteGradient(Image *image, int index0, Rgb rgb0, int index1, Rgb rgb1);

void setColor(Image *image, int color);

void setFont(Image *image, const Font *font);

void putChar(Image *image, int x, int y, int chIndex);

void fillRectangle(Image *image, int x0, int y0, int x1, int y1);


/*=================================================================================================================*/
#pragma mark - > WRITTING IMAGE TO A FILE


Bool fwriteBmpImage(Image *image, FILE *file);

Bool fwriteGifImage(Image *image, FILE *file);


#endif /* bas2img_image_h */
