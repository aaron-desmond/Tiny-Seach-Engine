#!/bin/bash
#
# testing.sh - Integration testing script for the TSE Crawler module
#
# Usage: make test (which redirects stdout/stderr using bash -v)
#
# Daisy Sanchez Garcia, June 2026

# Define variables for testing outputs
DATA_DIR="data"
TEST_DIR_1="$DATA_DIR/letters-depth1"
TEST_DIR_2="$DATA_DIR/letters-depth3"

echo "=== Starting Crawler Testing ==="

# Clean and setup data environment directories
rm -rf $DATA_DIR
mkdir -p $TEST_DIR_1
mkdir -p $TEST_DIR_2

# -------------------------------------------------------------
# 1. Error Handling / Parameter Validation Tests
# -------------------------------------------------------------
echo -e "\n--- Test 1: Missing arguments ---"
./crawler 2>&1

echo -e "\n--- Test 2: Invalid external seed URL ---"
./crawler https://www.google.com $TEST_DIR_1 1 2>&1

echo -e "\n--- Test 3: Invalid data directory ---"
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html /nonexistent/dir 1 2>&1

echo -e "\n--- Test 4: Invalid depth limits ---"
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html $TEST_DIR_1 11 2>&1


# -------------------------------------------------------------
# 2. Functional Crawl Logs Emulation
# -------------------------------------------------------------
echo -e "\n--- Test 5: Standard crawl (Letters, Depth 1) ---"
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html $TEST_DIR_1 1

echo -e "\n--- Test 6: Deep crawl matching course example (Letters, Depth 3) ---"
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html $TEST_DIR_2 3


# -------------------------------------------------------------
# 3. Valgrind Memory Integrity Verification
# -------------------------------------------------------------
echo -e "\n--- Test 7: Valgrind Memory Leak Inspection ---"
valgrind --leak-check=full --show-leak-kinds=all ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html $TEST_DIR_1 1

echo -e "\n=== Testing Complete ==="
