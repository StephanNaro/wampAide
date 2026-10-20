// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include "options.hpp"

struct ParsedOptions {
    std::string config_path;
    std::optional<uint16_t> log_count;
};

std::optional<ParsedOptions> parse_commandline_options(
    const std::vector<std::string>& args,
    const std::string& program_name,
    const Options& allowed_opts
);
