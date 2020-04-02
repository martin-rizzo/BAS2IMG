/**
 * @file       export.c
 * @date       Feb 16, 2020
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
#include <stdio.h>
#include "bas2img.h"
#include "helpers.h"
#include "error.h"
#include "bmp.h"

/**
 * Stores a font image of 1-bit-per-pixel into the provided buffer
 * @param buffer       The buffer where the image will be stored
 * @param bufferSize   The buffer size (in bytes)
 * @param scanlineSize The number of bytes from one line of pixels to the next (negative means make an "upside-down" image)
 * @param orientation  The order of characters in the image (vertical slices, horizontal slices)
 */
static void exportFontToImageBuffer(Byte*       buffer,
                                    int         bufferSize,
                                    int         scanlineSize,
                                    Orientation orientation,
                                    const Font  *font)
{
    Bool upsideDown = FALSE;
    const Byte *fontdata = font->data;
    int x,y,col,row,segment,line,charIdx;
    assert( buffer!=NULL );
    assert( bufferSize>=(scanlineSize*FONT_IMG_HEIGHT) );
    /* this function assumes square images to make easier */
    /* support horizontal & vertical orientation          */
    assert( FONT_IMG_WIDTH==FONT_IMG_HEIGHT );
    assert( CHAR_IMG_WIDTH==CHAR_IMG_HEIGHT );
    
    /* handle "upside-down" images */
    if (scanlineSize<0) { scanlineSize=-scanlineSize; upsideDown=TRUE; }
    assert( scanlineSize>=FONT_IMG_WIDTH/8 );
    
    /* write one by one all of 256 characters */
    charIdx=0; for (y=0; y<(FONT_IMG_HEIGHT/CHAR_IMG_HEIGHT); ++y) {
        for (x=0; x<(FONT_IMG_WIDTH/CHAR_IMG_WIDTH); ++x,++charIdx) {
            assert( charIdx<=255 );
            col = (orientation==HORIZONTAL ? x : y);
            row = (orientation==HORIZONTAL ? y : x);
            for (segment=0; segment<CHAR_IMG_HEIGHT; ++segment) {
                line = row*CHAR_IMG_HEIGHT+segment; if (upsideDown) { line = (FONT_IMG_HEIGHT-1)-line; }
                buffer[ line*scanlineSize + col ] = fontdata[ charIdx*CHAR_IMG_HEIGHT + segment];
                
            }
        }
    }
}

/**
 * Writes the font image to the provided file using BMP format
 * @param outputFile      The file where the font image will be written
 * @param outputFilePath  The path to file where the font image will be written (only used for error report)
 * @param imageFormat     The format of the exported image, ex: GIF, BMP, ...
 * @param orientation     The order of characters in the image (vertical slices, horizontal slices)
 * @param font            The font to export
 */
static Bool exportFontToBmpFile(FILE*        outputFile,
                                const utf8*  outputFilePath,
                                ImageFormat  imageFormat,
                                Orientation  orientation,
                                const Font*  font)
{
    int scanlineSize=0,pixelDataSize=0; Byte* pixelData=NULL;
    static const Byte colorTable[] = { 255,255,255,0,  0,0,0,0 };
    
    if (success) { /* 1) generate the font image in a buffer */
        scanlineSize  = getBmpScanlineSize2(FONT_IMG_WIDTH, FONT_IMG_BITSPERPIXEL);
        pixelDataSize = FONT_IMG_HEIGHT * scanlineSize;
        pixelData     = malloc( pixelDataSize );
        exportFontToImageBuffer(pixelData, pixelDataSize, -scanlineSize, orientation, font);
    }
    if (success && imageFormat==GIF) { /* 2a) write font image buffer into a GIF file */
        if (!fwriteGif(FONT_IMG_WIDTH, FONT_IMG_HEIGHT, -scanlineSize, FONT_IMG_BITSPERPIXEL,
                       colorTable, sizeof(colorTable),
                       pixelData, pixelDataSize,
                       outputFile))
        { error(ERR_CANNOT_WRITE_FILE,outputFilePath); }
    }
    if (success && imageFormat==BMP) { /* 2a) write font image buffer into a BMP file */
        if (!fwriteBmp(FONT_IMG_WIDTH, FONT_IMG_HEIGHT, -scanlineSize, FONT_IMG_BITSPERPIXEL,
                       colorTable, sizeof(colorTable),
                       pixelData, pixelDataSize,
                       outputFile))
        { error(ERR_CANNOT_WRITE_FILE,outputFilePath); }
    }
    free(pixelData);
    return success ? TRUE : FALSE;
}
    
/**
 * Exports the provided font to an image file
 *
 * The image is stored in the current working directory and the name
 * of the file is generated concatenating a suffix with the font name.
 * @param font         The font to export
 * @param imageFormat  The format of the exported image, ex: GIF, BMP, ...
 * @param orientation  The order of characters in the image (vertical slices, horizontal slices)
 */
Bool exportFont(const Font *font, ImageFormat imageFormat, Orientation orientation) {
    const utf8 *outputFilePath=NULL, *outputFileName=NULL, *imageExtension;
    FILE *outputFile=NULL;
    assert( font!=NULL );
    assert( orientation==HORIZONTAL || orientation==VERTICAL );

    if (success) {
        imageExtension = getImageExtension(imageFormat,NULL);
        outputFileName = allocConcatenation(FONT_IMG_PREFIX, font->name);
        outputFilePath = allocFilePath(outputFileName, imageExtension, FORCED_EXTENSION);
        if (!outputFileName || !outputFileName) { error(ERR_NOT_ENOUGH_MEMORY,0); }
    }
    if (success) {
        outputFile = fopen(outputFilePath, "wb");
        if (!outputFile) { error(ERR_CANNOT_CREATE_FILE,outputFilePath); }
    }
    if (success) {
        printf("Exporting font %s to file '%s'\n", font->name, outputFilePath);
        exportFontToBmpFile(outputFile, outputFilePath, imageFormat, orientation, font);
    }
    /* clean up and return */
    if (outputFile) { fclose(outputFile); }
    free((void*)outputFilePath);
    free((void*)outputFileName);
    return success ? TRUE : FALSE;
}

