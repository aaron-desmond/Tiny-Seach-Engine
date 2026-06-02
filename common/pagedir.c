/*
 * pagedir.c - implements a module that handles the pagesaver which writes a oage to the pageDirectory and
 * marking it as a Crawler-produced pageDirectory. Encapsulates how to initialze and validate a pageDirectory,
 * and how to read/write page files
 *
 * Modified by Lila Hwang, May 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "pagedir.h"
#include "../libcs50/webpage.h"
#include "../libcs50/file.h"
#include "../libcs50/mem.h"

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

bool pagedir_validate(const char* pageDirectory) {
    if (pageDirectory == NULL) {
        return false;
    }
    //Allocating space for the path to the crawler marker file
    char* path = malloc(strlen(pageDirectory) + strlen("/.crawler") + 1);
    if (path == NULL) {
        return false;
    }

    //Construct the path for crawler file in that directory
    sprintf(path, "%s/.crawler", pageDirectory);

    // Open the file for reading (does not create or modify it)
    FILE* fp = fopen(path, "r");
    free(path);

    if (fp == NULL) {
        return false;
    }

    fclose(fp);
    return true;
}

char* pagedir_getURL(const char* pageDirectory, int docID) {
    if (pageDirectory == NULL || docID < 1) {
        return NULL;
    }

    //Allocate space for path
    char* path = malloc(strlen(pageDirectory) + 12);
    if (path == NULL) {
        return NULL;
    }

    //Construct path using docID
    sprintf(path, "%s/%d", pageDirectory, docID);

    //Open file for reading
    FILE* fp = fopen(path, "r");
    free(path);

    if (fp == NULL) {
        return NULL;
    }

    //URL is the first line of the page file
    char* url = file_readLine(fp);
    fclose(fp);

    return url;
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

/* ***************** pagedir_load() ******************** */
/* see pagedir.h for description */
webpage_t*
pagedir_load(const char* pageDirectory, int docID) {
  char path[100];

  sprintf(path, "%s/%d", pageDirectory, docID);

  FILE* fp = fopen(path, "r");

  if (fp == NULL) {
    return NULL;
  }

  char* url = file_readLine(fp);
  char* depthStr = file_readLine(fp);
  int depth = atoi(depthStr);

  char* html = file_readFile(fp);

  free(depthStr);
  fclose(fp);

  webpage_t* webpage = webpage_new(url, depth, html);

  return webpage;
}
