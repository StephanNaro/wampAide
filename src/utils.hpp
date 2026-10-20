// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <string>
#include "options.hpp"

#ifndef WAMPAIDE_VERSION
#define WAMPAIDE_VERSION "0.2.0"
#endif

void print_version();
void print_usage(const std::string& program_name, const Options& allowed_opts);
