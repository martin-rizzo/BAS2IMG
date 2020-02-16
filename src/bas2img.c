/**
 * @file       bas2img.c
 * @date       Feb 15, 2020
 * @author     Martin Rizzo | <martinrizzo@gmail.com>
 * @copyright  Copyright (c) 2020 Martin Rizzo.
 *             This project is released under the MIT License.
 * -------------------------------------------------------------------------
 *  BAS2IMG -
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define VERSION   "0.1"
#define COPYRIGHT "Copyright (c) 2020 Martin Rizzo"
#define DIR_SEPARATOR1 '\\'
#define DIR_SEPARATOR2 '/'
#define EXT_SEPARATOR   '.'
#define paramIs(param,name1,name2) (strcmp(param,name1)==0 || strcmp(param,name2)==0)
typedef char utf8;                        /* < unicode variable width character encoding */
typedef int Bool; enum { FALSE=0, TRUE }; /* < Boolean */

/* supported errors */
typedef enum ErrorID {
    SUCCESS=0, ERR_UNKNOWN_PARAM=-1000, ERR_FILE_NOT_FOUND, ERR_FILE_TOO_LARGE, ERR_FILE_TOO_SMALL,
    ERR_NOT_ENOUGH_MEMORY, ERR_GIF_NOT_SUPPORTED
} ErrorID;

typedef enum ExtensionMethod { OPTIONAL_EXTENSION, FORCED_EXTENSION } ExtensionMethod;
typedef enum ImageFormat     { BMP, GIF             } ImageFormat;
typedef enum Orientation     { HORIZONTAL, VERTICAL } Orientation;
typedef enum Mode            { GENERATE_IMAGE, LIST_ALL_MACHINES, EXPORT_CHARSET, IMPORT_CHARSET } Mode;

typedef struct Error { ErrorID id; const utf8 *str; } Error;


/*=================================================================================================================*/
#pragma mark - > HELPER FUNCTIONS

/**
 * Replaces the sign '$' contained in message with the text provided in 'str'
 * @param buffer   The buffer where the composed string will be written
 * @param message  The message to copy to the buffer
 * @param str      The text to use as a replacement for the sign '$'
 */
static const utf8 * strblend(utf8 *buffer, const utf8 *message, const utf8 *str) {
    utf8 *dest=buffer; const utf8 *ptr=message;
    while (*ptr!='$' && *ptr!='\0') { *dest++=*ptr++; }
    if (*ptr=='$') { ++ptr; while (*str!='\0') { *dest++=*str++; } while (*ptr!='\0') { *dest++=*ptr++; } }
    *dest='\0';
    return buffer;
}

/**
 *
 */

static const utf8* allocFileName(const utf8* originalFilePath, const utf8* newExtension, ExtensionMethod method) {
    assert( originalFilePath!=NULL && newExtension!=NULL );
    assert( method==OPTIONAL_EXTENSION || method==FORCED_EXTENSION );
    const utf8 *sour; utf8 *dest, *oldExtension=NULL;
    const utf8 *fileName;
    
    fileName = dest = malloc( strlen(originalFilePath)+strlen(newExtension)+1 );
    for (sour=originalFilePath; *sour!='\0'; *dest++=*sour++) {
        if (*sour==EXT_SEPARATOR) { oldExtension=dest; }
    }
    if (method==FORCED_EXTENSION || (method==OPTIONAL_EXTENSION && !oldExtension)) {
        if (oldExtension) { dest=oldExtension; }
        sour=newExtension; while (*sour!='\0') { *dest++=*sour++; }
    }
    *dest='\0';
    return fileName;
}



/*=================================================================================================================*/
#pragma mark - > HANDLING ERRORS

static Error theError = { SUCCESS, NULL };

static Bool err2(ErrorID errorID, const utf8 *str) {
    theError.id  = errorID;
    theError.str = str;
    return errorID==SUCCESS;
}

#define err(id) err2(id,NULL);

#ifdef NDEBUG
#    define DLOG(x)
#else
#    define DLOG(x) printf x; printf("\n")
#endif


static Bool printErrorMessage(void) {
    const utf8 *message; Error* error=&theError;
    utf8 *buffer = NULL;
    switch (error->id) {
        case SUCCESS:                message = "SUCCESS"; break;
        case ERR_UNKNOWN_PARAM:      message = "unknown parameter '$'"; break;
        case ERR_FILE_NOT_FOUND:     message = "file not found";    break;
        case ERR_FILE_TOO_LARGE:     message = "the file '$' is too large"; break;
        case ERR_FILE_TOO_SMALL:     message = "the file '$' is too small"; break;
        case ERR_NOT_ENOUGH_MEMORY:  message = "not enough memory"; break;
        case ERR_GIF_NOT_SUPPORTED:  message = "GIF format isn't supported yet"; break;
        default:                     message = "unknown error";     break;
    }
    if (error->str)  {
        buffer  = malloc(strlen(message)+strlen(error->str)+1);
        message = strblend(buffer,message,error->str);
    }
    printf("%s %s\n", "error:", message);
    free( buffer );
    return error->id!=SUCCESS;
}


