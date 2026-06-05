#pragma once
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <cmath>
#include <utility>
#include <type_traits>
#include "types.h"
#include "mis_checker.h"

struct GraphRunStats {
    double mean_ms;
    double run_std_ms;
};

struct BenchmarkSummary {
    std::string algorithm_name;
    std::string graph_type;
    double mean_time_ms;
    double graph_std_ms;
    double mean_run_std_ms;
};

namespace benchmark_detail {

inline std::pair<double, double> mean_and_sample_std(const std::vector<double>& values) {
    if (values.empty()) return {0.0, 0.0};

    const double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    if (values.size() <= 1) return {mean, 0.0};

    double sum_sq_diff = 0.0;
    for (double v : values) {
        const double diff = v - mean;
        sum_sq_diff += diff * diff;
    }
    return {mean, std::sqrt(sum_sq_diff / (values.size() - 1))};
}

inline double mean_of(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
}

} // namespace benchmark_detail

template<typename GraphT>
class Benchmarker {
public:
    using SolverFunc = std::function<NodeList(const GraphT&)>;

    Benchmarker(int num_runs = 5, bool verify_correctness = false);

    void add_algorithm(const std::string& name, SolverFunc solver);

    template<typename GraphFactory>
    void run_suite(const std::string& graph_type, int num_graphs, GraphFactory&& make_graph);

    void print_results() const;

private:
    int num_runs_;
    bool verify_correctness_;
    struct AlgoInfo {
        std::string name;
        SolverFunc solver;
    };
    std::vector<AlgoInfo> algorithms_;
    std::vector<BenchmarkSummary> summaries_;
};

template<typename GraphT>
Benchmarker<GraphT>::Benchmarker(int num_runs, bool verify_correctness)
    : num_runs_(num_runs), verify_correctness_(verify_correctness) {}

template<typename GraphT>
void Benchmarker<GraphT>::add_algorithm(const std::string& name, SolverFunc solver) {
    algorithms_.push_back({name, solver});
}

template<typename GraphT>
template<typename GraphFactory>
void Benchmarker<GraphT>::run_suite(
    const std::string& graph_type,
    int num_graphs,
    GraphFactory&& make_graph
) {
    MISChecker checker;

    for (const auto& algo : algorithms_) {
        std::vector<double> per_graph_means;
        std::vector<double> per_graph_run_stds;
        per_graph_means.reserve(num_graphs);
        per_graph_run_stds.reserve(num_graphs);

        for (int graph_idx = 0; graph_idx < num_graphs; ++graph_idx) {
            GraphT g = make_graph(graph_idx);
            std::vector<double> times;
            times.reserve(num_runs_);
            NodeList mis;

            constexpr int warmup_runs = 2;
            for (int run = 0; run < warmup_runs; run++) {
                mis = algo.solver(g);
                if (verify_correctness_ && checker.check_mis(g, mis) != "MIS correct!") {
                    std::cerr << "CORRECTNESS FAILED: " << algo.name
                                << " on " << graph_type << " graph #" << graph_idx << "\n";
                    std::cerr << "Reason: " << checker.check_mis(g, mis) << "\n";
                }
            }

            for (int run = 0; run < num_runs_; ++run) {
                const auto start = std::chrono::high_resolution_clock::now();
                mis = algo.solver(g);
                const auto end = std::chrono::high_resolution_clock::now();

                times.push_back(
                    std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0
                );
            }

            if (verify_correctness_ && checker.check_mis(g, mis) != "MIS correct!") {
                std::cerr << "CORRECTNESS FAILED: " << algo.name
                            << " on " << graph_type << " graph #" << graph_idx << "\n";
                std::cerr << "Reason: " << checker.check_mis(g, mis) << "\n";
            }

            const auto [mean_ms, run_std_ms] = benchmark_detail::mean_and_sample_std(times);
            per_graph_means.push_back(mean_ms);
            per_graph_run_stds.push_back(run_std_ms);
        }

        const auto [mean_time_ms, graph_std_ms] =
            benchmark_detail::mean_and_sample_std(per_graph_means);
        const double mean_run_std_ms = benchmark_detail::mean_of(per_graph_run_stds);

        summaries_.push_back({
            algo.name,
            graph_type,
            mean_time_ms,
            graph_std_ms,
            mean_run_std_ms
        });
    }
}

template<typename GraphT>
void Benchmarker<GraphT>::print_results() const {
    if (summaries_.empty()) return;

    constexpr int table_width = 110;
    std::cout << "\n" << std::setfill('=') << std::setw(table_width) << "" << std::setfill(' ') << "\n";
    std::cout << std::left << std::setw(20) << "Algorithm"
              << std::setw(30) << "Graph Type"
              << std::right << std::setw(14) << "Mean Time"
              << std::right << std::setw(14) << "Graph Std"
              << std::right << std::setw(14) << "Avg Run Std"
              << "\n";
    std::cout << std::setfill('-') << std::setw(table_width) << "" << std::setfill(' ') << "\n";

    for (const auto& res : summaries_) {
        std::cout << std::left << std::setw(20) << res.algorithm_name
                  << std::setw(30) << res.graph_type
                  << std::right << std::fixed << std::setprecision(3)
                  << std::setw(11) << res.mean_time_ms << " ms"
                  << std::setw(11) << res.graph_std_ms << " ms"
                  << std::setw(11) << res.mean_run_std_ms << " ms"
                  << "\n";
    }
    std::cout << std::setfill('=') << std::setw(table_width) << "" << std::setfill(' ') << "\n";
}
