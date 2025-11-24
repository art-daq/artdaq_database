# StorageProviders Module Documentation

## Overview

This directory contains comprehensive documentation for all source files in the artdaq-database StorageProviders module. The StorageProviders module implements multiple database backend adapters, allowing artdaq-database to work with different storage systems through a unified interface.

**Module Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/`

**Documentation Created**: November 13, 2025

---

## Module Architecture

### Provider Pattern

The StorageProviders module implements a provider pattern where:
- **StorageProvider<TYPE, IMPL>** - Template class providing unified interface
- **TYPE** - Document type (e.g., JSONDocument)
- **IMPL** - Concrete provider implementation (FileSystemDB, MongoDB, UconDB)

### Three Provider Implementations

1. **FileSystemDB** - Filesystem-based storage
2. **MongoDB** - MongoDB database server
3. **UconDB** - REST API to UConDB service

Each provider implements the same interface but uses different storage backends.

---

## Documentation Structure

### Root Files

#### [common.h.md](./common.h.md)
Aggregator header providing common utilities for all providers.

**Contents**: Includes SharedCommon headers for standard library utilities.

#### [storage_providers.h.md](./storage_providers.h.md)
Core StorageProvider template class interface.

**Key Contents**:
- `StorageProvider<TYPE, IMPL>` template class
- Pass-Key Idiom for factory pattern
- Template method declarations for all operations
- `make_database_metadata()` function

**Operations Defined**:
- `readDocument()` / `writeDocument()` - Document I/O
- `findConfigurations()` - Query configurations
- `configurationComposition()` - Get configuration entities
- `findVersions()` - Find document versions
- `findEntities()` - Discover entities
- `listCollections()` / `listDatabases()` - Database discovery
- `databaseMetadata()` - Retrieve metadata
- `searchCollection()` - Generic search

#### [storage_providers.cpp.md](./storage_providers.cpp.md)
Implementation of `make_database_metadata()` function.

**Purpose**: Generate JSON metadata for new databases including creation info, format version, and system details.

---

## Provider Implementations

### FileSystemDB Provider

**Location**: [FileSystemDB/README.md](./FileSystemDB/README.md)

**Description**: Filesystem-based database where directories are collections and JSON files are documents.

**Best For**:
- Development and testing
- Small databases (< 1000 documents)
- Portable databases
- No server requirements

**Features**:
- Simple directory structure
- Human-readable JSON files
- JSON-based search index
- No external dependencies

**Limitations**:
- Limited scalability
- No transaction support
- Limited concurrency
- Performance constraints

**Files Documented**: 8 files (provider, index, read/write, connection, filesystem utilities)

---

### MongoDB Provider

**Location**: [MongoDB/README.md](./MongoDB/README.md)

**Description**: Production-grade database using MongoDB server via mongocxx C++ driver.

**Best For**:
- Production deployments
- Large databases (millions of documents)
- High-performance queries
- Scalability requirements

**Features**:
- Full MongoDB query language
- Indexed queries (B-tree)
- Replication and sharding
- ACID transactions
- Aggregation pipeline

**Advantages**:
- Scalability to petabytes
- High availability
- Rich query capabilities
- Enterprise features

**Files Documented**: 9 files (provider, BSON/JSON conversion, read/write, connection, utilities)

---

### UconDB Provider

**Location**: [UconDB/README.md](./UconDB/README.md)

**Description**: REST API client for Fermilab's Unified Configuration Database service.

**Best For**:
- Multi-experiment environments
- Centralized configuration management
- Version control requirements
- Collaborative editing

**Features**:
- REST HTTP/HTTPS API
- Folder-based organization
- Version tagging
- Web interface
- Audit trail

**Advantages**:
- No local database installation
- Centralized service
- Multi-site access
- Built-in version control

**Files Documented**: 7 files (provider, REST API client, read/write, connection)

---

## Choosing a Provider

### Decision Matrix

| Requirement | FileSystemDB | MongoDB | UconDB |
|-------------|--------------|---------|--------|
| Development/Testing | ✓✓✓ | ✓✓ | ✓ |
| Production | ✓ | ✓✓✓ | ✓✓✓ |
| Scalability | ✓ | ✓✓✓ | ✓✓ |
| Performance | ✓✓ | ✓✓✓ | ✓✓ |
| No Server Required | ✓✓✓ | ✗ | ✓✓✓ |
| Offline Operation | ✓✓✓ | ✓✓ | ✗ |
| Multi-Site | ✓ | ✓✓ | ✓✓✓ |
| Version Control | ✓ | ✓ | ✓✓✓ |
| Setup Complexity | ✓✓✓ | ✓✓ | ✓✓✓ |

### Recommendations

**Use FileSystemDB when**:
- Developing or testing
- Need portable database
- Want human-readable files
- Database is small (< 1000 documents)

**Use MongoDB when**:
- Running in production
- Need high performance
- Database is large (> 10,000 documents)
- Need complex queries
- Require scalability

**Use UconDB when**:
- Part of Fermilab experiment
- Need centralized configuration
- Require version control
- Multiple sites/collaborators
- Want web interface access

---

## Common Interface

All providers implement the same interface through the StorageProvider template:

```cpp
// Create provider (example with FileSystemDB)
auto db = FileSystemDB::create(config);
auto provider = StorageProvider<JSONDocument, FileSystemDB>::create(db);

