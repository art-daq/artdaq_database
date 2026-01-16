# dispatch_mongodb.h

**Path:** `artdaq-database/ConfigurationDB/dispatch_mongodb.h`

**Purpose:** Declares the MongoDB storage provider dispatch functions that route database operations to a MongoDB backend. This provider leverages MongoDB's document database capabilities for scalable, production-ready configuration storage with full query support, replication, and concurrent access.


## Key Concepts

### MongoDB Provider

The MongoDB provider implements all database operations using MongoDB:
- **Documents:** Stored as BSON documents in MongoDB collections
- **Collections:** Native MongoDB collections with indexing support
- **Queries:** Full MongoDB query language support
- **Scalability:** Built-in support for replication and sharding

### Connection Configuration

Connection details are specified via the `ARTDAQ_DATABASE_URI` environment variable:

```
mongodb://[username:password@]host[:port]/database[?options]
```

**Examples:**
```bash
# Local MongoDB, default port
export ARTDAQ_DATABASE_URI="mongodb://localhost/artdaq_db"

# Remote MongoDB with authentication
export ARTDAQ_DATABASE_URI="mongodb://user:pass@db.example.com:27017/artdaq_db"

# Replica set connection
export ARTDAQ_DATABASE_URI="mongodb://host1,host2,host3/artdaq_db?replicaSet=rs0"
```

### Forward Declarations

The header uses forward declarations to minimize header dependencies:
- `JSONDocument` - Document type from `docrecord` namespace
- `ManageDocumentOperation` - Standard operation options class
- `ManageAliasesOperation` - Alias-specific operation options class

## Thread Safety

- **Thread-safe:** Yes, with proper connection handling
- **Concurrent access:** MongoDB driver handles connection pooling and concurrent operations
- **Locking:** Database-level locking managed by MongoDB server

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | Common utilities, macros, and type definitions |

## Functions

All functions are declared in the `artdaq::database::configuration::mongo` namespace.

### Document Operations

#### `writeDocument(ManageDocumentOperation const& options, JSONDocument const& insert_payload) -> void`

**Brief:** Inserts or updates a configuration document in a MongoDB collection.

**Parameters:**
- `options` - Operation parameters including collection name and operation type
- `insert_payload` - JSON document containing the data to write

**Preconditions:**
- `options.operation()` must be `writedocument` or `overwritedocument`
- `options.provider()` must be `mongo`
- MongoDB connection must be available

**Postconditions:**
- Document is stored in the specified collection
- If document with same ID exists, it is replaced (for overwrite) or a new version is created

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Wrong operation or provider option |
| `runtime_error` | MongoDB connection failure |
| `runtime_error` | Write operation failure |

**Thread Safety:** Thread-safe via connection pooling

**Side Effects:**
- Creates or updates a document in MongoDB
- Network I/O to MongoDB server

---

#### `readDocument(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Reads a single configuration document from MongoDB, returning the latest version if multiple matches exist.

**Parameters:**
- `options` - Operation parameters including collection and query filters
- `query_payload` - JSON query specifying which document to retrieve

**Preconditions:**
- `options.operation()` must be `readdocument`
- `options.provider()` must be `mongo`

**Returns:** The matching `JSONDocument`, or the highest-versioned match if multiple exist

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | No documents found matching query |
| `runtime_error` | Wrong operation or provider option |
| `runtime_error` | MongoDB connection failure |

**Thread Safety:** Thread-safe

---

#### `readDocuments(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Reads all configuration documents matching a query from MongoDB.

**Parameters:**
- `options` - Operation parameters including collection and query filters
- `query_payload` - JSON query specifying which documents to retrieve

**Preconditions:**
- `options.operation()` must be `readdocument`
- `options.provider()` must be `mongo`

**Returns:** Vector of matching `JSONDocument` objects (may be empty)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Wrong operation or provider option |
| `runtime_error` | MongoDB connection failure |

**Thread Safety:** Thread-safe

---

#### `markDocumentReadonly(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Updates a document's metadata to mark it as read-only, preventing future modifications.

**Parameters:**
- `options` - Operation parameters identifying the target document
- `query_payload` - JSON query specifying which document to mark

**Returns:** The updated document with readonly flag set

**Thread Safety:** Thread-safe

---

#### `markDocumentDeleted(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Updates a document's metadata to mark it as deleted (soft delete).

**Parameters:**
- `options` - Operation parameters identifying the target document
- `query_payload` - JSON query specifying which document to mark

**Returns:** The updated document with deleted flag set

**Thread Safety:** Thread-safe

---

### Version Management

#### `findVersions(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all available versions of a configuration document using MongoDB aggregation.

