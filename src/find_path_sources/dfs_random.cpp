#include "find_path_headers/dfs_random.hpp"
#include "find_path_headers/bfs.hpp"
#include <vector>
#include <algorithm>
#include <random>
#include <stack>

// Static RNG for better performance (avoids frequent re-seeding)
static std::mt19937 rng(std::random_device{}());

bool dfs_path(const Graph& graph, int source, int sink, Path& path, FFStats* stats) {
    const int n = graph.size();
    bfs_state::reset(n);

    std::vector<std::pair<int, int>> parent(n, {-1, -1});
    std::stack<int> s;
    s.push(source);
    bfs_state::visited[source] = bfs_state::visitedToken;

    // Pre-allocate order vector to avoid reallocations
    std::vector<int> order;
    order.reserve(64); // Typical max degree for many graphs

    // Statistics counters
    int visited_nodes = 1;
    int visited_arcs_residual = 0;
    int visited_arcs_forward = 0;

    while (!s.empty()) {
        const int u = s.top();
        s.pop();
    
        const auto& neighbors = graph.get_neighbors(u);
        const int degree = neighbors.size();

        // Optimized shuffling for current node
        order.resize(degree);
        std::iota(order.begin(), order.end(), 0);
        std::shuffle(order.begin(), order.end(), rng);
    
        for (const int i : order) {
            const Edge& e = neighbors[i];
            const int residual = e.capacity - e.flow;

            if (residual > 0) {  // More efficient than method call
                visited_arcs_residual++;
                if (e.capacity > 0) visited_arcs_forward++;

                if (bfs_state::visited[e.to] != bfs_state::visitedToken) {
                    bfs_state::visited[e.to] = bfs_state::visitedToken;
                    parent[e.to] = {u, i};
                    s.push(e.to);
                    visited_nodes++;

                    if (e.to == sink) {
                        // Early path reconstruction
                        path.clear();
                        for (int v = sink; v != source; v = parent[v].first) {
                            path.emplace_back(parent[v].first, parent[v].second);
                        }
                        std::reverse(path.begin(), path.end());

                        stats->visited_nodes_per_iter.push_back(visited_nodes);
                        stats->visited_forward_arcs_per_iter.push_back(visited_arcs_forward);
                        stats->visited_residual_arcs_per_iter.push_back(visited_arcs_residual);
                        return true;
                    }
                }
            }
        }
    }

    stats->visited_nodes_per_iter.push_back(visited_nodes);
    stats->visited_forward_arcs_per_iter.push_back(visited_arcs_forward);
    stats->visited_residual_arcs_per_iter.push_back(visited_arcs_residual);
    return false;
}