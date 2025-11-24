# provider_filedb.cpp

## File Overview

This implementation file provides the template specializations for FileSystemDB storage operations. It implements the core database operations (findConfigurations, readDocument, writeDocument, etc.) using a filesystem-based storage model where directories represent collections and files represent documents.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/FileSystemDB/provider_filedb.cpp`

**Lines of Code**: 501

**Purpose**: Implementation of FileSystemDB storage provider operations

## Dependencies

### Project Headers
- `"artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"` - FileSystemDB declarations
- `"artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h"` - Search index functionality
- `"artdaq-database/StorageProviders/common.h"` - Common utilities
- `"artdaq-database/SharedCommon/sharedcommon_common.h"` - Shared utilities
- `"artdaq-database/BasicTypes/basictypes.h"` - Basic type definitions
- `"artdaq-database/JsonDocument/JSONDocumentBuilder.h"` - JSON document construction

### TRACE Configuration

```cpp
#define TRACE_NAME "provider_filedb.cpp"
```

**Purpose**: Set TRACE identifier for debugging output from this file.

## Namespace Aliases

```cpp
namespace db = artdaq::database;
namespace dbfs = artdaq::database::filesystem;
namespace dbfsl = dbfs::literal;
namespace apiliteral = db::configapi::literal;
```

**Purpose**: Shorten long namespace paths for readability.

---

## Template Specialization: findConfigurations

```cpp
template <>
template <>
std::vector<JSONDocument>
StorageProvider<JSONDocument, FileSystemDB>::findConfigurations(
    JSONDocument const& query_payload);
```

### Purpose

Find all configurations in the database, returning them sorted by most recent assignment timestamp.

### Parameters

- **query_payload** - JSONDocument containing search criteria
  - Must not be empty (validated with `confirm()`)

### Returns

Vector of JSONDocument objects, each representing a found configuration with metadata for how to access it.

### Algorithm

1. **Scan Collections**: Iterate through all collection directories
2. **Query Index**: For each collection, query the search index
3. **Collect Timestamps**: Gather all assignment timestamps for each configuration
4. **Sort**: Sort configurations by most recent timestamp (reverse chronological)
5. **Build Results**: Create JSON documents describing how to access each configuration

### Implementation Details

#### Data Structures

```cpp
using timestamp_t = unsigned long long;
using ordered_timestamps_t = std::set<timestamp_t>;
using config_timestamps_t = std::map<std::string, ordered_timestamps_t>;
```

**Purpose**:
- `timestamp_t`: Unix timestamp (seconds since epoch)
- `ordered_timestamps_t`: Set of timestamps for a configuration (auto-sorted)
- `config_timestamps_t`: Map from configuration name to its timestamps

#### Reverse Sorting

```cpp
auto reverse_timestamp_cmp = [](const timestamp_t& a, const timestamp_t& b) {
    return a > b;
};
using timestamp_configs_t = std::multimap<timestamp_t, std::string,
                                          decltype(reverse_timestamp_cmp)>;
```

**Purpose**: Sort configurations by timestamp in reverse chronological order (newest first).

#### Process Flow

```cpp
// 1. Get database path and expand environment variables
auto collection = _provider->connection();
collection = expand_environment_variables(collection);

// 2. Find all collections
auto dir_name = dbfs::mkdir(collection).append("/");
auto collection_names = dbfs::find_subdirs(dir_name);

// 3. For each collection, query for configurations
for (auto const& collection_name : collection_names) {
    auto index_path = boost::filesystem::path(dir_name.c_str())
                          .append(collection_name)
                          .append(dbfsl::search_index);

    SearchIndex search_index(index_path);
    auto configentityname_pairs = search_index.findAllGlobalConfigurations(query_payload);

    // Collect timestamps for each configuration
    for (auto const& pair : configentityname_pairs) {
        auto timestamps = search_index.getConfigurationAssignedTimestamps(...);
        // Convert to epoch seconds and store
    }
}

