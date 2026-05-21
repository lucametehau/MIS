#pragma once
#include <string>
#include <vector>
#include <functional>
#include "types.h"

struct BenchmarkResult {
    std::string algorithm_name;
    std::string graph_name;
    double average_time_ms;
    bool is_correct;
    std::size_t mis_size;
};

class Benchmarker {
public:
    using SolverFunc = std::function<NodeList(const Graph&)>;

    Benchmarker(int num_runs = 5);

    void add_algorithm(const std::string& name, SolverFunc solver);
    void run(const std::string& graph_name, const Graph& g);
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
