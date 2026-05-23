#include "graph_generator.h"
#include <random>
#include <algorithm>
#include <numeric>
#include <cmath>

GraphGenerator::GraphGenerator(uint32_t seed) : gen_(seed) {}

Graph GraphGenerator::generate_uniform(std::size_t n, double p) {
    Graph g(n);
    std::bernoulli_distribution dist(p);

    for (std::size_t i = 0; i < n; i++) {
        for (std::size_t j = i + 1; j < n; j++) {
            if (dist(gen_)) {
                g[i].push_back(j);
                g[j].push_back(i);
            }
        }
    }
    return g;
}

Graph GraphGenerator::generate_sparse_uniform(std::size_t n, double p) {
    Graph g(n);
    if (n < 2) return g;
    
    // Limit expected number of edges to 100 * n
    // Total possible edges: M = n * (n - 1) / 2
    // Expected edges: E = p * M
    // p * n * (n - 1) / 2 <= 100 * n  => p <= 200 / (n - 1)
    double p_max = 200.0 / (static_cast<double>(n) - 1.0);
    if (p > p_max) {
        p = p_max;
    }
    
    std::uniform_real_distribution<double> u_dist(0.0, 1.0);
    for (std::size_t i = 0; i < n; i++) {
        // geometric distribution to skip edges
        for (std::size_t j = i + 1; j < n; ) {
            double r = u_dist(gen_);
            if (p > 0) {
                std::size_t skip = std::log(1.0 - r) / std::log(1.0 - p);
                j += skip;
            } else {
                break;
            }

            if (j < n) {
                g[i].push_back(j);
                g[j].push_back(i);
                j++;
            }
        }
    }
    return g;
}

Graph GraphGenerator::generate_scale_free(std::size_t n, std::size_t m0, std::size_t m) {
    if (m0 < m) m0 = m;
    if (n < m0) n = m0;
    
    Graph g(n);
    std::vector<uint32_t> pool;

    // clique of size m0
    for (std::size_t i = 0; i < m0; i++) {
        for (std::size_t j = i + 1; j < m0; j++) {
            g[i].push_back(j);
            g[j].push_back(i);
            pool.push_back(i);
            pool.push_back(j);
        }
    }

    for (auto i = m0; i < n; i++) {
        std::vector<uint32_t> targets;
        std::uniform_int_distribution<std::size_t> dist(0, pool.size() - 1);
        
        while (targets.size() < m) {
            auto target = pool[dist(gen_)];
            if (std::find(targets.begin(), targets.end(), target) == targets.end()) {
                targets.push_back(target);
            }
        }

        for (auto target : targets) {
            g[i].push_back(target);
            g[target].push_back(i);
            pool.push_back(i);
            pool.push_back(target);
        }
    }

    return g;
}

GraphCSR GraphGenerator::generate_uniform_csr(
    std::size_t n,
    double p)
{
    return GraphCSR(generate_uniform(n, p));
}

GraphCSR GraphGenerator::generate_sparse_uniform_csr(
    std::size_t n,
    double p)
{
    return GraphCSR(generate_sparse_uniform(n, p));
}

GraphCSR GraphGenerator::generate_scale_free_csr(
    std::size_t n,
    std::size_t m0,
    std::size_t m)
{
    return GraphCSR(generate_scale_free(n, m0, m));
}