// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <sstream>
#include <iomanip>

struct OptionDef {
    std::string short_name;
    std::string long_name;
    std::string desc;
    std::string hint;
    bool has_arg{false};
};

class Matches {
public:
    bool opt_present(const std::string& name) const {
        return values_.find(name) != values_.end();
    }

    std::optional<std::string> opt_str(const std::string& name) const {
        auto it = values_.find(name);
        if (it != values_.end() && it->second.has_value()) {
            return it->second;
        }
        return std::nullopt;
    }

    void set(const std::string& short_name, const std::string& long_name, std::optional<std::string> val) {
        if (!short_name.empty()) {
            values_[short_name] = val;
        }
        if (!long_name.empty()) {
            values_[long_name] = val;
        }
    }

private:
    std::unordered_map<std::string, std::optional<std::string>> values_;
};

class Options {
public:
    void optopt(std::string short_name, std::string long_name, std::string desc, std::string hint) {
        defs_.push_back({std::move(short_name), std::move(long_name), std::move(desc), std::move(hint), true});
    }

    void optflag(std::string short_name, std::string long_name, std::string desc) {
        defs_.push_back({std::move(short_name), std::move(long_name), std::move(desc), "", false});
    }

    bool parse(const std::vector<std::string>& args, Matches& matches, std::string& error_msg) const {
        for (size_t i = 0; i < args.size(); ++i) {
            const std::string& arg = args[i];
            if (arg.rfind("--", 0) == 0) {
                // Long option: --opt or --opt=val
                std::string opt_part = arg.substr(2);
                std::string val_part;
                bool has_equal = false;
                auto eq_pos = opt_part.find('=');
                if (eq_pos != std::string::npos) {
                    val_part = opt_part.substr(eq_pos + 1);
                    opt_part = opt_part.substr(0, eq_pos);
                    has_equal = true;
                }

                const OptionDef* found = nullptr;
                for (const auto& def : defs_) {
                    if (def.long_name == opt_part) {
                        found = &def;
                        break;
                    }
                }

                if (!found) {
                    error_msg = "Unrecognized option: '" + opt_part + "'";
                    return false;
                }

                if (found->has_arg) {
                    if (has_equal) {
                        matches.set(found->short_name, found->long_name, val_part);
                    } else if (i + 1 < args.size()) {
                        matches.set(found->short_name, found->long_name, args[++i]);
                    } else {
                        error_msg = "Argument to option '--" + opt_part + "' missing";
                        return false;
                    }
                } else {
                    if (has_equal) {
                        error_msg = "Option '--" + opt_part + "' does not take an argument";
                        return false;
                    }
                    matches.set(found->short_name, found->long_name, std::nullopt);
                }
            } else if (arg.rfind("-", 0) == 0 && arg.size() > 1) {
                // Short option: -o, -oVAL, or -o VAL
                std::string opt_part = arg.substr(1, 1);
                std::string rest = arg.substr(2);

                const OptionDef* found = nullptr;
                for (const auto& def : defs_) {
                    if (def.short_name == opt_part || (def.short_name == "v" && opt_part == "V")) {
                        found = &def;
                        break;
                    }
                }

                if (!found) {
                    error_msg = "Unrecognized option: '" + opt_part + "'";
                    return false;
                }

                if (found->has_arg) {
                    if (!rest.empty()) {
                        matches.set(found->short_name, found->long_name, rest);
                    } else if (i + 1 < args.size()) {
                        matches.set(found->short_name, found->long_name, args[++i]);
                    } else {
                        error_msg = "Argument to option '-" + opt_part + "' missing";
                        return false;
                    }
                } else {
                    if (!rest.empty()) {
                        error_msg = "Unrecognized option: '" + rest + "'";
                        return false;
                    }
                    matches.set(found->short_name, found->long_name, std::nullopt);
                }
            } else {
                error_msg = "Unexpected argument: '" + arg + "'";
                return false;
            }
        }
        return true;
    }

    std::string usage(const std::string& brief) const {
        std::ostringstream oss;
        oss << brief << "\n\nOptions:\n";
        for (const auto& def : defs_) {
            std::string opt_str = "    ";
            if (!def.short_name.empty()) {
                opt_str += "-" + def.short_name;
            }
            if (!def.long_name.empty()) {
                if (!def.short_name.empty()) opt_str += ", ";
                opt_str += "--" + def.long_name;
            }
            if (def.has_arg && !def.hint.empty()) {
                opt_str += " " + def.hint;
            }

            if (opt_str.size() < 24) {
                oss << opt_str << std::string(24 - opt_str.size(), ' ') << def.desc << "\n";
            } else {
                oss << opt_str << "\n" << std::string(24, ' ') << def.desc << "\n";
            }
        }
        return oss.str();
    }

private:
    std::vector<OptionDef> defs_;
};
