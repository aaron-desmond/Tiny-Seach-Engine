/*
 * counters.c - 'counters' module
 *
 * see counters.h for more information
 * 
 * Charlotte Crawford, May 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "counters.h"
#include "mem.h"

/**************** local types ****************/
typedef struct ctrnode {
  int key;
  int count;
  struct ctrnode *next;
} ctrnode_t;

/**************** global types ****************/
typedef struct counters {
  struct ctrnode *head;
} counters_t;

/**************** global functions ****************/
/* see counters.h */

/**************** local functions ****************/
static ctrnode_t* ctrnode_new(const int key);

/**************** counters_new ****************/
counters_t* counters_new(void) {
  counters_t* ctrs = mem_malloc(sizeof(counters_t));

  if (ctrs == NULL) {
    // error allocating memeory for counters
    return NULL;
  }
  else {
    ctrs->head = NULL;
    return ctrs;
  }
}

/**************** counters_add ****************/
int counters_add(counters_t* ctrs, const int key) {
  if (ctrs == NULL || key < 0) {
    return 0;
  }

  int count = counters_get(ctrs, key);
  if (counters_set(ctrs, key, count + 1)) {
    return count + 1;
  }
  else {
    return 0;
  }
}

/**************** ctrnode_new ****************/
ctrnode_t* ctrnode_new(const int key) {
  ctrnode_t* node = mem_malloc(sizeof(ctrnode_t));

  if (node == NULL) {
    return NULL;
  }
  else {
    node->key = key;
    node->count = 1;
    node->next = NULL;
    return node;
  }
}

/**************** counters_get ****************/
int counters_get(counters_t* ctrs, const int key) {
  if (ctrs != NULL && key >= 0) {
    ctrnode_t* curr = ctrs->head;
    while (curr != NULL) {
      if (curr->key == key) {
        return curr->count;
      }
      curr = curr->next;
    }
  }
  return 0;
}

/**************** counters_set ****************/
bool counters_set(counters_t* ctrs, const int key, const int count) {
  if (ctrs != NULL && key >= 0 && count >= 0) {
    ctrnode_t* curr = ctrs->head;
    while (curr != NULL) {
      if (curr->key == key) {
        curr->count = count;
        return true;
      }
      curr = curr->next;
    }

    // make a new counter if key couldn't be found in ctrs
    ctrnode_t* new = ctrnode_new(key);

    // check if memory allocation failed
    if (new == NULL) {
      return false;
    }
    new->count = count;
    new->next = ctrs->head;
    ctrs->head = new;
    return true;
  }
  return false;
}

/**************** counters_print ****************/
void counters_print(counters_t* ctrs, FILE* fp) {
  if (fp != NULL) {
    if (ctrs != NULL) {
      fputc('{', fp);
      for (ctrnode_t* node = ctrs->head; node != NULL; node = node->next) {
        fprintf(fp, "%d=%d", node->key, node->count);
        fputc(',', fp);
      }
      fputc('}', fp);
    }
    else {
      fputs("(null)", fp);
    }
  }
}

/**************** counters_iterate ****************/
void counters_iterate(counters_t* ctrs, void* arg, void (*itemfunc)(void* arg, const int key, const int count)) {
  if (ctrs != NULL && itemfunc != NULL) {
    for (ctrnode_t* node = ctrs->head; node != NULL; node = node->next) {
      (*itemfunc)(arg, node->key, node->count);
    }
  }
}


/**************** counters_delete ****************/
void counters_delete(counters_t* ctrs) {
  if (ctrs != NULL) {
    for (ctrnode_t* node = ctrs->head; node != NULL; ) {
      ctrnode_t* next = node->next;
      mem_free(node);
      node = next;
    }
    mem_free(ctrs);
  }
}
