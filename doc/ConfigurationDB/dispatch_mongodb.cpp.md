# dispatch_mongodb.cpp

**Path:** `artdaq-database/ConfigurationDB/dispatch_mongodb.cpp`

**Implements:** [dispatch_mongodb.h](./dispatch_mongodb.h.md)

**Purpose:** Provides the MongoDB-specific implementation of the database operation dispatch layer. This file routes configuration database operations to the MongoDB storage provider, implementing document CRUD operations, configuration management, and metadata queries against a MongoDB backend.

## Implementation Overview

This implementation file provides concrete MongoDB-specific logic for all database operations declared in `dispatch_mongodb.h`. Each function:
1. Validates that the operation type and provider match expected values
2. Creates a MongoDB database connection using `DBI::DBConfig` and `DBI::DB::create()`
3. Instantiates a `DBProvider<JSONDocument>` to execute the actual database operation
4. Processes results (often using regex to extract structured data from JSON filter strings)
5. Returns results in a standardized JSON format

The implementation uses the TRACE logging system extensively for debugging, with trace levels ranging from 25-30 for different operations.

## Key Algorithms

### Document Version Selection

When `readDocument()` retrieves multiple documents, it selects the highest version:

**Steps:**
1. Call `readDocuments()` to get all matching documents
2. If multiple documents returned, use `std::max_element` with `docrec::compareDocumentVersions`
3. Swap the selected document into the return value

**Why this approach:** Ensures that the most recent version is returned when multiple versions exist for the same entity.

### Configuration Name Extraction

The `findConfigurations()` function extracts unique configuration names:

**Steps:**
1. Query MongoDB for all configurations matching the search payload
2. Maintain a `seenValues` list initialized with `apiliteral::notprovided`
3. For each result, extract the configuration name from the filter child
4. Use the `isNew` lambda to deduplicate names
5. Format each unique configuration as a JSON object with name and query

**Why this approach:** Deduplicates configurations to avoid returning the same configuration multiple times when multiple versions exist.

### Regex-Based Filter Parsing

Several functions use regex to parse MongoDB filter JSON strings:

**Example pattern for `configurationComposition()`:**
```cpp
"\"configurations\\.name\"\\s*:\\s*\"((\\\\\"|[^\"])*)\"\\,"
"\\s*\"entities\\.name\"\\s*:\\s*\"((\\\\\"|[^\"])*)\"
```

This extracts configuration and entity names from filter JSON like:
```json
{"configurations.name": "MyConfig", "entities.name": "component01"}
```

### findCompositionsContaining Algorithm

The `findCompositionsContaining()` function uses MongoDB aggregation to find global configurations:

**Steps:**
1. Validate operation type, provider, configuration type, and version
2. Build a MongoDB match query for `configurations.<type>.version` and optionally `configurations.<type>.entity`
3. Create an aggregation pipeline with `$match` and `$project` stages
4. Execute the search against the `SystemConfiguration` collection
5. Extract composition names from the results

**Why this approach:** Uses MongoDB's native aggregation for efficient querying of nested document structures.

## Thread Safety

- **Thread-safe:** No (each function creates its own database connection)
- **Concurrent access:** Safe when called from multiple threads (each call is independent)
- **Locking:** No internal locking; relies on MongoDB's concurrency control

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | Common definitions and TRACE macros |
| `artdaq-database/ConfigurationDB/dispatch_mongodb.h` | Function declarations for this implementation |
| `artdaq-database/ConfigurationDB/options_operations.h` | `ManageDocumentOperation` and `ManageAliasesOperation` classes |
| `artdaq-database/BasicTypes/basictypes.h` | Basic type definitions |
| `artdaq-database/DataFormats/shared_literals.h` | JSON and API literal constants |
| `artdaq-database/JsonDocument/JSONDocument.h` | `JSONDocument` class for JSON manipulation |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | Builder pattern for constructing documents |
| `artdaq-database/SharedCommon/helper_functions.h` | Utility functions like `replace_all()` |
| `artdaq-database/StorageProviders/MongoDB/provider_mongodb.h` | MongoDB provider implementation |

## Internal Functions

### Connection Pattern

All functions follow this connection pattern:
```cpp
auto config = DBI::DBConfig{};
auto database = DBI::DB::create(config);
auto provider = DBI::DBProvider<JSONDocument>::create(database);
```

**Called by:** All dispatch functions

