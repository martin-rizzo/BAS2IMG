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
#include "bas2img.h"
#include "types.h"
#include "helpers.h"
#include "error.h"
#include "database.h"
#include "lines.h"


Bool generateImageFromLines(void) {
    return FALSE;
}


static Bool generateImageFromBasicBuffer(FILE           *imageFile,
                                         ImageFormat    imageFormat,
                                         Orientation    orientation,
                                         const Byte     *basicBuffer,
                                         long           basicBufferSize,
                                         const Computer *computer,
                                         const Config   *config
                                         ) {
    
    int i;
    Lines lines = NULL;
    
    lines = allocLinesFromBasicBuffer( basicBuffer, basicBufferSize, computer->decoder->decode );
    for ( i=0 ; lines[i] ; ++i ) {
        printf("line %d len=%d\n", i,lines[i]->length);
    }
    
    
    /* clean up and return */
    if (lines) { freeLines(lines); }
    return success ? TRUE : FALSE;
}



/**
 * Generates an image displaying the source code of the provided BASIC program
 *
 * @param imageFilePath  The path to the output image (NULL = use the BASIC program name)
 * @param imageFormat    The format of the output image (only BMP format is supported)
 * @param basicFilePath  The path to the BASIC program used as input
 * @param computer       The computer for which to process the BASIC program
 * @param config         The configuration used to generate the image
 */
Bool generateImageFromBASIC(const utf8     *imageFilePath,
                            ImageFormat    imageFormat,
                            Orientation    orientation,
                            const utf8     *basicFilePath,
                            const Computer *computer,
                            const Config   *config)
{
    const utf8 * imageExtension;
    FILE *imageFile=NULL, *basicFile=NULL;
    Byte *basicBuffer=NULL; long basicBufferSize=0;
    
    assert( orientation==HORIZONTAL || orientation==VERTICAL );
    assert( basicFilePath!=NULL && computer!=NULL && config!=NULL );
    if (imageFormat==GIF) { return error(ERR_GIF_NOT_SUPPORTED,0); }
    
    /* add extensions (when appropiate) */
    basicFilePath = allocFilePath(basicFilePath, ".bas", OPTIONAL_EXTENSION);
    
    /* make the path to the image file */
    imageExtension = getImageExtension(imageFormat);
    if (imageFilePath) { imageFilePath = allocFilePath(imageFilePath,imageExtension,OPTIONAL_EXTENSION); }
    else               { imageFilePath = allocFilePath(basicFilePath,imageExtension,FORCED_EXTENSION  ); }
    
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
        generateImageFromBasicBuffer( imageFile, imageFormat, orientation, basicBuffer, basicBufferSize, computer, config);
    }
    /*-------------------------------------------------------------------*/
    
    /* clean up and return */
    if (basicBuffer  ) { free((void*)basicBuffer); }
    if (basicFile    ) { fclose(basicFile); }
    if (imageFile    ) { fclose(imageFile); }
    if (basicFilePath) { free((void*)basicFilePath); }
    if (imageFilePath) { free((void*)imageFilePath); }
    return success ? TRUE : FALSE;
}
