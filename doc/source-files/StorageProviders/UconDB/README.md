# UconDB Provider Documentation

## Overview

This directory contains comprehensive documentation for all source files in the UconDB storage provider. The UconDB provider interfaces with Fermilab's Unified Configuration Database (UConDB) service via REST API, providing centralized configuration management for multiple experiments.

**Module Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/UconDB/`

**Documentation Created**: November 13, 2025

---

## Architecture

### UConDB Service

UConDB is Fermilab's centralized configuration database service:
- **Multi-Experiment**: Shared service for DUNE, NOvA, MicroBooNE, etc.
- **Web Service**: RESTful HTTP/HTTPS API
- **Version Control**: Full configuration history
- **Web Interface**: GUI for browsing and editing configurations

### Communication Model

- **Protocol**: HTTP/HTTPS REST API
- **Format**: JSON request/response
- **Authentication**: Token-based or username/password
- **Operations**: GET, POST, PUT via REST endpoints

---

## Documentation Files

### Core Provider Files

#### [provider_ucondb.h.md](./provider_ucondb.h.md)
UConDB provider class and configuration.

**Key Contents**:
- `UconDB` class - REST API connection manager
- `DBConfig` struct - Connection configuration
- Authentication support
- Type aliases

#### [provider_ucondb.cpp.md](./provider_ucondb.cpp.md)
Implementation of query operations via REST API.

**Operations**:
- `findConfigurations()` - Query configurations
- `findVersions()` - Find versions
- `findEntities()` - Discover entities
- `listCollections()` / `listDatabases()` - Database discovery
- `databaseMetadata()` - Retrieve metadata

---

### REST API Client

#### [ucondb_api.h.md](./ucondb_api.h.md) / [ucondb_api.cpp.md](./ucondb_api.cpp.md)
REST API client implementation.

**Key Functions**:
- `folders()` - List configuration folders
- `tags()` - List version tags
- `objects()` - List configuration objects
- `get_object()` - Retrieve specific object
- `create_folder()` - Create new folder
- `put_object()` - Upload/update object

**HTTP Client**: libcurl-based implementation with SSL support

---

### Read/Write Operations

#### [provider_ucondb_readwrite.cpp.md](./provider_ucondb_readwrite.cpp.md)
Document I/O via REST API.

**Functions**:
- `readDocument()` - HTTP GET for queries
- `writeDocument()` - HTTP POST/PUT for updates

---

### Supporting Files

#### [provider_ucondb_headers.h.md](./provider_ucondb_headers.h.md)
Aggregator header for UConDB implementation.

#### [provider_connection.cpp.md](./provider_connection.cpp.md)
Connection management and REST client initialization.

---

## Key Features

### Advantages

1. **Centralized**: Single source of truth for all experiments
2. **Multi-Experiment**: Shared infrastructure
3. **Web Interface**: GUI access for browsing/editing
4. **Version Control**: Full history tracking
5. **No Local Installation**: Service-based, no database server to maintain
6. **Access Control**: Fine-grained permissions
7. **Auditing**: Complete audit trail

### UConDB-Specific Features

- **Folders**: Hierarchical organization
- **Tags**: Version labeling (e.g., "production", "test")
- **Validity Time**: Time-based version selection
- **Templates**: Configuration templates
- **Inheritance**: Configuration inheritance

---

## Configuration

### Connection URI

```
ucondb://http[s]://hostname:port/database
```

**Examples**:
```
ucondb://http://ucondb-dev.fnal.gov:8080/artdaq_db
ucondb://https://ucondb.fnal.gov/production_db
```

### Environment Variables

```bash
export ARTDAQ_DATABASE_URI="ucondb://https://ucondb.fnal.gov:8080/artdaq_prod"
```

### Authentication

Set credentials via environment:
```bash
export UCONDB_USER="username"
export UCONDB_PASS="password"
```

Or via URI:
```
ucondb://https://username:password@ucondb.fnal.gov:8080/database
```

---

## Usage Example

```cpp
#include "artdaq-database/StorageProviders/UconDB/provider_ucondb.h"

