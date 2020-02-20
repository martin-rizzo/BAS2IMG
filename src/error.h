/**
 * @file       error.h
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
#ifndef bas2img_error_h
#define bas2img_error_h
#include "types.h"


/* supported errors */
typedef enum ErrorID {
    SUCCESS=0, ERR_UNKNOWN_PARAM, ERR_FILE_NOT_FOUND, ERR_FILE_TOO_LARGE, ERR_FILE_TOO_SMALL,
    ERR_CANNOT_CREATE_FILE, ERR_CANNOT_READ_FILE, ERR_CANNOT_WRITE_FILE, ERR_NOT_ENOUGH_MEMORY,
    ERR_GIF_NOT_SUPPORTED, ERR_FILE_IS_NOT_BMP, ERR_BMP_MUST_BE_128PX, ERR_BMP_MUST_BE_1BIT,
    ERR_BMP_UNSUPPORTED_FORMAT, ERR_BMP_INVALID_FORMAT, ERR_NONEXISTENT_FONT,
    ERR_INTERNAL_ERROR
} ErrorID;

typedef struct Error { ErrorID id; const utf8 *str; } Error;

extern Error theError;

#define isRunning() (theError.id==SUCCESS)

#define err(id) err2(id,NULL);

Bool err2(ErrorID errorID, const utf8 *str);

Bool printErrorMessage(void);


#ifdef NDEBUG
#    define DLOG(x)
#else
#    define DLOG(x) printf x; printf("\n")
#endif



#endif /* bas2img_error_h */


