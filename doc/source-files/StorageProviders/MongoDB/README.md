# MongoDB Provider Documentation

## Overview

This directory contains comprehensive documentation for all source files in the MongoDB storage provider. The MongoDB provider interfaces with MongoDB database servers using the official mongocxx C++ driver, providing production-grade, scalable document storage.

**Module Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/MongoDB/`

**Documentation Created**: November 13, 2025

---

## Architecture

### MongoDB C++ Driver

Uses the official **mongocxx/bsoncxx** drivers for MongoDB communication:
- **mongocxx**: MongoDB client library
- **bsoncxx**: BSON (Binary JSON) document format library

### Communication Model

- **Protocol**: MongoDB wire protocol over TCP
- **Format**: BSON documents (binary JSON)
- **Queries**: MongoDB query language with full indexing support
- **Operations**: Find, insert, update, aggregation pipeline

---

## Documentation Files

### Core Provider Files

#### [provider_mongodb.h.md](./provider_mongodb.h.md)
MongoDB provider class and configuration.

**Key Contents**:
- `MongoDB` class - Database connection manager
- `DBConfig` struct - Connection configuration
- Type aliases for provider usage
- Debug functions

#### [provider_mongodb.cpp.md](./provider_mongodb.cpp.md)
Implementation of query operations.

**Operations**:
- `findConfigurations()` - Query configurations
- `configurationComposition()` - Get configuration entities
- `findVersions()` - Find document versions
- `findEntities()` - Discover entities
- `listCollections()` / `listDatabases()` - Database discovery
- `databaseMetadata()` - Retrieve metadata

---

### BSON/JSON Conversion

#### [mongo_json.h.md](./mongo_json.h.md) / [mongo_json.cpp.md](./mongo_json.cpp.md)
BSON ↔ JSON conversion utilities.

**Key Functions**:
- `to_json()` - Convert BSON to JSON
- `from_json()` - Convert JSON to BSON
- `to_json_unescaped()` - Human-readable JSON output
- Special handling for MongoDB types (ObjectId, Date, Binary)

---

### Read/Write Operations

#### [provider_mongodb_readwrite.cpp.md](./provider_mongodb_readwrite.cpp.md)
Document I/O operations.

**Functions**:
- `readDocument()` - Query and retrieve documents
- `writeDocument()` - Insert/update documents

**Features**:
- Full MongoDB query language
- Efficient indexed queries
- Automatic ObjectId generation
- Upsert support

---

### Supporting Files

#### [provider_mongodb_headers.h.md](./provider_mongodb_headers.h.md)
Aggregator header with mongocxx/bsoncxx includes.

#### [provider_connection.cpp.md](./provider_connection.cpp.md)
Connection management and initialization.

**Features**:
- Singleton mongocxx::instance
- Connection string parsing
- Auto-metadata creation

#### [mongo_functions.cpp.md](./mongo_functions.cpp.md)
MongoDB-specific utility functions.

#### [helper_functions.cpp.md](./helper_functions.cpp.md)
MongoDB provider helper utilities.

---

## Key Features

### Advantages

1. **Scalability**: Handles millions of documents
2. **Performance**:
   - Indexed queries (B-tree indexes)
   - Aggregation pipeline
   - Query optimization
3. **Reliability**:
   - Replication and high availability
   - Automatic failover
   - Data persistence
4. **Query Power**:
   - Rich query language
   - Aggregation framework
   - Geospatial queries
5. **Transactions**: ACID transactions (MongoDB 4.0+)
6. **Schema Flexibility**: Dynamic schema

### Production Features

- **Authentication**: Username/password, Kerberos, LDAP
- **Encryption**: TLS/SSL, encryption at rest
- **Monitoring**: Built-in monitoring and profiling
- **Backup**: Point-in-time recovery
- **Sharding**: Horizontal scaling

---

## Configuration

### Connection URI

```
mongodb://[username:password@]host[:port][/database][?options]
```

**Examples**:
```
mongodb://localhost:27017/artdaq_db
mongodb://user:pass@mongodb.fnal.gov:27017/production_db?authSource=admin
mongodb://host1:27017,host2:27017,host3:27017/artdaq_db?replicaSet=rs0
```

### Environment Variables

```bash
export ARTDAQ_DATABASE_URI="mongodb://prodserver:27017/artdaq_production"
```

---

## Usage Example

```cpp
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"

