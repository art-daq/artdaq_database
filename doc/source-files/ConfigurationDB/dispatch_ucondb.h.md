# dispatch_ucondb.h

**Path:** `artdaq-database/ConfigurationDB/dispatch_ucondb.h`

**Purpose:** Declares the UconDB-specific dispatch functions for configuration database operations. UconDB (Microservices Configuration Database) is a REST API-based storage provider. This header provides the interface for routing database operations to the UconDB backend, following the same API pattern as MongoDB and FileSystemDB dispatch layers.


## Key Concepts

### UconDB Storage Provider

UconDB is a web-service-based configuration storage system that provides:
- REST API access to configuration data
- Remote storage capabilities over HTTP/HTTPS
- Integration with existing web infrastructure

### Dispatch Pattern

The dispatch functions in this header serve as an abstraction layer between the high-level ConfigurationDB API and the low-level UconDB provider. Each function:
1. Validates operation parameters
2. Delegates to the appropriate UconDB provider method
3. Transforms results into the standardized JSON format

### Namespace Organization

The functions are organized in the `artdaq::database::configuration::ucon` namespace:
- `artdaq::database` - Root database namespace
- `configuration` - Configuration management subsystem
- `ucon` - UconDB-specific implementations

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Each function creates its own provider instance; concurrent calls from different threads should be safe but are not explicitly synchronized
- **Locking:** No internal locking; relies on provider implementation

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | Common definitions, types, and TRACE macros |

## Forward Declarations

The header forward-declares the following classes to minimize include dependencies:

| Class | Namespace | Purpose |
|-------|-----------|---------|
| `JSONDocument` | `artdaq::database::docrecord` | Core document type for JSON data |
| `ManageDocumentOperation` | `artdaq::database::configuration` | Operation options for document management |
| `ManageAliasesOperation` | `artdaq::database::configuration` | Operation options for alias management |

## Functions

### Document Operations

#### `writeDocument(ManageDocumentOperation const& options, JSONDocument const& insert_payload) -> void`

**Brief:** Writes a JSON document to UconDB storage, creating a new document or updating an existing one.

**Parameters:**
- `options` - Operation options specifying write mode and target collection
- `insert_payload` - JSON document containing the data to store

**Preconditions:**
- `options.operation()` must be `writedocument` or `overwritedocument`
- `options.provider()` must be `ucon`

**Postconditions:**
- Document is persisted to UconDB

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Invalid operation or provider |
| `runtime_error` | Network/connection failure |

**Thread Safety:** Not thread-safe

**Side Effects:**
- Network I/O to UconDB server

---

#### `readDocument(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Reads a single JSON document from UconDB matching the query criteria, returning the highest version if multiple matches exist.

**Parameters:**
- `options` - Operation options specifying read parameters
- `query_payload` - JSON document containing search/filter criteria

**Returns:** The matching JSONDocument with the highest version.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | No documents found |
| `runtime_error` | Invalid operation or provider |

**Thread Safety:** Not thread-safe

---

#### `readDocuments(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Reads all JSON documents from UconDB matching the query criteria.

**Parameters:**
- `options` - Operation options specifying read parameters
- `query_payload` - JSON document containing search/filter criteria

**Preconditions:**
- `options.operation()` must be `readdocument`
- `options.provider()` must be `ucon`

**Returns:** Vector of all matching JSONDocument objects.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Invalid operation or provider |

**Thread Safety:** Not thread-safe

---

#### `markDocumentReadonly(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Marks a document as read-only in UconDB, preventing further modifications.

**Parameters:**
- `options` - Operation options identifying the target document
- `query_payload` - JSON document specifying which document to mark

**Returns:** Updated JSONDocument with read-only status.

**Thread Safety:** Not thread-safe

---

#### `markDocumentDeleted(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Marks a document as deleted in UconDB (soft delete).

**Parameters:**
- `options` - Operation options identifying the target document
- `query_payload` - JSON document specifying which document to mark

**Returns:** Updated JSONDocument with deleted status.

**Thread Safety:** Not thread-safe

---

### Entity Operations

#### `findEntities(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all entities matching the specified criteria in UconDB.

**Parameters:**
- `options` - Operation options specifying `findentities` operation and `ucon` provider
- `query_payload` - JSON document containing filter criteria (often unused; search based on options)

