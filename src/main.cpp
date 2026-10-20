// SPDX-License-Identifier: GPL-3.0-or-later
#include "options.hpp"
#include "cli_options.hpp"
#include "config.hpp"
#include "utils.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <cctype>

namespace {

bool starts_with_case_insensitive(const std::string& str, const std::string& prefix) {
    if (str.size() < prefix.size()) return false;
    for (size_t i = 0; i < prefix.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(str[i])) !=
            std::tolower(static_cast<unsigned char>(prefix[i]))) {
            return false;
        }
    }
    return true;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

void limit_logs(const std::string& logpath, uint16_t limitlogs_count) {
    using FileEntry = std::pair<std::string, std::filesystem::file_time_type>;
    std::vector<FileEntry> access_files;
    std::vector<FileEntry> error_files;

    std::error_code ec;
    std::filesystem::path dir_path(logpath);
    if (!std::filesystem::exists(dir_path, ec)) {
        std::cout << "Error in limit_logs: logpath does not exist\n";
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(dir_path, ec)) {
        if (ec) {
            std::cout << "Warning: Failed to process directory entry: " << ec.message() << "\n";
            continue;
        }

        std::error_code entry_ec;
        if (entry.is_regular_file(entry_ec)) {
            std::string filename = entry.path().filename().string();
            auto modified = entry.last_write_time(entry_ec);
            if (entry_ec) {
                modified = std::filesystem::file_time_type::min();
            }

            if (starts_with_case_insensitive(filename, "access.log.")) {
                access_files.emplace_back(entry.path().string(), modified);
            } else if (starts_with_case_insensitive(filename, "error.log.")) {
                error_files.emplace_back(entry.path().string(), modified);
            }
        }
    }

    std::vector<std::pair<std::vector<FileEntry>*, std::string>> patterns = {
        {&access_files, "access.log.*"},
        {&error_files, "error.log.*"}
    };

    for (auto& [files_ptr, pattern_name] : patterns) {
        auto& files = *files_ptr;

        // Sort files by modified time (newest first)
        std::sort(files.begin(), files.end(), [](const auto& a, const auto& b) {
            if (a.second != b.second) {
                return a.second > b.second;
            }
            return a.first > b.first;
        });

        // Check if files exceed count
        if (files.size() <= static_cast<size_t>(limitlogs_count)) {
            std::cout << "No " << pattern_name << " files to delete: "
                      << files.size() << " files found, limit is " << limitlogs_count << "\n";
            continue;
        }

        // Print files that would be deleted (oldest files)
        std::cout << pattern_name << " files exceeding limit (" << limitlogs_count << "):\n";
        for (size_t i = limitlogs_count; i < files.size(); ++i) {
            std::cout << "  " << files[i].first << "\n";
        }

        // Prompt user for deletion
        size_t to_delete_count = files.size() - limitlogs_count;
        std::cout << "Delete these " << to_delete_count << " " << pattern_name << " files? (y/n): ";
        std::cout.flush();

        std::string input;
        if (!std::getline(std::cin, input)) {
            return;
        }

        std::string trimmed = trim(input);
        for (char& c : trimmed) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }

        if (trimmed == "y") {
            for (size_t i = limitlogs_count; i < files.size(); ++i) {
                std::error_code rm_ec;
                std::filesystem::remove(files[i].first, rm_ec);
                if (rm_ec) {
                    std::cout << "Warning: Failed to delete file: " << rm_ec.message() << "\n";
                } else {
                    std::cout << "Deleted: " << files[i].first << "\n";
                }
            }
        } else {
            std::cout << "Deletion of " << pattern_name << " files cancelled\n";
        }
    }
}

} // namespace

int main(int argc, char* argv[]) {
    std::string program_name = (argc > 0) ? argv[0] : "wampAide";
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }

    Options allowed_opts;
    allowed_opts.optopt("c", "config", "set configuration file", "FILE");
    allowed_opts.optflag("h", "help", "print this help menu");
    allowed_opts.optopt("l", "limitlogs", "limit logfiles to count", "COUNT");
    allowed_opts.optflag("v", "version", "print version information");

    auto commandline_options = parse_commandline_options(args, program_name, allowed_opts);
    if (!commandline_options.has_value()) {
        return 0;
    }

    auto config_options = load_config_options(*commandline_options, program_name, allowed_opts);
    if (!config_options.has_value()) {
        return 0;
    }

    if (commandline_options->log_count.has_value()) {
        const std::string& log_path = config_options->log_path;
        limit_logs(log_path, *commandline_options->log_count);
    }

    return 0;
}
