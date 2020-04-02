/**
 * @file       helpers.h
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
#ifndef bas2img_helpers_h
#define bas2img_helpers_h
#include "types.h"


typedef enum ExtensionMethod {
    OPTIONAL_EXTENSION, FORCED_EXTENSION
} ExtensionMethod;


/**
 * Replaces the sign '$' contained in message with the text provided in 'str'
 * @param buffer   The buffer where the composed string will be written
 * @param message  The message to copy to the buffer
 * @param str      The text to use as a replacement for the sign '$' (optional, can be NULL)
 */
const utf8 * strblend(utf8 *buffer, const utf8 *message, const utf8 *str);

/**
 * Returns the file size in bytes
 */
long getFileSize(FILE *file);

/**
 * Returns the file extension corresponding to the image format
 * @param imageFormat    The format of the image, ex: GIF, BMP, ...
 * @param referencePath  (optional) A path the reference to the original file, can be NULL.
 */
const utf8 * getImageExtension(ImageFormat imageFormat, const utf8* referencePath);


const utf8 * allocConcatenation(const utf8 *firstString, const utf8 *secondString);

const utf8 * allocFilePath(const utf8* originalFilePath, const utf8* newExtension, ExtensionMethod method);


/**
 * Allocates a string containing the name and extension of the file indicated by the path
 * @param filePath  The path to the file
 * @returns
 *      A new allocated string containing the name and extension,
 *      it must be deallocated with 'free()'
 */
const utf8 * allocFileNameWithExtension(const utf8* filePath);

/**
 * Allocates a string containing the name (without extension) of the file indicated by the path
 * @param filePath  The path to the file
 * @returns
 *      A new allocated string containing the name (without extension),
 *      it must be deallocated with 'free()'
 */
const utf8 * allocFileNameWithoutExtension(const utf8* filePath);

/**
 * Allocates a string containing the provided string but with the prefix removed
 * @param originalString   The string that the prefix will be removed
 * @param prefixToRemove   The prefix to remove
 * @returns
 *      A new allocated string containing the original string but with the prefix removed,
 *      it must be deallocated with 'free()'
 */
const utf8 * allocStringWithoutPrefix(const utf8 *originalString, const utf8 *prefixToRemove);

/**
 * Returns the first positive value from the provided list of values
 * @param value1   first value to check
 * @param value2   second value to check
 * @param value3   third value to check
 */
int firstPositiveValue(int value1, int value2, int value3);


#endif /* bas2img_helpers_h */

