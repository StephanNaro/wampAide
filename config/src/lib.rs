// SPDX-License-Identifier: GPL-3.0-or-later
use cli_options::ParsedOptions;
use ini::ini;
use std::collections::HashMap;
use std::path::Path;
use utils::print_usage;
use getopts::Options;

pub fn load_config_options(commandline_options: &ParsedOptions, program_name: &str, allowed_opts: &Options) -> Result<HashMap<String, HashMap<String, Option<String>>>, ()> {
    match ini!(safe commandline_options.config_path.as_str()) {
        Ok(ini) => {
            if let Some(default_section) = ini.get("default") {
                if let Some(Some(log_path)) = default_section.get("log_path") {
                    if !Path::new(log_path).is_dir() {
                        println!("Error: log_path '{}' is not a valid directory", log_path);
                        print_usage(program_name, allowed_opts);
                        return Err(());
                    }
                    if let Some(Some(min_logs_str)) = default_section.get("minimum_logs") {
                        match min_logs_str.parse::<u16>() {
                            Ok(min_logs) => {
                                // Check limitlogs_count against minimum_logs
                                if let Some(limitlogs_count) = commandline_options.log_count {
                                    if limitlogs_count < min_logs {
                                        println!("Error: limitlogs count {} in command line is less than minimum_logs {} in config", limitlogs_count, min_logs);
                                        print_usage(program_name, allowed_opts);
                                        return Err(());
                                    }
                                }
                                Ok(ini)
                            },
                            Err(_) => {
                                println!("Error: minimum_logs '{}' is not a valid number", min_logs_str);
                                print_usage(program_name, allowed_opts);
                                Err(())
                            }
                        }
                    } else {
                        println!("Error: minimum_logs missing in [default] section");
                        print_usage(program_name, allowed_opts);
                        Err(())
                    }
                } else {
                    println!("Error: log_path missing in [default] section");
                    print_usage(program_name, allowed_opts);
                    Err(())
                }
            } else {
                println!("Error: [default] section missing in config file");
                print_usage(program_name, allowed_opts);
                Err(())
            }
        },
        Err(_) => {
            println!("Error: Failed to load or parse config file");
            print_usage(program_name, allowed_opts);
            Err(())
        }
    }
}