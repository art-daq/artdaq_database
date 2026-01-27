# fhicl2json.cc

**Path:** `artdaq-database/Utilities/fhicl2json.cc`

**Purpose:** Format conversion utility that converts FHiCL (Fermilab Hierarchical Configuration Language) files to JSON format. This tool is essential for viewing FHiCL configurations in a structured JSON format or for processing with JSON-based tools.

## Overview

The `fhicl2json` utility reads a FHiCL configuration file and converts it to JSON format suitable for storage in the artdaq-database. It supports extracting just the main data subtree (without metadata envelope) or outputting the full document structure. Output is written to stdout, allowing piping to files or other utilities.

## Key Concepts

### FHiCL Format

FHiCL (Fermilab Hierarchical Configuration Language) is a configuration language used in the art framework and artdaq. It supports:
- Key-value pairs with automatic type inference
- Nested tables (similar to JSON objects)
- Sequences (similar to JSON arrays)
- `#include` directives for file composition
- Prolog sections for reusable definitions
- References and substitution syntax

### Conversion Process

The tool uses the `fhicljson` library to:
1. Parse FHiCL syntax into an intermediate representation
2. Convert to the artdaq-database internal JSON document format
3. Wrap the data in a document envelope with metadata

### Document Structure

The output JSON has this structure:
```json
{
  "document": {
    "data": {
      "main": { /* actual configuration data */ },
      "prolog": { /* prolog definitions if present */ }
    },
    "metadata": { /* document metadata */ }
  }
}
```

### Main Subtree Extraction

When the `--main` flag is used, only the `document.data.main` subtree is extracted and output, removing the metadata envelope. This is useful when you want just the raw configuration data.

## Thread Safety

- **Thread Safety:** Not thread-safe
- **Concurrent Access:** Safe to run multiple instances on different files
- **Locking:** No internal locking; uses global locale settings

## Dependencies

| Include | Purpose |
|---------|---------|
| `<fstream>` | File input stream for reading FHiCL file |
| `<iostream>` | Console I/O for stdout/stderr output |
| `boost/program_options.hpp` | Command-line parsing |
| `artdaq-database/ConfigurationDB/configurationdb.h` | Database configuration and locale setup |
| `artdaq-database/DataFormats/Fhicl/fhicljsondb.h` | FHiCL to JSON conversion functions |
| `artdaq-database/DataFormats/Json/json_common.h` | JSON common definitions |
| `artdaq-database/JsonDocument/JSONDocument.h` | JSON document manipulation for `--main` extraction |
| `artdaq-database/SharedCommon/process_exit_codes.h` | Exit code constants |
| `artdaq-database/SharedCommon/printStackTrace.h` | Exception diagnostic information |

## Command-Line Options

| Option | Short | Required | Description |
|--------|-------|----------|-------------|
| `--config` | `-c` | Yes | Path to FHiCL configuration file |
| `--main` | `-m` | No | Output only the `document.data.main` subtree |
| `--help` | `-h` | No | Display help message |

## Functions

### `main(argc, argv) -> int`

**Brief:** Entry point that parses command-line arguments, reads the FHiCL file, converts it to JSON format, optionally extracts the main subtree, and outputs the result to stdout.

**Parameters:**
- `argc` - Number of command-line arguments
- `argv` - Array of command-line argument strings

**Preconditions:**
- The FHiCL file specified by `--config` must exist and be readable
- If file contains `#include` directives, `FHICL_FILE_PATH` should be set

**Returns:**
- `process_exit_code::SUCCESS` (0) - Conversion completed successfully
- `1` - Help message displayed
- `2` - No configuration file specified
- `-1` - Command-line parsing error
- `process_exit_code::FAILURE` (1) - Conversion failed
- `process_exit_code::UNCAUGHT_EXCEPTION` (144) - Unhandled exception occurred

**Postconditions:**
- JSON output written to stdout

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `bpo::error` | Command-line parsing error (caught and reported) |
| (caught at top level) | All other exceptions caught and reported |

**Thread Safety:** Not thread-safe; uses global locale

**Side Effects:**
- Sets `FHICL_FILE_PATH` to "." if not already set
- Sets default locale
- Writes JSON to stdout

### `fhicl_to_json(fcl, filename) -> result_t`

**Brief:** Converts a FHiCL string to JSON format using the fhicljson library.

**Parameters:**
- `fcl` - The FHiCL content as a string
- `filename` - Original filename (used for error messages and include resolution)

**Preconditions:**
- `fcl` should contain valid FHiCL syntax

**Returns:** A `std::pair<bool, std::string>`:
- First: `true` if conversion succeeded, `false` otherwise
- Second: On success, the JSON output; on failure, an error message

**Postconditions:**
- On success, return value contains valid JSON

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally by the conversion library |

**Thread Safety:** Not thread-safe (uses global locale)

**Side Effects:**
- Sets default locale via `artdaq::database::set_default_locale()`

**Example:**
```cpp
auto fhicl_content = "threshold: 100\nenabled: true\n";
auto result = fhicl_to_json(fhicl_content, "config.fcl");
if (result.first) {
  std::cout << result.second;  // JSON output
} else {
  std::cerr << "Error: " << result.second;
}
```

### `json_to_fhicl(jsn, filename) -> result_t`

