# dispatch_filedb.h

**Path:** `artdaq-database/ConfigurationDB/dispatch_filedb.h`

**Purpose:** Declares the FileSystemDB storage provider dispatch functions that route database operations to file-based storage. This provider stores configuration documents as JSON files organized in a directory structure, making it ideal for development, testing, standalone deployments, and scenarios where human-readable storage is beneficial.


## Key Concepts

### FileSystemDB Provider

The FileSystemDB provider implements all database operations using the local filesystem:
- **Documents:** Stored as individual JSON files
- **Collections:** Represented as directories
- **Indexes:** Maintained as separate index files for query optimization
- **No external dependencies:** Works without MongoDB or other database servers

### Storage Structure

The FileSystemDB organizes data in a hierarchical directory structure:

```
database_root/                    # From ARTDAQ_DATABASE_URI
  +-- Collection1/                # Collection = directory
  |     +-- doc_abc123.json       # Document = JSON file
  |     +-- doc_def456.json
  |     +-- .index                # Optional index file
  +-- Collection2/
  |     +-- doc_789xyz.json
  +-- SystemConfiguration/        # Global configuration storage
  |     +-- config1.json
  +-- SystemMetadata/             # System-level metadata
        +-- database_info.json
```

### Forward Declarations

The header uses forward declarations to minimize header dependencies and improve compile times:
- `JSONDocument` - Document type from `docrecord` namespace
- `ManageDocumentOperation` - Standard operation options class
- `ManageAliasesOperation` - Alias-specific operation options class

## Thread Safety

- **Thread-safe:** Conditional
- **Concurrent access:** File-level locking provides protection; concurrent writes to different files are safe; concurrent writes to the same file require external synchronization
- **Locking:** Uses file locks for atomic operations on individual documents

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | Common utilities, macros, error handling, and type definitions |

## Namespaces

### `artdaq::database::configuration::filesystem`

Main namespace containing all FileSystemDB dispatch functions.

### `artdaq::database::configuration::debug`

Debug namespace containing trace/logging enablement functions.

## Functions

All functions are declared in the `artdaq::database::configuration::filesystem` namespace unless otherwise noted.

### Document Operations

#### `writeDocument(ManageDocumentOperation const& options, JSONDocument const& insert_payload) -> void`

**Brief:** Writes a configuration document to the filesystem as a JSON file in the appropriate collection directory.

**Parameters:**
- `options` - Operation parameters including collection name, operation type (`writedocument` or `overwritedocument`), and provider (`filesystem`)
- `insert_payload` - JSON document containing the filter (document ID) and document content to write

**Preconditions:**
- `options.operation()` must be `writedocument` or `overwritedocument`
- `options.provider()` must be `filesystem`
- Database directory must exist and be writable
- `insert_payload` must contain valid JSON with `filter` and `document` fields

**Postconditions:**
- Document is written to the appropriate collection directory
- Index is updated if applicable
- File permissions match system umask

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Wrong operation option specified |
| `runtime_error` | Wrong provider option specified |
| `runtime_error` | File write failure (permissions, disk full, etc.) |

**Thread Safety:** Safe for concurrent writes to different documents; requires external synchronization for same document

**Side Effects:** Creates or overwrites file on disk; may update index file

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dispatch_filedb.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"

namespace fs = artdaq::database::configuration::filesystem;
namespace cf = artdaq::database::configuration;
using artdaq::database::docrecord::JSONDocument;

