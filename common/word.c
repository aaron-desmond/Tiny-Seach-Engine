/*
 * word.c - 'word' module
 * 
 * see word.h for more information
 * 
 * Charlotte Crawford, May 2026
 */

#include <stdio.h>
#include <ctype.h>

/**************** word_normalize ****************/
char* word_normalize(char* word) {
    for (int i = 0; word[i]; i++) {
        word[i] = tolower(word[i]);
    }
    return word;
}
