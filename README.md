# wampAide

A lightweight command-line utility for Windows that inspects Apache logs (`access.log.*` and `error.log.*`), enforces retention limits by keeping the newest files, and prompts the user before deleting old logs. Configuration is managed via an INI file and command-line arguments.

Originally prototyped in Rust, `wampAide` has been vibe-translated and -tested to modern C++ (C++20) for self-contained, statically linked deployment on Windows with zero external runtime dependencies.

## Software Requirements

### 1. To Build
- **CMake** (v3.20 or newer).
- **C++20 compiler**: GCC / MinGW (tested with GCC 16), Clang, or MSVC.
- Zero external package dependencies (no vcpkg or conan required).

### 2. To Run
- **Windows** (tested on Windows 11).
- The binary is statically linked (`-static -static-libgcc -static-libstdc++`) and runs standalone without requiring external runtime DLLs.

## Building

Generate build files and compile using CMake:

```cmd
cmake -B build -G "MinGW Makefiles"
cmake --build build
```

The resulting executable will be placed in `build/wampAide.exe`.

## Usage

```cmd
wampAide.exe -c CONFIG_FILE [options]
```

### Options

| Option | Long Option | Description |
| :--- | :--- | :--- |
| `-c FILE` | `--config FILE` | **Required**. Path to the configuration INI file. |
| `-l COUNT` | `--limitlogs COUNT` | Number of recent logs to keep per pattern. Must be &ge; `minimum_logs` from config. |
| `-v` / `-V` | `--version` | Display version information and exit. |
| `-h` | `--help` | Display version and usage help menu. |

### Example

Using the provided [sample.ini](sample.ini):

```cmd
wampAide.exe -c sample.ini -l 35
```

### Configuration File (`sample.ini`)

```ini
[default]
log_path = c:\wampAideTest
minimum_logs = 14
```

- `log_path`: Target directory containing Apache log files.
- `minimum_logs`: Lower bound safety threshold preventing accidental deletion beyond a sensible count.

## Testing

See [Testing.md](Testing.md) for test scenarios and sample log creation commands.

## Potential Future Improvements

- Enforce the use of at least two command-line options, including the config file option.
- `limit_logs()` should continue to keep separate counts for different file patterns, but print all candidates together before asking for deletion confirmation only once.
- Support relative paths and environment variables in the config file.
- Add a `--dry-run` mode for `limit_logs`.
- Configure file patterns directly in the config file.
- Add robust handling for edge cases (e.g., file lock/permission errors).
- Incorporate tasks currently performed by external Batch and PHP scripts.

## Notes

- Originally created as a win experience in Rust development after struggling with embedded Rust's steep learning curve, subsequently vibe-translated and -tested to modern C++20 for portable native deployment.

## License

This project is licensed under the GNU General Public License v3.0 (GPL-3.0). See [LICENSE](LICENSE) for details.
