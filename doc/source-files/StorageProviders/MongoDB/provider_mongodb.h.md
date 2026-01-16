# provider_mongodb.h

**Path:** `artdaq-database/StorageProviders/MongoDB/provider_mongodb.h`

**Purpose:** Defines the MongoDB storage provider that interfaces with MongoDB database servers using the mongocxx C++ driver. This provider offers production-grade, scalable document storage with full MongoDB query capabilities, connection pooling, and support for replica sets and sharded clusters.


## Key Concepts

### MongoDB C++ Driver Integration

The provider uses the official MongoDB C++ driver (mongocxx/bsoncxx) for all database operations. The driver handles connection pooling, automatic reconnection, and wire protocol communication with MongoDB servers.

### Singleton Instance Pattern

The mongocxx library requires a single global `mongocxx::instance` object that must outlive all other mongocxx objects. The provider manages this as a singleton through `getInstance()` in the connection implementation file.

### PassKey Idiom Factory Pattern

The `MongoDB` class uses the PassKey idiom to enforce construction through the `create()` factory method. This enables:
- Connection caching per thread and URI combination
- Controlled object creation preventing direct instantiation
- Resource management through shared pointers

## Thread Safety

- **Thread-safe:** Yes
- **Concurrent access:** Multiple threads can share a MongoDB instance safely
- **Locking:** Connection cache protected by mutex; mongocxx driver handles internal thread safety

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/StorageProviders/storage_providers.h` | Base `StorageProvider` template class |
| `<mongocxx/client.hpp>` | MongoDB C++ driver client class for database connections |
| `<mongocxx/instance.hpp>` | MongoDB driver instance singleton management |

## Classes/Structures

### `DBConfig`

Configuration structure for MongoDB connections. Holds the connection URI and provides methods to construct it from environment variables or explicit values.

**Thread Safety:** Thread-safe (immutable after construction)

#### Constructor: `DBConfig()`

**Brief:** Creates a default configuration using environment variables or default localhost settings.

**Preconditions:**
- None (uses sensible defaults if environment variables are not set)

**Postconditions:**
- `uri` member is populated with a valid MongoDB connection string

**Environment Variables:**
| Variable | Purpose |
|----------|---------|
| `ARTDAQ_DATABASE_URI` | Override default connection URI (must start with `mongodb://`) |
| `ARTDAQ_DATABASE_CLIENT_CERT` | Path to X.509 client certificate for TLS authentication |
| `ARTDAQ_DATABASE_CA_CERT` | Path to CA certificate for TLS verification |

**Example:**
```cpp
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"

using namespace artdaq::database::mongo;

// Uses ARTDAQ_DATABASE_URI environment variable or defaults to localhost:27017
DBConfig config;
std::cout << "Connecting to: " << config.connectionURI() << std::endl;
```

#### Constructor: `DBConfig(const std::string& uri_)`

**Brief:** Creates a configuration with an explicit MongoDB connection URI.

**Parameters:**
- `uri_` - MongoDB connection URI string (must not be empty)

**Preconditions:**
- `uri_` must be a non-empty, valid MongoDB connection URI

**Postconditions:**
- `uri` member equals the provided `uri_` value

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `confirm` assertion failure | When `uri_` is empty |

**Example:**
```cpp
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"

using namespace artdaq::database::mongo;

// Explicit URI for production server
DBConfig config("mongodb://dbserver.fnal.gov:27017/production_db");

// With authentication
DBConfig authConfig("mongodb://user:pass@dbserver:27017/db?authSource=admin");

// Replica set configuration
DBConfig rsConfig("mongodb://host1:27017,host2:27017/db?replicaSet=rs0");
```

#### Method: `connectionURI() const -> std::string`

**Brief:** Returns the MongoDB connection URI string.

**Returns:** The complete MongoDB connection URI

**Thread Safety:** Safe (const method on immutable data)

### `MongoDB`