/*=================================================================================================================*/
#pragma mark - > IMPORTING / EXPORTING CHARSET


Bool importCharset(const utf8 *imageName, ImageFormat imageFormat) {
    const utf8 *inputFilePath;
    const utf8 *outputFilePath;
    assert( imageName!=NULL );
    if (imageFormat==GIF) { return err(ERR_GIF_NOT_SUPPORTED); }
    
    inputFilePath  = allocFileName(imageName, ".bmp", OPTIONAL_EXTENSION);
    outputFilePath = allocFileName(imageName, ".h"  , FORCED_EXTENSION  );
    
    /*
    file       = fopen(fileName,"rb");
    fileSize   = getFileSize(file);
    fileBuffer = malloc( fileSize );
     */
    
    /* process */
    DLOG(("inputFilePath = %s", inputFilePath));
    DLOG(("outputFilePath = %s", outputFilePath));
    
    free((void*)inputFilePath);
    free((void*)outputFilePath);
    return TRUE;
}

Bool exportCharset(const utf8 *machine) {
    return TRUE;
}

/*=================================================================================================================*/
#pragma mark - > MAIN


int main(int argc, char* argv[]) {
    int i;
    Bool printHelpAndExit      = FALSE;
    Bool printVersionAndExit   = FALSE;
    Bool briefListing          = TRUE;
    Mode        mode           = GENERATE_IMAGE;
    ImageFormat imageFormat    = BMP;
    Orientation orientation    = HORIZONTAL;
    const utf8 *inputFileName  = NULL;
    const utf8 *machineName    = NULL;
    const utf8 *param;
    const utf8 *help[] = {
        "USAGE: bas2img [options] file.bas","",
        "  OPTIONS:",
        "    -m  --machine <name>  ",
        "    -l  --list            list names of all available machines",
        "    -L  --list-all        list names of all available machines and its variations",
        "    -b  --bmp             generate BMP image (default)",
        "    -g  --gif             generate GIF image",
        "    -H  --horizontal      use horizontal orientation (default)",
        "    -V  --vertical        use vertical orientation",
        "    -X  --export-charset  draw the machine character-set into the image",
        "    -@  --import-charset  (intended for development use)",
        "    -h, --help            display this help and exit",
        "    -v, --version         output version information and exit",
        NULL
    };

    /* process all parameters */
    for (i=1; i<argc; ++i) { param=argv[i];
        if      ( param[0]!='-' ) { inputFileName=param; }
        else if ( paramIs(param,"-m","--machine"       ) ) { machineName=argv[++i];   }
        else if ( paramIs(param,"-l","--list"          ) ) { mode=LIST_ALL_MACHINES; briefListing=TRUE;   }
        else if ( paramIs(param,"-L","--list-all"      ) ) { mode=LIST_ALL_MACHINES; briefListing=FALSE;  }
        else if ( paramIs(param,"-b","--bmp"           ) ) { imageFormat=BMP;          }
        else if ( paramIs(param,"-g","--gif"           ) ) { imageFormat=GIF;          }
        else if ( paramIs(param,"-H","--horizontal"    ) ) { orientation=HORIZONTAL;   }
        else if ( paramIs(param,"-V","--vertical"      ) ) { orientation=VERTICAL;     }
        else if ( paramIs(param,"-X","--export-charset") ) { mode=EXPORT_CHARSET;      }
        else if ( paramIs(param,"-@","--import-charset") ) { mode=IMPORT_CHARSET;      }
        else if ( paramIs(param,"-h","--help"          ) ) { printHelpAndExit=TRUE;    }
        else if ( paramIs(param,"-v","--version"       ) ) { printVersionAndExit=TRUE; }
        else    { err2(ERR_UNKNOWN_PARAM,param); printErrorMessage(); return 0; }
    }
    
    if ( printHelpAndExit ) {
        i=0; while (help[i]!=NULL) { printf("%s\n",help[i++]); }
        return 0;
    }
    if ( printVersionAndExit ) {
        printf("BAS2IMG version %s\n", VERSION  );
        printf("%s\n"                , COPYRIGHT);
        return 0;
    }
    switch ( mode ) {
            /*
        case GENERATE_IMAGE:    generateImage(outputFileName, inputFileName, machineName, imageFormat); break;
        case LIST_ALL_MACHINES: listAllMachines(briefListing); break;
        case EXPORT_CHARSET:    exportCharset(machineName); break;
             */
        case IMPORT_CHARSET:    importCharset(inputFileName,imageFormat); break;
        default: break;
    }
    return 0;
}



