#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include "mis.h"
#include "graph_generator.h"
#include "benchmark.h"
#include "benchmark_config.h"
#include "weighted_mis.h"
#include "weighted_benchmark.h"

struct LegacyConfig {
    int nr_graphs = 5;
    int nr_runs = 10;
    std::size_t n = 5000000;
    double c = 10.0;
    uint32_t base_seed = 42;
    bool verify = true;
    bool run_normal = false;
    bool run_csr = false;
    bool run_weighted = false;
};

void print_usage(const char* prog_name) {
    std::cout << "Usage: " << prog_name << " [options]\n\n"
              << "Config-driven benchmarks:\n"
              << "  --config <file>       Run experiments from JSON config\n"
              << "  --output <file>       Override config output CSV path\n"
              << "  --warm-cache          Only generate/load graphs into cache\n"
              << "  --dry-run             Print planned experiments, no execution\n\n"
              << "Legacy options:\n"
              << "  -g, --graphs <num>    Number of graphs to generate (default: 5)\n"
              << "  -r, --runs <num>      Number of runs per graph (default: 10)\n"
              << "  -n <num>              Number of nodes in the graph (default: 5000000)\n"
              << "  -c <num>              Constant for sparse graph (edges = c*n) (default: 10.0)\n"
              << "  -v, --verify <bool>   Verify results (default: true)\n"
              << "  --normal              Run normal graph benchmarks\n"
              << "  --csr                 Run CSR graph benchmarks\n"
              << "  --weighted            Run weighted graph benchmarks\n"
              << "  --all                 Run all legacy benchmarks\n"
              << "  --help                Show this help message\n";
}

void run_normal_benchmarks(const LegacyConfig& cfg) {
    Benchmarker<Graph> bench(cfg.nr_runs, cfg.verify);

    bench.add_algorithm("Sequential", [](const Graph& g) {
        return MISSolver<Graph>(g).find(Algorithm::Sequential);
    });

    bench.add_algorithm("Luby 1t", [](const Graph& g) {
        return MISSolver<Graph>(g).find(Algorithm::Luby, 1);
    });

    bench.add_algorithm("Luby 2t", [](const Graph& g) {
        return MISSolver<Graph>(g).find(Algorithm::Luby, 2);
    });

    bench.add_algorithm("Luby 4t", [](const Graph& g) {
        return MISSolver<Graph>(g).find(Algorithm::Luby, 4);
    });

    bench.add_algorithm("Luby 8t", [](const Graph& g) {
        return MISSolver<Graph>(g).find(Algorithm::Luby, 8);
    });

    bench.add_algorithm("Luby", [](const Graph& g) {
        return MISSolver<Graph>(g).find(Algorithm::Luby);
    });

    bench.add_algorithm("Luby Improved", [](const Graph& g) {
        return MISSolver<Graph>(g).find(Algorithm::LubyImproved);
    });

    std::cout << "\nBenchmarking Uniform Sparse (n=" << cfg.n << ", c=" << cfg.c << ")...\n";
    bench.run_suite("Uniform Sparse", cfg.nr_graphs, [&](int i) {
        GraphGenerator gen(cfg.base_seed + static_cast<uint32_t>(i));
        return gen.generate_sparse_uniform_csr(cfg.n, cfg.c).to_adjacency_list();
    });

    std::cout << "\nBenchmarking Scale-Free (n=" << cfg.n << ", m0=10, m=5)...\n";
    bench.run_suite("Scale-Free", cfg.nr_graphs, [&](int i) {
        GraphGenerator gen(cfg.base_seed + static_cast<uint32_t>(i));
        return gen.generate_scale_free(cfg.n, 10, 5);
    });

    bench.print_results();
}

