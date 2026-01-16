# UconDB Provider

**Path:** `artdaq-database/StorageProviders/UconDB/`

**Purpose:** Implements a storage provider for Fermilab's Unified Configuration Database (UConDB) service via REST API. This provider enables centralized configuration management across multiple experiments with version control, access control, audit trail, and web interface support.

## Overview

The UconDB provider communicates with Fermilab's UConDB service to provide:
- Centralized configuration storage shared across experiments
- Version control with tags and validity times
- Fine-grained access control and audit trail
- Web interface for browsing and editing
- No local database server required

## Files in This Module

| File | Purpose |
|------|---------|
| `provider_ucondb.h` | Main provider class, DBConfig, type aliases |
| `provider_ucondb.cpp` | Query operation implementations |
| `provider_ucondb_headers.h` | Aggregated includes for implementation files |
| `provider_ucondb_readwrite.cpp` | Document read/write operations |
| `provider_connection.cpp` | Connection management and initialization |
| `ucondb_api.h` | REST API client declarations |
| `ucondb_api.cpp` | REST API client implementation using libcurl |

## Key Classes

### UconDB
Main provider class that manages the connection to the UconDB REST service.

### DBConfig
Configuration structure holding the UconDB connection URI with support for authentication.

### UconDB API Client
REST client implementation using libcurl for HTTP/HTTPS operations.

## Usage Example

```cpp
#include "artdaq-database/StorageProviders/UconDB/provider_ucondb.h"

using namespace artdaq::database::ucon;

// Configure UConDB
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

## Configuration

### URI Format

```
ucondb://http[s]://hostname[:port]/database
```

**Examples:**
```
ucondb://http://ucondb-dev.fnal.gov:8080/artdaq_db
ucondb://https://ucondb.fnal.gov/production_db
ucondb://https://username:password@ucondb.fnal.gov:8080/database
```

### Environment Variables

| Variable | Purpose |
|----------|---------|
| `ARTDAQ_DATABASE_URI` | Set UconDB connection URI |
| `UCONDB_USER` | Authentication username |
| `UCONDB_PASS` | Authentication password |

## UconDB Concepts

### Folders
Hierarchical organization similar to a filesystem:
```
/Detectors/TPC/Config1
/Detectors/ECAL/Config2
/RunControl/Settings
```

### Tags
Version labels for configurations:
- `production` - Current production version
- `test` - Testing version
- `v2.0` - Specific version tag

### Validity Time
Time-based version selection for historical queries.

## REST API Endpoints

| Endpoint | Method | Purpose |
|----------|--------|---------|
| `/folders` | GET | List folders |
| `/folders/{folder}/objects` | GET/POST | List/create objects |
| `/folders/{folder}/objects/{object}` | GET | Get specific object |
| `/folders/{folder}/tags` | GET | List tags |
| `/metadata` | GET | Database metadata |

## Advantages

1. **Centralized**: Single source of truth for all experiments
2. **Multi-Experiment**: Shared infrastructure
3. **Web Interface**: GUI access for browsing/editing
4. **Version Control**: Full history tracking
5. **No Local Installation**: Service-based
6. **Access Control**: Fine-grained permissions
7. **Auditing**: Complete audit trail

## Performance Characteristics

| Scenario | Latency |
|----------|---------|
| On-site (Fermilab) | ~1-10 ms |
| Remote | ~50-200 ms |
| Large response | Depends on size |

### Recommended Use Cases

- Production environments with centralized management
- Multi-site experiments sharing configurations
- Need complete configuration history
- Multiple users/teams editing configurations
- Audit requirements

### Not Recommended For

- High-frequency writes (> 100/sec)
- Offline operation (requires network)
- Very large documents (> 10 MB)

## Thread Safety

**Thread-safe**: HTTP operations use connection pooling via libcurl.

## Debugging

Enable TRACE debugging:
```cpp
artdaq::database::ucon::debug::enable();      // All debugging
artdaq::database::ucon::debug::ReadWrite();   // I/O only
artdaq::database::ucon::debug::UconDBAPI();   // API calls only
```

## Error Handling

### HTTP Status Codes

| Code | Meaning |
|------|---------|
| 200 OK | Success |
| 201 Created | New object created |
| 400 Bad Request | Invalid request |
| 401 Unauthorized | Authentication required |
| 404 Not Found | Object/folder not found |
| 500 Server Error | UconDB service error |

## Web Interface

Access the UconDB web interface at:
```
https://ucondb.fnal.gov
```

Features: Browse, view history, edit, manage tags, search, export/import.
