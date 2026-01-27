# dboperation_managedocument.h

**Path:** `artdaq-database/ConfigurationDB/dboperation_managedocument.h`

**Purpose:** Declares the core document management functions for the configuration database. These operations handle reading, writing, versioning, and metadata management for individual configuration documents. This is the primary low-level interface for CRUD operations on configuration documents.


## Key Concepts

### Document Operations

This header provides the primary interface for working with configuration documents:
- **Read/Write**: Load and store configuration data in the database
- **Versioning**: Find available versions of configurations
- **Entity Management**: Add/remove entity associations to configuration versions
- **Protection**: Mark documents as read-only or soft-deleted

### Document Structure

Each document stored in the database contains:
- **Configuration data**: The actual configuration content (JSON, FHiCL, or XML format)
- **Metadata**: Version identifier, entity name, timestamps, and status flags
- **Object identifier**: A unique database ID (auto-generated)

### Two-Namespace Pattern

Operations are declared in two namespaces:
- **`opts`**: Takes `ManageDocumentOperation` objects directly - used for programmatic access
- **`json`**: Takes JSON string payloads - used for REST API and CLI compatibility

### Result Type

All functions return `result_t`, which is a `std::pair<bool, std::string>`:
- `first`: Success flag (`true` = success, `false` = failure)
- `second`: On success, contains the result data; on failure, contains the error message

## Thread Safety

- **Thread-safe:** Conditional
- **Concurrent access:** Each function call is independent; concurrent calls to the same document may result in race conditions
- **Locking:** No internal locking; callers must provide external synchronization if needed

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/SharedCommon/returned_result.h` | Provides `result_t` type definition for return values |

## Functions

### Namespace: opts

Functions in this namespace accept `ManageDocumentOperation` objects containing all operation parameters.

---

#### `write_document(options, conf) -> result_t`

**Brief:** Writes a configuration document to the database. Creates a new document or updates an existing one based on the version and entity specified in the options.

**Parameters:**
- `options` - A `ManageDocumentOperation` object containing operation parameters:
  - `collection`: Target collection name
  - `version`: Version identifier for the document
  - `entity`: Entity name associated with the document
  - `format`: Data format (json, fhicl, xml)
- `conf` - Configuration data to write; may be modified to include generated metadata on output

**Preconditions:**
- The `options` object must have collection, version, and entity set
- The `conf` string must contain valid configuration data in the specified format
- The target document must not be marked as read-only (if updating)

**Returns:** `result_t` where:
- On success: `{true, conf}` with updated configuration data including any generated metadata
- On failure: `{false, error_message}` with diagnostic information

**Postconditions:**
- On success, the document is persisted in the database
- The document version, entity, and collection form a unique key

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional - not safe for concurrent writes to the same document

**Side Effects:**
- Creates or modifies a document in the database storage
- Updates document timestamps

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_managedocument.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include <iostream>

namespace cf = artdaq::database::configuration;

void storeDocument() {
  // Create operation options
  cf::ManageDocumentOperation opts{"writedocument"};
  opts.collection("DetectorConfigs");
  opts.version("v1.0");
  opts.entity("Component1");

  // Configuration data to store
  std::string config = R"({"threshold": 100, "enabled": true})";

  // Perform write operation
  auto result = cf::opts::write_document(opts, config);
  if (result.first) {
    std::cout << "Document written successfully" << std::endl;
    std::cout << "Result: " << result.second << std::endl;
  } else {
    std::cerr << "Write failed: " << result.second << std::endl;
  }
}
```

---

#### `read_document(options, conf) -> result_t`

**Brief:** Reads a configuration document from the database by version and entity. Retrieves the configuration data matching the specified search criteria.

**Parameters:**
- `options` - A `ManageDocumentOperation` object containing query criteria:
  - `collection`: Collection to query
  - `version`: Version to retrieve (or alias name)
  - `entity`: Entity name
  - `format`: Desired output format
- `conf` - Output buffer that will receive the configuration data

**Preconditions:**
- The `options` object must specify collection, version, and entity
- A document matching the criteria must exist in the database

**Returns:** `result_t` where:
- On success: `{true, conf}` with the configuration data
- On failure: `{false, error_message}` if document not found or other error

**Postconditions:**
- On success, `conf` contains the requested configuration data in the specified format

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Safe - read-only operation

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_managedocument.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include <iostream>