// 4. Sort by most recent timestamp
// 5. Build result documents
```

### Return Format

Each returned JSONDocument contains:

```json
{
  "provider": "filesystem",
  "format": "gui",
  "operation": "confcomposition",
  "searchfilter": {
    "configurations": "MyConfiguration"
  }
}
```

**Fields**:
- `provider`: Always "filesystem"
- `format`: Always "gui" (GUI-friendly format)
- `operation`: "confcomposition" (get configuration composition)
- `searchfilter`: Contains configuration name

### Usage Example

```cpp
JSONDocument query;
query.setFilter("{}");  // Empty filter = all configurations

auto configs = provider->findConfigurations(query);
for (auto const& config : configs) {
    std::cout << "Configuration: " << config << std::endl;
}
```

### Debugging

```cpp
TLOG(15) << "FileSystemDB::findConfigurations() begin";
TLOG(15) << "FileSystemDB::findConfigurations() args data=<" << query_payload << ">";
TLOG(15) << "FileSystemDB::findConfigurations() search returned " << n << " configurations.";
```

Enable with: `artdaq::database::filesystem::debug::enable();`

---

## Template Specialization: configurationComposition

```cpp
template <>
template <>
std::vector<JSONDocument>
StorageProvider<JSONDocument, FileSystemDB>::configurationComposition(
    JSONDocument const& query_payload);
```

### Purpose

Get the composition (constituent entities) of a configuration.

### Parameters

- **query_payload** - JSONDocument specifying which configuration

### Returns

Vector of JSONDocuments, each representing an entity in the configuration.

### Algorithm

1. Scan all collections
2. Query search index for configurations matching query
3. For each configuration, find all entities
4. Remove duplicates (same entity appears once)
5. Return documents describing how to read each entity

### Implementation Details

#### Duplicate Elimination

```cpp
auto seenValues = std::list<std::string>{};

auto hasSeenValue = [&v = seenValues](auto const& name) {
    if (std::find(v.begin(), v.end(), name) != v.end()) {
        return false;  // Already seen
    }
    v.emplace_back(name);
    return true;  // First time seeing this
};

for (auto const& pair : configentityname_pairs) {
    if (!hasSeenValue(pair.second)) continue;  // Skip duplicates
    // Process entity
}
```

**Purpose**: Ensure each entity appears only once in the results, even if it appears in multiple collections.

### Return Format

Each returned JSONDocument:

```json
{
  "collection": "ComponentConfigs",
  "provider": "filesystem",
  "format": "gui",
  "operation": "readdocument",
  "searchfilter": {
    "configurations": "MyConfig",
    "entities": "TPC_01"
  }
}
```

**Usage**: These documents can be passed to `readDocument()` to fetch the actual entity configurations.

---

## Template Specialization: findVersions

```cpp
template <>
template <>
std::vector<JSONDocument>
StorageProvider<JSONDocument, FileSystemDB>::findVersions(
    JSONDocument const& filter);
```

### Purpose

Find all versions of entities or configurations.

### Parameters

- **filter** - JSONDocument containing:
  - `collection`: Which collection to search
  - `searchfilter`: Search criteria (entity name or configuration name)

### Returns

Vector of JSONDocuments, each representing a version.

### Algorithm

The function has two modes:

#### Mode 1: Find Versions by Entity Name

When `searchfilter` does NOT contain `configurations` field:

1. Extract entity name from filter
2. Query index for all versions of that entity
3. Return documents for each version

```cpp
if (search_ast.count(apiliteral::filter::configurations) == 0) {
    auto versionentityname_pairs = search_index.findVersionsByEntityName(query_payload);
    // Build result documents
}
```

#### Mode 2: Find Versions by Configuration Name

When `searchfilter` contains `configurations` field:

1. Extract configuration name (and optional entity name)
2. Query index for all versions in that configuration
3. Return documents for each version

```cpp
else {
    auto versionentityname_pairs = search_index.findVersionsByGlobalConfigName(query_payload);
    // Build result documents
}
```

### Return Format

```json
{
  "collection": "ComponentConfigs",
  "provider": "filesystem",
  "format": "gui",
  "operation": "readdocument",
  "searchfilter": {
    "version": "v1.2.3",
    "entities": "TPC_01"
  }
}
```

### Usage Example

```cpp
// Find versions of an entity
JSONDocument filter;
filter.setCollection("ComponentConfigs");
filter.setFilter("{\"entities\": \"TPC_01\"}");

