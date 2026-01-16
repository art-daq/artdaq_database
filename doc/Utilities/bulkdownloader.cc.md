# bulkdownloader.cc

**Path:** `artdaq-database/Utilities/bulkdownloader.cc`

**Purpose:** High-performance multi-threaded bulk export utility for downloading configuration files from the database to the filesystem. This tool retrieves all documents associated with a configuration and writes them as FHiCL files using parallel workers.

## Overview

The `bulkdownloader` utility exports a complete configuration from the artdaq-database to disk as FHiCL files. It first retrieves the configuration composition (a list of all documents in the configuration), then uses multiple worker threads to download and write files in parallel. This is the counterpart to `bulkloader`.

## Key Concepts

### Configuration Composition

Before downloading, the tool queries the database for the "configuration composition" - a JSON document containing queries for all entities that make up the configuration. Each query specifies the exact version of each document to retrieve.

### Two-Phase Processing

1. **Phase 1:** Retrieve configuration composition and build query list
2. **Phase 2:** Parallel workers process queries and write files

### File Organization

Downloaded files are organized based on their collection:
- `SystemLayout` collection items (e.g., `schema.fcl`) are placed in the root output directory
- Other items go into a subdirectory named after the configuration prefix

## Thread Safety

- **Thread Safety:** Thread-safe for concurrent file downloading
- **Concurrent Access:** Multiple threads safely access the query queue via mutex
- **Locking:**
  - `std::mutex queries_mutex` protects the queries vector
  - `std::mutex file_names_mutex` protects the completed file names list
  - `std::atomic<int>` used for `total_error_count`

## Dependencies

| Include | Purpose |
|---------|---------|
| `test/common.h` | Common test utilities and process exit codes |
| `artdaq-database/BasicTypes/data_json.h` | JSON data type support |
| `artdaq-database/DataFormats/Json/json_common.h` | JSON parsing utilities |
| `artdaq-database/DataFormats/Json/json_types_impl.h` | JSON type implementations for `unwrap` |
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
| `--path` | `-p` | Yes | Path to output directory for FHiCL files |
| `--configuration` | `-c` | Yes | Configuration name to download |
| `--run` | `-r` | Yes | Run number |
| `--threads` | `-t` | No | Number of worker threads (default: hardware concurrency) |
| `--help` | `-h` | No | Display help message |

## Functions

### `main(argc, argv) -> int`

**Brief:** Entry point that retrieves the configuration composition, builds a work queue of document queries, spawns worker threads to download files in parallel, and reports statistics upon completion.

**Parameters:**
- `argc` - Number of command-line arguments
- `argv` - Array of command-line argument strings

**Preconditions:**
- `ARTDAQ_DATABASE_URI` environment variable must be set
- The configuration must exist in the database
- Output directory must be writable

**Returns:**
- `process_exit_code::SUCCESS` (0) - All files downloaded successfully
- `process_exit_code::HELP` (1) - Help message displayed
- `process_exit_code::FAILURE` (1) - Configuration composition retrieval or JSON parsing failed
- `process_exit_code::INVALID_ARGUMENT` (128) - Missing required arguments
- `process_exit_code::UNCAUGHT_EXCEPTION` (144) - Unhandled exception occurred

**Postconditions:**
- All configuration files are written to the output directory
- Performance statistics are printed to stdout

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `bpo::error` | Command-line parsing error (caught and reported) |
| (caught internally) | All other exceptions caught at top level |

**Thread Safety:** Creates multiple threads internally; safe to call once per process

**Side Effects:**
- Creates worker threads
- Creates files and directories on disk
- Prints progress and statistics to stdout/stderr

### `read_document(options, file_name) -> int`

**Brief:** Reads a document from the database and writes it to a file in FHiCL format.

**Parameters:**
- `options` - The `ManageDocumentOperation` options object containing the query
- `file_name` - Path where the FHiCL file should be written

**Preconditions:**
- `file_name` must not be empty
- Parent directory must exist and be writable

**Returns:**
- `process_exit_code::SUCCESS` (0) on success
- `process_exit_code::FAILURE` (1) if read or write fails
- `process_exit_code::UNCAUGHT_EXCEPTION` (144) if an exception occurs

**Postconditions:**
- On success, file is written to disk with FHiCL content

**Throws:**

| Exception | Condition |
|-----------|-----------|
| (caught internally) | All exceptions are caught and reported to stderr |

**Thread Safety:** Thread-safe; called from multiple worker threads concurrently

**Side Effects:**
- Reads document from database
- Creates a file on disk
- Logs errors to stderr on failure

**Example:**
```cpp
auto options = Options{"Worker"};
options.readJsonData(query_json);
options.format(cfo::data_format_t::fhicl);

int result = read_document(options, "/output/BoardReader1.fcl");
if (result != process_exit_code::SUCCESS) {
  std::cerr << "Failed to download file\n";
}
```

### `configuration_composition(options, confcomp) -> int`

**Brief:** Retrieves the configuration composition JSON from the database, containing queries for all documents in the configuration.

**Parameters:**
- `options` - The `ManageDocumentOperation` options object specifying the configuration
- `confcomp` - Output string to receive the composition JSON

**Preconditions:**
- `options` must have valid configuration and format settings
- Database connection must be available

**Returns:**
- `process_exit_code::SUCCESS` (0) on success
- `process_exit_code::FAILURE` (1) if retrieval fails
- `process_exit_code::UNCAUGHT_EXCEPTION` (144) if an exception occurs

**Postconditions:**
- On success, `confcomp` contains the JSON composition data with all document queries

**Throws:**

| Exception | Condition |
|-----------|-----------|
| (caught internally) | All exceptions are caught and reported to stderr |

**Thread Safety:** Thread-safe

**Side Effects:**
- Reads from database
- Logs errors to stderr on failure

