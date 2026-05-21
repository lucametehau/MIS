# MIS

This project aims to showcase different implementations, mainly parallel, of the Maximal Independent Set (MIS).

In `mis.cpp`, under the `enum class Algorithm` we have the following algorithms:

- ``Algorithm::Sequential``, the base implementation of MIS, sequential, non-parallel. $O(N+M)$ complexity.
- ``Algorithm::Luby``, the base implementation of Luby's algorithm, parallel. $O((N+M)/T\cdot log(N))$ complexity, where $T$ is the number of threads.
- ``Algorithm::LubyImproved``, improved implementation of Luby's algorithm. Small optimization, but also tried to use ``#pragma omp parallel for schedule(dynamic, 32)``, dynamic scheduling, to account for unbalanced degrees.

# Timing results

For now, we are using 2 types of graphs to test our algorithms: **Sparse Graphs**, **Scale-Free Graphs**, the latter resembling social network graphs.

```
Generating Uniform Sparse Graph (n=1000000, p=0.0004)...

--- Benchmark: Uniform Sparse ---
[Sequential]
MIS correct! | Size: 26494 | Time: 1785.32ms
[Luby]
MIS correct! | Size: 26546 | Time: 41.466ms
[Luby Improved]
MIS correct! | Size: 26579 | Time: 27.252ms

Generating Scale-Free Graph (n=1000000, m0=10, m=5)...

--- Benchmark: Scale-Free (Social Network style) ---
[Sequential]
MIS correct! | Size: 245161 | Time: 844.82ms
[Luby]
MIS correct! | Size: 337510 | Time: 49.525ms
[Luby Improved]
MIS correct! | Size: 337230 | Time: 17.914ms
```