#pragma once
#include "types.h"
#include <vector>
#include <random>
#include <cmath>
#include <omp.h>

// weighhted MIS: 3 approaches comparing how weight-biased priority affects total MIS weight

// aproach 1: greedy
// we select vertices with the highest weight among active neihgbors
NodeList weighted_greedy_mis(const WeightedGraph& g) {
    const auto n = g.size();
    if (n == 0) return {};

    std::vector<uint8_t> is_active(n, 1);
    NodeList mis;

    bool any_active = true;
    while (any_active) {
        std::vector<uint32_t> newly_added;
        #pragma omp parallel
        {
            std::vector<uint32_t> local_newly_added;
            #pragma omp for
            for (std::size_t node = 0; node < n; node++) {
                if (!is_active[node]) {
                    continue;
                }
                bool is_max = true;
                for (auto son : g[node]) {
                    if (is_active[son]) {
                        if (g.weight(son) > g.weight(node) ||
                            (g.weight(son) == g.weight(node) && son > node)) {
                            is_max = false;
                            break;
                        }
                    }
                }
                if (is_max) {
                    local_newly_added.push_back(node);
                }
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

// approach 2: weighted luby 
// we have random priorty using weighted random sampling
// random priority = uniform^(1/weight), thus higher wight= higher priority

NodeList weighted_sampling_mis(const WeightedGraph& g) {
    const auto n = g.size();
    if (n == 0) return {};

    std::vector<uint8_t> is_active(n, 1); 
    std::vector<double> priorities(n, 0.0);
    NodeList mis;

    bool any_active = true;
    while (any_active) {
        #pragma omp parallel
        {
            static thread_local std::mt19937 gen(std::random_device{}());
            std::uniform_real_distribution<double> dist(0.0, 1.0);
            #pragma omp for
            for (std::size_t node = 0; node < n; node++) {
                if (is_active[node])
                    priorities[node] = std::pow(dist(gen), 1.0 / g.weight(node));
            }
        }

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

                if (is_max) {
                    local_newly_added.push_back(node);
                }
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