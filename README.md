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
Sequential          Uniform Sparse                      8.837 ms      0.368 ms      0.417 ms
Luby 1t             Uniform Sparse                    209.911 ms      0.721 ms      3.407 ms
Luby 2t             Uniform Sparse                    115.407 ms      1.716 ms      2.117 ms
Luby 4t             Uniform Sparse                     66.841 ms      1.366 ms      2.560 ms
Luby 8t             Uniform Sparse                     46.948 ms      1.792 ms      1.675 ms
Luby                Uniform Sparse                     41.771 ms      5.498 ms     11.246 ms
Luby Improved       Uniform Sparse                     27.178 ms      2.910 ms      1.929 ms
Sequential          Scale-Free                         14.516 ms      0.235 ms      0.448 ms
Luby 1t             Scale-Free                        113.840 ms      1.052 ms      2.173 ms
Luby 2t             Scale-Free                         74.672 ms      0.563 ms      1.518 ms
Luby 4t             Scale-Free                         51.824 ms      0.206 ms      0.851 ms
Luby 8t             Scale-Free                         63.341 ms     43.632 ms     23.487 ms
Luby                Scale-Free                         67.346 ms     51.054 ms     36.415 ms
Luby Improved       Scale-Free                         27.630 ms     10.511 ms     10.854 ms
==============================================================================================================

==============================================================================================================
Algorithm           Graph Type                         Mean Time     Graph Std   Avg Run Std
--------------------------------------------------------------------------------------------------------------
Sequential CSR      Uniform Sparse_csr                  9.806 ms      1.121 ms      0.305 ms
Luby 1t CSR         Uniform Sparse_csr                192.208 ms      1.242 ms      3.607 ms
Luby 2t CSR         Uniform Sparse_csr                104.150 ms      0.479 ms      1.746 ms
Luby 4t CSR         Uniform Sparse_csr                 62.719 ms      3.325 ms      1.816 ms
Luby 8t CSR         Uniform Sparse_csr                 42.141 ms      0.312 ms      1.634 ms
Luby CSR            Uniform Sparse_csr                 59.318 ms     45.520 ms     25.582 ms
Luby Improved CSR   Uniform Sparse_csr                 24.641 ms      0.807 ms      2.796 ms
Sequential CSR      Scale-Free_csr                      5.254 ms      0.102 ms      0.159 ms
Luby 1t CSR         Scale-Free_csr                     73.781 ms      1.570 ms      2.005 ms
Luby 2t CSR         Scale-Free_csr                     44.325 ms      0.468 ms      0.674 ms
Luby 4t CSR         Scale-Free_csr                     29.745 ms      0.173 ms      0.566 ms
Luby 8t CSR         Scale-Free_csr                     25.240 ms      4.114 ms      0.460 ms
Luby CSR            Scale-Free_csr                     23.798 ms      4.311 ms      7.638 ms
Luby Improved CSR   Scale-Free_csr                     14.763 ms      1.609 ms      2.322 ms
==============================================================================================================
```