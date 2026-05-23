#include <iostream>
#include <vector>
#include "mis.h"
#include "graph_generator.h"
#include "benchmark.h"

int main() {
    GraphGenerator generator;
    Benchmarker<Graph> bench(5);

    bench.add_algorithm("Sequential", [](const Graph& g) {
        return MISSolver<Graph>(g).find(Algorithm::Sequential);
    });

    bench.add_algorithm("Luby", [](const Graph& g) {
        return MISSolver<Graph>(g).find(Algorithm::Luby);
    });

    bench.add_algorithm("Luby Improved", [](const Graph& g) {
        return MISSolver<Graph>(g).find(Algorithm::LubyImproved);
    });

    const std::size_t n = 100000;
    
    std::cout << "Generating Uniform Sparse Graph (n=" << n << ", p=0.0004)...\n";
    const auto g_uniform = generator.generate_sparse_uniform(n, 0.0004);
    bench.run("Uniform Sparse", g_uniform);

    std::cout << "\nGenerating Scale-Free Graph (n=" << n << ", m0=10, m=5)...\n";
    const auto g_scale_free = generator.generate_scale_free(n, 10, 5);
    bench.run("Scale-Free", g_scale_free);

    bench.print_results();

    return 0;
}
