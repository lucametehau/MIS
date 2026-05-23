#include <iostream>
#include <vector>
#include "mis.h"
#include "graph_generator.h"
#include "benchmark.h"

int main() {
    Benchmarker<Graph> bench(20);
    Benchmarker<GraphCSR> benchCSR(20);

    bench.add_algorithm("Sequential", [](const Graph& g) {
        return MISSolver<Graph>(g).find(Algorithm::Sequential);
    });

    bench.add_algorithm("Luby", [](const Graph& g) {
        return MISSolver<Graph>(g).find(Algorithm::Luby);
    });

    bench.add_algorithm("Luby Improved", [](const Graph& g) {
        return MISSolver<Graph>(g).find(Algorithm::LubyImproved);
    });

    benchCSR.add_algorithm("Sequential_csr", [](const GraphCSR& g) {
        return MISSolver<GraphCSR>(g).find(Algorithm::Sequential);
    });

    benchCSR.add_algorithm("Luby_csr", [](const GraphCSR& g) {
        return MISSolver<GraphCSR>(g).find(Algorithm::Luby);
    });

    benchCSR.add_algorithm("Luby Improved_csr", [](const GraphCSR& g) {
        return MISSolver<GraphCSR>(g).find(Algorithm::LubyImproved);
    });

    const std::size_t n = 1000000;
    GraphGenerator generator(42);

    std::cout << "Generating Uniform Sparse Graph (n=" << n << ", p=0.0004)...\n";
    const auto g_uniform = generator.generate_sparse_uniform(n, 0.0004);
    bench.run("Uniform Sparse", g_uniform);

    std::cout << "\nGenerating Scale-Free Graph (n=" << n << ", m0=10, m=5)...\n";
    const auto g_scale_free = generator.generate_scale_free(n, 10, 5);
    bench.run("Scale-Free", g_scale_free);

    bench.print_results();
    
    std::cout << "Generating Uniform Sparse Graph (n=" << n << ", p=0.0004)...\n";
    const auto g_uniform_csr = GraphCSR(g_uniform);
    benchCSR.run("Uniform Sparse_csr", g_uniform_csr);

    std::cout << "\nGenerating Scale-Free Graph (n=" << n << ", m0=10, m=5)...\n";
    const auto g_scale_free_csr = GraphCSR(g_scale_free);
    benchCSR.run("Scale-Free_csr", g_scale_free_csr);

    benchCSR.print_results();

    GraphGenerator generator2(42);
    return 0;
}
