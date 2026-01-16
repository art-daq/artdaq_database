# migrate_database.cc

**Path:** `artdaq-database/Utilities/migrate_database.cc`

**Purpose:** Database migration utility for converting FileSystemDB documents to an updated schema format. This tool reads all documents from an existing database, applies `JSONDocumentMigrator` transformations, and writes them to a new database at a `_migrated` suffix location.

## Overview

The `migrate_database` utility performs schema migrations on FileSystemDB databases. It iterates through all collections and documents, applies the `JSONDocumentMigrator` transformation to update document structure, and stores the migrated documents in a new database. This is essential when the document schema changes between software versions.

## Key Concepts

### Non-Destructive Migration

The tool creates a new database rather than modifying the original:
- Source: `filesystemdb:///path/to/database`
- Target: `filesystemdb:///path/to/database_migrated`

This ensures the original data remains intact if migration fails or produces unexpected results.

### JSONDocumentMigrator

The migration uses the `JSONDocumentMigrator` class to transform documents. This class:
- Updates document schema versions
- Renames deprecated fields
- Restructures nested objects for compatibility
- Adds new required fields with default values

### Collection Preservation

All collections are processed except for `system_metadata`, which contains internal database information that should not be migrated (it is regenerated for the new database).

### Document Reconstruction

Each document is:
1. Read from the source database
2. Wrapped in a `JSONDocumentMigrator`
3. Rebuilt using `JSONDocumentBuilder`
4. Assigned its collection metadata
5. Written to the target database with its original ID

## Thread Safety

- **Thread Safety:** Not thread-safe (single-threaded operation)
- **Concurrent Access:** Do not run multiple migrations on the same database simultaneously
- **Locking:** No internal locking; relies on exclusive file system access

## Dependencies

| Include | Purpose |
|---------|---------|
| `test/common.h` | Common test utilities and process exit codes |
| `artdaq-database/BasicTypes/basictypes.h` | Basic type definitions |
| `artdaq-database/ConfigurationDB/dboperation_metadata.h` | `system_metadata` constant |
| `artdaq-database/JsonDocument/JSONDocument.h` | JSON document class |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | Document builder pattern |
| `artdaq-database/JsonDocument/JSONDocumentMigrator.h` | Schema migration logic |
| `artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h` | FileSystemDB provider |
| `boost/program_options.hpp` | Command-line parsing |
| `boost/filesystem.hpp` | Directory iteration |

## Command-Line Options

| Option | Short | Required | Description |
|--------|-------|----------|-------------|
| `--uri` | `-u` | Yes | Source database URI (must be `filesystemdb://` scheme) |
| `--help` | `-h` | No | Display help message |

## Functions

### `main(argc, argv) -> int`

**Brief:** Entry point that validates the database URI, iterates through all collections and documents in the source database, applies schema migration to each document, and writes the migrated documents to a new database with `_migrated` suffix.

**Parameters:**
- `argc` - Number of command-line arguments
- `argv` - Array of command-line argument strings

**Preconditions:**
- URI must use the `filesystemdb://` scheme
- Source database directory must exist and be readable
- Parent directory of target database must be writable

**Returns:**
- `process_exit_code::SUCCESS` (0) - Migration completed successfully
- `process_exit_code::HELP` (1) - Help message displayed
- `process_exit_code::INVALID_ARGUMENT` (128) - Missing `--uri` argument
- `process_exit_code::INVALID_ARGUMENT | 1` (129) - URI is not a FileSystemDB URI
- `process_exit_code::UNCAUGHT_EXCEPTION` (144) - Unhandled exception occurred

**Postconditions:**
- A new database exists at `{original_uri}_migrated`
- All documents from original database are migrated with updated schema
- Original database remains unchanged

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `bpo::error` | Command-line parsing error (caught and reported) |
| (caught per-document) | Document read/write failures are caught and logged |
| (caught at top level) | All other exceptions caught and reported |

**Thread Safety:** Not thread-safe; single-threaded execution

**Side Effects:**
- Creates a new database directory
- Creates migrated document files
- Prints progress to stdout
- Logs errors to stderr

**Example:**
```bash
# Migrate a FileSystemDB database
migrate_database -u filesystemdb:///data/configurations/v1
# Creates: filesystemdb:///data/configurations/v1_migrated
```

## Usage Examples

### Basic Migration

```bash
# Migrate a FileSystemDB database
migrate_database -u filesystemdb:///data/configurations/v1
# Creates: filesystemdb:///data/configurations/v1_migrated
```

### Using Environment Variables in Path

```bash
# Using environment variable expansion
export CONFIG_DB=/data/configurations/production
migrate_database -u filesystemdb://$CONFIG_DB
```

### Verifying Migration

```bash
# After migration, compare document counts
ls /data/configurations/v1/MyCollection/*.json | wc -l
ls /data/configurations/v1_migrated/MyCollection/*.json | wc -l
```

## Implementation Details

### Migration Algorithm

