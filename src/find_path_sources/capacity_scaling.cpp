#include "find_path_headers/capacity_scaling.hpp"
#include <queue>
#include <stack>
#include <limits>
#include <algorithm>
#include <cmath>

namespace {
    std::vector<uint64_t> visited;
    uint64_t visitedToken = 1;

    void reset(size_t n) {
        if (visited.size() < n) visited.assign(n, 0);
        visitedToken++;
    }
}

static bool dfs_with_delta(const Graph& graph, int s, int t, Path& path, int delta, FFStats* stats) {
    int n = graph.size();
    std::vector<std::pair<int, int>> parent(n, {-1, -1});
    std::stack<int> st;
    st.push(s);
    visited[s] = visitedToken;

    int visited_nodes = 1;
    int visited_arcs_residual = 0;
    int visited_arcs_forward = 0;
    bool found = false;

    while (!st.empty() && !found) {
        int u = st.top();
        st.pop();

        const auto& neighbors = graph.get_neighbors(u);
        for (size_t i = 0; i < neighbors.size() && !found; ++i) {
            const Edge& e = neighbors[i];
            int residual = e.remaining_capacity();
            if (residual >= delta) {
                if (e.capacity > 0) {
                    ++visited_arcs_forward;
                }
                ++visited_arcs_residual;
                
                if (visited[e.to] != visitedToken) {
                    visited[e.to] = visitedToken;
                    parent[e.to] = {u, static_cast<int>(i)};
                    st.push(e.to);
                    ++visited_nodes;
                    if (e.to == t) {
                        found = true;
                    }
                }
            }
        }
    }

    stats->visited_nodes_per_iter.push_back(visited_nodes);
    stats->visited_forward_arcs_per_iter.push_back(visited_arcs_forward);
    stats->visited_residual_arcs_per_iter.push_back(visited_arcs_residual);

    if (!found) return false;

    // Reconstruct path
    path.clear();
    for (int u = t; u != s; u = parent[u].first) {
        int prev = parent[u].first;
        int idx = parent[u].second;
        path.push_back({prev, idx});
    }
    std::reverse(path.begin(), path.end());
    return true;
}

bool capacity_scaling_path(const Graph& graph, int s, int t, Path& path, FFStats* stats) {
    // Initialize max capacity and delta only once
    if (!stats->max_cap_initialized) {
        stats->max_cap = 0;
        for (int u = 0; u < graph.size(); ++u) {
            for (const Edge& e : graph.get_neighbors(u)) {
                stats->max_cap = std::max(stats->max_cap, e.remaining_capacity());
            }
        }
        
        // Calculate initial delta as the highest power of 2 <= max_cap
        if (stats->max_cap > 0) {
            stats->delta = 1 << (31 - __builtin_clz(stats->max_cap));
        } else {
            stats->delta = 0;
        }
        
        stats->max_cap_initialized = true;
    }

    // Early exit if no capacity remains
    if (stats->max_cap == 0) return false;

    // Textbook approach: No early exit based on max_cap
    while (stats->delta > 0) {
        reset(graph.size());
        if (dfs_with_delta(graph, s, t, path, stats->delta, stats)) {
            // Key change: Skip max_cap recomputation entirely
            return true;
        }
        // Always halve delta (no max_cap checks)
        stats->delta >>= 1;
    }
    return false;
}