**Parameters:**
- `options` - Operation parameters specifying `findversions` operation
- `query_payload` - Search criteria (often unused; search based on options)

**Preconditions:**
- `options.operation()` must be `findversions`
- `options.provider()` must be `mongo`

**Returns:** Vector of JSON documents, each containing version information and a query to retrieve that version

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Wrong operation or provider option |
| `runtime_error` | Filter string format is unsupported |

**Thread Safety:** Thread-safe

---

#### `findEntities(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all unique entities in the specified collection using MongoDB's distinct query.

**Parameters:**
- `options` - Operation parameters specifying `findentities` operation
- `query_payload` - Search criteria (often unused; search based on options)

**Preconditions:**
- `options.operation()` must be `findentities`
- `options.provider()` must be `mongo`

**Returns:** Vector of JSON documents, each containing entity name and a query to retrieve it

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Wrong operation or provider option |
| `runtime_error` | Filter string format is unsupported |

**Thread Safety:** Thread-safe

---

#### `addEntity(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Adds a new entity to a document's entity list using MongoDB's $push operator.

**Parameters:**
- `options` - Operation parameters specifying the entity details
- `query_payload` - JSON document containing the entity definition

**Returns:** The updated document

**Thread Safety:** Thread-safe

---

#### `removeEntity(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Removes an entity from a document's entity list using MongoDB's $pull operator.

**Parameters:**
- `options` - Operation parameters identifying the entity to remove
- `query_payload` - JSON document specifying the target entity

**Returns:** The updated document

**Thread Safety:** Thread-safe

---

### Alias Operations

#### `addVersionAlias(ManageAliasesOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Creates an alias (human-readable name) pointing to a specific document version.

**Parameters:**
- `options` - ManageAliasesOperation specifying the alias details
- `query_payload` - JSON document identifying the target version

**Returns:** Confirmation document

**Thread Safety:** Thread-safe

---

#### `removeVersionAlias(ManageAliasesOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Removes an existing version alias.

**Parameters:**
- `options` - ManageAliasesOperation identifying the alias to remove
- `query_payload` - JSON document specifying the target alias

**Returns:** Confirmation document

**Thread Safety:** Thread-safe

---

#### `findVersionAliases(ManageAliasesOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all aliases defined for versions in the specified scope using MongoDB query. Note: Currently not implemented and throws an exception.

**Parameters:**
- `options` - ManageAliasesOperation specifying search parameters
- `query_payload` - JSON document containing filter criteria

**Returns:** Never returns normally (throws exception)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Always - function is not implemented |

**Thread Safety:** N/A (throws immediately)

---

#### `addConfigurationAlias(ManageAliasesOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Creates an alias for a global configuration.

**Parameters:**
- `options` - ManageAliasesOperation specifying the alias details
- `query_payload` - JSON document identifying the target configuration

**Returns:** Confirmation document

**Thread Safety:** Thread-safe

---

#### `removeConfigurationAlias(ManageAliasesOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Removes an existing global configuration alias.

**Parameters:**
- `options` - ManageAliasesOperation identifying the alias to remove
- `query_payload` - JSON document specifying the target alias

**Returns:** Confirmation document

**Thread Safety:** Thread-safe

---

### Global Configuration Operations

#### `findConfigurations(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all global configurations using MongoDB aggregation to extract unique configuration names.

**Parameters:**
- `options` - Operation parameters specifying `findconfigs` operation
- `query_payload` - JSON document containing search criteria

**Preconditions:**
- `options.operation()` must be `findconfigs`
- `options.provider()` must be `mongo`

**Returns:** Vector of configuration metadata documents with deduplicated names

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Wrong operation or provider option |

**Thread Safety:** Thread-safe

---

#### `findCompositionsContaining(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all global configuration compositions that contain a specific configuration type, version, and entity using MongoDB's embedded document queries.

**Parameters:**
- `options` - Operation parameters specifying `findcompositionscontaining` operation; must include collection (configuration type), version, and optionally entity
- `query_payload` - JSON document (currently unused, search based on options)

**Preconditions:**
- `options.operation()` must be `findcompositionscontaining`
- `options.provider()` must be `mongo`
- `options.collection()` (configuration type) must be provided and not empty
- `options.version()` must be provided and not empty

**Returns:** Vector of composition names that reference the specified configuration

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Wrong operation or provider option |
| `runtime_error` | Configuration type (collection) is empty or not provided |
| `runtime_error` | Version is empty or not provided |

**Thread Safety:** Thread-safe

---

#### `configurationComposition(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Retrieves the complete composition (list of all component configurations) for a global configuration.

**Parameters:**
- `options` - Operation parameters specifying `confcomposition` operation
- `query_payload` - JSON document identifying the configuration

