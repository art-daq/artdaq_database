# provider_connection.cpp

**Path:** `artdaq-database/StorageProviders/MongoDB/provider_connection.cpp`

**Implements:** [provider_mongodb.h](./provider_mongodb.h.md)

**Purpose:** Implements MongoDB connection management, including the singleton driver instance, connection caching, configuration initialization from environment variables, and TLS/X.509 authentication setup.

## Implementation Overview

This file manages the lifecycle of MongoDB connections:
1. Maintains the single required `mongocxx::instance` as a singleton
2. Implements the `DBConfig` constructors for configuration handling
3. Implements `MongoDB::create()` factory with connection caching
4. Implements `MongoDB::connection()` with lazy metadata initialization
5. Handles TLS certificate configuration for secure connections

## Key Algorithms

### Connection Caching Algorithm

The `create()` factory method caches MongoDB connections to avoid creating redundant connections to the same server.

**Steps:**
1. Build a cache key from thread ID and connection URI
2. Lock the connection cache mutex
3. Check if a connection with this key already exists
4. If exists, return the cached connection
5. If not, create a new `MongoDB` instance and cache it
6. Return the new or cached connection

**Why this approach:** MongoDB connections are expensive to create and maintain. Caching per thread + URI prevents connection proliferation while ensuring thread safety (each thread gets its own connection).

### TLS Configuration Algorithm

The `DBConfig()` default constructor handles TLS authentication setup.

**Steps:**
1. Check for `ARTDAQ_DATABASE_URI` environment variable
2. If URI starts with `mongodb://`, use it as base URI
3. Check for `ARTDAQ_DATABASE_CLIENT_CERT` environment variable
4. If client certificate exists and file is accessible:
   a. Append TLS and X.509 authentication parameters to URI
   b. Check for `ARTDAQ_DATABASE_CA_CERT` for CA certificate
   c. If CA cert exists, append it to URI

**Why this approach:** Environment variables allow flexible configuration without code changes, supporting both development (no TLS) and production (full TLS/X.509) deployments.

## Functions

### `getInstance() -> mongocxx::instance&`

**Brief:** Returns the singleton MongoDB driver instance, creating it on first call.

**Returns:** Reference to the global `mongocxx::instance`

**Thread Safety:** Safe (C++11 static initialization guarantees thread-safe initialization)

**Side Effects:**
- Creates the global mongocxx instance on first call

**Implementation Notes:**
The mongocxx library requires exactly one `mongocxx::instance` to exist before any other mongocxx objects are created. This function ensures proper initialization order using C++11 static local initialization.

```cpp
mongocxx::instance& getInstance() {
  static mongocxx::instance _instance{};
  return _instance;
}
```

## Class Implementations

### `DBConfig::DBConfig()`

**Brief:** Default constructor that initializes MongoDB configuration from environment variables.

**Preconditions:**
- None (uses sensible defaults if environment variables are not set)

**Postconditions:**
- `uri` member contains a valid MongoDB connection string

**Environment Variables:**
| Variable | Purpose | Example |
|----------|---------|---------|
| `ARTDAQ_DATABASE_URI` | Base MongoDB connection URI | `mongodb://dbserver:27017/mydb` |
| `ARTDAQ_DATABASE_CLIENT_CERT` | Path to X.509 client certificate | `/etc/ssl/client.pem` |
| `ARTDAQ_DATABASE_CA_CERT` | Path to CA certificate | `/etc/ssl/ca.pem` |

**Default Behavior:**
When no environment variables are set, connects to:
```
mongodb://127.0.0.1:27017/test_configuration_db
```

**TLS URI Construction:**
When certificates are provided, the URI is extended:
```
mongodb://host:27017/db&tls=true&authMechanism=MONGODB-X509&tlsCertificateKeyFile=/path/to/cert&tlsCAFile=/path/to/ca
```

### `DBConfig::DBConfig(const std::string& uri_)`

**Brief:** Explicit constructor that uses the provided URI directly.

**Parameters:**
- `uri_` - MongoDB connection URI string

**Preconditions:**
- `uri_` must not be empty

**Postconditions:**
- `uri` member equals the provided value

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `confirm` assertion failure | When `uri_` is empty |

### `MongoDB::create(DBConfig const& config) -> std::shared_ptr<MongoDB>`

