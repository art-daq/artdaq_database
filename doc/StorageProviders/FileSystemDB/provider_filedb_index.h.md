# provider_filedb_index.h

**Path:** `artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h`

**Purpose:** Defines the `SearchIndex` class that provides fast document lookup capabilities for the FileSystemDB provider. The index maintains in-memory mappings between searchable attributes (version, configuration, entity, run, version alias) and document object IDs, enabling efficient queries without scanning all document files.

## Key Concepts

### Inverted Index Structure
The SearchIndex implements an inverted index where each searchable attribute (version, entity, configuration, run, version alias) maps to a list of document IDs that have that attribute value. This enables O(log n) lookups instead of O(n) file scanning.

### Multi-Criteria AND Logic
When searching with multiple criteria, the index finds documents matching ALL criteria by computing the intersection of matching document IDs for each criterion using a count-based approach.

### Lazy Persistence
The index is kept in memory for fast queries and only written to disk when modified (`_isDirty` flag) upon destruction, ensuring efficient operation while maintaining durability.

### Path-Based Mutex System
Each index file path has an associated mutex to prevent concurrent access to the same index from multiple threads, ensuring thread-safe operations within a single process.

## Thread Safety

- **Thread-safe:** Partially (per-path locking)
- **Concurrent access:** Safe for multiple SearchIndex instances on different paths; single instance per path is locked for its lifetime
- **Locking:** Acquires path-specific mutex on construction, releases on destruction

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/Json/json_common.h` | JSON data structures (`object_t`, `array_t`) |
| `artdaq-database/StorageProviders/common.h` | Common utilities |
| `artdaq-database/StorageProviders/storage_providers.h` | Provider interface and `object_id_t` |
| `<boost/filesystem.hpp>` | Filesystem path handling |
| `<map>` | Index data structures and path mutex storage |
| `<memory>` | Smart pointers for mutex management |
| `<mutex>` | Thread synchronization |

## Classes/Structures

### `SearchIndex`

In-memory inverted index for fast document lookup with automatic persistence.

**Thread Safety:** Thread-safe through path-based locking; only one SearchIndex instance can access a given path at a time

#### Constructors

##### `SearchIndex(boost::filesystem::path const& path)`

**Brief:** Constructs a SearchIndex for the given index file path, acquiring the path-specific mutex and loading or creating the index.

**Parameters:**
- `path` - Path to the `index.json` file

**Preconditions:**
- Path must be writable
- Parent directory must exist or be creatable

**Postconditions:**
- Index is loaded into memory and ready for queries
- Path mutex is held until destruction

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Index file is corrupt and auto-rebuild is disabled |
| `runtime_error` | Failed to create index file |

#### Destructor

##### `~SearchIndex()`

**Brief:** Saves the index to disk if modified and releases the path mutex. Exceptions are caught and logged to ensure safe destruction.

**Postconditions:**
- If `_isDirty`, index is written to disk
- Path mutex is released

#### Deleted Operations

The following operations are explicitly deleted to prevent misuse:

- `SearchIndex()` - Default constructor
- `SearchIndex& operator=(SearchIndex const&)` - Copy assignment
- `SearchIndex& operator=(SearchIndex&&)` - Move assignment
- `SearchIndex(SearchIndex&&)` - Move constructor

#### Static Methods

##### `getMutexForPath(path) -> std::mutex&`

**Brief:** Returns the mutex associated with a specific file path, creating one if needed.

**Parameters:**
- `path` - Path to get mutex for

**Returns:** Reference to the path-specific mutex

**Thread Safety:** Thread-safe (uses internal mutex for path map access)

#### Search Methods

##### `findDocumentIDs(search) -> std::vector<object_id_t>`

**Brief:** Finds documents matching all search criteria (AND logic).

**Parameters:**
- `search` - JSONDocument containing search criteria (version, entity, configurations, runs, version_aliases)

**Preconditions:**
- `search` must not be empty

**Returns:** Vector of object IDs matching all criteria (empty if no matches)

**Thread Safety:** Thread-safe (index is locked during SearchIndex lifetime)

**Example:**
```cpp
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h"

