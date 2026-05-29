#include "graph_generator.h"
#include <random>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <omp.h>
#include <atomic>

GraphGenerator::GraphGenerator(uint32_t seed) : gen_(seed), seed_(seed) {}

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
    std::vector<std::size_t> pool;

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
    if (n < 2)
        return GraphCSR();

    // Keep graph sparse
    double p_max = 5.0 / (static_cast<double>(n) - 1.0);
    if (p > p_max)
        p = p_max;

    const int nt = omp_get_max_threads();

    // ---------------------------------------------------------------------
    // Phase 1:
    // Parallel edge generation into thread-local edge lists
    // ---------------------------------------------------------------------

    using Edge = std::pair<std::size_t, std::size_t>;

    std::vector<std::vector<Edge>> local_edges(nt);

    #pragma omp parallel
    {
        const int tid = omp_get_thread_num();

        std::mt19937 gen(seed_ + tid); // using the fixed seed plus thread id for reproducibility

        std::uniform_real_distribution<double> u_dist(0.0, 1.0);

        auto& edges = local_edges[tid];

        #pragma omp for schedule(static)
        for (std::size_t i = 0; i < n; i++) {

            for (std::size_t j = i + 1; j < n; ) {

                double r = u_dist(gen);

                std::size_t skip =
                    static_cast<std::size_t>(
                        std::log(1.0 - r) / std::log(1.0 - p));

                j += skip;

                if (j < n) {
                    edges.emplace_back(i, j);
                    j++;
                }
            }
        }
    }

    // ---------------------------------------------------------------------
    // Phase 2:
    // Degree counting
    // ---------------------------------------------------------------------

    std::vector<std::size_t> degree(n, 0);

    #pragma omp parallel for schedule(static)
    for (int tid = 0; tid < nt; tid++) {

        for (const auto& [u, v] : local_edges[tid]) {

            #pragma omp atomic
            degree[u]++;

            #pragma omp atomic
            degree[v]++;
        }
    }

    // ---------------------------------------------------------------------
    // Phase 3:
    // Prefix sum -> offsets
    // ---------------------------------------------------------------------

    std::vector<uint32_t> offsets(n + 1, 0);

    for (std::size_t i = 0; i < n; i++) {
        offsets[i + 1] = offsets[i] + degree[i];
    }

    // ---------------------------------------------------------------------
    // Phase 4:
    // Allocate CSR edge array
    // ---------------------------------------------------------------------

    std::vector<Node> edges(offsets[n]);

    // Write cursors
    std::vector<std::atomic<std::size_t>> cursor(n);

    for (std::size_t i = 0; i < n; i++) {
        cursor[i].store(offsets[i], std::memory_order_relaxed);
    }

    // ---------------------------------------------------------------------
    // Phase 5:
    // Parallel CSR fill
    // ---------------------------------------------------------------------

    #pragma omp parallel for schedule(static)
    for (int tid = 0; tid < nt; tid++) {

        for (const auto& [u, v] : local_edges[tid]) {

            std::size_t pu =
                cursor[u].fetch_add(1, std::memory_order_relaxed);

            std::size_t pv =
                cursor[v].fetch_add(1, std::memory_order_relaxed);

            edges[pu] = v;
            edges[pv] = u;
        }
    }

    // ---------------------------------------------------------------------
    // Phase 6:
    // Sort neighbors for deterministic traversal order
    // ---------------------------------------------------------------------

    #pragma omp parallel for schedule(dynamic, 1024)
    for (std::size_t u = 0; u < n; u++) {

        std::sort(
            edges.begin() + offsets[u],
            edges.begin() + offsets[u + 1]);
    }

    return GraphCSR(
        std::move(offsets),
        std::move(edges));
}


GraphCSR GraphGenerator::generate_scale_free_csr(
    std::size_t n,
    std::size_t m0,
    std::size_t m)
{
    return GraphCSR(generate_scale_free(n, m0, m));
}

WeightedGraph GraphGenerator::add_weights_uniform(const Graph& g, double min_w, double max_w) {
    std::uniform_real_distribution<double> dist(min_w, max_w);
    std::vector<double> weights(g.size());
    for (auto &w : weights) {
        w = dist(gen_);
    }
    return  WeightedGraph{g, std::move(weights)};
}

WeightedGraph GraphGenerator::add_weights_exp(const Graph& g) {
    std::exponential_distribution<double> dist(0.5);
    std::vector<double> weights(g.size());
    for (auto& w : weights) {
        w = 1.0 + dist(gen_);
    }
    return WeightedGraph{g, std::move(weights)};
}

// we assign high weigts to an "important" cluser of nodes (which account for a fractaion of all nodes), and then we assign low weight to the rest
WeightedGraph GraphGenerator::add_weights_clustered(const Graph& g, double low_weight, double high_weight, double fraction) {
    std::bernoulli_distribution dist(fraction);
    std::vector<double> weights(g.size());
    for (auto &w : weights) {
        if (dist(gen_)) {
            w = high_weight;  
        } else {
            w = low_weight; 
        }
    }
    return WeightedGraph{g, std::move(weights)};
}