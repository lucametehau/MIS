#pragma once

#include "types.h"
#include "graph_generator.h"

#include <cstdint>
#include <filesystem>
#include <string>

enum class GraphFamily {
    UniformSparse,
    ScaleFree,
};

struct GraphSpec {
    GraphFamily family = GraphFamily::UniformSparse;
    std::size_t n = 0;
    double c = 10.0;
    std::size_t m0 = 10;
    std::size_t m = 5;
};

std::string cache_filename_csr(const GraphSpec& spec, uint32_t seed);
std::string cache_filename_adj(const GraphSpec& spec, uint32_t seed);

bool save_graph_csr(const GraphCSR& g, const std::filesystem::path& path);
bool load_graph_csr(GraphCSR& g, const std::filesystem::path& path);

bool save_graph_adj(const Graph& g, const std::filesystem::path& path);
bool load_graph_adj(Graph& g, const std::filesystem::path& path);

GraphCSR get_or_build_csr(
    const GraphSpec& spec,
    uint32_t seed,
    const std::filesystem::path& cache_dir,
    bool warm_only = false
);

Graph get_or_build_adj(
    const GraphSpec& spec,
    uint32_t seed,
    const std::filesystem::path& cache_dir,
    bool warm_only = false
);

GraphCSR build_csr(const GraphSpec& spec, uint32_t seed);
Graph build_adj(const GraphSpec& spec, uint32_t seed);

std::string graph_family_string(GraphFamily family);
