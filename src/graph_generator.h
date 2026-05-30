#pragma once
#include "types.h"
#include <random>

class GraphGenerator {
public:
    GraphGenerator(uint32_t seed = 42);

    // O(N^2) complexity - standard Erdős-Rényi G(n, p)
    Graph generate_uniform(std::size_t n, double p = 0.5);

    // O(N + M) complexity for sparse graphs
    Graph generate_sparse_uniform(std::size_t n, double c);

    // Barabási–Albert model for scale-free graphs (unbalanced)
    // n: total number of nodes
    // m0: initial number of nodes (fully connected)
    // m: number of edges to attach from a new node to existing nodes
    Graph generate_scale_free(std::size_t n, std::size_t m0, std::size_t m);

    GraphCSR generate_uniform_csr(std::size_t n, double p);

    GraphCSR generate_sparse_uniform_csr(std::size_t n, double c);

    GraphCSR generate_scale_free_csr(
        std::size_t n,
        std::size_t m0,
        std::size_t m
    );

    // generates random weights for each vertex in the graph
    // 3 differnet approaches for assigning weights used later for benchmarking
    WeightedGraph add_weights_uniform(const Graph& g, double min_w = 1.0, double max_w = 10.0);
    WeightedGraph add_weights_exp(const Graph& g);
    WeightedGraph add_weights_clustered(const Graph& g, double low_weight = 1.0, double high_weight = 10.0, double fraction = 0.1);


private:
    std::mt19937 gen_;
    uint32_t seed_;
};
