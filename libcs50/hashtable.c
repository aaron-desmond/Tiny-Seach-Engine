/*
 * hashtable.c - 'hashtable' module
 *
 * see hashtable.h for more information
 *
 * Charlotte Crawford, May 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "hash.h"
#include "set.h"
#include "mem.h"

/**************** global types ****************/
typedef struct hashtable {
  set_t** slots;    // array of sets, one for each slot
  int num_slots;
} hashtable_t;

/**************** hashtable_new ****************/
hashtable_t* hashtable_new(const int num_slots) {
  hashtable_t* ht = mem_malloc(sizeof(hashtable_t));
  // if memory allocation failed or num_slots is an invalid number
  if (ht == NULL) {
    return NULL;
  }
  if (num_slots <= 0) {
    mem_free(ht);
    return NULL;
  }

  // allocate memory for slots and check
  ht->slots = mem_malloc(num_slots * sizeof(set_t*));
  if (ht->slots == NULL) {
    mem_free(ht);
    return NULL;
  }

  // initialize each slot to null
  for (int i = 0; i < num_slots; i++) {
    ht->slots[i] = NULL;
  }
  ht->num_slots = num_slots;
  return ht;
}

/**************** hashtable_insert ****************/
bool hashtable_insert(hashtable_t* ht, const char* key, void* item) {
  if (ht == NULL || key == NULL || item == NULL) {
    return false;
  }

  int index = hash_jenkins(key, ht->num_slots);

  if (ht->slots[index] == NULL) {
    // initialize set at hashed index
    ht->slots[index] = set_new();
    if (ht->slots[index] == NULL) {
      return false;
    }
  }
  return set_insert(ht->slots[index], key, item);
}

/**************** hashtable_find ****************/
void* hashtable_find(hashtable_t* ht, const char* key) {
  if (ht != NULL && key != NULL) {
    int index = hash_jenkins(key, ht->num_slots);
    if (ht->slots[index] == NULL) {
      return NULL;
    }
    return set_find(ht->slots[index], key);
  }
  return NULL;
}

/**************** hashtable_print ****************/
void hashtable_print(hashtable_t* ht, FILE* fp, void (*itemprint)(FILE* fp, const char* key, void* item)) {
  if (fp != NULL) {
    if (ht != NULL) {
      fputs("{\n", fp);
      for (int i = 0; i < ht->num_slots; i++) {
        set_print(ht->slots[i], fp, itemprint);
        fprintf(fp, "\n");
      }
      fputc('}', fp);
    }
    else {
      fputs("(null)", fp);
    }
  }
}

/**************** hashtable_iterate ****************/
void hashtable_iterate(hashtable_t* ht, void* arg, void (*itemfunc)(void* arg, const char* key, void* item)) {
  if (ht != NULL && itemfunc != NULL) {
    for (int i = 0; i < ht->num_slots; i++) {
      set_iterate(ht->slots[i], arg, itemfunc);
    }
  }
}

/**************** hashtable_delete ****************/
void hashtable_delete(hashtable_t* ht, void (*itemdelete)(void* item) ) {
  if (ht != NULL) {
    for (int i = 0; i < ht->num_slots; i++) {
      set_delete(ht->slots[i], itemdelete);
    }
    mem_free(ht->slots);
    mem_free(ht);
  }
}