void run_csr_benchmarks(const LegacyConfig& cfg) {
    Benchmarker<GraphCSR> benchCSR(cfg.nr_runs, cfg.verify);

    benchCSR.add_algorithm("Sequential CSR", [](const GraphCSR& g) {
        return MISSolver<GraphCSR>(g).find(Algorithm::Sequential);
    });

    benchCSR.add_algorithm("Luby 1t CSR", [](const GraphCSR& g) {
        return MISSolver<GraphCSR>(g).find(Algorithm::Luby, 1);
    });

    benchCSR.add_algorithm("Luby 2t CSR", [](const GraphCSR& g) {
        return MISSolver<GraphCSR>(g).find(Algorithm::Luby, 2);
    });

    benchCSR.add_algorithm("Luby 4t CSR", [](const GraphCSR& g) {
        return MISSolver<GraphCSR>(g).find(Algorithm::Luby, 4);
    });

    benchCSR.add_algorithm("Luby 8t CSR", [](const GraphCSR& g) {
        return MISSolver<GraphCSR>(g).find(Algorithm::Luby, 8);
    });

    benchCSR.add_algorithm("Luby CSR", [](const GraphCSR& g) {
        return MISSolver<GraphCSR>(g).find(Algorithm::Luby);
    });

    benchCSR.add_algorithm("Luby Improved CSR", [](const GraphCSR& g) {
        return MISSolver<GraphCSR>(g).find(Algorithm::LubyImproved);
    });

    benchCSR.add_algorithm("Luby GPU CSR", [](const GraphCSR& g) {
        return MISSolver<GraphCSR>(g).find(Algorithm::LubyGPU);
    });

    std::cout << "\nBenchmarking Scale-Free (csr) (n=" << cfg.n << ", m0=10, m=5)...\n";
    benchCSR.run_suite("Scale-Free_csr", cfg.nr_graphs, [&](int i) {
        GraphGenerator gen(cfg.base_seed + static_cast<uint32_t>(i));
        return gen.generate_scale_free_csr(cfg.n, 10, 5);
    });

    std::cout << "\nBenchmarking Uniform Sparse (csr) (n=" << cfg.n << ", c=" << cfg.c << ")...\n";
    benchCSR.run_suite("Uniform Sparse_csr", cfg.nr_graphs, [&](int i) {
        GraphGenerator gen(cfg.base_seed + static_cast<uint32_t>(i));
        return gen.generate_sparse_uniform_csr(cfg.n, cfg.c);
    });

    benchCSR.print_results();
}

void run_weighted_benchmarks(const LegacyConfig& cfg) {
    WeightedBenchmarker<WeightedGraph> benchWeighted(cfg.nr_runs, cfg.verify);

    benchWeighted.add_algorithm("WGreedy Seq", [](const WeightedGraph& g) {
        return weighted_greedy_sequential_mis(g);
    });

    benchWeighted.add_algorithm("WSampling Seq", [](const WeightedGraph& g) {
        return weighted_sampling_sequential_mis(g);
    });

    benchWeighted.add_algorithm("WGreedy 1t", [](const WeightedGraph& g) {
        omp_set_num_threads(1);
        return weighted_greedy_mis(g);
    });
    benchWeighted.add_algorithm("WGreedy 2t", [](const WeightedGraph& g) {
        omp_set_num_threads(2);
        return weighted_greedy_mis(g);
    });
    benchWeighted.add_algorithm("WGreedy 3t", [](const WeightedGraph& g) {
        omp_set_num_threads(3);
        return weighted_greedy_mis(g);
    });
    benchWeighted.add_algorithm("WGreedy 4t", [](const WeightedGraph& g) {
        omp_set_num_threads(4);
        return weighted_greedy_mis(g);
    });
    // benchWeighted.add_algorithm("WGreedy 8t", [](const WeightedGraph& g) {
    //     omp_set_num_threads(8);
    //     return weighted_greedy_mis(g);
    // });
    // benchWeighted.add_algorithm("WGreedy 16t", [](const WeightedGraph& g) {
    //     omp_set_num_threads(16);
    //     return weighted_greedy_mis(g);
    // });
    
    benchWeighted.add_algorithm("WSampling 1t", [](const WeightedGraph& g) {
        omp_set_num_threads(1);
        return weighted_sampling_mis(g);
    });
    benchWeighted.add_algorithm("WSampling 2t", [](const WeightedGraph& g) {
        omp_set_num_threads(2);
        return weighted_sampling_mis(g);
    });
    benchWeighted.add_algorithm("WSampling 3t", [](const WeightedGraph& g) {
        omp_set_num_threads(3);
        return weighted_sampling_mis(g);
    });
    benchWeighted.add_algorithm("WSampling 4t", [](const WeightedGraph& g) {
        omp_set_num_threads(4);
        return weighted_sampling_mis(g);
    });
    // benchWeighted.add_algorithm("WSampling 8t", [](const WeightedGraph& g) {
    //     omp_set_num_threads(8);
    //     return weighted_sampling_mis(g);
    // });
    // benchWeighted.add_algorithm("WSampling 16t", [](const WeightedGraph& g) {
    //     omp_set_num_threads(16);
    //     return weighted_sampling_mis(g);
    // });

    benchWeighted.print_results();
    benchWeighted.write_csv("results/weighted_thread_scaling_and_quality.csv");
}