**Purpose:** Creates a fresh MongoDB connection for each operation, ensuring isolation between operations.

## Functions

### `writeDocument(options, insert_payload) -> void`

**Brief:** Writes a JSON document to MongoDB, either creating a new document or overwriting an existing one depending on the operation type.

**Parameters:**
- `options` - Operation options specifying `writedocument` or `overwritedocument` operation and `mongo` provider
- `insert_payload` - JSON document containing the data to write

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::writedocument` or `apiliteral::operation::overwritedocument`
- `options.provider()` must be `apiliteral::provider::mongo`

**Postconditions:**
- Document is persisted to MongoDB
- Object ID is logged at TRACE level 25

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `writedocument` or `overwritedocument` |
| `runtime_error` | Provider option is not `mongo` |

**Thread Safety:** Not thread-safe. Each call creates its own database connection.

**Side Effects:**
- Creates or updates a document in MongoDB
- Logs to TRACE at level 25

---

### `readDocument(options, search_payload) -> JSONDocument`

**Brief:** Reads a single JSON document from MongoDB matching the search criteria, returning the highest version if multiple matches exist.

**Parameters:**
- `options` - Operation options (must specify `readdocument` operation)
- `search_payload` - JSON document containing search/filter criteria

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::readdocument`
- `options.provider()` must be `apiliteral::provider::mongo`

**Returns:** The matching `JSONDocument` with the highest version, or throws if no documents found.

**Postconditions:**
- Returns exactly one document (the highest version if multiple exist)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | No documents found matching the search criteria |

**Thread Safety:** Not thread-safe.

**Side Effects:**
- Logs to TRACE at level 26

---

### `readDocuments(options, search_payload) -> std::vector<JSONDocument>`

**Brief:** Reads all JSON documents from MongoDB matching the search criteria, returning them as a vector.

