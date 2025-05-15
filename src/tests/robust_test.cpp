#include "graph.hpp"
#include "ford_fulkerson.hpp"
#include "find_path_headers/bfs.hpp"
#include "find_path_headers/dfs_random.hpp"
#include "find_path_headers/fattest.hpp"
#include "find_path_headers/capacity_scaling.hpp"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <numeric> // for accumulate
#include <cmath> 

struct StrategyConfig {
    std::string name;
    PathFindingStrategy strategy;
    AlgorithmType type;
};

// Função para verificar se um número é inteiro
template <typename T>
bool is_integer(T value) {
    return std::floor(value) == value;
}

// Função para imprimir um número com a precisão condicional
template <typename T>
void print_with_precision(T value) {
    if (is_integer(value)) {
        std::cout << std::defaultfloat << value;
    } else {
        std::cout << std::fixed << std::setprecision(9) << value;
    }
}

int main() {
    const int num_repetitions = 5;

    std::vector<StrategyConfig> strategies = {
        {"bfs", bfs_path, AlgorithmType::BFS_EDMONDS_KARP},
        {"dfs", dfs_path, AlgorithmType::DFS_RANDOM},
        {"fat", fattest_path, AlgorithmType::FATTEST_PATH},
        {"scaling", capacity_scaling_path, AlgorithmType::CAPACITY_SCALING}
    };

    // Read the input graph once
    Graph original;
    original.read_dimacs(std::cin);
    int source = original.get_source();
    int sink   = original.get_sink();

    std::cerr << "source: " << source << ", sink: " << sink << "\n";

    std::cerr << "n: " << original.size()
          << ", m: " << original.num_edges() << "\n";


    std::cout << std::fixed << std::setprecision(9);
      
    for (const auto& config : strategies) {
        // Accumulators for averages
        double sum_runtime = 0.0;
        double sum_iterations = 0.0;
        double sum_bound = 0.0;
        double sum_r = 0.0;
        double sum_m = 0.0;
		double sum_n = 0.0;
        double sum_cFrac = 0.0;
        double sum_rBar = 0.0;
        double sum_time_per_mI = 0.0;
        double sum_time_per_nI = 0.0;
        double sum_time_over_nm = 0.0;
        double sum_time_over_I_sntm = 0.0;
        double sum_s_bar = 0.0;
        double sum_t_bar_forward = 0.0;
        double sum_t_bar_residual = 0.0;
        double sum_insert_norm = 0.0;
        double sum_delete_norm = 0.0;
        double sum_update_norm_m = 0.0;
        double sum_update_norm_theoretical = 0.0;
        int   max_flow = -1;

        for (int rep = 0; rep < num_repetitions; ++rep) {
            // Copy graph and reset stats
            Graph copy = original;
            FFStats stats;

            // Run algorithm (stats.total_runtime set inside ford_fulkerson)
            max_flow = ford_fulkerson(copy, source, sink, config.strategy, config.type, &stats);

            // Accumulate
            sum_runtime                  += stats.total_runtime;
            sum_iterations               += stats.iterations;
            sum_bound                    += stats.bound;
            sum_r                        += stats.r;
            sum_m                        += static_cast<double>(stats.m);
			sum_n                        += static_cast<double>(stats.n);
            sum_cFrac                    += stats.cFrac;
            sum_rBar                     += stats.rBar;
            sum_time_per_mI              += stats.time_per_mI;
            sum_time_per_nI              += stats.time_per_nI;
            sum_time_over_nm    += stats.time_over_nm;
            sum_time_over_I_sntm         += stats.time_over_I_sntm;
            sum_s_bar                    += stats.s_bar;
            sum_t_bar_forward            += stats.t_bar_forward;
            sum_t_bar_residual           += stats.t_bar_residual;
            sum_insert_norm              += stats.avg_insert_normalized;
            sum_delete_norm              += stats.avg_delete_normalized;
            sum_update_norm_m            += stats.avg_update_normalized_m;
            sum_update_norm_theoretical  += stats.avg_update_normalized_theoretical;
        }

        // Compute averages
        double inv = 1.0 / num_repetitions;
        double avg_runtime                 = sum_runtime * inv;
        double avg_iterations              = sum_iterations * inv;
        double avg_bound                   = sum_bound * inv;
        double avg_r                       = sum_r * inv;
        double avg_m                       = sum_m * inv;
		double avg_n                       = sum_n * inv;
        double avg_cFrac                   = sum_cFrac * inv;
        double avg_rBar                    = sum_rBar * inv;
        double avg_time_per_mI             = sum_time_per_mI * inv;
        double avg_time_per_nI             = sum_time_per_nI * inv;
        double avg_time_over_nm   = sum_time_over_nm * inv;
        double avg_time_over_I_sntm        = sum_time_over_I_sntm * inv;
        double avg_s_bar                   = sum_s_bar * inv;
        double avg_t_bar_forward           = sum_t_bar_forward * inv;
        double avg_t_bar_residual          = sum_t_bar_residual * inv;
        double avg_insert_norm             = sum_insert_norm * inv;
        double avg_delete_norm             = sum_delete_norm * inv;
        double avg_update_norm_m           = sum_update_norm_m * inv;
        double avg_update_norm_theoretical = sum_update_norm_theoretical * inv;

        // Print CSV line to stdout
        std::cout 
            << config.name << ";"
            << max_flow << ";";
        // Usando a função print_with_precision para os tempos
        print_with_precision(avg_runtime);
        std::cout << ";";
        print_with_precision(avg_iterations);
        std::cout << ";";
        print_with_precision(avg_bound);
        std::cout << ";";
        print_with_precision(avg_r);
        std::cout << ";";
        print_with_precision(avg_m);
        std::cout << ";";
		print_with_precision(avg_n);
        std::cout << ";";
        print_with_precision(avg_cFrac);
        std::cout << ";";
        print_with_precision(avg_rBar);
        std::cout << ";";
        print_with_precision(avg_time_per_mI);
        std::cout << ";";
        print_with_precision(avg_time_per_nI);
        std::cout << ";";
        print_with_precision(avg_time_over_nm);
        std::cout << ";";
        print_with_precision(avg_time_over_I_sntm);
        std::cout << ";";
        print_with_precision(avg_s_bar);
        std::cout << ";";
        print_with_precision(avg_t_bar_forward);
        std::cout << ";";
        print_with_precision(avg_t_bar_residual);
        std::cout << ";";
        print_with_precision(avg_insert_norm);
        std::cout << ";";
        print_with_precision(avg_delete_norm);
        std::cout << ";";
        print_with_precision(avg_update_norm_m);
        std::cout << ";";
        print_with_precision(avg_update_norm_theoretical);
        std::cout << "\n";

    }

    //std::cout << std::flush;
    return 0;
}
