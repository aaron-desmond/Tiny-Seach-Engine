# TSE Querier

## Functionality Implemented

This querier implements the **full specification** (30/30 points):
- Prints the set of documents containing all words in the query (AND default)
- Supports explicit `and` and `or` operators
- `and` has higher precedence than `or`
- Results printed in decreasing order by score

## Assumptions

- `pageDirectory` must contain a `.crawler` file and at minimum a file `1`.
- The index file follows the format produced by the TSE Indexer: one word per line followed by space-separated `docID count` pairs.
- Words shorter than 3 letters are treated the same as any other word; queries containing them will simply return an empty result set.
- Blank or whitespace-only query lines are silently ignored.
- The AND score is the minimum of the two operand scores; the OR score is their sum.

## Usage

```
./querier pageDirectory indexFilename
```

Reads queries from stdin one per line. Prints `Query? ` prompt when stdin is
an interactive terminal. Type `Ctrl-D` (EOF) to exit.

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
