/*
 * crawler.c - Implements the crawler module for the Tiny-Serarch-Engine(TSE)
 *
 * Crawls the web and retrieves webpages starting from a "seed" URL. It parses the seed webpage, extracts any embedded
 * URLs, and retrieves each of those pages.
 * 
 * Daisy Sanchez Garcia, June 2026
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../libcs50/webpage.h"
#include "../libcs50/bag.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/mem.h"
#include "../common/pagedir.h"
#include "../common/word.h"

/*
 * Function Prototypes 
 */
int main(const int argc, char* argv[]);
static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);

/*
 * main function - calls parseArgs and crawl, then exists 0
 */
int main(const int argc, char* argv[]){
    char* seedURL = NULL;
    char* pageDirectory = NULL;
    int maxDepth = 0;

    // Parse parameters; if validation fails, parseArgs exits the program
    parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);

    // Run the crawler
    crawl(seedURL, pageDirectory, maxDepth);

    return 0;
}

/*
 * parseArgs() function - takes arguments from the command line which are extracted into the function parameters
 * returns only if successful
 */
static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth) {
    if (argc != 4) {
        fprintf(stderr, "Usage: ./crawler seedURL pageDirectory maxDepth\n");
        exit(1);
    }

    // Validate and normalize Seed URL
    char* tempURL = normalizeURL(argv[1]);
    if (tempURL == NULL) {
        fprintf(stderr, "Error: Invalid seed URL format.\n");
        exit(2);
    }

    *seedURL = tempURL;
    
    if (!isInternalURL(*seedURL)) {
      free(*seedURL);
        fprintf(stderr, "Error: Seed URL must be internal to the CS50 playground.\n");
        exit(3);
    }

    // Validate Page Directory using pagedir module
    *pageDirectory = argv[2];
    if (!pagedir_init(*pageDirectory)) {
      free(*seedURL);
      fprintf(stderr, "Error: Directory '%s' is invalid or not writeable.\n", *pageDirectory);
        exit(4);
    }

    // Validate Max Depth
    char extra;
    if (sscanf(argv[3], "%d%c", maxDepth, &extra) != 1 || *maxDepth < 0 || *maxDepth > 10) {
      free(*seedURL);
      fprintf(stderr, "Error: maxDepth must be an integer between 0 and 10.\n");
        exit(5);
    }
}


/*
 * crawl function - does the work of crawling from seedURL to maxDepth and saving pages in pageDirectory
 */
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth) {
    // Initialize data structures
    hashtable_t* pagesSeen = hashtable_new(200);
    mem_assert(pagesSeen, "Error: Failed to allocate hashtable");

    bag_t* pagesToCrawl = bag_new();
    mem_assert(pagesToCrawl, "Error: Failed to allocate bag");

    //Insert seedURL into hashtable using the permanent argv pointer
    hashtable_insert(pagesSeen, seedURL, "");

    // Copy exclusively for ownership by the webpage structure
    char* webpageSeedCopy = malloc(strlen(seedURL) + 1);
    mem_assert(webpageSeedCopy, "Error: Failed allocating seed copy for webpage_t");
    strcpy(webpageSeedCopy, seedURL);

    webpage_t* seedPage = webpage_new(webpageSeedCopy, 0, NULL);
    mem_assert(seedPage, "Error: Failed to allocate seed webpage structure");
    bag_insert(pagesToCrawl, seedPage);
    
    int docID = 1;
    webpage_t* currPage;

    // Execution Loop
    while ((currPage = bag_extract(pagesToCrawl)) != NULL) {
        // Fetch HTML content safely (handles the required 1-second delay automatically)
        if (webpage_fetch(currPage)) {
            printf("%d Fetched: %s\n", webpage_getDepth(currPage), webpage_getURL(currPage));
            
            // Save the page structure to your page directory using your common module
            pagedir_save(currPage, pageDirectory, docID++);

            // Scan the HTML for links only if we haven't hit max depth limits
            if (webpage_getDepth(currPage) < maxDepth) {
                printf("%d Scanning: %s\n", webpage_getDepth(currPage), webpage_getURL(currPage));
                pageScan(currPage, pagesToCrawl, pagesSeen);
            }
        } else {
            fprintf(stderr, "Warning: Failed to fetch webpage %s\n", webpage_getURL(currPage));
        }
        
        // Clean up the extracted webpage object
        webpage_delete(currPage);
    }

    // Cleanup allocations
    bag_delete(pagesToCrawl, webpage_delete);
    hashtable_delete(pagesSeen, NULL);

    free(seedURL);
}

/*
 * pageScan function - implements pagescanner. Given a webpage, scans given page to extract URLs and ignore non-internal
 * URLs. adds URL to hashtable pages_seen and bag pages_to_crawl if URL not already seen before
 */
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen) {
    if (page == NULL || pagesToCrawl == NULL || pagesSeen == NULL) {
        return;
    }

    int pos = 0;
    char* nextURL;
    int currentDepth = webpage_getDepth(page);

    // Retrieve URLs sequentially from the webpage context
    while ((nextURL = webpage_getNextURL(page, &pos)) != NULL) {
      printf("%d Found: %s\n", currentDepth, nextURL);
      if (isInternalURL(nextURL)) {
	// Normalize URLs to prevent duplicates
	char* normalizedURL = normalizeURL(nextURL);
            if (normalizedURL != NULL) {
                // Attempt to insert the URL into the hashtable
                if (hashtable_insert(pagesSeen, normalizedURL, "")) {   
		  printf("%d Added: %s\n", currentDepth, normalizedURL);

		  // Allocate a separate clone for the webpage_t block to own
		  char* webpageURLCopy = malloc(strlen(normalizedURL) + 1);
		  mem_assert(webpageURLCopy, "Error: pageScan failed copying URL for webpage");
		  strcpy(webpageURLCopy, normalizedURL);
		  free(normalizedURL);

		  webpage_t* discoveredPage = webpage_new(webpageURLCopy, currentDepth + 1, NULL);
		  mem_assert(discoveredPage, "Error: pageScan failed to allocate webpage structure");
                    
		  bag_insert(pagesToCrawl, discoveredPage);

		  free(nextURL);
			
                } else {
		  printf("%d IgnDupl: %s\n", currentDepth, normalizedURL);
		  free(normalizedURL);
		  free(nextURL);
                }
            } else {
	      free(nextURL);
	    }
        } else {
	printf("%d IgnExtrn: %s\n", currentDepth, nextURL);
	free(nextURL);
        }
       
    }
}
