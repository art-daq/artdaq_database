# provider_connection.cpp

**Path:** `artdaq-database/StorageProviders/UconDB/provider_connection.cpp`

**Implements:** [provider_ucondb.h](./provider_ucondb.h.md)

**Purpose:** Implements connection management for the UconDB storage provider, including URI configuration, environment variable handling, and HTTP connection initialization. This file provides the constructors and accessors that configure how the UconDB client connects to the REST API server.

## Implementation Overview

This file provides the implementations of `DBConfig` constructors and `UconDB` class methods that handle connection establishment to the UconDB REST API. The connection configuration supports both explicit URI specification and environment variable overrides, allowing flexible deployment configurations.

## Key Algorithms

### URI Resolution Algorithm

The default constructor implements a priority-based URI resolution:

**Steps:**
1. Initialize with default URI: `http://127.0.0.1:80/test_db` (composed from literals)
2. Check `ARTDAQ_DATABASE_URI` environment variable
3. If set, expand any embedded environment variables using `expand_environment_variables()`
4. Validate that the expanded value starts with `"http://"` prefix
5. If valid, override the default URI with the environment value

**Why this approach:** Allows compile-time defaults for development while enabling runtime configuration for production deployments without code changes. The prefix check ensures only valid UconDB URIs are accepted.

### Authentication Resolution

Authentication credentials are resolved at UconDB construction time:

**Steps:**
1. Check `ARTDAQ_DATABASE_AUTH` environment variable
2. If set, use its value as the credentials string
3. Otherwise default to `"user:pass"`

**Why this approach:** Keeps credentials out of code and configuration files, allowing secure deployment practices. The default value allows development without explicit credential setup.

## Class Implementations

### `DBConfig`

#### `DBConfig()`

**Brief:** Default constructor that builds the connection URI from environment variables or falls back to compile-time defaults.

**Implementation Details:**
1. Initializes `uri` member using string concatenation of literals:
   - `dbucl::UCONURI` ("http://")
   - `dbucl::hostname` ("127.0.0.1")
   - `:` + `dbucl::port` (80)
   - `/` + `dbucl::db_name` ("test_db")
   - Result: `"http://127.0.0.1:80/test_db"`
2. Reads `ARTDAQ_DATABASE_URI` environment variable
3. If set, expands any nested environment variable references
4. If expanded value starts with `"http://"` and is longer than the prefix, uses it as the URI

**Thread Safety:** Safe (constructor, no shared state)

**Environment Variables:**
| Variable | Purpose |
|----------|---------|
| `ARTDAQ_DATABASE_URI` | Override default connection URI |

**Example:**
```bash
# Set environment variable
export ARTDAQ_DATABASE_URI="http://ucondb.fnal.gov:8080/production"

# Code uses environment value
DBConfig config;  // uri = "http://ucondb.fnal.gov:8080/production"
```

---

#### `DBConfig(const std::string& uri_)`

**Brief:** Constructor that accepts an explicit URI string, bypassing environment variable resolution.

**Parameters:**
- `uri_` - The complete UconDB connection URI (must not be empty)

**Preconditions:**
- `uri_` must not be empty (validated via `confirm()` assertion)

**Postconditions:**
- `uri` member equals `uri_` parameter

**Throws:**
| Exception | Condition |
|-----------|-----------|
| Assertion failure | When `uri_` is empty |

**Thread Safety:** Safe (constructor)

**Implementation Details:**
- Directly assigns `uri_` to `uri` member
- No environment variable expansion or format validation
- Relies on `confirm()` macro to validate non-empty input

---

### `UconDB`

#### `connection() -> std::string&`

**Brief:** Returns a mutable reference to the connection URL for HTTP operations.

**Returns:** Reference to `_connection` string containing the base URL

**Thread Safety:** Not safe for concurrent modification; safe for concurrent read access

**Usage:**
```cpp
auto provider = UconDB::create(DBConfig{});
std::string url = provider->connection();
url.append("/app/folders");  // Build full endpoint URL
```

---

#### `timeout() const -> long`

**Brief:** Returns the HTTP request timeout value in seconds.

**Returns:** Hardcoded value of `5` seconds

**Thread Safety:** Safe (const method returning constant value)

**Side Effects:** None

**Note:** The timeout value is currently not configurable. See Maintenance Notes for enhancement suggestions.

---

#### `authentication() const -> std::string const&`

**Brief:** Returns the authentication credentials for HTTP basic/digest authentication.

**Returns:** Const reference to `_userpass` string in "username:password" format

**Thread Safety:** Safe (const method returning const reference)

**Usage:**
```cpp
// Used by libcurl for HTTP authentication
curl_easy_setopt(curl, CURLOPT_USERPWD, provider->authentication().c_str());
```

---

#### `UconDB(DBConfig config, PassKeyIdiom const&)`

**Brief:** Constructs a UconDB instance with the provided connection configuration. Uses the PassKey idiom to enforce factory-based construction.

