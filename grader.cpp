#include "grader.h"
#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

using namespace std;

// Scoring formula: score = 100 * min(1, sqrt(150/K)) * min(1, (400/Q)^1.5)
// Hard limits (beyond these = protocol error)
static const int MAX_K = 10000;
static const int MAX_Q = 5000;
static const int MAX_MOVES = 5;

// ============ Grader state ============

static int g_N;
static vector<int> g_preorder;
static vector<pair<int,int>> g_orig_edges;
static map<int, set<int>> g_orig_adj;

static int g_K = 0;
static vector<pair<int,int>> g_alice_edges;
static map<int, set<int>> g_alice_adj;
static bool g_alice_set = false;

static int g_start_node;
static int g_current_node;
static int g_query_count;
static int g_move_count;
static bool g_explored;
static bool g_answered;
static vector<pair<int,int>> g_bob_answer;
static bool g_verbose = false;

// ============ Grader-provided functions ============

void setTree(int K, vector<pair<int,int>> edges) {
    if (g_alice_set) {
        fprintf(stderr, "PROTOCOL ERROR: setTree() called more than once\n");
        exit(1);
    }
    if (K < 1 || K > MAX_K) {
        fprintf(stderr, "PROTOCOL ERROR: K=%d out of range [1, %d]\n", K, MAX_K);
        exit(1);
    }
    if ((int)edges.size() != K - 1) {
        fprintf(stderr, "PROTOCOL ERROR: %d edges for K=%d (expected %d)\n",
                (int)edges.size(), K, K - 1);
        exit(1);
    }

    g_K = K;
    g_alice_edges = edges;
    g_alice_adj.clear();
    for (auto& [u, v] : edges) {
        if (u < 1 || u > K || v < 1 || v > K) {
            fprintf(stderr, "PROTOCOL ERROR: Alice edge (%d,%d) has label outside [1,%d]\n",
                    u, v, K);
            exit(1);
        }
        g_alice_adj[u].insert(v);
        g_alice_adj[v].insert(u);
    }

    // Verify connectivity via BFS from node 1
    set<int> visited;
    vector<int> bfs;
    bfs.push_back(1);
    visited.insert(1);
    for (int i = 0; i < (int)bfs.size(); i++) {
        int u = bfs[i];
        for (int v : g_alice_adj[u]) {
            if (!visited.count(v)) {
                visited.insert(v);
                bfs.push_back(v);
            }
        }
    }
    if ((int)visited.size() != K) {
        fprintf(stderr, "PROTOCOL ERROR: Alice's tree is not connected (%d/%d reachable)\n",
                (int)visited.size(), K);
        exit(1);
    }

    g_alice_set = true;
}

int ask(vector<int> A, vector<int> B) {
    g_query_count++;
    if (g_query_count > MAX_Q) {
        fprintf(stderr, "PROTOCOL ERROR: Exceeded %d queries (query #%d)\n", MAX_Q, g_query_count);
        exit(1);
    }

    set<int> setB(B.begin(), B.end());
    for (int a : A) {
        if (setB.count(a)) {
            fprintf(stderr, "PROTOCOL ERROR: A and B not disjoint (element %d)\n", a);
            exit(1);
        }
    }

    for (int a : A) {
        if (g_orig_adj.count(a)) {
            for (int v : g_orig_adj[a]) {
                if (setB.count(v)) {
                    if (g_verbose)
                        fprintf(stderr, "  ask(|A|=%d,|B|=%d) -> 1\n",
                                (int)A.size(), (int)B.size());
                    return 1;
                }
            }
        }
    }
    if (g_verbose)
        fprintf(stderr, "  ask(|A|=%d,|B|=%d) -> 0\n", (int)A.size(), (int)B.size());
    return 0;
}

pair<int, vector<int>> doExplore() {
    if (g_explored) {
        fprintf(stderr, "PROTOCOL ERROR: doExplore() called more than once\n");
        exit(1);
    }
    g_explored = true;
    g_current_node = g_start_node;

    vector<int> neighbors;
    if (g_alice_adj.count(g_current_node)) {
        neighbors.assign(g_alice_adj[g_current_node].begin(),
                         g_alice_adj[g_current_node].end());
    }
    sort(neighbors.begin(), neighbors.end());

    if (g_verbose)
        fprintf(stderr, "  doExplore() -> node=%d deg=%d\n",
                g_current_node, (int)neighbors.size());
    return {g_current_node, neighbors};
}

pair<int, vector<int>> doMove(int target) {
    if (!g_explored) {
        fprintf(stderr, "PROTOCOL ERROR: doMove() called before doExplore()\n");
        exit(1);
    }
    g_move_count++;
    if (g_move_count > MAX_MOVES) {
        fprintf(stderr, "PROTOCOL ERROR: Exceeded %d moves (move #%d)\n", MAX_MOVES, g_move_count);
        exit(1);
    }
    if (!g_alice_adj.count(g_current_node) ||
        !g_alice_adj[g_current_node].count(target)) {
        fprintf(stderr, "PROTOCOL ERROR: %d is not a neighbor of %d\n",
                target, g_current_node);
        exit(1);
    }

    g_current_node = target;
    vector<int> neighbors(g_alice_adj[g_current_node].begin(),
                          g_alice_adj[g_current_node].end());
    sort(neighbors.begin(), neighbors.end());

    if (g_verbose)
        fprintf(stderr, "  doMove(%d) -> deg=%d\n", target, (int)neighbors.size());
    return {g_current_node, neighbors};
}

