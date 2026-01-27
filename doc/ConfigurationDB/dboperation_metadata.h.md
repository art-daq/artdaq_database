# dboperation_metadata.h

**Path:** `artdaq-database/ConfigurationDB/dboperation_metadata.h`

**Purpose:** Declares functions for retrieving database metadata, including lists of available databases, collections, and database information. These operations provide introspection capabilities for the configuration database, enabling discovery of available resources and administrative tasks.


## Key Concepts

### Metadata Operations

Metadata operations provide information about the database structure rather than the configuration data itself:
- **List Databases**: Enumerate all available databases in the system
- **List Collections**: Enumerate all collections (configuration types) within a database
- **Read DB Info**: Get detailed information about a specific database

### Use Cases

These operations are essential for:
- **Discovery**: Finding what configuration types are available
- **Administration**: Monitoring database health and size
- **Dynamic UIs**: Building interfaces that enumerate database contents
- **Debugging**: Verifying database connectivity and structure
- **Tooling**: Automation scripts that need to discover resources

### Two-Namespace Pattern

Operations are declared in two namespaces:
- **`opts`**: Takes `ManageDocumentOperation` objects directly - for programmatic access
- **`json`**: Takes JSON string payloads - for REST API and CLI compatibility

## Thread Safety

- **Thread-safe:** Yes (read-only operations)
- **Concurrent access:** Multiple threads can safely query metadata simultaneously
- **Locking:** No internal locking needed for read-only operations

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/SharedCommon/returned_result.h` | Provides `result_t` type definition for return values |

## Forward Declarations

### `ManageDocumentOperation`

Forward-declared class from `artdaq::database::configuration` namespace that encapsulates operation parameters including collection name, query filter, output format, and storage provider selection.

## Functions

### Namespace: `opts`

Functions in this namespace accept `ManageDocumentOperation` objects containing all operation parameters.

---

#### `list_databases(options, conf) -> result_t`

**Brief:** Lists all available databases in the system. Returns a list of database names that can be accessed by the current connection.

**Parameters:**
- `options` - `ManageDocumentOperation const&` - Operation configuration object containing provider settings
- `conf` - `std::string&` - Output buffer that will receive the database list as JSON

**Preconditions:**
- The database connection must be established
- The `options` object must be properly initialized

**Returns:** `result_t` where:
- On success: `{true, conf}` where `conf` contains a JSON array of database names
- On failure: `{false, error_message}` with diagnostic information

**Postconditions:**
- On success, `conf` contains a valid JSON array
- The list reflects the current state of available databases

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Safe - read-only operation

**Side Effects:**
- Queries the storage provider for database metadata
- May trigger network operations for MongoDB provider

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_metadata.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include <iostream>

using namespace artdaq::database::configuration;

void listAllDatabases() {
  // Create options for listing databases
  ManageDocumentOperation opts{"listdatabases"};
  std::string dbList;

  // Execute the operation with error handling
  auto result = opts::list_databases(opts, dbList);
  if (result.first) {
    std::cout << "Available databases: " << dbList << std::endl;
    // Output: ["config_db", "test_db", "production_db"]
  } else {
    std::cerr << "Error listing databases: " << result.second << std::endl;
  }
}
```

---

#### `list_collections(options) -> result_t`

**Brief:** Lists all collections (configuration types) in the current database. Returns the names of all collections that contain configuration documents.

**Parameters:**
- `options` - `ManageDocumentOperation const&` - Operation configuration containing:
  - `collection`: (Optional) Prefix filter for collection names
  - `format`: Output format (gui, csv, json)

**Preconditions:**
- The database connection must be established
- A database must be selected (or use default)

**Returns:** `result_t` where:
- On success: `{true, json_array}` containing collection names
- On failure: `{false, error_message}`

**Postconditions:**
- The returned JSON array contains all matching collection names

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Safe - read-only operation

**Side Effects:**
- Queries the storage provider for collection list
- May trigger network operations for MongoDB provider

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_metadata.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include <iostream>

using namespace artdaq::database::configuration;

