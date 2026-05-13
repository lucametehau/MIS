#include <iostream>
#include <vector>
#include <chrono>
#include "mis.h"
#include "graph_generator.h"
#include "mis_checker.h"

int main() {
    GraphGenerator generator;
    MISChecker checker;

    const auto n = 1000;
    std::cout << "Generating graph with " << n << " nodes...\n";
    const auto g = generator.generate_uniform(n);
    
    {
        std::cout << "--- Sequential ---\n";
        MIS<Algorithm::Sequential> solver(g);
        auto start = std::chrono::high_resolution_clock::now();
        const auto mis = solver.find();
        auto end = std::chrono::high_resolution_clock::now();
        const auto msg = checker.check_mis(g, mis);
        std::cout << msg << "\n";
        std::cout << "MIS size: " << mis.size() << "\n";
        std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
    }

    {
        std::cout << "\n--- Luby ---\n";
        MIS<Algorithm::Luby> solver(g);
        auto start = std::chrono::high_resolution_clock::now();
        const auto mis = solver.find();
        auto end = std::chrono::high_resolution_clock::now();
        const auto msg = checker.check_mis(g, mis);
        std::cout << msg << "\n";
        std::cout << "MIS size: " << mis.size() << "\n";
        std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
    }

    return 0;
}