#pragma once
#include <vector>
#include <iostream>
#include <cstdint>

using Node = uint32_t;
using NodeList = std::vector<Node>;
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

class GraphCSR {
public:
    GraphCSR() = default;

    // Build from adjacency-list graph
    explicit GraphCSR(const std::vector<NodeList>& graph) {
        build(graph);
    }

    void build(const std::vector<NodeList>& graph) {
        const std::size_t n = graph.size();

        offsets_.resize(n + 1);
        offsets_[0] = 0;

        // Compute prefix sums
        for (std::size_t u = 0; u < n; ++u) {
            offsets_[u + 1] = offsets_[u] + graph[u].size();
        }

        // Allocate edge storage
        edges_.resize(offsets_[n]);

        // Fill edges
        std::size_t idx = 0;
        for (const auto& neighbors : graph) {
            for (Node v : neighbors) {
                edges_[idx++] = v;
            }
        }
    }

    std::size_t size() const {
        return offsets_.size() - 1;
    }

    // Neighbor iterable view
    class NeighborRange {
    public:
        NeighborRange(const Node* begin, const Node* end)
            : begin_(begin), end_(end) {}

        const Node* begin() const { return begin_; }
        const Node* end() const { return end_; }

    private:
        const Node* begin_;
        const Node* end_;
    };

    NeighborRange operator[](std::size_t u) const {
        return NeighborRange(
            edges_.data() + offsets_[u],
            edges_.data() + offsets_[u + 1]
        );
    }

    // Optional accessors
    const std::vector<Node>& edges() const {
        return edges_;
    }

    const std::vector<std::size_t>& offsets() const {
        return offsets_;
    }

private:
    std::vector<std::size_t> offsets_;
    std::vector<Node> edges_;
};