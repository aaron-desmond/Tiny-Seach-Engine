# CS50 TSE Querier
## Design Spec

In this document, we focus on the implementation-independent design designions for the Querier of the Tiny Search Engine (TSE).

### User interface

As described in the assignment, the querier interacts with the user through the command-line which must always have two arguments and standard input. 

``` bash
$ ./querier pageDirectory indexFilename
```

### Inputs and outputs

**Input**:
- Takes command-line arguments: pageDirectory and indexFilename
- Searches queries from stdin, one per line


**Output**: 
- A ranked list of matching documents that satisfy the query in decreasing rank order according to its score. Each matching document output includes its score, document ID, and URL. 
- if query syntax is invalid, print an error message
- If no documents match, it outputs a message that no match was found. 

### Functional decomposition into modules

We anticipate the following primary functions/modules:
 1. *main*: Initializes the index data structure and runs the query loop.
 2. *processQuery*: Takes the queries from stdin and handles the query logic.
 3. *tokenizeQuery*:Takes an input and converts characters to lowercase letters. It validates that the input contains only letters and spaces, and breaks the string into an array of words. 
 4. *validateSyntax*: Goes through words to ensure that they have a valid syntax (ie. no starting or ending operators, no adjacent operators).
 5. *evaluateQuery*: Loops through the words and evaluates matches using AND and OF, returning a set of document counters.
 6. *rankAndPrint*: Takes the final counter set, sorts the documents in decreasing order of score, and prints them.

Some helper modules: 
 1. *index*: to load and look up words
 2. *counter*: to track and combine document IDs and scores
 3. *pagedir/word*: To validate directories and string normalization


### Pseudo code for logic/algorithmic flow

The Querier will run as follows:

	Parse command line arguments and validate pageDirectory and indexFilename
	Load the index from indexFilename into an index hashtable structure
	While reading a line from stdin succeeds:
	  If line contains invalid characters (non-alpha, non-space):
		Print an error message
		Continue to next prompt
	  Tokenize the line into an array of words
	  Print the “clean query”
	  If validateSyntax() fails:
		Print a syntax error message
		Continue to next prompt
	  Final_counter = evaluateQuery()
	  randAndPrint()
	  Clean up memory for this query iteration

where processQuery:

    Strips special characters and parses through tokenizeQuery
    Print clean query
    Verify valid grammar through validateSyntax
    Process with evaluateQuery to get matches
    Present results to stdout with rankAndPrint
    Memory cleanup by deleting and freeing counters and freeing word arrays


where tokenizeQuery:

    Initializes an array of strings and count to 0
    Loops through string length 
    If character isalpha
    Change to lowercase
    Check if in word
    If space
    	End of word with null terminator
    	Add to array and increment count
    Error if neither alphabet or space

where validateSyntax:

	If “and”or “or at the beginning or end, erro
	Iterate through array of words
		If current word is “and” or “or”
			If next word is “and” or “or”
				Error
	Return true

where evaluateQuery:

	Loops through array of normalized words
		If “or”
			Finish current “and” sum and then merge into sum
		If “and”
			Continue

where rankAndPrint:

	While matching counters object still contains scores > 0:
		Use a counters_iterate function to find the document with the maximum score
		If max == 0:
			Break
		Read the first line of the document file in pageDirectory to extract its URL
		Print “score: [max] dox: [docID] URL: [url]”
		Set the counter value for this docID to 0

### Major data structures

The key data structure is the index, mapping from word to (docID, #occurrences) pairs. The index is a hashtable keyed by word and storing counters as items. The counters is keyed by docID and stores a count of the number of occurrences of that word in the document with that ID.

### Testing plan

To test the querier module as a whole, the queriertest program will present queries and direct the querier to act from the shared directory’s crawler and indexer as well as invalid inputs. This will include missing arguments, too many arguments, non-existent directories, inaccessible destination files, and invalid syntax which are possible mistakes that can be made. This will also run the program through Valgrind for memory leaks.
	7. invalid `indexFile` (read-only directory)
	7. invalid `indexFile` (existing, read-only file)
0. Run *indexer* on a variety of pageDirectories and use *indextest* as one means of validating the resulting index.
0. Run *valgrind* on both *indexer* and *indextest* to ensure no memory leaks or errors.
