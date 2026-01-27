# MongoDB Provider

**Path:** `artdaq-database/StorageProviders/MongoDB/`

**Purpose:** Implements a production-grade MongoDB storage provider for artdaq-database using the official mongocxx C++ driver. This provider offers scalable document storage with full MongoDB query capabilities, connection pooling, replica set support, and enterprise features for mission-critical deployments.

## Overview

The MongoDB provider interfaces with MongoDB database servers to provide:
- Scalable storage for millions of documents
- Full MongoDB query language support including aggregation pipelines
- Indexed queries with O(log n) performance
- Automatic connection pooling and caching
- Replica set and sharded cluster support
- TLS/X.509 authentication for secure deployments

## Architecture

```
+-------------------+
|  ConfigurationDB  |  (dispatch_mongodb.cpp selects this provider)
+--------+----------+
         |
+--------v----------+
|   MongoDB Class   |  (provider_mongodb.h)
|  - create()       |  Factory with connection caching
|  - connection()   |  Returns mongocxx::database&
+--------+----------+
         |
+--------v----------+
| StorageProvider<> |  Template specializations:
|  - readDocument   |  (provider_mongodb_readwrite.cpp)
|  - writeDocument  |  (provider_mongodb_readwrite.cpp)
|  - findConfigs    |  (provider_mongodb.cpp)
|  - findEntities   |  (provider_mongodb.cpp)
|  - listCollections|  (provider_mongodb.cpp)
+--------+----------+
         |
+--------v----------+
|    mongocxx       |  Official MongoDB C++ driver
|    bsoncxx        |  BSON serialization
+-------------------+
```

## Files in This Module

| File | Purpose |
|------|---------|
| [provider_mongodb.h](./provider_mongodb.h.md) | Main provider class (`MongoDB`), `DBConfig`, type aliases, debug functions |
| [provider_mongodb.cpp](./provider_mongodb.cpp.md) | Query operation implementations: find configurations, entities, versions, collections, databases |
| [provider_mongodb_headers.h](./provider_mongodb_headers.h.md) | Aggregated includes for implementation files, namespace aliases |
| [provider_mongodb_readwrite.cpp](./provider_mongodb_readwrite.cpp.md) | Core document I/O: `readDocument()` and `writeDocument()` |
| [provider_connection.cpp](./provider_connection.cpp.md) | Connection management, singleton instance, TLS configuration |
| [mongo_json.h](./mongo_json.h.md) | BSON/JSON conversion function declarations |
| [mongo_json.cpp](./mongo_json.cpp.md) | BSON/JSON conversion implementations |
| [helper_functions.cpp](./helper_functions.cpp.md) | Query utilities including wildcard-to-regex transformation |

## Key Classes

### MongoDB

Main provider class that manages the connection to a MongoDB database server. Uses the mongocxx driver for all database operations.

**Key Methods:**
- `create(DBConfig const&)` - Factory method with connection caching
- `connection()` - Returns reference to the mongocxx database
- `list_databases()` - Enumerates databases on the server

### DBConfig

Configuration structure holding the MongoDB connection URI with support for:
- Username/password authentication
- X.509 certificate authentication
- Replica set connections
- TLS encryption

## Usage Example

```cpp
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"

using namespace artdaq::database;
using namespace artdaq::database::mongo;

void example() {
  // Configure MongoDB connection
  DBConfig config("mongodb://localhost:27017/artdaq_db");

  // Create provider (connection is cached per thread)
  auto db = MongoDB::create(config);

  // Create a StorageProvider for JSONDocument operations
  auto provider = StorageProvider<JSONDocument, MongoDB>::create(db);

  // Write a document
  JSONDocument writeDoc(R"({
    "document": {
      "collection": "Configurations",
      "version": "1.0.0",
      "entities": [{"name": "detector_front_end"}],
      "data": {"threshold": 100, "enabled": true}
    }
  })");

  try {
    object_id_t id = provider->writeDocument(writeDoc);
    std::cout << "Stored with ID: " << id << std::endl;

    // Read it back
    JSONDocument readQuery(R"({
      "collection": "Configurations",
      "filter": {"version": "1.0.0"}
    })");

    auto results = provider->readDocument(readQuery);
    for (const auto& doc : results) {
      std::cout << "Found: " << doc << std::endl;
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}
```

## Configuration

### URI Format

```
mongodb://[username:password@]host[:port][/database][?options]
```

**Examples:**
```
mongodb://localhost:27017/artdaq_db
mongodb://user:pass@mongodb.fnal.gov:27017/production_db?authSource=admin
mongodb://host1:27017,host2:27017,host3:27017/artdaq_db?replicaSet=rs0
```

### Environment Variables

| Variable | Purpose |
|----------|---------|
| `ARTDAQ_DATABASE_URI` | Set MongoDB connection URI (overrides defaults) |
| `ARTDAQ_DATABASE_CLIENT_CERT` | Path to X.509 client certificate for TLS |
| `ARTDAQ_DATABASE_CA_CERT` | Path to CA certificate for TLS verification |

## MongoDB Query Language

### Simple Queries
```javascript
{"run": 12345}                              // Exact match
{"run": {"$gte": 12000}}                    // Greater than or equal
{"status": {"$in": ["active", "pending"]}}  // In list
```

### Wildcard Searches
The provider supports `*` wildcards that are converted to MongoDB regex:
```cpp
// Search for all configurations starting with "production_"
JSONDocument query(R"({"configurations": "production_*"})");
auto results = provider->findConfigurations(query);
```

### Complex Queries
```javascript
{"$and": [
    {"run": {"$gte": 12000}},
    {"status": "active"}
]}
```

## Thread Safety

**Thread-safe:** The mongocxx driver handles all thread safety concerns.
- Automatic connection pooling per thread
- Multiple threads can share a `MongoDB` instance
- Each thread gets its own connection from the cache

## Performance Characteristics

| Operation | Complexity |
|-----------|------------|
| Indexed query | O(log n) |
| Full scan | O(n) |
| Insert | O(log n) with indexes |
| Update | O(log n) with indexes |

### Scalability Limits

| Resource | Capacity |
|----------|----------|
| Documents per collection | Billions |
| Collections per database | Thousands |
| Document size | Up to 16 MB |
| Database size | Petabytes (with sharding) |

## Debugging

Enable TRACE debugging:
```cpp
artdaq::database::mongo::debug::enable();       // All debugging
artdaq::database::mongo::debug::connection();   // Connection only
artdaq::database::mongo::debug::ReadWrite();    // I/O only
```

## Deployment Topologies

### Standalone
Single MongoDB server for development and testing.

### Replica Set
Multiple servers with automatic failover for high availability.

### Sharded Cluster
Horizontally scaled across multiple servers for massive datasets.

## When to Use MongoDB

- Large datasets (> 10,000 documents)
- Need fast indexed queries
- Production/mission-critical applications
- Need to scale beyond single server
- Complex queries with aggregation
- Need enterprise features (auth, encryption)

## Dependencies

### External
- **mongocxx** - MongoDB C++ driver (included in build)
- **bsoncxx** - BSON serialization library (included in build)

### Internal
- `StorageProviders/storage_providers.h` - Base template
- `StorageProviders/common.h` - Common utilities
- `JsonDocument/` - Document model
- `SharedCommon/` - Helper functions

## See Also

- [StorageProviders README](../README.md) - Overview of all providers
- [FileSystemDB](../FileSystemDB/README.md) - Alternative file-based provider
- [External: MongoDB C++ Driver](https://mongocxx.org/) - Official driver documentation
- [External: MongoDB Manual](https://www.mongodb.com/docs/manual/) - MongoDB reference
