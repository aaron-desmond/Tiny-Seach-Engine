# Makefile for common directory
#   Builds and compiles all these utility functions into a common library.
#
# Charlotte Crawford, May 2026

# object files, and the target library

OBJS = pagedir.o word.o index.o
LIB = common.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make

# Build $(LIB) by archiving object files
$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

# Dependencies: object files depend on header files
pagedir.o: pagedir.h ../libcs50/webpage.h
word.o: word.h
index.o: index.h ../libcs50/hashtable.h ../libcs50/counters.h ../libcs50/mem.h

.PHONY: clean 

# clean up after our compilation
clean:
	rm -f core
	rm -f $(LIB) *~ *.o