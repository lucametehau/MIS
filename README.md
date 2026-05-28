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
Sequential          Uniform Sparse                     10.173 ms      0.990 ms      0.489 ms
Luby 1t             Uniform Sparse                    220.110 ms      7.866 ms     11.611 ms
Luby 2t             Uniform Sparse                    118.410 ms      5.752 ms      4.449 ms
Luby 4t             Uniform Sparse                     68.886 ms      4.813 ms      3.017 ms
Luby 8t             Uniform Sparse                     46.660 ms      1.011 ms      1.579 ms
Luby                Uniform Sparse                     39.868 ms      3.342 ms      5.303 ms
Luby Improved       Uniform Sparse                     32.953 ms     13.767 ms      8.830 ms
Sequential          Scale-Free                         16.447 ms      0.578 ms      0.226 ms
Luby 1t             Scale-Free                        121.499 ms      3.419 ms      4.140 ms
Luby 2t             Scale-Free                         81.631 ms      3.765 ms      2.532 ms
Luby 4t             Scale-Free                         55.314 ms      0.644 ms      1.028 ms
Luby 8t             Scale-Free                         40.632 ms      0.101 ms      0.863 ms
Luby                Scale-Free                         77.833 ms     44.535 ms     43.724 ms
Luby Improved       Scale-Free                         34.660 ms     26.432 ms     10.725 ms
==============================================================================================================

==============================================================================================================
Algorithm           Graph Type                         Mean Time     Graph Std   Avg Run Std
--------------------------------------------------------------------------------------------------------------
Sequential CSR      Uniform Sparse_csr                  9.011 ms      1.547 ms      0.431 ms
Luby 1t CSR         Uniform Sparse_csr                205.244 ms      4.769 ms      6.474 ms
Luby 2t CSR         Uniform Sparse_csr                125.107 ms      8.123 ms      3.680 ms
Luby 4t CSR         Uniform Sparse_csr                 66.962 ms      0.473 ms      1.309 ms
Luby 8t CSR         Uniform Sparse_csr                 44.819 ms      0.217 ms      1.504 ms
Luby CSR            Uniform Sparse_csr                 40.196 ms      1.582 ms      4.324 ms
Luby Improved CSR   Uniform Sparse_csr                 30.915 ms      4.152 ms      6.674 ms
Sequential CSR      Scale-Free_csr                      5.666 ms      0.152 ms      0.152 ms
Luby 1t CSR         Scale-Free_csr                     76.682 ms      3.298 ms      3.255 ms
Luby 2t CSR         Scale-Free_csr                     44.349 ms      0.226 ms      0.708 ms
Luby 4t CSR         Scale-Free_csr                     29.272 ms      0.290 ms      0.407 ms
Luby 8t CSR         Scale-Free_csr                     23.394 ms      0.436 ms      0.494 ms
Luby CSR            Scale-Free_csr                     20.783 ms      0.599 ms      0.914 ms
Luby Improved CSR   Scale-Free_csr                     14.416 ms      0.531 ms      0.780 ms
==============================================================================================================
```

```
Benchmarking Weighted MIS (Uniform weights)...

Benchmarking Weighted MIS (Exponential weights)...

Benchmarking Weighted MIS (Clustered weights)...

==============================================================================================================
Algorithm           Graph Type                         Mean Time     Graph Std   Avg Run Std   Mean Weight
--------------------------------------------------------------------------------------------------------------
Weighted Greedy     Uniform Sparse (uniform)           48.346 ms     10.319 ms     19.023 ms 221205.577
Weighted Sampling   Uniform Sparse (uniform)          101.725 ms     62.736 ms     86.907 ms 169862.792
Weighted Greedy     Scale-Free (uniform)               34.835 ms      5.984 ms      7.885 ms2220850.598
Weighted Sampling   Scale-Free (uniform)               65.297 ms     47.481 ms     41.514 ms1998436.761
Weighted Greedy     Uniform Sparse (exp)               44.856 ms      2.182 ms      7.825 ms 183315.814
Weighted Sampling   Uniform Sparse (exp)               65.743 ms     33.578 ms     24.710 ms 102926.192
Weighted Greedy     Scale-Free (exp)                   53.372 ms     44.933 ms     35.131 ms1338124.128
Weighted Sampling   Scale-Free (exp)                   40.659 ms      5.625 ms     11.284 ms1133123.088
Weighted Greedy     Uniform Sparse (clustered)         63.872 ms      4.375 ms      7.339 ms 163565.000
Weighted Sampling   Uniform Sparse (clustered)         45.382 ms      3.298 ms      5.214 ms 107985.080
Weighted Greedy     Scale-Free (clustered)             41.219 ms     12.018 ms     25.399 ms1051920.600
Weighted Sampling   Scale-Free (clustered)             62.458 ms     25.564 ms     44.257 ms 877655.860
==============================================================================================================
```