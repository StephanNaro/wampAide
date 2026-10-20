// SPDX-License-Identifier: GPL-3.0-or-later
#include "cli_options.hpp"
#include "utils.hpp"
#include <iostream>
#include <cctype>

namespace {
bool parse_u16(const std::string& str, uint16_t& val) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    }
    try {
        unsigned long long n = std::stoull(str);
        if (n > 65535) return false;
        val = static_cast<uint16_t>(n);
        return true;
    } catch (...) {
        return false;
    }
}
} // namespace

std::optional<ParsedOptions> parse_commandline_options(
    const std::vector<std::string>& args,
    const std::string& program_name,
    const Options& allowed_opts
) {
    Matches matches;
    std::string error_msg;
    if (!allowed_opts.parse(args, matches, error_msg)) {
        std::cout << "Error: " << error_msg << "\n";
        print_usage(program_name, allowed_opts);
        return std::nullopt;
    }

    if (matches.opt_present("v")) {
        print_version();
        return std::nullopt;
    }

    if (matches.opt_present("h")) {
        print_version();
        print_usage(program_name, allowed_opts);
        return std::nullopt;
    }

    if (!matches.opt_present("c")) {
        std::cout << "Error: Configuration file required\n";
        print_usage(program_name, allowed_opts);
        return std::nullopt;
    }

    std::string config_path = matches.opt_str("c").value_or("");
    if (matches.opt_present("l")) {
        std::string count_str = matches.opt_str("l").value_or("");
        uint16_t limitlogs_count = 0;
        if (parse_u16(count_str, limitlogs_count)) {
            return ParsedOptions{std::move(config_path), limitlogs_count};
        } else {
            std::cout << "Error: Invalid limitlogs count '" << count_str << "'\n";
            print_usage(program_name, allowed_opts);
            return std::nullopt;
        }
    }

    return ParsedOptions{std::move(config_path), std::nullopt};
}