void listAllCollections() {
  // Create options for listing collections
  ManageDocumentOperation opts{"listcollections"};
  opts.format(options::data_format_t::gui);

  // Execute the operation
  auto result = opts::list_collections(opts);
  if (result.first) {
    std::cout << "Collections: " << result.second << std::endl;
    // Output: ["DetectorConfigs", "TriggerConfigs", "DAQConfigs", ...]
  } else {
    std::cerr << "Error listing collections: " << result.second << std::endl;
  }
}
```

---

#### `read_dbinfo(options, conf) -> result_t`

**Brief:** Retrieves detailed information about the database. Returns metadata such as database size, document counts, and other administrative information.

**Parameters:**
- `options` - `ManageDocumentOperation const&` - Operation configuration object
- `conf` - `std::string&` - Output buffer that will receive the database information as JSON

**Preconditions:**
- The database connection must be established
- Appropriate permissions to read database metadata

**Returns:** `result_t` where:
- On success: `{true, conf}` where `conf` contains a JSON object with database information
- On failure: `{false, error_message}`

**Postconditions:**
- On success, `conf` contains database metadata as JSON

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Safe - read-only operation

**Side Effects:**
- Queries the storage provider for database statistics
- May trigger network operations for MongoDB provider

**Note:** The specific fields returned may vary depending on the storage provider (FileSystemDB vs MongoDB).

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_metadata.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include <iostream>

using namespace artdaq::database::configuration;

void getDatabaseStatistics() {
  // Create options for reading database info
  ManageDocumentOperation opts{"readdbinfo"};
  std::string dbInfo;

  // Execute the operation with error handling
  auto result = opts::read_dbinfo(opts, dbInfo);
  if (result.first) {
    std::cout << "Database info: " << dbInfo << std::endl;
    // Output: {"name": "config_db", "collections": 15, "documents": 1234, ...}
  } else {
    std::cerr << "Error reading database info: " << result.second << std::endl;
  }
}
```

---

### Namespace: `json`

Functions in this namespace accept JSON string payloads for REST API compatibility.

---

#### `list_databases(task_payload) -> result_t`

**Brief:** Lists all available databases using a JSON payload to specify options. Designed for REST API and CLI integration.

**Parameters:**
- `task_payload` - `std::string const&` - JSON string containing operation parameters

**Preconditions:**
- `task_payload` must not be empty

**Returns:** `result_t` with JSON array of database names on success

**Postconditions:**
- On success, result contains JSON array of database names

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Safe - read-only operation

**Side Effects:**
- Creates internal `ManageDocumentOperation` object
- Queries the storage provider for database list

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_metadata.h"
#include <iostream>

using namespace artdaq::database::configuration;

void listDatabasesViaJson() {
  // Create JSON payload with options
  std::string payload = R"({"dataformat": "gui"})";

  // Execute the operation
  auto result = json::list_databases(payload);
  if (result.first) {
    std::cout << "Databases: " << result.second << std::endl;
  } else {
    std::cerr << "Error: " << result.second << std::endl;
  }
}
```

---

#### `list_collections(task_payload) -> result_t`

**Brief:** Lists all collections using a JSON payload to specify options. Designed for REST API and CLI integration.

**Parameters:**
- `task_payload` - `std::string const&` - JSON string containing:
  - `collection`: (Optional) Prefix filter
  - `dataformat`: Output format (gui, csv, json)

**Preconditions:**
- `task_payload` must not be empty

**Returns:** `result_t` with JSON array of collection names on success

**Postconditions:**
- On success, result contains JSON array of collection names

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Safe - read-only operation

**Side Effects:**
- Creates internal `ManageDocumentOperation` object
- Queries the storage provider for collection list

---

#### `read_dbinfo(task_payload) -> result_t`

**Brief:** Retrieves database information using a JSON payload. Designed for REST API and CLI integration.

**Parameters:**
- `task_payload` - `std::string const&` - JSON string containing operation parameters

**Preconditions:**
- `task_payload` must not be empty

**Returns:** `result_t` with database metadata as JSON on success

**Postconditions:**
- On success, result contains database metadata as JSON object

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Safe - read-only operation

**Side Effects:**
- Creates internal `ManageDocumentOperation` object
- Queries the storage provider for database statistics

---

### Namespace: `debug`

#### `Metadata() -> void`

**Brief:** Enables TRACE logging for all metadata operations. Activates verbose debug output for troubleshooting database introspection issues.

**Parameters:** None

**Preconditions:** None

**Returns:** void

**Postconditions:**
- TRACE logging is enabled for metadata operations
- Subsequent operations will produce detailed trace output

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Function does not throw |

**Thread Safety:** Not thread-safe - should be called during initialization only

**Side Effects:**
- Modifies TRACE logging configuration globally
- Calls `detail::Metadata()` to enable internal logging

---

#### `detail::Metadata() -> void`

**Brief:** Enables TRACE logging for internal detail functions. Provides more granular debug output for implementation-level troubleshooting.

**Parameters:** None

**Preconditions:** None

**Returns:** void

**Postconditions:**
- TRACE logging is enabled for detail-level metadata operations

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Function does not throw |

**Thread Safety:** Not thread-safe - should be called during initialization only

**Side Effects:**
- Modifies TRACE logging configuration globally

## Relationship to Other Components

```
dboperation_metadata.h
        |
        +-- ManageDocumentOperation (options container)
        |
        +-- detail_metadata.cpp (actual implementation)
        |
        +-- dispatch_*.h (provider-specific implementations)
        |
        +-- ConfigurationInterface (high-level API consumer)
