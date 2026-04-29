#include "graph_generator.h"
#include <random>

// Erdős-Rényi G(n, 0.5) model
// O(N^2) complexity
Graph GraphGenerator::generate_uniform(std::size_t n) {
    Graph g(n);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution dist(0.5);

    for (std::size_t i = 0; i < n; i++) {
        for (std::size_t j = i + 1; j < n; j++) {
            if (dist(gen))
                g[i].push_back(j);
        }
    }

    return g;
}