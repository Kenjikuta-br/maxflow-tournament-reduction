#include "ford_fulkerson.hpp"
#include <algorithm>
#include <climits>
#include <cmath>
#include <limits>
#include <chrono>

// forward declarations of helpers
void computeCriticalStats(const Graph& graph, FFStats* stats);
void populateStats(FFStats* stats,const Graph& graph,int s,int iterations,AlgorithmType type);


// Augments the flow along a valid s-t path.
// Returns how much flow was added (bottleneck capacity).
int augment(Graph& graph, const Path& path, FFStats* stats) {
    int bottleneck = INT_MAX;
    auto& adj = graph.get_adj(); // Reference to adjacency list for flow updates

    // Find the bottleneck: minimum residual capacity in the path
    for (const auto& [u, idx] : path) {
        const Edge& e = adj[u][idx];
        bottleneck = std::min(bottleneck, e.remaining_capacity()); // using method
    }

    // conta *quais* arcos do path serão saturated agora
    for (auto [u, idx] : path) {
        auto& e = adj[u][idx];
        if (e.remaining_capacity() == bottleneck) {
            int idx = e.id + stats->offset;
            ++stats->criticalCount[idx];
        }
    }

    // Apply the bottleneck flow to the path
    for (const auto& [u, idx] : path) {
        Edge& e = adj[u][idx];
        Edge& rev = adj[e.to][e.rev];
        e.augment(bottleneck, rev); // using method
    }

    return bottleneck;
}

// Repeatedly finds augmenting paths and applies flow until none remain
int ford_fulkerson(Graph& graph, int s, int t, PathFindingStrategy find_path, AlgorithmType type, FFStats* stats) {
    int max_flow = 0;
    Path path;
    int iterations = 0;

	stats->n = graph.num_vertices();
    stats->m = graph.num_edges();
    stats->m_residual = graph.num_edges_residual();

    // Resize the vector to contain a counter per arc, initialized to zero.
    stats->criticalCount.resize(stats->m_residual, 0);
    stats->offset = -graph.get_min_edge_id(); 

    // Start timer
    auto start = std::chrono::high_resolution_clock::now();

    // Main loop: search-augment-repeat
    while (find_path(graph, s, t, path, stats)) {
        ++iterations;
        max_flow += augment(graph, path, stats);
        //std::cerr << "Current max flow: " << max_flow << std::endl;
    }

    if(type == AlgorithmType::BFS_EDMONDS_KARP){
        computeCriticalStats(graph, stats);
    }else{
        // Don't do anything cause only computeCriticalStats fi Edmonds-Karp
    }

    // End measuring time
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    stats->total_runtime = elapsed.count(); // tempo total em segundos (double)


    // Populate algorithm-specific stats
    populateStats(stats, graph, s, iterations, type);

    return max_flow;
}


// Helper function to compute critical edge statistics (C_frac and r_bar)
void computeCriticalStats(const Graph& graph, FFStats* stats) {
    int n = graph.size();
    double half_n = n / 2.0;
    int m = stats->criticalCount.size();

    int saturated = 0;
    double soma_ra = 0;
    for (int a = 0; a < m; ++a) {
        if (stats->criticalCount[a] > 0) {
            ++saturated;
            soma_ra += (stats->criticalCount[a] / half_n);   // r_a = C_a / (n/2)
        }
    }
    stats->cFrac = double(saturated) / m;          // fraction of critical edges
    stats->rBar  = (saturated > 0) ? soma_ra / saturated : 0.0; // saturated = C_frac * m = C x m
}

