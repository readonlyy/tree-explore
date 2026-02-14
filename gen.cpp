// Test generator for Tree Speculation problem.
// Usage: ./gen <N> <type> [seed]
// Types: random, path, star, binary
// Output format:
//   Line 1: N
//   Line 2: preorder traversal (N integers)
//   Lines 3..N+1: edges of original tree (N-1 lines)

#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <string>
#include <random>
#include <functional>
#include <cstdlib>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: ./gen <N> <type> [seed]" << endl;
        cerr << "Types: random, path, star, binary, caterpillar, spider,\n"
             << "       broom, doublestar, comb, deep, wide, random_deep" << endl;
        return 1;
    }

    int N = atoi(argv[1]);
    string type = argv[2];
    unsigned seed = argc > 3 ? atoi(argv[3]) : 42;

    mt19937 rng(seed);

    vector<pair<int,int>> edges;
    vector<vector<int>> adj(N + 1);

    auto add_edge = [&](int u, int v) {
        edges.push_back({u, v});
        adj[u].push_back(v);
        adj[v].push_back(u);
    };

    if (type == "path") {
        // 1-2-3-...-N  (depth N-1)
        for (int i = 1; i < N; i++) add_edge(i, i + 1);

    } else if (type == "star") {
        // node 1 connected to all others (depth 1)
        for (int i = 2; i <= N; i++) add_edge(1, i);

    } else if (type == "binary") {
        // complete binary tree (depth ~log N)
        for (int i = 2; i <= N; i++) add_edge(i / 2, i);

    } else if (type == "caterpillar") {
        // path spine 1-2-...-L, rest are leaves hanging off spine
        int L = max(2, N / 3);
        for (int i = 1; i < L; i++) add_edge(i, i + 1);
        for (int i = L + 1; i <= N; i++) {
            int spine = uniform_int_distribution<int>(1, L)(rng);
            add_edge(spine, i);
        }

    } else if (type == "spider") {
        // center node 1 with ~sqrt(N) legs of equal length
        int legs = max(2, (int)sqrt(N));
        int leg_len = max(1, (N - 1) / legs);
        int cur = 2;
        for (int l = 0; l < legs && cur <= N; l++) {
            add_edge(1, cur);
            for (int j = 1; j < leg_len && cur + 1 <= N; j++) {
                add_edge(cur, cur + 1);
                cur++;
            }
            cur++;
        }
        // leftover nodes: attach to node 1
        while (cur <= N) { add_edge(1, cur); cur++; }

    } else if (type == "broom") {
        // long path 1-2-...-H, then a star at node H
        int H = N / 2;
        for (int i = 1; i < H; i++) add_edge(i, i + 1);
        for (int i = H + 1; i <= N; i++) add_edge(H, i);

    } else if (type == "doublestar") {
        // two hubs connected: 1-2, half the nodes on 1, half on 2
        add_edge(1, 2);
        for (int i = 3; i <= N; i++) {
            add_edge(i % 2 == 1 ? 1 : 2, i);
        }

    } else if (type == "comb") {
        // path 1-2-...-H, each spine node gets one leaf
        int H = N / 2;
        for (int i = 1; i < H; i++) add_edge(i, i + 1);
        int cur = H + 1;
        for (int i = 1; i <= H && cur <= N; i++, cur++) {
            add_edge(i, cur);
        }

    } else if (type == "deep") {
        // very deep: ternary tree that's intentionally unbalanced
        // each node's first child gets a long chain
        int cur = 2;
        for (int i = 1; i <= N && cur <= N; i++) {
            int children_cnt = (i <= 3) ? 3 : 1;
            for (int c = 0; c < children_cnt && cur <= N; c++, cur++) {
                add_edge(i, cur);
            }
        }

    } else if (type == "wide") {
        // 3-level tree: root -> ~10 nodes -> rest are leaves
        int mid = min(10, N - 1);
        for (int i = 2; i <= mid + 1 && i <= N; i++) add_edge(1, i);
        for (int i = mid + 2; i <= N; i++) {
            int par = uniform_int_distribution<int>(2, mid + 1)(rng);
            add_edge(par, i);
        }

    } else if (type == "random_deep") {
        // random tree biased toward deep: parent = max(1, i - sqrt(i))
        for (int i = 2; i <= N; i++) {
            int lo = max(1, i - (int)sqrt(i));
            int par = uniform_int_distribution<int>(lo, i - 1)(rng);
            add_edge(par, i);
        }

    } else { // random
        for (int i = 2; i <= N; i++) {
            int par = uniform_int_distribution<int>(1, i - 1)(rng);
            add_edge(par, i);
        }
    }

    // Build rooted tree at node 1, compute preorder traversal
    vector<vector<int>> children(N + 1);
    vector<bool> visited(N + 1, false);
    queue<int> q;
    q.push(1);
    visited[1] = true;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v : adj[u]) {
            if (!visited[v]) {
                visited[v] = true;
                children[u].push_back(v);
                q.push(v);
            }
        }
    }
    // Sort children for deterministic traversal order
    for (int i = 1; i <= N; i++)
        sort(children[i].begin(), children[i].end());

    vector<int> preorder;
    function<void(int)> dfs = [&](int u) {
        preorder.push_back(u);
        for (int v : children[u]) dfs(v);
    };
    dfs(1);

    // Output
    cout << N << "\n";
    for (int i = 0; i < N; i++) {
        if (i) cout << " ";
        cout << preorder[i];
    }
    cout << "\n";
    for (auto [u, v] : edges) {
        cout << u << " " << v << "\n";
    }

    return 0;
}