using namespace artdaq::database::mongo;

// Configure
DBConfig config("mongodb://localhost:27017/artdaq_db");

// Create provider
auto db = MongoDB::create(config);
auto provider = MongoDBProvider<JSONDocument>::create(db);

// Write document
JSONDocument doc;
doc.setCollection("Configurations");
doc.setData("{\"run\": 12345, \"config\": \"production\"}");
object_id_t id = provider->writeDocument(doc);

// Query documents
JSONDocument query;
query.setCollection("Configurations");
query.setFilter("{\"run\": {\"$gte\": 12000}}");
auto results = provider->readDocument(query);
```

---

## MongoDB Query Language

### Simple Queries
```javascript
{"run": 12345}                    // Exact match
{"run": {"$gte": 12000}}         // Greater than or equal
{"status": {"$in": ["active", "pending"]}}  // In list
```

### Complex Queries
```javascript
{"$and": [
    {"run": {"$gte": 12000}},
    {"status": "active"}
]}
```

### Aggregation
```javascript
[
    {"$match": {"status": "active"}},
    {"$group": {"_id": "$run", "count": {"$sum": 1}}},
    {"$sort": {"count": -1}}
]
```

---

## Performance Characteristics

### Time Complexity

- **Indexed Query**: O(log n)
- **Full Scan**: O(n)
- **Insert**: O(log n) with indexes
- **Update**: O(log n) with indexes

### Scalability

- **Documents per Collection**: Billions
- **Collections per Database**: Thousands
- **Document Size**: Up to 16 MB
- **Database Size**: Petabytes (with sharding)

### Recommended Use Cases

- **Large Datasets**: > 10,000 documents
- **High Performance**: Need fast indexed queries
- **Production**: Mission-critical applications
- **Scalability**: Need to grow beyond single server
- **Complex Queries**: Need aggregation and analytics

---

## Deployment Topologies

### Standalone
Single MongoDB server for development/testing.

### Replica Set
Multiple servers with automatic failover for high availability.

### Sharded Cluster
Horizontally scaled across multiple servers for massive datasets.

---

## Debugging

Enable TRACE debugging:

```cpp
artdaq::database::mongo::debug::enable();       // All debugging
artdaq::database::mongo::debug::connection();   // Connection only
artdaq::database::mongo::debug::ReadWrite();    // I/O only
```

---

## Thread Safety

**MongoDB Driver**: Thread-safe
**Concurrent Operations**: Supported
**Connection Pooling**: Automatic

---

## File List

All documented MongoDB provider files:

1. **provider_mongodb.h** - Provider declarations
2. **provider_mongodb.cpp** - Query operations
3. **provider_mongodb_headers.h** - Header aggregator
4. **provider_mongodb_readwrite.cpp** - I/O operations
5. **provider_connection.cpp** - Connection management
6. **mongo_json.h** - BSON/JSON conversion headers
7. **mongo_json.cpp** - Conversion implementations
8. **mongo_functions.cpp** - Utility functions
9. **helper_functions.cpp** - Helper utilities

---

## Related Documentation

- **FileSystemDB Provider**: [../FileSystemDB/README.md](../FileSystemDB/README.md)
- **UconDB Provider**: [../UconDB/README.md](../UconDB/README.md)
- **Storage Provider Interface**: [../storage_providers.h.md](../storage_providers.h.md)
- **MongoDB Documentation**: https://docs.mongodb.com/

---

**Documentation generated for artdaq-database MongoDB provider**
**Target audience**: Junior to intermediate C++ developers
**Last updated**: November 13, 2025
