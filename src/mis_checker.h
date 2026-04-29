#pragma once
#include "types.h"
#include <string>

class MISChecker {
public:
    MISChecker() = default;

    std::string check_mis(const Graph &g, const NodeList &mis) const;
};