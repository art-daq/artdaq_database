# rebuild_database_index.cc

**Path:** `artdaq-database/Utilities/rebuild_database_index.cc`

**Purpose:** Command-line utility that rebuilds search indexes for FileSystemDB databases. This tool reads all documents from an existing database, deletes the old index files, and recreates them by writing documents to a new database location with auto-rebuild enabled. Use this when indexes become corrupted or out of sync with document contents.

## Key Concepts

### FileSystemDB Search Indexes

FileSystemDB maintains an `index.json` file in each collection directory that enables fast document searches without scanning every file. These indexes contain extracted metadata from documents and must stay synchronized with actual document content.

### Non-Destructive Rebuild

The tool creates a new database at `{original_uri}_new` rather than modifying the original in place. This provides a safety net if something goes wrong during the rebuild process.

### Auto-Rebuild Mode

By enabling `shouldAutoRebuildSearchIndex(true)`, the FileSystemDB provider automatically updates indexes when documents are written. The tool leverages this to rebuild indexes by reading and re-writing each document.

## Thread Safety

- **Thread Safety:** Not thread-safe (single-threaded operation)
- **Concurrent Access:** Do not run while database is actively being accessed by other processes
- **Locking:** No internal locking; assumes exclusive database access

## Dependencies

| Include | Purpose |
|---------|---------|
| `test/common.h` | Common test utilities and helper macros |
| `artdaq-database/BasicTypes/basictypes.h` | Basic type definitions including JsonData |
| `artdaq-database/ConfigurationDB/dboperation_metadata.h` | Database metadata operations and system_metadata constant |
| `artdaq-database/JsonDocument/JSONDocument.h` | Core JSON document class |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | Builder pattern for constructing documents |
| `artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h` | FileSystemDB storage provider |
| `artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h` | Index management and shouldAutoRebuildSearchIndex |
| `boost/program_options.hpp` | Command-line argument parsing |
| `boost/filesystem.hpp` | Directory iteration and file operations |

## Command-Line Interface

### Synopsis

```bash
rebuild_database_index -u <database-uri> [options]
```

### Options

