# CS50 TSE - Common Library

The `common` directory serves as a shared library module containing code utilized across multiple subsystems of the Tiny Search Engine (TSE), including the Crawler, Indexer, and Querier.

Currently, it implements the:
* `pagedir` module, which handles initializing a page directory and safely saving fetched webpages to a directory.
* `index` module, which implements a hashtable that contains a set of words as keys and counters as items, which contain document IDs as keys and the number of occurrences of the word in that document as the item.
* `word` module which contains a function to normalize words to all lowercase.

## Interface

### pagedir
* `bool pagedir_init(const char* pageDirectory);` - initializes a directory by constructing a path to `.crawler` and verifying write permissions.
* `void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);` - saves a `webpage_t` into a formatted text file named by its `docID` inside the given directory.

### index
* `index_t* index_new(const int num_slots);` - creates a new empty index
* `bool index_add(index_t* index, const char* word, int docID);` - increments the counter for the specified word and docID in the given index
* `int index_find(index_t* index, const char* word, int docID);` - returns the count associated with the specified word and docID
* `void index_delete(index_t* index);` - deletes index and its associated sets and counters
* `void index_write(index_t* index, char* indexFilename);` - handles writing the index to a file
* `index_t* index_read(char* indexFilename);` - handles reading the index file created by index_write and creating and returning an index object

### word
* `char* word_normalize(char* word);` - normalizes a word to all lowercase

## Assumptions

* The `pageDirectory` passed into `pagedir_init` must already exist as a directory; this module handles file creation within it but does not dynamically create system directories.
* `docID` values passed to `pagedir_save` are assumed to be unique, positive integers greater than or equal to 1.
* `index_read` assumes that the file path provided leads to a file constructed using the index_write function.

## Compilation

To build `common.a`, execute:
```bash
make
```

To clean up, run: 
```bash
make clean
```