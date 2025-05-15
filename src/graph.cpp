// graph.cpp
#include "graph.hpp"
#include <sstream>
#include <stdexcept>
#include <iostream> //debug for residual graph
#include <map>
#include <set>
#include <limits>


// Constructor that initializes the graph with n nodes
Graph::Graph(int n) : n(n), adj_list(n) {}

// Adds a forward and reverse edge (for residual graph) between 'from' and 'to'
void Graph::add_edge(int from, int to, int capacity) {
    int fid = next_edge_id++;   // ID for forward edge
    int rid = next_edge_id_reverse--;
    // Forward edge: from → to with given capacity
    Edge forward = {fid, to, from, static_cast<int>(adj_list[to].size()), capacity, 0};
    // Reverse edge: to → from with 0 capacity (initially) and fid = -1 to sinalize it is a backward edge, used for flow cancellation
    Edge backward = {rid, from, to, static_cast<int>(adj_list[from].size()), 0, 0};

    adj_list[from].push_back(forward);
    adj_list[to].push_back(backward);
}

int Graph::size() const {
    return n;
}

int Graph::num_vertices() const {
    return size();
}

// Count only edges with positive capacity (original forward edges)
int Graph::num_edges() const {
    int count = 0;
    for (const auto& vec : adj_list) {
        for (const Edge& e : vec) {
            if (e.capacity > 0) ++count;
        }
    }
    return count;
}

int Graph::num_edges_residual() const {
    int count = 0;
    for (const auto& neighbors : adj_list) {
        for ([[maybe_unused]]const Edge& e : neighbors) {
            count++;
        }
    }
    return count;
}

int Graph::get_min_edge_id() const {
    int min_id = std::numeric_limits<int>::max();
    for (const auto& neighbors : adj_list) {
        for (const Edge& e : neighbors) {
            min_id = std::min(min_id, e.id);
        }
    }
    return min_id;
}

// Sum of capacities of all outgoing edges from node s
int Graph::total_out_capacity(int s) const {
    int sum = 0;
    for ( const Edge& e : adj_list[s]) {
        sum += e.capacity;
    }
    return sum;
}

// Returns the adjacency list of a given node
const std::vector<Edge>& Graph::adj(int u) const {
    return adj_list[u];
}

// Returns the list of edges from node u (same as adj)
const std::vector<Edge>& Graph::get_neighbors(int u) const {
    return adj_list[u];
}

// Returns the full adjacency list for modifying flows
std::vector<std::vector<Edge>>& Graph::get_adj() {
    return adj_list;
}

int Graph::get_source() const {
    return source;
}

int Graph::get_sink() const {
    return sink;
}

// Reads a graph in DIMACS max-flow format from an input stream
void Graph::read_dimacs(std::istream& in) {
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == 'c') continue; // Skip comments

        std::istringstream iss(line);
        char type;
        iss >> type;

        if (type == 'p') {
            std::string problem_type;
            int nodes, arcs;
            iss >> problem_type >> nodes >> arcs;
            if (problem_type != "max") {
                throw std::runtime_error("Only 'max' problems are supported");
            }
            n = nodes;
            adj_list.assign(n, {});
        } else if (type == 'n') {
            int id;
            char role;
            iss >> id >> role;
            if (role == 's') source = id - 1;
            if (role == 't') sink = id - 1;
        } else if (type == 'a') {
            int from, to, cap;
            iss >> from >> to >> cap;
            add_edge(from - 1, to - 1, cap);
            //std::cout << "Adding edge: " << from - 1 << " -> " << to - 1 << " with capacity " << cap << std::endl;

        }
    }



    if (source == -1 || sink == -1) {
        throw std::runtime_error("Source or sink node not defined");
    }

    compress_graph();

    // Imprime o grafo residual no terminal
    // print_residual_graph();
}


void Graph::compress_graph() {
    // Mapa para armazenar capacidades de ida e volta entre pares (u, v)
    std::map<std::pair<int, int>, int> forward_capacity;

    for (int u = 0; u < n; ++u) {
        for (const Edge& e : adj_list[u]) {
            if (e.capacity > 0) {
                forward_capacity[{u, e.to}] += e.capacity;
            }
        }
    }

    // Nova lista de adjacência consolidada
    std::vector<std::vector<Edge>> new_adj(n);
    std::set<std::pair<int, int>> handled;

    for (const auto& [pair, cap_uv] : forward_capacity) {
        int u = pair.first;
        int v = pair.second;

        if (handled.count({u, v}) || handled.count({v, u})) continue;
        handled.insert({u, v});
        handled.insert({v, u});

        int cap_uv_final = forward_capacity[{u, v}];
        int cap_vu_final = forward_capacity[{v, u}];

        // Criação de uma nova aresta forward (u → v)
        int fid = next_edge_id_aux++; // Cria um novo fid

        // u → v (forward)
        Edge forward = {fid, v, u, static_cast<int>(new_adj[v].size()), cap_uv_final, 0};
        new_adj[u].push_back(forward);

        // v → u (reverse)
        Edge backward = {-1, u, v, static_cast<int>(new_adj[u].size()) - 1, cap_vu_final, 0};
        new_adj[v].push_back(backward);
    }

    adj_list = std::move(new_adj);
}

// Imprime o grafo residual no terminal
void Graph::print_residual_graph() const {
    std::cout << "Residual Graph:\n";

    for (int u = 0; u < n; ++u) {
        for (const Edge& e : adj_list[u]) {
            // Mostra todas as arestas, inclusive reversas
            std::cout << "  " << u << " -> " << e.to
                      << " | cap: " << e.capacity
                      << " | flow: " << e.flow
                      << " | residual: " << (e.capacity - e.flow)
                      << "\n";
        }
    }
}