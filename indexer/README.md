# TSE Indexer

## Functionality Implemented

This indexer implements the **full specification** for the Tiny Search Engine indexer and indextest

indexer:
- Validates command-line arguments
- Verifies that the provided pageDirectory was produced by the crawler
- Reads crawler-produced webpage files from the pageDirectory
- Extracts and normalizes words from each webpage
- Ignores words shorter than three characters
- Builds an inverted index mapping words to (docID, count) pairs
- Writes the completed index to an output file

indextest:
- Reads an index file produced by the indexer
- Reconstructs the index in memory
- Writes the reconstructed index to a second output file

## Assumptions

- `pageDirectory` must contain a `.crawler` file and at minimum a file `1`.
- Crawled page files follow the crawler spec format:
  - line 1: URL
  - line 2: crawl depth
  - remaining lines: HTML content

## Usage

indexer:

```
./indexer pageDirectory indexFilename
```

indextest:
```
./indextest index-output rebuilt-index
```

## Compilation

```
make
```

## Testing

```
make test
```

Output is saved to `testing.out`. Requires `valgrind` to be installed for the
memory-leak test.
