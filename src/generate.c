/**
 * @file       generate.c
 * @date       Feb 22, 2020
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
#include "globals.h"
#include "helpers.h"
#include "error.h"
#include "database.h"
#include "rows.h"
#include "image.h"

#define NumberOfColors 256

static Bool generateImageFromRows(FILE           *outputFile,
                                  const Rows     rows,
                                  const Config   *config
                                  ) {
    int width, height, charWidth, charHeight;
    int x,y,i, length;
    const Char256 *sour;
    const Computer* computer;
    Image *image;
    const Rgb black = { 0,0,0 };
    const Rgb blue  = { 64,64,255 };
    const Rgb white = { 255,255,255 };
    assert( outputFile!=NULL );
    assert( rows!=NULL );
    assert( config!=NULL && config->computer!=NULL );

    /*
    for ( i=0 ; lines[i] ; ++i ) {
        printf("line %d len=%d\n", i,lines[i]->length);
    }
    */
    
    computer   = config->computer;
    charWidth  = firstPositiveValue(config->charWidth,  computer->charWidth,  8);
    charHeight = firstPositiveValue(config->charHeight, computer->charHeight, 8);
    width      = getMaxRowLength(rows) * charWidth;
    height     = getNumberOfRows(rows) * charHeight;
    image      = allocImage(width,height);
    
    setPaletteGradient(image, 0,blue,   7,white);
    setPaletteGradient(image, 8,white, 15,black);

    /* Draw palette for testing
     x=64; y=64;
     for (i=0; i<=15; ++i,x+=10) { setColor(image,i); fillRectangle(image,x,y,x+10,y+20); }
     */
    
    setColor(image,7);
    setFont(image,computer->font);
    y=0;
    for (i=0; rows[i]; ++i) {
        x      = 0;
        sour   = rows[i]->chars;
        length = rows[i]->length;
        while (length-->0) {
            drawChar(image,x,y,charWidth,charHeight,*sour++);
            x+=charWidth; }
        y+=charHeight;
    }
    
    switch (config->imageFormat) {
        default:
        case GIF: fwriteGifImage(image,outputFile); break;
        case BMP: fwriteBmpImage(image,outputFile); break;
    }
    freeImage(image);
    return TRUE;
}

/**
 * Generates an image displaying the source code contained in the provided buffer
 * @param outputFile       The output file where the image will be stored
 * @param basicBuffer      A buffer containing the BASIC program
 * @param basicBufferSize  The length of `basicBuffer` in number of bytes
 * @param config           The configuration used to generate the image
 */
static Bool generateImageFromBasicBuffer(FILE         *outputFile,
                                         const Byte   *basicBuffer,
                                         long         basicBufferSize,
                                         const Config *config
                                         ) {
    
    Rows rows; int wrapLength;
    const Computer* computer;
    assert( outputFile!=NULL );
    assert( basicBuffer!=NULL && basicBufferSize>0 );
    assert( config!=NULL && config->computer!=NULL );
    
    computer   = config->computer;
    wrapLength = config->lineWrapping ? config->lineWidth : 0;
    assert( computer->decoder && computer->decoder->decode );
    rows = allocRowsFromBasicBuffer( basicBuffer, basicBufferSize, wrapLength, computer->decoder->decode );
    if (rows) {
        generateImageFromRows(outputFile,rows,config);
        freeRows(rows);
    }
    return success ? TRUE : FALSE;
}

/**
 * Generates an image displaying the source code of the provided BASIC program
 *
 * @param imageFilePath  The path to the output image (NULL = use the BASIC program name)
 * @param basicFilePath  The path to the BASIC program used as input
 * @param config         The configuration used to generate the image
 */
Bool generateImageFromBASIC(const utf8     *imageFilePath,
                            const utf8     *basicFilePath,
                            const Config   *config)
{
    const utf8  *imageExtension, *basicFileName;
    FILE *imageFile=NULL, *basicFile=NULL;
    Byte *basicBuffer=NULL; long basicBufferSize=0;
    
    assert( basicFilePath!=NULL && config!=NULL );
    
    /* add extensions (when appropiate) */
    basicFilePath = allocFilePath(basicFilePath, ".bas", OPTIONAL_EXTENSION);
    basicFileName = allocFileNameWithExtension(basicFilePath);
    
    /* make the path to the image file */
    imageExtension = getImageExtension(config->imageFormat,basicFilePath);
    if (imageFilePath) { imageFilePath = allocFilePath(imageFilePath,imageExtension,OPTIONAL_EXTENSION); }
    else               { imageFilePath = allocFilePath(basicFileName,imageExtension,FORCED_EXTENSION  ); }
    
    /*-------------------------------------------------------------------*/

    if (success) { /* 1) open BASIC file for reading */
        basicFile = fopen(basicFilePath,"rb");
        if (!basicFile) { error(ERR_FILE_NOT_FOUND,basicFilePath); }
    }
    if (success) { /* 2) get size of the BASIC file and verify it is valid */
        basicBufferSize = getFileSize(basicFile);
        if (basicBufferSize<MIN_FILE_SIZE) { error(ERR_FILE_TOO_SMALL,basicFilePath); }
        if (basicBufferSize>MAX_FILE_SIZE) { error(ERR_FILE_TOO_LARGE,basicFilePath); }
    }
    if (success) { /* 3) allocate space to load the complete BASIC file to memory */
        basicBuffer = malloc(basicBufferSize);
        if (!basicBuffer) { error(ERR_NOT_ENOUGH_MEMORY,0); }
    }
    if (success) { /* 4) load the BASIC file */
        if ( basicBufferSize!=fread(basicBuffer,1,basicBufferSize,basicFile) ) {
            error(ERR_CANNOT_READ_FILE,basicFilePath);
        }
    }
    if (success) { /* 5) open image file for writting */
        imageFile = fopen(imageFilePath,"wb");
        if (!imageFile) { error(ERR_CANNOT_CREATE_FILE,imageFilePath); }
    }
    if (success) { /* 6) proceed! */
        printf("Generating the image '%s' containing the source code of %s\n", imageFilePath, basicFilePath);
        generateImageFromBasicBuffer(imageFile, basicBuffer, basicBufferSize, config);
    }
    /*-------------------------------------------------------------------*/
    
    /* clean up and return */
    if (basicBuffer  ) { free((void*)basicBuffer); }
    if (basicFile    ) { fclose(basicFile); }
    if (imageFile    ) { fclose(imageFile); }
    if (basicFileName) { free((void*)basicFileName); }
    if (basicFilePath) { free((void*)basicFilePath); }
    if (imageFilePath) { free((void*)imageFilePath); }
    return success ? TRUE : FALSE;
}
