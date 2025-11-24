# bulkdownloader.cc

## Overview

`bulkdownloader` is a high-performance utility for bulk-exporting database configurations to FHiCL files. It is the complement to `bulkloader`, using multi-threading to efficiently retrieve and write configuration files from the database.

## Purpose and Use Cases

- Bulk exporting database configurations to file system
- Creating file-based backups of database configurations
- Migrating configurations from database to file-based storage
- Generating configuration snapshots for archival or analysis

## Location

**File**: `/home/user/artdaq-database/artdaq-database/Utilities/bulkdownloader.cc`

## Command-Line Arguments

### Required Arguments

| Argument | Short | Type | Description |
|----------|-------|------|-------------|
| `--path` | `-p` | string | Target path for exported FHiCL files |
| `--configuration` | `-c` | string | Configuration name |
| `--run` | `-r` | size_t | Run number |

### Optional Arguments

| Argument | Short | Type | Description | Default |
|----------|-------|------|-------------|---------|
| `--threads` | `-t` | size_t | Number of worker threads | Hardware concurrency |
| `--help` | `-h` | - | Display help message | - |

## Main Workflow/Algorithm

```
1. Parse command-line arguments
2. Initialize ManageDocumentOperation
3. Request configuration composition from database
   - Operation: confcomposition
   - Format: GUI (JSON)
4. Parse composition JSON to extract document queries
5. For each document in composition:
   a. Extract collection, entity, and filter information
   b. Serialize query to JSON
   c. Construct output filename:
      - SystemLayout documents → path/entity.fcl
      - RunHistory documents → path/config_prefix/entity.fcl
6. Spawn worker threads
7. Each worker thread:
   a. Acquire lock and get next query from queue
   b. Read document from database
   c. Convert to FHiCL format
   d. Write to file
   e. Track success/error counts
8. Wait for all workers to complete
9. Display performance statistics
```

## Key Functions

### main()
Primary entry point that orchestrates the bulk download process.

**Steps**:
1. Parse command-line arguments
2. Retrieve configuration composition
3. Parse JSON composition to extract document queries
4. Build query list with target filenames
5. Launch worker threads with shared query queue
6. Collect and report statistics

### read_document()
```cpp
int read_document(Options const& options, std::string const& file_name)
```

Reads a single document from database and writes to file:
1. Calls `configuration::opts::read_document`
2. Converts result to FHiCL format
3. Writes to specified file
4. Returns success/failure status

### configuration_composition()
```cpp
int configuration_composition(Options const& options, std::string& confcomp)
```

Retrieves configuration composition:
1. Calls `configuration::opts::configuration_composition`
2. Returns JSON composition string
3. Used to determine which documents to download

## Configuration Composition Processing

### JSON Structure
The composition JSON contains a `search` array with query objects:

```json
{
  "search": [
    {
      "query": {
        "collection": "RunHistory",
        "filter": {
          "entities": "MyEntity"
        }
      }
    }
  ]
}
```

### Query Extraction
For each query in the composition:
1. Extract `collection` field
2. Extract `entities` from filter
3. Serialize entire query for database operation
4. Construct appropriate output path

### File Naming Convention
- **SystemLayout documents**: `{path}/{entity}.fcl`
- **RunHistory documents**: `{path}/{config_prefix}/{entity}.fcl`

Where `config_prefix` is the configuration name without trailing digits.

## Thread Safety

The utility implements thread-safe bulk downloading:
- **Mutex-protected query queue**: Ensures no duplicate processing
- **Mutex-protected file list**: Thread-safe collection of processed files
- **Atomic error counter**: Lock-free error tracking
- **Per-thread Options**: Each worker maintains its own Options object

## Performance Characteristics

### Parallel Processing
- Creates one worker thread per CPU core by default
- Threads operate independently on separate queries
- Two mutex locks: one for queries, one for file list

### Statistics Reported
- Total files downloaded
- Thread count used
- Total elapsed time (milliseconds)
- Average time per file (milliseconds)

### Performance Formula
```
Average per-file time = (Total time × min(CPU cores, thread count)) / File count
```

## Usage Examples

### Basic Bulk Download
```bash
bulkdownloader -p /path/to/export -c MyConfig -r 1001
```

