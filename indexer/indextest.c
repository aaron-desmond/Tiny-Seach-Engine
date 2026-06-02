/*
 * indextest.c - tests the index module by reading an index file 
 * and writing it back out to a new index file
 * 
 * usage: ./indextest oldIndexFilename newIndexFilename
 * 
 * Charlotte Crawford, May 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include "index.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: ./indextest oldIndexFilename newIndexFilename\n");
        return 1;
    }

    char* oldIndexFilename = argv[1];
    char* newIndexFilename = argv[2];

    index_t* index = index_read(oldIndexFilename);
    if (index == NULL) {
        fprintf(stderr, "indextest: failed to read index from '%s'\n", oldIndexFilename);
        return 1;
    }

    index_write(index, newIndexFilename);
    index_delete(index);
}