**Preconditions:**
- `options.operation()` must be `findentities`
- `options.provider()` must be `ucon`

**Returns:** Vector of JSON documents, each containing an entity name and its query.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Invalid operation or provider |
| `runtime_error` | Filter string format is unsupported |

**Thread Safety:** Not thread-safe

---

#### `addEntity(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Adds a new entity to the configuration in UconDB.

**Parameters:**
- `options` - Operation options specifying the entity details
- `query_payload` - JSON document containing the entity definition

**Returns:** JSONDocument confirming the entity addition.

**Thread Safety:** Not thread-safe

---

#### `removeEntity(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Removes an entity from the configuration in UconDB.

**Parameters:**
- `options` - Operation options identifying the entity to remove
- `query_payload` - JSON document specifying the target entity

**Returns:** JSONDocument confirming the entity removal.

**Thread Safety:** Not thread-safe

---

### Version Operations

#### `findVersions(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all versions of documents matching the specified criteria in UconDB.

**Parameters:**
- `options` - Operation options specifying `findversions` operation and `ucon` provider
- `query_payload` - JSON document containing filter criteria (often unused; search based on options)

**Preconditions:**
- `options.operation()` must be `findversions`
- `options.provider()` must be `ucon`

**Returns:** Vector of JSON documents, each containing a version name and its query.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Invalid operation or provider |
| `runtime_error` | Filter string format is unsupported |

**Thread Safety:** Not thread-safe

---

#### `addVersionAlias(ManageAliasesOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Adds an alias (human-readable name) for a specific version in UconDB.

**Parameters:**
- `options` - ManageAliasesOperation specifying the alias details
- `query_payload` - JSON document identifying the target version

**Returns:** JSONDocument confirming the alias creation.

**Thread Safety:** Not thread-safe

---

#### `removeVersionAlias(ManageAliasesOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Removes a version alias from UconDB.

**Parameters:**
- `options` - ManageAliasesOperation identifying the alias to remove
- `query_payload` - JSON document specifying the target alias

**Returns:** JSONDocument confirming the alias removal.

**Thread Safety:** Not thread-safe

---

#### `findVersionAliases(ManageAliasesOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>` [NOT IMPLEMENTED]

**Brief:** Finds all version aliases matching the specified criteria. Currently not implemented for UconDB.

**Parameters:**
- `options` - ManageAliasesOperation specifying search parameters
- `query_payload` - JSON document containing filter criteria

**Returns:** Never returns normally (throws exception).

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Always - function is not implemented |

**Thread Safety:** N/A (throws immediately)

---

### Configuration Operations

#### `findConfigurations(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all unique configurations matching the search criteria in UconDB.

**Parameters:**
- `options` - Operation options specifying `findconfigs` operation and `ucon` provider
- `query_payload` - JSON document containing filter criteria

**Preconditions:**
- `options.operation()` must be `findconfigs`
- `options.provider()` must be `ucon`

**Returns:** Vector of JSON documents, each containing a configuration name and its query.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Invalid operation or provider |

**Thread Safety:** Not thread-safe

---

#### `findCompositionsContaining(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>` [NOT IMPLEMENTED]

**Brief:** Finds global configurations containing a specific component. Currently not implemented for UconDB.

**Parameters:**
- `options` - Operation options specifying search parameters
- `query_payload` - JSON document containing filter criteria

**Returns:** Never returns normally (throws exception).

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Always - function is not implemented for UconDB |

**Thread Safety:** N/A (throws immediately)

---

#### `configurationComposition(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Retrieves the composition of a configuration, showing all entities and their relationships.

**Parameters:**
- `options` - Operation options specifying `confcomposition` operation and `ucon` provider
- `query_payload` - JSON document specifying the configuration

**Preconditions:**
- `options.operation()` must be `confcomposition`
- `options.provider()` must be `ucon`

**Returns:** JSON document containing the configuration composition with format: `{ "search": [ { "name": "config:entity", "query": {...} }, ... ] }`

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Invalid operation or provider |
| `runtime_error` | No search filters were found |
| `runtime_error` | Filter string format is unsupported |

**Thread Safety:** Not thread-safe

---

