# storage_providers.h

**Path:** `artdaq-database/StorageProviders/storage_providers.h`

**Purpose:** Defines the core `StorageProvider` template class that provides a unified interface for database storage operations. This template-based abstraction allows the artdaq-database system to work with different backend storage implementations (FileSystemDB, MongoDB, UconDB) through a consistent API, implementing the Strategy pattern for pluggable storage backends.

## Key Concepts

### Template-Based Provider Abstraction
The `StorageProvider` class template takes two parameters:
- `TYPE`: The storable document type (typically `JSONDocument`)
- `IMPL`: The concrete backend implementation (e.g., `FileSystemDB`, `MongoDB`, `UconDB`)

This design enables compile-time polymorphism where operations are delegated to the specific backend implementation, providing better performance than virtual function dispatch.

### PassKey Idiom
The class uses the PassKey idiom to control instantiation. The `create()` factory method is the only public way to construct a `StorageProvider`, ensuring proper initialization and enabling connection pooling strategies.

### Unified Storage Operations
The provider exposes templated methods for document read/write operations and various query methods for configurations, versions, entities, and collections. All backends implement the same interface, allowing the application code to switch providers through configuration without code changes.

## Thread Safety

- **Thread-safe:** Depends on underlying implementation
- **Concurrent access:** Read operations are thread-safe if the backend is thread-safe; writes may require synchronization
- **Locking:** No internal locking; relies on backend implementation

## Dependencies

| Include | Purpose |
|---------|---------|
| `<memory>` | Smart pointer support (`std::shared_ptr`) for provider management |
| `<string>` | String handling for database URIs and metadata |
| `<vector>` | Container for returning multiple document results |
| `artdaq-database/SharedCommon/configuraion_api_literals.h` | API literal constants for operation names |
| `artdaq-database/SharedCommon/shared_datatypes.h` | Shared type definitions including `object_id_t` |

## Classes/Structures

### `StorageProvider<TYPE, IMPL>`

Template class that wraps a backend implementation and provides a uniform storage API.

**Thread Safety:** Depends on the `IMPL` type. FileSystemDB is not thread-safe for writes; MongoDB and UconDB are thread-safe.

#### Type Aliases

| Alias | Definition | Purpose |
|-------|------------|---------|
| `StorableType` | `TYPE` | The document type being stored |
| `Provider` | `StorageProvider<TYPE, IMPL>` | Self-referential type alias |
| `ProviderSPtr` | `std::shared_ptr<Provider>` | Shared pointer to provider instance |

#### Methods

##### `create(std::shared_ptr<IMPL> const& provider) -> ProviderSPtr` [static]

**Brief:** Factory method that creates a new StorageProvider instance wrapping the given backend implementation.

**Parameters:**
- `provider` - Shared pointer to the backend implementation (must not be null)

**Preconditions:**
- `provider` must be a valid, non-null shared pointer

**Returns:** Shared pointer to the newly created StorageProvider instance

**Postconditions:**
- Returned pointer is valid and ready for use

**Thread Safety:** Thread-safe

**Example:**
```cpp
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"

using namespace artdaq::database;
using namespace artdaq::database::filesystem;

void createProvider() {
  auto config = DBConfig("filesystemdb:///data/artdaq/db");
  auto backend = FileSystemDB::create(config);
  auto provider = StorageProvider<JSONDocument, FileSystemDB>::create(backend);
  // Use provider for operations...
}
```

##### `StorageProvider(std::shared_ptr<IMPL> const& provider, PassKeyIdiom const&)`

**Brief:** Constructor that initializes the StorageProvider with a backend implementation. Protected by the PassKey idiom to enforce use of the `create()` factory method.

**Parameters:**
- `provider` - Shared pointer to the backend implementation
- `PassKeyIdiom const&` - PassKey token (only obtainable through `create()`)

**Thread Safety:** Thread-safe

##### `readDocument(FILTER const& filter) -> std::vector<TYPE>`

**Brief:** Retrieves documents from the database that match the specified filter criteria.

**Parameters:**
- `filter` - JSON document containing search criteria

**Preconditions:**
- Filter must be a valid, non-empty document

**Returns:** Vector of documents matching the filter (may be empty if none match)

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Database connection failure or query error |

**Thread Safety:** Safe for concurrent calls if backend supports it

##### `writeDocument(TYPE const& document) -> object_id_t`

**Brief:** Writes a document to the database and returns its assigned object ID.

**Parameters:**
- `document` - The document to store

**Preconditions:**
- Document must be valid and non-empty

**Returns:** The object ID assigned to the stored document

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Write failure or validation error |

**Thread Safety:** May require external synchronization depending on backend

##### `findConfigurations(FILTER const& filter) -> std::vector<FILTER>`

**Brief:** Finds all global configurations in the database, optionally filtered by criteria.

**Parameters:**
- `filter` - Search criteria (can contain configuration name patterns)

**Returns:** Vector of JSON documents describing available configurations, sorted by most recent assignment

##### `addConfiguration(FILTER const& filter) -> std::vector<FILTER>`

**Brief:** Adds a new configuration to the database. (Currently returns empty vector for FileSystemDB)

**Parameters:**
- `filter` - Configuration details to add

**Returns:** Vector of result documents (implementation-dependent)

##### `findVersions(FILTER const& filter) -> std::vector<FILTER>`

**Brief:** Finds all versions of an entity or configuration matching the filter.

**Parameters:**
- `filter` - Must contain either `entities` or `configurations` field

**Returns:** Vector of JSON documents with version information

##### `findEntities(FILTER const& filter) -> std::vector<FILTER>`

**Brief:** Finds all entities in the database matching the filter criteria.

