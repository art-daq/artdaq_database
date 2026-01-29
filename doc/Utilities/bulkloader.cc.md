# bulkloader.cc

**Path:** `artdaq-database/Utilities/bulkloader.cc`

**Purpose:** High-performance multi-threaded bulk import utility for loading FHiCL configuration files into the database. This tool parallelizes the loading process using multiple worker threads to achieve near-linear speedup for I/O-bound operations.

## Overview

The `bulkloader` utility imports FHiCL configuration files from a directory into the artdaq-database. It uses a thread pool of worker threads that simultaneously read files from a shared queue and write them to the database. The tool is commonly used for archiving run configurations or migrating configurations between systems.

## Key Concepts

### Multi-threaded Work Queue

The tool implements a producer-consumer pattern:
1. The main thread populates a shared queue with file paths
2. Worker threads pop files from the queue and process them
3. Atomic counters track success/failure statistics
4. A mutex protects access to the shared file queue

### Collection Assignment

Files are assigned to collections based on their filename:
- Files ending in `.fcl` (except `schema.fcl`) are assigned to the `RunHistory` collection
- `schema.fcl` files are assigned to the `SystemLayout` collection
- The entity name is derived from the filename stem (without extension)

### Configuration Naming

The configuration is identified by a combination of run number and configuration name in the format: `{run}/{configuration}`. For example: `1001/demo_safemode`.

## Thread Safety

- **Thread Safety:** Thread-safe for concurrent file loading
- **Concurrent Access:** Multiple threads safely access the shared file queue via mutex
- **Locking:**
  - `std::mutex` protects `file_names` vector
  - `std::atomic<int>` used for `loaded_file_count` and `total_error_count`

## Dependencies

| Include | Purpose |
|---------|---------|
| `test/common.h` | Common test utilities and process exit codes |
| `artdaq-database/BasicTypes/data_json.h` | JSON data type support |
| `artdaq-database/ConfigurationDB/configuration_common.h` | Configuration common definitions |
| `artdaq-database/ConfigurationDB/dboperation_managedocument.h` | Document management operations |
| `<thread>` | `std::thread` for worker threads |
| `<mutex>` | `std::mutex` for queue protection |
| `<atomic>` | `std::atomic` for thread-safe counters |
| `boost/filesystem.hpp` | File path operations |
| `boost/program_options.hpp` | Command-line parsing |

## Command-Line Options

| Option | Short | Required | Description |
|--------|-------|----------|-------------|
| `--path` | `-p` | Yes | Path to directory containing FHiCL files |
| `--configuration` | `-c` | Yes | Configuration name (e.g., `demo_safemode`) |
| `--run` | `-r` | Yes | Run number |
| `--threads` | `-t` | No | Number of worker threads (default: hardware concurrency) |
| `--debug` | `-d` | No | Verify by reading configuration composition back from database |
| `--help` | `-h` | No | Display help message |

## Functions

### `main(argc, argv) -> int`

**Brief:** Entry point that parses command-line arguments, spawns worker threads to load FHiCL files in parallel, waits for completion, and reports statistics.

**Parameters:**
- `argc` - Number of command-line arguments
- `argv` - Array of command-line argument strings

**Preconditions:**
- `ARTDAQ_DATABASE_URI` environment variable must be set
- The path specified must exist and contain `.fcl` files
- Configuration name and run number must be provided

**Returns:**
- `process_exit_code::SUCCESS` (0) - All files loaded successfully
- `process_exit_code::HELP` (1) - Help message displayed
- `process_exit_code::FAILURE` (1) - One or more files failed to load
- `process_exit_code::INVALID_ARGUMENT` (128) - Missing required arguments
- `process_exit_code::UNCAUGHT_EXCEPTION` (144) - Unhandled exception occurred

**Postconditions:**
- All successfully loaded files are stored in the database
- Performance statistics are printed to stdout

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `bpo::error` | Command-line parsing error (caught and reported) |
| (caught internally) | All other exceptions caught at top level |

**Thread Safety:** Creates multiple threads internally; safe to call once per process

**Side Effects:**
- Creates worker threads
- Creates/modifies documents in database
- Prints progress and statistics to stdout/stderr

### `write_document(options, file_name) -> int`

**Brief:** Reads a FHiCL file from disk and writes it to the database as a document.

**Parameters:**
- `options` - The `ManageDocumentOperation` options object configured for this file
- `file_name` - Absolute path to the FHiCL file to load

**Preconditions:**
- `file_name` must not be empty
- The file must exist and be readable

**Returns:**
- `process_exit_code::SUCCESS` (0) on success
- `process_exit_code::FAILURE` (1) if the write operation fails
- `process_exit_code::UNCAUGHT_EXCEPTION` (144) if an exception occurs

**Postconditions:**
- On success, document is stored in the database

**Throws:**

| Exception | Condition |
|-----------|-----------|
| (caught internally) | All exceptions are caught and reported to stderr |

**Thread Safety:** Thread-safe; called from multiple worker threads concurrently

**Side Effects:**
- Reads file from disk
- Creates or updates a document in the database
- Logs errors to stderr on failure

**Example:**
```cpp
auto options = Options{"Worker"};
options.collection("RunHistory");
options.entity("BoardReader1");
options.format(data_format_t::fhicl);
options.operation(apiliteral::operation::writedocument);

int result = write_document(options, "/path/to/BoardReader1.fcl");
if (result != process_exit_code::SUCCESS) {
  std::cerr << "Failed to load file\n";
}
```

### `configuration_composition(options, confcomp) -> int`

**Brief:** Retrieves the configuration composition from the database for verification in debug mode. This queries the database to get a list of all documents that make up the configuration.