#### `assignConfiguration(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Assigns a document to a configuration in UconDB.

**Parameters:**
- `options` - Operation options specifying `assignconfig` operation and `ucon` provider
- `query_payload` - JSON document identifying the target document

**Preconditions:**
- `options.operation()` must be `assignconfig`
- `options.provider()` must be `ucon`

**Returns:** Updated configuration composition after the assignment.

**Thread Safety:** Not thread-safe (performs read-modify-write)

---

#### `removeConfiguration(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Removes a configuration assignment from a document in UconDB.

**Parameters:**
- `options` - Operation options specifying `removeconfig` operation and `ucon` provider
- `query_payload` - JSON document identifying the target document

**Preconditions:**
- `options.operation()` must be `removeconfig`
- `options.provider()` must be `ucon`

**Returns:** Updated configuration composition after the removal.

**Thread Safety:** Not thread-safe (performs read-modify-write)

---

#### `addConfigurationAlias(ManageAliasesOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Adds an alias for a configuration in UconDB.

**Parameters:**
- `options` - ManageAliasesOperation specifying the alias details
- `query_payload` - JSON document identifying the target configuration

**Returns:** JSONDocument confirming the alias creation.

**Thread Safety:** Not thread-safe

---

#### `removeConfigurationAlias(ManageAliasesOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Removes a configuration alias from UconDB.

**Parameters:**
- `options` - ManageAliasesOperation identifying the alias to remove
- `query_payload` - JSON document specifying the target alias

**Returns:** JSONDocument confirming the alias removal.

**Thread Safety:** Not thread-safe

---

### Collection and Database Operations

#### `searchCollection(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>` [NOT IMPLEMENTED]

**Brief:** Performs a general search query against a UconDB collection. Currently not implemented.

**Parameters:**
- `options` - Operation options specifying search parameters
- `query_payload` - JSON document containing the search query

**Returns:** Never returns normally (throws exception).

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Always - function is not implemented |

**Thread Safety:** N/A (throws immediately)

---

#### `listCollections(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Lists all collections available in UconDB.

**Parameters:**
- `options` - Operation options specifying `listcollections` operation and `ucon` provider
- `query_payload` - JSON document containing optional filters

**Preconditions:**
- `options.operation()` must be `listcollections`
- `options.provider()` must be `ucon`

**Returns:** Vector of JSON documents, each containing a collection name.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Invalid operation or provider |

**Thread Safety:** Not thread-safe

---

#### `listDatabases(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Lists all databases available in the UconDB server.

**Parameters:**
- `options` - Operation options specifying `listdatabases` operation and `ucon` provider
- `query_payload` - JSON document containing optional filters

**Preconditions:**
- `options.operation()` must be `listdatabases`
- `options.provider()` must be `ucon`

**Returns:** Vector of JSON documents, each containing a database name.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Invalid operation or provider |

**Thread Safety:** Not thread-safe

---

#### `readDbInfo(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Reads database metadata and information from UconDB.

**Parameters:**
- `options` - Operation options specifying `readdbinfo` operation and `ucon` provider
- `query_payload` - JSON document containing query parameters

**Preconditions:**
- `options.operation()` must be `readdbinfo`
- `options.provider()` must be `ucon`

**Returns:** JSON document containing database metadata wrapped in a "search" object.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Invalid operation or provider |

**Thread Safety:** Not thread-safe

---

### Debug Functions

#### `debug::UconDB() -> void`

**Brief:** Enables TRACE debugging for all UconDB dispatch operations by setting trace levels and modes.

**Postconditions:**
- TRACE name set to "dispatch_ucondb.cpp"
- All trace levels enabled

**Thread Safety:** Not thread-safe (modifies global TRACE state)

---

## Implementation Status

The following functions are **not implemented** and will throw `runtime_error` when called:

| Function | Status | Notes |
|----------|--------|-------|
| `findVersionAliases()` | Not implemented | Version alias search not supported |
| `searchCollection()` | Not implemented | Generic collection search not supported |
| `findCompositionsContaining()` | Not implemented | Composition search not supported |

## Usage Example

