# refactorfhicl.cc

**Path:** `artdaq-database/Utilities/refactorfhicl.cc`

**Purpose:** Experimental command-line utility for refactoring large FHiCL configuration files into modular components. The tool parses a FHiCL file, extracts top-level tables into separate files with versioned names, and replaces them with `#include` directives. This helps organize monolithic configurations into reusable modules.

**Status:** Experimental/Incomplete - The table content extraction logic is partially commented out in the source code. Use with caution.

## Key Concepts

### FHiCL Table Extraction

The tool identifies top-level FHiCL tables (nested configuration blocks) and writes each to a separate file. The original table location is replaced with an `#include` directive referencing the extracted file.

### Version Tracking with Hashing

A hash-based versioning system tracks extracted content. When the same logical table appears multiple times with different content, each unique version gets a distinct version suffix (e.g., `tablename_v1.fcl`, `tablename_v2.fcl`). Identical content reuses existing versions.

```cpp
auto lookup_version = [](auto const& name, auto const& buffer) {
  static auto subconfig_versionhash_map =
    std::map<std::string, std::vector<std::size_t>>{};

  auto hash = std::hash<std::string>{}(buffer);
  // Returns _v1, _v2, etc. based on content hash
};
```

### Output Directory

All refactored files are written to a `../refactored` directory relative to the current working directory. This directory is created automatically if it does not exist.

## Thread Safety

- **Thread Safety:** Not thread-safe
- **Concurrent Access:** Not designed for concurrent use
- **Locking:** No internal locking mechanisms

## Dependencies

| Include | Purpose |
|---------|---------|
| `<fstream>` | File stream I/O for reading/writing FHiCL files |
| `<iostream>` | Console output for status messages |
| `<regex>` | Regular expressions for include directive handling |
| `boost/program_options.hpp` | Command-line argument parsing |
| `boost/any.hpp` | Type-erased value storage |
| `boost/exception/diagnostic_information.hpp` | Exception diagnostics |
| `artdaq-database/SharedCommon/fileststem_functions.h` | Directory creation (mkdir) |
| `artdaq-database/SharedCommon/printStackTrace.h` | Exception stack traces |
| `artdaq-database/SharedCommon/process_exit_codes.h` | Standard exit codes |
| `cetlib/includer.h` | FHiCL include file resolution |
| `fhiclcpp/extended_value.h` | FHiCL value representation with type information |
| `fhiclcpp/intermediate_table.h` | Parsed FHiCL structure |
| `fhiclcpp/parse.h` | FHiCL parser functions |
| `fhiclcpp/parse_shims.h` | Parser mode control (snippet mode) |

## Command-Line Interface

### Synopsis

```bash
refactorfhicl -c <config-file> [options]
```

### Options

| Option | Short | Required | Description |
|--------|-------|----------|-------------|
| `--config` | `-c` | Yes | Path to FHiCL configuration file to refactor |
| `--prune` | `-p` | No | Remove `@nil` values from output |
| `--unresolved` | `-u` | No | Do not fully resolve includes (currently has a bug - see Known Issues) |
| `--showprolog` | `-s` | No | Include the FHiCL prolog section in output |
| `--help` | `-h` | No | Display help message and exit |

## Functions

### `main(argc, argv) -> int`

**Brief:** Entry point that parses command-line arguments, reads and parses the input FHiCL file, and writes refactored output with extracted tables to the `../refactored` directory.

**Parameters:**
- `argc` - Number of command-line arguments
- `argv` - Array of command-line argument strings

**Preconditions:**
- The `--config` argument must be provided
- Input FHiCL file must exist and be readable
- `FHICL_FILE_PATH` environment variable should be set (defaults to "." if unset)
- Parent directory of `../refactored` must be writable

**Returns:** Process exit code (0 for success, non-zero for various error conditions)

**Postconditions:**
- On success, refactored files exist in `../refactored/` directory
- Main configuration file and extracted table files are created

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `boost::program_options::error` | Invalid command-line arguments |
| FHiCL parse errors | Malformed FHiCL syntax |
| Any exception | Caught at top level, returns UNCAUGHT_EXCEPTION |

**Thread Safety:** Not thread-safe

**Side Effects:**
- Creates `../refactored/` directory if it does not exist
- Writes multiple files to the output directory
- May modify FHICL_FILE_PATH environment variable

### `operator<<(out, tuple) -> std::ostream&`

**Brief:** Custom output operator that handles FHiCL value serialization with table extraction. Tables are written to separate files and replaced with include directives; sequences and atoms are written inline.

