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
#include "database.h"
#include "generate.h"
#include "import.h"
#include "export.h"
#include "bmp.h"
#include "gif.h"
#define VERSION   "0.1"
#define COPYRIGHT "Copyright (c) 2020 Martin Rizzo"


/*=================================================================================================================*/
#pragma mark - > HELPER FUNCTIONS

/**
 * Returns `TRUE` if param is equal to any of provided names
 */
#define isOption(param,nameToCheck1,nameToCheck2) \
    (strcmp(param,nameToCheck1)==0 || strcmp(param,nameToCheck2)==0)

/**
 * Returns `TRUE` if command is equal to the provided name
 */
#define isCommand(command,nameToCheck) \
    (strcmp(command,nameToCheck)==0)

/**
 * Returns the first character of the provided string
 */
#define firstChar(str) \
    (str[0])

/**
 * Returns the next argument in the array or empty string if there is not more arguments
 */
static const utf8 * getOptionCfg(int *inout_index, int argc, char* argv[]) {
    const int nexti = (*inout_index)+1; const utf8 *nextparam = "";
    if ( nexti<argc && argv[nexti][0]!='-' ) { (*inout_index)=nexti; nextparam=argv[nexti]; }
    return nextparam;
}

/**
 * Prints the provided text lines to stdout
 * @param helpTextLines  An array of strings containing each text line to print
 * @param isMainHelp     `TRUE` when printing the application main help
 */
static Bool printHelp(const utf8** helpTextLines, Bool isMainHelp) {
    int i; static const utf8 *mainHelpFooterLines[] = {
        "",
        "LIST OF COMMANDS:",
        "   list-computers    list names of available computers",
        "   list-fonts        list available computer fonts",
        "   export-font       draw the specified font into an image",
        "   import-font       (intended for development use)",
        NULL
    };
    i=0; while (helpTextLines[i]) { printf("%s\n",helpTextLines[i++]); }
    if (isMainHelp) { printHelp(mainHelpFooterLines,FALSE); }
    return TRUE;
}

/**
 * Prints the BAS2IMG version to stdout
 */
static Bool printVersion(void) {
    printf("BAS2IMG version %s\n", VERSION  );
    printf("%s\n"                , COPYRIGHT);
    return TRUE;
}


/*=================================================================================================================*/
#pragma mark - > SUB-COMMANDS

/**
 * Handles the command to generate the image of the BASIC program source
 * @param argc  The number of elements in the 'argv'
 * @param argv  An array containing each command-line parameter (starting at argv[1])
 * @returns     `TRUE` if the image is generated successfully
 */
static int cmdGenerateImage(int argc, char* argv[]) {
    int i; const utf8 *param; Config config;
    static const utf8 *help[] = {
        "USAGE:",
        "   bas2img #<computer-name> [options] file.bas",
        "   bas2img COMMAND [options]"
        "",
        "  OPTIONS:",
        "    !<font-name>             force to use a specific font",
        "    -b  --bmp                generate BMP image (default)",
        "    -g  --gif                generate GIF image",
        "    -c  --char-width <n>     width of each character in pixels (default = 8)",
        "    -l  --line-length <n>    maximum number of character per line (default = 0)",
        "    -w  --wrap               wrap long lines",
        "    -s  --scale <n>          scale each character by <n>",
        "    -H  --horizontal         use horizontal orientation (default)",
        "    -V  --vertical           use vertical orientation",
        "    -o  --output <file>      write the generated image to <file>",
        "    -h, --help               display this help and exit",
        "    -v, --version            output version information and exit",
        NULL
    };
    const utf8 *computerName   = NULL;
    const utf8 *fontName       = NULL;
    const utf8 *basicFilePath  = NULL;
    const utf8 *outputFilePath = NULL;
    Bool   printHelpAndExit    = (argc<=1);
    Bool   printVersionAndExit = FALSE;
    config.charWidth    = 0; /* < 0 = use computer default */
    config.charHeight   = 0; /* < 0 = use computer default */
    config.charScale    = 0; /* < 0 = use computer default */
    config.margin       = 0;
    config.padding      = 0;
    config.lineWidth    = 0;
    config.lineWrapping = FALSE;
    config.imageFormat  = BMP;
    config.orientation  = HORIZONTAL;
    config.computer     = NULL;

    /* process all parameters */
    for (i=1; i<argc; ++i) { param=argv[i];
        if      ( firstChar(param)=='#' ) { computerName  = param; }
        else if ( firstChar(param)=='!' ) { fontName      = param; }
        else if ( firstChar(param)!='-' ) { basicFilePath = param; }
        else if ( isOption(param,"-b","--bmp"        ) ) { config.imageFormat=BMP;          }
        else if ( isOption(param,"-g","--gif"        ) ) { config.imageFormat=GIF;          }
        else if ( isOption(param,"-c","--char-width" ) ) { config.charWidth=atoi(getOptionCfg(&i,argc,argv)); }
        else if ( isOption(param,"-l","--line-length") ) { config.lineWidth=atoi(getOptionCfg(&i,argc,argv)); }
        else if ( isOption(param,"-w","--wrap"       ) ) { config.lineWrapping=TRUE; }
        else if ( isOption(param,"-s","--scale"      ) ) { config.charScale=atoi(getOptionCfg(&i,argc,argv)); }
        else if ( isOption(param,"-H","--horizontal" ) ) { config.orientation=HORIZONTAL;   }
        else if ( isOption(param,"-V","--vertical"   ) ) { config.orientation=VERTICAL;     }
        else if ( isOption(param,"-o","--output"     ) ) { outputFilePath=param; }
        else if ( isOption(param,"-h","--help"       ) ) { printHelpAndExit=TRUE;    }
        else if ( isOption(param,"-v","--version"    ) ) { printVersionAndExit=TRUE; }
        else    { return error(ERR_UNKNOWN_PARAM,param); }
    }
    
    if      ( printHelpAndExit    ) { return printHelp(help,TRUE); }
    else if ( printVersionAndExit ) { return printVersion();       }
    
    if (!computerName) { return error(ERR_MISSING_COMPUTER_NAME,0); }
    config.computer = getComputer(computerName);
    if (!config.computer) { return error(ERR_NONEXISTENT_COMPUTER,computerName); }
    
    if (!basicFilePath) { return error(ERR_MISSING_BAS_PATH,0); }
    generateImageFromBASIC(outputFilePath, basicFilePath, &config);
    return success ? TRUE : FALSE;
}

