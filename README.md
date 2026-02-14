# Tree Speculation — Local Judge

## Quick Start

```bash
./run_tests.sh
```

This compiles everything, generates N=100 test cases, and prints a results table.

## Writing Your Solution

Implement `alice()` in `alice.cpp` and `bob()` in `bob.cpp`. Include `grader.h`.

```cpp
// alice.cpp
#include "grader.h"
void alice(int N, std::vector<int> P) {
    // encode P into a tree, then call:
    setTree(K, edges);
}

// bob.cpp
#include "grader.h"
void bob(int N) {
    // use ask(), doExplore(), doMove() to find edges, then call:
    answer(edges);
}
```

## Manual Run

```bash
g++ -O2 -std=c++17 -o run grader.cpp alice.cpp bob.cpp
./run tests/test_random.txt --all-starts -v
```

### Flags

- `--all-starts` — test all starting nodes
- `--start S` — test a specific starting node
- `-v` — verbose output

## Generating Tests

```bash
g++ -O2 -std=c++17 -o gen gen.cpp
./gen 100 random 42 > test.txt
```

Types: `random`, `path`, `star`, `binary`, `caterpillar`, `spider`, `broom`, `doublestar`, `comb`, `deep`, `wide`, `random_deep`

## Scoring

```
score = 100 * min(1, sqrt(150/K)) * min(1, (400/Q)^1.5)
```