int main(int argc, char* argv[]) {
    ConfigRunOptions config_opts;
    LegacyConfig legacy;
    bool any_bench_selected = false;
    bool use_config = false;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--config") == 0) {
            if (i + 1 < argc) {
                config_opts.config_path = argv[++i];
                use_config = true;
            }
        } else if (std::strcmp(argv[i], "--output") == 0) {
            if (i + 1 < argc) config_opts.output_override = argv[++i];
        } else if (std::strcmp(argv[i], "--warm-cache") == 0) {
            config_opts.warm_cache = true;
        } else if (std::strcmp(argv[i], "--dry-run") == 0) {
            config_opts.dry_run = true;
        } else if (std::strcmp(argv[i], "-g") == 0 || std::strcmp(argv[i], "--graphs") == 0) {
            if (i + 1 < argc) legacy.nr_graphs = std::stoi(argv[++i]);
        } else if (std::strcmp(argv[i], "-r") == 0 || std::strcmp(argv[i], "--runs") == 0) {
            if (i + 1 < argc) legacy.nr_runs = std::stoi(argv[++i]);
        } else if (std::strcmp(argv[i], "-n") == 0) {
            if (i + 1 < argc) legacy.n = std::stoull(argv[++i]);
        } else if (std::strcmp(argv[i], "-c") == 0) {
            if (i + 1 < argc) legacy.c = std::stod(argv[++i]);
        } else if (std::strcmp(argv[i], "-v") == 0 || std::strcmp(argv[i], "--verify") == 0) {
            if (i + 1 < argc) legacy.verify = (std::string(argv[++i]) == "true" || argv[i][0] == '1');
        } else if (std::strcmp(argv[i], "--normal") == 0) {
            legacy.run_normal = true;
            any_bench_selected = true;
        } else if (std::strcmp(argv[i], "--csr") == 0) {
            legacy.run_csr = true;
            any_bench_selected = true;
        } else if (std::strcmp(argv[i], "--weighted") == 0) {
            legacy.run_weighted = true;
            any_bench_selected = true;
        } else if (std::strcmp(argv[i], "--all") == 0) {
            legacy.run_normal = true;
            legacy.run_csr = true;
            legacy.run_weighted = true;
            any_bench_selected = true;
        } else if (std::strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else {
            std::cerr << "Unknown option: " << argv[i] << "\n";
            print_usage(argv[0]);
            return 1;
        }
    }

    if (use_config) {
        if (config_opts.config_path.empty()) {
            std::cerr << "--config requires a file path\n";
            return 1;
        }
        return run_from_config(config_opts);
    }

    if (!any_bench_selected) {
        legacy.run_csr = true;
    }

    std::cout << "Configuration:\n"
              << "  Graphs:  " << legacy.nr_graphs << "\n"
              << "  Runs:    " << legacy.nr_runs << "\n"
              << "  Nodes:   " << legacy.n << "\n"
              << "  C:       " << legacy.c << "\n"
              << "  Verify:  " << (legacy.verify ? "yes" : "no") << "\n\n";

    if (legacy.run_normal) {
        run_normal_benchmarks(legacy);
    }

    if (legacy.run_csr) {
        run_csr_benchmarks(legacy);
    }

    if (legacy.run_weighted) {
        run_weighted_benchmarks(legacy);
    }

    return 0;
}
