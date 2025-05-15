#include "dinics.hpp"
#include <vector>
#include <queue>
#include <algorithm>

// Builds level graph using BFS from source
bool bfs_level(const Graph& graph, std::vector<int>& level, int s, int t) {
    int n = graph.size();
    level.assign(n, -1);
    level[s] = 0;

    std::queue<int> q;
    q.push(s);

    while (!q.empty()) {
        int u = q.front(); q.pop();

        for (const Edge& e : graph.get_neighbors(u)) {
            // Only consider forward edges with remaining capacity
            if (e.capacity - e.flow > 0 && level[e.to] == -1) {
                level[e.to] = level[u] + 1;
                q.push(e.to);
            }
        }
    }

    // If sink is unreachable, return false
    return level[t] != -1;
}

// DFS to send flow through level graph
int dfs_flow(Graph& graph, std::vector<int>& level, std::vector<int>& ptr, int u, int t, int pushed) {
    if (pushed == 0) return 0;
    if (u == t) return pushed;

    auto& adj = graph.get_adj();

    // Try all edges starting from ptr[u]
    for (int& cid = ptr[u]; cid < (int)adj[u].size(); ++cid) {
        Edge& e = adj[u][cid];
        // Follow only valid level edges with capacity
        if (level[e.to] != level[u] + 1 || e.capacity - e.flow <= 0) continue;

        int tr = dfs_flow(graph, level, ptr, e.to, t, std::min(pushed, e.capacity - e.flow));
        if (tr == 0) continue;

        // Push flow and update reverse edge
        e.flow += tr;
        adj[e.to][e.rev].flow -= tr;
        return tr;
    }

    return 0;
}

// Computes max flow using Dinic's algorithm
int dinic_max_flow(Graph& graph, int s, int t) {
    int flow = 0;
    std::vector<int> level;
    std::vector<int> ptr;

    // Rebuild level graph and find blocking flow repeatedly
    while (bfs_level(graph, level, s, t)) {
        ptr.assign(graph.size(), 0);

        while (int pushed = dfs_flow(graph, level, ptr, s, t, 1e9)) {
            flow += pushed;
        }
    }

    return flow;
}