auto versions = provider->findVersions(filter);
// Returns: v1.0, v1.1, v1.2, v2.0, etc.
```

---

## Template Specialization: findEntities

```cpp
template <>
template <>
std::vector<JSONDocument>
StorageProvider<JSONDocument, FileSystemDB>::findEntities(
    JSONDocument const& filter);
```

### Purpose

Find all entities (components, systems) in the database.

### Parameters

- **filter** - JSONDocument with search criteria

### Returns

Vector of JSONDocuments, each representing a found entity.

### Algorithm

1. Scan all collections
2. Query each collection's index for entities
3. Remove duplicates (entities can appear in multiple collections)
4. Sort entities alphabetically
5. Return documents for accessing each entity

### Duplicate Removal

```cpp
if (configentity_names.size() > 1) {
    std::sort(configentity_names.begin(), configentity_names.end());
    auto unique_configentity_names = std::vector<std::string>{};

    std::unique_copy(configentity_names.begin(),
                    configentity_names.end(),
                    back_inserter(unique_configentity_names));
    configentity_names.swap(unique_configentity_names);
}
```

**Purpose**: Use `std::unique_copy` to eliminate duplicate entity names.

### Return Format

```json
{
  "collection": "ComponentConfigs",
  "provider": "filesystem",
  "format": "gui",
  "operation": "findversions",
  "searchfilter": {
    "entities": "TPC_01"
  }
}
```

**Note**: The `operation` is `findversions`, allowing the GUI to query for versions of this entity.

---

## Template Specialization: listCollections

```cpp
template <>
template <>
std::vector<JSONDocument>
StorageProvider<JSONDocument, FileSystemDB>::listCollections(
    JSONDocument const& query_payload);
```

### Purpose

List all collections (tables) in the database.

### Parameters

- **query_payload** - JSONDocument (typically empty for list operations)

### Returns

Vector of JSONDocuments, each representing a collection.

### Algorithm

1. Get database root directory
2. Find all subdirectories (each is a collection)
3. Filter out system collections
4. Return documents for each user collection

### System Collections Filtering

```cpp
for (auto const& collection_name : collection_names) {
    if (collection_name == "system.indexes" ||
        collection_name == system_metadata) {
        continue;  // Skip system collections
    }
    // Include this collection
}
```

**System Collections**:
- `system.indexes`: MongoDB compatibility (unused in FileSystemDB)
- `SystemMetadata`: Database metadata (filtered from user list)

### Return Format

```json
{
  "collection": "ComponentConfigs",
  "provider": "filesystem",
  "format": "gui",
  "operation": "findversions",
  "searchfilter": {}
}
```

### Usage Example

```cpp
JSONDocument query("{}");
auto collections = provider->listCollections(query);

for (auto const& col : collections) {
    auto doc = JSONDocument(col);
    std::string name = doc.findChild("collection").value();
    std::cout << "Collection: " << name << std::endl;
}
```

---

## Template Specialization: addConfiguration

```cpp
template <>
template <>
std::vector<JSONDocument>
StorageProvider<JSONDocument, FileSystemDB>::addConfiguration(
    JSONDocument const& query_payload);
```

### Purpose

Add a new configuration to the database.

### Status

**NOT IMPLEMENTED** - Function body is empty.

### Parameters

- **query_payload** - Configuration details

### Returns

Empty vector (currently)

### Future Implementation

This function would:
1. Parse configuration details from query_payload
2. Create new configuration entry in index
3. Link entities to configuration
4. Return success document

---

## Template Specialization: listDatabases

```cpp
template <>
template <>
std::vector<JSONDocument>
StorageProvider<JSONDocument, FileSystemDB>::listDatabases(
    JSONDocument const& query_payload);
