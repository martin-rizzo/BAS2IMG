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


#define MIN_FILE_SIZE        (0)           /* < minimum size for loadable files (in bytes)  */
#define MAX_FILE_SIZE        (1024L*1024L) /* < maximum size for loadable files (in bytes)  */
#define MAX_LINE_LENGTH      512           /* < maximum number of chars in a line of code   */
#define CHAR_IMG_WIDTH       8             /* < width  of each font character (in pixels)   */
#define CHAR_IMG_HEIGHT      8             /* < height of each font character (in pixels)   */
#define FONT_IMG_WIDTH       128           /* < the font image width (in pixels)            */
#define FONT_IMG_HEIGHT      128           /* < the font image height (in pixels)           */
#define FONT_IMG_NUMOFCOLORS 2             /* < the number of colors of font images         */
#define FONT_IMG_PREFIX      "font__"      /* < prefix used when exporting fonts            */
#define isOption(param,opname1,opname2)   (strcmp(param,opname1)==0 || strcmp(param,opname2)==0)



#endif /* bas2img_h */
