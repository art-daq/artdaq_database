# provider_filedb_index.h

## File Overview

This header file defines the SearchIndex class that provides fast document lookup capabilities for the FileSystemDB provider. The index maintains mappings between searchable attributes (version, configuration, entity, run) and document object IDs, enabling efficient queries without scanning all files.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h`

**Lines of Code**: 123

**Purpose**: Fast indexed search for filesystem-based document storage

## Dependencies

### Project Headers
- `"artdaq-database/DataFormats/Json/json_common.h"` - JSON data structures
- `"artdaq-database/StorageProviders/common.h"` - Common utilities
- `"artdaq-database/StorageProviders/storage_providers.h"` - Provider interface

### Third-Party Libraries
- `<boost/filesystem.hpp>` - Filesystem path handling

## Namespace: artdaq::database::filesystem::index

## Class: SearchIndex

### Purpose

The SearchIndex class implements an in-memory inverted index that maps searchable attributes to document IDs. Each collection maintains an `index.json` file that is loaded into memory for fast queries.

### Constructor and Destructor

```cpp
SearchIndex(boost::filesystem::path const& path);
~SearchIndex();
```

**Constructor Parameters**:
- `path` - Path to the index file (typically `collection/index.json`)

**Behavior**:
- Opens or creates index file
- Loads index into memory
- Constructor opens, destructor auto-saves if dirty

### Deleted Operations

```cpp
SearchIndex() = delete;
SearchIndex& operator=(SearchIndex const&) = delete;
SearchIndex& operator=(SearchIndex&&) = delete;
SearchIndex(SearchIndex&&) = delete;
```

**Design**: Non-copyable, non-movable to prevent accidental index duplication.

---

## Public Search Methods

### findDocumentIDs
```cpp
std::vector<object_id_t> findDocumentIDs(JSONDocument const& search);
```

**Purpose**: Find document IDs matching search criteria.

**Parameters**:
- `search` - JSON search criteria (e.g., `{"version": "v1.0", "entities": "TPC"}`)

**Returns**: Vector of matching document object IDs

**Algorithm**:
- Parses search JSON into criteria
- For each criterion, finds matching document IDs
- Returns intersection of all matches (AND logic)

**Example**:
```cpp
JSONDocument search("{\"version\": \"v1.0\", \"entities\": \"TPC_01\"}");
auto ids = index.findDocumentIDs(search);
// Returns: ["507f1f77bcf86cd799439011", ...]
```

---

### findAllGlobalConfigurations
```cpp
std::vector<std::pair<std::string, std::string>>
findAllGlobalConfigurations(JSONDocument const& search);
```

**Purpose**: Find all global configurations and their entities.

**Returns**: Vector of pairs: `(configuration_name, entity_name)`

**Usage**: Discover what configurations exist and what entities they contain.

---

### findVersionsByGlobalConfigName
```cpp
std::vector<std::pair<std::string, std::string>>
findVersionsByGlobalConfigName(JSONDocument const& search);
```

**Purpose**: Find all versions within a specific configuration.

**Parameters**:
- `search` - Must contain `configurations` field

**Returns**: Pairs of `(version, entity_name)`

---

### findVersionsByEntityName
```cpp
std::vector<std::pair<std::string, std::string>>
findVersionsByEntityName(JSONDocument const& search);
```

**Purpose**: Find all versions of a specific entity.

**Parameters**:
- `search` - Must contain `entities` field

**Returns**: Pairs of `(version, entity_name)`

---

### findEntities
```cpp
std::vector<std::string> findEntities(JSONDocument const& search);
```

**Purpose**: Find all entity names matching criteria.

**Returns**: Vector of entity names

---

## Public Modification Methods

### addDocument
```cpp
bool addDocument(JSONDocument const& doc, object_id_t const& oid);
```

**Purpose**: Add a document to the index.

**Parameters**:
- `doc` - Document containing indexable fields
- `oid` - Document's object ID

**Returns**: true if successful

**Side Effect**: Marks index as dirty (will be saved on destruction)

---

### removeDocument
```cpp
bool removeDocument(JSONDocument const& doc, object_id_t const& oid);
```

**Purpose**: Remove a document from the index.

**Parameters**:
- `doc` - Document to remove
- `oid` - Document's object ID

**Returns**: true if successful

---

### getConfigurationAssignedTimestamps
```cpp
std::vector<std::string> getConfigurationAssignedTimestamps(JSONDocument const& search);
```

**Purpose**: Get all timestamps when a configuration was assigned.

**Returns**: Vector of ISO 8601 timestamp strings

---

## Private Index Management Methods

### Addition Methods

```cpp
void _addVersion(object_id_t const& oid, std::string const& version);
void _addId(object_id_t const& oid);
void _addConfiguration(object_id_t const& oid, std::string const& config);
void _addConfigurationAssigned(timestamp_t const& ts, std::string const& config);
void _addEntity(object_id_t const& oid, std::string const& entity);
void _addVersionAlias(object_id_t const& oid, std::string const& alias);
void _addRun(object_id_t const& oid, std::string const& run);
```

**Purpose**: Add mappings to specific index attributes.

### Removal Methods

```cpp
void _removeVersion(object_id_t const& oid, std::string const& version);
void _removeId(object_id_t const& oid);
void _removeConfiguration(object_id_t const& oid, std::string const& config);
void _removeConfigurationAssigned(timestamp_t const& ts, std::string const& config);
void _removeEntity(object_id_t const& oid, std::string const& entity);
void _removeVersionAlias(object_id_t const& oid, std::string const& alias);
void _removeRun(object_id_t const& oid, std::string const& run);
```

**Purpose**: Remove mappings from specific index attributes.

### Matching Methods

```cpp
std::vector<object_id_t> _matchVersion(std::string const& version) const;
std::vector<object_id_t> _matchVersionAlias(std::string const& alias) const;
std::vector<object_id_t> _matchConfiguration(std::string const& config) const;
std::vector<object_id_t> _matchEntity(std::string const& entity) const;
std::vector<object_id_t> _matchObjectId(std::string const& oid) const;
std::vector<object_id_t> _matchObjectIds(std::string const& oid) const;
std::vector<object_id_t> _matchRun(std::string const& run) const;
```

**Purpose**: Find document IDs matching a specific attribute value.

---

## Private Utility Methods

### getObjectIds
```cpp
std::vector<object_id_t> getObjectIds() const;
```

**Purpose**: Get all document IDs in the index (for empty searches).

---

### _build_ouid_map
```cpp
void _build_ouid_map(std::map<std::string, std::string>& map,
                    std::string const& attribute) const;
