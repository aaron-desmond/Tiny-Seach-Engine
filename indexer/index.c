/*
 * index.c - 'index' module
 * 
 * see index.h for more information 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"
#include "hash.h"
#include "set.h"
#include "counters.h"
#include "mem.h"

/**************** global types ****************/
typedef struct index {
    set_t** slots;
    int num_slots;
} index_t;


/**************** global functions ****************/
/* see index.h */

/**************** index_new ****************/
index_t* index_new() {
    index_t* index = mem_malloc(sizeof(index_t));

    if (index == NULL) {
        return NULL;
    }
    if (num_slots <= 0) {
        mem_free(index);
        return NULL;
    }

    index->slots = mem_malloc(num_slots * sizeof(set_t*));
    if (index->slots == NULL) {
        mem_free(index);
        return NULL;
    }
    for (int i = 0; i < num_slots; i++) {        
        index->slots[i] = NULL;
    }
    index->num_slots = num_slots;
    return index;
}

/**************** index_add ****************/
bool index_add(index_t* index, const char* word, int docID) {
    if (index == NULL || word == NULL || docID <= 0) {
        return false;
    }

    int hash_index = hash_jenkins(key, index->num_slots);

    if (index->slots[hash_index] == NULL) {
        // initialize set at hashed index
        index->slots[hash_index] = set_new();
        if (index->slots[hash_index] == NULL) {
            return false;
        }
        counters_t* word_ctr = counters_new();
        if (word_ctr == NULL) {
            return false;
        }
        int ctr_add = counters_add(word_ctr, docID);
        if (ctr_add == 0) { return false; }
        
        // add word as key and pointer to counter as item
        return set_insert(index->slots[hash_index], word, word_ctr);
    }
    else {
        counters_t* ctr = set_find(index->slots[hash_index], word);
        // there is a set but the word has not been added yet
        if (ctr == NULL) {
            counters_t* word_ctr = counters_new();
            if (word_ctr == NULL) {
                return false;
            }
            int ctr_add = counters_add(word_ctr, docID);
            if (ctr_add == 0) { return false; }
            return set_add(index->slots[hash_index], word, word_ctr);
        }
        // there is a set and the word is already in the set
        else {
            int ctr_add = counters_add(ctr, docID);
            if (ctr_add == 0) { return false; }
            return true;
        }
    }
}

/**************** index_find ****************/
int index_find(index_t* index, char* word, int docID) {
    if (index != NULL && word != NULL) {
        int hash_index = hash_jenkins(key, index->num_slots);
        if (index->slots[hash_index] == NULL) {
            return 0;
        }
        counters_t* ctrs = set_find(index->slots[hash_index], key);
        return counters_get(ctrs, docID);
    }
    return 0;
}

/**************** index_delete ****************/
void index_delete(index_t* index) {
    if (index != NULL) {
        for (int i = 0; i < index->num_slots; i++) {
            set_delete(index->slots[i], counters_delete);
        }
        mem_free(index->slots);
        mem_free(index);
    }
}

/**************** index_read ****************/
index_t* index_read(char* indexFilename); 

/**************** index_write ****************/
void index_write(index_t* index, FILE* indexFile);