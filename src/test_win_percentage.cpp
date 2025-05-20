#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <regex>
#include <map>
#include <algorithm>

namespace fs = std::filesystem;

int main() {
    const std::string dataset_dir = "dataset2";
    const std::string binary_path = "./bin/tournament";
    const std::string output_csv = "dataset2_summary.csv";

    std::ofstream csv(output_csv);
    if (!csv.is_open()) {
        std::cerr << "Failed to open CSV output file.\n";
        return 1;
    }

    csv << "alpha,bias,percentage_sim\n";
    csv.flush();

    std::regex pattern(R"(alpha_(\d+_\d+)__bias_([+-]\d+_\d+))");

    // Lista de (alpha, bias, path)
    std::vector<std::tuple<double, double, fs::path>> configs;

    for (const auto& entry : fs::directory_iterator(dataset_dir)) {
        if (!entry.is_directory()) continue;

        std::string folder_name = entry.path().filename().string();
        std::smatch match;
        if (std::regex_match(folder_name, match, pattern)) {
            std::string alpha_str = match[1].str();
            std::string bias_str = match[2].str();
            std::replace(alpha_str.begin(), alpha_str.end(), '_', '.');
            std::replace(bias_str.begin(), bias_str.end(), '_', '.');
            double alpha = std::stod(alpha_str);
            double bias = std::stod(bias_str);
            configs.emplace_back(alpha, bias, entry.path());
        } else {
            std::cerr << "Skipping unrecognized folder format: " << folder_name << "\n";
        }
    }

    // Ordena por alpha crescente, depois bias crescente
    std::sort(configs.begin(), configs.end());

    // Processa na ordem correta
    for (const auto& [alpha, bias, path] : configs) {
        int total = 0;
        int sim_count = 0;

        for (const auto& file : fs::directory_iterator(path)) {
            if (!file.is_regular_file()) continue;

            std::string command = binary_path + " < " + file.path().string();
            FILE* pipe = popen(command.c_str(), "r");
            if (!pipe) {
                std::cerr << "Failed to run command: " << command << "\n";
                continue;
            }

            char buffer[128];
            std::string result;
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                result += buffer;
            }
            pclose(pipe);

            result.erase(remove_if(result.begin(), result.end(), ::isspace), result.end());

            if (result == "sim") {
                sim_count++;
            }

            total++;
        }

        double percentage = (total > 0) ? (100.0 * sim_count / total) : 0.0;
        csv << alpha << "," << bias << "," << percentage << "\n";
        csv.flush();

        std::cout << "Processed alpha=" << alpha << ", bias=" << bias << ": " << percentage << "% sim\n";
    }

    csv.close();
    std::cout << "Results written to " << output_csv << "\n";

    return 0;
}
