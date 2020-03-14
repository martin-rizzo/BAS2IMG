/**
 * @file       types.h
 * @date       Feb 15, 2020
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
#ifndef bas2img_types_h
#define bas2img_types_h


/* generic simple types */

typedef          char utf8;               /* < unicode variable width character encoding                        */
typedef unsigned char Byte;               /* < a 8-bits unsigned value (0,255)                                  */
typedef unsigned char Char256;            /* < one of 256 characters defined in the home computer character-set */
typedef int Bool; enum { FALSE=0, TRUE }; /* < a boolean value (TRUE/FALSE)                                     */

/* bas2img simple types */

typedef enum ImageFormat { BMP, GIF             } ImageFormat;
typedef enum Orientation { HORIZONTAL, VERTICAL } Orientation;


/* bas2img more complex types */

/**
 * Prototype of function used to verify if a stream of bytes can be decoded to BASIC lines
 * @param sour     The source buffer containing the encoded BASIC program
 * @param sourLen  The source buffer length in number of bytes
 */
typedef Bool (*IsDecodableFunc)(const Byte *sour, int sourLen);

/**
 * The smaller buffer size guaranteed when decoding BASIC lines
 */
#define MIN_DECODE_BUF_SIZE 32

/**
 * Prototype of function used to decode basic lines
 *
 * The destination buffer is guaranteed to have space for at least 32 bytes (MIN_DECODE_BUF_SIZE)
 *
 * @param inout_dest   The destination buffer where to store the decoded data.
 * @param inout_sour   The source buffer with the content to decode
 * @param sourLen      The source buffer length in number of bytes (always greater than zero)
 */
typedef Bool (*DecodeFunc)(Byte **inout_dest, const Byte **inout_sour, int sourLen);




typedef struct Rgb {
    Byte r,g,b;
} Rgb;

typedef struct Font {
    const char *name;
    const char *description;
    unsigned char data[2048];
} Font;

typedef struct Decoder {
    const char      *name;
    const char      *description;
    IsDecodableFunc isDecodable;
    DecodeFunc      decode;
    
} Decoder;

typedef struct Computer {
    const char    *name;
    const char    *description;
    const Decoder *decoder;
    const Font    *font;
} Computer;

typedef struct Config {
    int  margin;
    int  padding;
    int  charWidth;     /* < character width in pixels  (default 8)        */
    int  charHeight;    /* < character height in pixels (default 8)        */
    int  lineWidth;     /* < maximum number of characters per line (0 = use the longest line length) */
    Bool lineWrapping;  /* < TRUE = wraps lines that exceed the line width */
    
} Config;


#endif /* bas2img_types_h */