**Example:**
```cpp
auto options = Options{argv[0]};
options.operation(apiliteral::operation::confcomposition);
options.configuration("1001/demo_safemode00003");
options.format(cfo::data_format_t::gui);

std::string composition;
int result = configuration_composition(options, composition);
// composition now contains JSON with all document queries
```

## Usage Examples

### Basic Usage

```bash
# Set database URI
export ARTDAQ_DATABASE_URI=filesystemdb:///path/to/db

# Download configuration to a directory
bulkdownloader -p /path/to/output -c demo_safemode00003 -r 1001
```

Output:
```
Downloaded 150 files with 8 threads in 3200 msecs.
Avarage file load time is 170 msecs.
```

### With Custom Thread Count

```bash
# Download with 4 worker threads
bulkdownloader -p /path/to/output -c demo_safemode00003 -r 1001 -t 4
```

## Implementation Details

### Download Process Algorithm

```cpp
// Phase 1: Get configuration composition
auto options1 = Options{argv[0]};
options1.operation(apiliteral::operation::confcomposition);
options1.configuration(run + "/" + configuration);
options1.format(cfo::data_format_t::gui);

auto confcomp = std::string{};
configuration_composition(options1, confcomp);

// Parse composition to extract queries
auto ast = jsn::object_t{};
jsn::JsonReader().read(confcomp, ast);
auto const& results = unwrap(ast).value_as<const jsn::array_t>(jsonliteral::search);

// Build query queue with file paths
for (auto& result : results) {
  auto const& query = unwrap(result).value_as<const jsn::object_t>(jsonliteral::query);
  auto const& collection = unwrap(query).value_as<const std::string>(apiliteral::option::collection);
  auto const& entity = unwrap(filter).value_as<const std::string>(apiliteral::filter::entities);

  // Determine file path based on collection
  auto file_name = dir_path;
  if (collection != "SystemLayout") {
    file_name += confprefix + "/";
  }
  file_name += entity + ".fcl";

  queries.emplace_back(file_name, serialized_query);
}

// Phase 2: Parallel download
for (size_t i = 0; i < nproc; i++) {
  workers.emplace_back([&queries, &queries_mutex, ...]() {
    while (true) {
      // Pop query from queue (under lock)
      // Read document from database
      // Write to file
    }
  });
}
```

## Output Structure

For a configuration named `demo_safemode00003`:

```
/path/to/output/
  schema.fcl                    # From SystemLayout collection
  demo_safemode/                # Subdirectory for non-SystemLayout
    component01.fcl
    component02.fcl
    EventBuilder1.fcl
    DataLogger1.fcl
    Dispatcher1.fcl
    ...
```

The configuration prefix is extracted from the configuration name by removing trailing digits:
- `demo_safemode00003` -> prefix is `demo_safemode`

## Performance Statistics

The tool outputs performance statistics upon completion:

```
Downloaded 150 files with 8 threads in 3200 msecs.
Avarage file load time is 170 msecs.
```

## Exit Codes

| Code | Constant | Meaning |
|------|----------|---------|
| 0 | `process_exit_code::SUCCESS` | All files downloaded successfully |
| 1 | `process_exit_code::HELP` | Help message displayed |
| 1 | `process_exit_code::FAILURE` | Configuration composition retrieval or JSON parsing failed |
| 128 | `process_exit_code::INVALID_ARGUMENT` | Missing required arguments |
| 129 | `process_exit_code::INVALID_ARGUMENT \| 1` | Missing `--path` argument |
| 130 | `process_exit_code::INVALID_ARGUMENT \| 2` | Missing `--configuration` argument |
| 131 | `process_exit_code::INVALID_ARGUMENT \| 3` | Missing `--run` argument |
| 144 | `process_exit_code::UNCAUGHT_EXCEPTION` | Unhandled exception occurred |

## Error Handling

- Individual file download failures are counted but do not stop the process
- Failed files are reported in the error count at the end
- The tool continues downloading remaining files even if some fail
- Configuration composition failure stops the process immediately

```cpp
if (result != process_exit_code::SUCCESS) {
  std::cerr << "Failed writing file:" << file_name << "\n";
  std::cerr << "Error message:" << result.second << "\n";
  errorcount++;
}
```

## Environment Variables

| Variable | Purpose |
|----------|---------|
| `ARTDAQ_DATABASE_URI` | Database connection URI (required) |

## Common Pitfalls

- **Directory Permissions:** Ensure write permissions on the output directory and ability to create subdirectories
- **Existing Files:** The tool overwrites existing files without warning
- **Configuration Not Found:** The tool will fail if the configuration doesn't exist in the database
- **Network Latency:** Performance depends on database connectivity (especially for MongoDB)
- **Disk Space:** Ensure sufficient space for all configuration files

### Anti-patterns

```bash
# DON'T: Download to a read-only directory
bulkdownloader -p /read-only/path -c config -r 1  # Will fail

# DO: Ensure directory is writable
mkdir -p /writable/output
bulkdownloader -p /writable/output -c config -r 1
```

## Relationship to Other Components

- **bulkloader:** Complementary tool for importing configurations to the database
- **conftool.py:** Python wrapper that uses bulkdownloader for remote export operations
- **ConfigurationDB:** Uses the `opts::read_document` and `opts::configuration_composition` functions
- **JsonReader/JsonWriter:** Used for parsing configuration composition JSON

## See Also

- [bulkloader.cc.md](./bulkloader.cc.md) - Bulk import operations
- [conftool_py.md](./conftool_py.md) - Python wrapper with export operations
- [schema_fcl.md](./schema_fcl.md) - Schema definition for collection assignment
- [dboperation_managedocument.h](../ConfigurationDB/dboperation_managedocument.h.md) - Document operations