void answer(vector<pair<int,int>> edges) {
    if (g_answered) {
        fprintf(stderr, "PROTOCOL ERROR: answer() called more than once\n");
        exit(1);
    }
    g_bob_answer = edges;
    g_answered = true;
}

// ============ Main ============

int main(int argc, char* argv[]) {
    const char* test_file = nullptr;
    bool all_starts = false;
    int specific_start = -1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--all-starts") == 0) {
            all_starts = true;
        } else if (strcmp(argv[i], "--start") == 0 && i + 1 < argc) {
            specific_start = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            g_verbose = true;
        } else if (argv[i][0] != '-') {
            test_file = argv[i];
        }
    }

    if (!test_file) {
        fprintf(stderr, "Usage: ./run <test_file> [--all-starts] [--start S] [-v]\n");
        return 1;
    }

    // Read test file
    ifstream fin(test_file);
    if (!fin) {
        fprintf(stderr, "Cannot open: %s\n", test_file);
        return 1;
    }
    fin >> g_N;
    g_preorder.resize(g_N);
    for (int i = 0; i < g_N; i++) fin >> g_preorder[i];
    g_orig_edges.resize(g_N - 1);
    for (int i = 0; i < g_N - 1; i++) {
        fin >> g_orig_edges[i].first >> g_orig_edges[i].second;
        int u = g_orig_edges[i].first, v = g_orig_edges[i].second;
        g_orig_adj[u].insert(v);
        g_orig_adj[v].insert(u);
    }
    fin.close();

    // Phase 1: Run Alice
    alice(g_N, g_preorder);
    if (!g_alice_set) {
        fprintf(stderr, "PROTOCOL ERROR: Alice did not call setTree()\n");
        // stdout: machine-readable result line
        printf("%-28s  ER  %5s  %5s  %-7s  %6.2f\n", test_file, "-", "-", "-", 0.0);
        return 1;
    }

    // Determine starting nodes
    vector<int> starts;
    if (specific_start > 0) {
        starts.push_back(specific_start);
    } else if (all_starts) {
        for (int i = 1; i <= g_K; i++) starts.push_back(i);
    } else {
        srand(42);
        starts.push_back(rand() % g_K + 1);
    }

    // Build original edge set for verification
    set<pair<int,int>> orig_set;
    for (auto [u, v] : g_orig_edges) {
        orig_set.insert({min(u, v), max(u, v)});
    }

    int pass_count = 0, fail_count = 0;
    int max_queries = 0;

    // Phase 2: Run Bob for each starting node
    for (int s : starts) {
        g_start_node = s;
        g_current_node = -1;
        g_query_count = 0;
        g_move_count = 0;
        g_explored = false;
        g_answered = false;
        g_bob_answer.clear();

        bob(g_N);

        max_queries = max(max_queries, g_query_count);

        if (!g_answered) {
            if (g_verbose)
                fprintf(stderr, "  [start=%d] PROTOCOL ERROR: Bob did not call answer()\n", s);
            fail_count++;
            continue;
        }

        if ((int)g_bob_answer.size() != g_N - 1) {
            if (g_verbose)
                fprintf(stderr, "  [start=%d] WRONG ANSWER: %d edges (expected %d)\n",
                        s, (int)g_bob_answer.size(), g_N - 1);
            fail_count++;
            continue;
        }

        set<pair<int,int>> ans_set;
        for (auto [u, v] : g_bob_answer) {
            ans_set.insert({min(u, v), max(u, v)});
        }

        if (ans_set == orig_set) {
            if (g_verbose)
                fprintf(stderr, "  [start=%d] ACCEPTED (Q=%d, M=%d)\n",
                        s, g_query_count, g_move_count);
            pass_count++;
        } else {
            if (g_verbose) {
                fprintf(stderr, "  [start=%d] WRONG ANSWER (Q=%d, M=%d)\n",
                        s, g_query_count, g_move_count);
                for (auto e : orig_set)
                    if (!ans_set.count(e))
                        fprintf(stderr, "    Missing: (%d,%d)\n", e.first, e.second);
                for (auto e : ans_set)
                    if (!orig_set.count(e))
                        fprintf(stderr, "    Extra: (%d,%d)\n", e.first, e.second);
            }
            fail_count++;
        }
    }

    // ============ Scoring ============
    // score = 100 * min(1, sqrt(150/K)) * min(1, (400/Q)^1.5)

    bool all_correct = (fail_count == 0);
    double score = 0.0;
    const char* verdict = "WA";

    if (all_correct) {
        double k_factor = min(1.0, sqrt(150.0 / g_K));
        double q_factor = min(1.0, pow(400.0 / max_queries, 1.5));
        score = 100.0 * k_factor * q_factor;
        verdict = (score >= 99.999) ? "AC" : "OK";
    }

    // stdout: one clean machine-readable line
    char starts_buf[32];
    snprintf(starts_buf, sizeof(starts_buf), "%d/%d", pass_count, (int)starts.size());
    printf("%-28s  %-2s  %5d  %5d  %-7s  %6.2f\n",
           test_file, verdict, g_K, max_queries, starts_buf, score);

    return (score >= 99.999) ? 0 : 1;
}
