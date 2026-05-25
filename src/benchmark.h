#pragma once
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <cmath>
#include "types.h"
#include "mis_checker.h"

struct BenchmarkResult {
    std::string algorithm_name;
    std::string graph_name;
    double average_time_ms;
    double stddev_time_ms;
    bool is_correct;
    std::size_t mis_size;
};

template<typename GraphT>
class Benchmarker {
public:
    using SolverFunc = std::function<NodeList(const GraphT&)>;

    Benchmarker(int num_runs = 5);

    void add_algorithm(const std::string& name, SolverFunc solver);
    void run(const std::string& graph_name, const GraphT& g);
    void print_results() const;

private:
    int num_runs_;
    struct AlgoInfo {
        std::string name;
        SolverFunc solver;
    };
    std::vector<AlgoInfo> algorithms_;
    std::vector<BenchmarkResult> results_;
};

template<typename GraphT>
Benchmarker<GraphT>::Benchmarker(int num_runs) : num_runs_(num_runs) {}

template<typename GraphT>
void Benchmarker<GraphT>::add_algorithm(const std::string& name, SolverFunc solver) {
    algorithms_.push_back({name, solver});
}

template<typename GraphT>
void Benchmarker<GraphT>::run(const std::string& graph_name, const GraphT& g) {
    MISChecker checker;
    for (const auto& algo : algorithms_) {
        std::vector<double> times;
        NodeList last_mis;
        bool all_correct = true;

        for (int i = 0; i < num_runs_; i++) {
            auto start = std::chrono::high_resolution_clock::now();
            last_mis = algo.solver(g);
            auto end = std::chrono::high_resolution_clock::now();
            
            times.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0);
            
            if constexpr (std::is_same_v<GraphT, Graph>) {
                 if (checker.check_mis(g, last_mis) != "MIS correct!") {
                    all_correct = false;
                }
            }
        }

        double avg_time = std::accumulate(times.begin(), times.end(), 0.0) / (double)num_runs_;

        double stddev_time = 0.0;
        if (num_runs_ > 1) {
            double sum_sq_diff = 0.0;
            for (double t : times) {
                const double diff = t - avg_time;
                sum_sq_diff += diff * diff;
            }
            stddev_time = std::sqrt(sum_sq_diff / (num_runs_ - 1));
        }

        results_.push_back({algo.name, graph_name, avg_time, stddev_time, all_correct, last_mis.size()});
    }
}

template<typename GraphT>
void Benchmarker<GraphT>::print_results() const {
    if (results_.empty()) return;

    constexpr int table_width = 98;
    std::cout << "\n" << std::setfill('=') << std::setw(table_width) << "" << std::setfill(' ') << "\n";
    std::cout << std::left << std::setw(20) << "Algorithm" 
              << std::setw(30) << "Graph Type" 
              << std::right << std::setw(12) << "Avg Time" 
              << std::right << std::setw(12) << "Std Dev" 
              << std::right << std::setw(10) << "MIS Size" 
              << "   Status" << "\n";
    std::cout << std::setfill('-') << std::setw(table_width) << "" << std::setfill(' ') << "\n";

    for (const auto& res : results_) {
        std::cout << std::left << std::setw(20) << res.algorithm_name 
                  << std::setw(30) << res.graph_name 
                  << std::right << std::fixed << std::setprecision(3) << std::setw(9) << res.average_time_ms << " ms"
                  << std::right << std::setw(9) << res.stddev_time_ms << " ms"
                  << std::right << std::setw(10) << res.mis_size 
                  << "   " << (res.is_correct ? "PASSED" : "FAILED") << "\n";
    }
    std::cout << std::setfill('=') << std::setw(table_width) << "" << std::setfill(' ') << "\n";
}
