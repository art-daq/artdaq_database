# dispatch_ucondb.cpp

**Path:** `artdaq-database/ConfigurationDB/dispatch_ucondb.cpp`

**Implements:** [dispatch_ucondb.h](./dispatch_ucondb.h.md)

**Purpose:** Provides the UconDB-specific implementation of the database operation dispatch layer. This file routes configuration database operations to the UconDB (Microservices Configuration Database) storage provider via REST API calls, implementing document CRUD operations, configuration management, and metadata queries.

## Implementation Overview

This implementation file provides concrete UconDB-specific logic for database operations declared in `dispatch_ucondb.h`. The implementation closely mirrors `dispatch_mongodb.cpp` in structure, with provider-specific adaptations for the UconDB REST API backend. Each function:
1. Validates that the operation type and provider match expected values (`ucon` provider)
2. Creates a UconDB database connection using `DBI::DBConfig` and `DBI::DB::create()`
3. Instantiates a `DBProvider<JSONDocument>` to execute REST API calls
4. Processes results (often using regex to extract structured data from JSON filter strings)
5. Returns results in a standardized JSON format

The implementation uses the TRACE logging system for debugging, with trace levels 25-30 for different operations.

## Key Algorithms

### Document Version Selection

When `readDocument()` retrieves multiple documents, it selects the highest version:

**Steps:**
1. Call `readDocuments()` to get all matching documents
2. If multiple documents returned, use `std::max_element` with `docrec::compareDocumentVersions`
3. Swap the selected document into the return value

**Why this approach:** Ensures that the most recent version is returned when multiple versions exist for the same entity.

### Configuration Name Extraction

The `findConfigurations()` function extracts unique configuration names with deduplication:

**Steps:**
1. Query UconDB for all configurations matching the search payload
2. Maintain a `seenValues` list initialized with `apiliteral::notprovided`
3. For each result, extract the configuration name from the filter child
4. Use the `isNew` lambda to deduplicate names
5. Format each unique configuration as a JSON object with name and query

**Why this approach:** Prevents duplicate configurations from appearing in results when multiple versions exist.

### Regex-Based Filter Parsing

Several functions use regex to parse filter JSON strings returned from UconDB:

**Example pattern for `configurationComposition()`:**
```cpp
"\"configurations\\.name\"\\s*:\\s*\"((\\\\\"|[^\"])*)\"\\,"
"\\s*\"entities\\.name\"\\s*:\\s*\"((\\\\\"|[^\"])*)\"
```

This extracts configuration and entity names from filter JSON.

## Thread Safety

- **Thread-safe:** No (each function creates its own database connection)
- **Concurrent access:** Safe when called from multiple threads (each call is independent)
- **Locking:** No internal locking; relies on provider implementation

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | Common definitions and TRACE macros |
| `artdaq-database/ConfigurationDB/dispatch_ucondb.h` | Function declarations for this implementation |
| `artdaq-database/ConfigurationDB/options_operations.h` | `ManageDocumentOperation` and `ManageAliasesOperation` classes |
| `artdaq-database/BasicTypes/basictypes.h` | Basic type definitions |
| `artdaq-database/DataFormats/shared_literals.h` | JSON and API literal constants |
| `artdaq-database/JsonDocument/JSONDocument.h` | `JSONDocument` class for JSON manipulation |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | Builder pattern for constructing documents |
| `artdaq-database/StorageProviders/UconDB/provider_ucondb.h` | UconDB REST API provider implementation |

## Internal Functions

### Connection Pattern

All functions follow this connection pattern:
```cpp
auto config = DBI::DBConfig{};
auto database = DBI::DB::create(config);
auto provider = DBI::DBProvider<JSONDocument>::create(database);
```

**Called by:** All dispatch functions

**Purpose:** Creates a fresh UconDB connection for each operation, ensuring isolation between operations.

## Functions

### `writeDocument(options, insert_payload) -> void`

**Brief:** Writes a JSON document to UconDB storage via REST API, either creating a new document or overwriting an existing one.

