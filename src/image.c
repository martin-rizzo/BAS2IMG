/**
 * @file       image.c
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
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"
#include "bmp.h"

#define swap(a,b) temp=(a); (a)=(b); (b)=temp
#define min(a,b)  ((a)<(b) ? (a) : (b))
#define lerp256(v0, v1, t) ( ((256-t)*v0 + t*v1) / 256 )
#define CHARWIDTH  8
#define CHARHEIGHT 8


/**
 * Allocates a new image with a specific size
 * @param width         The width of the image
 * @param height        The height of the image
 */
Image * allocImage(int width, int height) {
    Image *image; int scanlineSize;
    assert( width>0 && height>0 );
    
    /* the most compatible scanline size */
    scanlineSize = getBmpScanlineSize(width,256);
    
    image = malloc(sizeof(Image));
    image->width          = width;
    image->height         = height;
    image->scanlineSize   = scanlineSize;
    image->colorTableSize = 256 * 4 * sizeof(Byte);
    image->colorTable     = malloc(image->colorTableSize);
    image->pixelDataSize  = image->height * image->scanlineSize;
    image->pixelData      = malloc(image->pixelDataSize);
    image->curColor       = 255;
    image->curFont        = NULL;
    memset(image->colorTable,0,image->colorTableSize);
    memset(image->pixelData,0,image->pixelDataSize);
    return image;
}

/**
 * Deallocate an image previously allocated with 'allocImage(..)'
 */
void freeImage(Image *image) {
    free(image->colorTable);
    free(image->pixelData);
    free(image);
}


/*=================================================================================================================*/
#pragma mark - > DRAWING


void setPaletteGradient(Image *image, int index0, Rgb rgb0, int index1, Rgb rgb1) {
    const int range = index1-index0;
    int i,t; Byte *ptr;

    ptr = &image->colorTable[4*index0];
    for (i=0; i<=range; ++i) {
        t = i*256/range;
        *ptr++ = lerp256(rgb0.b, rgb1.b, t);
        *ptr++ = lerp256(rgb0.g, rgb1.g, t);
        *ptr++ = lerp256(rgb0.r, rgb1.r, t);
        *ptr++ = 0;
    }
}


void setColor(Image *image, int color) {
    assert( image!=NULL );
    image->curColor = color;
}

void setFont(Image *image, const Font *font) {
    assert( image!=NULL );
    image->curFont = font;
}

/**
 * Draws a character at specified position using the current color and font
 * @param image      the image where the character will be drawn
 * @param x          the X coordinate of the top-left corner of the ch
 * @param y          the Y coordinate of the top-left corner
 * @param maxWidth   maximum width available to draw the character
 * @param maxHeight  maximum height available to draw the character
 * @param charIndex  index of the character to draw (ex: 65 -> "A")
 */
void drawChar(Image *image, int x, int y, int maxWidth, int maxHeight, Char256 charIndex) {
    const Byte *sour;
    Byte *dest;
    int i, j, segment, mask, color, scanlineSize;
    const int charWidth  = min(maxWidth ,CHARWIDTH );
    const int charHeight = min(maxHeight,CHARHEIGHT);
    assert( image!=NULL );
    
    if (!image->curFont) { return; }
    
    scanlineSize = image->scanlineSize;
    sour         = &image->curFont->data[charIndex*CHARHEIGHT];
    dest         = &image->pixelData[y*scanlineSize + x];
    color        = image->curColor;
    for (j=0; j<charHeight; ++j) {
        segment=*sour++; mask=0x80;
        for (i=0; i<charWidth; ++i) {
            if (segment&mask) { *dest=color; }
            ++dest; mask>>=1;
        }
        dest += (scanlineSize - charWidth);
    }
}

void fillRectangle(Image *image, int left, int top, int right, int bottom) {
    Byte *ptr; int width, height, scanlineSize, color, temp;
    
    /* fix rectangles with inverted coordinates */
    if (left>right) { swap(left,right); }
    if (top>bottom) { swap(top,bottom); }
    
    /* clipping */
    if (left<=0) { left=0; } else if (left>=image->width) { return; }
    if (top <=0) { top =0; } else if (top>=image->height) { return; }
    if (right>=image->width) { right=image->width; } else if (right<=0 ) { return; }
    if (bottom>=image->height) { bottom=image->height; } else if (bottom<=0 ) { return; }
    
    /* filling */
    width=right-left;
    if (width>0) {
        scanlineSize = image->scanlineSize;
        color        = image->curColor;
        ptr          = &image->pixelData[top*scanlineSize + left];
        height=bottom-top; while (height-->0) {
            memset(ptr,color,width);
            ptr += scanlineSize;
        }
    }
}


/*=================================================================================================================*/
#pragma mark - > WRITTING IMAGE TO A FILE

Bool fwriteBmpImage(Image *image, FILE *file) {
    BmpHeader header;
    setBmpHeader(&header, image->width, -image->height, 256);
    return fwriteBmp(&header, image->colorTable, image->colorTableSize, image->pixelData, image->pixelDataSize, file);
}

Bool fwriteGifImage(Image *image, FILE *file) {
    assert( FALSE );
    return FALSE;
}

