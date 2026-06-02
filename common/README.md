# CS50 TSE - Common Library

The `common` directory serves as a shared library module containing code utilized across multiple subsystems of the Tiny Search Engine (TSE), including the Crawler, Indexer, and Querier.

Currently, it implements the `pagedir` module, which handles initializing a page directory and safely saving fetched webpages to a directory.

## Interface

### pagedir
* `bool pagedir_init(const char* pageDirectory);`
  Initializes a directory by constructing a path to `.crawler` and verifying write permissions.
* `void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);`
  Saves a `webpage_t` into a formatted text file named by its `docID` inside the given directory.

## Assumptions
* The `pageDirectory` passed into `pagedir_init` must already exist as a directory; this module handles file creation within it but does not dynamically create system directories.
* `docID` values passed to `pagedir_save` are assumed to be unique, positive integers greater than or equal to 1.

## Files
* `pagedir.c` - Implementation of page directory management functions.
* `pagedir.h` - Function prototypes and interface documentation.
* `Makefile` - Compilation instructions generating `common.a`.

## Compilation & Testing
To compile the library, execute:
```bash
make