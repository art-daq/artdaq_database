# ucondb_api.h

**Path:** `artdaq-database/StorageProviders/UconDB/ucondb_api.h`

**Purpose:** Declares the REST API client interface for communicating with the UconDB service. This header defines functions that wrap HTTP operations for folder management, object storage, and retrieval using the UconDB REST protocol.

## Key Concepts

### UconDB REST API

UconDB (Unified Conditions Database) is Fermilab's conditions database service that provides a REST API for configuration storage. Key concepts:

- **Folders:** Containers for related objects (similar to collections in MongoDB or directories in a filesystem)
- **Objects:** Named data blobs stored within folders, identified by unique names
- **Tags:** Labels for versioning and categorization, allowing multiple versions of objects
- **Validity Time (tv):** Unix timestamp for time-based queries, enabling point-in-time retrieval

### Result Type Pattern

Functions that can fail return `result_t`, which is a type alias for `std::pair<bool, std::string>`:
- `first`: Boolean success indicator (`true` = success, `false` = failure)
- `second`: Result string (data payload on success, error message on failure)

Use the `Success()` and `Failure()` helper functions to construct results.

## Thread Safety

- **Thread-safe:** Yes (each function creates and manages its own libcurl handle)
- **Concurrent access:** Safe for multiple threads to call API functions simultaneously
- **Locking:** No internal locking required; libcurl handles are not shared

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/SharedCommon/returned_result.h` | `result_t` type definition and `Success()`/`Failure()` helpers |
| `artdaq-database/StorageProviders/UconDB/provider_ucondb.h` | `UconDBSPtr_t` type definition |
| `artdaq-database/StorageProviders/storage_providers.h` | Base storage provider types |

## Functions

### `folders(const UconDBSPtr_t& provider) -> std::list<std::string>`

**Brief:** Retrieves the list of all folder names from the UconDB server. Folders are the top-level containers for organizing configuration objects.

**Parameters:**
- `provider` - Shared pointer to UconDB instance containing connection settings

**Preconditions:**
- `provider` must be a valid, non-null shared pointer

**Returns:** List of folder names as strings (empty list if no folders exist or on error)

**Postconditions:**
- Returned list contains all accessible folder names

**Side Effects:**
- HTTP GET request to `/app/folders` endpoint
- Network I/O with configurable timeout

**Thread Safety:** Safe

**Complexity:** O(n) where n is the number of folders

**REST Endpoint:** `GET /app/folders`

**Example Response from Server:**
```json
["Detectors", "RunControl", "Triggers"]
```

**Example:**
```cpp
#include "artdaq-database/StorageProviders/UconDB/ucondb_api.h"

using namespace artdaq::database::ucon;

void listAllFolders() {
  auto provider = UconDB::create(DBConfig{});
  auto folderList = folders(provider);

  for (const auto& folder : folderList) {
    std::cout << "Folder: " << folder << std::endl;
  }
}
```

---

### `tags(const UconDBSPtr_t& provider, std::string const& folder) -> std::list<std::string>`

**Brief:** Retrieves all tags associated with a specific folder. Tags are used for versioning and categorizing objects within a folder.

**Parameters:**
- `provider` - Shared pointer to UconDB instance
- `folder` - Name of the folder to query

**Preconditions:**
- `folder` must not be empty (validated via `confirm()`)
- `provider` must be valid and non-null

**Returns:** List of tag names as strings (empty list if no tags or on error)

**Throws:**
| Exception | Condition |
|-----------|-----------|
| Assertion failure | When `folder` is empty |

**Side Effects:**
- HTTP GET request to `/app/tags?folder={folder}` endpoint

**Thread Safety:** Safe

**REST Endpoint:** `GET /app/tags?folder={folder}`

---

### `objects(const UconDBSPtr_t& provider, std::string const& folder) -> std::list<std::string>`

**Brief:** Retrieves all object names within a specific folder. Objects are the individual configuration items stored in UconDB.

**Parameters:**
- `provider` - Shared pointer to UconDB instance
- `folder` - Name of the folder to query

**Preconditions:**
- `folder` must not be empty (validated via `confirm()`)

**Returns:** List of object names as strings (empty list if no objects or on error)

**Throws:**
| Exception | Condition |
|-----------|-----------|
| Assertion failure | When `folder` is empty |

**Side Effects:**
- HTTP GET request to `/app/objects?folder={folder}` endpoint

**Thread Safety:** Safe

**REST Endpoint:** `GET /app/objects?folder={folder}`

**Example:**
```cpp
#include "artdaq-database/StorageProviders/UconDB/ucondb_api.h"

using namespace artdaq::database::ucon;

