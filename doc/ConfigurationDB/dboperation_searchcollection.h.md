# dboperation_searchcollection.h

**Path:** `artdaq-database/ConfigurationDB/dboperation_searchcollection.h`

**Purpose:** Declares the generic collection search function that allows querying documents within a collection using flexible filter criteria. This provides a lower-level search interface compared to the specialized find_* functions, enabling arbitrary queries against the configuration database.


## Key Concepts

### Collection Search

The search_collection operation provides a generic query interface for finding documents in the configuration database:
- Accepts arbitrary filter criteria as JSON
- Returns matching documents based on provider-specific query logic
- More flexible than specialized find_versions/find_entities operations
- Supports both structured options objects and JSON string payloads

### Query Filters

Filters can match on various document attributes:
- Version identifiers
- Entity names
- Custom metadata fields
- Date ranges
- Combinations of the above using logical operators

### Two-Namespace Pattern

This header follows the ConfigurationDB two-namespace pattern:
- **opts namespace**: Takes `ManageDocumentOperation` objects directly for programmatic use
- **json namespace**: Takes JSON string payloads for REST API compatibility

## Thread Safety

- **Thread-safe:** Conditional
- **Concurrent access:** Multiple threads can call search functions concurrently with different options
- **Locking:** No internal locking; thread safety depends on the underlying storage provider

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/SharedCommon/returned_result.h` | Provides `result_t` type for operation results |

## Forward Declarations

### `ManageDocumentOperation`

Forward-declared class from `artdaq::database::configuration` namespace that encapsulates search operation parameters including collection name, query filter, output format, and storage provider selection.

## Functions

### Namespace: `opts`

---

#### `search_collection(options, results) -> result_t`

**Brief:** Searches for documents in a collection matching the specified filter criteria and populates the results string with matching documents.

**Parameters:**
- `options` - `ManageDocumentOperation const&` - Search configuration object containing:
  - `collection` - Name of the collection to search
  - `queryFilter` - JSON filter criteria for matching documents
  - `format` - Output format (gui, csv, json, etc.)
  - `provider` - Storage provider to use (mongo, filesystem, ucon)
- `results` - `std::string&` - Output buffer that receives the search results

**Preconditions:**
- The `options` object must have a valid collection name set
- The storage provider specified must be available and configured

**Returns:** `result_t` - A pair where:
- `first` (bool): `true` if the search completed successfully
- `second` (string): On success, contains the same data as `results`; on failure, contains an error message

**Postconditions:**
- On success, `results` contains JSON-formatted search results
- On failure, `results` may be empty or contain partial data

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Function is `noexcept`; all exceptions are caught and returned as failures |

**Thread Safety:** Conditional - safe for concurrent calls with different options objects

**Side Effects:**
- Performs database queries through the configured storage provider
- May trigger network operations for MongoDB or UconDB providers

**Complexity:** O(n) where n is the number of documents matching the filter

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_searchcollection.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include <iostream>

using namespace artdaq::database::configuration;

void searchConfigurations() {
  // Create search options
  ManageDocumentOperation opts{"searchcollection"};
  opts.collection("DAQConfigs");
  opts.queryFilter(R"({"version": "v1.0"})");
  opts.format(options::data_format_t::gui);

  // Execute search with error handling
  std::string results;
  auto result = opts::search_collection(opts, results);

  if (result.first) {
    std::cout << "Search results: " << results << std::endl;
  } else {
    std::cerr << "Search failed: " << result.second << std::endl;
  }
}
```

---

### Namespace: `json`

---

#### `search_collection(task_payload) -> result_t`

**Brief:** JSON-based search interface that parses the query payload and executes a collection search, designed for REST API compatibility.

**Parameters:**
- `task_payload` - `std::string const&` - JSON string containing search parameters including:
  - `collection` - Name of the collection to search
  - `filter` - Query filter criteria
  - `format` - Output format

**Preconditions:**
- `task_payload` must be a valid, non-empty JSON string
- The JSON must contain required fields for the search operation

**Returns:** `result_t` - A pair where:
- `first` (bool): `true` if the search completed successfully
- `second` (string): On success, contains JSON search results; on failure, contains an error message

**Postconditions:**
- On success, the result contains JSON-formatted matching documents
- On failure, returns a descriptive error message

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Function is `noexcept`; all exceptions are caught and returned as failures |

**Thread Safety:** Conditional - safe for concurrent calls

**Side Effects:**
- Creates internal `ManageDocumentOperation` object
- Performs database queries through the configured storage provider
- May trigger network operations for MongoDB or UconDB providers

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_searchcollection.h"
#include <iostream>

using namespace artdaq::database::configuration;

