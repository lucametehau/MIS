#include "luby_gpu.h"
#include <cuda_runtime.h>
#include <curand_kernel.h>
#include <device_launch_parameters.h>
#include <iostream>
#include <vector>

__device__ inline unsigned int hash_priority(int id, int iteration) {
    unsigned int state = id * 747796405u + 2891336453u + iteration;
    unsigned int word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    unsigned int h = (word >> 22u) ^ word;
    return h;
}

__global__ void init_states_kernel(uint8_t* is_active, uint8_t* in_mis, int n) {
    int id = blockIdx.x * blockDim.x + threadIdx.x;
    if (id < n) {
        is_active[id] = 1;
        in_mis[id] = 0;
    }
}

__global__ void select_assign_priorities_fused_candidates_kernel(
    const uint32_t* offsets, const Node* edges,
    const uint8_t* is_active, uint8_t* candidates, 
    int iteration_num, int n) 
{
    int u = blockIdx.x * blockDim.x + threadIdx.x;
    if (u < n && is_active[u]) {
        bool is_max = true;
        auto prio_u = hash_priority(u, iteration_num);
        
        for (auto i = offsets[u]; i < offsets[u+1]; i++) {
            auto v = edges[i];
            if (is_active[v]) {
                auto prio_v = hash_priority(v, iteration_num);
                if (prio_v > prio_u || (prio_v == prio_u && v > u)) {
                    is_max = false;
                    break;
                }
            }
        }
        candidates[u] = is_max;
    } else if (u < n) {
        candidates[u] = 0;
    }
}

__global__ void update_mis_and_active_kernel(
    const uint32_t* offsets, const Node* edges,
    uint8_t* is_active, uint8_t* in_mis,
    const uint8_t* candidates, int* d_any_active, int n) 
{
    int u = blockIdx.x * blockDim.x + threadIdx.x;
    if (u < n && is_active[u]) {
        if (candidates[u]) {
            in_mis[u] = 1;
            is_active[u] = 0;
        } else {
            bool neighbor_selected = false;
            for (size_t i = offsets[u]; i < offsets[u+1]; i++) {
                Node v = edges[i];
                if (candidates[v]) {
                    neighbor_selected = true;
                    break;
                }
            }
            if (neighbor_selected) {
                is_active[u] = 0;
            } else {
                *d_any_active = 1;
            }
        }
    }
}

NodeList luby_gpu_mis(const GraphCSR& g) {
    int n = static_cast<int>(g.size());
    if (n == 0) return {};

    const auto& host_offsets = g.offsets();
    const auto& host_edges = g.edges();

    uint32_t* d_offsets;
    Node* d_edges;
    uint8_t* d_is_active;
    uint8_t* d_in_mis;
    uint8_t* d_candidates;
    int* d_any_active;

    cudaMalloc(&d_offsets, (n + 1) * sizeof(uint32_t));
    cudaMalloc(&d_edges, host_edges.size() * sizeof(Node));
    cudaMalloc(&d_is_active, n * sizeof(uint8_t));
    cudaMalloc(&d_in_mis, n * sizeof(uint8_t));
    cudaMalloc(&d_candidates, n * sizeof(uint8_t));
    cudaMalloc(&d_any_active, sizeof(int));

    cudaMemcpy(d_offsets, host_offsets.data(), (n + 1) * sizeof(uint32_t), cudaMemcpyHostToDevice);
    cudaMemcpy(d_edges, host_edges.data(), host_edges.size() * sizeof(Node), cudaMemcpyHostToDevice);

    int blockSize = 256;
    int gridSize = (n + blockSize - 1) / blockSize;

    init_states_kernel<<<gridSize, blockSize>>>(d_is_active, d_in_mis, n);
    cudaDeviceSynchronize();

    int h_any_active = 1, iteration_num = 0;
    while (h_any_active) {
        h_any_active = 0;
        cudaMemcpy(d_any_active, &h_any_active, sizeof(int), cudaMemcpyHostToDevice);

        select_assign_priorities_fused_candidates_kernel<<<gridSize, blockSize>>>(d_offsets, d_edges, d_is_active, d_candidates, iteration_num, n);
        update_mis_and_active_kernel<<<gridSize, blockSize>>>(d_offsets, d_edges, d_is_active, d_in_mis, d_candidates, d_any_active, n);
        
        cudaMemcpy(&h_any_active, d_any_active, sizeof(int), cudaMemcpyDeviceToHost);
        iteration_num++;
    }

    std::cout << iteration_num << " iterations for a graph of " << n << " nodes\n";

    std::vector<uint8_t> h_in_mis(n);
    cudaMemcpy(h_in_mis.data(), d_in_mis, n * sizeof(uint8_t), cudaMemcpyDeviceToHost);

    NodeList mis;
    for (int i = 0; i < n; ++i) {
        if (h_in_mis[i]) {
            mis.push_back(static_cast<Node>(i));
        }
    }

    cudaFree(d_offsets);
    cudaFree(d_edges);
    cudaFree(d_is_active);
    cudaFree(d_in_mis);
    cudaFree(d_candidates);
    cudaFree(d_any_active);

    return mis;
}
