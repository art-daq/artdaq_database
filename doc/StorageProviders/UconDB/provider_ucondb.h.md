# provider_ucondb.h

**Path:** `artdaq-database/StorageProviders/UconDB/provider_ucondb.h`

**Purpose:** Defines the UconDB storage provider that interfaces with Fermilab's Unified Configuration Database (UConDB) service via REST API. This provider enables centralized configuration management across multiple experiments with version control, access control, and web interface support.


## Key Concepts

### UConDB REST API

UConDB (Unified Conditions Database) is a Fermilab service that provides a REST API for storing and retrieving configuration data. Unlike MongoDB, UConDB uses a simpler folder/object hierarchy and is accessed via HTTP requests rather than a native database driver.

### Folder-Based Organization

UconDB uses a hierarchical folder structure for organizing configurations, similar to a filesystem. Each folder can contain objects (configurations) and is analogous to a collection in MongoDB or a directory in FileSystemDB.

### PassKey Idiom

The `PassKeyIdiom` class restricts construction of `UconDB` instances to the `create()` factory method, ensuring proper initialization through the controlled creation path. This pattern prevents direct construction while allowing `std::make_shared` to work.

## Thread Safety

- **Thread-safe:** Yes, with conditions
- **Concurrent access:** HTTP operations are thread-safe via libcurl connection pooling
- **Locking:** No internal locking; relies on libcurl's thread safety

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/StorageProviders/storage_providers.h` | Base `StorageProvider` template class |

## Namespaces

### `artdaq::database::ucon`

Primary namespace containing all UconDB provider components.

### `artdaq::database::ucon::debug`

Debug namespace with tracing enablement functions for troubleshooting.

### `artdaq::database::ucon::literal`

Compile-time constants for UconDB configuration defaults.

## Classes/Structures

### `DBConfig`

Configuration structure for UconDB connections, holding the connection URI.

**Thread Safety:** Thread-safe (immutable after construction)

#### Constructors

##### `DBConfig()`

**Brief:** Default constructor that builds the connection URI from the `ARTDAQ_DATABASE_URI` environment variable or uses default values.

**Postconditions:**
- `uri` contains a valid UconDB connection URI

**Example:**
```cpp
#include "artdaq-database/StorageProviders/UconDB/provider_ucondb.h"

using namespace artdaq::database::ucon;

void createDefaultConfig() {
  // Uses ARTDAQ_DATABASE_URI environment variable if set,
  // otherwise defaults to http://127.0.0.1:80/test_db
  DBConfig config;
  std::cout << "URI: " << config.connectionURI() << std::endl;
}
```

##### `DBConfig(const std::string& uri_)`

**Brief:** Constructs configuration with an explicit connection URI.

**Parameters:**
- `uri_` - The UconDB connection URI (must not be empty)

**Preconditions:**
- `uri_` must be a non-empty string

**Postconditions:**
- `uri` equals `uri_`

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `assertion` | When `uri_` is empty (via `confirm()` macro) |

**Example:**
```cpp
try {
  DBConfig config("http://ucondb.fnal.gov:8080/artdaq_db");
  // Use config...
} catch (...) {
  std::cerr << "Invalid configuration" << std::endl;
}
```

#### Methods

##### `connectionURI() const -> const std::string`

**Brief:** Returns the connection URI string for establishing HTTP connections to the UconDB service.

**Returns:** The connection URI

**Thread Safety:** Safe (const method on immutable data)

#### Members

| Member | Type | Description |
|--------|------|-------------|
| `uri` | `std::string` | UconDB connection URI |

---

### `UconDB`

The concrete provider backend for UconDB storage, managing HTTP connections to the REST API via libcurl.

**Thread Safety:** Thread-safe for concurrent read operations

#### Factory Method

##### `static create(DBConfig const& config) -> std::shared_ptr<UconDB>`

**Brief:** Factory method to create a managed UconDB instance with the given configuration.

**Parameters:**
- `config` - The database configuration containing the connection URI

**Returns:** Shared pointer to the created UconDB instance

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/StorageProviders/UconDB/provider_ucondb.h"

using namespace artdaq::database::ucon;

void createUconDB() {
  DBConfig config("http://ucondb.fnal.gov:8080/artdaq_db");
  auto db = UconDB::create(config);
  // Use db...
}
```

#### Constructor

##### `UconDB(DBConfig config, PassKeyIdiom const&)`

**Brief:** Constructs a UconDB instance with the given configuration. Not directly callable due to PassKeyIdiom restriction.

**Parameters:**
- `config` - The database configuration
- `PassKeyIdiom` - Access control token (only `create()` can provide this)

**Preconditions:**
- Must be called through `create()` factory method

#### Methods

##### `connection() -> std::string&`

