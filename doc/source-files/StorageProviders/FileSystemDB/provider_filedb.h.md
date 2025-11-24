# provider_filedb.h

## File Overview

This header file defines the FileSystemDB storage provider implementation for artdaq-database. It provides a filesystem-based database backend that stores JSON documents as files in a hierarchical directory structure, offering a simple, portable alternative to traditional database servers.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h`

**Lines of Code**: 76

**Purpose**: Filesystem-based storage provider with directory-as-database and file-as-document paradigm

## Dependencies

### Project Headers
- `"artdaq-database/StorageProviders/storage_providers.h"` - Base StorageProvider template

### Standard Library
- `<vector>` - Container for file paths

### Third-Party Libraries
- `<boost/filesystem.hpp>` - Cross-platform filesystem operations

## Namespace: artdaq::database::filesystem

All types and functions in this file are declared within the `artdaq::database::filesystem` namespace.

## Filesystem Utility Functions

### find_subdirs
```cpp
std::list<std::string> find_subdirs(std::string const& directory);
```

**Purpose**: Find all subdirectories within a given directory.

**Parameters**:
- `directory` - Path to search

**Returns**: List of subdirectory names (not full paths)

**Usage**: Used to discover collections (each collection is a subdirectory).

---

### find_siblingdirs
```cpp
std::list<std::string> find_siblingdirs(std::string const& directory);
```

**Purpose**: Find all sibling directories (directories at the same level).

**Parameters**:
- `directory` - Reference directory path

**Returns**: List of sibling directory names

**Usage**: Used to discover databases (each database is a sibling directory).

---

### find_documents
```cpp
std::list<object_id_t> find_documents(std::string const& directory);
```

**Purpose**: Find all document files in a directory.

**Parameters**:
- `directory` - Path to search

**Returns**: List of document IDs (filenames without extension)

**Usage**: Discover all documents in a collection.

**Implementation Note**: Returns object IDs derived from filenames.

---

### mkdir
```cpp
std::string mkdir(std::string const& directory);
```

**Purpose**: Create directory and all parent directories (like `mkdir -p`).

**Parameters**:
- `directory` - Path to create

**Returns**: The created directory path

**Usage**: Ensure directory exists before writing files.

**Behavior**:
- Creates intermediate directories if needed
- Returns successfully if directory already exists
- Throws exception on permission errors

---

### check_if_file_exists
```cpp
bool check_if_file_exists(std::string const& filepath);
```

**Purpose**: Check if a file exists.

**Parameters**:
- `filepath` - Path to check

**Returns**: true if file exists, false otherwise

**Usage**: Validate file presence before reading.

---

### list_files_in_directory
```cpp
using file_paths_t = std::vector<boost::filesystem::path>;
file_paths_t list_files_in_directory(boost::filesystem::path const& path,
                                     std::string const& ext);
```

**Purpose**: List all files with a specific extension in a directory.

**Parameters**:
- `path` - Directory to search
- `ext` - File extension to filter (e.g., ".json")

**Returns**: Vector of file paths

**Usage**: Find all JSON files in a collection directory.

---

## Debug Namespace

### debug::enable
```cpp
namespace debug {
    void enable();
    void ReadWrite();
}
```

**Purpose**: Enable verbose TRACE debugging for the FileSystemDB provider.

**Usage**:
```cpp
artdaq::database::filesystem::debug::enable();
// Now all FileSystemDB operations will produce debug output
```

**Functions**:
- `enable()` - Enable all FileSystemDB debugging
- `ReadWrite()` - Enable read/write operation debugging specifically

---

## Literal Constants

### Namespace: literal

```cpp
namespace literal {
    constexpr auto FILEURI = "filesystemdb://";
    constexpr auto search_index = "index.json";
    constexpr auto db_name = "test_configuration_db";
}
```

**Constants**:

#### FILEURI
- **Value**: `"filesystemdb://"`
- **Purpose**: URI scheme for filesystem database connections
- **Usage**: Prefix for filesystem database URIs
- **Example**: `"filesystemdb:///data/artdaq/configs"`

#### search_index
- **Value**: `"index.json"`
- **Purpose**: Filename for collection search index
- **Usage**: Each collection directory contains an `index.json` file for fast searching

#### db_name
- **Value**: `"test_configuration_db"`
- **Purpose**: Default database name for testing
- **Usage**: Used in unit tests and examples

---

## Struct: DBConfig

### Purpose

Configuration structure for FileSystemDB connections.

### Declaration

```cpp
struct DBConfig final {
    DBConfig();
    DBConfig(const std::string& uri_);

