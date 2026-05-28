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
    std::set<uint32_t> nodes_set;
    const auto n = g.size();

    for (std::size_t i = 0; i < n; i++)
        nodes_set.insert(i);

    while (!nodes_set.empty()) {
        const auto it = nodes_set.begin();
        const auto node = *it;
        nodes_set.erase(it);
        mis.push_back(node);

        for (auto &son : g[node]) {
            nodes_set.erase(son);
        }
    }

    return mis;
}

template <typename GraphT>
NodeList luby_mis(const GraphT& g) {
    const auto n = g.size();
    if (n == 0) return {};

    std::vector<uint8_t> is_active(n, 1); // not bool!!!
    std::vector<double> priorities(n, 0.0);
    NodeList mis;
    
    bool any_active = true;
    while (any_active) {
        // assign random priorities to active nodes
        #pragma omp parallel
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
        #pragma omp parallel
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
        #pragma omp parallel for reduction(||:any_active)
        for (std::size_t i = 0; i < n; i++) {
            if (is_active[i]) {
                any_active = true;
            }
        }
    }

    return mis;
}

template <typename GraphT>
NodeList luby_improved_mis(const GraphT& g) {
    const auto n = g.size();
    if (n == 0) return {};

    std::vector<uint8_t> is_active(n, 1);
    std::vector<double> priorities(n, 0.0);
    
    std::vector<uint32_t> active_nodes(n);
    std::iota(active_nodes.begin(), active_nodes.end(), 0);

    NodeList mis;

    while (!active_nodes.empty()) {
        const size_t num_active = active_nodes.size();

        #pragma omp parallel
        {
            static thread_local std::mt19937 gen(std::random_device{}());
            std::uniform_real_distribution<double> dist(0.0, 1.0);
            #pragma omp for schedule(static)
            for (std::size_t i = 0; i < num_active; i++) {
                priorities[active_nodes[i]] = dist(gen);
            }
        }

        std::vector<uint32_t> newly_added;
        #pragma omp parallel
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

        #pragma omp parallel for schedule(dynamic, 32)
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
        #pragma omp parallel
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

    NodeList find(Algorithm algo) {
        switch (algo) {
            case Algorithm::Sequential:
                return sequential_mis(g_);

            case Algorithm::Luby:
                return luby_mis(g_);

            case Algorithm::LubyImproved:
                return luby_improved_mis(g_);
        }

        return {};
    }

private:
    const GraphT& g_;
};
