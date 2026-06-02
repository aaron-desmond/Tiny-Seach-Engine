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

Implementation Details
The crawler.c contains four primary functions:

main: Manages control flow.

parseArgs: Validates CLI parameters cleanly using defensive checks.

crawl: The central execution loop which manages the process of allocations.

pageScan: Extracts embedded target URLs from text bodies.

It leverages the shared pagedir module situated inside ../common to write output configurations.

Assumptions
The pageDirectory parameters must refer to pre-existing directories.

Memory constraints are checked through mem_assert.

Known Bugs
[If your implementation has known bugs or minor leaks under specific boundary edge-cases, document them explicitly here to satisfy the 50% penalty mitigation policy stated in Canvas. If none, write "None known."]

Testing
To build and execute testing.sh, run:

```bash
make test
```
The output will be saved to testing.out. 