using namespace artdaq::database::filesystem::index;

void searchDocuments() {
  SearchIndex index(collection_path / "index.json");
  auto search = JSONDocument{R"({"version":"v1.0","entities":"TPC_01"})"};
  auto ids = index.findDocumentIDs(search);
  // ids contains documents that have BOTH version v1.0 AND entity TPC_01
}
```

##### `findAllGlobalConfigurations(search) -> std::vector<std::pair<std::string, std::string>>`

**Brief:** Finds all configurations and their associated entities.

**Parameters:**
- `search` - Filter criteria (can be empty JSON for all configurations)

**Returns:** Vector of (configuration_name, entity_name) pairs

##### `findVersionsByGlobalConfigName(search) -> std::vector<std::pair<std::string, std::string>>`

**Brief:** Finds all versions within a specific configuration.

**Parameters:**
- `search` - Must contain `configurations` field

**Returns:** Vector of (version, entity_name) pairs

##### `findVersionsByEntityName(search) -> std::vector<std::pair<std::string, std::string>>`

**Brief:** Finds all versions of a specific entity.

**Parameters:**
- `search` - Must contain `entities` field

**Returns:** Vector of (entity_name, version) pairs

##### `findEntities(search) -> std::vector<std::string>`

**Brief:** Finds all entity names matching the filter.

**Parameters:**
- `search` - Filter criteria for entity names (supports prefix matching with wildcard `*`)

**Returns:** Vector of matching entity names

##### `getConfigurationAssignedTimestamps(search) -> std::vector<std::string>`

**Brief:** Gets all timestamps when a configuration was assigned.

**Parameters:**
- `search` - Must contain `configurations` field

**Returns:** Vector of ISO 8601 timestamp strings

#### Modification Methods

##### `addDocument(document, oid) -> bool`

**Brief:** Adds a document to the index, extracting and indexing all searchable attributes (version, configurations, entities, aliases, runs).

**Parameters:**
- `document` - JSONDocument containing the document data
- `oid` - Object ID to associate with the document

**Preconditions:**
- Neither `document` nor `oid` may be empty

**Returns:** `true` if successful, `false` on parsing error

**Postconditions:**
- Index is marked dirty and will be saved on destruction

**Side Effects:**
- Sets `_isDirty = true`

##### `removeDocument(document, oid) -> bool`

**Brief:** Removes a document from the index.

**Parameters:**
- `document` - JSONDocument containing the document data (for attribute extraction)
- `oid` - Object ID to remove

**Preconditions:**
- Neither `document` nor `oid` may be empty

**Returns:** `true` if successful, `false` on error

**Postconditions:**
- Index is marked dirty and will be saved on destruction

## Index File Structure

The index is stored as JSON in `index.json`:
```json
{
  "version": {
    "v1.0": ["507f1f77bcf86cd799439011", "507f1f77bcf86cd799439012"],
    "v2.0": ["507f1f77bcf86cd799439013"]
  },
  "entities": {
    "TPC_01": ["507f1f77bcf86cd799439011"],
    "ECAL_01": ["507f1f77bcf86cd799439012"]
  },
  "configurations": {
    "RunConfig": ["507f1f77bcf86cd799439011", "507f1f77bcf86cd799439012"]
  },
  "configurations.assigned": {
    "RunConfig": ["2025-01-15T10:30:00.000Z", "2025-01-16T14:00:00.000Z"]
  },
  "runs": {
    "12345": ["507f1f77bcf86cd799439011"]
  },
  "version_aliases": {
    "latest": ["507f1f77bcf86cd799439013"]
  },
  "ouid": ["507f1f77bcf86cd799439011", "507f1f77bcf86cd799439012", "507f1f77bcf86cd799439013"]
}
```

## Functions

### `shouldAutoRebuildSearchIndex(enable) -> bool`

**Brief:** Gets or sets whether corrupt indexes should be automatically rebuilt by scanning document files.

**Parameters:**
- `enable` - If `true`, enables auto-rebuild (optional, only affects first call)

**Returns:** Current auto-rebuild setting

**Thread Safety:** Thread-safe (static initialization)

**Example:**
```cpp
// Enable auto-rebuild before first SearchIndex use
shouldAutoRebuildSearchIndex(true);

