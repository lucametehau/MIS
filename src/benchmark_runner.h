#pragma once

#include "benchmark.h"
#include "mis_checker.h"
#include "algorithm_registry.h"
#include "types.h"

#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>

struct BenchmarkCsvRow {
    std::string suite;
    std::string label;
    std::string algorithm;
    std::string graph_family;
    std::size_t n = 0;
    double c = 0.0;
    std::size_t m0 = 0;
    std::size_t m = 0;
    std::string representation;
    std::size_t threads = 0;
    int nr_graphs = 0;
    int nr_runs = 0;
    double mean_time_ms = 0.0;
    double graph_std_ms = 0.0;
    double mean_run_std_ms = 0.0;
};

inline void write_csv_header(std::ostream& out) {
    out << "suite,label,algorithm,graph_family,n,c,m0,m,representation,threads,"
        << "nr_graphs,nr_runs,mean_time_ms,graph_std_ms,mean_run_std_ms\n";
}

inline void append_csv_row(std::ostream& out, const BenchmarkCsvRow& row) {
    out << row.suite << ','
        << row.label << ','
        << row.algorithm << ','
        << row.graph_family << ','
        << row.n << ','
        << row.c << ','
        << row.m0 << ','
        << row.m << ','
        << row.representation << ','
        << row.threads << ','
        << row.nr_graphs << ','
        << row.nr_runs << ','
        << row.mean_time_ms << ','
        << row.graph_std_ms << ','
        << row.mean_run_std_ms << '\n';
}

template <typename GraphT, typename SolverFunc>
BenchmarkCsvRow run_single_case_on_graphs(
    const std::vector<GraphT>& graphs,
    SolverFunc solver,
    const std::string& algorithm_name,
    int nr_runs,
    bool verify,
    const BenchmarkCsvRow& meta
) {
    MISChecker checker;
    std::vector<double> per_graph_means;
    std::vector<double> per_graph_run_stds;
    per_graph_means.reserve(graphs.size());
    per_graph_run_stds.reserve(graphs.size());

    for (std::size_t graph_idx = 0; graph_idx < graphs.size(); ++graph_idx) {
        const GraphT& g = graphs[graph_idx];
        std::vector<double> times;
        times.reserve(nr_runs);
        NodeList mis;

        constexpr int warmup_runs = 2;
        for (int run = 0; run < warmup_runs; run++) {
            const auto start = std::chrono::high_resolution_clock::now();
            mis = solver(g);
            const auto end = std::chrono::high_resolution_clock::now();

            if (verify) {
                const auto verdict = checker.check_mis(g, mis);
                if (verdict != "MIS correct!") {
                    std::cerr << "CORRECTNESS FAILED: " << algorithm_name
                            << " on graph #" << graph_idx
                            << " — " << verdict << "\n";
                }
            }
        }

        for (int run = 0; run < nr_runs; ++run) {
            const auto start = std::chrono::high_resolution_clock::now();
            mis = solver(g);
            const auto end = std::chrono::high_resolution_clock::now();
            times.push_back(
                std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0
            );
        }

        if (verify) {
            const auto verdict = checker.check_mis(g, mis);
            if (verdict != "MIS correct!") {
                std::cerr << "CORRECTNESS FAILED: " << algorithm_name
                          << " on graph #" << graph_idx
                          << " — " << verdict << "\n";
            }
        }

        const auto [mean_ms, run_std_ms] = benchmark_detail::mean_and_sample_std(times);
        per_graph_means.push_back(mean_ms);
        per_graph_run_stds.push_back(run_std_ms);
    }

    const auto [mean_time_ms, graph_std_ms] =
        benchmark_detail::mean_and_sample_std(per_graph_means);
    const double mean_run_std_ms = benchmark_detail::mean_of(per_graph_run_stds);

    BenchmarkCsvRow row = meta;
    row.algorithm = algorithm_name;
    row.nr_graphs = static_cast<int>(graphs.size());
    row.nr_runs = nr_runs;
    row.mean_time_ms = mean_time_ms;
    row.graph_std_ms = graph_std_ms;
    row.mean_run_std_ms = mean_run_std_ms;
    return row;
}
