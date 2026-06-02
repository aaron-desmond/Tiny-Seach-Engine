#include <stdbool.h>
#include "../libcs50/webpage.h"

/* 
 * Verifies whether the given directory exists and contains the '.crawler' file
 * Returns true if valid, false otherwise
 */
bool pagedir_init(const char* pageDirectory);

/*
 * Verifies whether the given directory exists and was produced by the crawler,
 * i.e. it contains a readable '.crawler' file. Unlike pagedir_init, this does
 * not create or modify any files.
 * Returns true if valid, false otherwise.
 */
bool pagedir_validate(const char* pageDirectory);

/*
 * Reads and returns the URL (first line) of the page file 'pageDirectory/docID'.
 * Returns a newly allocated string the caller must free, or NULL if the file
 * cannot be opened, docID < 1, or pageDirectory is NULL.
 */
char* pagedir_getURL(const char* pageDirectory, int docID);

/*
 * Constructs the path 'pageDirectory/docID', reads the URL, and HTML
 * content from that file, and returns a newly allocated webpage_t structure.
 * Caller is repsonsible for later calling webpage_delete() on the returned pointer.
 * Returns NULL if any error occurs during reading or memory allocation.
 */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

/*                                                                                * parse the file pageDirectory/docID for the elements needed to generate and return its associated webpage                                                       
 *                                                                               
 * Parameters: const char* pageDirectory, int docID                              
 * Returns: webpage_t* webpage associated with docID of pageDirectory            
 */
webpage_t* pagedir_load(const char* pageDirectory, int docID);
