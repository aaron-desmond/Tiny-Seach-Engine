# CS50 TSE Indexer
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

We use three data structures: a ‘hashtable’ that hashes words to an index that contains a ‘set’ of words as keys and ‘counters’ as items that contain document IDs as keys and the number of occurrences of the word in that document as the item. All of these together work to form the index module. 

## Control flow

The Indexer is implemented in one file `indexer.c`, with three functions.

### main

The `main` function parses and validates arguments and calls `indexBuild` then exits 0 if successful

### indexBuild

The `indexBuild` function writes a file to represent the index of words and their corresponding document occurrences. It iterates through the pageDirectory provided by crawler.c and calls `indexPage` for each file to update the index data structure for the file and writes the index once all webpages have been examined.

    Create new index
    Initialize docID to 1
    While document file for docID in pageDirectory exists
    	Load that webpage
    	If webpage load successful
    		indexPage that webpage
    		Delete that webpage
    	Increment docID
    Return the index

### indexPage

The `indexPage` function, given a webpage document, extracts its keywords (normalizing them if necessary) and creates/updates their counters in the index.

    Initialize word position to 0
    While there is another word in webpage
    	Get the next word
    	If word length >= 3
    		Normalize word to lower case
    		Search for word in index
    		If word not in index
    			Add word to index
    		Increment the count of the word and docID
    	Free the word

## Other modules

### pagedir

We use the module `pagedir` as described and pseudocoded in the `crawler` implementation spec.

### index

The `index` module will provide the data structure to represent the in-memory index, and functions to read and write index files. Index will be implemented as a hashtable containing sets of counters as described in data structures section.

index_new(): create and initialize a new index data structure

    Allocate memory for new index
    Create a hashtable to store words and their counters
    Store hashtable in index
    Return the index

index_add(): add a word occurrence for a given docID to the index

    Search for word in index
    If word not found
    	Create a new counters structure
    	Insert word and this counters into index
    Increment counter for this docID

index_find(): find the counters associated with word

    Search for word in index
    If word found
    	Return counters of word
    Else
    	Return NULL

index_delete(): delete the index, freeing all associated memory

    For each word in index
    	Delete its counters
    	Free the word
    Delete the hashtable
    Free the index

index_read(): read an index from a file and construct it into memory

    Open index file
    Create new index
    While there is another line in file
    	Read the word from line
    	While there are more docID/count pairs on line
    		Read the docID and count
    		Add the count to the index for that word and docID
    Close file
    Return index

index_write(): write the index to a file

    For each word in index
    	Write word to file
    	For each docID/count pair for that word
    		Write the docID and count to file
    	Write a newline

### word

The `word` module will provide a function to normalize a word by making it lowercase.

    For each char in word
	    Convert char to lower case
    Return normalized word

### libcs50

We leverage the modules of libcs50, most notably, `hashtable`, `set`, `counter`, and `webpage`.
See that directory for module interfaces.

## Function prototypes

### indexer

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `indexer.c` and is not repeated here.

```c
int main(const int argc, char* argv[]);
void indexBuild(index_t* index, (char* pageDirectory)
void indexPage(index_t* index, webpage_t* webpage, int docID);
```

### pagedir

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `pagedir.h` and is not repeated here.

```c
bool pagedir_init(const char* pageDirectory);
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);
```

### word

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `word.h` and is not repeated here.

```c
char* word_normalize(char* word);
```

### index

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `index.h` and is not repeated here.

```c
typedef struct index {} index_t;
index_t* index_new();
void index_add(index_t* index, char* word, int docID);
counters_t* index_find(index_t* index, char* word);
void index_delete(index_t* index);
index_t* index_read(char* indexFilename); 
void index_write(index_t* index);

```

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

Out-of-memory errors are handled by variants of the mem_assert functions, which result in a message printed to stderr and a non-zero exit status. We anticipate out-of-memory errors to be rare and thus allow the program to crash (cleanly) in this way.

All code uses defensive-programming tactics to catch and exit (using variants of the mem_assert functions), e.g., if a function receives bad parameters.

That said, certain errors are caught and handled internally: for example, pagedir_init returns false if there is any trouble creating the .indexer file, allowing the Indexer to decide what to do; the webpage module returns false when URLs are not retrievable, and the Indexer does not treat that as a fatal error.

## Testing plan

Here is an implementation-specific testing plan.

### Unit testing

Create a testing program that tests the `index` module and all its functions. Since word is a smaller module, brief testing.

### Integration/system testing

To test the indexer module as a whole,the indextest program will invoke the indexer with a variety of command-line arguments. This will include missing arguments, too many arguments, non-existent directories, and inaccessible destination files which are possible mistakes that can be made. This will also run the program through Valgrind. 
