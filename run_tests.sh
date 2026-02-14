#!/bin/bash
set -e
cd "$(dirname "$0")"

g++ -O2 -std=c++17 -o gen gen.cpp
g++ -O2 -std=c++17 -o run grader.cpp alice.cpp bob.cpp

mkdir -p tests

TYPES="random path star binary caterpillar spider broom doublestar comb deep wide random_deep"
for t in $TYPES; do
    ./gen 100 $t 1 > tests/test_${t}.txt
done
# extra random seeds
for s in 2 3 4 5; do
    ./gen 100 random $s > tests/test_random${s}.txt
done

echo ""
printf "%-28s  %-2s  %5s  %5s  %-7s  %6s\n" "Test" "  " "K" "Q" "Starts" "Score"
echo "----------------------------  --  -----  -----  -------  ------"
for f in tests/test_*.txt; do
    ./run "$f" --all-starts 2>/dev/null || true
done
echo ""
