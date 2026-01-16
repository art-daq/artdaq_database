# readfhicl.cc

**Path:** `artdaq-database/Utilities/readfhicl.cc`

**Purpose:** FHiCL file reader and display utility with options for include resolution, nil value pruning, and prolog section display. This tool helps developers inspect and debug FHiCL configurations by showing them in various processed forms.

## Overview

The `readfhicl` utility reads FHiCL configuration files and outputs them with various processing options. Unlike `fhicl2json`, this tool outputs FHiCL format (not JSON) and provides fine-grained control over include resolution, nil value handling, and prolog visibility. It uses the official fhiclcpp library for parsing.

## Key Concepts

### Snippet Mode vs Full Resolution

- **Snippet Mode (default):** Preserves `#include` directives as special keys (e.g., `fhicl_pound_include_0:`), allowing inspection of the file structure without resolving includes
- **Full Resolution (`-r`):** Resolves all `#include` directives, expanding included files inline to show the complete merged configuration

### Prolog Sections

FHiCL files can have a prolog section (`BEGIN_PROLOG` ... `END_PROLOG`) containing reusable definitions. By default, prolog content is hidden. Use `--showprolog` to display it separately from the main configuration.

### Nil Value Pruning

FHiCL supports `@nil` values to explicitly mark undefined keys. The `--prune` option removes these entries from output, showing only keys with actual values.

### Include Directive Transformation

In snippet mode, `#include` directives are transformed to preserve their location:
```fhicl
#include "file.fcl"
```
becomes:
```fhicl
fhicl_pound_include_0:
```

## Thread Safety

- **Thread Safety:** Not thread-safe
- **Concurrent Access:** Safe to run multiple instances on different files
- **Locking:** No internal locking

## Dependencies

| Include | Purpose |
|---------|---------|
| `<fstream>` | File input stream for reading FHiCL file |
| `<iostream>` | Console I/O for stdout/stderr output |
| `<regex>` | Regular expressions for include directive handling |
| `boost/program_options.hpp` | Command-line parsing |
| `cetlib/includer.h` | FHiCL include file resolution |
| `fhiclcpp/extended_value.h` | FHiCL value representation with metadata |
| `fhiclcpp/intermediate_table.h` | FHiCL parse tree representation |
| `fhiclcpp/parse.h` | FHiCL parser interface |
| `fhiclcpp/parse_shims.h` | Parser shims for snippet mode control |
| `artdaq-database/SharedCommon/process_exit_codes.h` | Exit code constants |
| `artdaq-database/SharedCommon/printStackTrace.h` | Exception diagnostic information |

## Command-Line Options

| Option | Short | Required | Description |
|--------|-------|----------|-------------|
| `--config` | `-c` | Yes | Path to FHiCL configuration file |
| `--resolve` | `-r` | No | Fully resolve all `#include` directives |
| `--prune` | `-p` | No | Remove `@nil` values from output |
| `--showprolog` | `-s` | No | Display the prolog section (hidden by default) |
| `--help` | `-h` | No | Display help message |

## Functions

### `main(argc, argv) -> int`

**Brief:** Entry point that parses command-line arguments, reads the FHiCL file, optionally transforms include directives, parses the content, and outputs the formatted FHiCL with the requested processing options applied.

**Parameters:**
- `argc` - Number of command-line arguments
- `argv` - Array of command-line argument strings

**Preconditions:**
- The FHiCL file specified by `--config` must exist and be readable
- If using `-r` with includes, `FHICL_FILE_PATH` should be set appropriately

**Returns:**
- `0` - File processed successfully
- `1` - Help message displayed
- `2` - No configuration file specified
- `-1` - Command-line parsing error
- `process_exit_code::UNCAUGHT_EXCEPTION` (144) - Unhandled exception (e.g., parse error, file not found)

**Postconditions:**
- Formatted FHiCL output written to stdout

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `bpo::error` | Command-line parsing error (caught and reported) |
| `fhicl::exception` | FHiCL parsing error (caught at top level) |
| (caught at top level) | All other exceptions caught and reported |

**Thread Safety:** Not thread-safe

**Side Effects:**
- Sets `FHICL_FILE_PATH` to "." if not already set
- Sets snippet mode in fhiclcpp parser (unless `-r` is used)
- Writes formatted FHiCL to stdout

### `operator<<(out, tuple) -> std::ostream&`

**Brief:** Custom output stream operator that formats `fhicl::extended_value` objects for display, handling tables, sequences, and primitive values with proper indentation and prolog filtering.

**Parameters:**
- `out` - Output stream to write to
- `tuple` - A tuple containing:
  - `fhicl::extended_value const&` - The FHiCL value to format
  - `std::size_t` - Current indentation level
  - `bool` - Whether to show prolog values (`true`) or main values (`false`)

