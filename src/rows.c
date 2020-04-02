/**
 * @file       rows.c
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
#include "globals.h"
#include "rows.h"
#define min(a,b)  ((a)<(b) ? (a) : (b))
#define MAX_COLUMN (LINE_BUF_SIZE-MIN_DECODE_BUF_SIZE)

/**
 * Allocates enough memory to contain the requested number of rows
 * @param numberOfRows   The maximum number of rows to contain
 * @returns              The array of rows with the requested capacity
 */
#define allocRows(numberOfRows) reallocRows(NULL,numberOfRows)

/**
 * Changes the capacity of an array of rows
 * @param rowsToRealloc  The array of rows that was previously allocated with allocRow(..)
 * @param numberOfRows   The new maximum number of rows to contain
 * @returns              The same array of rows but with the capacity updated
 */
static Rows reallocRows(Rows rowsToRealloc, int numberOfRows) {
    SingleRowPtr oldRow;
    Rows rows; int i;
    assert( numberOfRows>1 );
    rows = malloc( (numberOfRows+1) * sizeof(SingleRowPtr) );
    
    oldRow = (rowsToRealloc!=NULL ? rowsToRealloc[0] : NULL);
    for (i=0; i<numberOfRows; ++i) {
        rows[i] = oldRow;
        if (oldRow!=NULL) { oldRow = rowsToRealloc[i+1]; }
    }
    rows[i]=NULL; /* < always finish the array with a NULL pointer */

    /* release all rows that were not copied */
    while (oldRow) { free(oldRow); oldRow = rowsToRealloc[++i];  }
    free(rowsToRealloc);
    return rows;
}

static SingleRowPtr allocSingleRow(const Char256* chars, int numberOfChars, int wrapLength) {
    SingleRowPtr row; Bool isEndOfLine=TRUE;
    assert( chars!=NULL && numberOfChars>=0 );
    
    if (wrapLength>0 && numberOfChars>wrapLength) { numberOfChars=wrapLength; isEndOfLine=FALSE; }
    row = malloc( sizeof(SingleRow) + numberOfChars*sizeof(Char256) );
    row->length      = numberOfChars;
    row->isEndOfLine = isEndOfLine;
    memcpy( row->chars, chars, numberOfChars );
    return row;
}


/*=================================================================================================================*/
#pragma mark - > PUBLIC FUNCTIONS


Rows allocRowsFromBasicBuffer(const Byte *basicBuffer,
                              long       basicBufferSize,
                              int        wrapLength,
                              DecodeFunc decodeFunc)
{
    Rows rows;
    const Byte *sour, *sourEnd;
    Byte *dest;
    
    Char256 lineBuffer[LINE_BUF_SIZE], *lineBufferPtr;
    int column,rowIdx,numberOfChars; Bool newline;
    
    assert( basicBuffer!=NULL );
    assert( basicBufferSize>0 );
    assert( decodeFunc!=NULL );
    
    rows    = allocRows(1024);
    rowIdx  = 0;
    sour    = basicBuffer;
    sourEnd = (basicBuffer + basicBufferSize);
    while (sour<sourEnd) {
        dest    = lineBuffer;
        /* decode a single line */
        newline=FALSE; while (!newline && sour<sourEnd) {
            column = (int)(dest-lineBuffer);
            if ( column<MAX_COLUMN ) { newline = !(*decodeFunc)( &dest, &sour, (int)(sourEnd-sour) ); }
            else                     { newline = TRUE; }
        }
        if (newline || dest>lineBuffer ) {
            /* copy the text line into the array of rows (wrapping the line when necessary) */
            lineBufferPtr = lineBuffer;
            numberOfChars = (int)(dest-lineBufferPtr);
            do {
                rows[rowIdx] = allocSingleRow(lineBufferPtr, numberOfChars, wrapLength);
                lineBufferPtr += rows[rowIdx]->length;
                numberOfChars -= rows[rowIdx]->length;
                ++rowIdx;
            } while (numberOfChars>0);
        }
    }
    return rows;
}


void freeRows(Rows rows) {
    int i; if (!rows) { return; }
    for (i=0; rows[i]; ++i) { free(rows[i]); }
    free(rows);
}



/**
 * Returns the length of the longest row
 */
int getMaxRowLength(const Rows rows) {
    int i, maxLength;
    assert( rows!=NULL );
    
    maxLength=0;
    for (i=0; rows[i]; ++i) {
        if (rows[i]->length > maxLength) { maxLength = rows[i]->length; }
    }
    return maxLength;
}

/**
 * Returns the total number of rows
 */
int getNumberOfRows(const Rows rows) {
    int i;
    assert( rows!=NULL );
    
    for (i=0; rows[i]; ++i) { }
    return i;
}

/**
 * Returns the length of the longest line
 *
 * ATTENTION: a line can span several rows because wrapping can split long lines in multiple rows
 * @param rows  A previously allocated array of rows of text containing the lines to search
 */
int getMaxLineLength_(const Rows rows) {
    int i, length, maxLength=0;
    assert( rows!=NULL );
    
    for (i=0; rows[i]; ++i) {
        length=rows[i]->length;
        while (!rows[i]->isEndOfLine && rows[++i]!=NULL) { length+=rows[i]->length; }
        if (length>maxLength) { maxLength=length; }
    }
    return maxLength;
}

/**
 * Returns the total number of lines
 *
 * ATTENTION: a line can span several rows because wrapping can split long lines in multiple rows
 * @param rows  A previously allocated array of rows of text containing the lines to count
 */
int getNumberOfLines_(const Rows rows) {
    int i,numberOfLines;
    assert( rows!=NULL );
    
    numberOfLines=0;
    for (i=0; rows[i]; ++i) {
        if (rows[i]->isEndOfLine || rows[i+1]==NULL) { ++numberOfLines; }
    }
    return numberOfLines;
}
