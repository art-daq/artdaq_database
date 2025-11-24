# provider_connection.cpp

## File Overview

This file implements the DBConfig and FileSystemDB connection management. It handles database URI configuration, environment variable expansion, and automatic database metadata creation on first connection.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/FileSystemDB/provider_connection.cpp`

**Lines of Code**: 83

**Purpose**: Database connection initialization and configuration

## DBConfig Constructors

### Default Constructor
```cpp
DBConfig::DBConfig()
```

**Behavior**:
1. Sets default URI from `ARTDAQ_DATABASE_DATADIR` environment variable
2. Checks for `ARTDAQ_DATABASE_URI` override
3. Validates URI has `filesystemdb://` prefix
4. Removes trailing slashes

**Default URI**: `filesystemdb://${ARTDAQ_DATABASE_DATADIR}/filesystemdb/test_configuration_db`

### URI Constructor
```cpp
DBConfig::DBConfig(const std::string& uri_)
```

**Parameters**:
- `uri_` - Full database URI (e.g., `filesystemdb:///data/configs`)

**Validation**: Confirms URI is not empty.

---

## FileSystemDB Constructor

```cpp
FileSystemDB::FileSystemDB(DBConfig config, PassKeyIdiom const&)
```

**Initialization**:
- Stores configuration
- Sets `_client` to connection URI
- Sets `_connection` to URI with trailing slash

---

## FileSystemDB::connection()

```cpp
std::string& FileSystemDB::connection()
```

### Purpose

Get the database connection path, creating database metadata if it doesn't exist.

### Algorithm

1. **Check Metadata**: Look for `SystemMetadata` collection
2. **If Exists**: Return connection string
3. **If Not Exists**: Create database metadata
   - Generate object ID
   - Create metadata JSON with `make_database_metadata()`
   - Write to `SystemMetadata/<oid>.json`
   - Return connection string

### Metadata Creation

```cpp
auto oid = generate_oid();

std::ostringstream oss;
oss << "{";
oss << make_database_metadata("artdaq", expand_environment_variables(_config.connectionURI())) << ",";
oss << db::quoted_(jsonliteral::id) << ": { ";
oss << db::quoted_(jsonliteral::oid) << ":" << db::quoted_(oid) << "}";
oss << "}";

auto filename = mkdir(collection) + "/" + oid + ".json";
db::write_buffer_to_file(oss.str(), filename);
```

**Metadata Includes**:
- Database name and URI
- Creation timestamp
- Creating user
- System information
- Database format version

### First-Use Initialization

This function implements lazy database initialization:
- **First call**: Creates `SystemMetadata` directory and metadata file
- **Subsequent calls**: Returns existing connection

---

## URI Format

### Valid URI Formats

```
filesystemdb:///absolute/path/to/database
filesystemdb://$ARTDAQ_DATABASE_DIR/configs
filesystemdb:///${HOME}/artdaq/db
```

### URI Processing

1. Environment variables expanded
2. `filesystemdb://` prefix stripped for filesystem operations
3. Trailing slashes normalized

---

## Environment Variables

### ARTDAQ_DATABASE_URI

Overrides default database location:
```bash
export ARTDAQ_DATABASE_URI="filesystemdb:///data/production/configs"
```

### ARTDAQ_DATABASE_DATADIR

Used in default URI construction:
```bash
export ARTDAQ_DATABASE_DATADIR="/data/artdaq"
```

---

## Usage Example

```cpp
// Using default configuration
DBConfig default_config;
auto db1 = FileSystemDB::create(default_config);

// Using custom URI
DBConfig custom_config("filesystemdb:///data/my_database");
auto db2 = FileSystemDB::create(custom_config);

// Get connection (creates metadata on first call)
std::string path = db2->connection();
// path = "/data/my_database/"
```

---

## Related Files

- **provider_filedb.h** - DBConfig and FileSystemDB declarations
- **storage_providers.cpp** - `make_database_metadata()` implementation
- **SharedCommon/helper_functions.cpp** - Environment variable expansion

---

**Documentation generated for artdaq-database FileSystemDB provider**