```

- **ManageDocumentOperation**: Options class that holds parameters for metadata operations
- **ConfigurationInterface**: High-level API that uses `list_collections` internally for discovery
- **detail_metadata.cpp**: Contains the actual implementation logic that queries providers
- **dispatch_*.h**: Provider-specific implementations (FileSystemDB, MongoDB, UconDB)

## Notes for Developers

### Key Behaviors

1. **Provider-Specific Results**: The information returned may vary by storage provider:
   - FileSystemDB: Returns directory-based information
   - MongoDB: Returns MongoDB-specific database statistics
2. **Performance**: These operations may be slow on large databases with many collections
3. **Caching**: Consider caching collection lists if called frequently
4. **Read-Only**: All metadata operations are read-only and do not modify the database

### Common Pitfalls

- **Empty payload**: JSON functions require a non-empty payload even if no specific options are needed
- **Large databases**: `list_collections` on databases with many collections can be slow
- **Provider differences**: Do not assume the same fields are returned for different providers

### Anti-patterns

```cpp
// DON'T do this - calling list_collections in a tight loop
for (int i = 0; i < 1000; i++) {
  auto result = opts::list_collections(opts);  // Expensive!
  // ... process result
}

// DO this instead - cache the result
auto result = opts::list_collections(opts);
if (result.first) {
  auto collections = parseCollections(result.second);
  for (int i = 0; i < 1000; i++) {
    // ... use cached collections
  }
}
```

### Typical Usage Pattern

```cpp
#include "artdaq-database/ConfigurationDB/dboperation_metadata.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include <iostream>

using namespace artdaq::database::configuration;

void discoverDatabase() {
  // Step 1: List available collections
  ManageDocumentOperation opts{"listcollections"};
  auto result = opts::list_collections(opts);

  if (!result.first) {
    std::cerr << "Failed to list collections: " << result.second << std::endl;
    return;
  }

  std::cout << "Available configuration types:\n" << result.second << std::endl;

  // Step 2: Get database info for administration
  opts = ManageDocumentOperation{"readdbinfo"};
  std::string dbInfo;
  result = opts::read_dbinfo(opts, dbInfo);

  if (result.first) {
    std::cout << "Database statistics:\n" << dbInfo << std::endl;
  }
}
```

## See Also

- [dboperation_metadata.cpp.md](./dboperation_metadata.cpp.md) - Implementation details
- [detail_metadata.cpp.md](./detail_metadata.cpp.md) - Internal implementation
- [configurationdbifc.h.md](./configurationdbifc.h.md) - High-level API
- [options_operation_managedocument.h.md](./options_operation_managedocument.h.md) - ManageDocumentOperation class
- [returned_result.h.md](../SharedCommon/returned_result.h.md) - result_t type definition

---

**Documentation generated for artdaq-database ConfigurationDB module**
