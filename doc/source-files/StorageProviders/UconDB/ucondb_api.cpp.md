# ucondb_api.cpp

## File Overview

Implementation of UConDB REST API client functions using HTTP library.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/UconDB/ucondb_api.cpp`

**Purpose**: UConDB REST API implementation

## Implemented Functions

### HTTP GET Operations
- Query configurations
- Retrieve versions
- List entities
- Get metadata

### HTTP POST Operations
- Create new configurations
- Upload new documents
- Register new versions

### HTTP PUT Operations
- Update existing configurations
- Modify metadata

## HTTP Client

**Library**: libcurl or similar HTTP client

**Features**:
- SSL/TLS support
- JSON encoding/decoding
- Error handling and retries
- Connection timeouts

## Error Handling

- HTTP status codes checked
- Network errors caught
- JSON parsing errors handled
- Detailed error messages

---

**Documentation generated for artdaq-database UConDB provider**
