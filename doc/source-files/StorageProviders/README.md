# StorageProviders Module

**Path:** `artdaq-database/StorageProviders/`

**Purpose:** Implements the storage provider abstraction layer for artdaq-database, providing a unified interface for different database backends (FileSystemDB, MongoDB, UconDB). This module enables the application to work with different storage systems through a consistent API using the Strategy pattern with compile-time template specialization.

## Overview

The StorageProviders module provides:
- A template-based `StorageProvider<TYPE, IMPL>` class that defines the storage interface
- Three concrete backend implementations for different use cases
- Consistent read/write/query operations across all backends
- Backend selection via URI scheme in `ARTDAQ_DATABASE_URI` environment variable

## Architecture

```
StorageProvider<TYPE, IMPL>
        |
        +-- StorageProvider<JSONDocument, FileSystemDB>
        |       |-- Filesystem-based storage
        |       +-- JSON files in directory structure
        |
        +-- StorageProvider<JSONDocument, MongoDB>
        |       |-- MongoDB document database
        |       +-- mongocxx C++ driver
        |
        +-- StorageProvider<JSONDocument, UconDB>
                |-- Fermilab UConDB service
                +-- REST API via libcurl
```

## Files in This Module

### Root Files

| File | Purpose |
|------|---------|
| `common.h` | Aggregator header for common includes |
| `storage_providers.h` | Template class definition and interface |
| `storage_providers.cpp` | Database metadata generation |

### Subdirectories

| Directory | Provider | Use Case |
|-----------|----------|----------|
| `FileSystemDB/` | Filesystem-based | Development, testing, small deployments |
| `MongoDB/` | MongoDB database | Production, large-scale, performance-critical |
| `UconDB/` | UConDB REST service | Fermilab experiments, centralized management |

## Key Concepts

### StorageProvider Template

The core `StorageProvider<TYPE, IMPL>` template provides:
- Factory pattern via `create()` method with PassKey idiom
- Document read/write operations
- Query operations (findConfigurations, findVersions, findEntities)
- Collection and database discovery

```cpp
template <typename TYPE, typename IMPL>
class StorageProvider final {
public:
    static ProviderSPtr create(std::shared_ptr<IMPL> const& provider);

    template <typename FILTER>
    std::vector<TYPE> readDocument(FILTER const&);

    object_id_t writeDocument(TYPE const&);

    template <typename FILTER>
    std::vector<FILTER> findConfigurations(FILTER const&);

    // ... additional query methods
};
```

### Backend Selection

Backends are selected by URI scheme:
```
filesystemdb:///path/to/db    -> FileSystemDB
mongodb://host:port/db        -> MongoDB
ucondb://http://host/db       -> UconDB
```

### Template Specialization

Each backend provides explicit template specializations for all StorageProvider methods in their implementation files.

## Provider Comparison

| Feature | FileSystemDB | MongoDB | UconDB |
|---------|-------------|---------|--------|
| **Storage** | Local filesystem | MongoDB server | REST service |
| **Scalability** | < 10K docs | Billions | Moderate |
| **Thread Safety** | No | Yes | Yes |
| **Setup** | None | MongoDB server | Network |
| **Best For** | Development | Production | Fermilab |
| **Performance** | O(log n) index | O(log n) index | Network latency |

## Usage Example

```cpp
// Select provider based on URI
std::string uri = getenv("ARTDAQ_DATABASE_URI");

if (uri.find("filesystemdb://") == 0) {
    // FileSystemDB provider
    auto db = FileSystemDB::create(DBConfig(uri));
    auto provider = FileSystemDBProvider<JSONDocument>::create(db);
}
else if (uri.find("mongodb://") == 0) {
    // MongoDB provider
    auto db = MongoDB::create(DBConfig(uri));
    auto provider = MongoDBProvider<JSONDocument>::create(db);
}
else if (uri.find("ucondb://") == 0) {
    // UconDB provider
    auto db = UconDB::create(DBConfig(uri));
    auto provider = UconDBProvider<JSONDocument>::create(db);
}

// All providers have the same interface
JSONDocument doc;
doc.setCollection("Configurations");
doc.setData("{\"run\": 12345}");
auto id = provider->writeDocument(doc);

JSONDocument query;
query.setFilter("{\"run\": 12345}");
auto results = provider->readDocument(query);
```

## Configuration

### Environment Variable

```bash
export ARTDAQ_DATABASE_URI="mongodb://localhost:27017/artdaq_db"
```

### URI Formats

**FileSystemDB:**
```
filesystemdb:///absolute/path/to/database
filesystemdb://$ARTDAQ_DATABASE_DIR/configs
```

**MongoDB:**
```
mongodb://localhost:27017/database
mongodb://user:pass@host:port/database?authSource=admin
mongodb://host1,host2,host3/database?replicaSet=rs0
```

**UconDB:**
```
ucondb://http://ucondb.fnal.gov:8080/database
ucondb://https://ucondb.fnal.gov/database
```

## Choosing a Provider

### FileSystemDB
**Best for:**
- Development and testing
- Small deployments (< 10K documents)
- Portable configurations
- Human-readable storage

**Limitations:**
- Not thread-safe
- Limited scalability
- No transactions

### MongoDB
**Best for:**
- Production deployments
- Large-scale storage
- High-performance queries
- Enterprise features

**Requirements:**
- MongoDB server
- Network connectivity

### UconDB
**Best for:**
- Fermilab experiments
- Centralized management
- Multi-site coordination
- Audit requirements

**Requirements:**
- Network to Fermilab
- Authentication credentials

## Debugging

Enable TRACE debugging per provider:
```cpp
artdaq::database::filesystem::debug::enable();  // FileSystemDB
artdaq::database::mongo::debug::enable();       // MongoDB
artdaq::database::ucon::debug::enable();        // UconDB
```

## Related Documentation

- [FileSystemDB Provider](./FileSystemDB/README.md)
- [MongoDB Provider](./MongoDB/README.md)
- [UconDB Provider](./UconDB/README.md)

## Adding New Backends

To add a new storage backend:
1. Create a new subdirectory (e.g., `NewDB/`)
2. Implement `DBConfig` struct for connection configuration
3. Implement main provider class with `create()` factory method
4. Provide template specializations for all `StorageProvider` methods
5. Add URI scheme detection in ConfigurationDB dispatch layer
6. Create documentation following the template structure
