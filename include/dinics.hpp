#pragma once

#include "graph.hpp"

// Returns the max flow from source to sink using Dinic's algorithm
int dinic_max_flow(Graph& graph, int source, int sink);