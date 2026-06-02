# TSE Indexer

The TSE indexer is a standalone program that reads the document files produced by the TSE crawler, builds an index, and writes that index to a file. Its companion, the index tester, loads an index file produced by the indexer and saves it to another file.

This directory also contains the indextester which

## Usage

### indexer
The indexer takes two command line arguments:
```
./indexer pageDirectory indexFilename
```
* pageDirectory: the pathname of a directory produced by the Crawler
* indexFilename: the pathname of a file into which the index should be written (creates the file if needed and overwrites the file if it already exists)

### indextest
The indextest takes two command line arguments:
```
./indextest oldIndexFilename newIndexFilename
```
* oldIndexFilename: the pathname a file containing an index as written by index_write
* newIndexFilename: the pathname of a file of where to write the index back out to

## Assumptions

- 



## Compilation

```
make
```

## Testing

```
make test
```
