/**
 * @file       database.c
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
#include <stdio.h>
#include <string.h>
#include "database.h"

/*=================================================================================================================*/
#pragma mark - > THE FONTS

extern Font font_atari;
extern Font font_msx;
extern Font font_msxdin;
static const Font *theFonts[] = {
    &font_atari,
    &font_msx,
    &font_msxdin,
    NULL
};


/*=================================================================================================================*/
#pragma mark - > THE DECODERS

extern Decoder decoder_atari;
extern Decoder decoder_msx;
extern Decoder decoder_msxasc;
static const Decoder *theDecoders[] = {
    &decoder_atari,
    &decoder_msx,
    &decoder_msxasc,
    NULL
};


/*=================================================================================================================*/
#pragma mark - > THE COMPUTERS

static const Computer computer_atari = { "atari", "Atari 8bits", &decoder_atari, &font_atari };
static const Computer computer_msx   = { "msx", "MSX", &decoder_msxasc, &font_msx };

static const Computer *theComputers[] = {
    &computer_atari,
    &computer_msx,
    NULL
};


/*=================================================================================================================*/
#pragma mark - > ACCESSING DATA

/**
 * Returns information of the font that match with the provided name
 * @param name  The name of the searched font
 * @returns     A 'Font' structure with the information or NULL if the font cannot be found
 */
const Font * getFont(const utf8 *name) {
    int i=0; while ( theFonts[i] && strcmp(theFonts[i]->name,name)!=0 ) { ++i; }
    return theFonts[i];
}

/**
 * Returns information of the computer that match with the provided name
 * @param name  The name of the searched computer
 * @returns     A 'Computer' structure with the information or NULL if the computer cannot be found
 */
const Computer * getComputer(const utf8 *name) {
    int i=0; while ( theComputers[i] && strcmp(theComputers[i]->name,name)!=0 ) { ++i; }
    return theComputers[i];
}

/**
 * Prints the list of available file decoders to stdout
 * @param printAll  If it is 'FALSE' then only main decoders will be printed
 */
void printAvailableDecoders(Bool printAll) {
    int i;
    printf("Available decoders:\n"); for (i=0; theDecoders[i]; ++i) {
        printf("    %-10s = %s\n", theDecoders[i]->name, theDecoders[i]->description);
    }
}

/**
 * Prints the list of available fonts to stdout
 * @param printAll  If it is 'FALSE' then only main fonts will be printed
 */
void printAvailableFonts(Bool printAll) {
    int i;
    printf("Available fonts:\n"); for (i=0; theFonts[i]; ++i) {
        printf("    %-10s = %s\n", theFonts[i]->name, theFonts[i]->description);
    }
}

/**
 * Prints the list of available computers to stdout
 * @param printAll  If it is 'FALSE' then only main computers will be printed
 */
void printAvailableComputers(Bool printAll) {
    int i;
    printf("Available computers:\n"); for (i=0; theComputers[i]; ++i) {
        printf("    %-10s = %s\n", theComputers[i]->name, theComputers[i]->description);
    }
}
