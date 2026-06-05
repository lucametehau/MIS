#include "graph_cache.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace {

constexpr uint32_t kCsrMagic = 0x47524353; // 'GRCS'
constexpr uint32_t kAdjMagic = 0x47524144; // 'GRAD'
constexpr uint32_t kFormatVersion = 1;

std::string family_token(GraphFamily family) {
    switch (family) {
        case GraphFamily::UniformSparse: return "uniform_sparse";
        case GraphFamily::ScaleFree: return "scale_free";
    }
    return "unknown";
}

template <typename T>
bool write_pod(std::ofstream& out, const T& value) {
    out.write(reinterpret_cast<const char*>(&value), sizeof(T));
    return static_cast<bool>(out);
}

template <typename T>
bool read_pod(std::ifstream& in, T& value) {
    in.read(reinterpret_cast<char*>(&value), sizeof(T));
    return static_cast<bool>(in);
}

} // namespace

std::string graph_family_string(GraphFamily family) {
    return family_token(family);
}

std::string cache_filename_csr(const GraphSpec& spec, uint32_t seed) {
    std::ostringstream oss;
    oss << family_token(spec.family) << "_n" << spec.n;
    if (spec.family == GraphFamily::UniformSparse) {
        oss << "_c" << std::fixed << std::setprecision(6) << spec.c;
    } else {
        oss << "_m0" << spec.m0 << "_m" << spec.m;
    }
    oss << "_seed" << seed << ".grph";
    return oss.str();
}

std::string cache_filename_adj(const GraphSpec& spec, uint32_t seed) {
    auto name = cache_filename_csr(spec, seed);
    name.replace(name.size() - 5, 5, ".adj");
    return name;
}

GraphCSR build_csr(const GraphSpec& spec, uint32_t seed) {
    GraphGenerator gen(seed);
    switch (spec.family) {
        case GraphFamily::UniformSparse:
            return gen.generate_sparse_uniform_csr(spec.n, spec.c);
        case GraphFamily::ScaleFree:
            return gen.generate_scale_free_csr(spec.n, spec.m0, spec.m);
    }
    return GraphCSR();
}

Graph build_adj(const GraphSpec& spec, uint32_t seed) {
    GraphGenerator gen(seed);
    switch (spec.family) {
        case GraphFamily::UniformSparse:
            return gen.generate_sparse_uniform(spec.n, spec.c);
        case GraphFamily::ScaleFree:
            return gen.generate_scale_free(spec.n, spec.m0, spec.m);
    }
    return Graph();
}

bool save_graph_csr(const GraphCSR& g, const std::filesystem::path& path) {
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;

    const auto& offsets = g.offsets();
    const auto& edges = g.edges();
    const uint64_t n = g.size();
    const uint64_t num_offsets = offsets.size();
    const uint64_t num_edges = edges.size();

    if (!write_pod(out, kCsrMagic)) return false;
    if (!write_pod(out, kFormatVersion)) return false;
    if (!write_pod(out, n)) return false;
    if (!write_pod(out, num_offsets)) return false;
    if (!write_pod(out, num_edges)) return false;

    out.write(reinterpret_cast<const char*>(offsets.data()),
              static_cast<std::streamsize>(num_offsets * sizeof(uint32_t)));
    out.write(reinterpret_cast<const char*>(edges.data()),
              static_cast<std::streamsize>(num_edges * sizeof(Node)));
    return static_cast<bool>(out);
}

bool load_graph_csr(GraphCSR& g, const std::filesystem::path& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return false;

    uint32_t magic = 0;
    uint32_t version = 0;
    uint64_t n = 0;
    uint64_t num_offsets = 0;
    uint64_t num_edges = 0;

    if (!read_pod(in, magic) || magic != kCsrMagic) return false;
    if (!read_pod(in, version) || version != kFormatVersion) return false;
    if (!read_pod(in, n)) return false;
    if (!read_pod(in, num_offsets)) return false;
    if (!read_pod(in, num_edges)) return false;

    std::vector<uint32_t> offsets(num_offsets);
    std::vector<Node> edges(num_edges);

    in.read(reinterpret_cast<char*>(offsets.data()),
            static_cast<std::streamsize>(num_offsets * sizeof(uint32_t)));
    in.read(reinterpret_cast<char*>(edges.data()),
            static_cast<std::streamsize>(num_edges * sizeof(Node)));

    if (!in) return false;

    g = GraphCSR(std::move(offsets), std::move(edges));
    if (g.size() != n) return false;
    return true;
}

