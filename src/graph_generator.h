#pragma once
#include "types.h"

class GraphGenerator {
public:
    GraphGenerator() = default;

    Graph generate_uniform(std::size_t n);
};