| Option | Short | Required | Description |
|--------|-------|----------|-------------|
| `--uri` | `-u` | Yes | Source database URI (must use filesystemdb:// scheme) |
| `--help` | `-h` | No | Display help message and exit |

## Functions

### `main(argc, argv) -> int`

**Brief:** Entry point that parses command-line arguments, validates the database URI, and orchestrates the index rebuild process for all collections in the specified FileSystemDB database.

**Parameters:**
- `argc` - Number of command-line arguments
- `argv` - Array of command-line argument strings

**Preconditions:**
- The `--uri` argument must be provided
- URI must use the `filesystemdb://` scheme
- Database directory must exist and be readable
- Write permission required for creating new database directory

**Returns:** Process exit code indicating success or failure

**Postconditions:**
- On success, a new database exists at `{uri}_new` with rebuilt indexes
- Original database remains unchanged

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `boost::program_options::error` | Invalid command-line arguments |
| Any exception | Caught and logged, returns UNCAUGHT_EXCEPTION |

**Thread Safety:** Not thread-safe

**Side Effects:**
- Creates new database directory at `{uri}_new`
- Deletes existing `index.json` files in source collections
- Writes rebuilt documents to new database location

## Implementation Details

### Index Rebuild Algorithm

1. **Parse and Validate URI:** Extract database path from `filesystemdb://` URI
2. **Create Target Database:** Initialize new database at `{uri}_new`
3. **Enable Auto-Rebuild:** Set `shouldAutoRebuildSearchIndex(true)` to trigger index updates on writes
4. **Enumerate Collections:** Find all subdirectories (collections) in the database path
5. **Process Each Collection:**
   - Skip the `system_metadata` collection
   - Delete existing `index.json` file
   - Iterate through all document files
   - Read each document, construct write request, and write to new database
6. **Report Progress:** Log success/failure for each document

### Key Code Patterns

**Enabling Auto-Rebuild:**
```cpp
DBI::index::shouldAutoRebuildSearchIndex(true);
```

**Deleting Old Index:**
```cpp
if (boost::filesystem::remove_all(collection_path + "/index.json") == 0u) {
  std::cout << "   Missing index: " << collection_path + "/index.json";
}
```

**Document Write Request Format:**
```cpp
oss << "{" << quoted_(jsonliteral::document) << ":" << builder << ",";
oss << quoted_(jsonliteral::filter) << ":" << to_id(oid) << ",";
oss << quoted_(jsonliteral::collection) << ":" << quoted_(collection_name) << "}";
provider->writeDocument(oss.str());
```

## Usage Examples

### Rebuild Corrupted Indexes

```bash
# Rebuild indexes for a test database
rebuild_database_index -u filesystemdb:///data/config_db/test_db

# Output shows progress:
# Database:/data/config_db/test_db
#
#
#  Collection:BoardReaders
#    Document: boardreader01 -> succeeded
#    Document: boardreader02 -> succeeded
#
#
#  Collection:EventBuilders
#    Document: EventBuilder01 -> succeeded
```

### After Manual Document Edits

```bash
# If you manually edited JSON files in the database
rebuild_database_index -u filesystemdb://${ARTDAQ_DATABASE_HOME}/databases/mydb

# Then replace the original with the rebuilt version
mv /path/to/mydb /path/to/mydb_backup
mv /path/to/mydb_new /path/to/mydb
```

## Exit Codes

| Code | Constant | Meaning |
|------|----------|---------|
| 0 | `SUCCESS` | Index rebuild completed successfully |
| 1 | `HELP` | Help message was displayed |
| 2+ | `INVALID_ARGUMENT` | Invalid or missing command-line arguments |
| Variable | `UNCAUGHT_EXCEPTION` | Unhandled exception occurred during processing |

## Output Format

```
Database:/path/to/database


 Collection:CollectionName1
   Document: doc1 -> succeeded
   Document: doc2 -> succeeded
   Document: doc3 -> failed


 Collection:CollectionName2
   Missing index: /path/to/database/CollectionName2/index.json
   Document: doc4 -> succeeded
```

## Error Handling

Individual document failures are caught and logged but do not stop the rebuild process:

```cpp
try {
  // Read and write document
  provider->writeDocument(oss.str());
  std::cout << " -> succeeded\n";
} catch (...) {
  std::cerr << "Failed to import a document: "
            << ::debug::current_exception_diagnostic_information() << "\n";
}
```

This allows partial recovery when some documents are corrupted while others remain valid.

## Common Pitfalls

- **Wrong URI Scheme:** Only `filesystemdb://` URIs are supported; MongoDB URIs will be rejected
- **Trailing Slash:** The tool automatically removes trailing slashes from the URI path
- **Disk Space:** Requires sufficient disk space to create a complete copy of the database
- **In-Use Database:** Running while other processes access the database may cause inconsistencies
- **Permissions:** Requires write permission to create the `_new` database directory

## Post-Rebuild Verification

After rebuilding indexes:

1. **Compare Document Counts:** Verify the same number of documents exist
   ```bash
   find /path/to/database -name "*.json" ! -name "index.json" | wc -l
   find /path/to/database_new -name "*.json" ! -name "index.json" | wc -l
   ```

2. **Test Queries:** Run searches against the new database to verify index correctness

3. **Replace Original:**
   ```bash
   mv /path/to/database /path/to/database_backup
   mv /path/to/database_new /path/to/database
   ```

## Relationship to Other Components

- **FileSystemDB Provider:** Uses the provider's writeDocument method to trigger index updates
- **provider_filedb_index.h:** Accesses the shouldAutoRebuildSearchIndex flag
- **migrate_database:** Similar utility for database format migrations
- **JSONDocumentBuilder:** Used to construct properly formatted documents for writing

## See Also

- [migrate_database.cc.md](./migrate_database.cc.md) - Database schema migration utility
- [provider_filedb_index.h.md](../StorageProviders/FileSystemDB/provider_filedb_index.h.md) - Index implementation details
- [provider_filedb.h.md](../StorageProviders/FileSystemDB/provider_filedb.h.md) - FileSystemDB storage provider
