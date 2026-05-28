#pragma once
#include "types.h"

#include <set>
#include <random>
#include <algorithm>
#include <unordered_set>
#include <vector>
#include <numeric>
#include <omp.h>

template <typename GraphT>
NodeList sequential_mis(const GraphT& g) {
    NodeList mis;
    const auto n = g.size();
    std::vector<uint8_t> active(n, 1);

    for (std::size_t i = 0; i < n; i++) {
        if (active[i]) {
            mis.push_back(i);

            for (auto &son : g[i])
                active[son] = 0;
        }
    }

    return mis;
}

template <typename GraphT>
NodeList luby_mis(const GraphT& g, std::size_t num_threads = 0) {
    const auto n = g.size();
    if (n == 0) return {};

    std::vector<uint8_t> is_active(n, 1); // not bool!!!
    std::vector<double> priorities(n, 0.0);
    NodeList mis;

    auto threads = num_threads ? num_threads : omp_get_max_threads();
    
    bool any_active = true;
    while (any_active) {
        // assign random priorities to active nodes
        #pragma omp parallel num_threads(threads)
        {
            static thread_local std::mt19937 gen(std::random_device{}());
            std::uniform_real_distribution<double> dist(0.0, 1.0);
            #pragma omp for
            for (std::size_t node = 0; node < n; node++) {
                if (is_active[node])
                    priorities[node] = dist(gen);
            }
        }

        // identify nodes that have the maximum priority among their active neighbors
        std::vector<uint32_t> newly_added;
        #pragma omp parallel num_threads(threads)
        {
            std::vector<uint32_t> local_newly_added;
            #pragma omp for
            for (std::size_t node = 0; node < n; node++) {
                if (!is_active[node]) 
                    continue;

                bool is_max = true;
                for (auto son : g[node]) {
                    if (is_active[son]) {
                        if (priorities[son] > priorities[node] || (priorities[son] == priorities[node] && son > node)) {
                            is_max = false;
                            break;
                        }
                    }
                }

                if (is_max)
                    local_newly_added.push_back(node);
            }
            #pragma omp critical
            {
                newly_added.insert(newly_added.end(), local_newly_added.begin(), local_newly_added.end());
            }
        }

        for (auto node : newly_added) {
            mis.push_back(node);
            is_active[node] = 0;
            for (auto son : g[node]) {
                is_active[son] = 0;
            }
        }

        any_active = false;
        #pragma omp parallel for num_threads(threads) reduction(||:any_active)
        for (std::size_t i = 0; i < n; i++) {
            if (is_active[i]) {
                any_active = true;
            }
        }
    }

    return mis;
}

template <typename GraphT>
NodeList luby_improved_mis(const GraphT& g, std::size_t num_threads = 0) {
    const auto n = g.size();
    if (n == 0) return {};

    std::vector<uint8_t> is_active(n, 1);
    std::vector<double> priorities(n, 0.0);
    
    std::vector<uint32_t> active_nodes(n);
    std::iota(active_nodes.begin(), active_nodes.end(), 0);

    NodeList mis;

    auto threads = num_threads ? num_threads : omp_get_max_threads();

    while (!active_nodes.empty()) {
        const size_t num_active = active_nodes.size();

        #pragma omp parallel num_threads(threads)
        {
            static thread_local std::mt19937 gen(std::random_device{}());
            std::uniform_real_distribution<double> dist(0.0, 1.0);
            #pragma omp for schedule(static)
            for (std::size_t i = 0; i < num_active; i++) {
                priorities[active_nodes[i]] = dist(gen);
            }
        }

        std::vector<uint32_t> newly_added;
        #pragma omp parallel num_threads(threads)
        {
            std::vector<uint32_t> local_newly_added;
            #pragma omp for schedule(dynamic, 32)
            for (std::size_t i = 0; i < num_active; i++) {
                auto node = active_nodes[i];
                bool is_max = true;
                for (auto son : g[node]) {
                    if (is_active[son]) {
                        if (priorities[son] > priorities[node] || (priorities[son] == priorities[node] && son > node)) {
                            is_max = false;
                            break;
                        }
                    }
                }
                if (is_max)
                    local_newly_added.push_back(node);
            }
            #pragma omp critical
            {
                newly_added.insert(newly_added.end(), local_newly_added.begin(), local_newly_added.end());
            }
        }

        #pragma omp parallel for num_threads(threads) schedule(dynamic, 32)
        for (std::size_t i = 0; i < newly_added.size(); i++) {
            uint32_t node = newly_added[i];
            is_active[node] = 0;
            for (auto son : g[node]) {
                is_active[son] = 0;
            }
        }

        #pragma omp critical
        {
            mis.insert(mis.end(), newly_added.begin(), newly_added.end());
        }

        std::vector<uint32_t> next_active_nodes;
        #pragma omp parallel num_threads(threads)
        {
            std::vector<uint32_t> local_next;
            #pragma omp for schedule(static)
            for (std::size_t i = 0; i < num_active; i++) {
                uint32_t u = active_nodes[i];
                if (is_active[u]) {
                    local_next.push_back(u);
                }
            }
            #pragma omp critical
            {
                next_active_nodes.insert(next_active_nodes.end(), local_next.begin(), local_next.end());
            }
        }
        active_nodes = std::move(next_active_nodes);
    }

    return mis;
}

enum class Algorithm {
    Sequential,
    Luby,
    LubyImproved
};

template<typename GraphT>
class MISSolver {
public:
    MISSolver() = delete;

    explicit MISSolver(const GraphT& g)
        : g_(g) {}

    NodeList find(Algorithm algo, std::size_t num_threads = 0) {
        switch (algo) {
            case Algorithm::Sequential:
                return sequential_mis(g_);

            case Algorithm::Luby:
                return luby_mis(g_, num_threads);

            case Algorithm::LubyImproved:
                return luby_improved_mis(g_, num_threads);
        }

        return {};
    }

private:
    const GraphT& g_;
};
