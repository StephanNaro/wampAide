// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include <cstdint>
#include "cli_options.hpp"
#include "options.hpp"

struct ConfigOptions {
    std::string log_path;
    uint16_t minimum_logs{0};
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> sections;
};

std::optional<ConfigOptions> load_config_options(
    const ParsedOptions& commandline_options,
    const std::string& program_name,
    const Options& allowed_opts
);
