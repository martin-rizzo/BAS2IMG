/**
 * @file       gif.h
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
#ifndef bas2img_gif_h
#define bas2img_gif_h
#include <stdio.h>
#include "types.h"


/**
 * Writes an image to file using the GIF format
 * @param width           The width of the image in pixels
 * @param height          The height of the image in pixels
 * @param scanlineSize    The number of bytes from one line of pixels to the next
 * @param numberOfColors  The number of colors of the image (valid values: 2 or 256)
 * @param colorTable      An array of RGBA elements (32bits) that maps the values in the pixel-data to rgb colors
 * @param colorTableSize  The size of `colorTable` in number of BYTES
 * @param pixelData       An array of values describing each pixel of the image
 * @param pixelDataSize   The size of `pixelData` in number of BYTES
 */
Bool fwriteGif(int width, int height, int scanlineSize, int numberOfColors,
               const void *colorTable, int colorTableSize,
               const void *pixelData,  int pixelDataSize,
               FILE *file);


#endif /* bas2img_gif_h */