**Parameters:**
- `options` - Operation options specifying `writedocument` or `overwritedocument` operation and `ucon` provider
- `insert_payload` - JSON document containing the data to write

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::writedocument` or `apiliteral::operation::overwritedocument`
- `options.provider()` must be `apiliteral::provider::ucon`

**Postconditions:**
- Document is persisted to UconDB via REST API
- Object ID is logged at TRACE level 25

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `writedocument` or `overwritedocument` |
| `runtime_error` | Provider option is not `ucon` |

**Thread Safety:** Not thread-safe. Each call creates its own connection.

**Side Effects:**
- Creates or updates a document in UconDB
- Makes HTTP/HTTPS network call
- Logs to TRACE at level 25

---

### `readDocument(options, search_payload) -> JSONDocument`

**Brief:** Reads a single JSON document from UconDB matching the search criteria, returning the highest version if multiple matches exist.

**Parameters:**
- `options` - Operation options (provider validated in `readDocuments`)
- `search_payload` - JSON document containing search/filter criteria

**Returns:** The matching `JSONDocument` with the highest version.

**Postconditions:**
- Returns exactly one document (the highest version if multiple exist)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | No documents found matching the search criteria |

**Thread Safety:** Not thread-safe.

**Side Effects:**
- Makes HTTP/HTTPS network call
- Logs to TRACE at level 26

---

### `readDocuments(options, search_payload) -> std::vector<JSONDocument>`

**Brief:** Reads all JSON documents from UconDB matching the search criteria, returning them as a vector.

**Parameters:**
- `options` - Operation options specifying `readdocument` operation and `ucon` provider
- `search_payload` - JSON document containing search/filter criteria

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::readdocument`
- `options.provider()` must be `apiliteral::provider::ucon`

**Returns:** Vector of all matching `JSONDocument` objects (may be empty).

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `readdocument` |
| `runtime_error` | Provider option is not `ucon` |

**Thread Safety:** Not thread-safe.

**Side Effects:**
- Makes HTTP/HTTPS network call
- Logs to TRACE at level 26

---

### `findConfigurations(options, search_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all unique configuration names in UconDB matching the search criteria, returning deduplicated results.

**Parameters:**
- `options` - Operation options specifying `findconfigs` operation and `ucon` provider
- `search_payload` - JSON document containing search criteria

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::findconfigs`
- `options.provider()` must be `apiliteral::provider::ucon`

**Returns:** Vector of JSON documents, each containing a unique configuration name and its query.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `findconfigs` |
| `runtime_error` | Provider option is not `ucon` |

**Thread Safety:** Not thread-safe.

**Side Effects:**
- Makes HTTP/HTTPS network call
- Logs to TRACE at levels 27, 28, 30

---

### `configurationComposition(options, search_payload) -> JSONDocument`

**Brief:** Retrieves the composition of a configuration from UconDB, showing all entities and their relationships within the specified configuration.

**Parameters:**
- `options` - Operation options specifying `confcomposition` operation and `ucon` provider
- `search_payload` - JSON document identifying the configuration

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::confcomposition`
- `options.provider()` must be `apiliteral::provider::ucon`

**Returns:** JSON document containing a "search" array with configuration composition details.

**Postconditions:**
- Returns a document with format: `{ "search": [ { "name": "config:entity", "query": {...} }, ... ] }`

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `confcomposition` |
| `runtime_error` | Provider option is not `ucon` |
| `runtime_error` | No search filters were found |
| `runtime_error` | Filter string format is unsupported (regex mismatch) |

**Thread Safety:** Not thread-safe.

**Side Effects:**
- Makes HTTP/HTTPS network call
- Logs to TRACE at level 28

---

### `findVersions(options, query_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all versions of documents matching the specified criteria in UconDB.

