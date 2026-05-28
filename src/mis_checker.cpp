#include "mis_checker.h"

std::string MISChecker::check_mis(const Graph &g, const NodeList &mis) const {
    const std::size_t n = g.size();

    std::vector<uint8_t> in_mis(n, 0);
    for (auto v : mis)
        in_mis[v] = 1;

    for (std::size_t node = 0; node < n; ++node) {
        uint32_t neighbor_in_mis = n;

        for (const auto &son : g[node]) {
            if (in_mis[son]) {
                neighbor_in_mis = son;
                break;
            }
        }

        if (in_mis[node]) {
            if (neighbor_in_mis < n)
                return "MIS incorrect! adjacent nodes found!";
        } else {
            if (neighbor_in_mis == n)
                return "MIS not maximal! node can be added!";
        }
    }

    return "MIS correct!";
}

double MISChecker::compute_mis_weight(const WeightedGraph& g, const NodeList& mis) const {
    double total_weight = 0.0;
    for (auto v : mis) {
        total_weight += g.weight(v);
    }
    return total_weight;
}