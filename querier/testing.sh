#!/bin/bash
#
# testing.sh - Integration tests for the TSE Querier
# Aaron Desmond, May 2026
#
# Tests cover: invalid arguments, bad query characters, operator syntax errors,
# single-word queries, AND queries (explicit and implicit), OR queries,
# AND/OR precedence, case insensitivity, blank lines, and valgrind.
#
# Run via: make test  (runs: bash -v testing.sh &> testing.out)

QUERIER=./querier
# Unique per-run temp paths so tests never collide with other users on a
# shared host (mktemp -d creates the directory; mktemp creates the file).
PAGEDIR=$(mktemp -d)
INDEXFILE=$(mktemp)

# Set up a minimal test fixture
touch "$PAGEDIR/.crawler"
printf "http://example.com/\n0\n<html>computer science dartmouth</html>\n" > "$PAGEDIR/1"
printf "computer 1 2\nscience 1 1\ndartmouth 1 3\n" > "$INDEXFILE"

echo "======== Test 1: Wrong number of arguments ========"
$QUERIER
$QUERIER $PAGEDIR
$QUERIER $PAGEDIR $INDEXFILE extra

echo "======== Test 2: Invalid pageDirectory (not crawler-produced) ========"
$QUERIER /tmp $INDEXFILE

echo "======== Test 3: Invalid index file ========"
$QUERIER $PAGEDIR /nonexistent/file

echo "======== Test 4: Bad characters in query ========"
echo "hello!" | $QUERIER $PAGEDIR $INDEXFILE
echo "cat & dog" | $QUERIER $PAGEDIR $INDEXFILE
echo "2fast" | $QUERIER $PAGEDIR $INDEXFILE

echo "======== Test 5: Operator at start or end ========"
echo "and" | $QUERIER $PAGEDIR $INDEXFILE
echo "or" | $QUERIER $PAGEDIR $INDEXFILE
echo "and computer" | $QUERIER $PAGEDIR $INDEXFILE
echo "or computer" | $QUERIER $PAGEDIR $INDEXFILE
echo "computer and" | $QUERIER $PAGEDIR $INDEXFILE
echo "computer or" | $QUERIER $PAGEDIR $INDEXFILE

echo "======== Test 6: Adjacent operators ========"
echo "computer and or science" | $QUERIER $PAGEDIR $INDEXFILE
echo "computer and and science" | $QUERIER $PAGEDIR $INDEXFILE
echo "computer or and science" | $QUERIER $PAGEDIR $INDEXFILE
echo "computer or or science" | $QUERIER $PAGEDIR $INDEXFILE

echo "======== Test 7: Word not in index ========"
echo "xyzzy" | $QUERIER $PAGEDIR $INDEXFILE

echo "======== Test 8: Single word queries ========"
echo "computer" | $QUERIER $PAGEDIR $INDEXFILE
echo "dartmouth" | $QUERIER $PAGEDIR $INDEXFILE

echo "======== Test 9: AND queries (explicit and implicit) ========"
echo "computer science" | $QUERIER $PAGEDIR $INDEXFILE
echo "computer and science" | $QUERIER $PAGEDIR $INDEXFILE
echo "computer and xyzzy" | $QUERIER $PAGEDIR $INDEXFILE

echo "======== Test 10: OR queries ========"
echo "computer or dartmouth" | $QUERIER $PAGEDIR $INDEXFILE
echo "xyzzy or dartmouth" | $QUERIER $PAGEDIR $INDEXFILE

echo "======== Test 11: AND/OR precedence ========"
# "computer science or dartmouth" = (computer AND science) OR dartmouth
echo "computer science or dartmouth" | $QUERIER $PAGEDIR $INDEXFILE

echo "======== Test 12: Blank and whitespace-only lines ========"
echo "" | $QUERIER $PAGEDIR $INDEXFILE
echo "   " | $QUERIER $PAGEDIR $INDEXFILE

echo "======== Test 13: Case insensitivity ========"
echo "COMPUTER Science" | $QUERIER $PAGEDIR $INDEXFILE
echo "DaRtMoUtH" | $QUERIER $PAGEDIR $INDEXFILE

echo "======== Test 14: Valgrind memory check ========"
echo "computer science" | valgrind --leak-check=full --error-exitcode=1 \
    $QUERIER $PAGEDIR $INDEXFILE
if [ $? -eq 0 ]; then
  echo "VALGRIND: no memory errors"
else
  echo "VALGRIND: memory errors detected"
fi

# Clean up the temporary fixture
rm -rf "$PAGEDIR" "$INDEXFILE"

echo "======== All tests complete ========"
