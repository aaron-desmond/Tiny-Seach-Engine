/*
 * index.c - 'index' module
 * 
 * see index.h for more information 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"
#include "hashtable.h"
#include "counters.h"
#include "mem.h"

/**************** global types ****************/
typedef struct index {
    hashtable_t* ht;
} index_t;

/**************** local functions ****************/
static void write_counters(void* arg, const int docID, const int count);
static void write_word(void* arg, const char* word, void* item);

/**************** global functions ****************/
/* see index.h */

/**************** index_new ****************/
index_t* index_new(const int num_slots) {
    index_t* index = mem_malloc(sizeof(index_t));
    if (index == NULL) {
        return NULL;
    }
    index->ht = hashtable_new(num_slots);
    if (index->ht == NULL) {
        mem_free(index);
        return NULL;
    }
    return index;
}

/**************** index_add ****************/
bool index_add(index_t* index, const char* word, int docID) {
    if (index == NULL || word == NULL || docID <= 0) {
        return false;
    }

    counters_t* ctr = hashtable_find(index->ht, word);

    if (ctr == NULL) {
        // word not in index yet, create new counterset and insert
        counters_t* word_ctr = counters_new();
        if (word_ctr == NULL) {
            return false;
        }
        int ctr_add = counters_add(word_ctr, docID);
        if (ctr_add == 0) { return false; }
        
        // add word as key and pointer to counter as item
        return hashtable_insert(index->ht, word, word_ctr);
    }
    else {
        // word is already in index, increment its counter
        int ctr_add = counters_add(ctr, docID);
        if (ctr_add == 0) { return false; }
        return true;
    }
}

/**************** index_find ****************/
int index_find(index_t* index, char* word, int docID) {
    if (index != NULL && word != NULL) {
        counters_t* ctr = hashtable_find(index->ht, word);
        if (ctr == NULL) {
            return 0;
        }
        return counters_get(ctrs, docID);
    }
    return 0;
}

/**************** index_delete ****************/
void index_delete(index_t* index) {
    if (index != NULL) {
        hashtable_delete(index->ht, counters_delete);
        mem_free(index);
    }
}

/**************** index_write ****************/
void index_write(index_t* index, char* indexFilename) {
    FILE* fp;
    if (index != NULL && indexFile != NULL && (fp = fopen(indexFilename, "w")) != NULL) {
        hashtable_iterate(index->ht, fp, write_word);
        fclose(fp);
    }
}

/**************** write_counters ***************
 * itemfunc to be passed in to counters_iterate
 * Prints each counter node to the file in the form 
 * "docID count " on the same line
 */
static void write_counters(void* arg, const int docID, const int count) {
    FILE* fp = arg;
    fprintf(fp, "%d %d ", docID, count);
}

/**************** write_word ***************
 * itemfunc to be passed into hashtable_iterate, and 
 * therefore set_iterate
 * Prints one word per line to the file and calls 
 * write_counters to print each word's counterset on the
 * same line as the word
 */
static void write_word(void* arg, const char* word, void* item) {
    FILE* fp = arg;
    fprintf(fp, "%s", word);
    counters_iterate((counters_t*)item, fp, write_counters);
    fprintf(fp, "\n");
}

/**************** index_read ****************/
index_t* index_read(char* indexFilename) {
    FILE* fp  = fopen(indexFilename, "r");
    if (fp == NULL) return NULL;

    int num_slots = file_numLines(fp);

    index_t* index = index_new(num_slots);
    if (index == NULL) {
        fclose(fp);
        return NULL;
    }

    char word[100];
    while (fscanf(fp, "%s", word) == 1) {
        int docID, count;
        while (fscanf(fp, "%d %d ", &docID, &count) == 2) {
            counters_t* ctr = hashtable_find(index->ht, word);
            if (ctr == NULL) {
                ctr = counters_new();
                hashtable_insert(index->ht, word, ctr);
            }
            counters_set(ctr, docID, count);
        }
    }
    fclose(fp);
    return index;
}