namespace cf = artdaq::database::configuration;

void readDocument() {
  cf::ManageDocumentOperation opts{"readdocument"};
  opts.collection("DetectorConfigs");
  opts.version("v1.0");
  opts.entity("Component1");

  std::string config;
  auto result = cf::opts::read_document(opts, config);
  if (result.first) {
    std::cout << "Configuration data: " << result.second << std::endl;
  } else {
    std::cerr << "Read failed: " << result.second << std::endl;
  }
}
```

---

#### `mark_document_readonly(options) -> result_t`

**Brief:** Marks a document as read-only to prevent any further modifications. This is a protection mechanism to preserve important configuration versions.

**Parameters:**
- `options` - A `ManageDocumentOperation` object specifying the target document:
  - `collection`: Collection containing the document
  - `version`: Version of the document
  - `entity`: Entity name

**Preconditions:**
- The specified document must exist
- The document must not already be marked as deleted

**Returns:** `result_t` where:
- On success: `{true, confirmation_message}`
- On failure: `{false, error_message}`

**Postconditions:**
- On success, the document cannot be modified or deleted
- Future write operations to this document will fail

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional - race conditions possible with concurrent modifications

**Side Effects:**
- Modifies the document's metadata to set read-only flag

---

#### `mark_document_deleted(options) -> result_t`

**Brief:** Marks a document as deleted (soft delete). The document is hidden from normal queries but not physically removed from storage. This allows for potential recovery.

**Parameters:**
- `options` - A `ManageDocumentOperation` object specifying the target document:
  - `collection`: Collection containing the document
  - `version`: Version of the document
  - `entity`: Entity name

**Preconditions:**
- The specified document must exist
- The document must not be marked as read-only

**Returns:** `result_t` where:
- On success: `{true, confirmation_message}`
- On failure: `{false, error_message}`

**Postconditions:**
- On success, the document is hidden from normal queries
- The document can potentially be recovered by an administrator

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional - race conditions possible with concurrent modifications

**Side Effects:**
- Modifies the document's metadata to set deleted flag

---

#### `find_versions(options) -> result_t`

**Brief:** Finds all available versions of a configuration for a given entity and collection. Returns a list of version identifiers that can be used to retrieve specific documents.

**Parameters:**
- `options` - A `ManageDocumentOperation` object specifying the search criteria:
  - `collection`: Collection to search
  - `entity`: (Optional) Filter by entity name
  - `format`: Output format

**Preconditions:**
- The `options` object must specify at least the collection name

**Returns:** `result_t` where:
- On success: `{true, json_array}` containing version identifiers
- On failure: `{false, error_message}`

**Postconditions:**
- The returned JSON array contains all matching version identifiers

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Safe - read-only operation

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_managedocument.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include <iostream>

namespace cf = artdaq::database::configuration;

void listVersions() {
  cf::ManageDocumentOperation opts{"findversions"};
  opts.collection("DetectorConfigs");
  opts.entity("Component1");

  auto result = cf::opts::find_versions(opts);
  if (result.first) {
    std::cout << "Available versions: " << result.second << std::endl;
    // Output: ["v1.0", "v1.1", "v2.0"]
  } else {
    std::cerr << "Error: " << result.second << std::endl;
  }
}
```

---

#### `find_entities(options) -> result_t`

**Brief:** Finds all entities that have configurations in a specific collection. Optionally filters by version to find entities with a particular configuration version.

**Parameters:**
- `options` - A `ManageDocumentOperation` object specifying the search criteria:
  - `collection`: Collection to search
  - `version`: (Optional) Filter by version
  - `format`: Output format

**Preconditions:**
- The `options` object must specify at least the collection name

**Returns:** `result_t` where:
- On success: `{true, json_array}` containing entity names
- On failure: `{false, error_message}`

**Postconditions:**
- The returned JSON array contains all matching entity names

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Safe - read-only operation

---

#### `add_entity(options) -> result_t`

**Brief:** Adds an entity association to an existing configuration version. This allows the same configuration to be shared among multiple entities.

**Parameters:**
- `options` - A `ManageDocumentOperation` object specifying:
  - `collection`: Collection containing the configuration
  - `version`: Version to associate
  - `entity`: New entity name to add

