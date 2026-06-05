#include "benchmark_config.h"

#include "benchmark_runner.h"
#include "graph_cache.h"
#include "algorithm_registry.h"

#include <json.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <omp.h>

using json = nlohmann::json;

namespace {

GraphFamily parse_graph_family(const std::string& s) {
    if (s == "uniform_sparse") return GraphFamily::UniformSparse;
    if (s == "scale_free") return GraphFamily::ScaleFree;
    throw std::runtime_error("unknown graph family: " + s);
}

GraphSpec parse_graph_spec(const json& g) {
    GraphSpec spec;
    spec.family = parse_graph_family(g.at("family").get<std::string>());
    spec.n = g.at("n").get<std::size_t>();
    if (spec.family == GraphFamily::UniformSparse) {
        spec.c = g.value("c", 10.0);
    } else {
        spec.m0 = g.value("m0", static_cast<std::size_t>(10));
        spec.m = g.value("m", static_cast<std::size_t>(5));
    }
    return spec;
}

BenchmarkCsvRow make_meta_row(
    const std::string& suite,
    const std::string& label,
    const GraphSpec& spec,
    Representation repr,
    std::size_t threads
) {
    BenchmarkCsvRow row;
    row.suite = suite;
    row.label = label;
    row.graph_family = graph_family_string(spec.family);
    row.n = spec.n;
    row.c = spec.c;
    row.m0 = spec.m0;
    row.m = spec.m;
    row.representation = representation_string(repr);
    row.threads = threads;
    return row;
}

struct ParsedCase {
    std::string name;
    AlgorithmId algo_id;
    Representation repr;
};

ParsedCase parse_case(const json& c) {
    ParsedCase pc;
    pc.name = c.at("name").get<std::string>();
    pc.algo_id = parse_algorithm(c.at("algorithm").get<std::string>());
    pc.repr = parse_representation(c.at("representation").get<std::string>());
    if (c.contains("threads")) {
        pc.algo_id.threads = c.at("threads").get<std::size_t>();
    }
    if (pc.algo_id.algo != Algorithm::Sequential && pc.algo_id.threads == 0) {
        pc.algo_id.threads = omp_get_max_threads();
    }
    return pc;
}

struct ParsedExperiment {
    std::string suite;
    std::string label;
    GraphSpec graph;
    int nr_graphs;
    int nr_runs;
    bool verify;
    std::vector<ParsedCase> cases;
};

int count_work(const std::vector<ParsedExperiment>& experiments) {
    int total = 0;
    for (const auto& exp : experiments) {
        total += exp.nr_graphs * static_cast<int>(exp.cases.size());
    }
    return total;
}

} // namespace

