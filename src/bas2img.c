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
#include "font.h"
#include "bmp.h"
#include "gif.h"
#define VERSION   "0.1"
#define COPYRIGHT "Copyright (c) 2020 Martin Rizzo"
#define MIN_FILE_SIZE   (0)           /* < minimum size for loadable files (in bytes)   */
#define MAX_FILE_SIZE   (1024L*1024L) /* < maximum size for loadable files (in bytes)   */
#define DIR_SEPARATOR1 '\\'
#define DIR_SEPARATOR2 '/'
#define EXT_SEPARATOR   '.'
#define FONTIMG_WIDTH       128  /* < the width in pixels of font images  */
#define FONTIMG_HEIGHT      128  /* < the height in pixels of font images */
#define FONTIMG_NUMOFCOLORS 2    /* < the number of colors of font images */
#define FONTIMG_PREFIX      "font__"
#define isOption(param,opname1,opname2)   (strcmp(param,opname1)==0 || strcmp(param,opname2)==0)
typedef unsigned char Byte;               /* < Byte (size=8bits)                         */
typedef char utf8;                        /* < unicode variable width character encoding */
typedef int Bool; enum { FALSE=0, TRUE }; /* < Boolean */

/* supported errors */
typedef enum ErrorID {
    SUCCESS=0, ERR_UNKNOWN_PARAM, ERR_FILE_NOT_FOUND, ERR_FILE_TOO_LARGE, ERR_FILE_TOO_SMALL,
    ERR_CANNOT_CREATE_FILE, ERR_CANNOT_READ_FILE, ERR_CANNOT_WRITE_FILE, ERR_NOT_ENOUGH_MEMORY,
    ERR_GIF_NOT_SUPPORTED, ERR_FILE_IS_NOT_BMP, ERR_BMP_MUST_BE_128PX, ERR_BMP_MUST_BE_1BIT,
    ERR_BMP_UNSUPPORTED_FORMAT, ERR_BMP_INVALID_FORMAT, ERR_NONEXISTENT_FONT
} ErrorID;

typedef enum ExtensionMethod { OPTIONAL_EXTENSION, FORCED_EXTENSION } ExtensionMethod;
typedef enum ImageFormat     { BMP, GIF             } ImageFormat;
typedef enum Orientation     { HORIZONTAL, VERTICAL } Orientation;
typedef enum Mode            { GENERATE_IMAGE, LIST_ALL_COMPUTERS, LIST_ALL_FONTS, EXPORT_FONT, IMPORT_FONT } Mode;

typedef struct Error { ErrorID id; const utf8 *str; } Error;



/*=================================================================================================================*/
#pragma mark - > FONTS

static const Font *theFonts[] = { &font__msx, &font__msx_din, NULL };

/**
 * Returns the information of the font that match with the provided name
 */
static const Font * getFontWithName(const utf8 *name) {
    int i=0; while ( theFonts[i]!=NULL && strcmp(theFonts[i]->name,name)!=0 ) { ++i; }
    return theFonts[i];
}

/**
 * Lists all available fonts
 */
static void listAllFonts(void) {
    int i;
    printf("Available fonts:\n"); for (i=0; theFonts[i]; ++i) {
        printf("    %-10s = %s\n", theFonts[i]->name, theFonts[i]->description);
    }
}

/*=================================================================================================================*/
#pragma mark - > HELPER FUNCTIONS

/**
 * Replaces the sign '$' contained in message with the text provided in 'str'
 * @param buffer   The buffer where the composed string will be written
 * @param message  The message to copy to the buffer
 * @param str      The text to use as a replacement for the sign '$' (optional, can be NULL)
 */
static const utf8 * strblend(utf8 *buffer, const utf8 *message, const utf8 *str) {
    utf8 *dest; const utf8 *ptr;
    assert( buffer!=NULL && message!=NULL );
    dest=buffer; ptr=message; while (*ptr!='$' && *ptr!='\0') { *dest++=*ptr++; }
    if (*ptr=='$' && str) { ++ptr; while (*str!='\0') { *dest++=*str++; } }
    while (*ptr!='\0') { *dest++=*ptr++; }; *dest='\0';
    return buffer;
}

