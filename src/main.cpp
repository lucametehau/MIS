#include <iostream>
#include <vector>
#include "mis.h"
#include "graph_generator.h"
#include "benchmark.h"

int main() {
    GraphGenerator generator;
    Benchmarker bench(5);

    bench.add_algorithm("Sequential", [](const Graph& g) {
        MIS<Algorithm::Sequential> solver(g);
        return solver.find();
    });

    bench.add_algorithm("Luby", [](const Graph& g) {
        MIS<Algorithm::Luby> solver(g);
        return solver.find();
    });

    bench.add_algorithm("Luby Improved", [](const Graph& g) {
        MIS<Algorithm::LubyImproved> solver(g);
        return solver.find();
    });

    const std::size_t n = 1000000;
    
    std::cout << "Generating Uniform Sparse Graph (n=" << n << ", p=0.0004)...\n";
    const auto g_uniform = generator.generate_sparse_uniform(n, 0.0004);
    bench.run("Uniform Sparse", g_uniform);

    std::cout << "\nGenerating Scale-Free Graph (n=" << n << ", m0=10, m=5)...\n";
    const auto g_scale_free = generator.generate_scale_free(n, 10, 5);
    bench.run("Scale-Free", g_scale_free);

    bench.print_results();

    return 0;
}
