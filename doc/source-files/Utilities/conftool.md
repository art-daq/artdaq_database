# conftool.cc

## Overview

`conftool` is a command-line wrapper utility that provides direct access to the artdaq-database configuration management interface. It serves as the main entry point for interacting with the database through the `conftool_impl` function.

## Purpose and Use Cases

- Direct command-line access to database configuration operations
- Testing and debugging database operations
- Wrapper for the Python conftool and library implementations
- Provides a C++ executable interface to the configuration database

## Location

**File**: `/home/user/artdaq-database/artdaq-database/Utilities/conftool.cc`

## Main Components

### Key Functions

1. **main()** - Primary entry point that:
   - Enables tracing for debugging
   - Registers ungraceful exit handlers
   - Configures fake time mode for testing
   - Delegates to `conftool_impl` for actual operation

### Workflow

```
1. Initialize Options (ManageDocumentOperation)
2. Call conftool_impl with command-line arguments
3. Process result:
   - For read operations: Write result to file
   - For write operations: Display success message
   - For other operations: Print results to stdout
4. Handle errors and exit appropriately
```

### Dependencies

- `artdaq-database/ConfigurationDB/common.h`
- `artdaq-database/ConfigurationDB/conftoolifc.h`
- `test/common.h`

## Command-Line Usage

The utility accepts various operations through command-line arguments processed by the `conftool_impl` function. The specific arguments depend on the operation being performed.

### Supported Operations

Based on the code, the utility handles:

- **readdocument**: Reads a document from the database and writes to file
- **readconfiguration**: Reads a configuration and writes to file
- **writedocument**: Writes a document to the database
- **writeconfiguration**: Writes a configuration to the database
- Other operations: Results printed to stdout

## Usage Examples

### Reading a Document
```bash
conftool --operation readdocument --collection RunHistory --entity MyEntity --version v1 --output result.json
```

### Writing a Document
```bash
conftool --operation writedocument --collection RunHistory --entity MyEntity --version v1 --input document.json
```

## Error Handling

The utility implements comprehensive error handling:

1. **Operation Failure**: Prints error message and diagnostic information
2. **Uncaught Exceptions**: Catches all exceptions and provides diagnostic info
3. **Exit Codes**:
   - `SUCCESS`: Operation completed successfully
   - `FAILURE`: Operation failed with error message
   - `UNCAUGHT_EXCEPTION`: Unhandled exception occurred

## Testing Features

The utility includes several testing and debugging features:
- Trace mode enabled via `impl::enable_trace()`
- Fake time mode for reproducible testing
- Ungraceful exit handlers for debugging crashes

## Implementation Notes

- Uses namespace aliases for cleaner code: `db`, `impl`
- All operations return a `std::pair<bool, std::string>` indicating success and result/error message
- Results are written to files for read operations, preserving original data format
- The commented-out line suggests optional auto-rebuilding of search indexes

## Related Files

- **Python wrapper**: `/home/user/artdaq-database/artdaq-database/Utilities/conftool.py`
- **Interface header**: `artdaq-database/ConfigurationDB/conftoolifc.h`
- **Common definitions**: `artdaq-database/ConfigurationDB/common.h`
