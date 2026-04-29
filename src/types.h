#pragma once
#include <vector>
#include <iostream>
#include <cstdint>

// devices we will use for our analysis
enum class Device {
    CPU, GPU
};

using NodeList = std::vector<uint32_t>;
using Graph = std::vector<NodeList>;

inline std::ostream& operator << (std::ostream &os, const NodeList &list) {
    for (auto &x : list)
        os << x << " ";
    return os;
}

inline std::ostream& operator << (std::ostream &os, const Graph &graph) {
    const auto n = graph.size();
    for (std::size_t node = 0; node < n; node++) {
        os << node << ": " << graph[node] << (node == n - 1 ? "" : "\n");
    }
    return os;
}