// Later: check current setting
if (shouldAutoRebuildSearchIndex()) {
  // Auto-rebuild is enabled
}
```

## Template Functions

### `_make_unique_sorted<TYPE>(array) -> void`

**Brief:** Sorts and removes duplicates from a JSON array in-place.

**Template Parameters:**
- `TYPE` - The type of elements in the array (typically `std::string`)

**Parameters:**
- `array` - JSON array to sort and deduplicate

**Postconditions:**
- Array is sorted and contains only unique elements

## Private Members

| Member | Type | Description |
|--------|------|-------------|
| `_index` | `object_t` | In-memory JSON index structure |
| `_path` | `boost::filesystem::path` | Path to the index file |
| `_isDirty` | `bool` | Whether the index has been modified |
| `_isOpen` | `bool` | Whether the index is currently open |
| `_path_lock` | `std::unique_lock<std::mutex>` | Lock on the path-specific mutex |

## Static Members

| Member | Type | Description |
|--------|------|-------------|
| `_path_mutexes` | `std::map<std::string, std::unique_ptr<std::mutex>>` | Map from path string to mutex |
| `_path_mutexes_guard` | `std::mutex` | Mutex protecting the path_mutexes map |

## Relationship to Other Components

### Provider Integration
- `provider_filedb.cpp`: Uses SearchIndex for all query operations
- `provider_filedb_readwrite.cpp`: Updates index on document write/delete

### File Storage
Each collection directory contains an `index.json` file managed by SearchIndex:
```
collection/
  |-- index.json          # Managed by SearchIndex
  |-- document1.json
  |-- document2.json
  +-- ...
```

## See Also

- [provider_filedb_index.cpp](./provider_filedb_index.cpp.md) - Implementation details
- [provider_filedb.h](./provider_filedb.h.md) - FileSystemDB provider
- [provider_filedb.cpp](./provider_filedb.cpp.md) - Query operations using SearchIndex

## Notes for Developers

### Performance

| Operation | Time Complexity |
|-----------|----------------|
| `findDocumentIDs()` (single criterion) | O(log n) |
| `findDocumentIDs()` (k criteria) | O(k * log n + intersection) |
| `addDocument()` | O(m) where m = attributes in document |
| `removeDocument()` | O(m) where m = attributes in document |

**Space Complexity:** O(n * m) where n = documents, m = average attributes per document

### Best Practices

```cpp
// GOOD: RAII pattern - index auto-saves on destruction
{
    SearchIndex index(path);
    index.addDocument(doc, oid);
} // Automatically saves here if modified

// GOOD: Handle potential corruption
try {
    SearchIndex index(path);
} catch (runtime_error const& e) {
    // Handle corrupted index - enable rebuild and retry
    shouldAutoRebuildSearchIndex(true);
    SearchIndex index(path);  // Will rebuild from documents
}
```

### Common Pitfalls

- **Pitfall 1:** Creating multiple SearchIndex instances for the same path in the same thread will deadlock. Use a single instance.
- **Pitfall 2:** Not handling exceptions from constructor can leave index in inconsistent state.
- **Pitfall 3:** Manually deleting index.json while SearchIndex is active causes undefined behavior.

### Debug Mode
Enable detailed logging:
```cpp
artdaq::database::filesystem::index::debug::enable();
```
