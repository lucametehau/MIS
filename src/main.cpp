#include <iostream>
#include <vector>
#include "mis.h"
#include "graph_generator.h"
#include "benchmark.h"

int main() {
    GraphGenerator generator(42);
    Benchmarker<GraphCSR> bench(5);

    bench.add_algorithm("Sequential_csr", [](const GraphCSR& g) {
        return MISSolver<GraphCSR>(g).find(Algorithm::Sequential);
    });

    bench.add_algorithm("Luby_csr", [](const GraphCSR& g) {
        return MISSolver<GraphCSR>(g).find(Algorithm::Luby);
    });

    bench.add_algorithm("Luby Improved_csr", [](const GraphCSR& g) {
        return MISSolver<GraphCSR>(g).find(Algorithm::LubyImproved);
    });

    const std::size_t n = 1000000;
    
    std::cout << "Generating Uniform Sparse Graph (n=" << n << ", p=0.0004)...\n";
    const auto g_uniform_csr = generator.generate_sparse_uniform_csr(n, 0.0004);
    bench.run("Uniform Sparse_csr", g_uniform_csr);

    std::cout << "\nGenerating Scale-Free Graph (n=" << n << ", m0=10, m=5)...\n";
    const auto g_scale_free_csr = generator.generate_scale_free_csr(n, 10, 5);
    bench.run("Scale-Free_csr", g_scale_free_csr);

    bench.print_results();

    GraphGenerator generator2(42);
    
    Benchmarker<Graph> bench2(5);

    bench2.add_algorithm("Sequential", [](const Graph& g) {
        return MISSolver<Graph>(g).find(Algorithm::Sequential);
    });

    bench2.add_algorithm("Luby", [](const Graph& g) {
        return MISSolver<Graph>(g).find(Algorithm::Luby);
    });

    bench2.add_algorithm("Luby Improved", [](const Graph& g) {
        return MISSolver<Graph>(g).find(Algorithm::LubyImproved);
    });
    
    std::cout << "Generating Uniform Sparse Graph (n=" << n << ", p=0.0004)...\n";
    const auto g_uniform = generator2.generate_sparse_uniform(n, 0.0004);
    bench2.run("Uniform Sparse", g_uniform);

    std::cout << "\nGenerating Scale-Free Graph (n=" << n << ", m0=10, m=5)...\n";
    const auto g_scale_free = generator2.generate_scale_free(n, 10, 5);
    bench2.run("Scale-Free", g_scale_free);

    bench2.print_results();

    return 0;
}