void listObjectsInFolder(const std::string& folderName) {
  auto provider = UconDB::create(DBConfig{});
  auto objectList = objects(provider, folderName);

  std::cout << "Objects in " << folderName << ":" << std::endl;
  for (const auto& obj : objectList) {
    std::cout << "  - " << obj << std::endl;
  }
}
```

---

### `get_object(const UconDBSPtr_t& provider, std::string const& folder, std::string const& object) -> result_t`

**Brief:** Retrieves a specific object's content from UconDB. This is the primary function for reading configuration data.

**Parameters:**
- `provider` - Shared pointer to UconDB instance
- `folder` - Name of the folder containing the object
- `object` - Name (ID) of the object to retrieve

**Preconditions:**
- `folder` must not be empty
- `object` must not be empty

**Returns:** `result_t` where:
- On success: `{true, object_content}` - the raw object data
- On failure: `{false, error_message}` - CURL error description

**Throws:**
| Exception | Condition |
|-----------|-----------|
| Assertion failure | When `folder` or `object` is empty |

**Side Effects:**
- HTTP GET request to `/app/get?folder={folder}&object={object}` endpoint

**Thread Safety:** Safe

**REST Endpoint:** `GET /app/get?folder={folder}&object={object}`

**Example:**
```cpp
#include "artdaq-database/StorageProviders/UconDB/ucondb_api.h"
#include <iostream>

using namespace artdaq::database::ucon;

void readConfiguration(const std::string& folder, const std::string& objectId) {
  auto provider = UconDB::create(DBConfig{});

  auto result = get_object(provider, folder, objectId);
  if (result.first) {
    std::cout << "Configuration content:\n" << result.second << std::endl;
  } else {
    std::cerr << "Error reading object: " << result.second << std::endl;
  }
}
```

---

### `create_folder(const UconDBSPtr_t& provider, std::string const& folder) -> result_t`

**Brief:** Creates a new folder in UconDB with default permissions. If the folder already exists, returns success without error.

**Parameters:**
- `provider` - Shared pointer to UconDB instance
- `folder` - Name of the folder to create

**Preconditions:**
- `folder` must not be empty

**Returns:** `result_t` where:
- On success (newly created): `{true, "Added"}`
- On success (already exists): `{true, "AlreadyExist"}`
- On failure: `{false, error_message}`

**Throws:**
| Exception | Condition |
|-----------|-----------|
| Assertion failure | When `folder` is empty |

**Side Effects:**
- HTTP GET request to `/app/create_folder?folder={folder}&read=read_only_user&write=db_writer&drop=no`
- Creates persistent folder on server if it does not exist
- May require authentication depending on server configuration

**Thread Safety:** Safe

**REST Endpoint:** `GET /app/create_folder?folder={folder}&read=...&write=...&drop=...`

**Default Permissions Applied:**
| Permission | Value | Description |
|------------|-------|-------------|
| `read` | `read_only_user` | Default read access role |
| `write` | `db_writer` | Default write access role |
| `drop` | `no` | Folder cannot be deleted |

---

### `put_object(const UconDBSPtr_t& provider, std::string const& folder, std::string const& buffer, std::string const& object, long const tv, std::list<std::string> const& tags, std::string const& key) -> result_t`

**Brief:** Stores an object in UconDB with optional versioning metadata. This is the primary function for writing configuration data.

**Parameters:**
- `provider` - Shared pointer to UconDB instance with authentication credentials
- `folder` - Name of the folder to store in
- `buffer` - The object content to store (typically JSON data)
- `object` - Name (ID) for the object
- `tv` - Validity time as Unix timestamp (0 for current time)
- `tags` - List of tags to associate with this version (can be empty)
- `key` - Optional key for keyed access (can be empty)

**Preconditions:**
- `folder` must not be empty
- `buffer` must not be empty
- `object` must not be empty
- `provider` must have valid authentication credentials

**Returns:** `result_t` where:
- On success: `{true, ""}`
- On auth failure: `{false, "Authorization required"}`
- On other failure: `{false, error_message}`

**Throws:**
| Exception | Condition |
|-----------|-----------|
| Assertion failure | When `folder`, `buffer`, or `object` is empty |

**Side Effects:**
- HTTP PUT request to `/app/put?folder={folder}&object={object}[&tv={tv}][&tag={tags}][&key={key}]`
- Requires HTTP Digest authentication
- Creates or updates the object on the server

**Thread Safety:** Safe

**REST Endpoint:** `PUT /app/put?folder={folder}&object={object}&...`

**Query Parameters:**
| Parameter | Condition | Format |
|-----------|-----------|--------|
| `folder` | Always included | URL-encoded folder name |
| `object` | Always included | URL-encoded object ID |
| `tv` | Only if `tv != 0` | Unix timestamp (seconds) |
| `tag` | Only if `tags` not empty | Comma-separated list |
| `key` | Only if `key` not empty | URL-encoded key string |

**Example:**
```cpp
#include "artdaq-database/StorageProviders/UconDB/ucondb_api.h"
#include <iostream>

