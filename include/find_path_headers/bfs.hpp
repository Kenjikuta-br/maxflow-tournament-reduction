#pragma once

#include "graph.hpp"
#include "ford_fulkerson.hpp" 
#include <vector>
#include <cstdint>  // for uint64_t

using Path = std::vector<std::pair<int, int>>;

namespace bfs_state {
    // Reusable visited array for BFS across calls
    inline std::vector<uint64_t> visited;

    // Token to avoid clearing the visited array every time
    inline uint64_t visitedToken = 1;

    // Ensures visited array is large enough and increments the token
    inline void reset(int n) {
        if (visited.size() < static_cast<size_t>(n)) visited.assign(n, 0);
        visitedToken++;
    }
} // namespace bfs_state

// Finds an s-t augmenting path using BFS.
// Returns true if a valid path was found, storing it in `path`.
bool bfs_path(const Graph& graph, int s, int t, Path& path, FFStats* stats);