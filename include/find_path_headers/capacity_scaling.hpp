#pragma once

#include "graph.hpp"
#include "ford_fulkerson.hpp" // for FFStats
#include <vector>
#include <cstdint>

// Path format: (from_node, edge_index)
using Path = std::vector<std::pair<int, int>>;

// Finds an s-t augmenting path using capacity scaling DFS.
// Only explores edges with residual capacity >= current delta threshold.
// Compatible with Ford-Fulkerson signature.
bool capacity_scaling_path(const Graph& graph, int s, int t, Path& path, FFStats* stats);
