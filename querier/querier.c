/*
 * querier.c - TSE Querier implementation
 *
 * Reads an index file and a crawler page directory, then answers ranked
 * search queries read from stdin one per line until EOF.
 *
 * Usage: querier pageDirectory indexFilename
 *
 * Aaron Desmond, May 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "../libcs50/counters.h"
#include "../libcs50/mem.h"
#include "../libcs50/file.h"
#include "../common/index.h"
#include "../common/pagedir.h"

/**************** local types ****************/

/* Holds two counters pointers for iterator callbacks. */
typedef struct {
  counters_t* result;
  counters_t* other;
} twoCtrs_t;

/* Holds the current maximum (key, count) found during iterate. */
typedef struct {
  int maxKey;
  int maxCount;
} maxCtr_t;

/**************** function prototypes ****************/

/* fileno() is provided by stdio but not declared by stdio.h */
int fileno(FILE* stream);

static void parseArgs(int argc, char* argv[],
                      char** pageDirectory, char** indexFilename);
static void processQuery(char* line, index_t* index, char* pageDirectory);
static char** tokenizeQuery(char* line, int* numWords);
static bool validateSyntax(char** words, int numWords);
static counters_t* evaluateQuery(char** words, int numWords, index_t* index);
static counters_t* andSequences(counters_t* result, counters_t* other);
static counters_t* orSequences(counters_t* result, counters_t* other);
static void rankAndPrint(counters_t* scores, char* pageDirectory);
static void printResult(int docID, int score, char* pageDirectory);
static counters_t* counters_copy(counters_t* ctrs);
static void prompt(void);

/* iterator callbacks */
static void andHelper(void* arg, const int key, const int count);
static void orHelper(void* arg, const int key, const int count);
static void maxHelper(void* arg, const int key, const int count);
static void countHelper(void* arg, const int key, const int count);
static void copyHelper(void* arg, const int key, const int count);

/**************** main ****************/
int
main(int argc, char* argv[])
{
  char* pageDirectory = NULL;
  char* indexFilename = NULL;
  parseArgs(argc, argv, &pageDirectory, &indexFilename);

  index_t* index = index_read(indexFilename);
  if (index == NULL) {
    fprintf(stderr, "querier: cannot load index from '%s'\n", indexFilename);
    exit(2);
  }

  char* line;
  prompt();
  while ((line = file_readLine(stdin)) != NULL) {
    processQuery(line, index, pageDirectory);
    mem_free(line);
    prompt();
  }

  index_delete(index);
  return 0;
}

/**************** parseArgs ****************/
/* Validate command-line arguments; exits non-zero with a stderr message
 * on any error.
 */
static void
parseArgs(int argc, char* argv[],
          char** pageDirectory, char** indexFilename)
{
  if (argc != 3) {
    fprintf(stderr, "usage: querier pageDirectory indexFilename\n");
    exit(1);
  }

  *pageDirectory = argv[1];
  *indexFilename = argv[2];

  if (!pagedir_validate(*pageDirectory)) {
    fprintf(stderr, "querier: '%s' is not a valid crawler directory\n",
            *pageDirectory);
    exit(1);
  }

  // verify pageDirectory/1 is readable (per requirements spec)
  int pathlen = strlen(*pageDirectory) + 10;
  char* path = mem_malloc_assert(pathlen, "parseArgs: path");
  snprintf(path, pathlen, "%s/1", *pageDirectory);
  FILE* fp = fopen(path, "r");
  mem_free(path);
  if (fp == NULL) {
    fprintf(stderr, "querier: cannot read '%s/1'\n", *pageDirectory);
    exit(1);
  }
  fclose(fp);

  // verify index file is readable
  fp = fopen(*indexFilename, "r");
  if (fp == NULL) {
    fprintf(stderr, "querier: cannot read index file '%s'\n", *indexFilename);
    exit(1);
  }
  fclose(fp);
}

/**************** prompt ****************/
/* Print "Query? " only when stdin is an interactive terminal. */
static void
prompt(void)
{
  if (isatty(fileno(stdin))) {
    printf("Query? ");
    fflush(stdout);
  }
}

/**************** tokenizeQuery ****************/
/* Convert line to lowercase in-place, split on whitespace into a malloc'd
 * array of char* pointers into line's own buffer. Validates that every
 * character is alphabetic or whitespace; prints an error and returns NULL
 * on any bad character. Sets *numWords to the word count.
 * Caller must free the returned array (but NOT the individual strings,
 * which point into line's buffer and are freed with line by the caller).
 */