**Preconditions:**
- The configuration version must exist in the collection
- The entity must not already be associated with this version

**Returns:** `result_t` where:
- On success: `{true, confirmation_message}`
- On failure: `{false, error_message}`

**Postconditions:**
- On success, the entity is associated with the configuration version
- The entity can now retrieve this configuration version

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional - race conditions possible with concurrent modifications

**Side Effects:**
- Modifies document metadata to add entity association

---

#### `remove_entity(options) -> result_t`

**Brief:** Removes an entity association from a configuration version. The configuration itself is not deleted, only the association with the specified entity.

**Parameters:**
- `options` - A `ManageDocumentOperation` object specifying:
  - `collection`: Collection containing the configuration
  - `version`: Version to disassociate
  - `entity`: Entity name to remove

**Preconditions:**
- The entity association must exist

**Returns:** `result_t` where:
- On success: `{true, confirmation_message}`
- On failure: `{false, error_message}`

**Postconditions:**
- On success, the entity is no longer associated with the configuration version

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional - race conditions possible with concurrent modifications

**Side Effects:**
- Modifies document metadata to remove entity association

---

### Namespace: json

Functions in this namespace accept JSON string payloads for REST API compatibility.

---

#### `write_document(task_payload, conf) -> result_t`

**Brief:** Writes a configuration document using a JSON payload to specify options. This is the REST API-compatible version of the write operation.

**Parameters:**
- `task_payload` - JSON string containing operation parameters (collection, version, entity, format)
- `conf` - Configuration data to write

**Preconditions:**
- `task_payload` must be a valid JSON object with required fields
- `conf` must contain valid configuration data
- `task_payload` must not be empty

**Returns:** `result_t` where:
- On success: `{true, result_data}`
- On failure: `{false, error_message}`

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_managedocument.h"
#include <iostream>

namespace cf = artdaq::database::configuration;

void writeViaJson() {
  std::string payload = R"({
    "collection": "DetectorConfigs",
    "version": "v1.0",
    "entity": "Component1",
    "dataformat": "json"
  })";

  std::string config = R"({"threshold": 100, "enabled": true})";

  auto result = cf::json::write_document(payload, config);
  if (result.first) {
    std::cout << "Document written successfully" << std::endl;
  } else {
    std::cerr << "Error: " << result.second << std::endl;
  }
}
```

---

#### `read_document(task_payload, conf) -> result_t`

**Brief:** Reads a configuration document using a JSON payload to specify query criteria. This is the REST API-compatible version of the read operation.

**Parameters:**
- `task_payload` - JSON string containing query parameters
- `conf` - Output buffer for configuration data

**Preconditions:**
- `task_payload` must be a valid JSON object with required fields
- `task_payload` must not be empty

**Returns:** `result_t` where:
- On success: `{true, conf}` with configuration data
- On failure: `{false, error_message}`

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Safe - read-only operation

---

#### `mark_document_readonly(task_payload) -> result_t`

**Brief:** Marks a document as read-only using a JSON payload to specify the target document.

**Parameters:**
- `task_payload` - JSON string specifying the document (collection, version, entity)

**Preconditions:**
- `task_payload` must be a valid JSON object
- `task_payload` must not be empty

**Returns:** `result_t` indicating success or failure

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional

---

#### `mark_document_deleted(task_payload) -> result_t`

**Brief:** Marks a document as deleted using a JSON payload to specify the target document.

**Parameters:**
- `task_payload` - JSON string specifying the document (collection, version, entity)

**Returns:** `result_t` indicating success or failure

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional

---

#### `find_versions(task_payload) -> result_t`

**Brief:** Finds all versions using a JSON payload to specify search criteria.

**Parameters:**
- `task_payload` - JSON string containing search parameters

**Preconditions:**
- `task_payload` must not be empty

**Returns:** `result_t` with JSON array of versions on success

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Safe - read-only operation

---

#### `find_entities(task_payload) -> result_t`

**Brief:** Finds all entities using a JSON payload to specify search criteria.

**Parameters:**
- `task_payload` - JSON string containing search parameters

**Preconditions:**
- `task_payload` must not be empty

**Returns:** `result_t` with JSON array of entity names on success

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Safe - read-only operation

---

#### `add_entity(task_payload) -> result_t`

**Brief:** Adds an entity association using a JSON payload to specify the operation.

**Parameters:**
- `task_payload` - JSON string specifying collection, version, and new entity

**Preconditions:**
- `task_payload` must not be empty

**Returns:** `result_t` indicating success or failure

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional

---

#### `remove_entity(task_payload) -> result_t`

**Brief:** Removes an entity association using a JSON payload to specify the operation.

**Parameters:**
- `task_payload` - JSON string specifying collection, version, and entity to remove

**Preconditions:**
- `task_payload` must not be empty

**Returns:** `result_t` indicating success or failure

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional

---

### Namespace: debug

#### `ManageDocuments() -> void`

**Brief:** Enables TRACE logging for all document management operations. Activates verbose debug output for troubleshooting document read/write issues.

**Thread Safety:** Not thread-safe - should be called during initialization only

**Side Effects:**
- Configures TRACE logging system
- Sets all trace levels to maximum verbosity

---

#### `detail::ManageDocuments() -> void`

**Brief:** Enables TRACE logging for the internal detail functions. Provides more granular debug output than the main function.

**Thread Safety:** Not thread-safe - should be called during initialization only

## Relationship to Other Components

```
                    +-----------------------+
                    |   ConfigurationInterface   |
                    +-----------+-----------+
                                |
                                v
