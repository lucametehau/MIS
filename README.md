# Maximal Independent Set (MIS)

This project explores high-performance parallel implementations of the Maximal Independent Set (MIS) algorithm. It provides a comprehensive benchmarking suite to evaluate performance across different hardware architectures (Multi-core CPUs and GPUs) and various graph topologies.

## Key Features

- **Multiple Implementations**:
    - **Sequential**: A baseline greedy implementation.
    - **Luby's Algorithm**: Parallel randomized algorithm using OpenMP.
    - **Improved Luby**: Optimized CPU implementation with active node tracking and deterministic hashing.
    - **CUDA/GPU**: Massively parallel implementation leveraging NVIDIA GPUs.
- **Optimized Graph Representation**: 
    - Full support for **Compressed Sparse Row (CSR)** format for high-cache locality and memory efficiency.
    - Comparative analysis against standard Adjacency Lists.
- **Benchmarking Framework**: 
    - JSON-based configuration system for reproducible experiments.
    - Automated result aggregation in CSV format.
    - Support for various graph models: Erdős-Rényi (Uniform) and Barabási–Albert (Scale-Free).
- **Visualization & Reporting**:
    - Python-based plotting scripts for performance analysis.
    - Professional LaTeX report template for synthesizing findings.

## Project Structure

- `src/`: C++ and CUDA source code.
- `configs/`: JSON configuration files for various benchmark sections.
- `report/`: LaTeX source for the performance report and generated figures.
- `scripts/`: Helper scripts for running full benchmark suites.
- `results/`: Output CSV files from benchmark runs.
- `plot_benchmarks.py`: Tool for generating visualizations from CSV data.

## Getting Started

### Prerequisites

- **Compiler**: GCC with OpenMP support (for CPU parallelization).
- **GPU Support**: NVIDIA CUDA Toolkit (nvcc) for the GPU implementation.
- **Python**: 3.x with `pandas` and `matplotlib` for plotting.
- **Build Tool**: Make.

### Building the Project

To build the main benchmark executable:

```bash
make
```

To build without CUDA support (if no GPU or `nvcc` is available):

```bash
make NO_CUDA=1
```

### Running Benchmarks

The project supports two benchmarking modes: **Config-driven** (recommended for reproducibility) and **Legacy CLI flags**.

#### 1. Config-driven Benchmarks (JSON)
Run specific experiments defined in the `configs/` directory:

```bash
./mis --config configs/section1_scaling_n.json
```

#### 2. Full Suite
Run all sections using the provided bash script. This will automatically build the project and run all configurations:

```bash
./scripts/run_all_sections.sh
```

#### 3. Legacy CLI Flags
Run simple benchmarks directly from the command line:

```bash
# Run CSR benchmarks with custom parameters
./mis -n 1000000 -c 10 --csr

# Run all legacy benchmarks (Normal, CSR, and Weighted)
./mis --all
```

Use `./mis --help` to see all available options.

#### 4. Weighted MIS Benchmarks
Run the weighted MIS benchmarks (thread scaling, solution quality, and density variation):

```bash
mkdir -p results
./mis --weighted -n 1000000
```

This produces two CSV files in `results/`:
- `weighted_thread_scaling_and_quality.csv` — thread scaling (1–4 threads) and solution quality (mean MIS weight) across all weight distributions and graph types
- `weighted_density.csv` — effect of graph density (c) on runtime and parallel speedup

### Generating the Report

1.  **Run Benchmarks**: Execute the benchmarks to populate the CSV files in `results/`.
2.  **Generate Figures**: Run the plotting script to refresh the PDF/PNG figures in `report/figures/`.
    ```bash
    python3 plot_benchmarks.py
    ```
3.  **Compile LaTeX**: Use your preferred LaTeX engine (e.g., `pdflatex`) to build `report/report.tex`.

## Acknowledgments

This project utilizes the foundational work of Michael Luby on parallel randomized algorithms (1986).
