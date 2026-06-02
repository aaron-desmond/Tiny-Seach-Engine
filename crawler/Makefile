# Makefile for the TSE Crawler
# 
# Daisy Sanchez Garcia, June 2026

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../common

OBJS = crawler.o
LIBS = ../common/common.a ../libcs50/libcs50.a

.PHONY: all test clean

all: crawler

crawler: $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $^ -o $@

# Object file dependencies
crawler.o: crawler.c ../libcs50/webpage.h ../libcs50/bag.h ../libcs50/hashtable.h ../libcs50/mem.h ../common/pagedir.h ../common/word.h

test: crawler testing.sh
	bash -v testing.sh &> testing.out

clean:
	rm -rf crawler *.o *.dSYM
	rm -f *~