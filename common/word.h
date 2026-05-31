/* 
 * word.h - header file for word module
 *
 * The *word* module contains functions to perform on string words.
 */

#ifndef __WORD_H
#define __WORD_H

#include <stdio.h>
#include <ctype.h>

/**************** functions ****************/

/**************** word_normalize ****************/
/* Normalizes a word to all lowercase
 * 
 * Caller provides:
 *   a string word to be normalized
 * We return:
 *   a string representing the inputted word in all lowercase
 */
char* word_normalize(char* word);

#endif // __WORD_H
