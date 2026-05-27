/*
 * pagedir.c - implements a module that handles the pagesaver which writes a oage to the pageDirectory and
 * marking it as a Crawler-produced pageDirectory. Encapsulates how to initialze and validate a pageDirectory,
 * and how to read/write page files
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "pagedir.h"
#include "../libcs50/webpage.h"

bool pagedir_init(const char* pageDirectory) {
    if (pageDirectory == NULL) {
        return false;
    }
    //Allocating space
    char* path = malloc(strlen(pageDirectory) + strlen("/.crawler") + 1);
    if (path == NULL) {
        fprintf(stderr, "Error: Out of memory in pagedir_init\n");
        return false;
    }

    //Construct the path for crawler file in that directory
    sprintf(path, "%s/.crawler", pageDirectory);

    // Open the file for writing
    FILE* fp = fopen(path, "w");
    free(path); //free the path string after use

    if (fp == NULL) {
        return false;
    }

    fclose(fp);
    return true;
}

void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID) {
    if (page == NULL || pageDirectory == NULL || docID < 1) {
        return;
    }

    //Allocate space for path
    char* path = malloc(strlen(pageDirectory) + 12);
    if (path == NULL) {
        fprintf(stderr, "Error: Out of memory\n");
        return;
    }

    //Construct path using docID
    sprintf(path, "%s/%d", pageDirectory, docID);

    //Open file for writing
    FILE* fp = fopen(path, "w");
    free(path);

    if (fp == NULL) {
        fprintf(stderr, "Error: Could not open file\n");
        return;
    }

    //Printing URL, depth, and contents of webpage
    fprintf(fp, "%s\n", webpage_getURL(page));
    fprintf(fp, "%d\n", webpage_getDepth(page));
    fprintf(fp, "%s\n", webpage_getHTML(page));

    fclose(fp);
}