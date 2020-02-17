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
#define MIN_FILE_SIZE   (0)           /* < minimum size for loadable files (in bytes)   */
#define MAX_FILE_SIZE   (1024L*1024L) /* < maximum size for loadable files (in bytes)   */
#define DIR_SEPARATOR1 '\\'
#define DIR_SEPARATOR2 '/'
#define EXT_SEPARATOR   '.'
#define isEqual(param,name1,name2)   (strcmp(param,name1)==0 || strcmp(param,name2)==0)
#define getNextStrParam(i,argc,argv) ((i)<((argc)-1) ? argv[++i] : "")
typedef unsigned char Byte;               /* < Byte (size=8bits)                         */
typedef char utf8;                        /* < unicode variable width character encoding */
typedef int Bool; enum { FALSE=0, TRUE }; /* < Boolean */

/* supported errors */
typedef enum ErrorID {
    SUCCESS=0, ERR_UNKNOWN_PARAM, ERR_FILE_NOT_FOUND, ERR_FILE_TOO_LARGE, ERR_FILE_TOO_SMALL,
    ERR_CANNOT_CREATE_FILE, ERR_CANNOT_READ_FILE, ERR_NOT_ENOUGH_MEMORY, ERR_GIF_NOT_SUPPORTED,
    ERR_FILE_IS_NOT_BMP, ERR_BMP_MUST_BE_128PX, ERR_BMP_MUST_BE_1BIT,
    ERR_BMP_UNSUPPORTED_FORMAT, ERR_BMP_INVALID_FORMAT
} ErrorID;

typedef enum ExtensionMethod { OPTIONAL_EXTENSION, FORCED_EXTENSION } ExtensionMethod;
typedef enum ImageFormat     { BMP, GIF             } ImageFormat;
typedef enum Orientation     { HORIZONTAL, VERTICAL } Orientation;
typedef enum Mode            { GENERATE_IMAGE, LIST_ALL_COMPUTERS, EXPORT_FONT, IMPORT_FONT } Mode;

typedef struct Error { ErrorID id; const utf8 *str; } Error;


/*=================================================================================================================*/
#pragma mark - > HELPER FUNCTIONS

/**
 * Replaces the sign '$' contained in message with the text provided in 'str'
 * @param buffer   The buffer where the composed string will be written
 * @param message  The message to copy to the buffer
 * @param str      The text to use as a replacement for the sign '$' (optional, can be NULL)
 */
