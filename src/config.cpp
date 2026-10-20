// SPDX-License-Identifier: GPL-3.0-or-later
#include "config.hpp"
#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cctype>

namespace {
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

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

bool parse_ini_file(const std::string& filepath, std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& ini) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    std::string current_section;

    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == ';' || trimmed[0] == '#') {
            continue;
        }

        if (trimmed.front() == '[' && trimmed.back() == ']') {
            current_section = trim(trimmed.substr(1, trimmed.size() - 2));
        } else {
            auto eq_pos = trimmed.find('=');
            if (eq_pos == std::string::npos) {
                return false;
            }
            std::string key = trim(trimmed.substr(0, eq_pos));
            std::string val = trim(trimmed.substr(eq_pos + 1));
            ini[current_section][key] = val;
        }
    }

    return true;
}
} // namespace

std::optional<ConfigOptions> load_config_options(
    const ParsedOptions& commandline_options,
    const std::string& program_name,
    const Options& allowed_opts
) {
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> ini;
    if (!parse_ini_file(commandline_options.config_path, ini)) {
        std::cout << "Error: Failed to load or parse config file\n";
        print_usage(program_name, allowed_opts);
        return std::nullopt;
    }

    auto default_it = ini.find("default");
    if (default_it == ini.end()) {
        std::cout << "Error: [default] section missing in config file\n";
        print_usage(program_name, allowed_opts);
        return std::nullopt;
    }

    const auto& default_section = default_it->second;

    auto log_path_it = default_section.find("log_path");
    if (log_path_it == default_section.end()) {
        std::cout << "Error: log_path missing in [default] section\n";
        print_usage(program_name, allowed_opts);
        return std::nullopt;
    }

    const std::string& log_path = log_path_it->second;
    std::error_code ec;
    if (!std::filesystem::is_directory(log_path, ec)) {
        std::cout << "Error: log_path '" << log_path << "' is not a valid directory\n";
        print_usage(program_name, allowed_opts);
        return std::nullopt;
    }

    auto min_logs_it = default_section.find("minimum_logs");
    if (min_logs_it == default_section.end()) {
        std::cout << "Error: minimum_logs missing in [default] section\n";
        print_usage(program_name, allowed_opts);
        return std::nullopt;
    }

    const std::string& min_logs_str = min_logs_it->second;
    uint16_t min_logs = 0;
    if (!parse_u16(min_logs_str, min_logs)) {
        std::cout << "Error: minimum_logs '" << min_logs_str << "' is not a valid number\n";
        print_usage(program_name, allowed_opts);
        return std::nullopt;
    }

    if (commandline_options.log_count.has_value()) {
        uint16_t limitlogs_count = *commandline_options.log_count;
        if (limitlogs_count < min_logs) {
            std::cout << "Error: limitlogs count " << limitlogs_count
                      << " in command line is less than minimum_logs " << min_logs
                      << " in config\n";
            print_usage(program_name, allowed_opts);
            return std::nullopt;
        }
    }

    ConfigOptions result;
    result.log_path = log_path;
    result.minimum_logs = min_logs;
    result.sections = std::move(ini);
    return result;
}