using namespace artdaq::database::ucon;

void storeConfiguration(const std::string& folder,
                        const std::string& objectId,
                        const std::string& jsonContent) {
  auto provider = UconDB::create(DBConfig{});

  // Store with current time, no tags, no key
  auto result = put_object(provider, folder, jsonContent, objectId,
                           0, {}, "");

  if (result.first) {
    std::cout << "Successfully stored configuration" << std::endl;
  } else {
    std::cerr << "Error storing: " << result.second << std::endl;
  }
}

void storeWithTags(const std::string& folder,
                   const std::string& objectId,
                   const std::string& jsonContent) {
  auto provider = UconDB::create(DBConfig{});

  // Store with tags
  std::list<std::string> tags = {"production", "v2.0"};
  auto result = put_object(provider, folder, jsonContent, objectId,
                           0, tags, "");

  if (!result.first) {
    std::cerr << "Error: " << result.second << std::endl;
  }
}
```

## Debug Functions

### `debug::UconDBAPI() -> void`

**Brief:** Enables TRACE debugging for all UconDB API operations. Configures maximum verbosity logging for troubleshooting HTTP communication issues.

**Side Effects:**
- Configures TRACE logging at maximum verbosity (all levels enabled)
- Sets TRACE name to "ucondb_api.cpp"
- May produce significant log output

**Thread Safety:** Safe (but affects global TRACE state)

**Example:**
```cpp
// Enable debugging before making API calls
artdaq::database::ucon::debug::UconDBAPI();
```

## Relationship to Other Components

### Provider Implementation

These API functions are called by `provider_ucondb_readwrite.cpp` to implement the abstract `StorageProvider` interface:

| StorageProvider Method | API Function(s) Used |
|------------------------|---------------------|
| `readDocument()` | `folders()`, `get_object()` |
| `writeDocument()` | `create_folder()`, `put_object()` |
| `findDocument()` | `objects()`, `folders()` |

### HTTP Client Architecture

All functions use libcurl for HTTP operations, configured with settings from the provider:

```
UconDB Provider Instance
    |
    +-- connection()      --> Base URL for all requests
    +-- timeout()         --> CURLOPT_TIMEOUT value (5 seconds)
    +-- authentication()  --> CURLOPT_USERPWD value (user:pass)
```

## Notes for Developers

### Authentication

- **Read operations** (`folders`, `tags`, `objects`, `get_object`) typically do not require authentication
- **Write operations** (`put_object`) require HTTP Digest authentication via `ARTDAQ_DATABASE_AUTH`
- **Folder creation** may require authentication depending on server configuration

### Error Handling Pattern

Always check the first element of `result_t` before using the second:

```cpp
auto result = get_object(provider, "folder", "object_id");
if (!result.first) {
    // Handle error: result.second contains error message
    std::cerr << "Error: " << result.second << std::endl;
    return;
}
// Use data: result.second contains object content
process(result.second);
```

### Common Pitfalls

- **Empty strings:** All folder/object name parameters are validated with `confirm()` and will cause assertion failures if empty in debug builds
- **Authentication failures:** Write operations will fail with "Authorization required" if credentials are incorrect or missing
- **Folder case sensitivity:** UconDB folder names are case-insensitive and automatically converted to lowercase by the server
- **Network timeouts:** Default timeout is 5 seconds; long-running operations may fail on slow networks

### Anti-patterns

```cpp
// DON'T do this - no error checking
auto result = get_object(provider, folder, object);
process(result.second);  // May be error message, not data!

// DO this instead - always check success
auto result = get_object(provider, folder, object);
if (result.first) {
    process(result.second);
} else {
    handleError(result.second);
}
```

## See Also

- [ucondb_api.cpp](./ucondb_api.cpp.md) - Implementation with libcurl HTTP client
- [provider_ucondb.h](./provider_ucondb.h.md) - UconDB provider class using this API
- [provider_ucondb_readwrite.cpp](./provider_ucondb_readwrite.cpp.md) - StorageProvider implementation
- [returned_result.h](../../SharedCommon/returned_result.h.md) - `result_t` type definition
- [External: UconDB Wiki](https://cdcvs.fnal.gov/redmine/projects/ucondb/wiki/Proposal) - Official UconDB REST API documentation
- [External: libcurl](https://curl.se/libcurl/c/) - HTTP client library documentation
