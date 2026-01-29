# conftool.cc

**Path:** `artdaq-database/Utilities/conftool.cc`

**Purpose:** The main command-line interface for all artdaq-database operations. This tool provides unified access to read/write documents and configurations, delegating to the `conftool_impl` function from the ConfigurationDB module.

## Overview

The `conftool` utility is the primary command-line interface for interacting with the artdaq-database. It serves as a thin wrapper around the `conftool_impl` function, handling initialization, command-line parsing, and result output. The tool supports various operations including reading/writing documents and configurations, with output directed to files or stdout depending on the operation type.

## Key Concepts

### Operation Modes

The tool operates in different modes based on the requested operation:

- **Read Document/Configuration (`readdocument`, `readconfiguration`):** Retrieves data from the database and writes it to a file specified by `resultFileName()`
- **Write Document/Configuration (`writedocument`, `writeconfiguration`):** Stores data in the database and returns success silently
- **Other Operations:** Returns results directly to stdout as a JSON string

### ManageDocumentOperation

The tool uses the `ManageDocumentOperation` class to parse and manage command-line options. This class encapsulates all parameters needed for database operations and provides methods for reading/writing option data as JSON.

### Fake Time Mode

The tool enables `useFakeTime(true)` which provides deterministic timestamps for testing purposes. This ensures consistent document metadata during automated tests.

## Thread Safety

- **Thread Safety:** Not thread-safe
- **Concurrent Access:** Single-threaded command-line tool; do not run multiple instances on the same database simultaneously without proper coordination
- **Locking:** No internal locking; relies on database provider locking mechanisms

## Dependencies

| Include | Purpose |
|---------|---------|
| `test/common.h` | Common test utilities including `debug` namespace and exception handling |
| `artdaq-database/ConfigurationDB/common.h` | Configuration database common definitions and literals |
| `artdaq-database/ConfigurationDB/conftoolifc.h` | The `conftool_impl` function that executes database operations |

## Command-Line Options

The tool inherits its command-line options from `ManageDocumentOperation`. Common options include:

| Option | Short | Description |
|--------|-------|-------------|
| `--operation` | `-o` | Database operation to perform (e.g., `readdocument`, `writedocument`) |
| `--collection` | `-c` | Target collection name |
| `--entity` | `-e` | Entity name |
| `--version` | `-v` | Version identifier |
| `--format` | `-f` | Data format (`json`, `fhicl`, `gui`) |
| `--result` | `-r` | Result file name for read operations |
| `--source` | `-s` | Source file name for write operations |

## Functions

### `main(argc, argv) -> int`

**Brief:** Entry point that initializes the configuration tool environment, parses command-line arguments, executes the requested database operation, and handles the result output appropriately based on operation type.

**Parameters:**
- `argc` - Number of command-line arguments
- `argv` - Array of command-line argument strings

**Preconditions:**
- `ARTDAQ_DATABASE_URI` environment variable should be set to specify the database connection

**Returns:**
- `process_exit_code::SUCCESS` (0) - Operation completed successfully
- `process_exit_code::FAILURE` (1) - Operation failed (error message provided via stdout)
- `process_exit_code::UNCAUGHT_EXCEPTION` (144) - Unhandled exception occurred

**Postconditions:**
- For read operations: Result written to file specified by `resultFileName()`
- For write operations: Document stored in database
- For other operations: Result printed to stdout

**Throws:**

| Exception | Condition |
|-----------|-----------|
| (caught internally) | All exceptions are caught and reported via `current_exception_diagnostic_information()` |

**Thread Safety:** Not thread-safe; single invocation expected

**Side Effects:**
- Enables trace logging via `impl::enable_trace()`
- Registers signal handlers for graceful error reporting
- Enables fake time mode for deterministic timestamps
- May create/modify files on disk (for read operations)
- May create/modify documents in database (for write operations)

**Example:**
```bash
# Reading a document from the database
conftool --operation readdocument \
         --collection MyProcesses \
         --entity component01 \
         --version v1.0 \
         --format fhicl \
         --result output.fcl
```

## Usage Examples

### Reading a Document

```bash
# Read a document from the database and save to file
export ARTDAQ_DATABASE_URI=filesystemdb:///path/to/db
conftool --operation readdocument \
         --collection MyProcesses \
         --entity component01 \
         --version v1.0 \
         --format fhicl \
         --result output.fcl
```

Output:
```
Wrote file:output.fcl
```

### Writing a Document