**Brief:** Factory method that creates or retrieves a cached MongoDB connection.

**Parameters:**
- `config` - MongoDB connection configuration

**Returns:** Shared pointer to a (possibly cached) MongoDB instance

**Thread Safety:** Safe (mutex-protected cache)

**Side Effects:**
- May create new network connection
- Updates connection cache

**Implementation Details:**
- Cache key format: `"Thread=<thread_id>,URI=<connection_uri>"`
- Uses `std::unordered_map` for O(1) average lookup
- Protected by `std::mutex` for thread safety
- Uses `try_emplace` for efficient insert-if-not-exists

```cpp
static std::mutex instances_mutex;
static auto instances = std::unordered_map<std::string, std::shared_ptr<MongoDB>>();

std::ostringstream oss;
oss << "Thread=" << std::this_thread::get_id() << ",URI=" << config.connectionURI();
auto key = oss.str();

std::lock_guard<std::mutex> guard(instances_mutex);
return instances.try_emplace(key, std::make_shared<MongoDB>(config, {})).first->second;
```

### `MongoDB::MongoDB(DBConfig config, PassKeyIdiom const&)`

**Brief:** Constructor that initializes the MongoDB client and connection.

**Parameters:**
- `config` - MongoDB connection configuration
- `PassKeyIdiom const&` - Access control token

**Preconditions:**
- Must be called through `create()` factory method
- MongoDB server must be reachable

**Postconditions:**
- Client connected to MongoDB
- Session started

**Implementation Details:**
- Stores a reference to the singleton `mongocxx::instance`
- Creates `mongocxx::client` with the configured URI
- Extracts database name from URI to create `mongocxx::database`
- Starts a session on the client for transaction support

### `MongoDB::connection() -> mongocxx::database&`

**Brief:** Returns the database connection, initializing metadata if needed.

**Returns:** Reference to the connected `mongocxx::database`

**Side Effects:**
- Creates system metadata collection and record if not present

**Implementation Details:**
- Checks if system metadata collection has documents
- If empty, inserts a metadata record with database info
- Returns the cached database reference

### `MongoDB::list_databases() -> mongocxx::cursor`

**Brief:** Lists all databases on the MongoDB server.

**Returns:** Cursor over database descriptor documents

**Implementation Details:**
- Calls `connection()` first to ensure initialization
- Delegates to mongocxx client's `list_databases()`

## Debug Functions

### `artdaq::database::mongo::debug::connection()`

**Brief:** Enables TRACE debugging for connection operations.

**Side Effects:**
- Configures TRACE logging levels for this file
- Logs confirmation at level 10

## Performance Considerations

- **Connection caching:** The per-thread caching strategy prevents connection pool exhaustion but may create more connections than necessary in heavily threaded applications.

- **Mutex contention:** The connection cache mutex is acquired briefly for each `create()` call. High-frequency connection creation may cause contention.

- **Lazy initialization:** System metadata is only created on first database access, avoiding unnecessary initialization for read-only operations.

## Error Handling Strategy

1. **Certificate validation:** Silently skips TLS configuration if certificate files do not exist
2. **URI validation:** Uses `confirm()` assertion for empty URI in explicit constructor
3. **Connection errors:** MongoDB driver exceptions propagate to caller

## Testing Notes

- **Unit tests:** Connection tests typically require a running MongoDB instance
- **Key test cases:**
  - Default configuration (localhost)
  - Explicit URI configuration
  - Connection caching verification
  - TLS configuration (requires certificate setup)

## Maintenance Notes

- The `#pragma GCC diagnostic ignored "-Wdeprecated-declarations"` suppresses warnings about deprecated mongocxx APIs. Monitor driver updates.

- Connection caching stores connections indefinitely. For long-running applications, consider implementing a cleanup strategy.

- The session startup (`_client.start_session()`) enables transaction support but is not strictly required for basic operations.

## See Also

- [provider_mongodb.h.md](./provider_mongodb.h.md) - Header declarations
- [provider_mongodb.cpp.md](./provider_mongodb.cpp.md) - Query operations
- [provider_mongodb_readwrite.cpp.md](./provider_mongodb_readwrite.cpp.md) - I/O operations
- [External: mongocxx Connection Guide](https://mongocxx.org/mongocxx-v3/connection-pool/) - Official connection documentation

---

**Last Updated:** 2026-01-14
