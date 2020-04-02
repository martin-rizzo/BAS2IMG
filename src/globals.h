/**
 * @file       globals.h
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
#ifndef bas2img_globals_h
#define bas2img_globals_h

/*=================================================================================================================*/
#pragma mark - > CONSTANTS

#define MIN_FILE_SIZE         (0)           /* < minimum size for loadable files (in bytes)       */
#define MAX_FILE_SIZE         (1024L*1024L) /* < maximum size for loadable files (in bytes)       */
#define LINE_BUF_SIZE         512           /* < length of buffer used to decode lines of BASIC   */
#define CHAR_IMG_WIDTH        8             /* < width  of each font character (in pixels)        */
#define CHAR_IMG_HEIGHT       8             /* < height of each font character (in pixels)        */
#define FONT_IMG_WIDTH        128           /* < font-image width (in pixels)                     */
#define FONT_IMG_HEIGHT       128           /* < font-image height (in pixels)                    */
#define FONT_IMG_BITSPERPIXEL 1             /* < font-image is 1 bit per pixel (black & white)    */
#define FONT_IMG_PREFIX       "font__"      /* < font-image file prefix used when exporting fonts */
#define MIN_DECODE_BUF_SIZE   32            /* < The smaller buffer size guaranteed when decoding BASIC lines */


/*=================================================================================================================*/
#pragma mark - > GENERIC SIMPLE TYPES

typedef          char utf8;               /* < unicode variable width character encoding  */
typedef unsigned char Byte;               /* < a 8-bits unsigned value (0,255)            */
typedef int Bool; enum { FALSE=0, TRUE }; /* < a boolean value (TRUE/FALSE)               */


/*=================================================================================================================*/
#pragma mark - > BAS2IMG TYPES

typedef unsigned char Char256;            /* < one of 256 characters defined in the home computer character-set */
typedef enum ImageFormat { BMP, GIF             } ImageFormat;
typedef enum Orientation { HORIZONTAL, VERTICAL } Orientation;

/**
 * Prototype of function used to verify if a stream of bytes can be decoded to BASIC lines
 * @param sour     The source buffer containing the encoded BASIC program
 * @param sourLen  The source buffer length in number of bytes
 */
typedef Bool (*IsDecodableFunc)(const Byte *sour, int sourLen);

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
    const int     charWidth;
    const int     charHeight;
} Computer;

typedef struct Config {
    int  charWidth;     /* < character width in pixels  (default 0) */
    int  charHeight;    /* < character height in pixels (default 0) */
    int  charScale;     /* < the magnification scale (default 0)    */
    int  margin;        /* < margin around the box  */
    int  padding;       /* < padding within the box */
    int  lineWidth;     /* < maximum number of characters per line (0 = use the longest line length) */
    Bool lineWrapping;  /* < TRUE = wraps lines that exceed the line width */
    ImageFormat    imageFormat;  /* < image file format (BMP, GIF, ...) */
    Orientation    orientation;  /* < image orientation (vertical or horizontal) */
    const Computer *computer;    /* < computer description */

} Config;


#endif /* bas2img_globals_h */

