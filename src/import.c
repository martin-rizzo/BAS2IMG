/**
 * @file       import.c
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
#include <stdio.h>
#include <string.h>
#include "bas2img.h"
#include "helpers.h"
#include "error.h"
#include "bmp.h"


/**
 * Writes C code representing an array containing all the font image stored in the buffer
 * @param outputFile    The file where the generated C code will be written
 * @param arrayName     The name of the C array
 * @param imageBuffer   The buffer containing the image of 256 characters of 8x8 pixels (1 bit per pixel)
 * @param scanlineSize  The number of bytes from one line of pixels to the next (negative means "upside-down" image)
 * @param orientation   The order of characters in the image (vertical slices, horizontal slices)
 */
static Bool writeCArrayFromImageBuffer(FILE       *outputFile,
                                       const utf8 *arrayName,
                                       const Byte *imageBuffer,
                                       int         scanlineSize,
                                       Orientation orientation) {
    Bool upsideDown = FALSE;
    int x,y,col,row,line,segment,charIdx; const utf8 *separator;
    const int lastCharIdx=255;
    assert( outputFile!=NULL );
    assert( arrayName!=NULL && strlen(arrayName)>0 );
    assert( imageBuffer!=NULL && scanlineSize!=0 );
    assert( orientation==HORIZONTAL || orientation==VERTICAL );
    fprintf(outputFile, "const Byte font_%s[2048] = {", arrayName);

    /* fix "upside-down" images */
    if (scanlineSize<0) { scanlineSize=-scanlineSize; upsideDown=TRUE; }
    /* write one by one all of 256 characters */
    charIdx=0; for (y=0; y<16; ++y) {
        for (x=0; x<16; ++x,++charIdx) {
            assert( charIdx<=lastCharIdx );
            if ((charIdx%2)==0) { fprintf(outputFile,"\n    "); }
            else                { fprintf(outputFile," "     ); }
            col = (orientation==HORIZONTAL ? x : y);
            row = (orientation==HORIZONTAL ? y : x);
            for (segment=0; segment<8; ++segment) {
                assert( scanlineSize>0 );
                line      = row*8+segment; if (upsideDown) { line = 127-line; }
                separator = (charIdx==lastCharIdx && segment==7) ? "" : ",";
                fprintf(outputFile, "0x%02x%s", imageBuffer[ (line*scanlineSize)+col ], separator);
            }
        }
    }
    fprintf(outputFile, "\n};\n");
    return TRUE;
}

/**
 * Writes the C code representing an array containing the font image stored in a BMP file
 *
 * The font image must be a 2-colors BMP file, it must be of size 128x128 pixels
 * and it must contain all the characters arranged in 16 columns and 16 rows
 * @param outputFile     The file where the generated C code will be written
 * @param arrayName      The name of the C array
 * @param imageFile      The file with the image encoded in BMP format
 * @param imageFileSize  The size in bytes of the BMP file
 * @param imageFilePath  The path to the BMP file (used for error report)
 * @param orientation    The order of characters in the image (vertical slices, horizontal slices)
 */