**Brief:** Returns a mutable reference to the connection URL string for HTTP requests.

**Returns:** Reference to the connection string

**Thread Safety:** Not safe for concurrent modification

##### `timeout() const -> long`

**Brief:** Returns the HTTP request timeout value in seconds.

**Returns:** Timeout value (currently hardcoded to 5 seconds)

**Thread Safety:** Safe

##### `authentication() const -> std::string const&`

**Brief:** Returns the authentication credentials string in "user:pass" format for HTTP Basic authentication.

**Returns:** Reference to the authentication string

**Thread Safety:** Safe

---

### `UconDB::PassKeyIdiom`

Private nested class that restricts UconDB construction to the `create()` factory method.

**Thread Safety:** N/A (construction helper only)

**Purpose:** Implements the PassKey idiom to ensure all UconDB instances are created through the factory method, which guarantees proper initialization.

## Type Aliases

```cpp
using UconDBSPtr_t = std::shared_ptr<UconDB>;

template <typename TYPE>
using UconDBProvider = StorageProvider<TYPE, UconDB>;

template <typename TYPE>
using DBProvider = UconDBProvider<TYPE>;

using DB = UconDB;
```

| Alias | Purpose |
|-------|---------|
| `UconDBSPtr_t` | Shared pointer type for UconDB instances |
| `UconDBProvider<TYPE>` | StorageProvider specialized for UconDB |
| `DBProvider<TYPE>` | Generic alias for UconDBProvider |
| `DB` | Short alias for UconDB class |

## Literal Constants

Defined in `artdaq::database::ucon::literal`:

| Constant | Value | Purpose |
|----------|-------|---------|
| `UCONURI` | `"http://"` | URI scheme prefix for UconDB |
| `search_index` | `"index.json"` | Search index filename |
| `db_name` | `"test_db"` | Default database name for testing |
| `hostname` | `"127.0.0.1"` | Default hostname |
| `port` | `80` | Default HTTP port |

## Functions

### Debug Functions

#### `debug::enable() -> void`

**Brief:** Enables all UconDB debugging including API calls and read/write operations.

**Side Effects:**
- Configures TRACE logging for all UconDB components
- Calls `debug::UconDBAPI()` and `debug::ReadWrite()`

**Thread Safety:** Safe

#### `debug::ReadWrite() -> void`

**Brief:** Enables debugging specifically for read/write operations.

**Side Effects:**
- Configures TRACE logging for read/write operations

**Thread Safety:** Safe

## Relationship to Other Components

### StorageProvider Template

`UconDB` serves as the `IMPL` template parameter for `StorageProvider<TYPE, IMPL>`. Template specializations in `provider_ucondb.cpp` and `provider_ucondb_readwrite.cpp` implement the actual storage operations.

### UconDB API Client

The provider uses `ucondb_api.h/cpp` for all REST API operations via libcurl, which handles HTTP requests, authentication, and response parsing.

### ConfigurationDB Dispatch

The ConfigurationDB layer selects UconDB when the `ARTDAQ_DATABASE_URI` environment variable starts with `http://` (indicating a UconDB REST endpoint rather than a MongoDB or filesystem URI).

## Notes for Developers

### Environment Variables

| Variable | Purpose | Example |
|----------|---------|---------|
| `ARTDAQ_DATABASE_URI` | Override default connection URI | `http://ucondb.fnal.gov:8080/artdaq_db` |
| `ARTDAQ_DATABASE_AUTH` | Authentication credentials | `username:password` |

### Network Requirements

- Requires network connectivity to UconDB service
- May require Fermilab VPN for internal services
- Default timeout is 5 seconds per request

### Common Pitfalls

- **Empty URI:** Passing an empty string to `DBConfig(const std::string&)` triggers an assertion failure
- **Authentication:** If `ARTDAQ_DATABASE_AUTH` is not set, defaults to `"user:pass"` which may not be valid for production
- **Timeouts:** Network timeouts are set to 5 seconds; long-running operations may fail

### Anti-patterns

```cpp
// DON'T do this - bypasses factory method (won't compile)
UconDB db(config, {}); // PassKeyIdiom is private

// DO this instead - use factory method
auto db = UconDB::create(config);
```

## See Also

- [storage_providers.h.md](../storage_providers.h.md) - Base StorageProvider template
- [ucondb_api.h.md](./ucondb_api.h.md) - REST API client interface
- [provider_ucondb.cpp.md](./provider_ucondb.cpp.md) - Query operation implementations
- [provider_ucondb_readwrite.cpp.md](./provider_ucondb_readwrite.cpp.md) - Read/write implementations
- [External: UConDB Wiki](https://cdcvs.fnal.gov/redmine/projects/ucondb/wiki) - UConDB documentation
