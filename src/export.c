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
#include "font.h"
#include "bmp.h"


static void exportFontToImageBuffer(Byte*       buffer,
                                    int         bufferSize,
                                    int         scanlineSize,
                                    Orientation orientation,
                                    const Font  *font)
{
    Bool upsideDown = FALSE;
    const Byte *fontdata = font->data;
    int x,y,col,row,segment,line,charIdx;
    assert( bufferSize>=(FONTIMG_SIZE*FONTIMG_SIZE)/8 );
    
    /* handle "upside-down" images */
    if (scanlineSize<0) { scanlineSize=-scanlineSize; upsideDown=TRUE; }
    /* write one by one all of 256 characters */
    charIdx=0; for (y=0; y<(FONTIMG_SIZE/CHAR_SIZE); ++y) {
        for (x=0; x<(FONTIMG_SIZE/CHAR_SIZE); ++x,++charIdx) {
            assert( charIdx<=255 );
            col = (orientation==HORIZONTAL ? x : y);
            row = (orientation==HORIZONTAL ? y : x);
            for (segment=0; segment<CHAR_SIZE; ++segment) {
                line = row*CHAR_SIZE+segment; if (upsideDown) { line = (FONTIMG_SIZE-1)-line; }
                buffer[ line*scanlineSize + col ] = fontdata[ charIdx*CHAR_SIZE + segment];
                
            }
        }
    }
}

/**
 * Writes the font image to the provided file using BMP format
 * @param outputFile      The file where the font image will be written
 * @param outputFilePath  The path to file where the font image will be written (only used for error report)
 * @param orientation     The order of characters in the image (vertical slices, horizontal slices)
 * @param font            The font to export
 */
static Bool exportFontToBmpFile(FILE         *outputFile,
                                const utf8   *outputFilePath,
                                Orientation  orientation,
                                const Font   *font)
{
    BmpHeader bmp; Byte* pixelData = NULL;
    static const Byte colorTable[] = { 255,255,255,0,  0,0,0,0 };
    
    if (isRunning()) { /* 1) set bmp header */
        if (!setBmpHeader(&bmp, FONTIMG_SIZE, FONTIMG_SIZE, FONTIMG_NUMOFCOLORS)) { err(ERR_INTERNAL_ERROR); }
    }
    if (isRunning()) { /* 2) copy font to an image-buffer and write it into file */
        pixelData = malloc(bmp.pixelDataSize);
        exportFontToImageBuffer(pixelData, bmp.pixelDataSize, -bmp.scanlineSize, orientation, font);
        if (!fwriteBmp(&bmp, colorTable, sizeof(colorTable), pixelData, bmp.pixelDataSize, outputFile))
        { err2(ERR_CANNOT_WRITE_FILE,outputFilePath); }
    }
    free(pixelData);
    return isRunning();
}
    
/**
 * Exports the provided font to an image file
 *
 * The image is stored in the current working directory and the name
 * of the file is generated concatenating a suffix with the font name.
 * @param font         The font to export
 * @param orientation  The order of characters in the image (vertical slices, horizontal slices)
 */
Bool exportFont(const Font *font, Orientation orientation) {
    const utf8 *outputFilePath=NULL, *outputFileName=NULL;  FILE *outputFile=NULL;
    assert( font!=NULL );
    assert( orientation==HORIZONTAL || orientation==VERTICAL );

    if (isRunning()) {
        outputFileName = allocConcatenation(FONTIMG_PREFIX, font->name);
        outputFilePath = allocFilePath(outputFileName, ".bmp", FORCED_EXTENSION);
        if (!outputFileName || !outputFileName) { err(ERR_NOT_ENOUGH_MEMORY); }
    }
    if (isRunning()) {
        outputFile = fopen(outputFilePath, "wb");
        if (!outputFile) { err2(ERR_CANNOT_CREATE_FILE,outputFilePath); }
    }
    if (isRunning()) {
        printf("Exporting font %s to file '%s'", font->name, outputFilePath);
        exportFontToBmpFile(outputFile, outputFilePath, orientation, font);
    }
    /* clean up and return */
    if (outputFile) { fclose(outputFile); }
    free((void*)outputFilePath);
    free((void*)outputFileName);
    return isRunning();
}

