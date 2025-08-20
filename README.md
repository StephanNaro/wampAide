# wampAide

Command line utility that looks for old Apache logs and prompts the user for their deletion. It parses the command line, and reads a config file, for settings.

## Software Requirements

1. **To Build**:
    - Overall, the 2024 edition of Rust.
    - For crate dependencies, see the individual `Cargo.toml` files.
2. **To Run**:
    - Windows. (Tested under Windows 11.)

## Usage

Run `wampAide --help` to see options.

- The NAME of a config file must always be supplied with `-c NAME`.
    - See [sample.ini](sample.ini) for sample config file.
- Only one other parameter is so far implemented: `-l COUNT`, which sets the limit, ie the number of logs.
- The full command line is 'wampAide -c NAME -l COUNT`.
    - eg 'wampAide -c sample.ini -l 35`.

## Testing

See [Testing.md](Testing.md)

## Potential Future Improvements

- Enforce the use of at least two command line options, including the config file option.
- `limit_logs`() should continue to keep separate counts for the different file patterns, but should print all together before asking for deletion only once.
- Use relative paths / environment variables in config file.
- Add dry-run mode for `limit_logs`.
- Configure patterns in config file.
- Handle edge cases (e.g., permission errors).
- Incorporate various solutions currently performed by my Batch and PHP scripts.

## Notes

- Nothing much of a program, but it has provided me with a win experience in Rust development after initially struggling with embedded projects' even steeper Rust learning curve.
- Being new to Rust, I couldn't have completed this without recourse to assistance from an AI agent.
- Being ***A***I, the agent couldn't have accomplished this without my firm, expert guidance.

## License

This project is licensed under the GNU General Public License v3.0 (GPL-3.0). See [LICENSE](LICENSE) for details.