**Parameters:**
- `options` - Operation options specifying `findversions` operation and `ucon` provider
- `query_payload` - Unused parameter (search is based on options)

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::findversions`
- `options.provider()` must be `apiliteral::provider::ucon`

**Returns:** Vector of JSON documents, each containing a version name and its query.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `findversions` |
| `runtime_error` | Provider option is not `ucon` |
| `runtime_error` | Filter string format is unsupported |

**Thread Safety:** Not thread-safe.

**Side Effects:**
- Makes HTTP/HTTPS network call
- Logs to TRACE at levels 28, 30

---

### `findEntities(options, query_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all entities matching the specified criteria in UconDB.

**Parameters:**
- `options` - Operation options specifying `findentities` operation and `ucon` provider
- `query_payload` - Unused parameter (search is based on options)

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::findentities`
- `options.provider()` must be `apiliteral::provider::ucon`

**Returns:** Vector of JSON documents, each containing an entity name and its query.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `findentities` |
| `runtime_error` | Provider option is not `ucon` |
| `runtime_error` | Filter string format is unsupported |

**Thread Safety:** Not thread-safe.

**Side Effects:**
- Makes HTTP/HTTPS network call
- Logs to TRACE at levels 28, 29, 30

---

### `assignConfiguration(options, search_payload) -> JSONDocument`

**Brief:** Assigns a document to a configuration by reading the document from UconDB, adding the configuration association, and writing it back.

**Parameters:**
- `options` - Operation options specifying `assignconfig` operation and `ucon` provider
- `search_payload` - JSON document identifying the target document

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::assignconfig`
- `options.provider()` must be `apiliteral::provider::ucon`

**Returns:** Updated configuration composition after the assignment.

**Postconditions:**
- Document is updated in UconDB with the new configuration association
- Returns the updated composition for verification

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `assignconfig` |
| `runtime_error` | Provider option is not `ucon` |

**Thread Safety:** Not thread-safe. Performs read-modify-write which is not atomic.

**Side Effects:**
- Modifies document in UconDB (multiple network calls)
- Logs to TRACE at level 30

---

### `removeConfiguration(options, search_payload) -> JSONDocument`

**Brief:** Removes a configuration association from a document by reading the document from UconDB, removing the configuration, and writing it back.

**Parameters:**
- `options` - Operation options specifying `removeconfig` operation and `ucon` provider
- `search_payload` - JSON document identifying the target document

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::removeconfig`
- `options.provider()` must be `apiliteral::provider::ucon`

**Returns:** Updated configuration composition after the removal.

**Postconditions:**
- Document is updated in UconDB with the configuration association removed
- Returns the updated composition for verification

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `removeconfig` |
| `runtime_error` | Provider option is not `ucon` |

**Thread Safety:** Not thread-safe. Performs read-modify-write which is not atomic.

**Side Effects:**
- Modifies document in UconDB (multiple network calls)
- Logs to TRACE at level 30

---

### `listCollections(options, search_payload) -> std::vector<JSONDocument>`

**Brief:** Lists all collections available in UconDB matching the search criteria.

**Parameters:**
- `options` - Operation options specifying `listcollections` operation and `ucon` provider
- `search_payload` - JSON document containing optional filter criteria

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::listcollections`
- `options.provider()` must be `apiliteral::provider::ucon`

**Returns:** Vector of JSON documents, each containing a collection name and its query.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `listcollections` |
| `runtime_error` | Provider option is not `ucon` |

**Thread Safety:** Not thread-safe.

**Side Effects:**
- Makes HTTP/HTTPS network call
- Logs to TRACE at levels 28, 30

---

### `listDatabases(options, search_payload) -> std::vector<JSONDocument>`

**Brief:** Lists all databases available in the UconDB server matching the search criteria.

**Parameters:**
- `options` - Operation options specifying `listdatabases` operation and `ucon` provider
- `search_payload` - JSON document containing optional filter criteria

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::listdatabases`
- `options.provider()` must be `apiliteral::provider::ucon`

**Returns:** Vector of JSON documents, each containing a database name and its query.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `listdatabases` |
| `runtime_error` | Provider option is not `ucon` |

**Thread Safety:** Not thread-safe.

**Side Effects:**
- Makes HTTP/HTTPS network call
- Logs to TRACE at levels 28, 30

---

### `readDbInfo(options, search_payload) -> JSONDocument`

**Brief:** Reads database metadata information from UconDB, such as statistics and configuration.

**Parameters:**
- `options` - Operation options specifying `readdbinfo` operation and `ucon` provider
- `search_payload` - JSON document containing optional query parameters

**Preconditions:**
- `options.operation()` must be `apiliteral::operation::readdbinfo`
- `options.provider()` must be `apiliteral::provider::ucon`

**Returns:** JSON document containing database metadata wrapped in a "search" object, or empty search result if no metadata found.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Operation option is not `readdbinfo` |
| `runtime_error` | Provider option is not `ucon` |

**Thread Safety:** Not thread-safe.

**Side Effects:**
- Makes HTTP/HTTPS network call
- Logs to TRACE at level 30

---

### `findVersionAliases(options, query_payload) -> std::vector<JSONDocument>` [NOT IMPLEMENTED]

**Brief:** Finds version aliases in UconDB. Currently not implemented and throws an exception.

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

### `searchCollection(options, query_payload) -> std::vector<JSONDocument>` [NOT IMPLEMENTED]

**Brief:** Performs a general search query against a UconDB collection. Currently not implemented.

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

### `findCompositionsContaining(options, search_payload) -> std::vector<JSONDocument>` [NOT IMPLEMENTED]

**Brief:** Finds global configurations containing a specific component. Currently not implemented for UconDB.

**Parameters:**
- `options` - Operation options (unused)
- `search_payload` - Search payload (unused)

**Returns:** Never returns normally.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Always - function is not implemented for UconDB |

**Thread Safety:** N/A (throws immediately).

---

### `debug::UconDB() -> void`

**Brief:** Enables TRACE debugging for all UconDB dispatch operations by setting trace levels and modes.

**Preconditions:**
- None

**Postconditions:**
- TRACE name set to "dispatch_ucondb.cpp"
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
3. **Exception propagation:** Let exceptions from the storage provider (including network errors) propagate up
4. **Logging:** Use TRACE logging for debugging, not for error reporting

## Performance Considerations

- **Network latency:** Unlike FileSystemDB, each operation requires HTTP/HTTPS round-trips. Consider batching operations where possible.
- **Connection overhead:** Each function call creates a new connection. Connection pooling at the provider level would improve performance.
- **Regex compilation:** Regex patterns are compiled on each function call; caching could improve high-frequency operations.
- **String operations:** Heavy use of `std::ostringstream` for JSON construction adds overhead.

## Testing Notes

- **Unit tests:** See `test/ConfigurationDB/` for UconDB-specific tests
- **Key test cases:** Document CRUD, configuration management, version queries
- **Test requirements:** UconDB server must be accessible for integration tests

## Maintenance Notes

- **Unimplemented functions:** `findVersionAliases`, `searchCollection`, and `findCompositionsContaining` throw "not implemented" exceptions. These need implementation if required.
- **Regex patterns:** The regex patterns for parsing filter JSON are fragile and tightly coupled to the filter format. Changes to filter format require corresponding regex updates.
- **API parity:** This implementation should maintain parity with `dispatch_mongodb.cpp` where possible. When updating one, consider updating the other.
- **TRACE levels:** Use levels 25-30 consistently with other dispatch implementations.

## See Also

- [dispatch_ucondb.h](./dispatch_ucondb.h.md) - Function declarations
- [dispatch_mongodb.cpp](./dispatch_mongodb.cpp.md) - MongoDB equivalent implementation
- [dispatch_filedb.cpp](./dispatch_filedb.cpp.md) - FileSystemDB equivalent
- [StorageProviders/UconDB/provider_ucondb.h](../StorageProviders/UconDB/provider_ucondb.h.md) - Underlying UconDB provider

---

**Documentation generated for artdaq-database ConfigurationDB module**
