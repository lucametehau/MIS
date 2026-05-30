#pragma once

#include "mis.h"
#include "types.h"

#include <functional>
#include <stdexcept>
#include <string>

enum class Representation {
    Csr,
    Adjacency,
};

struct AlgorithmId {
    Algorithm algo = Algorithm::Sequential;
    std::size_t threads = 0;
};

inline Representation parse_representation(const std::string& s) {
    if (s == "csr") return Representation::Csr;
    if (s == "adjacency" || s == "adj") return Representation::Adjacency;
    throw std::runtime_error("unknown representation: " + s);
}

inline std::string representation_string(Representation r) {
    return r == Representation::Csr ? "csr" : "adjacency";
}

inline AlgorithmId parse_algorithm(const std::string& s) {
    if (s == "sequential") return {Algorithm::Sequential, 0};
    if (s == "luby") return {Algorithm::Luby, 0};
    if (s == "luby_improved") return {Algorithm::LubyImproved, 0};
    if (s == "luby_gpu") return {Algorithm::LubyGPU, 0};
    if (s == "luby_gpu2") return {Algorithm::LubyGPU2, 0};
    throw std::runtime_error("unknown algorithm: " + s);
}

inline std::function<NodeList(const Graph&)> make_adj_solver(AlgorithmId id) {
    return [id](const Graph& g) {
        return MISSolver<Graph>(g).find(id.algo, id.threads);
    };
}

inline std::function<NodeList(const GraphCSR&)> make_csr_solver(AlgorithmId id) {
    return [id](const GraphCSR& g) {
        return MISSolver<GraphCSR>(g).find(id.algo, id.threads);
    };
}
