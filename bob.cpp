// Sample Bob: brute-force queries all pairs (i,j) with i < j.
// Works only for N <= 28 (C(28,2) = 378 <= 400 queries).
// Does not use exploration. For testing only.

#include "grader.h"

void bob(int N) {
    std::vector<std::pair<int,int>> found;

    for (int i = 1; i <= N; i++)
        for (int j = i + 1; j <= N; j++)
            if (ask({i}, {j}))
                found.push_back({i, j});

    answer(found);
}