    std::string uri;
    const std::string connectionURI() const { return uri; };
};
```

### Members

#### uri
- **Type**: `std::string`
- **Purpose**: Database connection URI
- **Format**: `"filesystemdb://<absolute-path>"`
- **Example**: `"filesystemdb:///data/artdaq/database"`

### Constructors

#### Default Constructor
```cpp
DBConfig();
```

**Purpose**: Create empty configuration.

**Post-condition**: `uri` is empty string.

#### URI Constructor
```cpp
DBConfig(const std::string& uri_);
```

**Purpose**: Create configuration with specified URI.

**Parameters**:
- `uri_` - Database URI

**Usage Example**:
```cpp
DBConfig config("filesystemdb:///data/configs");
```

### Methods

#### connectionURI
```cpp
const std::string connectionURI() const;
```

**Purpose**: Get the connection URI.

**Returns**: The URI string

**Usage**: Access the configured database path.

---

## Class: FileSystemDB

### Purpose

The FileSystemDB class implements the concrete provider backend for filesystem-based storage. It manages the connection to a filesystem directory that serves as the database.

### Factory Method and Pass-Key Idiom

```cpp
class FileSystemDB final {
 public:
  static std::shared_ptr<FileSystemDB> create(DBConfig const& config);

  class PassKeyIdiom {
   private:
    friend std::shared_ptr<FileSystemDB> FileSystemDB::create(DBConfig const& config);
    PassKeyIdiom() {}
  };

  explicit FileSystemDB(DBConfig config, PassKeyIdiom const&);
  // ...
};
```

### Factory Method: create

```cpp
static std::shared_ptr<FileSystemDB> create(DBConfig const& config);
```

**Purpose**: Factory method to create FileSystemDB instances.

**Parameters**:
- `config` - Database configuration (URI)

**Returns**: Shared pointer to new FileSystemDB instance

**Usage Example**:
```cpp
DBConfig config("filesystemdb:///data/artdaq/database");
auto db = FileSystemDB::create(config);
```

**Design**: Uses Pass-Key Idiom to enforce factory pattern.

### Constructor

```cpp
explicit FileSystemDB(DBConfig config, PassKeyIdiom const&);
```

**Purpose**: Construct FileSystemDB instance.

**Parameters**:
- `config` - Database configuration
- `PassKeyIdiom const&` - Pass-key to restrict direct construction

**Access**: Public, but can only be called through `create()` factory.

**Initialization**: Stores configuration and prepares for database operations.

### Methods

#### connection
```cpp
std::string& connection();
```

**Purpose**: Get the database connection path.

**Returns**: Reference to the connection string (filesystem path)

**Usage**: Access the root directory path for the database.

**Example**:
```cpp
auto db = FileSystemDB::create(config);
std::string path = db->connection();
// path = "/data/artdaq/database"
```

### Private Members

```cpp
private:
  DBConfig _config;
  std::string _client;
  std::string _connection;
```

**Members**:
- **_config**: Stored configuration
- **_client**: Client identifier (may be unused for filesystem)
- **_connection**: Processed connection path

---

## Type Aliases

### FileSystemDBProvider<TYPE>
```cpp
template <typename TYPE>
using FileSystemDBProvider = StorageProvider<TYPE, FileSystemDB>;
```

**Purpose**: Convenience alias for FileSystemDB storage provider.

**Template Parameters**:
- `TYPE` - Document type (typically JSONDocument)

**Usage Example**:
```cpp
using Provider = FileSystemDBProvider<JSONDocument>;
auto provider = Provider::create(db_impl);
```

### DBProvider<TYPE>
```cpp
template <typename TYPE>
using DBProvider = FileSystemDBProvider<TYPE>;
```

**Purpose**: Short alias for FileSystemDBProvider.

**Usage**: More concise code in provider implementations.

### DB
```cpp
using DB = FileSystemDB;
```

**Purpose**: Short alias for FileSystemDB class.

**Usage**: Concise code in implementation files.

---

## Filesystem Database Structure

### Directory Layout

```
/data/artdaq/database/               # Database root (from URI)
├── SystemMetadata/                  # System metadata collection
│   ├── index.json                   # Search index
│   └── 507f1f77bcf86cd799439011.json  # Metadata document
├── ComponentConfigs/                # User collection
│   ├── index.json                   # Search index
│   ├── 507f1f77bcf86cd799439012.json  # Document
│   └── 507f1f77bcf86cd799439013.json  # Document
└── RunConfigurations/               # Another collection
    ├── index.json
    └── ...
```

### Structure Details

1. **Database Root**: Top-level directory specified in URI
2. **Collections**: Each subdirectory is a collection
3. **Documents**: Each JSON file is a document
4. **Index**: Each collection has an `index.json` for searching
5. **Naming**: Documents named by their object ID

### Advantages

- **Portability**: Works on any filesystem
- **Simplicity**: Easy to understand and debug
- **Accessibility**: Files can be examined with text editors
- **Backup**: Standard filesystem backup tools work
- **Version Control**: Can be tracked with git (for small databases)

### Limitations

- **Performance**: Slower than dedicated databases for large datasets
- **Concurrency**: Limited by filesystem locking
- **Scalability**: Not suitable for very large databases
- **Transactions**: No native transaction support

---

## Usage Example: Complete Workflow

```cpp
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"

using namespace artdaq::database;
using namespace artdaq::database::filesystem;

// 1. Create configuration
DBConfig config("filesystemdb:///data/artdaq/configs");

// 2. Create database instance
auto db = FileSystemDB::create(config);

