# migrate_database.cc

## Overview

`migrate_database` is a database migration utility that copies documents from an old database format to a new format. It processes all collections and documents, applying migrations through the `JSONDocumentMigrator` and writes them to a new database location.

## Purpose and Use Cases

- Migrating databases to newer formats
- Upgrading document schemas
- Creating migrated copies of databases
- Data preservation during format changes

## Location

**File**: `/home/user/artdaq-database/artdaq-database/Utilities/migrate_database.cc`

## Command-Line Arguments

### Required Arguments

| Argument | Short | Type | Description |
|----------|-------|------|-------------|
| `--uri` | `-u` | string | Source database URI |

### Optional Arguments

| Argument | Short | Type | Description |
|----------|-------|------|-------------|
| `--help` | `-h` | - | Display help message |

## Main Workflow/Algorithm

```
1. Parse command-line arguments
2. Validate URI is a filesystem database (filesystemdb://)
3. Extract database path from URI
4. Create target database with "_migrated" suffix
5. For each collection in source database:
   a. Skip system_metadata collection
   b. For each document (JSON file) in collection:
      i.   Read source document
      ii.  Parse as JSONDocument
      iii. Apply migrations via JSONDocumentMigrator
      iv.  Build new document with JSONDocumentBuilder
      v.   Set collection metadata
      vi.  Write to target database
6. Report success/failure for each document
```

## Key Functions

### main()
Primary entry point that orchestrates the migration process.

**Steps**:
1. Validate command-line arguments
2. Verify filesystem database URI format
3. Remove trailing slash from URI
4. Extract database path and expand environment variables
5. Create new database with "_migrated" suffix
6. Enumerate collections (subdirectories)
7. For each collection, enumerate documents (JSON files)
8. Process each document through migration pipeline
9. Report results

### Document Processing Pipeline

```cpp
// Read source
std::string source;
db::read_buffer_from_file(source, filepath);

// Parse and migrate
JSONDocument doc{source};
JSONDocumentBuilder builder{JSONDocumentMigrator{doc}};

// Set metadata
builder.setCollection({"{\"collection\":\"" + collection_name + "\"}"});

// Build query
{
  "document": builder,
  "filter": {"_id": oid},
  "collection": collection_name
}

// Write to new database
provider->writeDocument(query_json);
```

## Database Format

### URI Format
```
filesystemdb:///path/to/database
```

### Source Database Structure
```
/path/to/database/
├── system_metadata/     # Skipped
├── RunHistory/
│   ├── doc1.json
│   ├── doc2.json
│   └── index.json       # Skipped
└── SystemLayout/
    ├── schema.json
    └── index.json
```

### Target Database Structure
```
/path/to/database_migrated/
├── RunHistory/
│   ├── doc1.json
│   └── doc2.json
└── SystemLayout/
    └── schema.json
```

## Document Migration

### Migration Classes

**JSONDocument**: Represents a JSON document
**JSONDocumentMigrator**: Applies schema migrations to documents
**JSONDocumentBuilder**: Constructs new documents with proper metadata

### Migration Process

1. **Parse**: Original document → `JSONDocument`
2. **Migrate**: `JSONDocument` → `JSONDocumentMigrator` (applies transformations)
3. **Build**: `JSONDocumentMigrator` → `JSONDocumentBuilder` (adds metadata)
4. **Set Collection**: Adds collection metadata to builder
5. **Serialize**: Builder → JSON string with filter and collection
6. **Write**: JSON string → new database

### Metadata Handling

The migration adds collection metadata:
```cpp
builder.setCollection({"{\"collection\":\"" + collection_name + "\"}"});
```

This likely embeds collection information into the document structure.

## Error Handling

### Document-Level Errors
- Each document migration is wrapped in try-catch
- Failures logged but don't stop overall migration
- Failed documents marked with "→ failed"
- Successful documents marked with "→ succeeded"

### Exit Codes
- `SUCCESS` (0): All documents migrated successfully
- `INVALID_ARGUMENT`: Missing or invalid URI
- `INVALID_ARGUMENT | 1`: Non-filesystem database URI
- `UNCAUGHT_EXCEPTION`: Unhandled exception occurred

### Error Messages
- File read failures: "Unable to read a file"
- Migration failures: "Failed to import a document" + diagnostic info

## Usage Examples

