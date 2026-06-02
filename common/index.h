/* 
 * index.h - header file for index module
 *
 * An *index* provides the data structure to represent the in-memory index, 
 * and functions to read and write index files. It is implemented as a 
 * hashtable that hashes words to an index that contains a set of words as 
 * keys and counters as items that contain document IDs as keys and the 
 * number of occurrences of the word in that document as the item.
 * 
 * Charlotte Crawford, May 2026
 */

#ifndef __INDEX_H
#define __INDEX_H

#include <stdio.h>
#include <stdbool.h>
#include "../libcs50/counters.h"

/**************** global types ****************/
typedef struct index index_t;  // opaque to users of the module

/**************** functions ****************/

/**************** index_new ****************/
/* Create a new (empty) index.
 *
 * Caller provides:
 *   number of slots to be used for the index for hashing (must be > 0).
 * We return:
 *   pointer to the new index; return NULL if error.
 * We guarantee:
 *   index is initialized empty.
 * Caller is responsible for:
 *   later calling index_delete.
 */
index_t* index_new(const int num_slots);

/**************** index_add ****************/
/* Insert item, identified by key (word), into the given index object.
 *
 * Caller provides:
 *   valid pointer to index, valid string for word, valid int for docID.
 * We return:
 *   false if index or word is NULL, docID <= 0, or any counters or set functions failed
 *   true iff new item was inserted.
 * Notes:
 *   The key string is copied for use by the index; that is, the module
 *   is responsible for allocating memory for a copy of the word string, and
 *   later deallocating that memory; thus, the caller is free to re-use or
 *   deallocate its word string after this call.
 */
bool index_add(index_t* index, const char* word, int docID);

/**************** index_find ****************/
/* Return the count associated with the given word and docID.
 *
 * Caller provides:
 *   valid pointer to hashtable, valid string for word, valid int for docID.
 * We return:
 *   the count corresponding to the given word and docID, if found;
 *   0 if index is NULL, word is NULL, word is not found, or docID is not found in the word counter set.
 * Notes:
 *   the index is unchanged by this operation.
 */
int index_find(index_t* index, const char* word, int docID);

/**************** index_get ****************/
/* Return the counter set associated with the given word.
 *
 * Caller provides:
 *   valid index pointer and valid word string.
 * We return:
 *   a pointer to the word's counter set (mapping docID->count), if found;
 *   NULL if index is NULL, word is NULL, or the word is not in the index.
 * Notes:
 *   the returned counter set is owned by the index; the caller must NOT
 *   modify or delete it. The index is unchanged by this operation.
 */
counters_t* index_get(index_t* index, const char* word);

/**************** index_delete ****************/
/* Delete index, calling a delete function on each item (in this case counter sets).
 *
 * Caller provides:
 *   valid index pointer.
 * We do:
 *   if index==NULL, do nothing.
 *   otherwise, call counters_delete on each counter set.
 *   free all the key strings, and the index itself.
 * Notes:
 *   We free the strings that represent key for each item, because
 *   this module allocated that memory in index_add.
 */
void index_delete(index_t* index);

/**************** index_write ****************/
/* Writes the index to a file, one word per line, in the format:
 *   word docID count docID count ...
 * 
 * Caller provides:
 *   valid index pointer, string representing the filename of where to write
 *   the index to.
 * We do:
 *   nothing if index==NULL, indexFilename==NULL, or the file cannot be opened for writing
 *   otherwise, iterate over each word in the index and write each word
 *   followed by all (docID, count) pairs to a single line
 * Notes:
 *   The output can be read and turned back into an index object by index_read.
 */
void index_write(index_t* index, char* indexFilename);

/**************** index_read ****************/
/* Read an index from a file created by index_write and return a new index_t
 * object containing its contents.
 *
 * Caller provides:
 *   valid file path to the index file
 * We return:
 *   return NULL if file cannot be opened 
 *   otherwise, return a new index based on the contents of the file with the 
 *   number of slots corresponding to the number of lines in the file
 * Notes:
 *   We assume that the file path provided leads to a file constructed using the 
 *   index_write function.
 *   The caller is responsible for later calling index_delete on the
 *   returned index.
 */
index_t* index_read(char* indexFilename); 

#endif // __INDEX_H