**Preconditions:**
- `options.operation()` must be `confcomposition`
- `options.provider()` must be `mongo`

**Returns:** JSON document containing the configuration composition with format: `{ "search": [ { "name": "config:entity", "query": {...} }, ... ] }`

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Wrong operation or provider option |
| `runtime_error` | No results found for the specified configuration |
| `runtime_error` | Filter string format is unsupported |

**Thread Safety:** Thread-safe

---

#### `assignConfiguration(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Assigns a document to a global configuration using MongoDB's $addToSet operator.

**Parameters:**
- `options` - Operation parameters specifying `assignconfig` operation
- `query_payload` - JSON document identifying the target document

**Preconditions:**
- `options.operation()` must be `assignconfig`
- `options.provider()` must be `mongo`

**Returns:** Updated configuration composition

**Postconditions:**
- Document is updated in MongoDB with the new configuration association
- Returns the updated composition for verification

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Wrong operation or provider option |

**Thread Safety:** Not atomic (performs read-modify-write)

---

#### `removeConfiguration(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Removes a document assignment from a global configuration using MongoDB's $pull operator.

**Parameters:**
- `options` - Operation parameters specifying `removeconfig` operation
- `query_payload` - JSON document identifying the target document

**Preconditions:**
- `options.operation()` must be `removeconfig`
- `options.provider()` must be `mongo`

**Returns:** Updated configuration composition

**Postconditions:**
- Document is updated in MongoDB with the configuration association removed

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Wrong operation or provider option |

**Thread Safety:** Not atomic (performs read-modify-write)

---

### Metadata Operations

#### `searchCollection(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Performs a full-text or structured search within a collection using MongoDB's query capabilities.

**Parameters:**
- `options` - Operation parameters specifying `searchcollection` operation
- `query_payload` - JSON document containing the search query

**Preconditions:**
- `options.operation()` must be `searchcollection`
- `options.provider()` must be `mongo`

**Returns:** Vector of matching documents

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Wrong operation or provider option |

**Thread Safety:** Thread-safe

---

#### `listCollections(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Lists all collections in the database using MongoDB's listCollections command.

**Parameters:**
- `options` - Operation parameters specifying `listcollections` operation
- `query_payload` - JSON document containing optional filter criteria

**Preconditions:**
- `options.operation()` must be `listcollections`
- `options.provider()` must be `mongo`

**Returns:** Vector of collection metadata documents

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Wrong operation or provider option |

**Thread Safety:** Thread-safe

---

#### `listDatabases(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Lists all available databases using MongoDB's listDatabases command.

**Parameters:**
- `options` - Operation parameters specifying `listdatabases` operation
- `query_payload` - JSON document containing optional filter criteria

**Preconditions:**
- `options.operation()` must be `listdatabases`
- `options.provider()` must be `mongo`

**Returns:** Vector of database metadata documents

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Wrong operation or provider option |

**Thread Safety:** Thread-safe

---

#### `readDbInfo(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> JSONDocument`

**Brief:** Reads database-level information and statistics using MongoDB's dbStats command.

**Parameters:**
- `options` - Operation parameters specifying `readdbinfo` operation
- `query_payload` - JSON document containing optional query parameters

**Preconditions:**
- `options.operation()` must be `readdbinfo`
- `options.provider()` must be `mongo`

**Returns:** JSON document containing database metadata including size, document counts, and index information; returns empty JSON if no metadata found

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Wrong operation or provider option |

**Thread Safety:** Thread-safe

---

### Debug Namespace

#### `debug::MongoDB() -> void`

**Brief:** Enables TRACE debugging for MongoDB operations by configuring trace output modes.

**Purpose:** Configure TRACE logging infrastructure for debugging MongoDB storage operations.

**Postconditions:**
- TRACE name set to "dispatch_mongodb.cpp"
- All trace levels enabled (0xFFFFFFFFFFFFFFFFLL)
- Memory and slow modes configured

**Thread Safety:** Not thread-safe (modifies global TRACE state)

---

## Usage Example

