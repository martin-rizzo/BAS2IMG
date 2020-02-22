/**
 * @file       error.c
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
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "error.h"
#include "helpers.h"


Error theError = { SUCCESS, NULL };

Bool err2(ErrorID errorID, const utf8 *str) {
    free((void*)theError.str);
    theError.id  = errorID;
    theError.str = str ? strdup(str) : NULL;
    return errorID==SUCCESS;
}

#define isRunning() (theError.id==SUCCESS)

#define err(id) err2(id,NULL);

#ifdef NDEBUG
#    define DLOG(x)
#else
#    define DLOG(x) printf x; printf("\n")
#endif


Bool printErrorMessage(void) {
    const utf8 *message; Error* error=&theError;
    utf8 *buffer = NULL;
    switch (error->id) {
        case SUCCESS:                message = "SUCCESS"; break;
        case ERR_UNKNOWN_PARAM:      message = "unknown parameter '$'"; break;
        case ERR_FILE_NOT_FOUND:     message = "file '$' cannot be found"; break;
        case ERR_FILE_TOO_LARGE:     message = "file '$' is too large"; break;
        case ERR_FILE_TOO_SMALL:     message = "file '$' is too small"; break;
        case ERR_CANNOT_CREATE_FILE: message = "file '$' cannot be created"; break;
        case ERR_CANNOT_READ_FILE:   message = "file '$' cannot be accessed"; break;
        case ERR_CANNOT_WRITE_FILE:  message = "file '$' cannot be written"; break;
        case ERR_NOT_ENOUGH_MEMORY:  message = "not enough memory"; break;
        case ERR_GIF_NOT_SUPPORTED:  message = "GIF format isn't supported yet"; break;
        case ERR_FILE_IS_NOT_BMP:    message = "file '$' is not a BMP file"; break;
        case ERR_BMP_MUST_BE_128PX:  message = "image in '$' must have a size of exactly 128 by 128 pixels"; break;
        case ERR_BMP_MUST_BE_1BIT:   message = "image in '$' must be 1 bit per pixel monochrome bitmap"; break;
        case ERR_BMP_UNSUPPORTED_FORMAT: message = "the BMP format in '$' is not supported by BAS2IMG"; break;
        case ERR_BMP_INVALID_FORMAT: message = "file '$' has a wrong BMP format or is corrupt"; break;
        case ERR_NONEXISTENT_FONT:   message = "The font '$' does not exist. Use the '--list-fonts' option for a list of available fonts."; break;
        case ERR_INTERNAL_ERROR:     message = "Internal error (?)"; break;
        default:                     message = "unknown error"; break;
    }
    if (error->str)  {
        buffer  = malloc(strlen(message)+strlen(error->str)+1);
        message = strblend(buffer,message,error->str);
    }
    printf("%s %s\n", "error:", message);
    free( (void*)buffer     ); buffer=NULL;
    free( (void*)error->str ); error->str=NULL;
    return error->id;
}

