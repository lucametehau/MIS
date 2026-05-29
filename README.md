# MIS

This project aims to showcase different implementations, mainly parallel, of the Maximal Independent Set (MIS).

In `mis.cpp`, under the `enum class Algorithm` we have the following algorithms:

- ``Algorithm::Sequential``, the base implementation of MIS, sequential, non-parallel. $O(N+M)$ complexity.
- ``Algorithm::Luby``, the base implementation of Luby's algorithm, parallel. $O((N+M)/T\cdot log(N))$ complexity, where $T$ is the number of threads.
- ``Algorithm::LubyImproved``, improved implementation of Luby's algorithm. Small optimization, but also tried to use ``#pragma omp parallel for schedule(dynamic, 32)``, dynamic scheduling, to account for unbalanced degrees.

# Timing results

For now, we are using 2 types of graphs to test our algorithms: **Sparse Graphs**, **Scale-Free Graphs**, the latter resembling social network graphs.

## Normal MIS

### Uniform Sparse = ~200 * N edges

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

### Uniform sparse = ~5 * N edges

```
==============================================================================================================
Algorithm           Graph Type                         Mean Time     Graph Std   Avg Run Std
--------------------------------------------------------------------------------------------------------------
Sequential CSR      Scale-Free_csr                     33.857 ms      1.287 ms      0.984 ms
Luby 1t CSR         Scale-Free_csr                    547.079 ms     35.133 ms     17.166 ms
Luby 2t CSR         Scale-Free_csr                    317.887 ms     15.681 ms      8.742 ms
Luby 4t CSR         Scale-Free_csr                    213.859 ms     12.070 ms     15.283 ms
Luby 8t CSR         Scale-Free_csr                    175.613 ms     14.891 ms     12.623 ms
Luby CSR            Scale-Free_csr                    194.601 ms     12.639 ms     33.443 ms
Luby Improved CSR   Scale-Free_csr                    151.991 ms     26.106 ms     40.225 ms
Luby GPU CSR        Scale-Free_csr                     79.293 ms      8.603 ms     12.775 ms
Sequential CSR      Uniform Sparse_csr                 29.422 ms      0.247 ms      0.884 ms
Luby 1t CSR         Uniform Sparse_csr                471.474 ms     14.050 ms     28.512 ms
Luby 2t CSR         Uniform Sparse_csr                262.123 ms      9.464 ms     20.256 ms
Luby 4t CSR         Uniform Sparse_csr                169.464 ms      8.720 ms      7.033 ms
Luby 8t CSR         Uniform Sparse_csr                134.001 ms      1.259 ms      3.694 ms
Luby CSR            Uniform Sparse_csr                161.269 ms     17.970 ms     30.512 ms
Luby Improved CSR   Uniform Sparse_csr                151.849 ms     58.939 ms     26.662 ms
Luby GPU CSR        Uniform Sparse_csr                 51.800 ms      0.883 ms      1.115 ms
==============================================================================================================
```

## Weighted

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


## Extras on School Machines

- $N=1000000,M=5*N$

```
==============================================================================================================
Algorithm           Graph Type                         Mean Time     Graph Std   Avg Run Std
--------------------------------------------------------------------------------------------------------------
Sequential CSR      Scale-Free_csr                      5.725 ms      1.627 ms      2.373 ms
Luby 1t CSR         Scale-Free_csr                     51.349 ms      0.474 ms      0.336 ms
Luby 2t CSR         Scale-Free_csr                     30.991 ms      0.948 ms      0.455 ms
Luby 4t CSR         Scale-Free_csr                     18.644 ms      0.686 ms      0.334 ms
Luby 8t CSR         Scale-Free_csr                     12.568 ms      0.050 ms      0.319 ms
Luby CSR            Scale-Free_csr                      9.687 ms      1.042 ms      1.679 ms
Luby Improved CSR   Scale-Free_csr                      6.592 ms      0.136 ms      0.210 ms
Luby GPU CSR        Scale-Free_csr                     12.212 ms      8.812 ms     12.631 ms
Sequential CSR      Uniform Sparse_csr                  5.251 ms      0.251 ms      0.275 ms
Luby 1t CSR         Uniform Sparse_csr                 44.526 ms      2.196 ms      1.162 ms
Luby 2t CSR         Uniform Sparse_csr                 24.719 ms      0.167 ms      0.472 ms
Luby 4t CSR         Uniform Sparse_csr                 14.841 ms      0.255 ms      0.361 ms
Luby 8t CSR         Uniform Sparse_csr                  9.772 ms      0.194 ms      0.443 ms
Luby CSR            Uniform Sparse_csr                 16.394 ms     13.720 ms     22.882 ms
Luby Improved CSR   Uniform Sparse_csr                 35.419 ms     58.008 ms     36.767 ms
Luby GPU CSR        Uniform Sparse_csr                  6.010 ms      0.017 ms      0.107 ms
==============================================================================================================
```

