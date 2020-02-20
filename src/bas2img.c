/**
 * @file       bas2img.c
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "bas2img.h"
#include "helpers.h"
#include "error.h"
#include "import.h"
#include "export.h"
#include "font.h"
#include "bmp.h"
#include "gif.h"
#define VERSION   "0.1"
#define COPYRIGHT "Copyright (c) 2020 Martin Rizzo"


typedef enum Mode { GENERATE_IMAGE, LIST_ALL_COMPUTERS, LIST_ALL_FONTS, EXPORT_FONT, IMPORT_FONT } Mode;



/*=================================================================================================================*/
#pragma mark - > HELPER FUNCTIONS

static const utf8 * getOptionCfg(int *inout_index, int argc, char* argv[]) {
    const int nexti = (*inout_index)+1; const utf8 *nextparam = "";
    if ( nexti<argc && argv[nexti][0]!='-' ) { (*inout_index)=nexti; nextparam=argv[nexti]; }
    return nextparam;
}

static const utf8 * firstValid(const utf8 *name1, const utf8 *name2, const utf8 *name3) {
    if (name1!=NULL && name1[0]!='\0') { return name1; }
    if (name2!=NULL && name2[0]!='\0') { return name2; }
    if (name3!=NULL && name3[0]!='\0') { return name3; }
    return "";
}


/*=================================================================================================================*/
#pragma mark - > MAIN


int main(int argc, char* argv[]) {
    int i; const Font *font;
    Bool printHelpAndExit      = FALSE;
    Bool printVersionAndExit   = FALSE;
    Bool briefListing          = TRUE;
    Mode        mode           = GENERATE_IMAGE;
    ImageFormat imageFormat    = BMP;
    Orientation orientation    = HORIZONTAL;
    const utf8 *inputFileName  = NULL;
    const utf8 *computerName   = NULL;
    const utf8 *fontName       = NULL;
    const utf8 *param;
    const utf8 *help[] = {
        "USAGE: bas2img [options] file.bas","",
        "  OPTIONS:",
        "    -c  --computer <name>  ",
        "    -l  --list            list names of all available computers",
        "    -f  --list-fonts      list all available computer fonts",
        "    -b  --bmp             generate BMP image (default)",
        "    -g  --gif             generate GIF image",
        "    -H  --horizontal      use horizontal orientation (default)",
        "    -V  --vertical        use vertical orientation",
        "    -X  --export-font     draw the computer font into the image",
        "    -@  --import-font     (intended for development use)",
        "    -h, --help            display this help and exit",
        "    -v, --version         output version information and exit",
        NULL
    };

    /* process all parameters */
    for (i=1; i<argc; ++i) { param=argv[i];
        if      ( param[0]!='-' ) { inputFileName=param; }
        else if ( isOption(param,"-c","--computer"   ) ) { computerName=getOptionCfg(&i,argc,argv);    }
        else if ( isOption(param,"-l","--list"       ) ) { mode=LIST_ALL_COMPUTERS; briefListing=TRUE;   }
        else if ( isOption(param,"-L","--list-all"   ) ) { mode=LIST_ALL_COMPUTERS; briefListing=FALSE;  }
        else if ( isOption(param,"-f","--list-fonts" ) ) { mode=LIST_ALL_FONTS;      }
        else if ( isOption(param,"-b","--bmp"        ) ) { imageFormat=BMP;          }
        else if ( isOption(param,"-g","--gif"        ) ) { imageFormat=GIF;          }
        else if ( isOption(param,"-H","--horizontal" ) ) { orientation=HORIZONTAL;   }
        else if ( isOption(param,"-V","--vertical"   ) ) { orientation=VERTICAL;     }
        else if ( isOption(param,"-X","--export-font") ) { mode=EXPORT_FONT; fontName=getOptionCfg(&i,argc,argv); }
        else if ( isOption(param,"-@","--import-font") ) { mode=IMPORT_FONT; fontName=getOptionCfg(&i,argc,argv); }
        else if ( isOption(param,"-h","--help"       ) ) { printHelpAndExit=TRUE;    }
        else if ( isOption(param,"-v","--version"    ) ) { printVersionAndExit=TRUE; }
        else    { err2(ERR_UNKNOWN_PARAM,param); printErrorMessage(); return 0; }
    }
    
    if ( printHelpAndExit ) {
        i=0; while (help[i]!=NULL) { printf("%s\n",help[i++]); }
        return 0;
    }
    else if ( printVersionAndExit ) {
        printf("BAS2IMG version %s\n", VERSION  );
        printf("%s\n"                , COPYRIGHT);
        return 0;
    }
    switch (mode) {
            
        case LIST_ALL_COMPUTERS:
            printf("list all computers operation is not implemented yet.\n");
            break;
            
        case LIST_ALL_FONTS:
            listAllFonts();
            break;
            
        case IMPORT_FONT:
            fontName = firstValid( fontName, inputFileName, NULL );
            writeCArrayFromImage(NULL, fontName, imageFormat, orientation);
            break;
            
        case EXPORT_FONT:
            fontName = firstValid( fontName, inputFileName, NULL );
            font     = getFontWithName(fontName);
            if (!font) { return err2(ERR_NONEXISTENT_FONT,fontName); }
            else       { exportFont(font,orientation);               }
            break;
            
        case GENERATE_IMAGE:
            printf("generate image operation is not implemented yet.\n");
            break;
    }
    return printErrorMessage();
}



