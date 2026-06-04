#include "luby_gpu.h"

#include <stdexcept>

namespace {

[[noreturn]] void no_cuda() {
    throw std::runtime_error("GPU support not built; compile without NO_CUDA=1");
}

} // namespace

NodeList luby_gpu_mis(const GraphCSR&) {
    no_cuda();
}

// NodeList luby_gpu_mis2(const GraphCSR&) {
//     no_cuda();
// }
