#include "graph.hpp"
#include "ford_fulkerson.hpp"
#include "dinics.hpp"
#include "find_path_headers/bfs.hpp"
#include "find_path_headers/dfs_random.hpp"
#include "find_path_headers/fattest.hpp"
#include "find_path_headers/capacity_scaling.hpp"


#include <iostream>
#include <string>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

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



    // 4. Decide se o time 1 pode vencer

    if (graph.get_cant_be_champion()) {
        std::cout << "não\n";
    }else if(max_flow == total_remaining_games)
        std::cout << "sim\n";
    else
        std::cout << "não\n";

    return 0;
}