# provider_filedb.h

**Path:** `artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h`

**Purpose:** Defines the FileSystemDB storage provider implementation for artdaq-database. This provider implements a filesystem-based database backend where directories serve as collections and JSON files serve as documents, offering a simple, portable alternative to traditional database servers ideal for development, testing, and small-scale deployments.

## Key Concepts

### Filesystem-as-Database Paradigm
The FileSystemDB provider maps database concepts to filesystem structures:
- **Database**: Root directory specified in the connection URI
- **Collection**: Subdirectory within the database root
- **Document**: JSON file named by its object ID (24-character hex string)
- **Index**: Each collection maintains an `index.json` file for fast searching

### PassKey Idiom Factory Pattern
The `FileSystemDB` class uses the PassKey idiom to enforce construction through the `create()` factory method. This ensures proper initialization and enables future connection caching if needed.

### Search Index Architecture
Each collection maintains a search index (`index.json`) that enables efficient multi-criteria queries without scanning all document files. The index maps searchable fields to document IDs, providing O(log n) lookup instead of O(n) file scanning.

## Thread Safety

- **Thread-safe:** No (for write operations)
- **Concurrent access:** Multiple readers are safe if no concurrent writers; writes require external synchronization
- **Locking:** No internal locking; use external mutex or file locking for concurrent writes

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/StorageProviders/storage_providers.h` | Base `StorageProvider` template class |
| `<vector>` | Container for file paths |
| `<boost/filesystem.hpp>` | Cross-platform filesystem operations |

## Classes/Structures

### `DBConfig`

Configuration structure for FileSystemDB connections.

**Thread Safety:** Thread-safe (immutable after construction)

#### Constructors

##### `DBConfig()`

**Brief:** Default constructor that reads the database URI from the `ARTDAQ_DATABASE_URI` environment variable.

**Postconditions:**
- `uri` member is set from environment or empty if not defined

##### `DBConfig(const std::string& uri_)`

**Brief:** Constructs configuration with an explicit URI.

**Parameters:**
- `uri_` - Database connection URI (format: `filesystemdb:///absolute/path`)

**Postconditions:**
- `uri` member equals the provided parameter

#### Members

| Member | Type | Description |
|--------|------|-------------|
| `uri` | `std::string` | Database connection URI |

#### Methods

##### `connectionURI() -> const std::string`

**Brief:** Returns the connection URI string.

**Returns:** The URI as a const string

### `FileSystemDB`

The concrete provider backend for filesystem-based storage.

**Thread Safety:** Not thread-safe for concurrent writes; thread-safe for concurrent reads when no writes are occurring

#### Methods

##### `create(DBConfig const& config) -> std::shared_ptr<FileSystemDB>` [static]

**Brief:** Factory method that creates a FileSystemDB instance using the PassKey idiom.

**Parameters:**
- `config` - Database configuration containing the URI

**Preconditions:**
- `config.uri` should be a valid filesystem URI

**Returns:** Shared pointer to the newly created FileSystemDB instance

**Postconditions:**
- Returned pointer is valid and ready for use
- Database directory is created if it does not exist

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Invalid URI or filesystem error |

**Thread Safety:** Thread-safe

**Example:**
```cpp
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"

using namespace artdaq::database::filesystem;

void createDatabase() {
  auto config = DBConfig("filesystemdb:///data/artdaq/config_db");
  auto db = FileSystemDB::create(config);
  // Database is ready for operations
}
```

##### `FileSystemDB(DBConfig config, PassKeyIdiom const&)`

**Brief:** Constructor that initializes the FileSystemDB with the given configuration. Protected by the PassKey idiom to enforce use of the `create()` factory method.

**Parameters:**
- `config` - Database configuration containing the URI
- `PassKeyIdiom const&` - PassKey token (only obtainable through `create()`)

**Thread Safety:** Thread-safe

##### `connection() -> std::string&`

**Brief:** Returns a reference to the database root directory path.

**Returns:** Reference to the connection string (database path)

**Thread Safety:** Not thread-safe if called concurrently with modifications

## Functions

### `find_subdirs(directory) -> std::list<std::string>`

**Brief:** Finds all subdirectories within a given directory, used to discover collections.

**Parameters:**
- `directory` - Path to search for subdirectories

**Returns:** List of subdirectory names (not full paths)

**Thread Safety:** Thread-safe for reading

### `find_siblingdirs(directory) -> std::list<std::string>`

**Brief:** Finds sibling directories at the same level as the given directory, used to discover databases.

**Parameters:**
- `directory` - Path whose siblings to find

**Returns:** List of sibling directory names

**Thread Safety:** Thread-safe for reading

### `find_documents(directory) -> std::list<object_id_t>`

**Brief:** Finds all document files in a directory and returns their object IDs derived from filenames.

**Parameters:**
- `directory` - Collection directory path

**Returns:** List of object IDs (derived from .json filenames without extension)

**Thread Safety:** Thread-safe for reading

### `mkdir(directory) -> std::string`

**Brief:** Creates a directory and all parent directories (equivalent to `mkdir -p`).

