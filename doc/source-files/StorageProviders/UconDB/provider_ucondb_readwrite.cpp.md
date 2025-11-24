# provider_ucondb_readwrite.cpp

## File Overview

Implements core read and write operations for UConDB provider using REST API.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/UconDB/provider_ucondb_readwrite.cpp`

**Purpose**: UConDB document I/O operations

## Template Specializations

### readDocument
```cpp
std::vector<JSONDocument> StorageProvider<JSONDocument, UconDB>::readDocument(JSONDocument const& arg);
```

**Implementation**:
1. Parse query filter
2. Build UConDB API request
3. Execute HTTP GET
4. Parse JSON response
5. Return documents

### writeDocument
```cpp
object_id_t StorageProvider<JSONDocument, UconDB>::writeDocument(JSONDocument const& arg);
```

**Implementation**:
1. Prepare document JSON
2. Determine if new or update
3. Execute HTTP POST (new) or PUT (update)
4. Parse response for document ID
5. Return ID

## REST API Communication

**Method**: HTTP/HTTPS requests
**Format**: JSON request/response bodies
**Authentication**: Optional token-based auth
**Error Handling**: HTTP status codes

---

**Documentation generated for artdaq-database UConDB provider**