### With Specific Thread Count
```bash
bulkdownloader -p /export/configs -c MyConfig -r 1001 -t 8
```

### Complete Example
```bash
bulkdownloader \
  --path /backup/run1001 \
  --configuration Production_v1 \
  --run 1001 \
  --threads 16
```

## Directory Structure

The downloader automatically creates subdirectories:

```
/path/to/export/
├── schema.fcl              # SystemLayout documents
├── config_prefix/          # RunHistory documents directory
│   ├── BoardReader01.fcl
│   ├── EventBuilder01.fcl
│   └── DataLogger01.fcl
```

## Error Handling

### File-Level Errors
- Individual document read failures logged but don't stop processing
- Error count reported at completion
- Failed files not included in success count

### Exit Codes
- `SUCCESS` (0): All files downloaded successfully
- `INVALID_ARGUMENT`: Missing or invalid command-line arguments
- `HELP`: Help message displayed
- `FAILURE`: Configuration composition retrieval failed
- `UNCAUGHT_EXCEPTION`: Unhandled exception occurred

## Common Scenarios

### Configuration Backup
Create file-based backup of database configuration:
```bash
bulkdownloader -p /backup/$(date +%Y%m%d) -c Production -r 1234
```

### Configuration Export for Analysis
Export configuration for offline analysis:
```bash
bulkdownloader -p /analysis/configs -c TestRun_v3 -r 5678
```

### Reverse Migration
Move configuration back to file-based system:
```bash
bulkdownloader -p /configs/legacy -c LegacyConfig -r 1
```

### Parallel Export Batches
Export multiple configurations in parallel:
```bash
for config in Config1 Config2 Config3; do
  bulkdownloader -p /export/$config -c $config -r 1001 &
done
wait
```

## Implementation Notes

### Design Patterns
- **Producer-Consumer**: Main thread produces query list, workers consume
- **Thread Pool**: Fixed number of worker threads process variable work
- **Two-Stage Pipeline**:
  1. Composition retrieval and query generation (single-threaded)
  2. Document retrieval and file writing (multi-threaded)

### JSON Processing
Uses custom JSON reader/writer from `artdaq-database/DataFormats/Json`:
- `JsonReader` for parsing composition
- `JsonWriter` for serializing queries
- Object model: `json::object_t`, `json::array_t`

### Path Construction
- Uses `/` separator (Unix-style paths)
- Extracts configuration prefix by removing trailing digits
- Creates separate directories for different collections

## Dependencies

- `artdaq-database/ConfigurationDB/configuration_common.h`
- `artdaq-database/ConfigurationDB/dboperation_managedocument.h`
- `artdaq-database/DataFormats/Json/json_common.h`
- `artdaq-database/DataFormats/Json/json_types_impl.h`
- `artdaq-database/BasicTypes/data_json.h`
- Boost.Filesystem for path manipulation
- Boost.Program_options for argument parsing
- C++ standard threading library

## Comparison with bulkloader

| Feature | bulkloader | bulkdownloader |
|---------|------------|----------------|
| Direction | Files → Database | Database → Files |
| Input | Directory of files | Configuration composition |
| Discovery | Filesystem listing | Database query |
| Format | FHiCL only | FHiCL output |
| Verification | Optional debug mode | N/A |
| Complexity | Simpler | More complex (composition parsing) |

## Performance Considerations

### Bottlenecks
- Database read operations
- Disk I/O for writing files
- Network latency (remote databases)
- Lock contention on query queue (minimal)

### Scaling
- Linear speedup up to number of CPU cores
- Beyond CPU count, diminishing returns
- Network databases benefit from more threads (latency hiding)

### Recommended Thread Counts
- **Local database**: Hardware concurrency
- **Network database**: 2× hardware concurrency (hide latency)
- **Slow network**: Even higher thread counts may help

### Optimization Notes
1. Composition retrieval is serial (one-time cost)
2. Query queue uses simple vector (efficient pop_back)
3. File list collection uses local storage + final merge (reduces lock contention)
4. Format conversion (database → FHiCL) may be CPU-intensive

## Typo Note

Line 193 contains a typo in the error message:
```cpp
std::cerr << "Error: Failed to export " << total_error_count << "file(s).\n";
```
Should include a space before "file(s)".