bool save_graph_adj(const Graph& g, const std::filesystem::path& path) {
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;

    const uint64_t n = g.size();
    if (!write_pod(out, kAdjMagic)) return false;
    if (!write_pod(out, kFormatVersion)) return false;
    if (!write_pod(out, n)) return false;

    for (uint64_t u = 0; u < n; ++u) {
        const uint64_t deg = g[u].size();
        if (!write_pod(out, deg)) return false;
        if (deg > 0) {
            out.write(reinterpret_cast<const char*>(g[u].data()),
                      static_cast<std::streamsize>(deg * sizeof(Node)));
        }
    }
    return static_cast<bool>(out);
}

bool load_graph_adj(Graph& g, const std::filesystem::path& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return false;

    uint32_t magic = 0;
    uint32_t version = 0;
    uint64_t n = 0;

    if (!read_pod(in, magic) || magic != kAdjMagic) return false;
    if (!read_pod(in, version) || version != kFormatVersion) return false;
    if (!read_pod(in, n)) return false;

    g.resize(static_cast<std::size_t>(n));
    for (uint64_t u = 0; u < n; ++u) {
        uint64_t deg = 0;
        if (!read_pod(in, deg)) return false;
        g[u].resize(deg);
        if (deg > 0) {
            in.read(reinterpret_cast<char*>(g[u].data()),
                    static_cast<std::streamsize>(deg * sizeof(Node)));
        }
    }
    return static_cast<bool>(in);
}

GraphCSR get_or_build_csr(
    const GraphSpec& spec,
    uint32_t seed,
    const std::filesystem::path& cache_dir,
    bool warm_only
) {
    std::filesystem::create_directories(cache_dir);
    const auto path = cache_dir / cache_filename_csr(spec, seed);

    GraphCSR g;
    if (std::filesystem::exists(path) && load_graph_csr(g, path)) {
        std::cout << "  [cache] loaded CSR " << path.filename().string() << "\n";
        return g;
    }

    std::cout << "  [cache] generating CSR " << path.filename().string() << " ...\n";
    g = build_csr(spec, seed);
    if (!save_graph_csr(g, path)) {
        std::cerr << "  [cache] warning: failed to save " << path << "\n";
    } else {
        std::cout << "  [cache] saved CSR " << path.filename().string() << "\n";
    }

    if (warm_only) {
        std::cout << "  [warm-cache] graph ready (skipping benchmarks for this instance)\n";
    }

    return g;
}

Graph get_or_build_adj(
    const GraphSpec& spec,
    uint32_t seed,
    const std::filesystem::path& cache_dir,
    bool warm_only
) {
    std::filesystem::create_directories(cache_dir);
    const auto path = cache_dir / cache_filename_adj(spec, seed);

    Graph g;
    if (std::filesystem::exists(path) && load_graph_adj(g, path)) {
        std::cout << "  [cache] loaded adjacency " << path.filename().string() << "\n";
        return g;
    }

    std::cout << "  [cache] generating adjacency " << path.filename().string() << " ...\n";
    g = build_adj(spec, seed);
    if (!save_graph_adj(g, path)) {
        std::cerr << "  [cache] warning: failed to save " << path << "\n";
    } else {
        std::cout << "  [cache] saved adjacency " << path.filename().string() << "\n";
    }

    if (warm_only) {
        std::cout << "  [warm-cache] graph ready (skipping benchmarks for this instance)\n";
    }

    return g;
}
