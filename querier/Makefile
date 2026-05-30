# Makefile for TSE querier
#
# Aaron Desmond, May 2026

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb
INCLUDES = -I../libcs50 -I../common
LIBS = ../common/common.a ../libcs50/libcs50.a

.PHONY: all clean test

all: querier

querier: querier.o $(LIBS)
	$(CC) $(CFLAGS) $^ -o $@

querier.o: querier.c ../common/index.h ../common/pagedir.h ../common/word.h \
           ../libcs50/counters.h ../libcs50/mem.h ../libcs50/file.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

test: querier
	bash -v testing.sh &> testing.out

clean:
	rm -f querier querier.o testing.out