**Brief:** Converts a JSON string back to FHiCL format. Defined for completeness but not used by the main function.

**Parameters:**
- `jsn` - The JSON content as a string
- `filename` - Output filename reference (modified by the function)

**Preconditions:**
- `jsn` should contain valid JSON in the expected document format

**Returns:** A `std::pair<bool, std::string>`:
- First: `true` if conversion succeeded, `false` otherwise
- Second: On success, the FHiCL output; on failure, an error message

**Postconditions:**
- On success, return value contains valid FHiCL

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally |

**Thread Safety:** Not thread-safe (uses global locale)

**Side Effects:**
- Sets default locale

## Usage Examples

### Basic Conversion

```bash
# Convert FHiCL to full JSON document
fhicl2json -c config.fcl > config.json
```

### Extract Main Subtree Only

```bash
# Get just the configuration data without metadata envelope
fhicl2json -c config.fcl -m > config_data.json
```

### Pipe to jq for Pretty-Printing

```bash
# Pretty-print the JSON output
fhicl2json -c config.fcl | jq .
```

### Validate FHiCL Syntax

```bash
# Check if FHiCL file is valid (will fail if syntax error)
fhicl2json -c config.fcl > /dev/null && echo "Valid" || echo "Invalid"
```

## Implementation Details

### Conversion Flow

1. **Environment Check:** Verifies `FHICL_FILE_PATH` is set; defaults to "." if not
2. **File Reading:** Reads the entire FHiCL file into a string buffer
3. **Locale Setup:** Sets default locale for consistent string handling
4. **Conversion:** Calls `fhicl_to_json()` to convert to JSON
5. **Subtree Extraction (optional):** If `--main` flag is set, uses `JSONDocument::findChild()` to extract subtree
6. **Output:** Writes JSON to stdout

### Output Format Examples

**Input FHiCL (`config.fcl`):**
```
threshold: 100
enabled: true
components: [comp1, comp2]
settings: {
  timeout: 30
}
```

**Full output (without `--main`):**
```json
{
  "document": {
    "data": {
      "main": {
        "threshold": "100",
        "enabled": "true",
        "components": ["comp1", "comp2"],
        "settings": {
          "timeout": "30"
        }
      }
    },
    "metadata": {
      "version": "1.0"
    }
  }
}
```

**With `--main` flag:**
```json
{
  "threshold": "100",
  "enabled": "true",
  "components": ["comp1", "comp2"],
  "settings": {
    "timeout": "30"
  }
}
```

## Exit Codes

| Code | Constant | Meaning |
|------|----------|---------|
| 0 | `process_exit_code::SUCCESS` | Conversion completed successfully |
| 1 | (help) | Help message displayed |
| 2 | (error) | No configuration file specified |
| -1 | (error) | Command-line parsing error |
| 1 | `process_exit_code::FAILURE` | Conversion failed (syntax error, file not found, etc.) |
| 144 | `process_exit_code::UNCAUGHT_EXCEPTION` | Unhandled exception occurred |

## Error Handling

File read and conversion errors are reported with descriptive messages:

```cpp
if (!result.first) {
  std::cerr << "Error: Convertion failed, error message: " << result.second << "\n";
  return process_exit_code::FAILURE;
}
```

Common error messages:
- "Unable to locate file": Include file not found in `FHICL_FILE_PATH`
- "syntax error": FHiCL parsing error with line/column information
- "circular dependency": Include files reference each other

## Environment Variables

| Variable | Purpose |
|----------|---------|
| `FHICL_FILE_PATH` | Search path for FHiCL `#include` directives. Colon-separated list of directories. Defaults to "." if not set |

**Example:**
```bash
export FHICL_FILE_PATH=/path/to/includes:/another/path
fhicl2json -c config.fcl
```

## Common Pitfalls

- **Missing Includes:** Ensure `FHICL_FILE_PATH` includes all directories containing included files
- **Circular Includes:** The FHiCL parser will fail on circular include dependencies
- **Binary Output:** The output is not pretty-printed by default; use `jq` for formatting
- **Encoding:** Assumes UTF-8 encoding for input files
- **Value Types:** FHiCL values are converted to strings in the JSON output

### Anti-patterns

```bash
# DON'T: Assume includes are found without FHICL_FILE_PATH
fhicl2json -c config_with_includes.fcl  # May fail

# DO: Set FHICL_FILE_PATH first
export FHICL_FILE_PATH=/path/to/fcl/includes
fhicl2json -c config_with_includes.fcl
```

## Relationship to Other Components

- **fhicljson Library:** Uses this library for the actual FHiCL parsing and conversion
- **JSONDocument:** Uses for subtree extraction with `--main` flag via `findChild()`
- **readfhicl:** Complementary tool that displays processed FHiCL (not JSON)
- **conftool:** Uses similar conversion internally for FHiCL format storage

## See Also

- [readfhicl.cc.md](./readfhicl.cc.md) - Read and display FHiCL with processing options
- [readwrite_js.md](./readwrite_js.md) - Node.js FHiCL/JSON round-trip utility
- [DataFormats/Fhicl/fhicljsondb.h](../DataFormats/Fhicl/fhicljsondb.h.md) - FHiCL/JSON conversion library
- [External: FHiCL specification](https://cdcvs.fnal.gov/redmine/projects/fhicl) - Official FHiCL documentation
