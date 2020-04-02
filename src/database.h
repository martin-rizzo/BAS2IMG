/**
 * @file       database.h
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
#ifndef bas2img_database_h
#define bas2img_database_h
#include "globals.h"


/**
 * Returns information of the font that match with the provided name
 * @param name  The name of the searched font
 * @returns     A 'Font' structure with the information or NULL if the font cannot be found
 */
const Font * getFont(const utf8 *name);

/**
 * Returns information of the computer that match with the provided name
 * @param name  The name of the searched computer
 * @returns     A 'Computer' structure with the information or NULL if the computer cannot be found
 */
const Computer * getComputer(const utf8 *name);


/**
 * Prints the list of available file decoders to stdout
 * @param printAll  If it is 'FALSE' then only main decoders will be printed
 */
void printAvailableDecoders(Bool printAll);

/**
 * Prints the list of available fonts to stdout
 * @param printAll  If it is 'FALSE' then only main fonts will be printed
 */
void printAvailableFonts(Bool printAll);

/**
 * Prints the list of available computers to stdout
 * @param printAll  If it is 'FALSE' then only main computers will be printed
 */
void printAvailableComputers(Bool printAll);



#endif /* bas2img_database_h */
