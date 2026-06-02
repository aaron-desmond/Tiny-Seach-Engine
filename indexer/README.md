# TSE Indexer

The TSE indexer is a standalone program that reads the document files produced by the TSE crawler, builds an index, and writes that index to a file. Its companion, the index tester, loads an index file produced by the indexer and saves it to another file.

## Assumptions

- 


## Usage

The indexer takes two command line arguments:
```
./indexer pageDirectory indexFilename
```



## Compilation

```
make
```

## Testing

```
make test
```