- $N=1000000,m=15*N$

```
==============================================================================================================
Algorithm           Graph Type                         Mean Time     Graph Std   Avg Run Std
--------------------------------------------------------------------------------------------------------------
Sequential CSR      Scale-Free_csr                      5.643 ms      1.664 ms      2.504 ms
Luby 1t CSR         Scale-Free_csr                     47.506 ms      0.517 ms      0.488 ms
Luby 2t CSR         Scale-Free_csr                     30.098 ms      0.455 ms      0.766 ms
Luby 4t CSR         Scale-Free_csr                     17.730 ms      0.104 ms      0.507 ms
Luby 8t CSR         Scale-Free_csr                     11.845 ms      0.244 ms      0.387 ms
Luby CSR            Scale-Free_csr                      8.994 ms      0.177 ms      0.295 ms
Luby Improved CSR   Scale-Free_csr                      6.735 ms      0.174 ms      0.218 ms
Luby GPU CSR        Scale-Free_csr                     11.794 ms      8.968 ms     12.960 ms
Sequential CSR      Uniform Sparse_csr                  4.695 ms      0.231 ms      0.332 ms
Luby 1t CSR         Uniform Sparse_csr                 62.057 ms      4.299 ms      1.152 ms
Luby 2t CSR         Uniform Sparse_csr                 35.138 ms      0.413 ms      0.693 ms
Luby 4t CSR         Uniform Sparse_csr                 22.029 ms      0.472 ms      0.725 ms
Luby 8t CSR         Uniform Sparse_csr                 15.228 ms      0.845 ms      1.341 ms
Luby CSR            Uniform Sparse_csr                 21.825 ms     20.313 ms     14.626 ms
Luby Improved CSR   Uniform Sparse_csr                 13.391 ms      8.579 ms     20.050 ms
Luby GPU CSR        Uniform Sparse_csr                  9.884 ms      0.137 ms      0.145 ms
==============================================================================================================
```

- $N=1000000,M=30*N$

```
==============================================================================================================
Algorithm           Graph Type                         Mean Time     Graph Std   Avg Run Std
--------------------------------------------------------------------------------------------------------------
Sequential CSR      Scale-Free_csr                      5.595 ms      1.650 ms      2.521 ms
Luby 1t CSR         Scale-Free_csr                     47.507 ms      0.850 ms      0.453 ms
Luby 2t CSR         Scale-Free_csr                     30.695 ms      0.753 ms      0.633 ms
Luby 4t CSR         Scale-Free_csr                     19.150 ms      0.283 ms      0.323 ms
Luby 8t CSR         Scale-Free_csr                     12.357 ms      0.426 ms      0.321 ms
Luby CSR            Scale-Free_csr                      9.788 ms      1.961 ms      0.281 ms
Luby Improved CSR   Scale-Free_csr                      6.596 ms      0.079 ms      0.178 ms
Luby GPU CSR        Scale-Free_csr                     11.868 ms      9.029 ms     12.949 ms
Sequential CSR      Uniform Sparse_csr                  5.072 ms      0.139 ms      0.422 ms
Luby 1t CSR         Uniform Sparse_csr                 77.981 ms      7.468 ms      3.689 ms
Luby 2t CSR         Uniform Sparse_csr                 44.872 ms      1.343 ms      1.515 ms
Luby 4t CSR         Uniform Sparse_csr                 26.583 ms      0.440 ms      0.555 ms
Luby 8t CSR         Uniform Sparse_csr                 18.335 ms      0.694 ms      1.760 ms
Luby CSR            Uniform Sparse_csr                 27.617 ms     27.618 ms     20.932 ms
Luby Improved CSR   Uniform Sparse_csr                  9.582 ms      0.041 ms      0.260 ms
Luby GPU CSR        Uniform Sparse_csr                 17.047 ms      0.371 ms      1.162 ms
==============================================================================================================
```

