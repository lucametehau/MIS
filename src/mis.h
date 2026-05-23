#pragma once
#include "mis_algos.h"

enum class Algorithm {
    Sequential,
    Luby,
    LubyImproved
};

template<typename GraphT>
class MISSolver {
public:
    MISSolver() = delete;

    explicit MISSolver(const GraphT& g)
        : g_(g) {}

    NodeList find(Algorithm algo) {
        switch (algo) {
            case Algorithm::Sequential:
                return sequential_mis(g_);

            case Algorithm::Luby:
                return luby_mis(g_);

            case Algorithm::LubyImproved:
                return luby_improved_mis(g_);
        }

        return {};
    }

private:
    const GraphT& g_;
};