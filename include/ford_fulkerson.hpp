#pragma once
#include "graph.hpp"
#include <functional>
#include <vector>

// Each pair represents an edge used in the path: (from_node, index of edge in graph[from_node])
using Path = std::vector<std::pair<int, int>>;

// Available path-finding strategies
enum class AlgorithmType { DFS_RANDOM, BFS_EDMONDS_KARP, FATTEST_PATH, CAPACITY_SCALING };

// Struct to collect algorithm-level statistics
struct FFStats {
    int iterations = 0;       // number of augmenting paths found
    double bound = 0.0;       // theoretical upper bound on iterations
    double r = 0.0;           // ratio iterations / bound
    int m = 0.0;                            // number of forward edges, will be initialized after read the graph
	int n = 0.0;

    // === Critical metrics, specific of Edmons-Karp (BFS)===
    int m_residual = 0.0;                   // number of residual edges, will be initialized after read the graph
    std::vector<int> criticalCount;        // counter C for each edge
    double cFrac = 0.0;                    // fraction of edges that were critical at least once (C)
    double rBar = 0.0;                     // average criticality among critical edges (r̄)
    int offset = 0;                        // auxiliar to calculate offset because we have negative ids (backward edges) and positive ids (forward edges)

    // === Time metrics ===
    double total_runtime = 0.0; // Total execution time in seconds (T(n, m))

    // Time normalized by metrics from the literature 
    double time_per_mI = 0.0;   // T / (m * I) — FF-type metric: time per arc per iteration
    double time_per_nI = 0.0;   // T / (n * I) — FF-type metric: time per vertex per iteration

    // Time normalized by 
    double time_over_nm = 0.0;  // T / (nm)

    // Time normalized by node/arc exploration (operation-based complexity)
    double time_over_I_sntm = 0.0;       // T / [I * (s̄ * n + t̄ * m)]


    // === Search behavior metrics ===
    std::vector<int> visited_nodes_per_iter;   // number of nodes visited in each augmenting path search
    std::vector<int> visited_forward_arcs_per_iter; // number of forward arcs inspected in each augmenting path search
    std::vector<int> visited_residual_arcs_per_iter;    // number of residual arcs inspected in each augmenting path search
    
    double s_bar = 0.0;                    // average fraction of nodes visited per iteration
    double t_bar_forward = 0.0;                    // average fraction of arcs inspected per iteration, only forward arcs
    double t_bar_residual = 0.0;           // average fraction of arcs inspected per iteration, all arcs in residual graph

    // === Heap operation counters (used only in fattest path) ===
    std::vector<int> heap_real_inserts_per_iter;      // number of real heap inserts per iteration (first time a node is inserted)
    std::vector<int> heap_implicit_updates_per_iter;  // number of implicit updates (reinsertions) per iteration
    std::vector<int> heap_total_inserts_per_iter;     // total heap insertions per iteration (real + updates)
    std::vector<int> heap_deleteMins_per_iter;        // number of deleteMin (pop) operations per iteration

    double avg_insert_normalized = 0.0;    // average insertions normalized by n
    double avg_delete_normalized = 0.0;    // average deletions normalized by n
    double avg_update_normalized_m = 0.0;  // average updates normalized by m
    double avg_update_normalized_theoretical = 0.0; // normalized updates by expected value ( (α−1)n ln n for m = n^α )

	// === Aux for capacity scaling ===
	bool max_cap_initialized = false;
	int delta;
	int max_cap;
};


// Type for a path-finding strategy function
using PathFindingStrategy = std::function<bool(const Graph&, int s, int t, Path& path, FFStats* stats)>;

// Runs Ford-Fulkerson algorithm using a given strategy.
// Returns the maximum flow value and optionally stores flow path stats.
int ford_fulkerson(Graph& graph, int s, int t, PathFindingStrategy find_path, AlgorithmType type, FFStats* stats);
