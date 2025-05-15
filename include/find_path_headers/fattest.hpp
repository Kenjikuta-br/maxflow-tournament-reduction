#pragma once

#include "graph.hpp"
#include "ford_fulkerson.hpp" // for FFStats
#include <vector>

// Stores the augmenting path as (from_node, edge_index) pairs
using Path = std::vector<std::pair<int, int>>;

/**
 * Finds an s-t path maximizing the bottleneck (minimum residual capacity along the path).
 * Returns true if such a path is found in the residual graph, false otherwise.
 */
bool fattest_path(const Graph& graph, int s, int t, Path& path, FFStats* stats);
