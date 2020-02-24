/**
 * @file       lines.c
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
#include <string.h>
#include "bas2img.h"
#include "lines.h"

/**
 * Allocates enough memory to contain the requested number of lines
 * @param numberOfLines  The maximum number of lines to contain
 * @returns              The array of lines with the requested capacity
 */
#define allocLines(numberOfLines) reallocLines(NULL,numberOfLines)

/**
 * Changes the number of lines that can be contained by 'linesToRealloc'
 * @param linesToRealloc  The array of lines that was previously allocated with allocLines(..)
 * @param numberOfLines   The new maximum number of lines to contain
 * @returns               The same array of lines but with the capacity updated
 */
static Lines reallocLines(Lines linesToRealloc, int numberOfLines) {
    SingleLinePtr oldLine;
    Lines lines; int i;
    assert( numberOfLines>1 );
    lines = malloc( (numberOfLines+1) * sizeof(SingleLinePtr) );
    
    oldLine = (linesToRealloc!=NULL ? linesToRealloc[0] : NULL);
    for (i=0; i<numberOfLines; ++i) {
        lines[i] = oldLine;
        if (oldLine!=NULL) { oldLine = linesToRealloc[i+1]; }
    }
    lines[i]=NULL; /* < always finish the list with a NULL pointer */

    /* release all lines that were not copied */
    while (oldLine) { free(oldLine); oldLine = linesToRealloc[++i];  }
    free(linesToRealloc);
    return lines;
}

static SingleLinePtr allocSingleLine(const void* bytes, int numberOfBytes) {
    SingleLinePtr line = malloc( sizeof(SingleLine) + numberOfBytes );
    line->length = numberOfBytes;
    memcpy( line->bytes, bytes, numberOfBytes );
    return line;
}


/*=================================================================================================================*/
#pragma mark - > PUBLIC FUNCTIONS


Lines allocLinesFromBasicBuffer(const Byte *basicBuffer,
                                long       basicBufferSize,
                                DecodeFunc decodeFunc)
{
    Lines lines;
    const Byte *sour, *sourEnd;
    Byte *dest, *destEnd;
    Bool newline;
    Byte lineBuffer[MAX_LINE_LENGTH];
    int row;
    
    assert( basicBuffer!=NULL );
    assert( basicBufferSize>0 );
    assert( decodeFunc!=NULL );
    
    lines   = allocLines(1024);
    row     = 0;
    sour    = basicBuffer;
    sourEnd = (basicBuffer + basicBufferSize);
    while (sour<sourEnd) {
        dest    = lineBuffer;
        destEnd = lineBuffer + MAX_LINE_LENGTH;
        /* decode a single line */
        newline=FALSE; while (!newline && sour<sourEnd) {
            if ( (destEnd-dest)>=32 ) { newline = !(*decodeFunc)( &dest, &sour, (int)(sourEnd-sour) ); }
            else                      { newline = TRUE; }
        }
        if (newline || dest>lineBuffer ) {
            lines[row++] = allocSingleLine(lineBuffer, (int)(dest-lineBuffer));
        }
    }
    
    
    
    return lines;
}


void freeLines(Lines lines) {
    int i;
    if (lines) {
        for (i=0; lines[i]; ++i) { free(lines[i]); }
        free(lines);
    }
}



int getMaxLineLength(const Lines lines) {
    int i, maxLength=0;
    assert( lines!=NULL );
    
    for (i=0; lines[i]; ++i) {
        if ( lines[i]->length > maxLength ) { maxLength = lines[i]->length; }
    }
    return maxLength;
}

int getNumberOfLines(const Lines lines) {
    int i;
    assert( lines!=NULL );
    
    i=0; while (lines[i]) { ++i; }
    return i;
}