// Same interface for all providers
auto results = provider->readDocument(query);
object_id_t id = provider->writeDocument(doc);
auto configs = provider->findConfigurations(filter);
auto collections = provider->listCollections(query);
```

This allows switching providers by changing configuration, not code.

---

## Configuration

### Connection URIs

Each provider uses a different URI scheme:

```cpp
// FileSystemDB
"filesystemdb:///absolute/path/to/database"

// MongoDB
"mongodb://hostname:port/database"

// UconDB
"ucondb://https://hostname:port/database"
```

### Environment Variable

All providers check `ARTDAQ_DATABASE_URI` environment variable:

```bash
# Development (FileSystemDB)
export ARTDAQ_DATABASE_URI="filesystemdb:///tmp/test_db"

# Production (MongoDB)
export ARTDAQ_DATABASE_URI="mongodb://prodserver:27017/artdaq_prod"

# Fermilab (UconDB)
export ARTDAQ_DATABASE_URI="ucondb://https://ucondb.fnal.gov:8080/artdaq"
```

---

## Performance Comparison

### Document Read (by ID)

| Provider | Time Complexity | Typical Latency |
|----------|----------------|-----------------|
| FileSystemDB | O(1) | ~1 ms |
| MongoDB | O(log n) | ~1 ms (indexed) |
| UconDB | O(1) | ~10-100 ms (network) |

### Query (multiple criteria)

| Provider | Time Complexity | Typical Time |
|----------|----------------|--------------|
| FileSystemDB | O(k * log n) | ~10 ms |
| MongoDB | O(log n) | ~1-5 ms (indexed) |
| UconDB | O(log n) | ~50-200 ms (network) |

### Document Write

| Provider | Time Complexity | Typical Latency |
|----------|----------------|-----------------|
| FileSystemDB | O(1) | ~1-5 ms |
| MongoDB | O(log n) | ~1-5 ms |
| UconDB | O(1) | ~50-200 ms (network) |

---

## Database Metadata

All providers store database metadata in the `SystemMetadata` collection:

```json
{
  "document": {
    "name": "artdaq_configuration_db",
    "uri": "mongodb://server:27017/artdaq_db",
    "locale": "en_US.UTF-8",
    "create_time": "2025-11-13T10:30:45.123-0600",
    "create_user": "artdaq",
    "uname": {...},
    "database_format": 1
  }
}
```

**Purpose**: Track database creation, format version (for migrations), and system info.

---

## Thread Safety

| Provider | Thread-Safe | Notes |
|----------|-------------|-------|
| FileSystemDB | No | Requires external synchronization |
| MongoDB | Yes | Driver handles thread safety |
| UconDB | Yes | HTTP client is thread-safe |

---

## Total Documentation

### Files Documented

- **Root**: 3 files (common.h, storage_providers.h/cpp)
- **FileSystemDB**: 8 files
- **MongoDB**: 9 files
- **UconDB**: 7 files
- **README files**: 4 files (root + 3 providers)

**Total**: **31 documentation files** covering all source files in StorageProviders module

### Documentation Features

Each file documentation includes:
- File overview and purpose
- Dependencies
- Detailed function/class documentation
- Usage examples
- Design rationale
- Performance considerations
- Thread safety notes
- Related files
- Best practices

---

## Usage Examples

### Switching Providers

```cpp
// Configuration-driven provider selection
std::string uri = getenv("ARTDAQ_DATABASE_URI");

