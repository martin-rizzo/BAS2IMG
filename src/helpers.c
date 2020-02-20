/**
 * @file       helpers.c
 * @date       Feb 17, 2020
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
#include "helpers.h"
#define DIR_SEPARATOR1  '\\'  /* < char used to separate directories in a path      */
#define DIR_SEPARATOR2  '/'   /* < char used to separate directories in a path      */
#define EXT_SEPARATOR   '.'   /* < char used as file extension separator            */

/**
 * Replaces the sign '$' contained in message with the text provided in 'str'
 * @param buffer   The buffer where the composed string will be written
 * @param message  The message to copy to the buffer
 * @param str      The text to use as a replacement for the sign '$' (optional, can be NULL)
 */
const utf8 * strblend(utf8 *buffer, const utf8 *message, const utf8 *str) {
    utf8 *dest; const utf8 *ptr;
    assert( buffer!=NULL && message!=NULL );
    dest=buffer; ptr=message; while (*ptr!='$' && *ptr!='\0') { *dest++=*ptr++; }
    if (*ptr=='$' && str) { ++ptr; while (*str!='\0') { *dest++=*str++; } }
    while (*ptr!='\0') { *dest++=*ptr++; }; *dest='\0';
    return buffer;
}

/**
 * Returns the file size in bytes
 */
long getFileSize(FILE *file) {
    long fileSize; assert( file!=NULL );
    fseek(file,0,SEEK_END); fileSize=ftell(file); rewind(file);
    return fileSize;
}


const utf8 * allocConcatenation(const utf8 *firstString, const utf8 *secondString) {
    utf8 *string;
    assert( firstString!=NULL && secondString!=NULL );
    string = malloc( strlen(firstString)+strlen(secondString)+1 );
    strcpy(string,firstString); strcat(string,secondString);
    return string;
}

/**
 *
 */
const utf8* allocFilePath(const utf8* originalFilePath, const utf8* newExtension, ExtensionMethod method) {
    const utf8 *fileName, *sour; utf8 *dest, *oldExtension=NULL;
    assert( originalFilePath!=NULL && newExtension!=NULL );
    assert( method==OPTIONAL_EXTENSION || method==FORCED_EXTENSION );
    
    fileName = dest = malloc( strlen(originalFilePath)+strlen(newExtension)+1 );
    for (sour=originalFilePath; *sour!='\0'; *dest++=*sour++) {
        if (*sour==EXT_SEPARATOR) { oldExtension=dest; }
    }
    if (method==FORCED_EXTENSION || (method==OPTIONAL_EXTENSION && !oldExtension)) {
        if (oldExtension) { dest=oldExtension; }
        sour=newExtension; while (*sour!='\0') { *dest++=*sour++; }
    }
    *dest='\0';
    return fileName;
}