/**
 * Returns the file size in bytes
 */
static long getFileSize(FILE *file) {
    long fileSize; assert( file!=NULL );
    fseek(file,0,SEEK_END); fileSize=ftell(file); rewind(file);
    return fileSize;
}

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

static const utf8 * allocConcatenation(const utf8 *firstString, const utf8 *secondString) {
    utf8 *string;
    assert( firstString!=NULL && secondString!=NULL );
    string = malloc( strlen(firstString)+strlen(secondString)+1 );
    strcpy(string,firstString); strcat(string,secondString);
    return string;
}

/**
 *
 */
static const utf8* allocFilePath(const utf8* originalFilePath, const utf8* newExtension, ExtensionMethod method) {
    const utf8 *fileName, *sour; utf8 *dest, *oldExtension=NULL;
    assert( originalFilePath!=NULL && newExtension!=NULL );
    assert( method==OPTIONAL_EXTENSION || method==FORCED_EXTENSION );
    
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
    free((void*)theError.str);
    theError.id  = errorID;
    theError.str = strdup(str);
    return errorID==SUCCESS;
}

#define isRunning() (theError.id==SUCCESS)

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
        default:                     message = "unknown error";     break;
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



/*=================================================================================================================*/
#pragma mark - > EXPORTING FONTS TO FILE

Bool exportFont(const Font *font, FILE *outputFile) {

    BmpHeader bmp;
    Byte colorTable[4 * FONTIMG_NUMOFCOLORS];
    Byte pixelData[(FONTIMG_WIDTH * FONTIMG_HEIGHT) / 8];
    setBmpHeader(&bmp, FONTIMG_WIDTH, FONTIMG_HEIGHT, FONTIMG_NUMOFCOLORS);
    fwriteBmp(&bmp, colorTable, sizeof(colorTable), pixelData, sizeof(pixelData), outputFile);
    return TRUE;
}

Bool exportFontWithName(const utf8 *name) {
    const utf8 *outputFilePath=NULL, *outputFileName=NULL;
    FILE *outputFile=NULL; const Font *font=NULL;
    assert( name!=NULL );
    
    if (isRunning()) {
        font = getFontWithName(name);
        if (!font) { err2(ERR_NONEXISTENT_FONT,name); }
    }
    if (isRunning()) {
        outputFileName = allocConcatenation(FONTIMG_PREFIX, name);
        outputFilePath = allocFilePath(outputFileName, ".bmp", FORCED_EXTENSION);
        if (!outputFileName || !outputFileName) { err(ERR_NOT_ENOUGH_MEMORY); }
    }
    if (isRunning()) {
        outputFile = fopen(outputFilePath, "wb");
        if (!outputFile) { err2(ERR_CANNOT_CREATE_FILE,outputFilePath); }
    }
    if (isRunning()) {
        printf("Exporting font %s to file '%s'", name, outputFilePath);
        exportFont(font, outputFile);
    }
    /* clean up and return */
    if (outputFile) { fclose(outputFile); }
    free((void*)outputFilePath);
    free((void*)outputFileName);
    return isRunning();
}

/*=================================================================================================================*/
#pragma mark - > IMPORTING FONT INTO C-ARRAY

/**
 * Writes C code representing an array containing all the font image stored in the buffer
 * @param outputFile    The file where the generated C code will be written
 * @param arrayName     The name of the C array
 * @param imageBuffer   The buffer containing the image of 256 characters of 8x8 pixels (1 bit per pixel)
 * @param scanlineSize  The number of bytes from one line of pixels to the next (negative means "upside-down" image)
 * @param orientation   The order of characters in the image (vertical slices, horizontal slices)
 */
