#include "mis_checker.h"
#include <set>
#include <format>

std::string MISChecker::check_mis(const Graph &g, const NodeList &mis) const {
    const std::set<uint32_t> mis_set(mis.begin(), mis.end());
    const auto n = g.size(); 

    for (std::size_t node = 0; node < n; node++) {
        uint32_t son_in_mis = n;
        for (auto &son : g[node]) {
            if (mis_set.count(son)) {
                son_in_mis = son;
                break;
            }
        }

        if (mis_set.count(node) && son_in_mis < n)
            return std::format("MIS incorrect! {} and {} are adjacent!", node, son_in_mis);

        if (!son_in_mis)
            return std::format("MIS not maximal! {} can be added!", node);
    }

    return "MIS correct!";
}