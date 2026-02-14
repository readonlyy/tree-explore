#ifndef GRADER_H
#define GRADER_H

#include <vector>
#include <utility>

// === Alice's interface ===
// Alice must implement this function.
// Receives N and the preorder traversal P.
// Must call setTree() exactly once to output the intermediate tree.
void alice(int N, std::vector<int> P);

// Alice calls this to set her intermediate tree.
//   K: number of nodes (1 <= K <= 150), labeled 1..K
//   edges: K-1 edges forming a tree
void setTree(int K, std::vector<std::pair<int,int>> edges);

// === Bob's interface ===
// Bob must implement this function.
// Receives N (number of nodes in the original tree).
// Must call answer() exactly once to output the reconstructed tree.
void bob(int N);

// Bob calls these:

// Query: is there an edge (u,v) in the original tree with u in A and v in B?
// A and B must be disjoint. Returns 1 if yes, 0 if no. Max 400 calls.
int ask(std::vector<int> A, std::vector<int> B);

// Start exploration of Alice's intermediate tree.
// Returns {starting_node, list_of_neighbors}. Call at most once.
// Must be called after all ask() calls.
std::pair<int, std::vector<int>> doExplore();

// Move to an adjacent node in Alice's tree.
// Returns {node, list_of_neighbors}. Max 5 calls. Must call doExplore() first.
std::pair<int, std::vector<int>> doMove(int target);

// Output the reconstructed tree (N-1 edges). Call exactly once.
void answer(std::vector<std::pair<int,int>> edges);

#endif
