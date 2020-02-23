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


/**
 * Generates an image displaying the source code of the provided BASIC program
 *
 * @param imageFilePath  The path to the output image (NULL = use the BASIC program name)
 * @param imageFormat    The format of the output image (only BMP format is supported)
 * @param basFilePath    The path to the BASIC program used as input
 * @param computer       The computer for which to process the BASIC program
 * @param config         The configuration used to generate the image
 */
Bool generateImageFromBASIC(const utf8     *imageFilePath,
                            ImageFormat    imageFormat,
                            Orientation    orientation,
                            const utf8     *basFilePath,
                            const Computer *computer,
                            const Config   *config)
{
    const utf8 * const imageExtension = getImageExtension(imageFormat);
    FILE *imageFile=NULL, *basFile=NULL; long basFileSize=0;
    
    assert( orientation==HORIZONTAL || orientation==VERTICAL );
    assert( basFilePath!=NULL && computer!=NULL && config!=NULL );
    if (imageFormat==GIF) { return error(ERR_GIF_NOT_SUPPORTED,0); }
    
    /* add extensions (when appropiate) */
    basFilePath = allocFilePath(basFilePath, ".bas", OPTIONAL_EXTENSION);
    /* make the path to the image file */
    if (imageFilePath) { imageFilePath = allocFilePath(imageFilePath,imageExtension,OPTIONAL_EXTENSION); }
    else               { imageFilePath = allocFilePath(basFilePath  ,imageExtension,FORCED_EXTENSION  ); }
    
    if (success) {
        basFile = fopen(basFilePath,"rb");
        if (!basFile) { error(ERR_FILE_NOT_FOUND,basFilePath); }
    }
    if (success) {
        basFileSize = getFileSize(basFile);
        if (basFileSize<MIN_FILE_SIZE) { error(ERR_FILE_TOO_SMALL,basFilePath); }
        if (basFileSize>MAX_FILE_SIZE) { error(ERR_FILE_TOO_LARGE,basFilePath); }
    }
    if (success) {
        imageFile = fopen(imageFilePath,"wb");
        if (!imageFile) { error(ERR_CANNOT_CREATE_FILE,imageFilePath); }
    }
    if (success) {
        printf("Generating the image '%s' containing the source code of %s\n", imageFilePath, basFilePath);
        /* generate( imageFile, imageFormat, orientation, basFile, basFileSize, computer, config) */
        
    }
    /* clean up and return */
    if (basFile      ) { fclose(basFile  ); }
    if (imageFile    ) { fclose(imageFile); }
    if (basFilePath  ) { free((void*)basFilePath  ); }
    if (imageFilePath) { free((void*)imageFilePath); }
    return success ? TRUE : FALSE;
}
