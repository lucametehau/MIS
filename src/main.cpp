#include <iostream>
#include <vector>
#include "mis.h"
#include "graph_generator.h"
#include "benchmark.h"
#include "weighted_mis.h"
#include "weighted_benchmark.h"

int main() {
    const int nr_graphs = 5;
    const int nr_runs = 10;
    const uint32_t base_seed = 42;
    const bool verify = true;

    const std::size_t n = 2000000;

    // Benchmarker<Graph> bench(nr_runs, verify);
    Benchmarker<GraphCSR> benchCSR(nr_runs, verify);

    // bench.add_algorithm("Sequential", [](const Graph& g) {
    //    return MISSolver<Graph>(g).find(Algorithm::Sequential);
    // });

    // bench.add_algorithm("Luby 1t", [](const Graph& g) {
    //     return MISSolver<Graph>(g).find(Algorithm::Luby, 1);
    // });

    // bench.add_algorithm("Luby 2t", [](const Graph& g) {
    //     return MISSolver<Graph>(g).find(Algorithm::Luby, 2);
    // });

    // bench.add_algorithm("Luby 4t", [](const Graph& g) {
    //     return MISSolver<Graph>(g).find(Algorithm::Luby, 4);
    // });

    // bench.add_algorithm("Luby 8t", [](const Graph& g) {
    //     return MISSolver<Graph>(g).find(Algorithm::Luby, 8);
    // });

    // bench.add_algorithm("Luby", [](const Graph& g) {
    //     return MISSolver<Graph>(g).find(Algorithm::Luby);
    // });

    // bench.add_algorithm("Luby Improved", [](const Graph& g) {
    //     return MISSolver<Graph>(g).find(Algorithm::LubyImproved);
    // });

    benchCSR.add_algorithm("Sequential CSR", [](const GraphCSR& g) {
       return MISSolver<GraphCSR>(g).find(Algorithm::Sequential);
    });

    // benchCSR.add_algorithm("Luby 1t CSR", [](const GraphCSR& g) {
    //     return MISSolver<GraphCSR>(g).find(Algorithm::Luby, 1);
    // });

    // benchCSR.add_algorithm("Luby 2t CSR", [](const GraphCSR& g) {
    //     return MISSolver<GraphCSR>(g).find(Algorithm::Luby, 2);
    // });

    // benchCSR.add_algorithm("Luby 4t CSR", [](const GraphCSR& g) {
    //     return MISSolver<GraphCSR>(g).find(Algorithm::Luby, 4);
    // });

    // benchCSR.add_algorithm("Luby 8t CSR", [](const GraphCSR& g) {
    //     return MISSolver<GraphCSR>(g).find(Algorithm::Luby, 8);
    // });

    // benchCSR.add_algorithm("Luby CSR", [](const GraphCSR& g) {
    //     return MISSolver<GraphCSR>(g).find(Algorithm::Luby);
    // });

    // benchCSR.add_algorithm("Luby Improved CSR", [](const GraphCSR& g) {
    //     return MISSolver<GraphCSR>(g).find(Algorithm::LubyImproved);
    // });

    benchCSR.add_algorithm("Luby GPU CSR", [](const GraphCSR& g) {
        return MISSolver<GraphCSR>(g).find(Algorithm::LubyGPU);
    });

    std::cout << "\nRunning benchmarks (nr_graphs=" << nr_graphs
              << ", nr_runs=" << nr_runs << ")...\n";

    std::cout << "\nBenchmarking Scale-Free (csr) (n=" << n << ", m0=10, m=5)...\n";
    benchCSR.run_suite("Scale-Free_csr", nr_graphs, [&](int i) {
        GraphGenerator gen(base_seed + static_cast<uint32_t>(i));
        return gen.generate_scale_free_csr(n, 10, 5);
    });

    std::cout << "\nBenchmarking Uniform Sparse (csr) (n=" << n << ", p=0.0004)...\n";
    benchCSR.run_suite("Uniform Sparse_csr", nr_graphs, [&](int i) {
        GraphGenerator gen(base_seed + static_cast<uint32_t>(i));
        return gen.generate_sparse_uniform_csr(n, 0.0004);
    });

    // std::cout << "\nBenchmarking Uniform Sparse (n=" << n << ", p=0.0004)...\n";
    // bench.run_suite("Uniform Sparse", nr_graphs, [&](int i) {
    //     GraphGenerator gen(base_seed + static_cast<uint32_t>(i));
    //     return gen.generate_sparse_uniform_csr(n, 0.0004).to_adjacency_list();
    // });

    // std::cout << "\nBenchmarking Scale-Free (n=" << n << ", m0=10, m=5)...\n";
    // bench.run_suite("Scale-Free", nr_graphs, [&](int i) {
    //     GraphGenerator gen(base_seed + static_cast<uint32_t>(i));
    //     return gen.generate_scale_free(n, 10, 5);
    // });

    // bench.print_results();
    benchCSR.print_results();


    // WeightedBenchmarker<WeightedGraph> benchWeighted(nr_runs, verify);

    // benchWeighted.add_algorithm("Weighted Greedy", [](const WeightedGraph& g) {
    //     return weighted_greedy_mis(g);
    // });

    // benchWeighted.add_algorithm("Weighted Sampling", [](const WeightedGraph& g) {
    //     return weighted_sampling_mis(g);
    // });

    // std::cout << "\nBenchmarking Weighted MIS (Uniform weights)...\n";
    // benchWeighted.run_suite("Uniform Sparse (uniform)", nr_graphs, [&](int i) {
    //     GraphGenerator gen(base_seed + static_cast<uint32_t>(i));
    //     return gen.add_weights_uniform(gen.generate_sparse_uniform(n, 0.0004));
    // });
    // benchWeighted.run_suite("Scale-Free (uniform)", nr_graphs, [&](int i) {
    //     GraphGenerator gen(base_seed + static_cast<uint32_t>(i));
    //     return gen.add_weights_uniform(gen.generate_scale_free(n, 10, 5));
    // });

    // std::cout << "\nBenchmarking Weighted MIS (Exponential weights)...\n";
    // benchWeighted.run_suite("Uniform Sparse (exp)", nr_graphs, [&](int i) {
    //     GraphGenerator gen(base_seed + static_cast<uint32_t>(i));
    //     return gen.add_weights_exp(gen.generate_sparse_uniform(n, 0.0004));
    // });
    // benchWeighted.run_suite("Scale-Free (exp)", nr_graphs, [&](int i) {
    //     GraphGenerator gen(base_seed + static_cast<uint32_t>(i));
    //     return gen.add_weights_exp(gen.generate_scale_free(n, 10, 5));
    // });

    // std::cout << "\nBenchmarking Weighted MIS (Clustered weights)...\n";
    // benchWeighted.run_suite("Uniform Sparse (clustered)", nr_graphs, [&](int i) {
    //     GraphGenerator gen(base_seed + static_cast<uint32_t>(i));
    //     return gen.add_weights_clustered(gen.generate_sparse_uniform(n, 0.0004));
    // });
    // benchWeighted.run_suite("Scale-Free (clustered)", nr_graphs, [&](int i) {
    //     GraphGenerator gen(base_seed + static_cast<uint32_t>(i));
    //     return gen.add_weights_clustered(gen.generate_scale_free(n, 10, 5));
    // });

    // benchWeighted.print_results();

    return 0;
}
