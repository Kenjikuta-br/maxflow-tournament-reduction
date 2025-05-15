#include "graph.hpp"
#include "ford_fulkerson.hpp"
#include "dinics.hpp"
#include "find_path_headers/bfs.hpp"
#include "find_path_headers/dfs_random.hpp"
#include "find_path_headers/fattest.hpp"
#include "find_path_headers/capacity_scaling.hpp"


#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    bool is_ford_fulkerson = true;
    bool is_cout_csv = false;

    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " bfs|dfs|fat|scaling|dinics < dimacs_graph\n";
        return 1;
    }

    std::string strategy_name = argv[1];
    PathFindingStrategy strategy = nullptr;
    AlgorithmType type;

    if (strategy_name == "bfs") {
        strategy = bfs_path;
        type = AlgorithmType::BFS_EDMONDS_KARP;
    } else if (strategy_name == "dfs") {
        strategy = dfs_path;
        type = AlgorithmType::DFS_RANDOM;
    } else if (strategy_name == "fat") {
        strategy = fattest_path;
        type = AlgorithmType::FATTEST_PATH;
    } else if (strategy_name == "scaling") {
        strategy = capacity_scaling_path;
        type = AlgorithmType::CAPACITY_SCALING;
    } else if (strategy_name == "dinics") {
        is_ford_fulkerson = false;
        //strategy = dinics_path;
        //std::cerr << "Ainda não implementado dinics\n";
    } else {
        std::cerr << "Estrategia inválida: " << strategy_name << ". Use bfs ou dfs.\n";
        return 1;
    }

    Graph graph;
    graph.read_dimacs(std::cin);


    int source = graph.get_source();
    int sink = graph.get_sink();
    int max_flow = -3;

    FFStats stats;

    if(is_ford_fulkerson){
        max_flow = ford_fulkerson(graph, source, sink, strategy, type, &stats);
    }else{ 
        max_flow = dinic_max_flow(graph, source, sink);
    }

    if(is_cout_csv){
        //csv header
        // max_flow; iterations; bound; r
        std::cout << max_flow << ";";
        std::cout << stats.iterations << ";";
        std::cout << stats.bound << ";";
        std::cout << stats.r << "\n";    

    }else{
        std::cout << "max flow:" << max_flow << "\n";
        std::cout << "iterations:" << stats.iterations << "\n";
        std::cout << "bound:" << stats.bound << "\n";
        std::cout << "r (iterations/max_iterations):" << stats.r << "\n";    
    }
   

    return 0;
}