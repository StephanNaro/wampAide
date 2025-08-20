// SPDX-License-Identifier: GPL-3.0-or-later
use getopts::Options;
use utils::print_usage;

// Struct to hold parsed command-line options
#[derive(Debug)]
pub struct ParsedOptions {
    pub config_path: String,
    pub log_count: Option<u16>,
}

pub fn parse_commandline_options(args: &[String], program_name: &str, allowed_opts: &Options) -> Result<ParsedOptions, ()> {
    match allowed_opts.parse(args) {
        Ok(matches) => {
            if matches.opt_present("h") {
                print_usage(program_name, allowed_opts);
                Err(())
            } else if !matches.opt_present("c") {
                println!("Error: Configuration file required");
                print_usage(program_name, allowed_opts);
                Err(())
            } else {
                let config_path = matches.opt_str("c").unwrap(); // Safe due to opt_present check
                if let Some(count_str) = matches.opt_str("l") {
                    match count_str.parse::<u16>() {
                        Ok(limitlogs_count) => Ok(ParsedOptions { config_path, log_count: Some(limitlogs_count) }),
                        Err(_) => {
                            println!("Error: Invalid limitlogs count '{}'", count_str);
                            print_usage(program_name, allowed_opts);
                            Err(())
                        }
                    }
                } else {
                    Ok(ParsedOptions { config_path, log_count: None })
                }
            }
        },
        Err(f) => {
            println!("Error: {}", f.to_string());
            print_usage(program_name, allowed_opts);
            Err(())
        }
    }
}