The concrete storage provider backend for MongoDB. Manages the connection to a MongoDB database server and provides access to database operations.

**Thread Safety:** Thread-safe (connection caching protected by mutex, mongocxx driver is thread-safe)

#### Static Method: `create(DBConfig const& config) -> std::shared_ptr<MongoDB>`

**Brief:** Factory method that creates or retrieves a cached MongoDB instance for the given configuration.

**Parameters:**
- `config` - MongoDB connection configuration

**Preconditions:**
- MongoDB server must be reachable at the specified URI

**Returns:** Shared pointer to a MongoDB instance (may be a cached instance)

**Postconditions:**
- Returned instance is connected to the MongoDB server
- Instance is cached for reuse by the same thread with the same URI

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `mongocxx::exception` | When connection to MongoDB fails |

**Thread Safety:** Safe (uses mutex-protected cache)

**Side Effects:**
- May establish new network connection to MongoDB server
- Updates internal connection cache

**Example:**
```cpp
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"

using namespace artdaq::database::mongo;

try {
  DBConfig config("mongodb://localhost:27017/artdaq_db");
  auto db = MongoDB::create(config);

  // Use the connection
  auto& connection = db->connection();

  // Second call with same config returns cached instance
  auto db2 = MongoDB::create(config);  // Same instance as db
} catch (const mongocxx::exception& e) {
  std::cerr << "MongoDB connection failed: " << e.what() << std::endl;
}
```

#### Constructor: `MongoDB(DBConfig config, PassKeyIdiom const&)`

**Brief:** Constructs a MongoDB instance with the given configuration. Only accessible through `create()` factory method.

**Parameters:**
- `config` - MongoDB connection configuration
- `PassKeyIdiom const&` - Access control token (only `create()` can provide this)

**Preconditions:**
- Must be called through `create()` factory method

**Postconditions:**
- Client connected to MongoDB server
- Session started on the connection

**Note:** Do not call this constructor directly. Use `MongoDB::create()` instead.

#### Method: `connection() -> mongocxx::database&`

**Brief:** Returns a reference to the connected MongoDB database, initializing system metadata if needed.

**Returns:** Reference to the `mongocxx::database` object for the configured database

**Postconditions:**
- System metadata collection exists in the database
- Returned reference is valid for database operations

**Side Effects:**
- Creates system metadata collection and record if they do not exist

**Thread Safety:** Safe (mongocxx handles thread safety)

**Example:**
```cpp
auto db = MongoDB::create(config);
auto& connection = db->connection();

// Get a collection
auto collection = connection.collection("Configurations");

// Perform operations
auto count = collection.count_documents({});
```

#### Method: `list_databases() -> mongocxx::cursor`

**Brief:** Lists all databases on the MongoDB server.

**Returns:** Cursor iterating over database descriptor documents

**Preconditions:**
- Connection must be established

**Thread Safety:** Safe

**Example:**
```cpp
auto db = MongoDB::create(config);
auto cursor = db->list_databases();

for (const auto& dbDoc : cursor) {
  std::cout << "Database: " << compat::to_json(dbDoc) << std::endl;
}
```

### `PassKeyIdiom` (nested in `MongoDB`)

**Brief:** Private access control class that enforces factory method construction pattern.

Only the `MongoDB::create()` static method can instantiate this class, preventing direct construction of `MongoDB` objects.

## Type Aliases

```cpp
template <typename TYPE>
using MongoDBProvider = StorageProvider<TYPE, MongoDB>;
```
**Brief:** Template alias combining `StorageProvider` with `MongoDB` backend for a specific document type.

```cpp
template <typename TYPE>
using DBProvider = MongoDBProvider<TYPE>;
```
**Brief:** Convenience alias for `MongoDBProvider`.

```cpp
using DB = MongoDB;
```
**Brief:** Short alias for the `MongoDB` class.

## Constants

### Namespace `artdaq::database::mongo::literal`