**Parameters:**
- `filter` - Search criteria for entity names

**Returns:** Vector of unique entity documents

##### `configurationComposition(FILTER const& filter) -> std::vector<FILTER>`

**Brief:** Gets the composition (list of entities) within a specific configuration.

**Parameters:**
- `filter` - Must contain configuration name

**Returns:** Vector of entity documents that compose the configuration

##### `listCollections(FILTER const& filter) -> std::vector<FILTER>`

**Brief:** Lists all collections (entity types) in the database.

**Parameters:**
- `filter` - Optional filter criteria

**Returns:** Vector of collection descriptor documents

##### `listDatabases(FILTER const& filter) -> std::vector<FILTER>`

**Brief:** Lists all available databases (sibling databases for FileSystemDB).

**Parameters:**
- `filter` - Optional filter criteria

**Returns:** Vector of database descriptor documents

##### `databaseMetadata(FILTER const& filter) -> std::vector<FILTER>`

**Brief:** Retrieves metadata about the database from the SystemMetadata collection.

**Parameters:**
- `filter` - Optional filter criteria

**Returns:** Vector of metadata documents

##### `searchCollection(FILTER const& filter) -> std::vector<FILTER>`

**Brief:** Performs advanced search within a collection. (Not implemented for FileSystemDB)

**Parameters:**
- `filter` - Advanced search criteria

**Returns:** Vector of matching documents

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Not implemented for FileSystemDB |

### `PassKeyIdiom` (Nested Class)

**Brief:** Private nested class that restricts construction to the factory method, implementing the PassKey idiom pattern. This ensures that `StorageProvider` instances can only be created through the `create()` factory method.

**Thread Safety:** Thread-safe (immutable after construction)

## Functions

### `make_database_metadata(name, uri) -> std::string`

**Brief:** Creates a JSON-formatted metadata string for newly initialized databases, capturing creation context and system information.

**Parameters:**
- `name` - Database name (must not be empty)
- `uri` - Connection URI for the database (must not be empty)

**Preconditions:**
- Both `name` and `uri` must be non-empty strings

**Returns:** JSON-formatted string containing database metadata

**Postconditions:**
- Returned string is valid JSON with all required metadata fields

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `assertion_failure` | If name or uri is empty (in debug builds) |

**Thread Safety:** Thread-safe (uses only local variables)

**Example:**
```cpp
#include "artdaq-database/StorageProviders/storage_providers.h"

void storeMetadata() {
  auto metadata = artdaq::database::make_database_metadata(
    "my_database",
    "filesystemdb:///data/artdaq/my_database"
  );
  // metadata contains: {"document":{"name":"my_database","uri":"...", ...}}
}
```

## Constants

| Constant | Value | Purpose |
|----------|-------|---------|
| `system_metadata` | `"SystemMetadata"` | Collection name for database metadata documents |
| `ouid_invalid` | `"000000000000000000000000"` | Invalid object ID placeholder (24 hex zeros) |

## Relationship to Other Components

### Backend Implementations
The template is specialized by three backend implementations:
- **FileSystemDB**: File-based storage using JSON files organized in directories
- **MongoDB**: MongoDB document database storage via mongocxx driver
- **UconDB**: Fermilab's UconDB web service storage via REST API

### ConfigurationDB Layer
The `ConfigurationDB` module uses `StorageProvider` instances through dispatch mechanisms to route operations to the appropriate backend based on the `ARTDAQ_DATABASE_URI` environment variable.

### Type Specializations
Each backend provides explicit template specializations in their implementation files:
```cpp
template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, MongoDB>::readDocument(JSONDocument const& filter) {
    // MongoDB-specific implementation
}
```

## See Also

- [storage_providers.cpp](./storage_providers.cpp.md) - Implementation of `make_database_metadata()`
- [FileSystemDB/provider_filedb.h](./FileSystemDB/provider_filedb.h.md) - FileSystemDB backend
- [MongoDB/provider_mongodb.h](./MongoDB/provider_mongodb.h.md) - MongoDB backend
- [UconDB/provider_ucondb.h](./UconDB/provider_ucondb.h.md) - UconDB backend
- [ConfigurationDB/configurationdbifc.h](../ConfigurationDB/configurationdbifc.h.md) - High-level API

## Notes for Developers

### Template Specialization Pattern
Backend implementations must provide explicit specializations for all `StorageProvider` methods. These are typically defined in the corresponding `provider_*_readwrite.cpp` and `provider_*.cpp` files.

### Connection Handling
The wrapped `_provider` member holds the actual backend connection. Connection pooling is handled by the backend implementation's factory method (e.g., MongoDB caches connections per thread+URI combination).

### Error Handling
Operations throw `runtime_error` exceptions with descriptive messages when failures occur. The exception message typically includes the backend name for debugging.

### Adding New Backends
To add a new storage backend:
1. Create a new class similar to `FileSystemDB`, `MongoDB`, or `UconDB`
2. Implement `connection()` method and any backend-specific configuration
3. Provide template specializations for all `StorageProvider` methods
4. Add URI scheme detection in the ConfigurationDB dispatch layer
5. Update documentation

### Common Pitfalls

- **Pitfall 1:** Forgetting to provide all template specializations will cause linker errors. Ensure all methods are specialized for your backend.
- **Pitfall 2:** Calling methods on a null provider will cause undefined behavior. Always use the `create()` factory method.

### Anti-patterns

```cpp
// DON'T do this - bypasses factory pattern
auto provider = StorageProvider<JSONDocument, FileSystemDB>(backend, PassKeyIdiom{});

// DO this instead - use factory method
auto provider = StorageProvider<JSONDocument, FileSystemDB>::create(backend);
```