Bool writeCArrayFromImageBuffer(FILE       *outputFile,
                                const utf8 *arrayName,
                                const Byte *imageBuffer,
                                int         scanlineSize,
                                Orientation orientation) {
    Bool upsideDown = FALSE;
    int x,y,col,row,line,segment,charIdx; const utf8 *separator;
    const int lastCharIdx=255;
    assert( outputFile!=NULL );
    assert( arrayName!=NULL && strlen(arrayName)>0 );
    assert( imageBuffer!=NULL && scanlineSize!=0 );
    assert( orientation==HORIZONTAL || orientation==VERTICAL );
    fprintf(outputFile, "const Byte font_%s[2048] = {", arrayName);

    /* fix "upside-down" images */
    if (scanlineSize<0) { scanlineSize=-scanlineSize; upsideDown=TRUE; }
    /* write one by one all of 256 characters */
    charIdx=0; for (y=0; y<16; ++y) {
        for (x=0; x<16; ++x,++charIdx) {
            assert( charIdx<=lastCharIdx );
            if ((charIdx%2)==0) { fprintf(outputFile,"\n    "); }
            else                { fprintf(outputFile," "     ); }
            col = (orientation==HORIZONTAL ? x : y);
            row = (orientation==HORIZONTAL ? y : x);
            for (segment=0; segment<8; ++segment) {
                assert( scanlineSize>0 );
                line      = row*8+segment; if (upsideDown) { line = 127-line; }
                separator = (charIdx==lastCharIdx && segment==7) ? "" : ",";
                fprintf(outputFile, "0x%02x%s", imageBuffer[ (line*scanlineSize)+col ], separator);
            }
        }
    }
    fprintf(outputFile, "\n};\n");
    return TRUE;
}

/**
 * Writes the C code representing an array containing the font image stored in a BMP file
 *
 * The font image must be a 2-colors BMP file, it must be of size 128x128 pixels
 * and it must contain all the characters arranged in 16 columns and 16 rows
 * @param outputFile     The file where the generated C code will be written
 * @param arrayName      The name of the C array
 * @param imageFile      The file with the image encoded in BMP format
 * @param imageFileSize  The size in bytes of the BMP file
 * @param imageFilePath  The path to the BMP file (used for error report)
 * @param orientation    The order of characters in the image (vertical slices, horizontal slices)
 */
Bool writeCArrayFromBitmapFile(FILE       *outputFile,
                               const utf8 *arrayName,
                               FILE       *imageFile,
                               long        imageFileSize,
                               const utf8 *imageFilePath,
                               Orientation orientation)
{
    static const unsigned requiredWidth  = 128;
    static const unsigned requiredHeight = 128;
    int pixelDataSize, requiredDataSize;
    Byte *imageBuffer=NULL; BmpHeader bmp;
    
    assert( outputFile!=NULL );
    assert( arrayName!=NULL && strlen(arrayName)>0 );
    assert( imageFile!=NULL && imageFileSize>0 && imageFilePath!=NULL );
    assert( orientation==HORIZONTAL || orientation==VERTICAL );
    assert( isRunning() );
    
    if (isRunning()) { /* 1) allocate space to load the complete file to memory */
        imageBuffer = malloc(imageFileSize);
        if (!imageBuffer) { err(ERR_NOT_ENOUGH_MEMORY); }
    }
    if (isRunning()) { /* 2) load the file */
        if ( imageFileSize!=fread(imageBuffer,1,imageFileSize,imageFile) ) {
            err2(ERR_CANNOT_READ_FILE,imageFilePath);
        }
    }
    if (isRunning()) { /* 3) extract bitmap header */
        if (!extractBmpHeader(&bmp, imageBuffer, imageFileSize)) { err(ERR_FILE_IS_NOT_BMP); }
    }
    if (isRunning()) { /* 4) verify correct bitmap format */
        pixelDataSize    = (unsigned)imageFileSize-bmp.pixelDataOffset;
        requiredDataSize = (requiredWidth/8)*requiredHeight;
        if      (bmp.fileType     !=0x4D42         ) { err(ERR_FILE_IS_NOT_BMP       ); }
        else if (bmp.fileSize     !=imageFileSize  ) { err(ERR_BMP_INVALID_FORMAT    ); }
        else if (bmp.imageWidth   !=requiredWidth  ) { err(ERR_BMP_MUST_BE_128PX     ); }
        else if (bmp.imageHeight  !=requiredHeight ) { err(ERR_BMP_MUST_BE_128PX     ); }
        else if (bmp.planes       !=1              ) { err(ERR_BMP_INVALID_FORMAT    ); }
        else if (bmp.bitsPerPixel !=1              ) { err(ERR_BMP_MUST_BE_1BIT      ); }
        else if (bmp.compression  !=0              ) { err(ERR_BMP_UNSUPPORTED_FORMAT); }
        else if (pixelDataSize    <requiredDataSize) { err(ERR_BMP_INVALID_FORMAT    ); }
    }
    if (isRunning()) { /* 5) write the C array to the output file */
        writeCArrayFromImageBuffer(outputFile, arrayName,
                                   &imageBuffer[bmp.pixelDataOffset], -bmp.scanlineSize, orientation);
    }
    free(imageBuffer);
    return isRunning();
}

