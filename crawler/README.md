# CS50 TSE - Crawler

The `crawler` is a sub-system of the Tiny Search Engine. It crawls through the web by extracting links sequentially, filtering out non-internal URLs, avoiding duplication through a tracking hashtable, and using a bag for facilitating page discovery. Extracted pages are saved into a directory structure.

## Usage
The crawler takes exactly three command-line arguments:
```bash
./crawler seedURL pageDirectory maxDepth
```
seedURL: The internal URL from which the crawl loop starts.

pageDirectory: An existing directory where discovered page structures are logged.

maxDepth: An integer from 0 to 10 limiting the link-hop distance from the seed.

## Implementation Details
The crawler.c contains four primary functions:

* `int main(const int argc, char* argv[]);`
    Manages control flow.

* `static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth);`
    Validates CLI parameters cleanly using defensive checks.

* `static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);` 
    The central execution loop which manages the process of allocations.

* `static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);` 
    Extracts embedded target URLs from text bodies.

It leverages the shared pagedir module situated inside ../common to write output configurations.

## Assumptions
The pageDirectory parameters must refer to pre-existing directories.

Memory constraints are checked through mem_assert.

## Testing
To build and execute testing.sh, run:

```bash
make test
```
The output will be saved to testing.out. 