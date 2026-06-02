# Makefile for TSE indexer
#
# Lila Hwang, May 2026

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../common -I../libcs50

LIBS = ../libcs50/libcs50-given.a
COMMON = ../common/index.c ../common/word.c ../common/pagedir.c

OBJS = indexer.o
TESTOBJS = indextest.o

.PHONY: all clean test

all: indexer indextest

indexer: $(OBJS) $(COMMON)
	$(CC) $(CLFAGS) $^ $(LIBS) -o indexer

indextest: $(TESTOBJS) $(COMMON)
	$(CC) $(CFLAGS) $^ $(LIBS) -o indextest

indexer.o: indexer.c
	$(CC) $(CLFAGS) -c indexer.c

indextest.o: indextest.c
	$(CC) $(CFLAGS) -c indextest.c

test: indexer indextest
	bash -v testing.sh

clean:
	rm -f *.o
	rm -f indexer
	rm -f indextest
	rm -f testing.out
	rm -f test-index
	rm -f test-index-2
