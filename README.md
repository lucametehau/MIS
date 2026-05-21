# MIS

This project aims to showcase different implementations, mainly parallel, of the Maximal Independent Set (MIS).

In `mis.cpp`, under the `enum class Algorithm` we have the following algorithms:

- ``Algorithm::Sequential``, the base implementation of MIS, sequential, non-parallel. $O(N+M)$ complexity.
- ``Algorithm::Luby``, the base implementation of Luby's algorithm, parallel. $O((N+M)/T\cdot log(N))$ complexity, where $T$ is the number of threads.
- ``Algorithm::LubyImproved``, improved implementation of Luby's algorithm. Small optimization, but also tried to use ``#pragma omp parallel for schedule(dynamic, 32)``, dynamic scheduling, to account for unbalanced degrees.

# Timing results

For now, we are using 2 types of graphs to test our algorithms: **Sparse Graphs**, **Scale-Free Graphs**, the latter resembling social network graphs.

```
Generating Uniform Sparse Graph (n=500000, p=0.0004)...

Generating Scale-Free Graph (n=500000, m0=10, m=5)...

=====================================================================================
Algorithm           Graph Type                             Avg Time  MIS Size   Status
-------------------------------------------------------------------------------------
Sequential          Uniform Sparse                       795.333 ms     13253   PASSED
Luby                Uniform Sparse                       160.102 ms     13330   PASSED
Luby Improved       Uniform Sparse                       171.574 ms     13383   PASSED
Sequential          Scale-Free                           332.016 ms    122404   PASSED
Luby                Scale-Free                            69.443 ms    168393   PASSED
Luby Improved       Scale-Free                            72.933 ms    168775   PASSED
=====================================================================================
```

```
Generating Uniform Sparse Graph (n=1000000, p=0.0004)...

Generating Scale-Free Graph (n=1000000, m0=10, m=5)...

=====================================================================================
Algorithm           Graph Type                             Avg Time  MIS Size   Status
-------------------------------------------------------------------------------------
Sequential          Uniform Sparse                      2388.343 ms     26563   PASSED
Luby                Uniform Sparse                       370.076 ms     26676   PASSED
Luby Improved       Uniform Sparse                       338.382 ms     26757   PASSED
Sequential          Scale-Free                           887.931 ms    244909   PASSED
Luby                Scale-Free                           165.470 ms    337121   PASSED
Luby Improved       Scale-Free                           149.279 ms    336883   PASSED
=====================================================================================
```