static char**
tokenizeQuery(char* line, int* numWords)
{
  *numWords = 0;

  // validate: only alpha and space allowed
  for (char* p = line; *p != '\0'; p++) {
    if (!isalpha((unsigned char)*p) && !isspace((unsigned char)*p)) {
      fprintf(stderr, "Error: bad character '%c' in query.\n", *p);
      return NULL;
    }
  }

  // first pass: count words
  int count = 0;
  bool inWord = false;
  for (char* p = line; *p != '\0'; p++) {
    if (isalpha((unsigned char)*p)) {
      if (!inWord) {
        count++;
        inWord = true;
      }
    } else {
      inWord = false;
    }
  }

  char** words = mem_malloc_assert((count + 1) * sizeof(char*),
                                   "tokenizeQuery: words array");

  // second pass: lowercase in-place, null-terminate, record pointers
  int idx = 0;
  inWord = false;
  for (char* p = line; *p != '\0'; p++) {
    if (isalpha((unsigned char)*p)) {
      *p = tolower((unsigned char)*p);
      if (!inWord) {
        words[idx++] = p;
        inWord = true;
      }
    } else {
      if (inWord) {
        *p = '\0';    // null-terminate this word inside line's buffer
        inWord = false;
      }
    }
  }

  *numWords = count;
  return words;
}

/**************** validateSyntax ****************/
/* Check that the word array has valid query syntax:
 *   - 'and'/'or' cannot be first or last
 *   - two operators cannot be adjacent
 * Prints an error and returns false on any violation.
 */
static bool
validateSyntax(char** words, int numWords)
{
  // first word cannot be an operator
  if (strcmp(words[0], "and") == 0 || strcmp(words[0], "or") == 0) {
    fprintf(stderr, "Error: '%s' cannot be first\n", words[0]);
    return false;
  }

  // last word cannot be an operator
  if (strcmp(words[numWords - 1], "and") == 0 ||
      strcmp(words[numWords - 1], "or") == 0) {
    fprintf(stderr, "Error: '%s' cannot be last\n", words[numWords - 1]);
    return false;
  }

  // no two adjacent operators
  for (int i = 0; i < numWords - 1; i++) {
    bool currOp = (strcmp(words[i], "and") == 0 ||
                   strcmp(words[i], "or") == 0);
    bool nextOp = (strcmp(words[i + 1], "and") == 0 ||
                   strcmp(words[i + 1], "or") == 0);
    if (currOp && nextOp) {
      fprintf(stderr, "Error: '%s' and '%s' cannot be adjacent\n",
              words[i], words[i + 1]);
      return false;
    }
  }

  return true;
}

/**************** processQuery ****************/
/* Top-level handler for one query line: tokenize, print clean query,
 * validate syntax, evaluate, rank and print results.
 * line is freed by the caller; this function frees intermediate data.
 */
static void
processQuery(char* line, index_t* index, char* pageDirectory)
{
  int numWords = 0;
  char** words = tokenizeQuery(line, &numWords);
  if (words == NULL) return;   // bad character — error already printed

  if (numWords == 0) {
    mem_free(words);
    return;                    // blank line: skip silently
  }

  // print clean query (lowercased, single-spaced)
  printf("Query:");
  for (int i = 0; i < numWords; i++) {
    printf(" %s", words[i]);
  }
  printf("\n");

  if (!validateSyntax(words, numWords)) {
    mem_free(words);
    return;
  }

  counters_t* result = evaluateQuery(words, numWords, index);
  mem_free(words);

  rankAndPrint(result, pageDirectory);
  counters_delete(result);
}

/**************** iterator helpers ****************/

/* andHelper: for each (key, count) in result, set result[key] = min(count, other[key]).
 * A docID absent from other gets count 0, effectively removing it from the AND result. */
static void
andHelper(void* arg, const int key, const int count)
{
  twoCtrs_t* two = arg;
  int otherCount = counters_get(two->other, key);
  int minCount = (count < otherCount) ? count : otherCount;
  counters_set(two->result, key, minCount);
}

/* orHelper: for each (key, count) in other, add count into result[key]. */
static void
orHelper(void* arg, const int key, const int count)
{
  counters_t* result = arg;
  int existing = counters_get(result, key);
  counters_set(result, key, existing + count);
}

/* maxHelper: track the (key, count) pair with the highest count seen. */
static void
maxHelper(void* arg, const int key, const int count)
{
  maxCtr_t* max = arg;
  if (count > max->maxCount) {
    max->maxCount = count;
    max->maxKey = key;
  }
}

/* countHelper: count entries with count > 0. */
static void
countHelper(void* arg, const int key, const int count)
{
  if (count > 0) {
    (*(int*)arg)++;
  }
}

/* copyHelper: copy one (key, count) pair into arg (a counters_t*). */
static void
copyHelper(void* arg, const int key, const int count)
{
  counters_set((counters_t*)arg, key, count);
}

