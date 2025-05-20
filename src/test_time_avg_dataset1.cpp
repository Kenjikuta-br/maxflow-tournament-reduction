#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <algorithm>

namespace fs = std::filesystem;

const std::string DATASET_DIR = "dataset1";
const std::string EXECUTABLE = "./bin/tournament_timer";
const std::string OUTPUT_CSV = "execution_times.csv";

double run_and_get_time(const std::string& instance_path) {
    std::ostringstream command;
    command << EXECUTABLE << " < " << instance_path;

    FILE* pipe = popen(command.str().c_str(), "r");
    if (!pipe) {
        std::cerr << "Failed to run command on: " << instance_path << std::endl;
        return -1;
    }

    double time = -1;
    int scan_result = fscanf(pipe, "%lf", &time);
    if (scan_result != 1) {
        std::cerr << "Failed to read time from: " << instance_path << std::endl;
        time = -1;
    }
    pclose(pipe);
    return time;
}

int main() {
    std::vector<std::pair<int, double>> results;

    // Process all team directories
    for (const auto& team_dir : fs::directory_iterator(DATASET_DIR)) {
        if (!team_dir.is_directory()) continue;

        int team_size = std::stoi(team_dir.path().filename().string());
        std::vector<double> times;

        // Process all instance files
        for (const auto& instance_file : fs::directory_iterator(team_dir.path())) {
            if (instance_file.path().extension() != ".txt") continue;

            double t = run_and_get_time(instance_file.path().string());
            if (t >= 0)
                times.push_back(t);
        }

        if (!times.empty()) {
            double sum = std::accumulate(times.begin(), times.end(), 0.0);
            double average = sum / times.size();
            results.emplace_back(team_size, average);
        }
    }

    // Sort by team size
    std::sort(results.begin(), results.end());

    // Write results to CSV file
    std::ofstream csv_file(OUTPUT_CSV);
    if (!csv_file.is_open()) {
        std::cerr << "Failed to open output file: " << OUTPUT_CSV << std::endl;
        return 1;
    }

    // Write CSV header
    csv_file << "TeamSize,TimeAvgSeconds\n";

    // Write data rows
    for (const auto& [team_size, avg_time] : results) {
        csv_file << team_size << "," << std::fixed << std::setprecision(6) << avg_time << "\n";
    }

    csv_file.close();
    std::cout << "Results saved to " << OUTPUT_CSV << std::endl;

    return 0;
}