using namespace artdaq::database::ucon;

// Configure
DBConfig config("ucondb://https://ucondb.fnal.gov:8080/artdaq_db");

// Create provider
auto db = UconDB::create(config);
auto provider = UconDBProvider<JSONDocument>::create(db);

// Read configuration
JSONDocument query;
query.setFolder("Detectors/TPC");
query.setTag("production");
auto results = provider->readDocument(query);

// Write configuration
JSONDocument doc;
doc.setFolder("Detectors/TPC");
doc.setObject("TPC_config");
doc.setData("{\"voltage\": 500, \"gain\": 1.5}");
object_id_t id = provider->writeDocument(doc);
```

---

## UConDB API Structure

### Common Endpoints

- `/folders` - List/create folders
- `/folders/{folder}/objects` - List objects in folder
- `/folders/{folder}/objects/{object}` - Get specific object
- `/folders/{folder}/tags` - List tags for folder
- `/metadata` - Database metadata

### Request Format

**GET Request**:
```
GET /folders/Detectors/TPC/objects/TPC_config?tag=production
```

**POST Request**:
```
POST /folders/Detectors/TPC/objects
Content-Type: application/json

{
  "object": "TPC_config",
  "data": {...},
  "tags": ["production", "v2.0"],
  "validity_time": 1699920000
}
```

---

## Performance Characteristics

### Network Latency

- **On-Site**: ~1-10 ms
- **Remote**: ~50-200 ms depending on location
- **Caching**: Recommended for frequently accessed configurations

### Recommended Use Cases

- **Production Environments**: Centralized configuration management
- **Multi-Site Experiments**: Configuration sharing across institutions
- **Version Tracking**: Need complete configuration history
- **Collaborative**: Multiple users/teams editing configurations
- **Audit Requirements**: Need detailed audit trail

### Not Recommended For

- **High-Frequency Writes**: > 100 writes/sec
- **Offline Operation**: Requires network connectivity
- **Very Large Documents**: > 10 MB (use file storage instead)

---

## UConDB Web Interface

Access the web interface at:
```
https://ucondb.fnal.gov
```

**Features**:
- Browse configurations
- View history
- Edit configurations
- Manage tags
- Search
- Export/import

---

## Debugging

Enable TRACE debugging:

```cpp
artdaq::database::ucon::debug::enable();      // All debugging
artdaq::database::ucon::debug::ReadWrite();   // I/O only
artdaq::database::ucon::debug::UconDBAPI();   // API calls only
```

---

## Error Handling

### HTTP Status Codes

- **200 OK**: Success
- **201 Created**: New object created
- **400 Bad Request**: Invalid request
- **401 Unauthorized**: Authentication required
- **404 Not Found**: Object/folder not found
- **500 Server Error**: UConDB service error

### Network Errors

- Connection timeouts
- DNS resolution failures
- SSL certificate validation errors

---

## Thread Safety

**HTTP Client**: Thread-safe with connection pooling
**Concurrent Operations**: Supported
**Connection Management**: Automatic

---

## File List

All documented UConDB provider files:

1. **provider_ucondb.h** - Provider declarations
2. **provider_ucondb.cpp** - Query operations
3. **provider_ucondb_headers.h** - Header aggregator
4. **provider_ucondb_readwrite.cpp** - I/O operations
5. **provider_connection.cpp** - Connection management
6. **ucondb_api.h** - REST API client headers
7. **ucondb_api.cpp** - REST API implementation

---

## Related Documentation

- **FileSystemDB Provider**: [../FileSystemDB/README.md](../FileSystemDB/README.md)
- **MongoDB Provider**: [../MongoDB/README.md](../MongoDB/README.md)
- **Storage Provider Interface**: [../storage_providers.h.md](../storage_providers.h.md)
- **UConDB Documentation**: https://cdcvs.fnal.gov/redmine/projects/ucondb

---

**Documentation generated for artdaq-database UConDB provider**
**Target audience**: Junior to intermediate C++ developers
**Last updated**: November 13, 2025
