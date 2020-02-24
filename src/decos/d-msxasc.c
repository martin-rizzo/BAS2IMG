/**
 * @file       d-msxasc.c
 * @date       Feb 21, 2020
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
#include "../types.h"

#define LF       0x0A  /* line feed             */
#define CR       0x0D  /* carriage return       */
#define EXTENDED 0x01  /* extended character    */
#define EOF      0x1A  /* end of file character */

/**
 * Returns TRUE if the provided file content is decodable by this decoder
 * @param sour     The buffer with the first bytes of the file content
 * @param sourLen  The buffer length in number of bytes
 */
static Bool isDecodable(const Byte *sour, int sourLen) {
    return TRUE;
}

/**
 * Decodes a minimal portion of the data
 *
 * The destination buffer is guaranteed to have space for at least 32 bytes.
 *
 * @param inout_dest   The destination buffer where to store the decoded data.
 * @param inout_sour   The source buffer with the content to decode
 * @param sourLen      The source buffer length in number of bytes (always greater than zero)
 */
static Bool decode(Byte **inout_dest, const Byte **inout_sour, int sourLen) {
    Byte       *dest = (*inout_dest);
    const Byte *sour = (*inout_sour);
    Bool     newline = FALSE;
    assert( inout_dest!=NULL && (*inout_dest)!=NULL );
    assert( inout_sour!=NULL && (*inout_sour)!=NULL );
    assert( sourLen>0 );
    
    
    if (sourLen>=2) {
        if      (sour[0]==LF) { sour+=(sour[1]==CR ? 2 : 1); newline=TRUE; }
        else if (sour[0]==CR) { sour+=(sour[1]==LF ? 2 : 1); newline=TRUE; }
        else if (sour[0]==EXTENDED) { ++sour; *dest++=(*sour++)-0x40; }
        else                        { *dest++=*sour++; }
    }
    /* when only left 1 character to decode in the source buffer */
    else {
        if (*sour==EOF || *sour==LF || *sour==CR || *sour==EXTENDED ) { ++sour; }
        else { *dest++=*sour++; }
    }
    
    
    (*inout_dest) = dest;
    (*inout_sour) = sour;
    return !newline;
}

const Decoder decoder_msxasc = {
    "msx-asc",
    "Decoder for MSX-BASIC programs stored as ASCII",
    isDecodable,
    decode };