void storeDetectorConfig() {
  cf::ManageDocumentOperation opts("detector_app");
  opts.operation(apiliteral::operation::writedocument);
  opts.provider(apiliteral::provider::filesystem);
  opts.collection("DetectorConfigs");

  auto doc = JSONDocument(R"({
    "filter": {"_id": "detector001", "version": "1.0"},
    "document": {
      "name": "MainDetector",
      "threshold": 100,
      "enabled": true
    }
  })");

  try {
    fs::writeDocument(opts, doc);
    std::cout << "Document stored successfully\n";
  } catch (const std::exception& e) {
    std::cerr << "Failed to store document: " << e.what() << "\n";
  }
}
```

#### `readDocument(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Reads a single configuration document from the filesystem, automatically selecting the latest version if multiple matches exist.

**Parameters:**
- `options` - Operation parameters including collection and query filters
- `query_payload` - JSON query specifying which document to retrieve (filter criteria)

**Preconditions:**
- `options.operation()` must be `readdocument`
- `options.provider()` must be `filesystem`
- At least one matching document must exist

**Returns:** The matching `JSONDocument`, or the highest-versioned match if multiple documents match the query

**Postconditions:**
- Returned document contains the latest version matching the query
- No modifications made to stored documents

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | No documents found matching query |
| `runtime_error` | Wrong operation or provider option |
| `runtime_error` | File read failure |

**Thread Safety:** Safe for concurrent reads

#### `readDocuments(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Reads all configuration documents matching a query from the filesystem, returning all versions.

**Parameters:**
- `options` - Operation parameters including collection and query filters
- `query_payload` - JSON query specifying which documents to retrieve

**Preconditions:**
- `options.operation()` must be `readdocument`
- `options.provider()` must be `filesystem`

**Returns:** Vector of matching `JSONDocument` objects (may be empty if no matches)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Wrong operation or provider option |

**Thread Safety:** Safe for concurrent reads

#### `markDocumentReadonly(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Marks a document as read-only to prevent future modifications, setting a flag in the document metadata.

**Parameters:**
- `options` - Operation parameters
- `query_payload` - Query identifying the document to mark

**Returns:** The updated document with readonly flag set

**Thread Safety:** Requires exclusive access to the target document

#### `markDocumentDeleted(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Marks a document as deleted (soft delete) without physically removing the file from disk.

**Parameters:**
- `options` - Operation parameters
- `query_payload` - Query identifying the document to mark as deleted

**Returns:** The updated document with deleted flag set

**Thread Safety:** Requires exclusive access to the target document

### Version Management

#### `findVersions(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all available versions of a configuration document in the specified collection.

**Parameters:**
- `options` - Operation parameters including collection name and entity filter
- `query_payload` - Query payload (may be unused for this operation)

**Preconditions:**
- `options.operation()` must be `findversions`
- `options.provider()` must be `filesystem`

**Returns:** Vector of JSON documents, each containing:
- `name`: Version string
- `query`: Filter to retrieve that specific version

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Wrong operation or provider option |
| `runtime_error` | Regex parsing failure on filter string |

**Thread Safety:** Safe for concurrent reads

#### `findEntities(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all entities (named components) in the specified collection.

**Parameters:**
- `options` - Operation parameters including collection name
- `query_payload` - Query payload (may be unused)

**Preconditions:**
- `options.operation()` must be `findentities`
- `options.provider()` must be `filesystem`

**Returns:** Vector of JSON documents, each containing:
- `name`: Entity name
- `query`: Filter to retrieve documents for that entity

**Thread Safety:** Safe for concurrent reads

#### `addEntity(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Adds a new entity reference to a document's entity list.

**Parameters:**
- `options` - Operation parameters
- `query_payload` - Query identifying the document and entity to add

**Returns:** The updated document with new entity added

#### `removeEntity(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Removes an entity reference from a document's entity list.

**Parameters:**
- `options` - Operation parameters
- `query_payload` - Query identifying the document and entity to remove

**Returns:** The updated document with entity removed

### Alias Operations

#### `addVersionAlias(ManageAliasesOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Creates an alias (human-readable name) pointing to a specific document version.

**Parameters:**
- `options` - Alias operation parameters including alias name and target version
- `query_payload` - Query identifying the target document

**Returns:** Confirmation document indicating alias was created

#### `removeVersionAlias(ManageAliasesOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Removes an existing version alias.

**Parameters:**
- `options` - Alias operation parameters
- `query_payload` - Query identifying the alias to remove

**Returns:** Confirmation document

#### `findVersionAliases(ManageAliasesOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all aliases defined for versions in the specified scope.

**Note:** This function is **not implemented** for FileSystemDB and will always throw.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Always - function is not implemented |

#### `addConfigurationAlias(ManageAliasesOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Creates an alias for a global configuration.

**Returns:** Confirmation document indicating alias was created

#### `removeConfigurationAlias(ManageAliasesOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Removes an existing global configuration alias.

**Returns:** Confirmation document

### Global Configuration Operations

#### `findConfigurations(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all global configurations matching the query criteria, filtering out duplicates.

**Parameters:**
- `options` - Operation parameters
- `query_payload` - Query criteria

**Preconditions:**
- `options.operation()` must be `findconfigs`
- `options.provider()` must be `filesystem`

**Returns:** Vector of unique configuration metadata documents with names and queries

**Thread Safety:** Safe for concurrent reads

#### `findCompositionsContaining(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all global configuration compositions that contain a specific configuration type, version, and optionally entity.

**Parameters:**
- `options` - Operation parameters including:
  - `collection()`: Configuration type to search for
  - `version()`: Version string to match
  - `entity()`: Optional entity name filter
- `query_payload` - Additional query parameters (may be unused)

**Preconditions:**
- `options.operation()` must be `findcompositionscontaining`
- `options.provider()` must be `filesystem`
- Configuration type and version must be provided

**Returns:** Vector of composition names (as JSON documents) that reference the specified configuration

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Wrong operation or provider option |
| `runtime_error` | Configuration type empty or not provided |
| `runtime_error` | Version empty or not provided |
| `runtime_error` | Grep command execution failure |

**Thread Safety:** Safe for concurrent reads

**Note:** This implementation uses shell `grep` commands internally, which has performance implications for large datasets.

#### `configurationComposition(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Retrieves the complete composition (list of all component configurations) for a global configuration.

**Parameters:**
- `options` - Operation parameters including configuration name
- `query_payload` - Query for the specific configuration

**Preconditions:**
- `options.operation()` must be `confcomposition`
- `options.provider()` must be `filesystem`

**Returns:** JSON document containing the configuration composition as a search result array

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | No search filters found |
| `runtime_error` | Unsupported filter string format |

#### `assignConfiguration(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Assigns a document to a global configuration by adding a configuration reference to the document.

**Parameters:**
- `options` - Operation parameters including configuration name to assign
- `query_payload` - Query identifying the target document

**Preconditions:**
- `options.operation()` must be `assignconfig`
- `options.provider()` must be `filesystem`

**Returns:** Updated configuration composition showing the new assignment

**Side Effects:** Modifies the target document to add configuration reference; writes updated document to disk

#### `removeConfiguration(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Removes a document assignment from a global configuration.

**Parameters:**
- `options` - Operation parameters including configuration name to remove
- `query_payload` - Query identifying the target document

**Preconditions:**
- `options.operation()` must be `removeconfig`
- `options.provider()` must be `filesystem`

**Returns:** Updated configuration composition without the removed assignment

**Side Effects:** Modifies the target document; writes updated document to disk

### Metadata Operations

#### `listCollections(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Lists all collections (directories) in the database.

**Parameters:**
- `options` - Operation parameters
- `query_payload` - Search filter (may be unused)

**Preconditions:**
- `options.operation()` must be `listcollections`
- `options.provider()` must be `filesystem`

**Returns:** Vector of collection metadata documents with names and queries

**Thread Safety:** Safe for concurrent reads

#### `searchCollection(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Performs a full-text or structured search within a collection.

**Note:** This function is **not implemented** for FileSystemDB and will always throw.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Always - function is not implemented |

#### `listDatabases(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Lists all available databases (top-level directories).

**Parameters:**
- `options` - Operation parameters
- `query_payload` - Search filter (may be unused)

**Preconditions:**
- `options.operation()` must be `listdatabases`
- `options.provider()` must be `filesystem`

**Returns:** Vector of database metadata documents with names and queries

**Thread Safety:** Safe for concurrent reads

#### `readDbInfo(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Reads database-level information and statistics.

**Parameters:**
- `options` - Operation parameters
- `query_payload` - Query for specific metadata

**Preconditions:**
- `options.operation()` must be `readdbinfo`
- `options.provider()` must be `filesystem`

**Returns:** JSON document containing database metadata, or empty search result if no metadata found

**Thread Safety:** Safe for concurrent reads

### Debug Functions

#### `debug::FileSystemDB() -> void`

**Brief:** Enables TRACE debugging for FileSystemDB operations by configuring trace output modes and levels.

**Purpose:** Configure TRACE logging infrastructure for debugging file-based storage operations.

**Preconditions:** TRACE system must be initialized

**Postconditions:** All TRACE levels enabled for "dispatch_filedb.cpp"

**Side Effects:** Modifies global TRACE configuration

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dispatch_filedb.h"

void enableDebugging() {
  artdaq::database::configuration::debug::FileSystemDB();
  // Now all TRACE output from dispatch_filedb.cpp will be visible
}
```

## Relationship to Other Components

```
dispatch_filedb.h (this file)
       |
       +-- Implemented by: dispatch_filedb.cpp
       |
       +-- Included by: dispatch_common.h (aggregation header)
       |
       +-- Uses: StorageProviders/FileSystemDB/provider_filedb.h (low-level storage)
       |
       +-- Called by: detail_*.cpp (provider dispatch)
       |
       +-- Uses: ManageDocumentOperation, ManageAliasesOperation (options)
```

## Notes for Developers

### File Locking Strategy

FileSystemDB uses file locks to ensure atomic operations:
- Advisory locks on document files during write operations
- Temporary file + rename pattern for atomic updates
- Index files are locked during updates

### Index Optimization

For better query performance, FileSystemDB maintains index files:
- Index stores document metadata (ID, version, entity, timestamps)
- Queries first check the index before reading full documents
- Index is rebuilt on demand if corrupted or missing

### Path Configuration

The database root path is extracted from the `ARTDAQ_DATABASE_URI` environment variable:
```
filesystemdb:///path/to/database  ->  /path/to/database
filesystemdb://./relative/path    ->  ./relative/path
```

### Unimplemented Functions

Some functions throw `runtime_error` as they are not implemented for FileSystemDB:
- `findVersionAliases()` - Alias system not fully implemented for filesystem
- `searchCollection()` - Full-text search not implemented

Use MongoDB provider for these features if needed.

### Common Pitfalls

- **Pitfall 1:** File permissions - Ensure the process has read/write access to the database directory and all subdirectories
- **Pitfall 2:** Disk space - FileSystemDB does not check for available space before writes; monitor disk usage
- **Pitfall 3:** Large datasets - Performance degrades with very large numbers of documents (>10,000 per collection); consider MongoDB for production scale
- **Pitfall 4:** Concurrent writes - While different files can be written concurrently, writing to the same document from multiple processes requires external coordination

### Anti-patterns

```cpp
// DON'T do this: Ignore operation validation
void badWrite(JSONDocument const& doc) {
  ManageDocumentOperation opts;
  // Missing: opts.operation(), opts.provider(), opts.collection()
  fs::writeDocument(opts, doc);  // Will throw runtime_error
}

// DO this instead: Always set required options
void goodWrite(JSONDocument const& doc) {
  ManageDocumentOperation opts("myapp");
  opts.operation(apiliteral::operation::writedocument);
  opts.provider(apiliteral::provider::filesystem);
  opts.collection("MyCollection");
  fs::writeDocument(opts, doc);
}
```

## See Also

- [dispatch_filedb.cpp](./dispatch_filedb.cpp.md) - Implementation of these functions
- [dispatch_mongodb.h](./dispatch_mongodb.h.md) - MongoDB provider with similar interface
- [dispatch_common.h](./dispatch_common.h.md) - Aggregation header for all providers
- [StorageProviders/FileSystemDB/provider_filedb.h](../StorageProviders/FileSystemDB/provider_filedb.h.md) - Low-level storage implementation
- [options_operations.h](./options_operations.h.md) - Operation options classes

---

**Documentation generated for artdaq-database ConfigurationDB module**