**Parameters:**
- `out` - Output stream to write to
- `tuple` - Tuple containing: key name, FHiCL extended_value, indentation level, prolog flag

**Returns:** Reference to the output stream

**Side Effects:**
- For TABLE values: Creates new file in `../refactored/` and writes include directive
- Writes formatted FHiCL content to output stream

### `lookup_version(name, buffer) -> std::string`

**Brief:** Lambda function that determines the version suffix for an extracted table based on content hash. Returns existing version if identical content was previously extracted, or creates new version number.

**Parameters:**
- `name` - Name of the table being extracted
- `buffer` - String content of the table

**Returns:** Version suffix string (e.g., "_v1", "_v2")

## Known Issues

### Incomplete Table Extraction

The table content extraction code is partially commented out in the source:

```cpp
if (value.is_a(::fhicl::TABLE)) {
  out << key << " : " << "{\n";
  std::stringstream ss;
  // The following is commented out:
  // for (auto& entry : tab) {
  //   ss << std::make_tuple<...>(entry.first, entry.second, ...);
  // }

  // Creates file with empty content
  auto tmp_file = std::string{key}.append(lookup_version(key, ss.str())).append(".fcl");
  std::ofstream os(std::string{target_path}.append("/").append(tmp_file));
  os << ss.str();  // ss is empty
  // ...
}
```

### --unresolved Flag Bug

The `--unresolved` flag handling has a bug where `fully_resolve` is always set to `true`:

```cpp
bool fully_resolve = true;

if (vm.count("unresolved") != 0u) {
  fully_resolve = true;  // BUG: Should be false!
}
```

## Usage Examples

### Basic Refactoring

```bash
# Set FHiCL include path
export FHICL_FILE_PATH=".:/path/to/includes"

# Refactor a configuration file
refactorfhicl -c large_config.fcl

# Check output
ls ../refactored/
# large_config.fcl  table1_v1.fcl  table2_v1.fcl
```

### Include Prolog Section

```bash
# Include prolog in output
refactorfhicl -c config.fcl -s

# The output will contain BEGIN_PROLOG/END_PROLOG sections
```

### Prune Nil Values

```bash
# Remove @nil values from refactored output
refactorfhicl -c config.fcl -p
```

## Output Structure

```
../refactored/
  original_config.fcl       # Main file with #include directives
  tablename1_v1.fcl         # First extracted table
  tablename2_v1.fcl         # Second extracted table (different content)
  tablename2_v2.fcl         # Same table name, different content version
```

### Example Output Format

**Main file (refactored):**
```
table1 : {
#include "table1_v1.fcl"
}

table2 : {
#include "table2_v1.fcl"
}
```

## Exit Codes

| Code | Meaning |
|------|---------|
| 0 | Refactoring completed successfully |
| 1 | Help message was displayed |
| 2 | No configuration file specified |
| 3 | Failed to create output directory |
| -1 | Command-line parsing error |
| Variable | UNCAUGHT_EXCEPTION for unhandled errors |

## Common Pitfalls

- **Empty Extracted Files:** Due to the incomplete implementation, extracted table files may be empty
- **Fixed Output Directory:** Output always goes to `../refactored`, which cannot be changed
- **Version Number Collisions:** Tables with identical names but slightly different content get different version numbers, which may not match expectations
- **FHICL_FILE_PATH:** If not set, uses current directory only; complex includes may fail

## Environment Variables

| Variable | Description |
|----------|-------------|
| `FHICL_FILE_PATH` | Colon-separated list of directories to search for FHiCL include files. Defaults to "." if not set. |

## Relationship to Other Components

- **readfhicl.cc:** Similar FHiCL parsing approach but for display rather than refactoring
- **fhicl2json.cc:** Format conversion utility using the same FHiCL parser
- **fhiclcpp library:** Provides the parsing and value representation infrastructure

## Future Improvements

To make this tool production-ready:

1. Uncomment and fix the table content extraction loop
2. Fix the `--unresolved` flag handling
3. Add a command-line option for output directory
4. Implement recursive table extraction for nested structures
5. Add validation of generated output
6. Improve error messages and recovery

## See Also

- [readfhicl.cc.md](./readfhicl.cc.md) - FHiCL display utility
- [fhicl2json.cc.md](./fhicl2json.cc.md) - FHiCL to JSON conversion
- [External: fhiclcpp](https://cdcvs.fnal.gov/redmine/projects/fhicl-cpp) - FHiCL C++ library documentation