void searchViaJson() {
  // Create JSON payload with search parameters
  std::string payload = R"({
    "operation": "searchcollection",
    "collection": "DAQConfigs",
    "filter": {"version": "v1.0"},
    "format": "gui"
  })";

  // Execute search
  auto result = json::search_collection(payload);

  if (result.first) {
    std::cout << "Found: " << result.second << std::endl;
  } else {
    std::cerr << "Error: " << result.second << std::endl;
  }
}
```

---

### Namespace: `debug`

---

#### `SearchCollection() -> void`

**Brief:** Enables TRACE logging for the search collection module at the public API level.

**Parameters:** None

**Preconditions:** None

**Returns:** void

**Postconditions:**
- TRACE logging is enabled for search collection operations
- Subsequent search operations will produce detailed trace output

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Function does not throw |

**Thread Safety:** Not thread-safe - should be called once at initialization

**Side Effects:**
- Modifies TRACE logging configuration globally

---

#### `detail::SearchCollection() -> void`

**Brief:** Enables TRACE logging for the internal detail implementation of search collection operations.

**Parameters:** None

**Preconditions:** None

**Returns:** void

**Postconditions:**
- TRACE logging is enabled for internal search implementation

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Function does not throw |

**Thread Safety:** Not thread-safe - should be called once at initialization

**Side Effects:**
- Modifies TRACE logging configuration globally

---

## Relationship to Other Components

```
dboperation_searchcollection.h
        |
        +-- ManageDocumentOperation (options/parameter container)
        |
        +-- dboperation_searchcollection.cpp (public wrapper implementation)
        |
        +-- detail_searchcollection.cpp (internal implementation logic)
        |
        +-- dispatch_mongodb.h / dispatch_filedb.h (provider-specific search)
        |
        +-- configuration_dbproviders.h (provider dispatch mechanism)
```

- **ManageDocumentOperation**: Provides the options/parameter container for search operations
- **dboperation_searchcollection.cpp**: Contains the implementation of public functions
- **detail_searchcollection.cpp**: Contains internal implementation logic called by the public functions
- **dispatch_mongodb.h / dispatch_filedb.h**: Provider-specific search implementations
- **configuration_dbproviders.h**: Provider dispatch mechanism

## Notes for Developers

### Common Pitfalls

- **Empty Filter**: Providing an empty query payload to `json::search_collection` returns a failure with `msg_EmptyFilter`
- **Provider Mismatch**: Ensure the storage provider is correctly configured before calling search functions
- **Format Compatibility**: Not all output formats are supported; check the implementation for valid format options

### Anti-patterns

```cpp
// DON'T do this: Not checking return value
std::string results;
opts::search_collection(opts, results);
std::cout << results;  // May be empty on failure

// DO this instead: Always check the result
auto result = opts::search_collection(opts, results);
if (result.first) {
  std::cout << results;
} else {
  std::cerr << "Search failed: " << result.second << std::endl;
}
```

### Performance Considerations

1. **Index Usage**: Ensure appropriate indexes exist in MongoDB for common query patterns
2. **Result Set Size**: Large result sets may impact performance; consider using pagination
3. **Filter Complexity**: Complex filters may require more processing time

### Typical Usage Pattern

```cpp
#include "artdaq-database/ConfigurationDB/dboperation_searchcollection.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include <iostream>

using namespace artdaq::database::configuration;

void performSearch() {
  // Step 1: Create and configure options
  ManageDocumentOperation opts{"searchcollection"};
  opts.collection("DetectorConfigs");
  opts.queryFilter(R"({"entity": "Detector_01", "version": {"$gte": "v2.0"}})");
  opts.format(options::data_format_t::json);

  // Step 2: Execute search with proper error handling
  std::string results;
  auto result = opts::search_collection(opts, results);

  if (!result.first) {
    std::cerr << "Search failed: " << result.second << std::endl;
    return;
  }

  // Step 3: Process results
  std::cout << "Found matching configurations:\n" << results << std::endl;
}
```

## See Also

- [dboperation_searchcollection.cpp.md](./dboperation_searchcollection.cpp.md) - Implementation details
- [detail_searchcollection.cpp.md](./detail_searchcollection.cpp.md) - Internal implementation
- [dboperation_managedocument.h.md](./dboperation_managedocument.h.md) - Document read/write operations
- [options_operation_managedocument.h.md](./options_operation_managedocument.h.md) - ManageDocumentOperation class definition
- [configuration_dbproviders.h.md](./configuration_dbproviders.h.md) - Storage provider dispatch
- [returned_result.h.md](../SharedCommon/returned_result.h.md) - Result type definition

---

**Documentation generated for artdaq-database ConfigurationDB module**
