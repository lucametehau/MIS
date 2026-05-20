#pragma once
#include "types.h"

class GraphGenerator {
public:
    GraphGenerator() = default;

    // O(N^2) complexity - standard Erdős-Rényi G(n, p)
    Graph generate_uniform(std::size_t n, double p = 0.5);

    // O(N + M) complexity for sparse graphs
    Graph generate_sparse_uniform(std::size_t n, double p);

    // Barabási–Albert model for scale-free graphs (unbalanced)
    // n: total number of nodes
    // m0: initial number of nodes (fully connected)
    // m: number of edges to attach from a new node to existing nodes
    Graph generate_scale_free(std::size_t n, std::size_t m0, std::size_t m);
};
