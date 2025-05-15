#pragma once

#include "graph.hpp"
#include "ford_fulkerson.hpp" // for FFStats
#include <vector>
#include <utility>

// Path is represented as a sequence of (prev_node, edge_index) pairs
using Path = std::vector<std::pair<int, int>>;

// Performs a randomized DFS from source to sink in the residual graph.
// Returns true if an s-t path is found and stores it in `path`.
bool dfs_path(const Graph& graph, int source, int sink, Path& path, FFStats* stats);
