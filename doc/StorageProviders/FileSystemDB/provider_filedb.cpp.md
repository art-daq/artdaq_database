# provider_filedb.cpp

**Path:** `artdaq-database/StorageProviders/FileSystemDB/provider_filedb.cpp`

**Implements:** [provider_filedb.h](./provider_filedb.h.md)

**Purpose:** Implements the template specializations for FileSystemDB storage operations, providing query operations (`findConfigurations`, `configurationComposition`, `findVersions`, `findEntities`, `listCollections`, `listDatabases`, `databaseMetadata`, `addConfiguration`, `searchCollection`) using a filesystem-based storage model where directories represent collections and JSON files represent documents.

## Implementation Overview

This file provides explicit template specializations for `StorageProvider<JSONDocument, FileSystemDB>`. Each method implements the corresponding storage operation by interacting with the filesystem structure and the `SearchIndex` class for efficient document lookup.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h` | Main provider header |
| `artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h` | SearchIndex class for fast queries |
| `artdaq-database/StorageProviders/common.h` | Common utilities |
| `artdaq-database/SharedCommon/sharedcommon_common.h` | Shared utilities |
| `artdaq-database/BasicTypes/basictypes.h` | Basic type definitions |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | JSON document construction |

## Key Algorithms

### Configuration Discovery (`findConfigurations`)

Discovers all configurations across all collections and returns them sorted by most recent assignment.

**Steps:**
1. Get database root path from provider connection
2. Expand environment variables in the path
3. Scan all subdirectories (collections) in the database
4. For each collection, query its SearchIndex for configurations using `findAllGlobalConfigurations()`
5. Collect assignment timestamps for each configuration using `getConfigurationAssignedTimestamps()`
6. Sort configurations by most recent timestamp (newest first) using reverse timestamp comparison
7. Build result JSON documents with operation metadata

**Why this approach:** Sorting by timestamp ensures that the most recently used configurations appear first, which is typically what users want. Using a multimap with reverse comparison provides efficient sorted iteration.

### Entity Composition (`configurationComposition`)

Gets all entities that belong to a specific configuration with duplicate elimination.

**Steps:**
1. Scan all collection directories
2. Query each SearchIndex for configuration-entity pairs using `findAllGlobalConfigurations()`
3. Track seen entities using a `hasSeenValue` lambda to eliminate duplicates
4. Build result documents for each unique entity

### Version Discovery (`findVersions`)

Finds versions with two different modes based on filter content.

**By Entity Name (no `configurations` in filter):**
1. Parse filter to extract collection name and search filter
2. Query SearchIndex with `findVersionsByEntityName()`
3. Build result documents with version-entity pairs

**By Configuration Name (has `configurations` in filter):**
1. Parse filter to extract configuration name
2. Query SearchIndex with `findVersionsByGlobalConfigName()`
3. Build result documents with version information

### Entity Discovery (`findEntities`)

Finds all unique entities across all collections.

**Steps:**
1. Scan all collection directories
2. Query each SearchIndex with `findEntities()`
3. Sort and deduplicate using `std::unique_copy()`
4. Build result documents

## Internal Functions

### Template Specialization: `findConfigurations`

**Brief:** Finds all global configurations in the database, returning them sorted by most recent assignment timestamp.

**Signature:**
```cpp
template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, FileSystemDB>::findConfigurations(
    JSONDocument const& query_payload);
```

**Called by:** ConfigurationDB dispatch layer

**Implementation Details:**
- Uses `std::multimap` with reverse comparison for sorted iteration by timestamp
- Uses `std::set` to track unique timestamps per configuration
- Collects timestamps from all collections before sorting

### Template Specialization: `configurationComposition`

**Brief:** Gets the composition (constituent entities) of a configuration with duplicate elimination.

**Signature:**
```cpp
template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, FileSystemDB>::configurationComposition(
    JSONDocument const& query_payload);
```

**Called by:** ConfigurationDB dispatch layer

**Implementation Details:**
- Uses `hasSeenValue` lambda with `std::list` to track and eliminate duplicate entities
- Returns list of unique entities that compose the configuration

### Template Specialization: `findVersions`

**Brief:** Finds all versions of entities or configurations matching the filter.

**Signature:**
```cpp
template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, FileSystemDB>::findVersions(
    JSONDocument const& filter);