- $N=10000000,M=3*N$

```
==============================================================================================================
Algorithm           Graph Type                         Mean Time     Graph Std   Avg Run Std
--------------------------------------------------------------------------------------------------------------
Sequential CSR      Scale-Free_csr                    116.504 ms      3.918 ms    177.688 ms
Luby 1t CSR         Scale-Free_csr                    862.190 ms     15.628 ms    179.666 ms
Luby 2t CSR         Scale-Free_csr                    553.491 ms     15.671 ms    179.431 ms
Luby 4t CSR         Scale-Free_csr                    357.638 ms      8.932 ms    175.634 ms
Luby 8t CSR         Scale-Free_csr                    264.374 ms      9.028 ms    179.470 ms
Luby CSR            Scale-Free_csr                    232.698 ms      9.269 ms    173.981 ms
Luby Improved CSR   Scale-Free_csr                    253.945 ms     72.065 ms    181.807 ms
Luby GPU CSR        Scale-Free_csr                    151.258 ms     10.245 ms    204.742 ms
Sequential CSR      Uniform Sparse_csr                 70.356 ms      3.895 ms      2.386 ms
Luby 1t CSR         Uniform Sparse_csr                562.327 ms     10.282 ms     19.924 ms
Luby 2t CSR         Uniform Sparse_csr                319.044 ms      3.651 ms      2.734 ms
Luby 4t CSR         Uniform Sparse_csr                200.775 ms      8.152 ms      7.240 ms
Luby 8t CSR         Uniform Sparse_csr                135.146 ms      8.631 ms      7.208 ms
Luby CSR            Uniform Sparse_csr                111.902 ms      4.236 ms     11.591 ms
Luby Improved CSR   Uniform Sparse_csr                 89.262 ms     10.994 ms     16.723 ms
Luby GPU CSR        Uniform Sparse_csr                 54.634 ms      0.599 ms      1.253 ms
============================================================================================================================================================================================================================
Algorithm           Graph Type                         Mean Time     Graph Std   Avg Run Std
--------------------------------------------------------------------------------------------------------------
Sequential CSR      Scale-Free_csr                    116.504 ms      3.918 ms    177.688 ms
Luby 1t CSR         Scale-Free_csr                    862.190 ms     15.628 ms    179.666 ms
Luby 2t CSR         Scale-Free_csr                    553.491 ms     15.671 ms    179.431 ms
Luby 4t CSR         Scale-Free_csr                    357.638 ms      8.932 ms    175.634 ms
Luby 8t CSR         Scale-Free_csr                    264.374 ms      9.028 ms    179.470 ms
Luby CSR            Scale-Free_csr                    232.698 ms      9.269 ms    173.981 ms
Luby Improved CSR   Scale-Free_csr                    253.945 ms     72.065 ms    181.807 ms
Luby GPU CSR        Scale-Free_csr                    151.258 ms     10.245 ms    204.742 ms
Sequential CSR      Uniform Sparse_csr                 70.356 ms      3.895 ms      2.386 ms
Luby 1t CSR         Uniform Sparse_csr                562.327 ms     10.282 ms     19.924 ms
Luby 2t CSR         Uniform Sparse_csr                319.044 ms      3.651 ms      2.734 ms
Luby 4t CSR         Uniform Sparse_csr                200.775 ms      8.152 ms      7.240 ms
Luby 8t CSR         Uniform Sparse_csr                135.146 ms      8.631 ms      7.208 ms
Luby CSR            Uniform Sparse_csr                111.902 ms      4.236 ms     11.591 ms
Luby Improved CSR   Uniform Sparse_csr                 89.262 ms     10.994 ms     16.723 ms
Luby GPU CSR        Uniform Sparse_csr                 54.634 ms      0.599 ms      1.253 ms
==============================================================================================================
```