```

### Purpose

List all databases (sibling directories of current database).

### Parameters

- **query_payload** - Query document

### Returns

Vector of JSONDocuments, each representing a database.

### Algorithm

1. Get current database path
2. Remove trailing directory component
3. Find all sibling directories
4. Filter out system directories
5. Return documents for each database

### Path Manipulation

```cpp
auto database = _provider->connection();

// Remove trailing slash
auto found = database.find_last_not_of("\\/");
if (found != std::string::npos) {
    database.erase(found + 1);
}

// Find siblings
auto database_names = dbfs::find_siblingdirs(database);
```

### System Database Filtering

```cpp
if (database_name == "system") {
    continue;  // Skip system database
}
```

### Return Format

```json
{
  "database": "artdaq_configuration_db",
  "provider": "filesystem",
  "format": "gui",
  "searchfilter": {}
}
```

---

## Template Specialization: databaseMetadata

```cpp
template <>
template <>
std::vector<JSONDocument>
StorageProvider<JSONDocument, FileSystemDB>::databaseMetadata(
    JSONDocument const& query_payload [[gnu::unused]]);
```

### Purpose

Retrieve database metadata (creation time, format version, etc.).

### Parameters

- **query_payload** - Marked `[[gnu::unused]]` (not used in current implementation)

### Returns

Vector containing metadata documents from SystemMetadata collection.

### Algorithm

1. Construct path to SystemMetadata collection
2. Strip URI scheme if present
3. Find all documents in SystemMetadata
4. Read each document
5. Return as JSONDocument vector

### URI Scheme Handling

```cpp
auto collection = _provider->connection() + system_metadata;
collection = expand_environment_variables(collection);

if (collection.find(dbfsl::FILEURI) == 0) {
    collection = collection.substr(strlen(dbfsl::FILEURI));
}
```

**Purpose**: Remove `filesystemdb://` prefix if present, leaving just the filesystem path.

### Document Reading

```cpp
for (auto const& oid : oids) {
    auto doc_path = boost::filesystem::path(collection)
                        .append(oid)
                        .replace_extension(".json");

    auto json = std::string{};
    db::read_buffer_from_file(json, {doc_path.c_str()});

    returnCollection.emplace_back(json);
}
```

**Process**: For each object ID, construct the file path, read the JSON content, and add to results.

### Metadata Content

Typical metadata document:

```json
{
  "document": {
    "name": "artdaq_configuration_db",
    "uri": "filesystemdb:///data/artdaq/configs",
    "locale": "en_US.UTF-8",
    "create_time": "2025-11-13T10:30:45.123-0600",
    "create_user": "artdaq",
    "uname": {...},
    "database_format": 1
  }
}
```

---

## Template Specialization: searchCollection

```cpp
template <>
template <>
std::vector<JSONDocument>
StorageProvider<JSONDocument, FileSystemDB>::searchCollection(
    JSONDocument const& query_payload);
```

### Purpose

Search within a collection using flexible criteria.

### Status

**NOT IMPLEMENTED** - Throws exception.

### Exception

```cpp
throw runtime_error("FileSystemDB")
    << "StorageProvider::FileSystemDB::searchCollection() is not implemented";
```

### Future Implementation

This function would allow arbitrary JSON-based queries similar to MongoDB's query language.

---

## Debug Functions

### debug::enable

```cpp
namespace filesystem {
namespace debug {
void enable() {
    TRACE_CNTL("name", TRACE_NAME);
    TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
    TRACE_CNTL("modeM", trace_mode::modeM);
    TRACE_CNTL("modeS", trace_mode::modeS);

    TLOG(10) << "artdaq::database::filesystem trace_enable";
    artdaq::database::filesystem::debug::ReadWrite();
    artdaq::database::filesystem::index::debug::enable();
}
}
}
```

### Purpose

Enable comprehensive TRACE debugging for FileSystemDB operations.

### Configuration