+---------------------------+---+---+---------------------------+
|  opts::write_document()   |       |   json::write_document()  |
|  opts::read_document()    |       |   json::read_document()   |
|  ...                      |       |   ...                     |
+-------------+-------------+       +-------------+-------------+
              |                                   |
              +----------------+------------------+
                               |
                               v
              +----------------+----------------+
              |   detail::write_document()      |
              |   detail::read_document()       |
              |   ...                           |
              +----------------+----------------+
                               |
                               v
              +----------------+----------------+
              |       Storage Providers         |
              |  (FileSystemDB, MongoDB, UconDB)|
              +---------------------------------+
```

- **ManageDocumentOperation**: Options class that holds all parameters for document operations (defined in `options_operations.h`)
- **ConfigurationInterface**: High-level API that uses these functions internally
- **detail_managedocument.cpp**: Contains the actual implementation logic that dispatches to storage providers
- **dispatch_*.h**: Provider-specific implementations (FileSystemDB, MongoDB, UconDB)

## Notes for Developers

### Key Behaviors

1. **Version Uniqueness**: The combination of version + entity + collection must be unique within the database
2. **Format Conversion**: Data is automatically converted between formats based on the format option
3. **Read-Only Check**: Writing to read-only documents will fail with an error
4. **Soft Delete**: Deleted documents are hidden but not physically removed; they can be recovered
5. **Error Handling**: All functions are `noexcept` and return errors via `result_t`

### Common Pitfalls

- **Empty payloads**: JSON functions will fail if `task_payload` is empty (returns `msg_EmptyFilter`)
- **Missing options**: Ensure all required options are set before calling operations
- **Concurrent writes**: Multiple threads writing to the same document can cause race conditions
- **Format mismatch**: Ensure the data format matches the format option

### Anti-patterns

```cpp
// DON'T do this - no error handling
auto result = cf::opts::write_document(opts, data);
// Immediately use data without checking result

// DO this instead - always check result
auto result = cf::opts::write_document(opts, data);
if (!result.first) {
  std::cerr << "Write failed: " << result.second << std::endl;
  return;
}
// Now safe to use data
```

```cpp
// DON'T do this - empty payload
std::string payload = "";
auto result = cf::json::write_document(payload, config);  // Will fail with msg_EmptyFilter

// DO this instead - ensure payload is valid JSON
std::string payload = R"({"collection": "MyConfigs", "version": "v1"})";
auto result = cf::json::write_document(payload, config);
```

## See Also

- [dboperation_managedocument.cpp.md](./dboperation_managedocument.cpp.md) - Implementation details
- [options_operations.h.md](./options_operations.h.md) - ManageDocumentOperation class
- [detail_managedocument.cpp.md](./detail_managedocument.cpp.md) - Internal implementation
- [configurationdbifc.h.md](./configurationdbifc.h.md) - High-level API that uses these functions
- [returned_result.h.md](../SharedCommon/returned_result.h.md) - result_t type definition

---

**Documentation generated for artdaq-database ConfigurationDB module**