```cpp
#include "artdaq-database/ConfigurationDB/dispatch_mongodb.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include <iostream>

namespace mongo = artdaq::database::configuration::mongo;
namespace cf = artdaq::database::configuration;
using artdaq::database::docrecord::JSONDocument;

void queryDocumentsExample() {
  // Create operation options
  cf::ManageDocumentOperation opts("myapp");
  opts.operation(apiliteral::operation::readdocument);
  opts.provider(apiliteral::provider::mongo);
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
    auto docs = mongo::readDocuments(opts, query);
    std::cout << "Found " << docs.size() << " documents\n";
    for (const auto& doc : docs) {
      std::cout << doc << "\n";
    }
  } catch (const std::runtime_error& e) {
    std::cerr << "Query failed: " << e.what() << "\n";
  }
}

void findCompositionsExample() {
  // Find all global configurations containing a specific component
  cf::ManageDocumentOperation opts("myapp");
  opts.operation(apiliteral::operation::findcompositionscontaining);
  opts.provider(apiliteral::provider::mongo);
  opts.collection("ComponentConfig");  // configuration type
  opts.version("v1.0");
  opts.entity("subsystem01");  // optional

  try {
    auto compositions = mongo::findCompositionsContaining(opts, JSONDocument{"{}"});
    std::cout << "Found " << compositions.size() << " compositions\n";
    for (const auto& comp : compositions) {
      std::cout << "Composition: " << comp << "\n";
    }
  } catch (const std::runtime_error& e) {
    std::cerr << "Search failed: " << e.what() << "\n";
  }
}
```

## Relationship to Other Components

- **dispatch_common.h:** Aggregates this header with other provider headers
- **dispatch_mongodb.cpp:** Contains the implementations of these functions
- **StorageProviders/MongoDB/provider_mongodb.h:** Low-level MongoDB driver wrapper
- **detail_*.cpp:** Calls these functions based on provider selection
- **ManageDocumentOperation:** Provides operation parameters

## Comparison with Other Providers

| Feature | MongoDB | FileSystemDB | UconDB |
|---------|---------|--------------|--------|
| Scalability | Excellent | Limited | Good |
| Concurrent access | Full support | File-level locking | HTTP-based |
| Complex queries | MongoDB query language | Limited | Limited |
| Full-text search | Supported | Not implemented | Not implemented |
| Dependencies | MongoDB server | None | HTTP server |
| Human-readable storage | No (BSON) | Yes (JSON files) | No |
| Setup complexity | Higher | Lower | Medium |
| findCompositionsContaining | Implemented | Implemented | Not implemented |

## Notes for Developers

### Connection Pooling

The MongoDB provider uses connection pooling internally:
- Connections are reused across operations
- Pool size is configurable via connection URI options
- Idle connections are automatically closed

### Query Translation

JSON queries are translated to MongoDB BSON queries:
- Filter fields map directly to MongoDB query operators
- Complex queries using MongoDB operators are supported
- Index optimization applies automatically

### Index Recommendations

For optimal performance, create indexes on commonly queried fields:
```javascript
db.collection.createIndex({"entities.name": 1})
db.collection.createIndex({"version": 1})
db.collection.createIndex({"configurations.name": 1})
```

### Error Handling

MongoDB-specific errors are wrapped in `runtime_error`:
- Connection failures include host/port information
- Query errors include the failing query
- Timeout errors are distinguishable from other failures

### Common Pitfalls

- **Pitfall 1:** Connection string format - Ensure the URI follows MongoDB connection string format exactly
- **Pitfall 2:** Authentication - Check that credentials are correct and the user has appropriate permissions
- **Pitfall 3:** Network timeouts - Adjust connection timeout for slow networks: `?connectTimeoutMS=10000`
- **Pitfall 4:** Write concern - Consider using `?w=majority` for critical data to ensure durability
- **Pitfall 5:** Unimplemented functions - `findVersionAliases()` throws "not implemented" exception

### Anti-patterns

```cpp
// DON'T: Ignore provider validation errors
try {
  mongo::readDocument(opts, query);  // opts.provider() might not be "mongo"
} catch (...) {
  // Swallowing exceptions hides configuration errors
}

// DO: Validate provider before calling
if (opts.provider() == apiliteral::provider::mongo) {
  try {
    auto doc = mongo::readDocument(opts, query);
    // Process document
  } catch (const std::runtime_error& e) {
    std::cerr << "MongoDB error: " << e.what() << "\n";
  }
}
```

## See Also

- [dispatch_mongodb.cpp](./dispatch_mongodb.cpp.md) - Implementation of these functions
- [dispatch_filedb.h](./dispatch_filedb.h.md) - FileSystemDB provider with similar interface
- [dispatch_ucondb.h](./dispatch_ucondb.h.md) - UconDB provider with similar interface
- [dispatch_common.h](./dispatch_common.h.md) - Aggregation header for all providers
- [StorageProviders/MongoDB/provider_mongodb.h](../StorageProviders/MongoDB/provider_mongodb.h.md) - Low-level driver wrapper
- [options_operations.h](./options_operations.h.md) - Operation options classes
- [External: MongoDB C++ Driver](https://mongocxx.org/) - mongocxx driver documentation

---

**Documentation generated for artdaq-database ConfigurationDB module**