- **name**: Set trace name to identify this module
- **lvlset**: Enable all trace levels (all bits set)
- **modeM/modeS**: Set trace modes for message and summary output

### Cascading Enable

```cpp
artdaq::database::filesystem::debug::ReadWrite();
artdaq::database::filesystem::index::debug::enable();
```

**Purpose**: Also enable debugging for read/write operations and index operations.

### Usage

```cpp
// At program start or before debugging
artdaq::database::filesystem::debug::enable();

// Now all TLOG statements will produce output
auto results = provider->findConfigurations(query);
```

---

## Common Patterns

### Query Payload Processing

Most functions follow this pattern:

```cpp
// 1. Validate
confirm(!query_payload.empty());

// 2. Log
TLOG(level) << "Function() begin";
TLOG(level) << "Function() args data=<" << query_payload << ">";

// 3. Get and expand database path
auto collection = _provider->connection();
collection = expand_environment_variables(collection);

// 4. Perform operation
// ...

// 5. Build results
auto returnCollection = std::vector<JSONDocument>();
// ... populate returnCollection

// 6. Return
return returnCollection;
```

### JSON Document Construction

Results use a consistent format:

```cpp
std::ostringstream oss;
oss << "{";
oss << db::quoted_(apiliteral::option::collection) << ":" << db::quoted_(collection_name) << ",";
oss << db::quoted_(apiliteral::option::provider) << ":" << db::quoted_(apiliteral::provider::filesystem) << ",";
oss << db::quoted_(apiliteral::option::format) << ":" << db::quoted_(apiliteral::format::gui) << ",";
oss << db::quoted_(apiliteral::option::operation) << ":" << db::quoted_(operation_type) << ",";
oss << db::quoted_(apiliteral::option::searchfilter) << ":" << filter_json;
oss << "}";

returnCollection.emplace_back(oss.str());
```

**Pattern**: Always include provider, format, operation, and searchfilter fields.

---

## Performance Considerations

### Index Usage

All search operations use the SearchIndex class rather than scanning files:

```cpp
SearchIndex search_index(index_path);
auto results = search_index.findVersionsByEntityName(query_payload);
```

**Performance**: O(1) to O(log n) for index lookups vs O(n) for file scanning.

### Environment Variable Expansion Caching

Environment variables are expanded on every call:

```cpp
collection = expand_environment_variables(collection);
```

**Optimization Opportunity**: Cache expanded paths to avoid repeated expansion.

### Directory Scanning

Collections are discovered by scanning directories:

```cpp
auto collection_names = dbfs::find_subdirs(dir_name);
```

**Performance**: O(n) where n is number of subdirectories. Acceptable for small numbers of collections.

---

## Error Handling

### Validation

All functions validate input:

```cpp
confirm(!query_payload.empty());
```

**Throws**: `runtime_exception` in release builds, assertion in debug builds.

### Exception Propagation

Exceptions from lower layers (Boost.Filesystem, file I/O) propagate to caller:

```cpp
// No try-catch blocks in most functions
// Caller must handle exceptions
```

**Design**: Functions are exception-neutral - they clean up but don't suppress exceptions.

---

## Related Files

- **provider_filedb.h** - Declarations
- **provider_filedb_index.h/cpp** - Search index implementation
- **provider_filedb_readwrite.cpp** - Read/write document implementations
- **storage_providers.h** - Base template

---

## Best Practices

### Error Checking

```cpp
// GOOD: Validate inputs
confirm(!query_payload.empty());

// GOOD: Check for empty results
if (results.empty()) {
    TLOG(15) << "No results found";
    return std::vector<JSONDocument>();
}
```

### TRACE Logging

```cpp
// GOOD: Log at function entry
TLOG(15) << "Function() begin";

// GOOD: Log important data
TLOG(15) << "Found " << results.size() << " items";

// GOOD: Use appropriate trace levels
// 10-14: Errors and warnings
// 15-19: Normal operations
// 20+: Detailed debugging
```

---

**Documentation generated for artdaq-database FileSystemDB provider**