```cpp
#include "artdaq-database/ConfigurationDB/dispatch_ucondb.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include <iostream>

namespace ucon = artdaq::database::configuration::ucon;
namespace cf = artdaq::database::configuration;
using artdaq::database::docrecord::JSONDocument;

void readFromUconDB() {
  // Create operation options
  cf::ManageDocumentOperation opts("myapp");
  opts.operation(apiliteral::operation::readdocument);
  opts.provider(apiliteral::provider::ucon);
  opts.collection("DetectorConfigs");

  // Create search query
  auto query = JSONDocument(R"({
    "filter": {
      "entities.name": "MainDetector",
      "version": "v1.0"
    },
    "collection": "DetectorConfigs"
  })");

  // Execute query with error handling
  try {
    auto doc = ucon::readDocument(opts, query);
    std::cout << "Found document: " << doc << "\n";
  } catch (const std::runtime_error& e) {
    std::cerr << "Read failed: " << e.what() << "\n";
    // Check if it's a network error, authentication error, etc.
  }
}

void listAvailableCollections() {
  cf::ManageDocumentOperation opts("myapp");
  opts.operation(apiliteral::operation::listcollections);
  opts.provider(apiliteral::provider::ucon);

  try {
    auto collections = ucon::listCollections(opts, JSONDocument{"{}"});
    std::cout << "Available collections:\n";
    for (const auto& coll : collections) {
      std::cout << "  - " << coll << "\n";
    }
  } catch (const std::runtime_error& e) {
    std::cerr << "Failed to list collections: " << e.what() << "\n";
  }
}
```

## Relationship to Other Components

- **dispatch_mongodb.h** - Equivalent interface for MongoDB backend
- **dispatch_filedb.h** - Equivalent interface for FileSystemDB backend
- **dispatch_common.h** - Common dispatch utilities shared across providers
- **options_operations.h** - Defines `ManageDocumentOperation` and `ManageAliasesOperation`
- **StorageProviders/UconDB/provider_ucondb.h** - Low-level UconDB provider implementation

## Comparison with Other Providers

| Feature | UconDB | MongoDB | FileSystemDB |
|---------|--------|---------|--------------|
| Connection type | HTTP/HTTPS | TCP | Filesystem |
| Setup complexity | Medium | Higher | Lower |
| Dependencies | HTTP server | MongoDB server | None |
| findVersionAliases | Not implemented | Not implemented | Implemented |
| searchCollection | Not implemented | Implemented | Implemented |
| findCompositionsContaining | Not implemented | Implemented | Implemented |

## Notes for Developers

### Network Dependency

Unlike FileSystemDB, UconDB requires network connectivity:
- Handle connection failures gracefully
- Consider timeout settings for slow networks
- Implement retry logic for transient failures

### API Consistency

This header maintains API consistency with `dispatch_mongodb.h` and `dispatch_filedb.h`. When adding new operations, ensure they are implemented consistently across all three dispatch headers.

### Common Pitfalls

- **Network dependency:** Unlike FileSystemDB, UconDB requires network connectivity. Handle connection failures gracefully.
- **Unimplemented functions:** Check the implementation status before using functions marked as not implemented.
- **Provider validation:** All functions validate the provider option; passing the wrong provider will throw an exception.
- **Non-atomic operations:** `assignConfiguration` and `removeConfiguration` perform read-modify-write operations that are not atomic.

### Anti-patterns

```cpp
// DON'T: Assume all functions are implemented
try {
  auto results = ucon::searchCollection(opts, query);  // Will throw!
} catch (...) {
  // Too late - should have checked implementation status first
}

// DO: Check implementation status or use implemented alternatives
if (opts.provider() == apiliteral::provider::ucon) {
  // searchCollection is not implemented for UconDB
  // Use findConfigurations or listCollections instead
  auto configs = ucon::findConfigurations(opts, query);
}
```

## See Also

- [dispatch_ucondb.cpp](./dispatch_ucondb.cpp.md) - Implementation file
- [dispatch_mongodb.h](./dispatch_mongodb.h.md) - MongoDB equivalent
- [dispatch_filedb.h](./dispatch_filedb.h.md) - FileSystemDB equivalent
- [StorageProviders/UconDB/provider_ucondb.h](../StorageProviders/UconDB/provider_ucondb.h.md) - UconDB provider

---

**Documentation generated for artdaq-database ConfigurationDB module**
