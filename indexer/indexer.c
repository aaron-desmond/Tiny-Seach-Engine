/*
 * Final Project: indexer.c 
 *
 * CS50, Dartmouth College
 *
 * Lila Hwang, May 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/index.h"
#include "../common/word.h"
#include "../common/pagedir.h"
#include "../libcs50/webpage.h"
#include "../libcs50/file.h"
#include "../libcs50/mem.h"

/* Function Prototypes */

static void parseArgs(int argc, char* argv[], char** pageDirectory, char** indexFilename);
static index_t* indexBuild(char* pageDirectory);
static void indexPage(index_t* index, webpage_t* webpage, int docID);

/*
 * call helper functions to validate arguments and create the index
 * access the index module to write and subsequently delete the index
 */
int main(const int argc, char* argv[]) {
  char* pageDirectory;
  char* indexFilename;

  parseArgs(argc, argv, &pageDirectory, &indexFilename);

  // generate index from given pageDirectory
  index_t* index = indexBuild(pageDirectory);

  if (index == NULL) {
    fprintf(stderr, "Failed indexBuild\n");
    return 1;
  }

  index_write(index, indexFilename);
  index_delete(index);

  return 0;
}

/* ********************** parseArgs() ************************* */
/*
 * validate program usage and assign arguments to variables
 * program accepts three arguments: compiled call, pageDirectory, indexFilename
 */
static void parseArgs(int argc, char* argv[], char** pageDirectory, char** indexFilename) {
  // validate arg count
  if (argc != 3) {
    fprintf(stderr, "usage: ./indexer pageDirectory indexFilename\n");
    exit(1);
  }
  
  *pageDirectory = argv[1];
  *indexFilename = argv[2];

  // validate crawler directory
  char path[200];
  sprintf(path, "%s/.crawler", *pageDirectory);
  
  FILE* fp = fopen(path, "r");
  if (fp == NULL) {
    fprintf(stderr, "Invalid crawler directory\n");
    exit(1);
  }
  fclose(fp);

  // validate writable index file
  FILE* fp1 = fopen(*indexFilename, "w");
  if (fp1 == NULL) {
    fprintf(stderr, "Unable to write index file\n");
    exit(1);
  }
  fclose(fp1);
}

/* ************************* indexBuild() ************************** */
/*
 * initialize new index and search each page of given pageDirectory to fill it
 * return the completed index once all webpages have been searched
 */
static index_t* indexBuild(char* pageDirectory) {
  index_t* index = index_new(200);

  if (index == NULL) {
    return NULL; // invalid index initialization
  }

  int docID = 1; // begin with first page in directory
  bool finished = false; // condition for while loop
  while (!finished) {
    webpage_t* webpage = pagedir_load(pageDirectory, docID);

    if (webpage == NULL) {
      finished = true; // failure to load webpage
    } else {
      indexPage(index, webpage, docID);
      webpage_delete(webpage);
      docID++;
    }
  }

  return index;
}

/* *********************** indexPage() ************************* */
/*
 * extract keywords from the given webpage and update the given index
 */
static void
indexPage(index_t* index, webpage_t* webpage, int docID) {
  int pos = 0;
  char* word;

  while ((word = webpage_getNextWord(webpage, &pos)) != NULL) {
    if (strlen(word) >= 3) { // 'significant' words only
      word_normalize(word); // convert word to lower case, if necessary
      index_add(index, word, docID); // update the index
    }

    // free the word
    mem_free(word);
  }
}