**Parameters:**
- `options` - Operation options specifying `readdocument` operation and `mongo` provider
- `search_payload` - JSON document containing search/filter criteria

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::readdocument`
- `options.provider()` must be `apiliteral::provider::mongo`

**Returns:** Vector of all matching `JSONDocument` objects (may be empty).

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `readdocument` |
| `runtime_error` | Provider option is not `mongo` |

**Thread Safety:** Not thread-safe.

**Side Effects:**
- Logs to TRACE at level 26

---

### `findConfigurations(options, search_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all unique configuration names in MongoDB matching the search criteria, returning deduplicated results.

**Parameters:**
- `options` - Operation options specifying `findconfigs` operation and `mongo` provider
- `search_payload` - JSON document containing search criteria

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::findconfigs`
- `options.provider()` must be `apiliteral::provider::mongo`

**Returns:** Vector of JSON documents, each containing a unique configuration name and its query.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `findconfigs` |
| `runtime_error` | Provider option is not `mongo` |

**Thread Safety:** Not thread-safe.

**Side Effects:**
- Logs to TRACE at levels 27, 28, 30

---

### `configurationComposition(options, search_payload) -> JSONDocument`

**Brief:** Retrieves the composition of a configuration, showing all entities and their relationships within the specified configuration.

**Parameters:**
- `options` - Operation options specifying `confcomposition` operation and `mongo` provider
- `search_payload` - JSON document identifying the configuration

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::confcomposition`
- `options.provider()` must be `apiliteral::provider::mongo`

**Returns:** JSON document containing a "search" array with configuration composition details.

**Postconditions:**
- Returns a document with format: `{ "search": [ { "name": "config:entity", "query": {...} }, ... ] }`

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `confcomposition` |
| `runtime_error` | Provider option is not `mongo` |
| `runtime_error` | No results found for the specified configuration |
| `runtime_error` | Filter string format is unsupported (regex mismatch) |

**Thread Safety:** Not thread-safe.

**Side Effects:**
- Logs to TRACE at level 28

---

### `findVersions(options, query_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all versions of documents matching the specified criteria in MongoDB.

**Parameters:**
- `options` - Operation options specifying `findversions` operation and `mongo` provider
- `query_payload` - Unused parameter (search is based on options)

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::findversions`
- `options.provider()` must be `apiliteral::provider::mongo`

**Returns:** Vector of JSON documents, each containing a version name and its query.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `findversions` |
| `runtime_error` | Provider option is not `mongo` |
| `runtime_error` | Filter string format is unsupported |

**Thread Safety:** Not thread-safe.

**Side Effects:**
- Logs to TRACE at levels 28, 30

---

### `findEntities(options, query_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all entities matching the specified criteria in MongoDB.

**Parameters:**
- `options` - Operation options specifying `findentities` operation and `mongo` provider
- `query_payload` - Unused parameter (search is based on options)

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::findentities`
- `options.provider()` must be `apiliteral::provider::mongo`

**Returns:** Vector of JSON documents, each containing an entity name and its query.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `findentities` |
| `runtime_error` | Provider option is not `mongo` |
| `runtime_error` | Filter string format is unsupported |

**Thread Safety:** Not thread-safe.

**Side Effects:**
- Logs to TRACE at levels 28, 29, 30

---

### `assignConfiguration(options, search_payload) -> JSONDocument`

**Brief:** Assigns a document to a configuration by reading the document, adding the configuration association, and writing it back to MongoDB.

**Parameters:**
- `options` - Operation options specifying `assignconfig` operation and `mongo` provider
- `search_payload` - JSON document identifying the target document

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::assignconfig`
- `options.provider()` must be `apiliteral::provider::mongo`

**Returns:** Updated configuration composition after the assignment.

**Postconditions:**
- Document is updated in MongoDB with the new configuration association
- Returns the updated composition for verification

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `assignconfig` |
| `runtime_error` | Provider option is not `mongo` |

**Thread Safety:** Not thread-safe. Performs read-modify-write which is not atomic.

**Side Effects:**
- Modifies document in MongoDB
- Logs to TRACE at level 30

---

### `removeConfiguration(options, search_payload) -> JSONDocument`

**Brief:** Removes a configuration association from a document by reading the document, removing the configuration, and writing it back to MongoDB.

**Parameters:**
- `options` - Operation options specifying `removeconfig` operation and `mongo` provider
- `search_payload` - JSON document identifying the target document

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::removeconfig`
- `options.provider()` must be `apiliteral::provider::mongo`

**Returns:** Updated configuration composition after the removal.

**Postconditions:**
- Document is updated in MongoDB with the configuration association removed
- Returns the updated composition for verification

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `removeconfig` |
| `runtime_error` | Provider option is not `mongo` |

**Thread Safety:** Not thread-safe. Performs read-modify-write which is not atomic.

**Side Effects:**
- Modifies document in MongoDB
- Logs to TRACE at level 30

---

### `listCollections(options, search_payload) -> std::vector<JSONDocument>`

**Brief:** Lists all collections in the MongoDB database matching the search criteria.

**Parameters:**
- `options` - Operation options specifying `listcollections` operation and `mongo` provider
- `search_payload` - JSON document containing optional filter criteria

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::listcollections`
- `options.provider()` must be `apiliteral::provider::mongo`

**Returns:** Vector of JSON documents, each containing a collection name and its query.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `listcollections` |
| `runtime_error` | Provider option is not `mongo` |

**Thread Safety:** Not thread-safe.

**Side Effects:**
- Logs to TRACE at levels 28, 30

---

### `listDatabases(options, search_payload) -> std::vector<JSONDocument>`

**Brief:** Lists all databases in the MongoDB server matching the search criteria.

**Parameters:**
- `options` - Operation options specifying `listdatabases` operation and `mongo` provider
- `search_payload` - JSON document containing optional filter criteria

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::listdatabases`
- `options.provider()` must be `apiliteral::provider::mongo`

**Returns:** Vector of JSON documents, each containing a database name and its query.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `listdatabases` |
| `runtime_error` | Provider option is not `mongo` |

**Thread Safety:** Not thread-safe.

**Side Effects:**
- Logs to TRACE at levels 28, 30

---

### `readDbInfo(options, search_payload) -> JSONDocument`

**Brief:** Reads database metadata information from MongoDB, such as statistics and configuration.

**Parameters:**
- `options` - Operation options specifying `readdbinfo` operation and `mongo` provider
- `search_payload` - JSON document containing optional query parameters

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::readdbinfo`
- `options.provider()` must be `apiliteral::provider::mongo`

**Returns:** JSON document containing database metadata, or empty JSON if no metadata found.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `readdbinfo` |
| `runtime_error` | Provider option is not `mongo` |

**Thread Safety:** Not thread-safe.

**Side Effects:**
- Logs to TRACE at level 30

---

### `searchCollection(options, query_payload) -> std::vector<JSONDocument>`

**Brief:** Performs a general search query against a MongoDB collection using the provided query payload.

**Parameters:**
- `options` - Operation options specifying `searchcollection` operation and `mongo` provider
- `query_payload` - JSON document containing the search query

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::searchcollection`
- `options.provider()` must be `apiliteral::provider::mongo`

**Returns:** Vector of JSON documents containing search results.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `searchcollection` |
| `runtime_error` | Provider option is not `mongo` |

**Thread Safety:** Not thread-safe.

**Side Effects:**
- Logs to TRACE at level 30

---

### `findVersionAliases(options, query_payload) -> std::vector<JSONDocument>` [NOT IMPLEMENTED]

**Brief:** Finds version aliases in MongoDB. Currently not implemented and throws an exception.

**Parameters:**
- `options` - Operation options (unused)
- `query_payload` - Query payload (unused)

**Returns:** Never returns normally.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Always - function is not implemented |

**Thread Safety:** N/A (throws immediately).

---

### `findCompositionsContaining(options, search_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all global configurations (compositions) that contain a specific configuration type and version, optionally filtered by entity.

**Parameters:**
- `options` - Operation options specifying `findcompositionscontaining` operation and `mongo` provider; must include collection (configuration type), version, and optionally entity
- `search_payload` - JSON document (currently unused, search based on options)

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::findcompositionscontaining`
- `options.provider()` must be `apiliteral::provider::mongo`
- `options.collection()` (configuration type) must be provided and not empty
- `options.version()` must be provided and not empty

**Returns:** Vector of JSON documents, each containing the name of a global configuration that contains the specified component.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `findcompositionscontaining` |
| `runtime_error` | Provider option is not `mongo` |
| `runtime_error` | Configuration type (collection) is empty or not provided |
| `runtime_error` | Version is empty or not provided |

**Thread Safety:** Not thread-safe.

**Side Effects:**
- Logs to TRACE at levels 27, 28, 29

---

### `debug::MongoDB() -> void`

**Brief:** Enables TRACE debugging for all MongoDB dispatch operations by setting trace levels and modes.

**Preconditions:**
- None

**Postconditions:**
- TRACE name set to "dispatch_mongodb.cpp"
- All trace levels enabled (0xFFFFFFFFFFFFFFFFLL)
- Memory and slow modes configured per `trace_mode::modeM` and `trace_mode::modeS`

**Throws:** None.

**Thread Safety:** Not thread-safe. Modifies global TRACE state.

**Side Effects:**
- Modifies global TRACE configuration
- Logs to TRACE at level 10

---

## Error Handling Strategy

All functions follow a consistent error handling pattern:
1. **Validation first:** Check operation and provider options at function entry
2. **Early failure:** Throw `runtime_error` with descriptive message if validation fails
3. **Exception propagation:** Let exceptions from the storage provider propagate up
4. **Logging:** Use TRACE logging for debugging, not for error reporting

## Performance Considerations

- **Connection overhead:** Each function call creates a new database connection. For bulk operations, consider using the Multitasker from ConfigurationDB.
- **Regex compilation:** Regex patterns are compiled on each function call. For high-frequency operations, this could be optimized by caching compiled patterns.
- **String operations:** Heavy use of `std::ostringstream` for JSON construction adds overhead compared to direct JSON library usage.

## Testing Notes

- **Unit tests:** See `test/ConfigurationDB/` for MongoDB-specific tests
- **Key test cases:** Document CRUD, configuration management, version queries
- **Test requirements:** MongoDB server must be running for integration tests

## Maintenance Notes

- The regex patterns for parsing filter JSON are fragile and tightly coupled to the filter format produced by the storage provider. Changes to filter format require corresponding regex updates.
- The `findVersionAliases` function is not implemented - this is a known limitation.
- TRACE level assignments (25-30) should remain consistent with other dispatch implementations.
- The `findCompositionsContaining` function searches the `SystemConfiguration` collection specifically; changes to the global configuration schema will require updates here.

## See Also

- [dispatch_mongodb.h](./dispatch_mongodb.h.md) - Function declarations
- [dispatch_filedb.cpp](./dispatch_filedb.cpp.md) - FileSystemDB equivalent implementation
- [dispatch_ucondb.cpp](./dispatch_ucondb.cpp.md) - UconDB equivalent implementation
- [StorageProviders/MongoDB/provider_mongodb.h](../StorageProviders/MongoDB/provider_mongodb.h.md) - Underlying MongoDB provider

---

**Documentation generated for artdaq-database ConfigurationDB module**
