#include "mis.h"
#include <set>

template<>
NodeList MIS<Device::CPU>::find() {
    NodeList mis;
    std::set<uint32_t> nodes_set;
    const auto n = g_.size();

    for (std::size_t i = 0; i < n; i++)
        nodes_set.insert(i);

    while (!nodes_set.empty()) {
        const auto it = nodes_set.begin();
        const auto node = *it;
        nodes_set.erase(it);
        mis.push_back(node);

        for (auto &son : g_[node]) {
            nodes_set.erase(son);
        }
    }

    return mis;
}

template<>
NodeList MIS<Device::GPU>::find() {
    return NodeList{};
}