/**
 * Handles the command to prints the list of available computers (list-computers)
 * @param argc  The number of elements in the 'argv'
 * @param argv  An array containing each command-line parameter (starting at argv[1])
 * @returns     `TRUE` if the list is printed successfully
 */
static Bool cmdListComputers(int argc, char* argv[]) {
    int i; const utf8 *param;
    static const utf8 *help[] = {
        "USAGE:",
        "   bas2img list-computers [options]",
        "",
        "  OPTIONS:",
        "    -a  --all         include all computers variations",
        "    -h  --help        display this help and exit",
        NULL
    };
    Bool printHelpAndExit = FALSE;
    Bool printAll         = FALSE;
    assert( argv!=NULL );
    /* process all parameters */
    for (i=1; i<argc; ++i) { param=argv[i];
        if      ( isOption(param,"-a","--all"    ) ) { printAll         = TRUE; }
        else if ( isOption(param,"-h","--help"   ) ) { printHelpAndExit = TRUE; }
        else    { return error(ERR_UNKNOWN_PARAM,param); }
    }
    if ( printHelpAndExit ) { return printHelp(help,FALSE); }
    printAvailableComputers(printAll);
    return success ? TRUE : FALSE;
}

/**
 * Handles the command to prints the list of available fonts (list-fonts)
 * @param argc  The number of elements in the 'argv'
 * @param argv  An array containing each command-line parameter (starting at argv[1])
 * @returns     `TRUE` if the list is printed successfully
 */
static Bool cmdListFonts(int argc, char* argv[]) {
    int i; const utf8 *param;
    static const utf8 *help[] = {
        "USAGE:",
        "   bas2img list-fonts [options]",
        "",
        "  OPTIONS:",
        "    -a  --all         include fonts of all computers variations",
        "    -h  --help        display this help and exit",
        NULL
    };
    Bool printHelpAndExit = FALSE;
    Bool printAll         = FALSE;
    assert( argv!=NULL );
    /* process al parameters */
    for (i=1; i<argc; ++i) { param=argv[i];
        if      ( isOption(param,"-a","--all"    ) ) { printAll         = TRUE; }
        else if ( isOption(param,"-h","--help"   ) ) { printHelpAndExit = TRUE; }
        else    { return error(ERR_UNKNOWN_PARAM,param); }
    }
    if ( printHelpAndExit ) { return printHelp(help,FALSE); }
    printAvailableFonts(printAll);
    return success ? TRUE : FALSE;
}

/**
 * Handles the command to export fonts (export-font)
 * @param argc  The number of elements in the 'argv'
 * @param argv  An array containing each command-line parameter (starting at argv[1])
 * @returns     `TRUE` if the font is exported successfully
 */