**Parameters:**
- `directory` - Path to create

**Returns:** The created directory path

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `boost::filesystem::filesystem_error` | Permission denied or invalid path |

**Thread Safety:** Thread-safe

### `check_if_file_exists(filepath) -> bool`

**Brief:** Checks whether a file exists at the given path.

**Parameters:**
- `filepath` - Full path to the file

**Returns:** `true` if file exists, `false` otherwise

**Thread Safety:** Thread-safe

### `list_files_in_directory(path, ext) -> file_paths_t`

**Brief:** Lists all files with a specific extension in a directory.

**Parameters:**
- `path` - Directory to search
- `ext` - File extension filter (e.g., ".json" or empty string for all files)

**Returns:** Vector of full file paths

**Thread Safety:** Thread-safe for reading

## Type Aliases

| Alias | Definition | Purpose |
|-------|------------|---------|
| `file_paths_t` | `std::vector<boost::filesystem::path>` | Container for file paths |
| `FileSystemDBProvider<TYPE>` | `StorageProvider<TYPE, FileSystemDB>` | Provider with FileSystemDB backend |
| `DBProvider<TYPE>` | `FileSystemDBProvider<TYPE>` | Shorthand alias |
| `DB` | `FileSystemDB` | Shorthand for the database class |

## Constants

| Constant | Value | Purpose |
|----------|-------|---------|
| `FILEURI` | `"filesystemdb://"` | URI scheme prefix for filesystem databases |
| `search_index` | `"index.json"` | Filename for collection search index |
| `db_name` | `"test_configuration_db"` | Default database name for testing |

## Debug Namespace

### `debug::enable()`

**Brief:** Enables all FileSystemDB debugging via TRACE, including read/write operations and index operations.

### `debug::ReadWrite()`

**Brief:** Enables read/write operation debugging specifically.

## Directory Structure

```
/data/artdaq/database/               # Database root (from URI)
|-- SystemMetadata/                  # System metadata collection
|   |-- index.json                   # Search index
|   +-- 507f1f77bcf86cd799439011.json  # Metadata document
|-- ComponentConfigs/                # User collection
|   |-- index.json                   # Search index
|   |-- 507f1f77bcf86cd799439012.json  # Document
|   +-- 507f1f77bcf86cd799439013.json  # Document
+-- RunConfigurations/               # Another collection
    |-- index.json
    +-- ...
```

## Relationship to Other Components

### StorageProvider Template
`FileSystemDB` is used as the `IMPL` template parameter for `StorageProvider<TYPE, IMPL>`. Template specializations in `provider_filedb.cpp` and `provider_filedb_readwrite.cpp` implement the actual storage operations.

### Search Index
The `SearchIndex` class (in `provider_filedb_index.h`) provides fast multi-criteria lookup. Each write operation updates the index, and read operations query the index before loading files.

### ConfigurationDB Dispatch
The ConfigurationDB layer selects FileSystemDB when the `ARTDAQ_DATABASE_URI` environment variable starts with `filesystemdb://`.

## See Also

- [provider_filedb.cpp](./provider_filedb.cpp.md) - Query operation implementations
- [provider_filedb_index.h](./provider_filedb_index.h.md) - SearchIndex class
- [provider_filedb_headers.h](./provider_filedb_headers.h.md) - Aggregator header
- [../storage_providers.h](../storage_providers.h.md) - Base provider template
- [../../ConfigurationDB/configurationdbifc.h](../../ConfigurationDB/configurationdbifc.h.md) - High-level API

## Notes for Developers

### URI Format
URIs must use absolute paths:
```cpp
// Correct
DBConfig config("filesystemdb:///data/artdaq/database");
DBConfig config("filesystemdb://$ARTDAQ_DATABASE_DIR/configs");

// Incorrect - relative paths cause undefined behavior
DBConfig config("filesystemdb://./configs");
```

### Performance Limits
Recommended limits for optimal performance:
- Documents per collection: < 10,000
- Collections per database: < 100
- Document size: < 10 MB
- Total database size: < 1 GB

For larger requirements, use MongoDB or UconDB providers.

### Error Handling
The provider throws:
- `runtime_error`: File I/O failures
- `invalid_argument`: Invalid URI or configuration
- `boost::filesystem::filesystem_error`: Filesystem operation failures

### Common Pitfalls

- **Pitfall 1:** Using relative paths in URIs leads to undefined behavior. Always use absolute paths.
- **Pitfall 2:** Concurrent writes without synchronization can corrupt the index. Use external locking.
- **Pitfall 3:** Not handling `boost::filesystem::filesystem_error` exceptions can cause crashes on permission errors.

### Anti-patterns

```cpp
// DON'T do this - relative path
auto config = DBConfig("filesystemdb://./mydb");

// DO this instead - absolute path
auto config = DBConfig("filesystemdb:///home/user/mydb");

// DON'T do this - direct construction
FileSystemDB db(config, PassKeyIdiom{});

// DO this instead - use factory method
auto db = FileSystemDB::create(config);
```
