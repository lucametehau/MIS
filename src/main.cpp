#include <iostream>
#include <vector>
#include "mis.h"
#include "graph_generator.h"
#include "benchmark.h"

int main() {
    Benchmarker<Graph> bench(5);
    Benchmarker<GraphCSR> benchCSR(5);

    //bench.add_algorithm("Sequential", [](const Graph& g) {
    //    return MISSolver<Graph>(g).find(Algorithm::Sequential);
    //});

    bench.add_algorithm("Luby", [](const Graph& g) {
        return MISSolver<Graph>(g).find(Algorithm::Luby);
    });

    bench.add_algorithm("Luby Improved", [](const Graph& g) {
        return MISSolver<Graph>(g).find(Algorithm::LubyImproved);
    });

    //benchCSR.add_algorithm("Sequential_csr", [](const GraphCSR& g) {
    //    return MISSolver<GraphCSR>(g).find(Algorithm::Sequential);
    //});

    benchCSR.add_algorithm("Luby_csr", [](const GraphCSR& g) {
        return MISSolver<GraphCSR>(g).find(Algorithm::Luby);
    });

    benchCSR.add_algorithm("Luby Improved_csr", [](const GraphCSR& g) {
        return MISSolver<GraphCSR>(g).find(Algorithm::LubyImproved);
    });

    const std::size_t n = 1000000;
    GraphGenerator generator(42);

    std::cout << "\nGenerating Uniform Sparse Graph (csr) (n=" << n << ", p=0.0004)...\n";
    const auto g_uniform_csr = generator.generate_sparse_uniform_csr(n, 0.0004);

    std::cout << "\nGenerating Scale-Free Graph (n=" << n << ", m0=10, m=5)...\n";
    const auto g_scale_free = generator.generate_scale_free(n, 10, 5);

    std::cout << "\nGenerating Uniform Sparse Graph (n=" << n << ", p=0.0004)...\n";
    const auto g_uniform = g_uniform_csr.to_adjacency_list();

    std::cout << "\nGenerating Scale-Free Graph (n=" << n << ", m0=10, m=5)...\n";
    const auto g_scale_free_csr = GraphCSR(g_scale_free);

    std::cout << "\nRunning benchmarks...\n";

    benchCSR.run("Uniform Sparse_csr", g_uniform_csr);
    std::cout << "a" << std::endl;
    benchCSR.run("Scale-Free_csr", g_scale_free_csr);
    std::cout << "a" << std::endl;
    bench.run("Uniform Sparse", g_uniform);
    std::cout << "a" << std::endl;
    bench.run("Scale-Free", g_scale_free);
    std::cout << "a" << std::endl;

    bench.print_results();
    benchCSR.print_results();

    GraphGenerator generator2(42);
    return 0;
}