**Preconditions:**
- `out` must be a valid output stream

**Returns:** Reference to the output stream for chaining

**Postconditions:**
- The value is formatted and written to the stream with appropriate indentation

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Does not throw; all formatting is done safely |

**Thread Safety:** Not thread-safe (standard stream operator)

**Side Effects:**
- Writes to the output stream

## Usage Examples

### Basic Display

```bash
# Display FHiCL file structure (includes not resolved)
readfhicl -c config.fcl
```

### Fully Resolved

```bash
# Set include path first
export FHICL_FILE_PATH=/path/to/includes

# Show fully resolved configuration with all includes expanded
readfhicl -c config.fcl -r
```

### With Nil Value Pruning

```bash
# Remove @nil values from output
readfhicl -c config.fcl -p
```

### Show Prolog Section

```bash
# Display prolog section separately
readfhicl -c config.fcl -s
```

### Combined Options

```bash
# Fully resolve, prune nil values, and show prolog
readfhicl -c config.fcl -r -p -s
```

## Implementation Details

### Include Directive Handling

In snippet mode, `#include` directives are converted to special keys before parsing:

```cpp
auto regex = std::regex{"(#include\\s)([^'\"]*)"};
std::smatch match;
auto begin = conf.cbegin();
size_t offset = 0;

while (std::regex_search(begin, conf.cend(), match, regex)) {
  conf.replace(offset + match.position(), match.length(),
               "fhicl_pound_include_" + std::to_string(idx++) + ":");
  offset += match.position() + match.length();
  std::advance(begin, match.position() + match.length());
}
```

### Prolog Filtering

Values are filtered by their `in_prolog` flag:

```cpp
if (value.in_prolog != in_prolog) {
  return out;  // Skip values not matching the current filter
}
```

## Output Format

### Standard Output
```fhicl
key1: value1
table1:
 {
  nested_key: nested_value
 }
sequence1:
 [ item1 item2 item3 ]
```

### With Prolog (`-s` flag)
```fhicl
BEGIN_PROLOG
prolog_key: prolog_value
reusable_table:
 {
  common_setting: 42
 }
END_PROLOG

main_key: main_value
uses_prolog: @local::reusable_table
```

### Without Pruning (shows nil values)
```fhicl
active_key: value
disabled_key:@nil
another_key: value
```

### With Pruning (`-p` flag)
```fhicl
active_key: value
another_key: value
```

## Exit Codes

| Code | Constant | Meaning |
|------|----------|---------|
| 0 | Success | File processed successfully |
| 1 | (help) | Help message displayed |
| 2 | (error) | No configuration file specified |
| -1 | (error) | Command-line parsing error |
| 144 | `process_exit_code::UNCAUGHT_EXCEPTION` | Unhandled exception (e.g., parse error, file not found) |

## Error Handling

- File not found errors are reported by the FHiCL parser
- Syntax errors are reported with line/column information
- Include resolution failures are handled by the cetlib includer

```cpp
} catch (...) {
  std::cerr << "Process exited with error: "
            << ::debug::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}
```

## Environment Variables

| Variable | Purpose |
|----------|---------|
| `FHICL_FILE_PATH` | Search path for FHiCL `#include` directives (colon-separated). Defaults to "." if not set |

**Example:**
```bash
export FHICL_FILE_PATH=/path/to/includes:/another/path
readfhicl -c config.fcl -r
```

## Common Pitfalls

- **Include Resolution:** Without `-r`, includes appear as placeholder keys (e.g., `fhicl_pound_include_0:`)
- **Prolog Visibility:** Prolog section is hidden by default; use `-s` to see it
- **Nil Values:** `@nil` values may clutter output; use `-p` to remove them
- **File Encoding:** Assumes UTF-8 encoding
- **Include Paths:** Ensure `FHICL_FILE_PATH` is set correctly when using `-r` with includes

### Anti-patterns

```bash
# DON'T: Expect to see includes without -r flag
readfhicl -c config.fcl  # Will show fhicl_pound_include_N placeholders

# DO: Use -r to see resolved includes
readfhicl -c config.fcl -r
```

## Relationship to Other Components

- **fhiclcpp Library:** Uses the official FHiCL C++ parser
- **cetlib:** Uses cetlib for include file resolution
- **fhicl2json:** Alternative that outputs JSON instead of FHiCL
- **refactorfhicl:** Related tool for restructuring FHiCL files

## See Also

- [fhicl2json.cc.md](./fhicl2json.cc.md) - Convert FHiCL to JSON
- [refactorfhicl.cc.md](./refactorfhicl.cc.md) - Refactor FHiCL by extracting tables
- [External: fhiclcpp](https://cdcvs.fnal.gov/redmine/projects/fhicl-cpp) - FHiCL C++ library documentation
