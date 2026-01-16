# FileSystemDB Provider

**Path:** `artdaq-database/StorageProviders/FileSystemDB/`

**Purpose:** Implements a filesystem-based storage provider for artdaq-database where directories serve as collections and JSON files serve as documents. This provider offers a simple, portable, human-readable alternative to traditional database servers, ideal for development, testing, and small-scale deployments.

## Overview

The FileSystemDB provider maps database concepts to filesystem structures:
- **Database**: Root directory specified in the connection URI
- **Collection**: Subdirectory within the database root
- **Document**: JSON file named by its 24-character hex object ID
- **Index**: Each collection maintains an `index.json` for fast searching

## Directory Structure

```
/data/artdaq/database/               # Database root (from URI)
|-- SystemMetadata/                  # System metadata collection
|   |-- index.json                   # Search index
|   +-- 507f1f77bcf86cd799439011.json  # Metadata document
|-- ComponentConfigs/                # User collection
|   |-- index.json
|   |-- 507f1f77bcf86cd799439012.json
|   +-- 507f1f77bcf86cd799439013.json
+-- RunConfigurations/               # Another collection
    |-- index.json
    +-- ...
```

## Files in This Module

| File | Purpose |
|------|---------|
| `provider_filedb.h` | Main provider class, DBConfig, filesystem utilities |
| `provider_filedb.cpp` | Query operation implementations |
| `provider_filedb_headers.h` | Aggregated includes for implementation files |
| `provider_filedb_index.h` | SearchIndex class for fast lookups |
| `provider_filedb_index.cpp` | SearchIndex implementation |
| `provider_filedb_readwrite.cpp` | Document read/write operations |
| `provider_connection.cpp` | Connection management and initialization |
| `filesystem_functions.cpp` | Filesystem utility implementations |

## Key Classes

### FileSystemDB
Main provider class that manages the connection to a filesystem directory serving as the database.

### DBConfig
Configuration structure holding the database URI and providing environment variable support.

### SearchIndex
In-memory inverted index with JSON persistence, enabling O(log n) lookups instead of O(n) file scanning.

## Usage Example

```cpp
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"

using namespace artdaq::database::filesystem;

// Configure database
DBConfig config("filesystemdb:///data/artdaq/configs");

// Create database instance
auto db = FileSystemDB::create(config);

// Create storage provider
auto provider = FileSystemDBProvider<JSONDocument>::create(db);

// Write document
JSONDocument doc;
doc.setCollection("RunConfigs");
doc.setData("{\"run\": 12345}");
object_id_t id = provider->writeDocument(doc);

// Read documents
JSONDocument query;
query.setCollection("RunConfigs");
query.setFilter("{\"run\": 12345}");
auto results = provider->readDocument(query);
```

## Configuration

### Environment Variables

| Variable | Purpose |
|----------|---------|
| `ARTDAQ_DATABASE_URI` | Override default database location |
| `ARTDAQ_DATABASE_DATADIR` | Base directory for default URI |

### URI Format

```
filesystemdb:///absolute/path/to/database
filesystemdb://$ARTDAQ_DATABASE_DIR/configs
```

## Advantages

1. **Simplicity**: Easy to understand and debug
2. **Portability**: Works on any filesystem
3. **Human-Readable**: JSON files can be examined with text editors
4. **Version Control**: Small databases can be tracked with git
5. **No Server**: No database server required
6. **Backup-Friendly**: Standard filesystem backup tools work

## Limitations

1. **Performance**: Slower than dedicated databases for large datasets
2. **Concurrency**: Not thread-safe; requires external synchronization
3. **Scalability**: Not suitable for very large databases
4. **Transactions**: No native transaction support

## Recommended Limits

| Resource | Limit |
|----------|-------|
| Documents per collection | < 10,000 |
| Collections per database | < 100 |
| Document size | < 10 MB |
| Total database size | < 1 GB |

For larger requirements, use MongoDB or UconDB providers.

## Thread Safety

**Not thread-safe**: Requires external synchronization for concurrent access.
- Multiple readers safe if no concurrent writers
- Writes require mutex or file locking
- Index updates are not atomic

## Debugging

Enable TRACE debugging:
```cpp
artdaq::database::filesystem::debug::enable();
```
