#pragma once

#include <string>

struct ConfigRunOptions {
    std::string config_path;
    std::string output_override;
    bool warm_cache = false;
    bool dry_run = false;
};

int run_from_config(const ConfigRunOptions& options);
