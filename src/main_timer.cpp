#include "graph.hpp"
#include "ford_fulkerson.hpp"
#include "dinics.hpp"
#include "find_path_headers/bfs.hpp"
#include "find_path_headers/dfs_random.hpp"
#include "find_path_headers/fattest.hpp"
#include "find_path_headers/capacity_scaling.hpp"


#include <iostream>
#include <string>
#include <chrono>

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    
     // 1. Constrói o grafo a partir do torneio
    Graph graph;
    graph.fromTournament(std::cin);


    // 2. Calcula fluxo máximo
    FFStats stats;
    int max_flow = ford_fulkerson(
        graph,
        graph.get_source(),
        graph.get_sink(),
        capacity_scaling_path,
        AlgorithmType::CAPACITY_SCALING,
        &stats
    );

    // 3. Verifica total de partidas restantes do time 1
    int total_remaining_games = graph.total_out_capacity(graph.get_source());

    (void)max_flow;
    (void)total_remaining_games;

        
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = end - start;
    std::cout << duration.count() << std::endl;

    return 0;
}