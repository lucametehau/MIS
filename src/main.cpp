#include <iostream>
#include <vector>
#include <chrono>
#include "mis.h"
#include "graph_generator.h"
#include "mis_checker.h"

void benchmark(const std::string& name, const Graph& g) {
    MISChecker checker;
    std::cout << "\n--- Benchmark: " << name << " ---\n";

    {
        std::cout << "[Sequential]\n";
        MIS<Algorithm::Sequential> solver(g);
        auto start = std::chrono::high_resolution_clock::now();
        const auto mis = solver.find();
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << checker.check_mis(g, mis) << " | Size: " << mis.size() 
                  << " | Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0 << "ms\n";
    }

    {
        std::cout << "[Luby]\n";
        MIS<Algorithm::Luby> solver(g);
        auto start = std::chrono::high_resolution_clock::now();
        const auto mis = solver.find();
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << checker.check_mis(g, mis) << " | Size: " << mis.size() 
                  << " | Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0 << "ms\n";
    }

    {
        std::cout << "[Luby Improved]\n";
        MIS<Algorithm::LubyImproved> solver(g);
        auto start = std::chrono::high_resolution_clock::now();
        const auto mis = solver.find();
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << checker.check_mis(g, mis) << " | Size: " << mis.size() 
                  << " | Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0 << "ms\n";
    }
}

int main() {
    GraphGenerator generator;

    const std::size_t n = 1000000;
    
    std::cout << "Generating Uniform Sparse Graph (n=" << n << ", p=0.0004)...\n";
    const auto g_uniform = generator.generate_sparse_uniform(n, 0.0004);
    benchmark("Uniform Sparse", g_uniform);

    std::cout << "\nGenerating Scale-Free Graph (n=" << n << ", m0=10, m=5)...\n";
    const auto g_scale_free = generator.generate_scale_free(n, 10, 5);
    benchmark("Scale-Free (Social Network style)", g_scale_free);

    return 0;
}
