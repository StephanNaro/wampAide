// SPDX-License-Identifier: GPL-3.0-or-later
use cli_options::parse_commandline_options;
use config::load_config_options;
use getopts::Options;
use std::env;
use std::fs;
use std::io::{self, Write};
use std::time::SystemTime;
use glob::glob;

fn limit_logs(logpath: &str, limitlogs_count: u16) -> io::Result<()> {
    // Collect files for each pattern separately
    let patterns = ["access.log.*", "error.log.*"];
    let mut access_files: Vec<(String, SystemTime)> = Vec::new();
    let mut error_files: Vec<(String, SystemTime)> = Vec::new();

    for pattern in patterns.iter() {
        let glob_pattern = format!("{}/{}", logpath, pattern);
        for entry in glob(&glob_pattern).expect("Failed to read glob pattern") {
            match entry {
                Ok(path) => {
                    if let Ok(metadata) = path.metadata() {
                        if metadata.is_file() {
                            let modified = metadata.modified().unwrap_or(SystemTime::UNIX_EPOCH);
                            let file_entry = (path.display().to_string(), modified);
                            if pattern.contains("access.log") {
                                access_files.push(file_entry);
                            } else {
                                error_files.push(file_entry);
                            }
                        }
                    }
                }
                Err(e) => println!("Warning: Failed to process file: {}", e),
            }
        }
    }

    // Process each pattern independently
    for (files, pattern_name) in [(&mut access_files, "access.log.*"), (&mut error_files, "error.log.*")] {
        // Sort files by modified time (newest first)
        files.sort_by(|a, b| b.1.cmp(&a.1));

        // Check if files exceed count
        if files.len() <= limitlogs_count as usize {
            println!("No {} files to delete: {} files found, limit is {}", pattern_name, files.len(), limitlogs_count);
            continue;
        }

        // Print files that would be deleted (oldest files)
        let to_delete = &files[limitlogs_count as usize..];
        println!("{} files exceeding limit ({}):", pattern_name, limitlogs_count);
        for (path, _) in to_delete {
            println!("  {}", path);
        }

        // Prompt user for deletion
        print!("Delete these {} {} files? (y/n): ", to_delete.len(), pattern_name);
        io::stdout().flush()?;
        let mut input = String::new();
        io::stdin().read_line(&mut input)?;
        if input.trim().to_lowercase() == "y" {
            for (path, _) in to_delete {
                fs::remove_file(path)?;
                println!("Deleted: {}", path);
            }
        } else {
            println!("Deletion of {} files cancelled", pattern_name);
        }
    }

    Ok(())
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let program_name = args[0].clone();

    let mut allowed_opts = Options::new();
    allowed_opts.optopt("c", "config", "set configuration file", "FILE");
    allowed_opts.optflag("h", "help", "print this help menu");
    allowed_opts.optopt("l", "limitlogs", "limit logfiles to count", "COUNT");

    let commandline_options = match parse_commandline_options(&args[1..], &program_name, &allowed_opts) {
        Ok(p) => p,
        Err(_) => return,
    };

    let config_options = match load_config_options(&commandline_options, &program_name, &allowed_opts) {
        Ok(cfg) => cfg,
        Err(_) => return,
    };

    // match-handling of non-config options
    if let Some(limitlogs_count) = commandline_options.log_count {
        let log_path = config_options["default"]["log_path"].clone().unwrap(); // Safe due to load_config_options checks
        if let Err(e) = limit_logs(&log_path, limitlogs_count) {
            println!("Error in limit_logs: {}", e);
        }
    }
}