// Helper function to populate algorithm-specific stats
void populateStats(FFStats* stats, const Graph& graph, int s, int iterations, AlgorithmType type) {

    stats->iterations = iterations;
    stats->n = graph.size();
    stats->m = graph.num_edges();
    stats->m_residual= graph.num_edges_residual();       
    int C = graph.total_out_capacity(s); 

    // ----------------------
    // Compute theoretical upper bound on number of iterations
    // based on the selected algorithm type
    // ----------------------
    switch (type) {
        case AlgorithmType::DFS_RANDOM:
            stats->bound = static_cast<double>(C);
            break;
        case AlgorithmType::BFS_EDMONDS_KARP:
            stats->bound = static_cast<double>(stats->n) * stats->m_residual / 2.0;
            break;
        case AlgorithmType::FATTEST_PATH:
            stats->bound = stats->m_residual * std::log2(std::max(1, C));
            break;
        case AlgorithmType::CAPACITY_SCALING:
            stats->bound = stats->m_residual * std::log2(std::max(1, C));
            break;
    }

    // ----------------------
    // Compute r = iterations / theoretical bound
    // ----------------------
    stats->r = (stats->bound > 0)
        ? static_cast<double>(iterations) / stats->bound
        : std::numeric_limits<double>::infinity();

   
    // ----------------------
    // Compute average fraction of visited nodes and arcs per iteration:
    // s̄ = (1/I) Σ (visited_nodes / n)
    // t̄ = (1/I) Σ (visited_arcs / m)
    // ----------------------
    double s_bar = 0, t_bar_forward = 0, t_bar_residual = 0;
    for (int i = 0; i < stats->iterations; ++i) {
        s_bar += static_cast<double>(stats->visited_nodes_per_iter[i]) / stats->n;
        t_bar_forward += static_cast<double>(stats->visited_forward_arcs_per_iter[i]) / stats->m;
        t_bar_residual += static_cast<double>(stats->visited_residual_arcs_per_iter[i]) / stats->m_residual;
    }
    s_bar /= stats->iterations;
    t_bar_forward /= stats->iterations;
    t_bar_residual /= stats->iterations;

    stats->s_bar = s_bar;
    stats->t_bar_forward = t_bar_forward;
    stats->t_bar_residual = t_bar_residual;

    // ----------------------
    // Heap-related metrics (only for Fattest Path)
    // Includes normalized insertions, deletions, and updates
    // ----------------------
    if (type == AlgorithmType::FATTEST_PATH) {
        int sum_inserts = 0, sum_deletes = 0, sum_updates = 0;
        for (int i = 0; i < stats->iterations; ++i) {
            sum_inserts += stats->heap_real_inserts_per_iter[i];
            sum_deletes += stats->heap_deleteMins_per_iter[i];
            sum_updates += stats->heap_implicit_updates_per_iter[i];
        }

        stats->avg_insert_normalized = static_cast<double>(sum_inserts) / (stats->n * stats->iterations);
        stats->avg_delete_normalized = static_cast<double>(sum_deletes) / (stats->n * stats->iterations);
        stats->avg_update_normalized_m = static_cast<double>(sum_updates) / (stats->m * stats->iterations);

        // Estimate α using m ≈ n^α → α = log_m(n)
        double alpha = std::log(static_cast<double>(stats->m)) / std::log(static_cast<double>(stats->n));
        double expected_updates = (alpha - 1.0) * stats->n * std::log(static_cast<double>(stats->n));
        stats->avg_update_normalized_theoretical = expected_updates > 0.0
            ? static_cast<double>(sum_updates) / (expected_updates * stats->iterations)
            : 0.0;

    } else {
        // Set to -1 if not FATTEST_PATH
        stats->avg_insert_normalized = -1.0;
        stats->avg_delete_normalized = -1.0;
        stats->avg_update_normalized_m = -1.0;
        stats->avg_update_normalized_theoretical = -1.0;
    }

    // ----------------------
    // Time complexity measurements from experimental test plan
    // ----------------------
    double T = stats->total_runtime;   // Total execution time in seconds
    double I = static_cast<double>(stats->iterations);  // Number of iterations

    // Time per arc per iteration: T / (m * I)
    stats->time_per_mI = (stats->m_residual > 0 && I > 0) ? T / (stats->m_residual * I) : 0.0;

    // Time per vertex per iteration: T / (n * I)
    stats->time_per_nI = (stats->n > 0 && I > 0) ? T / (stats->n * I) : 0.0;

    // Time normalized by p: T / (nm)
    stats->time_over_nm = (stats->n > 0 && stats->m_residual > 0) 
        ? T / (stats->n * stats->m_residual) : 0.0;

    // Time normalized by iteration complexity: T / [I * (s̄ * n + t̄ * m)]
    stats->time_over_I_sntm = (I > 0 && stats->n > 0 && stats->m_residual > 0)
        ? T / (I * (stats->s_bar * stats->n + stats->t_bar_residual * stats->m_residual)) : 0.0;

}