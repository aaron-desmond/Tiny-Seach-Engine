/* 
 * index.h - header file for index module
 *
 * An *index* provides the data structure to represent the in-memory index, 
 * and functions to read and write index files. It is implemented as a 
 * hashtable that hashes words to an index that contains a set of words as 
 * keys and counters as items that contain document IDs as keys and the 
 * number of occurrences of the word in that document as the item.
 */

#ifndef __INDEX_H
#define __INDEX_H

#include <stdio.h>
#include <stdbool.h>

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
/* 
 * 
 * Caller provides:
 * We do:
 * Notes:
 */
void index_write(index_t* index, FILE* indexFile);

/**************** index_read ****************/
/* 
 *
 * Caller provides:
 * We do:
 * Notes:
 */
index_t* index_read(char* indexFilename); 

#endif // __INDEX_H