**Parameters:**
- `options` - The `ManageDocumentOperation` options object with configuration details
- `confcomp` - Output string to receive the composition JSON

**Preconditions:**
- `options` must have valid configuration and format settings
- Database connection must be available

**Returns:**
- `process_exit_code::SUCCESS` (0) on success
- `process_exit_code::FAILURE` (1) if retrieval fails
- `process_exit_code::UNCAUGHT_EXCEPTION` (144) if an exception occurs

**Postconditions:**
- On success, `confcomp` contains the JSON composition data

**Throws:**

| Exception | Condition |
|-----------|-----------|
| (caught internally) | All exceptions are caught and reported to stderr |

**Thread Safety:** Thread-safe

**Side Effects:**
- Prints composition JSON to stdout in debug mode
- Logs errors to stderr on failure

## Usage Examples

### Basic Usage

```bash
# Set database URI
export ARTDAQ_DATABASE_URI=filesystemdb:///path/to/db

# Load all FHiCL files from a directory
bulkloader -p /path/to/configs -c demo_safemode -r 1001
```

Output:
```
Loaded 150 files with 8 threads in 2500 msecs.
Avarage file load time is 133 msecs.
```

### With Custom Thread Count

```bash
# Load with 4 worker threads
bulkloader -p /path/to/configs -c demo_safemode -r 1001 -t 4
```

### With Debug Verification

```bash
# Load and verify by reading configuration composition back
bulkloader -p /path/to/configs -c demo_safemode -r 1001 -d true
```

This mode queries the database after loading to retrieve the configuration composition, verifying that documents were stored correctly.

## Implementation Details

### Worker Thread Algorithm

Each worker thread executes the following loop:

```cpp
workers.emplace_back([&file_names, &file_names_mutex, &loaded_file_count, &total_error_count, stropts]() {
  auto options = Options{"Worker"};
  options.readJsonData(stropts);  // Copy shared options

  while (true) {
    {
      std::lock_guard<std::mutex> lock(file_names_mutex);
      if (file_names.empty()) {
        // Report thread-local counts to global atomics
        loaded_file_count.fetch_add(filecount);
        total_error_count.fetch_add(errorcount);
        return;
      }
      file_name = file_names.back();
      file_names.pop_back();
    }

    // Determine collection and entity from filename
    if (file_name.rfind(".fcl") != std::string::npos) {
      if (file_name.rfind("schema.fcl") == std::string::npos) {
        options.collection("RunHistory");
        options.entity(stem_of_filename);
      } else {
        options.collection("SystemLayout");
        options.entity("schema");
      }
      write_document(options, file_name);
    }
  }
});
```

## Performance Statistics

The tool outputs performance statistics upon completion:

```
Loaded 150 files with 8 threads in 2500 msecs.
Avarage file load time is 133 msecs.
```

The "average file load time" accounts for parallelization, representing the effective per-file time as if processed serially.

## Exit Codes

| Code | Constant | Meaning |
|------|----------|---------|
| 0 | `process_exit_code::SUCCESS` | All files loaded successfully |
| 1 | `process_exit_code::HELP` | Help message displayed |
| 1 | `process_exit_code::FAILURE` | One or more files failed to load |
| 128 | `process_exit_code::INVALID_ARGUMENT` | Missing required arguments |
| 129 | `process_exit_code::INVALID_ARGUMENT \| 1` | Missing `--path` argument |
| 130 | `process_exit_code::INVALID_ARGUMENT \| 2` | Missing `--configuration` argument |
| 131 | `process_exit_code::INVALID_ARGUMENT \| 3` | Missing `--run` argument |
| 144 | `process_exit_code::UNCAUGHT_EXCEPTION` | Unhandled exception occurred |

## Error Handling

- Individual file failures do not stop the loading process
- Failed files are counted in `total_error_count` and logged to stderr
- If any files fail, the tool returns `FAILURE` exit code after all threads complete
- Each file failure logs both the filename and error message

```cpp
if (result != process_exit_code::SUCCESS) {
  std::cerr << "Failed loading file:" << file_name << "\n";
  std::cerr << "Error message:" << result.second << "\n";
  errorcount++;
}
```

## Environment Variables

| Variable | Purpose |
|----------|---------|
| `ARTDAQ_DATABASE_URI` | Database connection URI (required) |

## Common Pitfalls

- **Empty Directory:** The tool will succeed but report 0 files loaded
- **Thread Count:** Too many threads may cause database contention; optimal is typically 2-8 threads depending on storage provider
- **Missing Schema:** Ensure `schema.fcl` is present in the directory for proper `SystemLayout` collection
- **Disk I/O:** Performance depends heavily on disk speed; SSDs recommended for FileSystemDB
- **File Extensions:** Only `.fcl` files are processed; other files are silently ignored

### Anti-patterns

```bash
# DON'T: Use more threads than files
bulkloader -p /small/dir -c config -r 1 -t 100  # Wasteful

# DO: Match thread count to workload
bulkloader -p /small/dir -c config -r 1 -t 4
```

## Relationship to Other Components

- **bulkdownloader:** Complementary tool for exporting configurations from the database
- **conftool.py:** Python wrapper that can invoke bulkloader for archive operations
- **ConfigurationDB:** Uses the underlying `opts::write_document` function for storage
- **FileSystemDB/MongoDB:** Works with any configured storage provider

## See Also

- [bulkdownloader.cc.md](./bulkdownloader.cc.md) - Bulk export operations
- [conftool_py.md](./conftool_py.md) - Python wrapper with archive operations
- [schema_fcl.md](./schema_fcl.md) - Schema definition for collection assignment
- [dboperation_managedocument.h](../ConfigurationDB/dboperation_managedocument.h.md) - Document operations
