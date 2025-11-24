# rebuild_database_index.cc

## Overview

`rebuild_database_index` is a utility for rebuilding search indexes in a filesystem-based artdaq-database. It creates a new database instance with fresh indexes, ensuring index consistency and resolving corruption issues.

## Purpose and Use Cases

- Rebuilding corrupted or inconsistent search indexes
- Creating indexes for databases that lack them
- Migrating to newer index formats
- Resolving index-related query problems
- Database maintenance and optimization

## Location

**File**: `/home/user/artdaq-database/artdaq-database/Utilities/rebuild_database_index.cc`

## Command-Line Arguments

### Required Arguments

| Argument | Short | Type | Description |
|----------|-------|------|-------------|
| `--uri` | `-u` | string | Database URI (must be filesystemdb://) |

### Optional Arguments

| Argument | Short | Type | Description |
|----------|-------|------|-------------|
| `--help` | `-h` | - | Display help message |

## Main Workflow/Algorithm

```
1. Parse command-line arguments
2. Validate URI is filesystem database
3. Remove trailing slash from URI
4. Extract database path from URI
5. Expand environment variables in path
6. Create new database with "_new" suffix
7. Configure to auto-rebuild search index
8. For each collection in source database:
   a. Skip system_metadata collection
   b. For each document (JSON file) in collection:
      i.   Read source document
      ii.  Parse as JSONDocument
      iii. Build document with JSONDocumentBuilder
      iv.  Construct write query with filter and collection
      v.   Write to new database (triggers index update)
9. Report success/failure for each document
```

## Key Functions

### main()
Primary entry point that orchestrates index rebuilding.

**Steps**:
1. Validate command-line arguments
2. Verify filesystem database URI
3. Extract and expand database path
4. Create new database instance with "_new" suffix
5. Enable automatic search index rebuilding
6. Enumerate collections
7. Process each document in each collection
8. Report results

### Document Processing Pipeline

```cpp
// Read source
std::string source;
db::read_buffer_from_file(source, filepath);

// Parse and rebuild
JSONDocument doc{source};
JSONDocumentBuilder builder{{source}};

// Build write query
{
  "document": builder,
  "filter": {"_id": oid},
  "collection": collection_name
}

// Write to new database (index auto-updates)
provider->writeDocument(query_json);
```

## Index Rebuilding

### Automatic Rebuilding Flag

The key mechanism is enabling auto-rebuild:
```cpp
DBI::index::shouldAutoRebuildSearchIndex(true);
```

This ensures every document write triggers index updates.

### Index Structure

The search index typically stored in:
```
/database/collection/index.json
```

Old index files are explicitly removed:
```cpp
boost::filesystem::remove_all(collection_path + "/index.json")
```

### Index Creation

For each document written:
1. Database provider extracts searchable fields
2. Adds/updates index entries
3. Persists index to `index.json`

## Database Format

### URI Format
```
filesystemdb:///path/to/database
```

Must start with `filesystemdb://` prefix.

### Source Database Structure
```
/path/to/database/
├── system_metadata/     # Skipped
├── RunHistory/
│   ├── doc1.json
│   ├── doc2.json
│   └── index.json       # Old index, removed
└── SystemLayout/
    ├── schema.json
    └── index.json       # Old index, removed
```

### Target Database Structure
```
/path/to/database_new/
├── RunHistory/
│   ├── doc1.json
│   ├── doc2.json
│   └── index.json       # New index, rebuilt
└── SystemLayout/
    ├── schema.json
    └── index.json       # New index, rebuilt
```

## Error Handling

### Document-Level Errors
- Each document rebuild wrapped in try-catch
- Failures logged but don't stop overall process
- Failed documents marked "→ failed"
- Successful documents marked "→ succeeded"

### Exit Codes
- `SUCCESS` (0): All documents processed successfully
- `INVALID_ARGUMENT`: Missing URI argument
- `HELP`: Help message displayed
- `INVALID_ARGUMENT | 1`: Non-filesystem database URI
- `UNCAUGHT_EXCEPTION`: Unhandled exception

### Error Messages
- Read failures: "Unable to read a file {path}"
- Import failures: "Failed to import a document: {diagnostic}"

## Usage Examples

### Basic Index Rebuild
```bash
rebuild_database_index -u filesystemdb:///var/lib/artdaq/database
```

### With Environment Variables
```bash
export DB_ROOT=/data/databases
rebuild_database_index -u filesystemdb://${DB_ROOT}/production
```

### Complete Example
```bash
# Source: /opt/artdaq/db
# Target: /opt/artdaq/db_new (with fresh indexes)
rebuild_database_index --uri filesystemdb:///opt/artdaq/db
```

## Common Scenarios

### Fixing Corrupted Indexes
When queries return incorrect results:
```bash
# Backup original
cp -r /var/lib/artdaq/database /var/lib/artdaq/database.backup

# Rebuild indexes
rebuild_database_index -u filesystemdb:///var/lib/artdaq/database

# Verify new database
ls -la /var/lib/artdaq/database_new/*/index.json

# If successful, replace
mv /var/lib/artdaq/database /var/lib/artdaq/database.old
mv /var/lib/artdaq/database_new /var/lib/artdaq/database
```

### Creating Missing Indexes
For databases lacking index.json:
```bash
rebuild_database_index -u filesystemdb:///legacy_database
# Creates new database with proper indexes
```

### After Database Corruption
When index files are damaged:
```bash
# Remove corrupted indexes
find /var/lib/artdaq/database -name "index.json" -delete

# Rebuild from documents
rebuild_database_index -u filesystemdb:///var/lib/artdaq/database
```

### Batch Processing
Rebuild indexes for multiple databases:
```bash
#!/bin/bash
for db in /data/databases/*; do
  echo "Rebuilding indexes for $db..."
  rebuild_database_index -u filesystemdb://$db
done
```

## Implementation Details

### Single-Threaded
Unlike bulkloader/bulkdownloader, this is single-threaded:
- Sequential processing ensures index consistency
- Simpler error handling
- Index operations may not benefit from parallelism

### Document Preservation
Uses `JSONDocumentBuilder` with source:
```cpp
JSONDocumentBuilder builder{{source}};
```
Preserves original document content exactly.

### Collection Filtering
Skipped items:
- `system_metadata` collection (internal metadata)
- `index.json` files (rebuilt, not copied)
- Subdirectories within collections

### Path Handling
- Removes trailing slash from URI
- Expands environment variables (`$VAR`, `${VAR}`)
- Uses boost::filesystem for cross-platform compatibility

### Auto-Rebuild Flag
Global setting affects all database operations:
```cpp
DBI::index::shouldAutoRebuildSearchIndex(true);
```
Every `writeDocument()` call triggers index update.

## Dependencies

- `artdaq-database/BasicTypes/basictypes.h`
- `artdaq-database/ConfigurationDB/dboperation_metadata.h`
- `artdaq-database/JsonDocument/JSONDocument.h`
- `artdaq-database/JsonDocument/JSONDocumentBuilder.h`
- `artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h`
- `artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h`
- Boost.Filesystem for directory operations
- Boost.Program_options for argument parsing

## Comparison with migrate_database

| Feature | migrate_database | rebuild_database_index |
|---------|------------------|------------------------|
| Purpose | Format migration | Index rebuilding |
| Document Changes | Yes (migrations applied) | No (exact copy) |
| Index Handling | Copies as-is | Rebuilds from scratch |
| Use When | Schema changes | Index corruption |
| Output Suffix | `_migrated` | `_new` |

## Performance Considerations

### Speed Factors
- Database size (document count and size)
- Index complexity (searchable field count)
- Disk I/O speed
- Document parsing complexity

### Typical Performance
- **Small DB** (100 docs): < 1 minute
- **Medium DB** (10,000 docs): 5-30 minutes
- **Large DB** (100,000+ docs): 1+ hours

### Bottlenecks
1. Document read operations
2. JSON parsing
3. Index creation and persistence
4. Disk I/O (especially index.json writes)

### Optimization Not Applied
Could be parallelized but isn't:
- Multiple threads could process different collections
- Careful locking needed for index updates
- Current design prioritizes safety over speed

## Safety Considerations

### Non-Destructive
- Creates new database with "_new" suffix
- Original database unchanged
- Index.json files explicitly removed in new DB (clean rebuild)

### Verification Steps
After rebuild:
1. Compare document counts:
   ```bash
   find old_db -name "*.json" ! -name "index.json" | wc -l
   find old_db_new -name "*.json" ! -name "index.json" | wc -l
   ```

2. Verify index files exist:
   ```bash
   ls -la new_db/*/index.json
   ```

3. Test queries:
   ```bash
   # Query the new database and verify results
   ```

4. Keep original until verified:
   ```bash
   # Only after successful testing:
   rm -rf old_db
   mv old_db_new old_db
   ```

### Disk Space Requirements
Needs space for:
- Original database
- New database (same size)
- Plus overhead for indexes (~1-10% of data size)

Estimate: **2.1× original database size**

## Troubleshooting

### Common Issues

**Missing index.json after rebuild**:
```
Cause: Write failures, check permissions
Solution: Verify write permissions on target directory
```

**Partial rebuild**:
```
Cause: Document read/parse failures
Solution: Check failed document messages, fix source documents
```

**Out of disk space**:
```
Cause: Insufficient space for duplicate database
Solution: Free up space or use different target directory
```

**Still seeing query problems**:
```
Cause: Issue not index-related
Solution: Check document content, query syntax, database configuration
```

## Related Utilities

- **migrate_database**: Migrates document schemas
- **bulkloader**: Loads documents (creates indexes)
- **bulkdownloader**: Exports documents (ignores indexes)

## Advanced Usage

### Custom Target Location
Modify code to specify different target:
```cpp
// Instead of: database_uri + "_new"
auto config = DBI::DBConfig{custom_target_uri};
```

### Selective Rebuilding
Modify to rebuild only certain collections:
```cpp
for (auto const& collection_name : collection_names) {
  if (collection_name != "RunHistory") continue;  // Only RunHistory
  // ... rebuild logic
}
```

### Progress Reporting
Add progress indicators:
```cpp
std::cout << "Processed " << ++count << " / " << total << " documents\r";
```
