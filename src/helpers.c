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
 * Duplicates the provided memory data allocating a new memory block
 * @param sour           Pointer to the data to be duplicated
 * @param numberOfBytes  The number of bytes to duplicate
 * @returns              A pointer to the newly allocated data (or NULL if an error occurred)
 */
void * memdup(const void * sour, size_t numberOfBytes) {
    void * dest;
    assert( sour!=NULL && numberOfBytes>0 );
    dest = malloc( numberOfBytes );
    if (dest) { memcpy(dest,sour,numberOfBytes); }
    return dest;
}

/**
 * Returns the file size in bytes
 */
long getFileSize(FILE *file) {
    long fileSize; assert( file!=NULL );
    fseek(file,0,SEEK_END); fileSize=ftell(file); rewind(file);
    return fileSize;
}

/**
 * Returns the file extension corresponding to the image format
 */
const utf8 * getImageExtension(ImageFormat imageFormat) {
    switch(imageFormat) {
        case BMP: return ".bmp";
        case GIF: return ".gif";
        default:  return ".nil";
    }
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


const utf8 * allocFileNameWithoutExtension(const utf8 *originalFilePath) {
    size_t length;
    const utf8 *begin, *end, *ptr; utf8 *fileName;
    assert( originalFilePath!=NULL );
    
    begin = originalFilePath;
    end   = NULL;
    for (ptr=originalFilePath; *ptr!='\0'; ++ptr) {
        if (*ptr==DIR_SEPARATOR1 || *ptr==DIR_SEPARATOR2) { begin=ptr+1; }
        else if (*ptr==EXT_SEPARATOR)                     { end=ptr;     }
    }
    if (end==NULL) { end=ptr; }
    
    length   = (end-begin);
    fileName = memdup(begin,length+1);
    fileName[length] = '\0';
    return fileName;
}

const utf8 * allocStringWithoutPrefix(const utf8 *originalString, const utf8 *prefixToRemove) {
    const utf8 *begin, *ptr, *prefix;
    assert( originalString!=NULL && prefixToRemove!=NULL );
    
    ptr = begin = originalString;
    prefix      = prefixToRemove;
    while (*ptr==*prefix) { ++ptr; ++prefix; }
    if (*prefix=='\0') { begin=ptr; }
    return strdup(begin);
}