static Bool writeCArrayFromBitmapFile(FILE       *outputFile,
                                      const utf8 *arrayName,
                                      FILE       *imageFile,
                                      long        imageFileSize,
                                      const utf8 *imageFilePath,
                                      Orientation orientation)
{
    static const unsigned requiredWidth  = 128;
    static const unsigned requiredHeight = 128;
    int pixelDataSize, requiredDataSize;
    Byte *imageBuffer=NULL; BmpHeader bmp;
    
    assert( outputFile!=NULL );
    assert( arrayName!=NULL && strlen(arrayName)>0 );
    assert( imageFile!=NULL && imageFileSize>0 && imageFilePath!=NULL );
    assert( orientation==HORIZONTAL || orientation==VERTICAL );
    assert( isRunning() );
    
    if (isRunning()) { /* 1) allocate space to load the complete file to memory */
        imageBuffer = malloc(imageFileSize);
        if (!imageBuffer) { err(ERR_NOT_ENOUGH_MEMORY); }
    }
    if (isRunning()) { /* 2) load the file */
        if ( imageFileSize!=fread(imageBuffer,1,imageFileSize,imageFile) ) {
            err2(ERR_CANNOT_READ_FILE,imageFilePath);
        }
    }
    if (isRunning()) { /* 3) extract bitmap header */
        if (!extractBmpHeader(&bmp, imageBuffer, imageFileSize)) { err(ERR_FILE_IS_NOT_BMP); }
    }
    if (isRunning()) { /* 4) verify correct bitmap format */
        pixelDataSize    = (unsigned)imageFileSize-bmp.pixelDataOffset;
        requiredDataSize = (requiredWidth/8)*requiredHeight;
        if      (bmp.fileType     !=0x4D42         ) { err(ERR_FILE_IS_NOT_BMP       ); }
        else if (bmp.fileSize     !=imageFileSize  ) { err(ERR_BMP_INVALID_FORMAT    ); }
        else if (bmp.imageWidth   !=requiredWidth  ) { err(ERR_BMP_MUST_BE_128PX     ); }
        else if (bmp.imageHeight  !=requiredHeight ) { err(ERR_BMP_MUST_BE_128PX     ); }
        else if (bmp.planes       !=1              ) { err(ERR_BMP_INVALID_FORMAT    ); }
        else if (bmp.bitsPerPixel !=1              ) { err(ERR_BMP_MUST_BE_1BIT      ); }
        else if (bmp.compression  !=0              ) { err(ERR_BMP_UNSUPPORTED_FORMAT); }
        else if (pixelDataSize    <requiredDataSize) { err(ERR_BMP_INVALID_FORMAT    ); }
    }
    if (isRunning()) { /* 5) write the C array to the output file */
        writeCArrayFromImageBuffer(outputFile, arrayName,
                                   &imageBuffer[bmp.pixelDataOffset], -bmp.scanlineSize, orientation);
    }
    free(imageBuffer);
    return isRunning();
}

/**
 * Creates a C code representing an array containing the font image
 *
 * @param outputFilePath The path to the file where the generated C code will be written (NULL = use image name)
 * @param imageFilePath  The path to the input image
 * @param imageFormat    The format of the input image (only BMP format is supported)
 * @param orientation    The order of characters in the image (vertical slices, horizontal slices)
 */
Bool writeCArrayFromImage(const utf8  *outputFilePath,
                          const utf8  *imageFilePath,
                          ImageFormat  imageFormat,
                          Orientation  orientation)
{
    FILE *outputFile=NULL, *imageFile=NULL; long imageFileSize=0;
    assert( imageFilePath!=NULL );
    assert( imageFormat==BMP || imageFormat==GIF );
    assert( orientation==HORIZONTAL || orientation==VERTICAL );
    assert( isRunning() );
    if (imageFormat==GIF) { return err(ERR_GIF_NOT_SUPPORTED); }
    
    /* add extensions (when appropiate) */
    imageFilePath = allocFilePath(imageFilePath, ".bmp", OPTIONAL_EXTENSION);
    if (outputFilePath) { outputFilePath = allocFilePath(outputFilePath,".c",OPTIONAL_EXTENSION); }
    else                { outputFilePath = allocFilePath(imageFilePath ,".c",FORCED_EXTENSION  ); }
    
    if (isRunning()) { /* 1) open image file for reading */
        imageFile = fopen(imageFilePath,"rb");
        if (!imageFile) { err2(ERR_FILE_NOT_FOUND,imageFilePath); }
    }
    if (isRunning()) { /* 2) get size of the image file and verify it is valid */
        imageFileSize = getFileSize(imageFile);
        if (imageFileSize<MIN_FILE_SIZE) { err2(ERR_FILE_TOO_SMALL,imageFilePath); }
        if (imageFileSize>MAX_FILE_SIZE) { err2(ERR_FILE_TOO_LARGE,imageFilePath); }
    }
    if (isRunning()) { /* 3) open header file for writing */
        outputFile = fopen(outputFilePath,"w");
        if (!outputFile) { err2(ERR_CANNOT_CREATE_FILE,outputFilePath); }
    }
    if (isRunning()) { /* 4) proceed! */
        printf("Creating font source code in '%s'\n", outputFilePath);
        writeCArrayFromBitmapFile(outputFile,"msx",imageFile,imageFileSize,imageFilePath,orientation);
    }
    /* clean up and return */
    if (imageFile     ) { fclose(imageFile ); imageFile =NULL; }
    if (outputFile    ) { fclose(outputFile); outputFile=NULL; }
    if (imageFilePath ) { free((void*)imageFilePath ); imageFilePath =NULL; }
    if (outputFilePath) { free((void*)outputFilePath); outputFilePath=NULL; }
    return isRunning();
}

