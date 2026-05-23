# CS50 TSE Querier
## Implementation Spec

In this document we reference the Requirements Specification and Design Specification and focus on the implementation-specific decisions.
The knowledge unit noted that an [implementation spec](https://github.com/CS50DartmouthFA2025/home/blob/main/knowledge/units/design.md#implementation-spec) may include many topics; not all are relevant to the TSE or the Indexer.
Here we focus on the core subset:

-  Data structures
-  Control flow: pseudo code for overall flow, and for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan

## Data structures 

We use the `index` module which implements a `hashtable` of `sets` of `counter` items as described in the `indexer` implementation spec. 

We also use the `counter` module for query results. 
- AND Operation: iterate both counters, keep shared DocIDs, take minimum of counts
- OR Operation: merge both counters, sum counts

An array of (docID, score) structs is used for ranking and is sorted by descending to return the maximum score.

## Control flow

### main

The `main` function parses and validates arguments, reads the indexFilename created by the indexer and creates an `index` object, loops to prompt and read stdin, and calls `processQuery` on each query/line until EOF.

### processQuery

The `processQuery` function cleans the syntax, validates the syntax, calls `evaluateQuery`, and ranks and prints the result.

### evaluateQuery

The `evaluateQuery` function loops over each word in the query, calls the proper operator functions ("and" or "or"), and returns a `counter` object with the docID and the score.

## Other modules

### index

The `index` module will provide the data structure to represent the in-memory index, and functions to read and write index files. Index will be implemented as a hashtable containing sets of counters as described in data structures section.

### libcs50

We leverage the modules of libcs50, most notably, `hashtable`, `set`, `counter`, and `webpage`.
See that directory for module interfaces.

## Function prototypes

### querier

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `querier.c` and is not repeated here (simple descriptions added below though, mostly for helper functions, and pseudocode is included in the design file).

```c
int main(int argc, char *argv[]);
static void parseArgs(int argc, *int argv[], char **pageDirectory, char **indexFilename) ~ Validate argument count
void processQuery(char* line, index_t* index, char* pageDirectory) ~ Top-level handler
static char* tokenizeQuery(char* line, int numWords) ~ cleans and splits line into word array, return NULL on bad character
static bool validateSyntax(char* words, int numWords) ~ Checks for operators at start/end, adjacent operators
counters_t* evaluateQuery(char* words, int numWords, index_t* index) ~ AND/OR Loop, returns counter object: DocID, score
static counters_t* andSequences(counters_t* result, counters_t* new) ~ AND 2 counters, intersection at min
static counters_t* orSequences(counters_t* result, counters_t* new) ~ OR 2 counters, union with sum
static void rankAndPrint(counters_t* scores, char* pageDirectory) ~ Copies counters into sortable array, sorts desc, prints
static void printResult(docID, score, pageDirectory) ~ Prints score, DocID, and URL for result
```

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

Out-of-memory errors are handled by variants of the mem_assert functions, which result in a message printed to stderr and a non-zero exit status. We anticipate out-of-memory errors to be rare and thus allow the program to crash (cleanly) in this way.

All code uses defensive-programming tactics to catch and exit (using variants of the mem_assert functions), e.g., if a function receives bad parameters.

## Testing plan

### Integration/System testing
To test the querier module as a whole, the queriertest program will present queries and direct the querier to act from the shared directory’s crawler and indexer as well as invalid inputs. This will include missing arguments, too many arguments, non-existent directories, and inaccessible destination files which are possible mistakes that can be made. This will also run the program through Valgrind. 