### Basic Migration
```bash
migrate_database -u filesystemdb:///var/lib/artdaq/database
```

### Migration with Environment Variables
```bash
export DB_ROOT=/data/databases
migrate_database -u filesystemdb://${DB_ROOT}/production
```

### Complete Example
```bash
# Source database: /opt/artdaq/db
# Target database: /opt/artdaq/db_migrated
migrate_database --uri filesystemdb:///opt/artdaq/db
```

## Common Scenarios

### Upgrading Database Format
After software update requiring new document format:
```bash
# Backup original
cp -r /var/lib/artdaq/database /var/lib/artdaq/database.backup

# Migrate
migrate_database -u filesystemdb:///var/lib/artdaq/database

# Verify migrated database
ls -la /var/lib/artdaq/database_migrated

# If successful, replace
mv /var/lib/artdaq/database /var/lib/artdaq/database.old
mv /var/lib/artdaq/database_migrated /var/lib/artdaq/database
```

### Testing Migration
Test migration without affecting production:
```bash
# Create test copy
cp -r /var/lib/artdaq/database /tmp/test_db

# Migrate test copy
migrate_database -u filesystemdb:///tmp/test_db

# Verify
diff -r /tmp/test_db /tmp/test_db_migrated
```

### Batch Migration
Migrate multiple databases:
```bash
#!/bin/bash
for db in db1 db2 db3; do
  echo "Migrating $db..."
  migrate_database -u filesystemdb:///data/$db
done
```

## Implementation Notes

### Design Patterns
- **Pipeline**: Read → Parse → Migrate → Build → Write
- **Visitor**: JSONDocumentMigrator applies transformations
- **Builder**: JSONDocumentBuilder constructs output
- **Provider**: DBProvider handles database abstraction

### Single-Threaded
Unlike bulkloader/bulkdownloader, this utility is single-threaded:
- Ensures migration order if needed
- Simpler error handling and reporting
- Migration operations may be complex enough to not benefit from parallelism

### File Filtering
Skipped items:
- `system_metadata` collection (special internal collection)
- `index.json` files (not document files)
- Subdirectories within collections

### Collection Enumeration
Uses `DBI::find_subdirs()` to enumerate collections, which returns top-level subdirectories of the database path.

### Document Identification
Document OID extracted from filename:
```cpp
auto oid = dir_iter->path().filename().replace_extension().string();
```
Removes `.json` extension to get document ID.

## Dependencies

- `artdaq-database/BasicTypes/basictypes.h`
- `artdaq-database/ConfigurationDB/dboperation_metadata.h`
- `artdaq-database/JsonDocument/JSONDocument.h`
- `artdaq-database/JsonDocument/JSONDocumentBuilder.h`
- `artdaq-database/JsonDocument/JSONDocumentMigrator.h`
- `artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h`
- Boost.Filesystem for directory iteration
- Boost.Program_options for argument parsing

## Limitations

### Database Support
- **Only supports filesystem databases** (filesystemdb:// URI)
- MongoDB and other providers not supported
- Validation explicitly checks for filesystem URI prefix

### Migration Transparency
- Actual migrations performed by `JSONDocumentMigrator` (black box here)
- No control over what migrations are applied
- Migration logic defined elsewhere in codebase

### No Rollback
- Migration creates new database, doesn't modify original
- Original database remains unchanged (safe)
- Manual intervention required to replace original

### No Progress Reporting
- No progress bar or percentage complete
- Only per-document success/failure messages
- Large databases may appear to hang without visible progress

## Safety Considerations

### Non-Destructive
- Creates new database with "_migrated" suffix
- Original database unchanged
- Safe to run without backup (though backup still recommended)

### Verification Recommended
After migration:
1. Verify document count matches
2. Spot-check migrated documents
3. Test database functionality
4. Keep original until verification complete

### Disk Space
- Requires disk space for complete database copy
- May need 2× original database size
- Plan accordingly for large databases

## Related Utilities

- **rebuild_database_index**: Rebuilds search indexes (may be needed after migration)
- **bulkloader**: Loads configurations into database
- **bulkdownloader**: Exports configurations from database

## Future Enhancements

Potential improvements:
- Multi-threaded processing for large databases
- Progress reporting (count/percentage)
- Support for other database providers
- Selective migration (specific collections)
- Dry-run mode (validation without writing)
- Migration verification step
