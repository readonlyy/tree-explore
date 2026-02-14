#!/bin/bash
# Stress tester: generates random tests and runs grader in a loop.
# Usage: ./stress.sh [max_N] [num_tests]

set -e
cd "$(dirname "$0")"

MAX_N=${1:-28}
NUM_TESTS=${2:-100}

# Compile if needed
if [ ! -f ./gen ] || [ gen.cpp -nt ./gen ]; then
    g++ -O2 -std=c++17 -o gen gen.cpp
fi
if [ ! -f ./run ] || [ grader.cpp -nt ./run ] || [ alice.cpp -nt ./run ] || [ bob.cpp -nt ./run ]; then
    g++ -O2 -std=c++17 -o run grader.cpp alice.cpp bob.cpp
fi

echo "Stress test: max_N=$MAX_N, num_tests=$NUM_TESTS"
echo ""

PASS=0
FAIL=0
TYPES=("random" "path" "star" "binary")

for i in $(seq 1 $NUM_TESTS); do
    N=$(( (RANDOM % (MAX_N - 1)) + 2 ))
    TYPE=${TYPES[$((RANDOM % 4))]}
    SEED=$i

    ./gen $N $TYPE $SEED > /tmp/tree_spec_stress.txt

    if ./run /tmp/tree_spec_stress.txt --all-starts 2>/dev/null; then
        PASS=$((PASS + 1))
        echo "Test $i: N=$N type=$TYPE seed=$SEED -> OK"
    else
        FAIL=$((FAIL + 1))
        echo "Test $i: N=$N type=$TYPE seed=$SEED -> FAILED"
        echo "  Rerun: ./run /tmp/tree_spec_stress.txt --all-starts -v"
        cp /tmp/tree_spec_stress.txt "/tmp/tree_spec_fail_${i}.txt"
    fi
done

echo ""
echo "========================================="
echo "Stress test: $PASS passed, $FAIL failed out of $NUM_TESTS"
[ $FAIL -eq 0 ]
