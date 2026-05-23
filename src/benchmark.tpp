#pragma once
#include "benchmark.h"
#include "mis_checker.h"
#include <chrono>
#include <iostream>
#include <iomanip>
#include <numeric>

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
        results_.push_back({algo.name, graph_name, avg_time, all_correct, last_mis.size()});
    }
}

template<typename GraphT>
void Benchmarker<GraphT>::print_results() const {
    if (results_.empty()) return;

    std::cout << "\n" << std::setfill('=') << std::setw(85) << "" << std::setfill(' ') << "\n";
    std::cout << std::left << std::setw(20) << "Algorithm" 
              << std::setw(35) << "Graph Type" 
              << std::right << std::setw(12) << "Avg Time" 
              << std::right << std::setw(10) << "MIS Size" 
              << "   Status" << "\n";
    std::cout << std::setfill('-') << std::setw(85) << "" << std::setfill(' ') << "\n";

    for (const auto& res : results_) {
        std::cout << std::left << std::setw(20) << res.algorithm_name 
                  << std::setw(35) << res.graph_name 
                  << std::right << std::fixed << std::setprecision(3) << std::setw(9) << res.average_time_ms << " ms"
                  << std::right << std::setw(10) << res.mis_size 
                  << "   " << (res.is_correct ? "PASSED" : "FAILED") << "\n";
    }
    std::cout << std::setfill('=') << std::setw(85) << "" << std::setfill(' ') << "\n";
}