static const utf8 * strblend(utf8 *buffer, const utf8 *message, const utf8 *str) {
    assert( buffer!=NULL && message!=NULL );
    utf8 *dest=buffer; const utf8 *ptr=message;
    while (*ptr!='$' && *ptr!='\0') { *dest++=*ptr++; }
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

/**
 *
 */

static const utf8* allocFilePath(const utf8* originalFilePath, const utf8* newExtension, ExtensionMethod method) {
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
        case ERR_NOT_ENOUGH_MEMORY:  message = "not enough memory"; break;
        case ERR_GIF_NOT_SUPPORTED:  message = "GIF format isn't supported yet"; break;
        case ERR_FILE_IS_NOT_BMP:    message = "file '$' is not a BMP file"; break;
        case ERR_BMP_MUST_BE_128PX:  message = "image in '$' must have a size of exactly 128 by 128 pixels"; break;
        case ERR_BMP_MUST_BE_1BIT:   message = "image in '$' must be 1 bit per pixel monochrome bitmap"; break;
        case ERR_BMP_UNSUPPORTED_FORMAT: message = "the BMP format in '$' is not supported by BAS2IMG"; break;
        case ERR_BMP_INVALID_FORMAT: message = "file '$' has a wrong BMP format or is corrupt"; break;
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
#pragma mark - > BITMAP

#define getInt16(ptr) (ptr[0] | ptr[1]<<8); ptr+=2;
#define getInt32(ptr) (ptr[0] | ptr[1]<<8 | ptr[2]<<16 | ptr[3]<<24); ptr+=4;

typedef struct BitmapHeader {
    unsigned fileType;
    unsigned fileSize;
    unsigned pixelDataOffset;
    unsigned headerSize;
    unsigned imageWidth;
    unsigned imageHeight;
    unsigned planes;
    unsigned bitsPerPixel;
    unsigned compression;
    unsigned totalColors;
    unsigned importantColors;
    unsigned scanlineSize;
} BitmapHeader;

typedef struct GifHeader {
    
} GifHeader;

Bool loadBitmapHeader(BitmapHeader *bmp, const Byte* data) {
    const Byte *ptr = data;
    bmp->fileType        = getInt16(ptr);
    bmp->fileSize        = getInt32(ptr);
    ptr+=4; /* skip reserved values */
    bmp->pixelDataOffset = getInt32(ptr);
    bmp->headerSize      = getInt32(ptr);
    bmp->imageWidth      = getInt32(ptr);
    bmp->imageHeight     = getInt32(ptr);
    bmp->planes          = getInt16(ptr);
    bmp->bitsPerPixel    = getInt16(ptr);
    bmp->compression     = getInt32(ptr);
    ptr+=12; /* skip imageSize, xPixelsPerMeter & yPixelsPerMeter */
    bmp->totalColors     = getInt32(ptr);
    bmp->importantColors = getInt32(ptr);
    bmp->scanlineSize    = ((bmp->imageWidth*bmp->bitsPerPixel-1)/32+1)*4;
    /*
    DLOG(("fileType: %d", bmp.fileType));
    DLOG(("fileSize: %d", bmp.fileSize));
    DLOG(("pixelDataOffset: %d", bmp.pixelDataOffset));
    DLOG(("headerSize: %d", bmp.headerSize));
    DLOG(("imageWidth: %d", bmp.imageWidth));
    DLOG(("imageHeight: %d", bmp.imageHeight));
    DLOG(("planes: %d", bmp.planes));
    DLOG(("bitsPerPixel: %d", bmp.bitsPerPixel));
    DLOG(("compression: %d", bmp.compression));
    DLOG(("totalColors: %d", bmp.totalColors));
    DLOG(("importantColors: %d", bmp.importantColors));
    DLOG(("scanlineSize: %d", bmp.scanlineSize));
    */
    return TRUE;
}


/*=================================================================================================================*/
#pragma mark - > EXPORTING FONTS


/*=================================================================================================================*/
#pragma mark - > IMPORTING FONTS

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
    for (charIdx=0,y=0; y<16; ++y) {
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
    Byte *imageBuffer=NULL; BitmapHeader bmp;
    
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
    if (isRunning()) { /* 3) extract bitmap header and verify correct format */
        loadBitmapHeader(&bmp, imageBuffer);
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
    if (isRunning()) { /* 4) write the C array to the output file */
        writeCArrayFromImageBuffer(outputFile, arrayName,
                                   &imageBuffer[bmp.pixelDataOffset], -bmp.scanlineSize, orientation);
    }
    free(imageBuffer);
    return isRunning();
}

/**
 * Creates a font header (.h) containing the C code to recreate the font image
 *
 * The font image must be a 2-colors BMP file, it must be of size 128x128 pixels
 * and it must contain all the characters arranged in 16 columns and 16 rows
 * @param headerFilePath  The full path to the new header (.h) to create (can be NULL)
 * @param imageFilePath   The full path to the input image
 * @param imageFormat     The format of the input image (only BMP format is supported)
 * @param orientation     The order of characters in the image (vertical slices, horizontal slices)
 */
Bool createFontHeader(const utf8 *headerFilePath, const utf8 *imageFilePath, ImageFormat imageFormat, Orientation orientation) {
    FILE *imageFile=NULL, *headerFile=NULL; long imageFileSize=0;
    assert( imageFilePath!=NULL );
    assert( imageFormat==BMP || imageFormat==GIF );
    assert( orientation==HORIZONTAL || orientation==VERTICAL );
    assert( isRunning() );
    if (imageFormat==GIF) { return err(ERR_GIF_NOT_SUPPORTED); }
    
    /* add extensions (when appropiate) */
    imageFilePath = allocFilePath(imageFilePath, ".bmp", OPTIONAL_EXTENSION);
    if (headerFilePath) { headerFilePath = allocFilePath(headerFilePath,".h",OPTIONAL_EXTENSION); }
    else                { headerFilePath = allocFilePath(imageFilePath ,".h",FORCED_EXTENSION  ); }
    
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
        headerFile = fopen(headerFilePath,"w");
        if (!headerFile) { err2(ERR_CANNOT_CREATE_FILE,headerFilePath); }
    }
    if (isRunning()) { /* 4) proceed! */
        printf("Creating font header %s\n", headerFilePath);
        writeCArrayFromBitmapFile(headerFile,"msx",imageFile,imageFileSize,imageFilePath,orientation);
    }
    /* clean up and return */
    if (imageFile     ) { fclose(imageFile ); imageFile =NULL; }
    if (headerFile    ) { fclose(headerFile); headerFile=NULL; }
    if (imageFilePath ) { free((void*)imageFilePath ); imageFilePath =NULL; }
    if (headerFilePath) { free((void*)headerFilePath); headerFilePath=NULL; }
    return isRunning();
}

Bool exportFont(const utf8 *computerName) {
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
    const utf8 *computerName   = NULL;
    const utf8 *param;
    const utf8 *help[] = {
        "USAGE: bas2img [options] file.bas","",
        "  OPTIONS:",
        "    -c  --computer <name>  ",
        "    -l  --list            list names of all available computers",
        "    -L  --list-all        list names of all available computers and its variations",
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
        else if ( isEqual(param,"-c","--computer"   ) ) { computerName=getNextStrParam(i,argc,argv);    }
        else if ( isEqual(param,"-l","--list"       ) ) { mode=LIST_ALL_COMPUTERS; briefListing=TRUE;   }
        else if ( isEqual(param,"-L","--list-all"   ) ) { mode=LIST_ALL_COMPUTERS; briefListing=FALSE;  }
        else if ( isEqual(param,"-b","--bmp"        ) ) { imageFormat=BMP;          }
        else if ( isEqual(param,"-g","--gif"        ) ) { imageFormat=GIF;          }
        else if ( isEqual(param,"-H","--horizontal" ) ) { orientation=HORIZONTAL;   }
        else if ( isEqual(param,"-V","--vertical"   ) ) { orientation=VERTICAL;     }
        else if ( isEqual(param,"-X","--export-font") ) { mode=EXPORT_FONT;         }
        else if ( isEqual(param,"-@","--import-font") ) { mode=IMPORT_FONT;         }
        else if ( isEqual(param,"-h","--help"       ) ) { printHelpAndExit=TRUE;    }
        else if ( isEqual(param,"-v","--version"    ) ) { printVersionAndExit=TRUE; }
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
    else if (mode==IMPORT_FONT) {
        createFontHeader(NULL, inputFileName, imageFormat, orientation);
    }
    else if (mode==EXPORT_FONT) {
        
        
    }
    else if (mode==LIST_ALL_COMPUTERS) {
        
    }
    else if (mode==GENERATE_IMAGE) {
        
    }
    return printErrorMessage();
}