```cpp
int main(int argc, char* argv[]) {
  // 1. Parse and validate URI
  auto database_uri = vm["uri"].as<std::string>();
  if (database_uri.find(DBI::literal::FILEURI) != 0) {
    std::cerr << "Only the filesystem database provider is supported.";
    return process_exit_code::INVALID_ARGUMENT | 1;
  }

  // 2. Remove trailing slash and expand environment variables
  auto database_path = database_uri.substr(strlen(DBI::literal::FILEURI));
  database_path = db::expand_environment_variables(database_path);

  // 3. Create new database at _migrated location
  auto config = DBI::DBConfig{database_uri + "_migrated"};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JSONDocument>::create(database);

  // 4. Iterate collections
  auto collection_names = DBI::find_subdirs(database_path);
  for (auto const& collection_name : collection_names) {
    if (collection_name == system_metadata) continue;  // Skip internal

    // 5. Iterate documents in collection
    for (auto dir_iter = ...; dir_iter != end; ++dir_iter) {
      if (is_directory || filename == "index.json") continue;

      // 6. Migrate each document
      auto source = read_file(dir_iter->path());
      auto doc = JSONDocument{source};
      JSONDocumentBuilder builder{JSONDocumentMigrator{doc}};
      builder.setCollection(...);

      // 7. Write to new database
      provider->writeDocument(serialized_document);
    }
  }
}
```

### Document Processing Details

```cpp
auto doc = JSONDocument{source};
JSONDocumentBuilder builder{JSONDocumentMigrator{doc}};
builder.setCollection({"{db::quoted_(apiliteral::option::collection):\""s + collection_name + "\"}"});

std::ostringstream oss;
oss << "{" << quoted_(jsonliteral::document) << ":" << builder << ",";
oss << quoted_(jsonliteral::filter) << ":" << to_id(oid) << ",";
oss << quoted_(jsonliteral::collection) << ":" << quoted_(collection_name) << "}";

provider->writeDocument(oss.str());
```

### File System Layout

```
Source: /path/to/database/
  Collection1/
    doc1.json
    doc2.json
    index.json          <- Skipped (index file)
  Collection2/
    doc3.json
  system_metadata/      <- Skipped entirely
    internal_data.json

Target: /path/to/database_migrated/
  Collection1/
    doc1.json           <- Migrated schema
    doc2.json           <- Migrated schema
    index.json          <- Rebuilt by provider
  Collection2/
    doc3.json           <- Migrated schema
  system_metadata/      <- Created fresh
```

## Exit Codes

| Code | Constant | Meaning |
|------|----------|---------|
| 0 | `process_exit_code::SUCCESS` | Migration completed successfully |
| 1 | `process_exit_code::HELP` | Help message displayed |
| 128 | `process_exit_code::INVALID_ARGUMENT` | Missing `--uri` argument |
| 129 | `process_exit_code::INVALID_ARGUMENT \| 1` | URI is not a FileSystemDB URI |
| 144 | `process_exit_code::UNCAUGHT_EXCEPTION` | Unhandled exception occurred |

## Error Handling

Individual document failures are logged but do not stop the migration:

```cpp
try {
  // ... process document ...
  std::cout << " -> succeeded\n";
} catch (...) {
  std::cerr << "Failed to import a document: "
            << ::debug::current_exception_diagnostic_information() << "\n";
  // Continue with next document
}
```

Output shows progress for each document:

```
Database:/path/to/database

 Collection:RunHistory
   Document: abc123 -> succeeded
   Document: def456 -> succeeded
   Document: ghi789 -> failed

 Collection:SystemLayout
   Document: schema -> succeeded
```

## Limitations

- **FileSystemDB Only:** Only supports `filesystemdb://` URIs. MongoDB databases should use `mongodump`/`mongorestore` with schema migration scripts.
- **No Rollback:** The new database must be manually deleted if migration results are unsatisfactory.
- **Sequential Processing:** Documents are processed one at a time (not parallelized).
- **No Resume:** If interrupted, migration must restart from the beginning.

## Environment Variables

| Variable | Purpose |
|----------|---------|
| None required | The URI is provided as a command-line argument |

Environment variables in the URI path are expanded automatically (e.g., `$HOME`, `${CONFIG_DIR}`).

## Common Pitfalls

- **Wrong URI Scheme:** Only `filesystemdb://` URIs are supported; other schemes will fail
- **Disk Space:** Ensure sufficient space for the migrated database (approximately same size as source)
- **Trailing Slash:** The tool handles trailing slashes automatically
- **File Permissions:** Need read permissions on source and write permissions for target location
- **Index Files:** The `index.json` files are regenerated; do not migrate them

### Anti-patterns

```bash
# DON'T: Use MongoDB URI
migrate_database -u mongodb://localhost:27017/mydb  # Will fail

# DO: Use FileSystemDB URI
migrate_database -u filesystemdb:///path/to/db
```

## Relationship to Other Components

- **JSONDocumentMigrator:** Core migration logic that transforms document schema
- **JSONDocumentBuilder:** Constructs migrated documents with proper structure
- **FileSystemDB Provider:** Handles reading source and writing target documents
- **rebuild_database_index:** May be needed after migration to optimize search indexes

## Maintenance Notes

When updating the document schema:
1. Update `JSONDocumentMigrator` to handle the new schema version
2. Test migration on a copy of production data
3. Document schema changes in release notes
4. Consider backward compatibility for incremental migrations

## See Also

- [rebuild_database_index.cc.md](./rebuild_database_index.cc.md) - Rebuild search indexes
- [JSONDocumentMigrator.h](../JsonDocument/JSONDocumentMigrator.h.md) - Migration implementation
- [JSONDocumentBuilder.h](../JsonDocument/JSONDocumentBuilder.h.md) - Document builder
- [provider_filedb.h](../StorageProviders/FileSystemDB/provider_filedb.h.md) - FileSystemDB provider
