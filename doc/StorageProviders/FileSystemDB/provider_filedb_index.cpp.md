# provider_filedb_index.cpp

**Path:** `artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.cpp`

**Implements:** [provider_filedb_index.h](./provider_filedb_index.h.md)

**Purpose:** Implements the `SearchIndex` class functionality for fast document lookup in the FileSystemDB provider. Provides an in-memory inverted index with persistent JSON storage, enabling efficient multi-criteria queries without file scanning.

## Implementation Overview

This file implements the `SearchIndex` class which maintains an inverted index mapping document attributes to object IDs. The index is loaded from disk on construction, modified in memory, and persisted on destruction if changes were made. A path-based mutex system ensures thread-safe access.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h` | Header file with class declaration |
| `artdaq-database/DataFormats/Json/json_types_impl.h` | JSON type implementations |
| `artdaq-database/SharedCommon/sharedcommon_common.h` | Shared utilities |
| `artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h` | FileSystemDB provider |
| `artdaq-database/StorageProviders/FileSystemDB/provider_filedb_headers.h` | Aggregator header |
| `artdaq-database/BasicTypes/data_json.h` | JSON data types |
| `artdaq-database/DataFormats/Json/json_common.h` | JSON common utilities |
| `<fstream>` | File I/O for index persistence |
| `<boost/filesystem.hpp>` | Filesystem operations |

## Key Algorithms

### Multi-Criteria Search (`findDocumentIDs`)

Implements AND logic across multiple search criteria using a count-based approach.

**Steps:**
1. Parse JSON search criteria into AST
2. If search is empty, return all object IDs from `getObjectIds()`
3. For each criterion, call appropriate `_match*()` method via function pointer map
4. Collect all matching document IDs with occurrence counts
5. Filter to keep only IDs matching ALL criteria (count >= criteria count)
6. Sort and deduplicate results

**Why this approach:** Count-based filtering is efficient for multi-criteria AND logic without expensive set intersection operations.

```cpp
// Pseudocode
std::map<object_id_t, int> match_counts;
for (auto& criterion : search_ast) {
    auto matches = _match*(criterion);
    for (auto& id : matches) {
        match_counts[id]++;
    }
}
// Keep only IDs matching all criteria
for (auto& [id, count] : match_counts) {
    if (count >= num_criteria) {
        results.push_back(id);
    }
}
```

### Inner Join Operations (`_indexed_filtered_innerjoin_over_ouid`)

Performs relational-style inner joins between indexed attributes over document IDs.

**Steps:**
1. Build ouid-to-left-attribute map using `_build_ouid_map()`
2. Filter right-table entries by prefix filter (supports `*` wildcard)
3. For each matching right entry, look up corresponding left value via ouid
4. Return pairs of (right_value, left_value)
5. Sort and deduplicate results

**Why this approach:** Enables queries like "find all versions in configuration X" by joining the version and configuration indices over document IDs.

### Index Persistence

**Open (`_open`):**
1. Check if index file exists; if not, create empty index with `_create()`
2. Read JSON from file
3. Parse into in-memory `_index` object
4. If parsing fails and `shouldAutoRebuildSearchIndex()` is true, rebuild from documents
5. Otherwise throw `runtime_error`

**Close (`_close`):**
1. If not dirty or not open, skip (return true)
2. Serialize `_index` to JSON using `JsonWriter`
3. Write to index file using `write_buffer_to_file()`
4. Return success status

**Rebuild (`_rebuild`):**
1. Create empty index with `_create()`
2. Open the new empty index
3. List all .json files in collection directory using `list_files_in_directory()`
4. Skip `index.json` itself
5. Read each document file and call `addDocument()`
6. Mark as dirty to ensure save

## Internal Functions

### Match Functions

Each `_match*` function queries a specific index category:

##### `_matchVersion(version) -> std::vector<object_id_t>`

**Brief:** Returns all document IDs with the specified version.

##### `_matchConfiguration(configuration) -> std::vector<object_id_t>`

**Brief:** Returns all document IDs in the specified configuration.

##### `_matchEntity(entity) -> std::vector<object_id_t>`

**Brief:** Returns all document IDs for the specified entity.

##### `_matchRun(run) -> std::vector<object_id_t>`

**Brief:** Returns all document IDs associated with the specified run.

##### `_matchVersionAlias(alias) -> std::vector<object_id_t>`

**Brief:** Returns all document IDs with the specified version alias.

##### `_matchObjectId(oid) -> std::vector<object_id_t>`

**Brief:** Returns the object ID itself (identity function for direct ID queries).

##### `_matchObjectIds(oids_json) -> std::vector<object_id_t>`

**Brief:** Parses a JSON object with `$in` or `$oid` fields and returns matching IDs.

### Add Functions

Each `_add*` function indexes a specific attribute:

##### `_addVersion(oid, version)`

**Brief:** Adds the oid to the version index entry, creating entry if needed.

##### `_addConfiguration(oid, configuration)`

**Brief:** Adds the oid to the configuration index entry.

##### `_addConfigurationAssigned(timestamp, configuration)`

**Brief:** Records when a configuration was assigned.

##### `_addEntity(oid, entity)`

**Brief:** Adds the oid to the entity index entry.

##### `_addVersionAlias(oid, alias)`

**Brief:** Adds the oid to the version alias index entry.

##### `_addRun(oid, run)`

**Brief:** Adds the oid to the run index entry.

##### `_addId(oid)`

**Brief:** Adds the oid to the master list of all object IDs (`ouid` array).

### Remove Functions

Each `_remove*` function removes an oid from a specific index entry:

##### `_removeVersion(oid, version)`

**Brief:** Removes the oid from the version index entry.

##### `_removeConfiguration(oid, configuration)`

**Brief:** Removes the oid from the configuration index entry.

##### `_removeConfigurationAssigned(timestamp, configuration)`

**Brief:** Removes a timestamp from a configuration's assignment list.

##### `_removeEntity(oid, entity)`

**Brief:** Removes the oid from the entity index entry.

##### `_removeVersionAlias(oid, alias)`

**Brief:** Removes the oid from the version alias index entry.

##### `_removeRun(oid, run)`

**Brief:** Removes the oid from the run index entry.

##### `_removeId(oid)`

**Brief:** Removes the oid from the master list.

### Helper Functions

##### `_build_ouid_map(map, attribute) -> void`

**Brief:** Builds a reverse map from ouid to attribute value for join operations.

**Called by:** `_indexed_filtered_innerjoin_over_ouid()`

##### `_filtered_attribute_list(attribute, prefix) -> std::vector<std::string>`

**Brief:** Returns all values of an attribute that match a prefix filter. Supports `*` wildcard at end of prefix.

**Called by:** `findEntities()`

##### `getObjectIds() -> std::vector<object_id_t>`

**Brief:** Returns all object IDs in the index from the `ouid` array.

**Called by:** `findDocumentIDs()` when search is empty (return all)

### Matching Function Dispatch

The `findDocumentIDs()` method uses a function pointer map for dispatch:

```cpp
auto matching_function_map = std::map<std::string, matching_function_t>{
    {apiliteral::filter::version, &SearchIndex::_matchVersion},
    {apiliteral::filter::configurations, &SearchIndex::_matchConfiguration},
    {apiliteral::filter::entities, &SearchIndex::_matchEntity},
    {apiliteral::filter::runs, &SearchIndex::_matchRun},
    {apiliteral::filter::version_aliases, &SearchIndex::_matchVersionAlias},
    {jsonliteral::oid, &SearchIndex::_matchObjectId},
    {jsonliteral::id, &SearchIndex::_matchObjectIds}
};
```

## Performance Considerations

- **In-memory operations:** All queries operate on the in-memory JSON structure
- **Hash map lookups:** O(1) average for attribute lookups in JSON objects
- **Intersection logic:** Uses count-based filtering for efficient multi-criteria matching
- **Lazy persistence:** Only writes to disk when modified, minimizing I/O

## Error Handling Strategy

- `confirm()` macros validate preconditions
- JSON parsing errors return empty results or false
- `std::out_of_range` exceptions are caught when index entries don't exist
- Exceptions during destructor are caught and logged but not propagated (destructor safety)

## Testing Notes

- **Unit tests:** `test/StorageProviders/FileSystemDB/provider_filedb_index_t.cc`
- **Key test cases:**
  - Multi-criteria AND search
  - Single criterion search
  - Empty search (return all)
  - Add/remove document indexing
  - Index persistence and reload
  - Index rebuild from documents

## Maintenance Notes

### Static Members

The class uses static members for path-based mutex management:
```cpp
std::map<std::string, std::unique_ptr<std::mutex>> SearchIndex::_path_mutexes;
std::mutex SearchIndex::_path_mutexes_guard;
```

These are initialized at file scope and persist for the application lifetime.

### Auto-Rebuild Feature

The `shouldAutoRebuildSearchIndex()` function uses static initialization:
```cpp
bool dbfsi::shouldAutoRebuildSearchIndex(bool rebuild) {
  static const bool _shouldAutoRebuildSearchIndex = rebuild;
  return _shouldAutoRebuildSearchIndex;
}
```

The setting is fixed after the first call, so it must be configured before any SearchIndex use.

### TRACE Logging Levels

| Level | Usage |
|-------|-------|
| 12-14 | File operations, open/close, errors |
| 15-19 | Search and modification operations |
| 20-26 | Detailed match operations |

### Debug Functions

```cpp
namespace filesystem {
namespace index {
namespace debug {
    void enable();  // Enable TRACE debugging for index operations
}}}
```

Enables all TRACE levels for the `provider_filedb_index.cpp` module.

### Index Entry Initialization

When adding to an index category, entries are initialized lazily:
```cpp
auto& version_ouid_list = versions[version];
if (version_ouid_list.type() == typeid(bool)) {
    version_ouid_list = jsn::array_t{};
}
```

This handles the case where a new key is created with default value.

## See Also

- [provider_filedb_index.h](./provider_filedb_index.h.md) - Header with class declaration
- [provider_filedb.cpp](./provider_filedb.cpp.md) - Uses SearchIndex for queries
- [provider_filedb_readwrite.cpp](./provider_filedb_readwrite.cpp.md) - Uses SearchIndex for document writes
