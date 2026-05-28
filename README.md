# MIS

This project aims to showcase different implementations, mainly parallel, of the Maximal Independent Set (MIS).

In `mis.cpp`, under the `enum class Algorithm` we have the following algorithms:

- ``Algorithm::Sequential``, the base implementation of MIS, sequential, non-parallel. $O(N+M)$ complexity.
- ``Algorithm::Luby``, the base implementation of Luby's algorithm, parallel. $O((N+M)/T\cdot log(N))$ complexity, where $T$ is the number of threads.
- ``Algorithm::LubyImproved``, improved implementation of Luby's algorithm. Small optimization, but also tried to use ``#pragma omp parallel for schedule(dynamic, 32)``, dynamic scheduling, to account for unbalanced degrees.

# Timing results

For now, we are using 2 types of graphs to test our algorithms: **Sparse Graphs**, **Scale-Free Graphs**, the latter resembling social network graphs.

```
Running benchmarks (nr_graphs=5, nr_runs=10)...

Benchmarking Uniform Sparse (csr) (n=1000000, p=0.0004)...

Benchmarking Scale-Free (csr) (n=1000000, m0=10, m=5)...

Benchmarking Uniform Sparse (n=1000000, p=0.0004)...

Benchmarking Scale-Free (n=1000000, m0=10, m=5)...

==============================================================================================================
Algorithm           Graph Type                         Mean Time     Graph Std   Avg Run Std
--------------------------------------------------------------------------------------------------------------
Sequential          Uniform Sparse                      9.817 ms      0.260 ms      0.294 ms
Luby 1t             Uniform Sparse                    226.291 ms      2.124 ms      3.319 ms
Luby                Uniform Sparse                     47.262 ms     14.699 ms     16.596 ms
Luby Improved       Uniform Sparse                     28.112 ms      1.828 ms      2.443 ms
Sequential          Scale-Free                         16.445 ms      0.649 ms      0.437 ms
Luby 1t             Scale-Free                        130.172 ms      2.813 ms      6.150 ms
Luby                Scale-Free                         76.912 ms     48.009 ms     47.493 ms
Luby Improved       Scale-Free                         22.305 ms      4.322 ms      4.254 ms
==============================================================================================================

==============================================================================================================
Algorithm           Graph Type                         Mean Time     Graph Std   Avg Run Std
--------------------------------------------------------------------------------------------------------------
Sequential CSR      Uniform Sparse_csr                  8.699 ms      0.236 ms      0.378 ms
Luby 1t CSR         Uniform Sparse_csr                223.402 ms      9.145 ms     10.463 ms
Luby CSR            Uniform Sparse_csr                 37.918 ms      1.313 ms      2.574 ms
Luby Improved CSR   Uniform Sparse_csr                 27.369 ms      1.834 ms      4.524 ms
Sequential CSR      Scale-Free_csr                      5.868 ms      0.077 ms      0.159 ms
Luby 1t CSR         Scale-Free_csr                     77.717 ms      1.840 ms      1.300 ms
Luby CSR            Scale-Free_csr                     23.562 ms      1.328 ms      3.895 ms
Luby Improved CSR   Scale-Free_csr                     16.713 ms      3.229 ms      6.176 ms
==============================================================================================================
```