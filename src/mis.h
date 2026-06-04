#pragma once
#include "types.h"
#include "luby_gpu.h"

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

inline uint64_t splitmix64(uint64_t x) {
    x = (x + 0x9e3779b97f4a7c15);
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
    x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
    return x ^ (x >> 31);
}

template <typename GraphT>
NodeList luby_improved_mis(const GraphT& g, std::size_t num_threads = 0) {
    const auto n = g.size();
    if (n == 0) return {};

    const int threads = num_threads ? static_cast<int>(num_threads) : omp_get_max_threads();

    std::vector<uint8_t>  is_active(n, 1);
    std::vector<uint32_t> active_nodes(n);
    std::iota(active_nodes.begin(), active_nodes.end(), 0);

    NodeList mis;
    mis.reserve(n / 4);

    std::vector<uint32_t> newly_added;
    std::vector<uint32_t> next_active;
    next_active.reserve(n);

    uint32_t round_counter = 0;

    while (!active_nodes.empty()) {
        ++round_counter;
        const size_t na = active_nodes.size();

        newly_added.clear();

        #pragma omp parallel num_threads(threads)
        {
            auto prio_of = [&](uint32_t v) -> uint64_t {
                return splitmix64((uint64_t(round_counter) << 32) | uint64_t(v));
            };

            std::vector<uint32_t> my_max;

            #pragma omp for schedule(dynamic, 256)
            for (size_t i = 0; i < na; i++) {
                const uint32_t v = active_nodes[i];
                if (!is_active[v]) continue;

                const uint64_t pv = prio_of(v);
                bool is_max = true;
                for (uint32_t u : g[v]) {
                    if (!is_active[u]) continue;
                    const uint64_t pu = prio_of(u);
                    if (pu > pv || (pu == pv && u > v)) {
                        is_max = false;
                        break;
                    }
                }
                if (is_max) my_max.push_back(v);
            }

            for (uint32_t v : my_max) {
                is_active[v] = 0;
                for (uint32_t u : g[v]) is_active[u] = 0;
            }

            #pragma omp critical
            {
                newly_added.insert(newly_added.end(), my_max.begin(), my_max.end());
            }
        }

        mis.insert(mis.end(), newly_added.begin(), newly_added.end());

        next_active.clear();
        #pragma omp parallel num_threads(threads)
        {
            std::vector<uint32_t> my_surv;
            #pragma omp for schedule(static) nowait
            for (size_t i = 0; i < na; i++) {
                const uint32_t u = active_nodes[i];
                if (is_active[u]) my_surv.push_back(u);
            }

            #pragma omp critical
            {
                next_active.insert(next_active.end(), my_surv.begin(), my_surv.end());
            }
        }

        active_nodes.swap(next_active);
    }

    return mis;
}


enum class Algorithm {
    Sequential,
    Luby,
    LubyImproved,
    LubyGPU,
    LubyGPU2
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

            case Algorithm::LubyGPU:
                if constexpr (std::is_same_v<GraphT, GraphCSR>) {
                    return luby_gpu_mis(g_);
                } else {
                    throw std::runtime_error("Please use CSR for GPU version of Luby");
                }

            // case Algorithm::LubyGPU2:
            //     if constexpr (std::is_same_v<GraphT, GraphCSR>) {
            //         return luby_gpu_mis2(g_);
            //     } else {
            //         throw std::runtime_error("Please use CSR for GPU version of Luby");
            //     }
        }

        return {};
    }

private:
    const GraphT& g_;
};