```

**Called by:** ConfigurationDB dispatch layer

**Implementation Details:**
- Detects mode by checking for `configurations` field in filter
- Routes to appropriate SearchIndex method based on mode

### Template Specialization: `findEntities`

**Brief:** Finds all entities in the database matching the filter criteria, removing duplicates.

**Signature:**
```cpp
template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, FileSystemDB>::findEntities(
    JSONDocument const& filter);
```

**Called by:** ConfigurationDB dispatch layer

**Implementation Details:**
- Scans all collections
- Sorts and deduplicates entity names using `std::sort()` and `std::unique_copy()`
- Returns unique entity list

### Template Specialization: `listCollections`

**Brief:** Lists all collections, filtering out system collections.

**Signature:**
```cpp
template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, FileSystemDB>::listCollections(
    JSONDocument const& query_payload);
```

**Called by:** ConfigurationDB dispatch layer

**Filtered Collections:**
- `system.indexes` - Internal MongoDB-style index collection
- `SystemMetadata` - Database metadata collection

### Template Specialization: `listDatabases`

**Brief:** Lists all databases (sibling directories of current database).

**Signature:**
```cpp
template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, FileSystemDB>::listDatabases(
    JSONDocument const& query_payload);
```

**Called by:** ConfigurationDB dispatch layer

**Filtered Databases:**
- `system` - Internal system directory

### Template Specialization: `databaseMetadata`

**Brief:** Retrieves database metadata from the SystemMetadata collection.

**Signature:**
```cpp
template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, FileSystemDB>::databaseMetadata(
    JSONDocument const& query_payload);
```

**Called by:** ConfigurationDB dispatch layer

**Implementation Details:**
- Reads all documents from the `SystemMetadata` collection
- Uses `read_buffer_from_file()` to read each document

### Template Specialization: `addConfiguration`

**Brief:** Placeholder for adding configurations.

**Signature:**
```cpp
template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, FileSystemDB>::addConfiguration(
    JSONDocument const& query_payload);
```

**Status:** Not fully implemented for FileSystemDB - returns empty vector

### Template Specialization: `searchCollection`

**Brief:** Performs advanced collection search.

**Signature:**
```cpp
template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, FileSystemDB>::searchCollection(
    JSONDocument const& query_payload);
```

**Status:** Not implemented - throws `runtime_error`

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Always - not implemented |

### Debug Functions

```cpp
namespace filesystem {
namespace debug {
    void enable();  // Enables TRACE debugging for all FileSystemDB operations
}}
```

**Brief:** Enables comprehensive TRACE logging for FileSystemDB module, including `debug::ReadWrite()` and `filesystem::index::debug::enable()`.

## Performance Considerations

- **Index-driven queries:** All search operations use SearchIndex for O(log n) lookups
- **Collection scanning:** `findConfigurations` and `findEntities` scan all collections, which can be slow with many collections
- **Memory usage:** Results are accumulated in memory before returning

## Error Handling Strategy

- All functions validate input with `confirm(!query_payload.empty())`
- Exceptions from lower layers (filesystem, JSON parsing) propagate to caller
- TRACE logging captures operation details for debugging

## Testing Notes

- **Unit tests:** `test/StorageProviders/FileSystemDB/provider_filedb_t.cc`
- **Key test cases:**
  - Finding configurations across multiple collections
  - Version discovery by entity and by configuration
  - Entity deduplication
  - Collection filtering (system collections excluded)

## Maintenance Notes

### Result Document Format
All operations return JSONDocuments with this consistent structure:
```json
{
  "collection": "CollectionName",
  "provider": "filesystem",
  "format": "gui",
  "operation": "operation_type",
  "searchfilter": { ... }
}
```

### TRACE Logging Levels
| Level | Usage |
|-------|-------|
| 10-14 | Errors and warnings |
| 15-19 | Normal operations |
| 20+ | Detailed debugging |

Enable debugging with:
```cpp
artdaq::database::filesystem::debug::enable();
```

### Environment Variable Expansion
Database paths are expanded on each call to support dynamic configuration:
```cpp
auto collection = _provider->connection();
collection = expand_environment_variables(collection);
```

## See Also

- [provider_filedb.h](./provider_filedb.h.md) - Header file with class declarations
- [provider_filedb_index.h](./provider_filedb_index.h.md) - SearchIndex class
- [provider_filedb_readwrite.cpp](./provider_filedb_readwrite.cpp.md) - Read/write implementations
