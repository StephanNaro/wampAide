// SPDX-License-Identifier: GPL-3.0-or-later
#include "utils.hpp"
#include <iostream>

void print_version() {
    std::cout << "wampAide " << WAMPAIDE_VERSION << "\n";
}

void print_usage(const std::string& program_name, const Options& allowed_opts) {
    std::string brief = "Usage: " + program_name + " -cFILE [options]";
    std::cout << allowed_opts.usage(brief);
}
