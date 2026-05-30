# Benchmark configs

Run benchmarks via `mis.exe --config <file>`.

## Quick start

```powershell
make
.\mis.exe --config configs/smoke_test.json
.\mis.exe --config configs/section0_csr_justify.json --dry-run
.\scripts\run_benchmarks.ps1 -Config configs/smoke_test.json
.\scripts\run_all_sections.ps1
```

Pre-warm graph cache before long runs:

```powershell
.\mis.exe --config configs/section1_scaling_n.json --warm-cache
```

## Config schema

| Field | Description |
|-------|-------------|
| `output` | CSV path (truncated each run) |
| `base_seed` | Seed for graph 0; instance `i` uses `base_seed + i` |
| `verify` | Default MIS correctness check |
| `cache_dir` | Directory for `.grph` / `.adj` cache files |
| `nr_graphs` / `nr_runs` | Defaults; overridable per experiment |

### Experiment

| Field | Description |
|-------|-------------|
| `suite` | Section id (e.g. `"1"`) — written to CSV |
| `label` | Short name for this row group |
| `graph` | See below |
| `cases` | List of algorithm runs |

### Graph

| Field | Description |
|-------|-------------|
| `family` | `uniform_sparse` or `scale_free` |
| `n` | Number of nodes |
| `c` | Uniform sparse: expected ~`c·n` edges |
| `m0`, `m` | Scale-free Barabási–Albert parameters |

### Case

| Field | Description |
|-------|-------------|
| `name` | Label in CSV |
| `algorithm` | `sequential`, `luby`, `luby_improved`, `luby_gpu`, `luby_gpu2` |
| `representation` | `csr` or `adjacency` |
| `threads` | OpenMP threads (required for parallel algos) |

## Shipped configs

| File | Report section |
|------|----------------|
| `section0_csr_justify.json` | §0 CSR vs adjacency |
| `section1_scaling_n.json` | §1 scaling with N |
| `section2_thread_scaling.json` | §2 thread scaling |
| `section3_topology.json` | §3 graph topology |
| `section4_summary.json` | §4 head-to-head (same graphs as §3) |
| `smoke_test.json` | Small sanity check |

## CSV columns

`suite,label,algorithm,graph_family,n,c,m0,m,representation,threads,nr_graphs,nr_runs,mean_time_ms,graph_std_ms,mean_run_std_ms`

Times are **means** (mean per graph, then mean/std across graphs), matching the legacy `Benchmarker`.