static Bool cmdExportFont(int argc, char* argv[]) {
    int i; const utf8 *param; const Font* font;
    static const utf8 *help[] = {
        "USAGE:",
        "   bas2img export-font [options] !<font-name>",
        "",
        "  OPTIONS:",
        "    -H  --horizontal         use horizontal orientation (default)",
        "    -V  --vertical           use vertical orientation",
        "    -o  --output <file>      write the output image to <file>",
        "    -h  --help               display this help and exit",
        NULL
    };
    const utf8* fontName       = NULL;
    const utf8* outputFilePath = NULL;
    Orientation orientation    = HORIZONTAL;
    Bool printHelpAndExit      = (argc<=1);
    assert( argv!=NULL );
    /* process al parameters */
    for (i=1; i<argc; ++i) { param=argv[i];
        if      ( firstChar(param)=='!'                ) { fontName         = param;      }
        else if ( isOption(param,"-H","--horizontal" ) ) { orientation      = HORIZONTAL; }
        else if ( isOption(param,"-V","--vertical"   ) ) { orientation      = VERTICAL;   }
        else if ( isOption(param,"-o","--output"     ) ) { outputFilePath   = param;      }
        else if ( isOption(param,"-h","--help"       ) ) { printHelpAndExit = TRUE;       }
        else  { return error(ERR_UNKNOWN_PARAM,param); }
    }
    
    if ( printHelpAndExit ) { return printHelp(help,FALSE); }
    
    if (!fontName) { return error(ERR_MISSING_FONT_NAME,0); }
    font = getFont(fontName);
    if (!font) { return error(ERR_NONEXISTENT_FONT,fontName); }
    exportFont(font,orientation);
    return success ? TRUE : FALSE;
}

/**
 * Handles the command to import fonts (import-font)
 * @param argc  The number of elements in the 'argv'
 * @param argv  An array containing each command-line parameter (starting at argv[1])
 * @returns     `TRUE` if the font is imported successfully
 */
static Bool cmdImportFont(int argc, char* argv[]) {
    int i; const utf8 *param;
    static const utf8 *help[] = {
        "USAGE:",
        "   bas2img import-font [options] <image-file>",
        "",
        "  OPTIONS:",
        "    -b  --bmp                generate BMP image (default)",
        "    -g  --gif                generate GIF image",
        "    -H  --horizontal         use horizontal orientation (default)",
        "    -V  --vertical           use vertical orientation",
        "    -o  --output <file>      write the generated C source code to <file>",
        "    -h  --help               display this help and exit",
        NULL
    };
    const utf8* fontName       = NULL;
    const utf8* imageFilePath  = NULL;
    const utf8* outputFilePath = NULL;
    Orientation orientation    = HORIZONTAL;
    ImageFormat imageFormat    = BMP;
    Bool printHelpAndExit      = (argc<=1);
    assert( argv!=NULL );
    /* process al parameters */
    for (i=1; i<argc; ++i) { param=argv[i];
        if      ( firstChar(param)=='!'                ) { fontName         = param;      }
        else if ( isOption(param,"-b","--bmp"        ) ) { imageFormat      = BMP;        }
        else if ( isOption(param,"-g","--gif"        ) ) { imageFormat      = GIF;        }
        else if ( isOption(param,"-H","--horizontal" ) ) { orientation      = HORIZONTAL; }
        else if ( isOption(param,"-V","--vertical"   ) ) { orientation      = VERTICAL;   }
        else if ( isOption(param,"-o","--output"     ) ) { outputFilePath   = param;      }
        else if ( isOption(param,"-h","--help"       ) ) { printHelpAndExit = TRUE;       }
        else  { return error(ERR_UNKNOWN_PARAM,param); }
    }

    if ( printHelpAndExit ) { return printHelp(help,FALSE); }
    
    if (!imageFilePath) { return error(ERR_MISSING_FONTIMG_PATH,0); }
    importArrayFromImage(outputFilePath, imageFilePath, imageFormat, orientation);
    return success ? TRUE : FALSE;
}


/*=================================================================================================================*/
#pragma mark - > MAIN

/**
 * Application starting point
 * @param argc  The number of elements in the 'argv' array
 * @param argv  An array containing each command-line parameter (starting at argv[1])
 */
int main(int argc, char* argv[]) {
    const utf8* const command = argc>1 ? argv[1] : "";
    
    /* handle commands */
    if      (isCommand(command,"list-computers")) { cmdListComputers(argc-1, argv+1); }
    else if (isCommand(command,"list-fonts"    )) { cmdListFonts    (argc-1, argv+1); }
    else if (isCommand(command,"import-font"   )) { cmdImportFont   (argc-1, argv+1); }
    else if (isCommand(command,"export-font"   )) { cmdExportFont   (argc-1, argv+1); }
    else if (isCommand(command,"generate-image")) { cmdGenerateImage(argc-1, argv+1); }
    else                                          { cmdGenerateImage(argc, argv);     }
    return printErrorMessage();
}