/* counters_copy: return a new counters_t with the same contents as ctrs.
 * Caller is responsible for calling counters_delete on the returned object. */
static counters_t*
counters_copy(counters_t* ctrs)
{
  counters_t* copy = counters_new();
  if (copy != NULL) {
    counters_iterate(ctrs, copy, copyHelper);
  }
  return copy;
}

/**************** andSequences ****************/
/* Compute the AND (intersection, min-score) of result and other.
 * Modifies result in-place by setting each docID's count to min(result, other).
 * DocIDs present in result but absent from other get count 0 (excluded).
 * Does NOT free or modify other (owned by the index).
 * Returns result.
 */
static counters_t*
andSequences(counters_t* result, counters_t* other)
{
  if (result == NULL || other == NULL) return result;
  twoCtrs_t two = {result, other};
  counters_iterate(result, &two, andHelper);
  return result;
}

/**************** orSequences ****************/
/* Compute the OR (union, summed scores) of result and other.
 * If result is NULL, returns other directly (caller now owns other).
 * Otherwise merges other's counts into result, frees other, and returns result.
 */
static counters_t*
orSequences(counters_t* result, counters_t* other)
{
  if (other == NULL) return result;
  if (result == NULL) return other;
  counters_iterate(other, result, orHelper);
  counters_delete(other);
  return result;
}

/**************** evaluateQuery ****************/
/* Evaluate the parsed query using AND-precedence-over-OR semantics.
 *
 * Maintains two running counters:
 *   andResult — the running AND product for the current andsequence
 *   result    — the running OR total across andsequences
 *
 * When 'or' is encountered, merge andResult into result and reset.
 * After the loop, merge the final andResult into result.
 *
 * Key ownership rules:
 *   wordCtrs — owned by the index, must not be freed or modified
 *   andResult and result — owned here; orSequences() consumes andResult
 *
 * Returns a new counters_t* the caller must delete (may be empty, not NULL).
 */
static counters_t*
evaluateQuery(char** words, int numWords, index_t* index)
{
  if (words == NULL || index == NULL) return counters_new();
  counters_t* result = NULL;      // running OR total
  counters_t* andResult = NULL;   // running AND product for current andsequence

  for (int i = 0; i < numWords; i++) {
    if (strcmp(words[i], "or") == 0) {
      // end of an andsequence: merge it into the OR total
      result = orSequences(result, andResult);
      andResult = NULL;

    } else if (strcmp(words[i], "and") == 0) {
      // explicit 'and' keyword: implicit behavior, nothing to do
      (void)0;

    } else {
      // regular word: look up in index
      counters_t* wordCtrs = index_find(index, words[i]);

      if (wordCtrs == NULL) {
        // word absent → AND with empty set makes andResult empty
        counters_delete(andResult);
        andResult = counters_new();

      } else if (andResult == NULL) {
        // first word in this andsequence: copy (cannot modify index's counters)
        andResult = counters_copy(wordCtrs);

      } else {
        // AND this word into the running product
        andSequences(andResult, wordCtrs);
      }
    }
  }

  // merge the final andsequence into the OR total
  result = orSequences(result, andResult);

  // guarantee a non-NULL return (empty counters instead of NULL)
  if (result == NULL) result = counters_new();
  return result;
}

/**************** rankAndPrint ****************/
/* Rank documents in scores by descending score (selection sort):
 * find the max, print it, zero it out, repeat until no scores remain.
 * Prints "No documents match." if the result set is empty.
 */
static void
rankAndPrint(counters_t* scores, char* pageDirectory)
{
  if (scores == NULL) {
    printf("No documents match.\n");
    return;
  }

  // count documents with score > 0
  int total = 0;
  counters_iterate(scores, &total, countHelper);
  if (total == 0) {
    printf("No documents match.\n");
    return;
  }

  printf("Matches %d document%s (ranked):\n", total, total == 1 ? "" : "s");

  // selection sort: find max, print, zero, repeat
  maxCtr_t max;
  do {
    max.maxKey   = 0;
    max.maxCount = 0;
    counters_iterate(scores, &max, maxHelper);
    if (max.maxCount > 0) {
      printResult(max.maxKey, max.maxCount, pageDirectory);
      counters_set(scores, max.maxKey, 0);
    }
  } while (max.maxCount > 0);
}

/**************** printResult ****************/
/* Print one result line: score, docID, and URL (first line of pageDirectory/docID). */
static void
printResult(int docID, int score, char* pageDirectory)
{
  if (pageDirectory == NULL) return;
  char* url = pagedir_getURL(pageDirectory, docID);
  if (url != NULL) {
    printf("score %3d doc %3d: %s\n", score, docID, url);
    mem_free(url);
  }
}
