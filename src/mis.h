#pragma once
#include "types.h"

template<Device d>
class MIS {
public:
    MIS() = delete;
    MIS(const Graph &g) : g_(g) {}

    NodeList find();

private:
    Graph g_;
};