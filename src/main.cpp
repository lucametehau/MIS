#include <iostream>
#include <vector>
#include "mis.h"
#include "graph_generator.h"
#include "mis_checker.h"

int main() {
    GraphGenerator generator;
    MISChecker checker;

    const auto g = generator.generate_uniform(10);
    MIS<Algorithm::Sequential> solver(g);
    const auto mis = solver.find();

    const auto msg = checker.check_mis(g, mis);
    std::cout << msg << "\n";
    
    std::cout << "Graph:\n" << g << "\n\n";
    std::cout << "MIS:\n" << mis << "\n";

    return 0;
}