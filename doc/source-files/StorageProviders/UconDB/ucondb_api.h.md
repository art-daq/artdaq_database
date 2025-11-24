# ucondb_api.h

## File Overview

This header defines the REST API interface for communicating with the UConDB service. It provides functions for HTTP requests to UConDB endpoints.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/UconDB/ucondb_api.h`

**Purpose**: UConDB REST API client interface

## Namespace: artdaq::database::ucondb::api

## Key Functions

### GET Requests
Functions for retrieving data from UConDB:
- Configuration queries
- Version lookups
- Entity searches
- Metadata retrieval

### POST Requests
Functions for creating new configurations:
- Upload new documents
- Create new versions
- Register entities

### PUT Requests
Functions for updating existing data:
- Modify configurations
- Update metadata

## API Endpoints

Typical UConDB API structure:
- `/configurations` - Configuration management
- `/entities` - Entity management
- `/versions` - Version management
- `/metadata` - Database metadata

## HTTP Client

**Implementation**: Uses HTTP client library (libcurl or similar) for REST API communication.

**Features**:
- JSON request/response handling
- Authentication support
- Error handling
- Connection pooling

## Usage Example

```cpp
#include "artdaq-database/StorageProviders/UconDB/ucondb_api.h"

// Query UConDB
std::string response = ucondb::api::get_configuration(
    "http://ucondb.fnal.gov:8080",
    "artdaq_db",
    "MyConfiguration"
);

// Parse response JSON
JSONDocument result(response);
```

---

**Documentation generated for artdaq-database UConDB provider**
