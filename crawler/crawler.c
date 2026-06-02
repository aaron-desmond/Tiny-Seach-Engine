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
    *seedURL = word_normalize(argv[1]);
    if (seedURL == NULL) {
        fprintf(stderr, "Error: Invalid seed URL format.\n");
        exit(2);
    }
    if (!isInternalURL(*seedURL)) {
        fprintf(stderr, "Error: Seed URL must be internal to the CS50 playground.\n");
        exit(3);
    }

    // Validate Page Directory using pagedir module
    *pageDirectory = argv[2];
    if (!pagedir_init(*pageDirectory)) {
        fprintf(stderr, "Error: Directory '%s' is invalid or not writeable.\n", *pageDirectory);
        exit(4);
    }

    // Validate Max Depth
    char extra;
    if (sscanf(argv[3], "%d%c", maxDepth, &extra) != 1 || *maxDepth < 0 || *maxDepth > 10) {
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

    // Allocate and insert the seed URL 
    char* seedCopy1 = malloc(strlen(seedURL)+1);
    char* seedCopy2 = malloc(strlen(seedURL)+1);
    mem_assert(seedCopy1, "Error: Failed to allocate memory for seed URL copy 1");
    mem_assert(seedCopy2, "Error: Failed to allocate memory for seed URL copy 2");

    strcpy(seedCopy1, seedURL);
    strcpy(seedCopy2, seedURL);

    hashtable_insert(pagesSeen, seedCopy1, "");

    webpage_t* seedPage = webpage_new(seedCopy2, 0, NULL);
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
    hashtable_delete(pagesSeen, NULL);
    bag_delete(pagesToCrawl, webpage_delete); 
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

    // Retrieve URLs sequentially from the webpage context
    while ((nextURL = webpage_getNextURL(page, &pos)) != NULL) {
        if (isInternalURL(nextURL)) {
            // Normalize URLs to prevent duplicates 
            if (normalizeURL(nextURL)) {
                // Attempt to insert the URL into the hashtable
                if (hashtable_insert(pagesSeen, nextURL, "")) {
                    char* pageURL = malloc(strlen(nextURL)+1);
                    strcpy(pageURL, nextURL);

                    webpage_t* discoveredPage =
                        webpage_new(pageURL,
                                    webpage_getDepth(page)+1,
                                    NULL);
                    
                    mem_assert(discoveredPage, "Error: Failed to allocate memory for discovered webpage");

                    bag_insert(pagesToCrawl, discoveredPage);
                    printf("Added: %s\n", nextURL); 
                } else {
                    // If hashtable already contains the key; free the duplicate string
                    mem_free(nextURL);
                }
            } else {
                mem_free(nextURL);
            }
        } else {
            mem_free(nextURL);
        }
    }
}