| Constant | Type | Value | Purpose |
|----------|------|-------|---------|
| `MONGOURI` | `constexpr auto` | `"mongodb://"` | URI scheme prefix for MongoDB connections |
| `hostname` | `constexpr auto` | `"127.0.0.1"` | Default hostname for local development |
| `port` | `constexpr auto` | `27017` | Default MongoDB port |
| `db_name` | `constexpr auto` | `"test_configuration_db"` | Default database name for testing |

## Functions

### Namespace `artdaq::database::mongo::debug`

#### `enable() -> void`

**Brief:** Enables all TRACE debugging for MongoDB operations including connections and read/write operations.

**Side Effects:**
- Configures TRACE logging levels
- Calls `connection()` and `ReadWrite()` debug enablers

#### `connection() -> void`

**Brief:** Enables TRACE debugging specifically for connection-related operations.

**Side Effects:**
- Configures TRACE logging for connection establishment and management

#### `ReadWrite() -> void`

**Brief:** Enables TRACE debugging for document read and write operations.

**Side Effects:**
- Configures TRACE logging for `readDocument()` and `writeDocument()` operations

## Relationship to Other Components

### StorageProvider Template

`MongoDB` serves as the `IMPL` template parameter for `StorageProvider<TYPE, IMPL>`. Template specializations in `provider_mongodb.cpp` and `provider_mongodb_readwrite.cpp` implement the actual storage operations:
- `readDocument()` - Query and retrieve documents
- `writeDocument()` - Insert or update documents
- `findConfigurations()` - Search for configuration names
- `findEntities()` - Search for entity names
- `findVersions()` - Search for version information
- `listCollections()` - Enumerate collections
- `listDatabases()` - Enumerate databases

### mongocxx Driver

The provider wraps the official MongoDB C++ driver, which handles:
- Connection pooling and management
- Wire protocol communication
- BSON serialization/deserialization
- Cursor management for large result sets
- Authentication and TLS

### ConfigurationDB Dispatch

The ConfigurationDB layer selects MongoDB when the `ARTDAQ_DATABASE_URI` environment variable starts with `mongodb://`. See `dispatch_mongodb.cpp` for dispatch logic.

## Common Pitfalls

- **Empty URI:** Passing an empty string to `DBConfig(const std::string&)` will trigger an assertion failure. Use the default constructor if you want automatic configuration.

- **Driver Instance Lifetime:** The mongocxx driver requires a single global instance. The provider handles this automatically, but be aware that creating MongoDB objects before the instance exists will cause undefined behavior.

- **Connection Errors:** Always wrap `create()` calls in try-catch blocks to handle connection failures gracefully.

### Anti-patterns

```cpp
// DON'T do this - direct construction bypasses factory pattern
// MongoDB db(config, MongoDB::PassKeyIdiom{});  // Won't compile

// DO this instead - use factory method
auto db = MongoDB::create(config);
```

```cpp
// DON'T do this - no error handling
auto db = MongoDB::create(config);  // May throw

// DO this instead - handle connection errors
try {
  auto db = MongoDB::create(config);
  // Use db
} catch (const mongocxx::exception& e) {
  std::cerr << "Connection failed: " << e.what() << std::endl;
}
```

## See Also

- [provider_mongodb.cpp.md](./provider_mongodb.cpp.md) - Query operation implementations
- [provider_mongodb_readwrite.cpp.md](./provider_mongodb_readwrite.cpp.md) - Document read/write implementations
- [provider_connection.cpp.md](./provider_connection.cpp.md) - Connection management details
- [mongo_json.h.md](./mongo_json.h.md) - BSON/JSON conversion utilities
- [storage_providers.h.md](../storage_providers.h.md) - Base `StorageProvider` template
- [External: MongoDB C++ Driver](https://mongocxx.org/) - Official driver documentation

---

**Last Updated:** 2026-01-14
