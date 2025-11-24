# FileSystemDB Provider Documentation

## Overview

This directory contains comprehensive documentation for all source files in the FileSystemDB storage provider. The FileSystemDB provider implements a filesystem-based database backend where directories serve as collections and JSON files serve as documents.

**Module Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/FileSystemDB/`

**Documentation Created**: November 13, 2025

---

## Architecture

### Storage Model

- **Database**: Root directory (e.g., `/data/artdaq/configs`)
- **Collections**: Subdirectories (e.g., `ComponentConfigs/`)
- **Documents**: JSON files named by object ID (e.g., `507f1f77bcf86cd799439011.json`)
- **Index**: Each collection has `index.json` for fast searching

### Directory Structure Example

```
/data/artdaq/database/
├── SystemMetadata/
│   ├── index.json
│   └── 507f1f77bcf86cd799439011.json
├── ComponentConfigs/
│   ├── index.json
│   ├── 507f1f77bcf86cd799439012.json
│   └── 507f1f77bcf86cd799439013.json
└── RunConfigurations/
    ├── index.json
    └── ...
```

---

## Documentation Files

### Core Provider Files

#### [provider_filedb.h.md](./provider_filedb.h.md)
Main provider class and filesystem utility functions.

**Key Contents**:
- `FileSystemDB` class - Database connection management
- `DBConfig` struct - Connection configuration
- Filesystem utilities (`mkdir`, `find_subdirs`, `find_documents`, etc.)
- Type aliases and factory methods

**Size**: Comprehensive documentation of provider interface

#### [provider_filedb.cpp.md](./provider_filedb.cpp.md)
Implementation of storage provider operations.

**Key Contents**:
- `findConfigurations()` - Find all configurations
- `configurationComposition()` - Get configuration entities
- `findVersions()` - Find document versions
- `findEntities()` - Find all entities
- `listCollections()` - List database collections
- `listDatabases()` - List sibling databases
- `databaseMetadata()` - Get database metadata

**Highlights**: Complete template specializations for all query operations

---

### Search Index

#### [provider_filedb_index.h.md](./provider_filedb_index.h.md)
SearchIndex class for fast document lookup.

**Key Contents**:
- `SearchIndex` class - In-memory inverted index
- `findDocumentIDs()` - Multi-criteria document search
- `addDocument()` / `removeDocument()` - Index updates
- Index structure and algorithms

**Purpose**: Enable O(log n) queries instead of O(n) file scanning

#### [provider_filedb_index.cpp.md](./provider_filedb_index.cpp.md)
SearchIndex implementation.

**Key Features**:
- Multi-criteria AND logic
- Efficient intersection algorithms
- Auto-save on destruction
- JSON-based persistent storage

---

### Read/Write Operations

#### [provider_filedb_readwrite.cpp.md](./provider_filedb_readwrite.cpp.md)
Core document read/write operations.

**Key Functions**:
- `readDocument()` - Query index and read files
- `writeDocument()` - Write files and update index

**Integration**: Seamless index integration for all I/O operations

---

### Supporting Files

#### [provider_filedb_headers.h.md](./provider_filedb_headers.h.md)
Aggregator header for implementation files.

**Purpose**: Centralized includes and type aliases

#### [provider_connection.cpp.md](./provider_connection.cpp.md)
Connection management and initialization.

**Key Features**:
- DBConfig constructors
- Environment variable handling
- Automatic metadata creation on first connection

#### [filesystem_functions.cpp.md](./filesystem_functions.cpp.md)
Filesystem utility implementations.

**Key Functions**:
- `mkdir()` - Create directories with permission checks
- `find_subdirs()` - Discover collections
- `find_documents()` - List documents in collection
- `check_if_file_exists()` - File existence check

---

## Key Features

### Advantages

1. **Simplicity**: Easy to understand and debug
2. **Portability**: Works on any filesystem
3. **Human-Readable**: JSON files can be examined with text editors
4. **Version Control**: Can track small databases with git
5. **No Server**: No database server required
6. **Backup-Friendly**: Standard filesystem backup tools work

### Limitations

1. **Performance**: Slower than dedicated databases for large datasets
2. **Concurrency**: Limited by filesystem locking
3. **Scalability**: Not suitable for very large databases
4. **Transactions**: No native transaction support

### Recommended Use Cases

- **Development**: Local testing and development
- **Small Deployments**: < 1000 documents per collection
- **Portable**: Databases that need to move between systems
- **Simple**: Applications that don't need advanced database features

---

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

---

## Configuration

### Environment Variables

- **ARTDAQ_DATABASE_URI**: Override default database location
  ```bash
  export ARTDAQ_DATABASE_URI="filesystemdb:///data/production/configs"
  ```

- **ARTDAQ_DATABASE_DATADIR**: Base directory for databases
  ```bash
  export ARTDAQ_DATABASE_DATADIR="/data/artdaq"
  ```

### URI Format

```
filesystemdb://<absolute-path>
filesystemdb://$ARTDAQ_DATABASE_DIR/configs
```

---

## Debugging

Enable TRACE debugging for all FileSystemDB operations:

```cpp
artdaq::database::filesystem::debug::enable();
```

This enables verbose logging for:
- File operations
- Index queries
- Document reads/writes
- Directory scanning

---

## Performance Characteristics

### Time Complexity

- **Read**: O(k * log n + m) where k=criteria, n=index size, m=matches
- **Write**: O(1) per document
- **Index Query**: O(log n) per criterion

### Scalability Limits

- **Documents per Collection**: < 10,000 recommended
- **Collections per Database**: < 100 recommended
- **Document Size**: < 10 MB recommended
- **Total Database Size**: < 1 GB recommended

For larger requirements, use MongoDB or UconDB providers.

---

## Thread Safety

**Not Thread-Safe**: FileSystemDB operations require external synchronization for concurrent access.

**Recommendations**:
- Use mutex or file locking for concurrent writes
- Multiple readers OK if no concurrent writers
- Index updates are not atomic

---

## File List

All documented files in this module:

1. **provider_filedb.h** - Provider class declarations
2. **provider_filedb.cpp** - Query operation implementations
3. **provider_filedb_index.h** - SearchIndex class
4. **provider_filedb_index.cpp** - SearchIndex implementation
5. **provider_filedb_headers.h** - Header aggregator
6. **provider_filedb_readwrite.cpp** - Read/write operations
7. **provider_connection.cpp** - Connection management
8. **filesystem_functions.cpp** - Filesystem utilities

---

## Migration and Compatibility

### Database Format Version

Each database includes a `database_format` version in SystemMetadata. This enables:
- Detection of old format databases
- Automatic migration to new formats
- Version-specific code paths

### Future Compatibility

When upgrading:
1. Check `database_format` in SystemMetadata
2. Run migration if needed
3. Update format version

---

## Related Documentation

- **StorageProviders Overview**: [../README.md](../README.md)
- **MongoDB Provider**: [../MongoDB/README.md](../MongoDB/README.md)
- **UconDB Provider**: [../UconDB/README.md](../UconDB/README.md)
- **Storage Provider Interface**: [../storage_providers.h.md](../storage_providers.h.md)

---

**Documentation generated for artdaq-database FileSystemDB provider**
**Target audience**: Junior to intermediate C++ developers
**Last updated**: November 13, 2025
