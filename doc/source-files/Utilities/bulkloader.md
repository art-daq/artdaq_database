# bulkloader.cc

## Overview

`bulkloader` is a high-performance utility for bulk-loading FHiCL configuration files into the artdaq-database. It uses multi-threading to efficiently process large numbers of configuration files in parallel.

## Purpose and Use Cases

- Bulk importing FHiCL configuration files into the database
- Initial database population from file system
- Migrating configurations from file-based storage to database
- High-performance batch loading operations

## Location

**File**: `/home/user/artdaq-database/artdaq-database/Utilities/bulkloader.cc`

## Command-Line Arguments

### Required Arguments

| Argument | Short | Type | Description |
|----------|-------|------|-------------|
| `--path` | `-p` | string | Path to directory containing FHiCL files |
| `--configuration` | `-c` | string | Configuration name |
| `--run` | `-r` | size_t | Run number |

### Optional Arguments

| Argument | Short | Type | Description | Default |
|----------|-------|------|-------------|---------|
| `--threads` | `-t` | size_t | Number of worker threads | Hardware concurrency |
| `--debug` | `-d` | bool | Read configuration back for verification | false |
| `--help` | `-h` | - | Display help message | - |

## Main Workflow/Algorithm

```
1. Parse command-line arguments
2. Initialize ManageDocumentOperation with format=fhicl, operation=writedocument
3. Set configuration as "run/configuration_name"
4. Enumerate all files in the specified directory
5. Spawn worker threads (default: hardware_concurrency)
6. Each worker thread:
   a. Acquires lock and gets next file from queue
   b. Determines collection and entity from filename
      - schema.fcl → SystemLayout collection, entity=schema
      - *.fcl → RunHistory collection, entity=filename stem
   c. Calls write_document to store in database
   d. Tracks success/error counts
7. Wait for all workers to complete
8. Calculate and display performance statistics
9. Optionally verify by reading configuration composition (debug mode)
10. Report any errors
```

## Key Functions

### main()
Primary entry point that orchestrates the bulk loading process.

**Steps**:
1. Parse command-line options using Boost.Program_options
2. Determine thread count (hardware concurrency or user-specified)
3. Configure operation options
4. List all files in target directory
5. Launch worker threads with shared file queue
6. Collect statistics and report performance

### write_document()
```cpp
int write_document(Options const& options, std::string const& file_name)
```

Writes a single document to the database:
1. Reads file content into buffer
2. Calls `configuration::opts::write_document`
3. Returns success/failure status
4. Logs errors to stderr

### configuration_composition()
```cpp
int configuration_composition(Options const& options, std::string& confcomp)
```

Retrieves and displays configuration composition for verification:
1. Calls `configuration::opts::configuration_composition`
2. Outputs composition JSON
3. Used in debug mode to verify loaded configuration

## Thread Safety

The utility implements thread-safe bulk loading:
- **Mutex-protected file queue**: Ensures no duplicate processing
- **Atomic counters**: Thread-safe tracking of loaded files and errors
- **Per-thread Options**: Each worker maintains its own Options object
- **Shared options string**: Serialized options passed to workers

## Performance Characteristics

### Parallel Processing
- Creates one worker thread per CPU core by default
- Threads operate independently on separate files
- No thread contention except for file queue access

### Statistics Reported
- Total files loaded
- Thread count used
- Total elapsed time (milliseconds)
- Average time per file (milliseconds)

### Performance Formula
```
Average per-file time = (Total time × min(CPU cores, thread count)) / File count
```

## Usage Examples

### Basic Bulk Load
```bash
bulkloader -p /path/to/configs -c MyConfig -r 1001
```

### With Specific Thread Count
```bash
bulkloader -p /path/to/configs -c MyConfig -r 1001 -t 8
```

### With Debug Verification
```bash
bulkloader -p /path/to/configs -c MyConfig -r 1001 -d true
```

### Complete Example
```bash
bulkloader \
  --path /data/daq/run1001 \
  --configuration MyDAQConfig \
  --run 1001 \
  --threads 16 \
  --debug true
```

## File Processing Rules

### Collection Determination
- **schema.fcl** → `SystemLayout` collection, entity = `schema`
- **other.fcl** → `RunHistory` collection, entity = file stem (without extension)

### Version and Run
- Both version and configuration are set to "run/configuration"
- Run number is set explicitly from command line

## Error Handling

### File-Level Errors
- Individual file failures are logged but don't stop processing
- Error count incremented atomically
- All errors reported at completion

### Exit Codes
- `SUCCESS` (0): All files loaded successfully
- `INVALID_ARGUMENT`: Missing or invalid command-line arguments
- `HELP`: Help message displayed
- `FAILURE`: One or more files failed to load
- `UNCAUGHT_EXCEPTION`: Unhandled exception occurred

## Common Scenarios

### Initial Database Population
Load all configuration files for a new run:
```bash
bulkloader -p /configs/run_1234 -c Production_v1 -r 1234
```

### Configuration Migration
Migrate existing file-based configs to database with verification:
```bash
bulkloader -p /legacy/configs -c Legacy_Import -r 1 -d true
```

### Performance Testing
Test loading speed with different thread counts:
```bash
for t in 1 2 4 8 16; do
  echo "Testing with $t threads:"
  bulkloader -p /configs -c TestConfig -r $t -t $t
done
```

## Implementation Notes

### Design Patterns
- **Producer-Consumer**: Main thread produces file list, workers consume
- **Thread Pool**: Fixed number of worker threads process variable work
- **Atomic Operations**: Lock-free counters for statistics

### Optimization Strategies
1. Pre-serialization of options for worker efficiency
2. File queue (vector) instead of more complex structures
3. Reserve capacity for output strings to reduce allocations
4. Minimal locking (only for file queue access)

## Dependencies

- `artdaq-database/ConfigurationDB/configuration_common.h`
- `artdaq-database/ConfigurationDB/dboperation_managedocument.h`
- `artdaq-database/BasicTypes/data_json.h`
- Boost.Filesystem for directory traversal
- Boost.Program_options for argument parsing
- C++ standard threading library

## Performance Considerations

### Bottlenecks
- Disk I/O for reading files
- Database write operations
- Lock contention on file queue (minimal)

### Scaling
- Linear speedup up to number of CPU cores
- Beyond CPU count, diminishing returns due to I/O limits
- Database performance becomes limiting factor with many threads

### Recommended Thread Counts
- **SSD storage**: Hardware concurrency or higher
- **HDD storage**: 2-4 threads to avoid disk thrashing
- **Network database**: Depends on network latency and bandwidth
