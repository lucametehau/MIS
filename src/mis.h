#pragma once
#include "types.h"

// algorithms we will use for our MIS solver
enum class Algorithm {
    Sequential, Luby, LubyImproved
};

template<Algorithm Algo>
class MIS {
public:
    MIS() = delete;
    MIS(const Graph &g) : g_(g) {}

    NodeList find();

private:
    Graph g_;
};