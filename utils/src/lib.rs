// SPDX-License-Identifier: GPL-3.0-or-later
use getopts::Options;

pub fn print_usage(program_name: &str, allowed_opts: &Options) {
    let brief = format!("Usage: {} -cFILE [options]", program_name);
    print!("{}", allowed_opts.usage(&brief));
}