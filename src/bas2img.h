/**
 * @file       bas2img.h
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
#ifndef bas2img_h
#define bas2img_h


#define MIN_FILE_SIZE       (0)           /* < minimum size for loadable files (in bytes)       */
#define MAX_FILE_SIZE       (1024L*1024L) /* < maximum size for loadable files (in bytes)       */
#define DIR_SEPARATOR1      '\\'          /* < char used to separate directories in a path      */
#define DIR_SEPARATOR2      '/'           /* < char used to separate directories in a path      */
#define EXT_SEPARATOR       '.'           /* < char used as file extension separator            */
#define CHAR_SIZE           8             /* < the width & height of each character (in pixels) */
#define FONTIMG_SIZE        128           /* < the width & height of font images (in pixels)    */
#define FONTIMG_NUMOFCOLORS 2             /* < the number of colors of font images              */
#define FONTIMG_PREFIX      "font__"      /* < prefix used when exporting fonts                 */
#define isOption(param,opname1,opname2)   (strcmp(param,opname1)==0 || strcmp(param,opname2)==0)
typedef unsigned char Byte;               /* < Byte (size=8bits)                                */
typedef char utf8;                        /* < unicode variable width character encoding        */
typedef int Bool; enum { FALSE=0, TRUE }; /* < Boolean                                          */

typedef enum ExtensionMethod { OPTIONAL_EXTENSION, FORCED_EXTENSION } ExtensionMethod;
typedef enum ImageFormat     { BMP, GIF             } ImageFormat;
typedef enum Orientation     { HORIZONTAL, VERTICAL } Orientation;



#endif /* bas2img_h */
