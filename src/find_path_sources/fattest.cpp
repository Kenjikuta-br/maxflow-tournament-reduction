#include "find_path_headers/fattest.hpp"
#include "find_path_headers/bfs.hpp" // for bfs_state utilities
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>

// Used in priority queue: stores node and its bottleneck capacity so far
struct State {
    int cap;
    int node;
    bool operator<(const State& other) const {
        return cap < other.cap; // max-heap by bottleneck
    }
};

bool fattest_path(const Graph& graph, int s, int t, Path& path, FFStats* stats) {
    int n = graph.size();
    bfs_state::reset(n);  // reset visited array using visitedToken trick

    std::vector<int> max_cap(n, 0); // max bottleneck capacity to each node
    std::vector<std::pair<int, int>> parent(n, {-1, -1}); // (prev_node, edge_index)

    int real_inserts = 0;
    int total_inserts = 0;
    int deleteMins = 0;
    int visited_nodes = 0; 
    int visited_arcs_residual = 0;
    int visited_arcs_forward =0;


    std::priority_queue<State> pq;
    pq.push({std::numeric_limits<int>::max(), s});
    max_cap[s] = std::numeric_limits<int>::max();
    ++total_inserts;
    ++real_inserts;

    while (!pq.empty()) {
        State state = pq.top();
        pq.pop();
        int u = state.node;
        ++deleteMins;

        // Skip node if already visited in this round
        if (bfs_state::visited[u] == bfs_state::visitedToken) continue;
        bfs_state::visited[u] = bfs_state::visitedToken;
        ++visited_nodes;

		if (u == t) {
			stats->visited_nodes_per_iter.push_back(visited_nodes);
			stats->visited_forward_arcs_per_iter.push_back(visited_arcs_forward);
			stats->visited_residual_arcs_per_iter.push_back(visited_arcs_residual);
			stats->heap_real_inserts_per_iter.push_back(real_inserts);
			stats->heap_total_inserts_per_iter.push_back(total_inserts);
			stats->heap_implicit_updates_per_iter.push_back(total_inserts - real_inserts);
			stats->heap_deleteMins_per_iter.push_back(deleteMins);
			break; // Found t with fattest path, can stop early
		}

        const auto& neighbors = graph.get_neighbors(u);
        for (size_t i = 0; i < neighbors.size(); ++i) {
            const Edge& e = neighbors[i];
            if(e.capacity > 0){
                ++visited_arcs_forward;
            }

            ++visited_arcs_residual;

            int residual = e.remaining_capacity();
            if (residual <= 0) continue;

            int cap = std::min(max_cap[u], residual);
            if (cap > max_cap[e.to] && bfs_state::visited[e.to] != bfs_state::visitedToken) {
				if (max_cap[e.to] == 0) { 
					++real_inserts; // First time seeing this node
				}
                max_cap[e.to] = cap;
                parent[e.to] = {u, static_cast<int>(i)};
                pq.push({cap, e.to});
                ++total_inserts;
	
            }
        }
    }

    stats->visited_nodes_per_iter.push_back(visited_nodes);
    stats->visited_forward_arcs_per_iter.push_back(visited_arcs_forward);
    stats->visited_residual_arcs_per_iter.push_back(visited_arcs_residual);
    stats->heap_real_inserts_per_iter.push_back(real_inserts);
    stats->heap_total_inserts_per_iter.push_back(total_inserts);
    stats->heap_implicit_updates_per_iter.push_back(total_inserts - real_inserts);
    stats->heap_deleteMins_per_iter.push_back(deleteMins);


    if (max_cap[t] == 0){
        return false;
    }

    // Reconstruct augmenting path from t to s
    path.clear();
    for (int u = t; u != s; u = parent[u].first) {
        int prev = parent[u].first;
        int idx = parent[u].second;
        path.push_back({prev, idx});
    }

    std::reverse(path.begin(), path.end());

    return true;
}