if (uri.find("filesystemdb://") == 0) {
    auto db = FileSystemDB::create(DBConfig(uri));
    auto provider = FileSystemDBProvider<JSONDocument>::create(db);
    // Use provider...
}
else if (uri.find("mongodb://") == 0) {
    auto db = MongoDB::create(DBConfig(uri));
    auto provider = MongoDBProvider<JSONDocument>::create(db);
    // Use provider...
}
else if (uri.find("ucondb://") == 0) {
    auto db = UconDB::create(DBConfig(uri));
    auto provider = UconDBProvider<JSONDocument>::create(db);
    // Use provider...
}
```

### Uniform Operations

```cpp
// Same code works with all providers
template <typename PROVIDER>
void processConfigurations(std::shared_ptr<PROVIDER> provider) {
    JSONDocument query("{}");
    auto configs = provider->findConfigurations(query);

    for (auto const& config : configs) {
        auto docs = provider->readDocument(config);
        // Process documents...
    }
}
```

---

## Debugging

Enable debugging for all providers:

```cpp
// FileSystemDB
artdaq::database::filesystem::debug::enable();

// MongoDB
artdaq::database::mongo::debug::enable();

// UconDB
artdaq::database::ucon::debug::enable();
```

---

## Migration Between Providers

To migrate from one provider to another:

1. **Export** from source provider (readDocument all collections)
2. **Transform** if needed (adjust for destination provider)
3. **Import** to destination provider (writeDocument to new database)
4. **Verify** data integrity
5. **Update** configuration to point to new provider

Example migration script structure:

```cpp
// Read from FileSystemDB
auto source = FileSystemDBProvider<JSONDocument>::create(...);
auto collections = source->listCollections(query);

// Write to MongoDB
auto dest = MongoDBProvider<JSONDocument>::create(...);
for (auto const& col : collections) {
    auto docs = source->readDocument(col);
    for (auto const& doc : docs) {
        dest->writeDocument(doc);
    }
}
```

---

## Related Documentation

- **SharedCommon Module**: [../SharedCommon/README.md](../SharedCommon/README.md)
- **JsonDocument Module**: [../JsonDocument/README.md](../JsonDocument/README.md)
- **BasicTypes Module**: [../BasicTypes/README.md](../BasicTypes/README.md)

---

**Documentation generated for artdaq-database StorageProviders module**
**Target audience**: Junior to intermediate C++ developers
**Last updated**: November 13, 2025

---

## Quick Reference

### FileSystemDB
- **URI**: `filesystemdb:///path`
- **Best For**: Development, small databases
- **Docs**: [FileSystemDB/README.md](./FileSystemDB/README.md)

### MongoDB
- **URI**: `mongodb://host:port/db`
- **Best For**: Production, large databases
- **Docs**: [MongoDB/README.md](./MongoDB/README.md)

### UconDB
- **URI**: `ucondb://https://host:port/db`
- **Best For**: Multi-site, centralized
- **Docs**: [UconDB/README.md](./UconDB/README.md)