/**
 * Creates a C code representing an array containing the font image
 *
 * @param outputFilePath The path to the file where the generated C code will be written (NULL = use image name)
 * @param imageFilePath  The path to the input image
 * @param imageFormat    The format of the input image (only BMP format is supported)
 * @param orientation    The order of characters in the image (vertical slices, horizontal slices)
 */
Bool writeCArrayFromImage(const utf8  *outputFilePath,
                          const utf8  *imageFilePath,
                          ImageFormat  imageFormat,
                          Orientation  orientation)
{
    FILE *outputFile=NULL, *imageFile=NULL; long imageFileSize=0;
    assert( imageFilePath!=NULL );
    assert( imageFormat==BMP || imageFormat==GIF );
    assert( orientation==HORIZONTAL || orientation==VERTICAL );
    assert( isRunning() );
    if (imageFormat==GIF) { return err(ERR_GIF_NOT_SUPPORTED); }
    
    /* add extensions (when appropiate) */
    imageFilePath = allocFilePath(imageFilePath, ".bmp", OPTIONAL_EXTENSION);
    if (outputFilePath) { outputFilePath = allocFilePath(outputFilePath,".c",OPTIONAL_EXTENSION); }
    else                { outputFilePath = allocFilePath(imageFilePath ,".c",FORCED_EXTENSION  ); }
    
    if (isRunning()) { /* 1) open image file for reading */
        imageFile = fopen(imageFilePath,"rb");
        if (!imageFile) { err2(ERR_FILE_NOT_FOUND,imageFilePath); }
    }
    if (isRunning()) { /* 2) get size of the image file and verify it is valid */
        imageFileSize = getFileSize(imageFile);
        if (imageFileSize<MIN_FILE_SIZE) { err2(ERR_FILE_TOO_SMALL,imageFilePath); }
        if (imageFileSize>MAX_FILE_SIZE) { err2(ERR_FILE_TOO_LARGE,imageFilePath); }
    }
    if (isRunning()) { /* 3) open header file for writing */
        outputFile = fopen(outputFilePath,"w");
        if (!outputFile) { err2(ERR_CANNOT_CREATE_FILE,outputFilePath); }
    }
    if (isRunning()) { /* 4) proceed! */
        printf("Creating font source code in '%s'\n", outputFilePath);
        writeCArrayFromBitmapFile(outputFile,"msx",imageFile,imageFileSize,imageFilePath,orientation);
    }
    /* clean up and return */
    if (imageFile     ) { fclose(imageFile ); imageFile =NULL; }
    if (outputFile    ) { fclose(outputFile); outputFile=NULL; }
    if (imageFilePath ) { free((void*)imageFilePath ); imageFilePath =NULL; }
    if (outputFilePath) { free((void*)outputFilePath); outputFilePath=NULL; }
    return isRunning();
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
            exportFontWithName( fontName );
            break;
            
        case GENERATE_IMAGE:
            printf("generate image operation is not implemented yet.\n");
            break;
    }
    return printErrorMessage();
}