// 3. Create storage provider
auto provider = FileSystemDBProvider<JSONDocument>::create(db);

// 4. Write a document
JSONDocument doc;
doc.setCollection("RunConfigs");
doc.setData("{\"run\": 12345, \"detectors\": [\"TPC\", \"ECAL\"]}");
object_id_t id = provider->writeDocument(doc);

// 5. Read documents
JSONDocument query;
query.setCollection("RunConfigs");
query.setFilter("{\"run\": 12345}");
auto results = provider->readDocument(query);

// 6. List collections
JSONDocument listQuery;
auto collections = provider->listCollections(listQuery);
for (auto const& col : collections) {
    std::cout << "Collection: " << col << std::endl;
}
```

---

## Implementation Details

### Template Specializations

The actual read/write operations are implemented as template specializations in `provider_filedb.cpp`:

```cpp
template <>
template <>
std::vector<JSONDocument>
StorageProvider<JSONDocument, FileSystemDB>::readDocument(JSONDocument const& query);

template <>
template <>
object_id_t
StorageProvider<JSONDocument, FileSystemDB>::writeDocument(JSONDocument const& doc);
```

This separation allows the header to remain clean while the implementation handles specific logic.

### Search Index

Each collection maintains an `index.json` file that enables fast searching without reading all documents. See `provider_filedb_index.h` for details on the index structure and search capabilities.

---

## Thread Safety

**FileSystemDB** operations are generally **not thread-safe** without external synchronization:

- **Reads**: Safe if no concurrent writes to same file
- **Writes**: Require external locking for same document
- **Index Updates**: Not atomic - concurrent writes may corrupt index

**Recommendation**: Use external synchronization (mutex, file locking) for concurrent access.

---

## Error Handling

### Validation
- URI must not be empty
- Path must be accessible
- Directory creation must succeed

### Exceptions Thrown
- `runtime_error` - File I/O failures
- `invalid_argument` - Invalid URI or configuration
- `boost::filesystem::filesystem_error` - Filesystem operation failures

### Error Recovery
```cpp
try {
    auto db = FileSystemDB::create(config);
} catch (boost::filesystem::filesystem_error const& e) {
    std::cerr << "Filesystem error: " << e.what() << std::endl;
    // Handle error (check permissions, disk space, etc.)
}
```

---

## Performance Considerations

### Optimization Strategies

1. **Index Usage**: Always use index for searches (don't scan all files)
2. **Caching**: Cache frequently accessed documents
3. **Bulk Operations**: Group writes to minimize index updates
4. **Path Expansion**: Cache expanded paths (environment variables)

### Scalability Limits

- **Documents per Collection**: < 10,000 recommended
- **Collections per Database**: < 100 recommended
- **Document Size**: < 10 MB recommended
- **Total Database Size**: < 1 GB recommended

For larger requirements, use MongoDB or UconDB providers.

---

## Testing

### Unit Testing

```cpp
// Test database creation
DBConfig config("filesystemdb:///tmp/test_db");
auto db = FileSystemDB::create(config);
assert(db != nullptr);

// Test directory creation
std::string path = db->connection();
assert(boost::filesystem::exists(path));

// Test writing and reading
auto provider = FileSystemDBProvider<JSONDocument>::create(db);
JSONDocument doc("{\"test\": \"data\"}");
doc.setCollection("TestCollection");
object_id_t id = provider->writeDocument(doc);
assert(id != ouid_invalid);
```

### Integration Testing

Test with realistic configurations and document structures to verify filesystem operations work correctly across different platforms.

---

## Related Files

- **provider_filedb.cpp** - Implementation of storage operations
- **provider_filedb_index.h/cpp** - Search index implementation
- **provider_filedb_readwrite.cpp** - Read/write operation implementations
- **provider_filedb_headers.h** - Common includes for FileSystemDB
- **provider_connection.cpp** - Connection management
- **filesystem_functions.cpp** - Filesystem utility implementations
- **storage_providers.h** - Base StorageProvider template

---

## Best Practices

### URI Format
```cpp
// GOOD: Absolute path
DBConfig config("filesystemdb:///data/artdaq/database");

// GOOD: With environment variable
DBConfig config("filesystemdb://$ARTDAQ_DATABASE_DIR/configs");

// BAD: Relative path (undefined behavior)
// DBConfig config("filesystemdb://./configs");
```

### Error Checking
```cpp
// GOOD: Check for errors
try {
    auto db = FileSystemDB::create(config);
    auto provider = FileSystemDBProvider<JSONDocument>::create(db);
} catch (std::exception const& e) {
    // Handle error
}

// BAD: Assume success
// auto db = FileSystemDB::create(config);  // May throw
```

### Resource Management
```cpp
// GOOD: Use RAII with shared_ptr
{
    auto db = FileSystemDB::create(config);
    // db automatically destroyed when out of scope
}

// GOOD: Long-lived provider
auto db = FileSystemDB::create(config);
// Keep db alive as long as needed
```

---

**Documentation generated for artdaq-database FileSystemDB provider**