```

**Purpose**: Build a map from attribute values to object IDs.

---

### _make_unique_sorted (Template)
```cpp
template <typename TYPE>
void _make_unique_sorted(jsn::array_t& ouids) const;
```

**Purpose**: Remove duplicates and sort an array of values.

**Implementation**:
```cpp
// 1. Extract values into vector
// 2. Sort
// 3. Remove duplicates with std::unique
// 4. Put back into JSON array
```

---

### _indexed_filtered_innerjoin_over_ouid
```cpp
std::vector<std::pair<std::string, std::string>>
_indexed_filtered_innerjoin_over_ouid(std::string const& attr1,
                                      std::string const& attr2,
                                      std::string const& filter) const;
```

**Purpose**: Perform an inner join over two indexed attributes.

**Example**: Join versions and configurations by object ID to find all version/configuration pairs.

---

### _filtered_attribute_list
```cpp
std::vector<std::string>
_filtered_attribute_list(std::string const& attribute,
                        std::string const& attribute_begins_with) const;
```

**Purpose**: Get all values of an attribute that begin with a prefix.

---

## Private File I/O Methods

```cpp
bool _open(boost::filesystem::path const& path);
bool _create(boost::filesystem::path const& path);
bool _close();
bool _rebuild(boost::filesystem::path const& path);
```

**File Operations**:
- `_open`: Load existing index from file
- `_create`: Create new empty index
- `_close`: Save index to file if dirty
- `_rebuild`: Rebuild index by scanning all documents

---

## Private Members

```cpp
private:
  object_t _index;                    // JSON object holding the index
  boost::filesystem::path _path;      // Path to index file
  bool _isDirty;                      // True if index needs saving
  bool _isOpen;                       // True if index is loaded
```

---

## Index Structure

The index is stored as a JSON object with this structure:

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
  "runs": {
    "12345": ["507f1f77bcf86cd799439011"]
  },
  "objectids": ["507f1f77bcf86cd799439011", "507f1f77bcf86cd799439012", ...]
}
```

**Index Attributes**:
- `version`: Maps version strings to document IDs
- `entities`: Maps entity names to document IDs
- `configurations`: Maps configuration names to document IDs
- `runs`: Maps run numbers to document IDs
- `objectids`: List of all document IDs

---

## Global Functions

### shouldAutoRebuildSearchIndex
```cpp
bool shouldAutoRebuildSearchIndex(bool enable = false);
```

**Purpose**: Enable/disable automatic index rebuilding.

**Parameters**:
- `enable` - true to enable auto-rebuild, false to disable

**Returns**: Current setting

**Usage**: Call with true to enable automatic index rebuilds when index is corrupted.

---

## Debug Functions

```cpp
namespace debug {
    void enable();
}
```

**Purpose**: Enable TRACE debugging for index operations.

---

## Usage Example

```cpp
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h"

// Open index
boost::filesystem::path index_path = "/data/configs/MyCollection/index.json";
SearchIndex index(index_path);

// Search for documents
JSONDocument search("{\"version\": \"v1.0\", \"entities\": \"TPC_01\"}");
auto ids = index.findDocumentIDs(search);

// Add new document to index
JSONDocument doc("{\"version\": \"v2.0\", \"entities\": \"TPC_01\"}");
object_id_t new_id = generate_oid();
index.addDocument(doc, new_id);
// Index auto-saved on destruction
```

---

## Performance Characteristics

**Time Complexity**:
- `findDocumentIDs`: O(k * log n) where k = number of criteria, n = index size
- `addDocument`: O(1) average
- `removeDocument`: O(1) average

**Space Complexity**: O(n * m) where n = documents, m = average attributes per document

**Optimization**: Index is kept in memory for fast queries, only written to disk when dirty.

---

## Thread Safety

**Not Thread-Safe**: Multiple concurrent access requires external synchronization.

**Recommendations**:
- Use reader-writer lock for concurrent reads
- Exclusive lock for writes

---

## Best Practices

```cpp
// GOOD: RAII - index auto-saves
{
    SearchIndex index(path);
    index.addDocument(doc, oid);
} // Saves here

// GOOD: Check for errors
try {
    SearchIndex index(path);
} catch (std::exception const& e) {
    // Handle corrupted index
}

// BAD: Don't keep index open too long
// SearchIndex global_index(path);  // Held open entire program
```

---

## Related Files

- **provider_filedb_index.cpp** - Implementation
- **provider_filedb.cpp** - Uses SearchIndex for queries
- **provider_filedb_readwrite.cpp** - Updates index on write operations

---

**Documentation generated for artdaq-database FileSystemDB provider**