int run_from_config(const ConfigRunOptions& options) {
    std::ifstream in(options.config_path);
    if (!in) {
        std::cerr << "Failed to open config: " << options.config_path << "\n";
        return 1;
    }

    json root;
    try {
        in >> root;
    } catch (const json::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << "\n";
        return 1;
    }

    const std::string output_path = options.output_override.empty()
        ? root.value("output", std::string("results/benchmark.csv"))
        : options.output_override;
    const uint32_t base_seed = root.value("base_seed", 42u);
    const bool default_verify = root.value("verify", false);
    const std::string cache_dir_str = root.value("cache_dir", std::string("cache/graphs"));
    const std::filesystem::path cache_dir(cache_dir_str);

    std::vector<ParsedExperiment> experiments;
    for (const auto& exp_json : root.at("experiments")) {
        ParsedExperiment exp;
        exp.suite = exp_json.value("suite", std::string(""));
        exp.label = exp_json.at("label").get<std::string>();
        exp.graph = parse_graph_spec(exp_json.at("graph"));
        exp.nr_graphs = exp_json.value("nr_graphs", root.value("nr_graphs", 10));
        exp.nr_runs = exp_json.value("nr_runs", root.value("nr_runs", 5));
        exp.verify = exp_json.value("verify", default_verify);
        for (const auto& c : exp_json.at("cases")) {
            exp.cases.push_back(parse_case(c));
        }
        experiments.push_back(std::move(exp));
    }

    std::cout << "Config: " << options.config_path << "\n"
              << "  Output:    " << output_path << "\n"
              << "  Cache dir: " << cache_dir << "\n"
              << "  Verify:    " << (default_verify ? "yes (default)" : "no (default)") << "\n"
              << "  Experiments: " << experiments.size() << "\n"
              << "  Graph×case cells: " << count_work(experiments) << "\n";

    if (options.dry_run) {
        int cell = 0;
        for (const auto& exp : experiments) {
            std::cout << "\n[" << exp.suite << "] " << exp.label
                      << " (n=" << exp.graph.n
                      << ", graphs=" << exp.nr_graphs
                      << ", runs=" << exp.nr_runs << ")\n";
            for (const auto& c : exp.cases) {
                std::cout << "  - " << c.name << " (" << representation_string(c.repr)
                          << ", threads=" << c.algo_id.threads << ")\n";
                ++cell;
            }
        }
        std::cout << "\n[dry-run] Would execute " << count_work(experiments)
                  << " graph instances × cases (see above).\n";
        return 0;
    }

    if (options.warm_cache) {
        std::cout << "\n[warm-cache] Populating graph cache only...\n";
    }

    std::filesystem::create_directories(std::filesystem::path(output_path).parent_path());

    std::ofstream csv_out;
    if (!options.warm_cache) {
        csv_out.open(output_path, std::ios::trunc);
        if (!csv_out) {
            std::cerr << "Failed to open output: " << output_path << "\n";
            return 1;
        }
        write_csv_header(csv_out);
    }

    int exp_index = 0;
    for (const auto& exp : experiments) {
        ++exp_index;
        std::cout << "\n=== Experiment " << exp_index << "/" << experiments.size()
                  << " [" << exp.suite << "] " << exp.label << " ===\n"
                  << "  family=" << graph_family_string(exp.graph.family)
                  << " n=" << exp.graph.n;
        if (exp.graph.family == GraphFamily::UniformSparse) {
            std::cout << " c=" << exp.graph.c;
        } else {
            std::cout << " m0=" << exp.graph.m0 << " m=" << exp.graph.m;
        }
        std::cout << " graphs=" << exp.nr_graphs << " runs=" << exp.nr_runs
                  << " verify=" << (exp.verify ? "yes" : "no") << "\n";

        bool has_csr = false;
        bool has_adj = false;
        for (const auto& c : exp.cases) {
            if (c.repr == Representation::Csr) has_csr = true;
            else has_adj = true;
        }

        if (options.warm_cache) {
            for (int graph_idx = 0; graph_idx < exp.nr_graphs; ++graph_idx) {
                const uint32_t seed = base_seed + static_cast<uint32_t>(graph_idx);
                std::cout << "  Graph instance " << (graph_idx + 1) << "/"
                          << exp.nr_graphs << " (seed=" << seed << ")\n";
                if (has_csr) {
                    (void)get_or_build_csr(exp.graph, seed, cache_dir, true);
                }
                if (has_adj) {
                    (void)get_or_build_adj(exp.graph, seed, cache_dir, true);
                }
            }
            continue;
        }

        std::vector<GraphCSR> csr_graphs;
        std::vector<Graph> adj_graphs;
        csr_graphs.reserve(exp.nr_graphs);
        adj_graphs.reserve(exp.nr_graphs);

        for (int graph_idx = 0; graph_idx < exp.nr_graphs; ++graph_idx) {
            const uint32_t seed = base_seed + static_cast<uint32_t>(graph_idx);
            std::cout << "\n  Loading graph " << (graph_idx + 1) << "/"
                      << exp.nr_graphs << " (seed=" << seed << ")\n";
            if (has_csr) {
                csr_graphs.push_back(get_or_build_csr(exp.graph, seed, cache_dir, false));
            }
            if (has_adj) {
                adj_graphs.push_back(get_or_build_adj(exp.graph, seed, cache_dir, false));
            }
        }

        for (const auto& c : exp.cases) {
            std::cout << "\n  Running case: " << c.name << "\n";
            auto meta = make_meta_row(exp.suite, exp.label, exp.graph, c.repr, c.algo_id.threads);

            BenchmarkCsvRow row;
            if (c.repr == Representation::Csr) {
                auto solver = make_csr_solver(c.algo_id);
                row = run_single_case_on_graphs(
                    csr_graphs, solver, c.name, exp.nr_runs, exp.verify, meta);
            } else {
                auto solver = make_adj_solver(c.algo_id);
                row = run_single_case_on_graphs(
                    adj_graphs, solver, c.name, exp.nr_runs, exp.verify, meta);
            }

            if (csv_out) {
                append_csv_row(csv_out, row);
                csv_out.flush();
            }
            std::cout << "    -> mean " << row.mean_time_ms << " ms"
                      << " (graph std " << row.graph_std_ms << " ms)\n";
        }
    }

    if (options.warm_cache) {
        std::cout << "\n[warm-cache] Done. Graphs stored under " << cache_dir << "\n";
    } else {
        std::cout << "\nResults written to " << output_path << "\n";
    }

    return 0;
}
