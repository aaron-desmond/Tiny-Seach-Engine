/*
 * set.c - 'set' module
 *
 * see set.h for more information
 * 
 * Charlotte Crawford, May 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "set.h"
#include "mem.h"

/**************** local types ****************/
typedef struct setnode {
  void* item;              // pointer to data for the item
  char* key;               // key for the item
  struct setnode *next;    // link to next node
} setnode_t;

/**************** global types ****************/
typedef struct set {
  struct setnode *head;    // head of the set of (key, item)s
} set_t;

/**************** global functions ****************/
/* see set.h */

/**************** local functions ****************/
static setnode_t* setnode_new(const char* key, void* item);

/**************** set_new() ****************/
set_t* set_new(void) {
  set_t* set = mem_malloc(sizeof(set_t));

  // check for allocation error
  if (set == NULL) {
    return NULL;
  }
  else {
    set->head = NULL;
    return set;
  }
}

/**************** set_insert() ****************/
bool set_insert(set_t* set, const char* key, void* item) {
  // checks if any parameter is null and if key is already in set
  if (set != NULL && key != NULL && item != NULL && set_find(set, key) == NULL) {
    // allocate new node to add to set
    setnode_t* new = setnode_new(key, item);
    if (new != NULL) {
      new->next = set->head;
      set->head = new;
      return true;
    }
  }
  return false;
}

/**************** setnode_new() ****************/
/* Allocate and initalize a setnode */
static setnode_t* setnode_new(const char* key, void* item) {
  setnode_t* node = mem_malloc(sizeof(setnode_t));

  if (node == NULL) {
    return NULL;
  }

  node->key = malloc(strlen(key) + 1);
  // check if memory was properly allocated
  if (node->key == NULL) {
    mem_free(node);          // free node if key copy fails
    return NULL;
  }
  strcpy(node->key, key);
  node->item = item;
  node->next = NULL;
  return node;
}

/**************** set_find() ****************/
void* set_find(set_t* set, const char* key) {
  // check for bad or empty set
  if (set != NULL && key != NULL && set->head != NULL) {
    setnode_t* curr = set->head;
    while (curr != NULL) {
      if (strcmp(curr->key, key) == 0) {
        return curr->item;
      }
      curr = curr->next;
    }
  }
  return NULL;
}

/**************** set_print() ****************/
void set_print(set_t* set, FILE* fp, void (*itemprint)(FILE* fp, const char* key, void* item) ) {
  if (fp != NULL) {
    if (set != NULL) {
      fputc('{', fp);
      for (setnode_t* node = set->head; node != NULL; node = node->next) {
        if (itemprint != NULL) {
          (*itemprint)(fp, node->key, node->item);
          fputc(',', fp);
        }
      }
      fputc('}', fp);
    }
    else {
      fputs("(null)", fp);
    }
  }
}

/**************** set_iterate() ****************/
void set_iterate(set_t* set, void* arg, void (*itemfunc)(void* arg, const char* key, void* item) ) {
  if (set != NULL && itemfunc != NULL) {
    for (setnode_t* node = set->head; node != NULL; node = node->next) {
      (*itemfunc)(arg, node->key, node->item);
    }
  }
}

/**************** set_delete() ****************/
void set_delete(set_t* set, void (*itemdelete)(void* item) ) {
  if (set != NULL) {
    for (setnode_t* node = set->head; node != NULL; ) {
      if (itemdelete != NULL) {
        (*itemdelete)(node->item);
      }
      free(node->key);
      setnode_t* next = node->next;
      mem_free(node);
      node = next;
    }
    mem_free(set);
  }
}