**Parameters:**
- `config` - Database configuration (moved into instance)
- `PassKeyIdiom const&` - Access control token (unused parameter, enforces that only `UconDB::create()` can construct instances)

**Preconditions:**
- Must be called via `UconDB::create()` factory method (enforced by PassKeyIdiom)

**Postconditions:**
- Instance is ready for use with configured connection settings

**Thread Safety:** Safe (constructor)

**Implementation Details:**
1. Moves `config` into `_config` member
2. Initializes `_client` with connection URI (currently unused placeholder)
3. Initializes `_connection` with connection URI for HTTP operations
4. Reads `ARTDAQ_DATABASE_AUTH` environment variable for credentials
5. Falls back to `"user:pass"` if environment variable not set

**Member Initialization:**
| Member | Initialized From |
|--------|------------------|
| `_config` | `config` parameter (moved) |
| `_client` | `_config.connectionURI()` |
| `_connection` | `_config.connectionURI()` |
| `_userpass` | `ARTDAQ_DATABASE_AUTH` env var or `"user:pass"` |

**Example:**
```cpp
// Correct: Use factory method
auto provider = UconDB::create(DBConfig{});

// Incorrect: Direct construction will fail to compile
// UconDB db(config, ???);  // Cannot create PassKeyIdiom externally
```

## Environment Variables

| Variable | Purpose | Default | Format |
|----------|---------|---------|--------|
| `ARTDAQ_DATABASE_URI` | Connection URI | `http://127.0.0.1:80/test_db` | `http://host:port/database` |
| `ARTDAQ_DATABASE_AUTH` | HTTP credentials | `user:pass` | `username:password` |

### Environment Variable Expansion

The `ARTDAQ_DATABASE_URI` value is processed through `expand_environment_variables()` which allows nested references:

```bash
export DB_HOST="ucondb.fnal.gov"
export DB_PORT="8080"
export ARTDAQ_DATABASE_URI="http://${DB_HOST}:${DB_PORT}/mydb"
# Result: http://ucondb.fnal.gov:8080/mydb
```

## URI Format

```
http://hostname[:port]/database
```

| Component | Required | Default | Description |
|-----------|----------|---------|-------------|
| Protocol | Yes | `http://` | Must be `http://` for UconDB (HTTPS not explicitly validated) |
| Hostname | Yes | `127.0.0.1` | UconDB server hostname or IP address |
| Port | No | `80` | HTTP port number |
| Database | Yes | `test_db` | Database/folder namespace |

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/BasicTypes/basictypes.h` | Basic type definitions |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | JSON document utilities |
| `artdaq-database/SharedCommon/common.h` | Helper functions including `expand_environment_variables()` |
| `artdaq-database/StorageProviders/UconDB/provider_ucondb.h` | Class declarations |

## Performance Considerations

- Constructor performs environment variable lookups (O(1) system calls)
- No network connections are established during construction
- Connection reuse is handled by libcurl in the API layer (`ucondb_api.cpp`)
- String operations are minimal and performed once at construction

## Error Handling Strategy

- `confirm()` macro validates non-empty URI in explicit constructor
- Environment variable reads return `nullptr` if not set (handled gracefully)
- No exceptions thrown during construction
- Invalid URIs (missing http:// prefix) are silently ignored, falling back to default

## Testing Notes

- **Unit tests:** Test with and without environment variables set
- **Key test cases:**
  - Default construction without environment variables
  - Default construction with valid `ARTDAQ_DATABASE_URI`
  - Default construction with invalid `ARTDAQ_DATABASE_URI` (missing http://)
  - Explicit URI construction with valid URI
  - Empty URI construction (should trigger assertion)
  - Authentication with and without `ARTDAQ_DATABASE_AUTH`
  - Environment variable expansion with nested references

## Maintenance Notes

### Timeout Value

The timeout is currently hardcoded to 5 seconds in `timeout()`. If this needs to be configurable:
1. Add `timeout` member to `DBConfig`
2. Parse from URI query parameter or separate environment variable
3. Update `timeout()` to return `_config.timeout`

### HTTPS Support

The code currently checks for `"http://"` prefix only. To add HTTPS support:
1. Modify prefix check to accept both `"http://"` and `"https://"`
2. Ensure libcurl SSL is properly configured in `ucondb_api.cpp`
3. Consider adding certificate verification options

### Unused _client Member

The `_client` member is initialized but appears unused. It may be:
- A placeholder for future connection pooling
- Leftover from refactoring
- Reserved for API compatibility

## See Also

- [provider_ucondb.h](./provider_ucondb.h.md) - Header file with class declarations
- [ucondb_api.cpp](./ucondb_api.cpp.md) - HTTP client using connection settings
- [SharedCommon/helper_functions.cpp](../../SharedCommon/helper_functions.cpp.md) - `expand_environment_variables()` implementation
- [External: libcurl](https://curl.se/libcurl/) - HTTP client library used for connections
