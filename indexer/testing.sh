#!/bin/bash

# testing.sh - test script for indexer

# Lila Hwang, May 2026

echo "********** Testing invalid arg count  **********"
./indexer

echo "********** Testing invalid crawl  directory **********"
./indexer fooDir test-index

echo "********* Testing valid crawl directory ***********"
./indexer /home/cs50/shared/tse/output/letters-10 test-index

echo "********** Testing indextest ************"
./indextest test-index test-index-2

echo "********* Comparing index files **********"
sort test-index > sorted1
sort test-index-2 > sorted2

diff sorted1 sorted2

rm -f sorted1 sorted2

echo "********* Valgrind test ***********"
valgrind --leak-check=full ./indexer /home/cs50/shared/tse/output/letters-10 test-index

echo "********** Done **********"