```bash
# Write a document to the database
export ARTDAQ_DATABASE_URI=filesystemdb:///path/to/db
conftool --operation writedocument \
         --collection MyProcesses \
         --entity component01 \
         --version v1.0 \
         --format fhicl \
         --source input.fcl
```

### Listing Configurations

```bash
# List all configurations
export ARTDAQ_DATABASE_URI=filesystemdb:///path/to/db
conftool --operation listconfigurations
```

Output:
```
Results:{"configurations":["config1","config2",...]}
```

## Implementation Details

### Main Function Flow

```cpp
int main(int argc, char* argv[]) try {
  // 1. Enable trace logging
  impl::enable_trace();

  // 2. Register signal handlers for clean error reporting
  debug::registerUngracefullExitHandlers();

  // 3. Enable deterministic timestamps for testing
  artdaq::database::useFakeTime(true);

  // 4. Create options object and parse command line
  auto options = std::make_unique<Options>(argv[0]);

  // 5. Execute the database operation
  auto result = db::conftool_impl(options, argc, argv);

  // 6. Handle result based on operation type
  if (!result.first) {
    // Report failure
    return process_exit_code::FAILURE;
  }

  // 7. Output results appropriately
  // - Read ops: write to file
  // - Write ops: return success
  // - Other ops: print to stdout

  return process_exit_code::SUCCESS;
} catch (...) {
  // Handle any uncaught exceptions
  return process_exit_code::UNCAUGHT_EXCEPTION;
}
```

### Result Handling

The tool handles operation results differently based on the operation type:

| Operation | Result Handling |
|-----------|-----------------|
| `readdocument` | Write result to file, print confirmation |
| `readconfiguration` | Write result to file, print confirmation |
| `writedocument` | Return success silently |
| `writeconfiguration` | Return success silently |
| Other | Print result to stdout as JSON |

## Exit Codes

| Code | Constant | Meaning |
|------|----------|---------|
| 0 | `process_exit_code::SUCCESS` | Operation completed successfully |
| 1 | `process_exit_code::FAILURE` | Operation failed (error message provided) |
| 144 | `process_exit_code::UNCAUGHT_EXCEPTION` | Unhandled exception occurred |

## Error Handling

The tool provides comprehensive error handling with diagnostic information:

```cpp
if (!result.first) {
  std::cout << "Failed; error message: " << result.second << "\n";
  std::cout << ::debug::current_exception_diagnostic_information();
  return process_exit_code::FAILURE;
}
```

All uncaught exceptions are caught at the top level and reported with stack trace information.

## Environment Variables

| Variable | Purpose |
|----------|---------|
| `ARTDAQ_DATABASE_URI` | Database connection URI (required). Examples: `filesystemdb:///path/to/db`, `mongodb://host:27017/dbname` |
| `FHICL_FILE_PATH` | Search path for FHiCL includes (required for FHiCL format operations) |

## Relationship to Other Components

- **ConfigurationDB Module:** Uses the `conftool_impl` function for all database operations
- **ManageDocumentOperation:** Uses this class for command-line parsing and option management
- **conftool.py:** Python wrapper that provides higher-level operations built on this tool
- **bulkloader/bulkdownloader:** Specialized tools for multi-threaded bulk operations

## Testing Notes

- The tool enables `useFakeTime(true)` to ensure consistent timestamps during testing
- Debug trace is enabled by default via `impl::enable_trace()` for troubleshooting
- Uses `registerUngracefullExitHandlers()` for clean error reporting on signals

## Common Pitfalls

- **Missing URI:** The tool will fail if `ARTDAQ_DATABASE_URI` environment variable is not set
- **File Permissions:** Ensure write permissions for the result file location when using read operations
- **Format Mismatch:** The `--format` option must match the actual data format stored in the database
- **Collection Case Sensitivity:** Collection names are case-sensitive

### Anti-patterns

```bash
# DON'T: Forget to set the database URI
conftool --operation listconfigurations  # Will fail

# DO: Set the environment variable first
export ARTDAQ_DATABASE_URI=filesystemdb:///path/to/db
conftool --operation listconfigurations
```

## See Also

- [conftool_py.md](./conftool_py.md) - Python wrapper with high-level operations
- [bulkloader.cc.md](./bulkloader.cc.md) - Multi-threaded bulk import operations
- [bulkdownloader.cc.md](./bulkdownloader.cc.md) - Multi-threaded bulk export operations
- [ConfigurationDB/conftoolifc.h](../ConfigurationDB/conftoolifc.h.md) - Interface implementation
- [options_operation_managedocument.h](../ConfigurationDB/options_operation_managedocument.h.md) - Command-line options class
