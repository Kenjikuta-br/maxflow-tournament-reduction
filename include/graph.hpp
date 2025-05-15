#pragma once

#include <vector>
#include <iostream>
#include <string>

// Represents a directed edge with a reverse edge in the residual graph
struct Edge {
    int id;       // Unique ID (forward >=0, reverse = -1)
    int to;       // Destination node
    int from;     // Source node (for debugging and utilities)
    int rev;      // Index of reverse edge in the destination's adjacency list
    int capacity; // Max capacity of the edge
    int flow;     // Current flow through this edge

    // True if this edge is a residual edge (no capacity)
    bool is_residual() const {
        return capacity == 0;
    }

    // Residual capacity = capacity - flow
    int remaining_capacity() const {
        return capacity - flow;
    }

    // Increases flow and decreases the reverse edge flow
    void augment(int bottleneck, Edge& reverse_edge) {
        flow += bottleneck;
        reverse_edge.flow -= bottleneck;
    }

    // Debug helper to show edge information
    std::string to_string(int s = -1, int t = -1) const {
        std::string u = (from == s ? "s" : (from == t ? "t" : std::to_string(from)));
        std::string v = (to   == s ? "s" : (to   == t ? "t" : std::to_string(to)));
        return "Edge " + u + " -> " + v +
               " | flow = " + std::to_string(flow) +
               " | capacity = " + std::to_string(capacity) +
               " | is residual: " + (is_residual() ? "true" : "false");
    }
};

class Graph {
public:
    Graph(int n = 0); // Initialize a graph with n nodes

    void add_edge(int from, int to, int capacity);         // Adds forward and reverse edge
    void read_dimacs(std::istream& in);                    // Load graph in DIMACS format
    void compress_graph();
    void print_residual_graph() const;

    // Basic accessors
    int size() const;                                      // Number of vertices
    int num_vertices() const;                              // Alias for size()
    int num_edges() const;                                 // Number of forward edges (original)
    int num_edges_residual() const;                        // Number of forward edges (residual)
    int get_min_edge_id() const;
    int total_out_capacity(int s) const;                   // Sum of capacities out of node s

    const std::vector<Edge>& adj(int u) const;             // Read-only access to adjacents
    std::vector<std::vector<Edge>>& get_adj();             // Writable adjacency list
    const std::vector<Edge>& get_neighbors(int u) const;   // Alias to adj()

    int get_source() const;                                // Source node index
    int get_sink() const;                                  // Sink node index

private:
    int n;                                                 // Number of nodes
    int source = -1, sink = -1;                            // Source/sink indices (initialized to -1)
    int next_edge_id = 0;
    int next_edge_id_reverse = 0;
    int next_edge_id_aux = 0;
    std::vector<std::vector<Edge>> adj_list;               // Adjacency list
};
