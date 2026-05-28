#include <stdbool.h>
#include "../libcs50/webpage.h"

/* 
 * Verifies whether the given directory exists and contains the '.crawler' file
 * Returns true if valid, false otherwise
 */
bool pagedir_init(const char* pageDirectory);

/*
 * Constructs the path 'pageDirectory/docID', reads the URL, and HTML
 * content from that file, and returns a newly allocated webpage_t structure.
 * Caller is repsonsible for later calling webpage_delete() on the returned pointer.
 * Returns NULL if any error occurs during reading or memory allocation.
 */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);