# provider_ucondb_headers.h

**Path:** `artdaq-database/StorageProviders/UconDB/provider_ucondb_headers.h`

**Purpose:** Aggregator header that centralizes all includes needed by UconDB provider implementation files. This header provides a single include point for implementation files, reducing duplication and ensuring consistent include ordering.


## Key Concepts

### Header Aggregation Pattern

This file follows the "aggregator header" pattern where a single header collects all dependencies needed by multiple implementation files. This simplifies maintenance by keeping the include list in one place and ensures all implementation files have consistent access to required types.

### Namespace Aliases

Defines short namespace aliases to reduce verbosity in implementation code, making the implementation files more readable and reducing typing errors.

### TRACE Configuration

Configures the TRACE logging system with a module-specific name for log filtering and identification.

## Thread Safety

- **Thread-safe:** N/A (header file with no executable code)

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/BasicTypes/basictypes.h` | Core type definitions |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | Document construction utilities |
| `artdaq-database/SharedCommon/common.h` | Common utilities and exceptions |
| `artdaq-database/SharedCommon/returned_result.h` | Result type for operation outcomes |
| `artdaq-database/StorageProviders/UconDB/provider_ucondb.h` | UconDB provider class |
| `artdaq-database/StorageProviders/UconDB/ucondb_api.h` | REST API client interface |

## Namespace Aliases

```cpp
namespace db = artdaq::database;
namespace dbuc = db::ucon;
namespace dbucl = dbuc::literal;
namespace jsonliteral = db::dataformats::literal;
```

| Alias | Full Namespace | Purpose |
|-------|----------------|---------|
| `db` | `artdaq::database` | Root namespace shortcut |
| `dbuc` | `artdaq::database::ucon` | UconDB namespace shortcut |
| `dbucl` | `artdaq::database::ucon::literal` | UconDB literals shortcut |
| `jsonliteral` | `artdaq::database::dataformats::literal` | JSON literal constants |

## Using Declarations

```cpp
using artdaq::database::docrecord::JSONDocument;
using artdaq::database::ucon::DBConfig;
using artdaq::database::ucon::UconDB;
using artdaq::database::ucon::UconDBSPtr_t;
using artdaq::database::docrecord::JSONDocumentBuilder;
```

| Type | Source Namespace | Purpose |
|------|------------------|---------|
| `JSONDocument` | `artdaq::database::docrecord` | Core document type for JSON data |
| `DBConfig` | `artdaq::database::ucon` | Configuration structure for UconDB |
| `UconDB` | `artdaq::database::ucon` | Provider class for UconDB operations |
| `UconDBSPtr_t` | `artdaq::database::ucon` | Shared pointer to UconDB instance |
| `JSONDocumentBuilder` | `artdaq::database::docrecord` | Builder pattern for document construction |

**Note:** `JSONDocument` appears twice in the using declarations (duplicate), but this is harmless.

## TRACE Configuration

```cpp
#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "PRVDR:UconDB_C"
```

Sets the TRACE module name to `"PRVDR:UconDB_C"` for log filtering and identification. The `#undef` ensures any previously defined TRACE_NAME is replaced.

## Relationship to Other Components

### Implementation Files

This header is included by:
- `provider_ucondb_readwrite.cpp` - Read/write operations

### Include Guard

Uses standard include guards (`_ARTDAQ_DATABASE_PROVIDER_UCOND_HEADERS_H_`) to prevent multiple inclusion.

## Usage Example

```cpp
// In an implementation file:
#include "artdaq-database/StorageProviders/UconDB/provider_ucondb_headers.h"

// All UconDB types and utilities are now available
void example() {
  // Use namespace aliases for brevity
  dbuc::DBConfig config;

  // Use imported types directly without namespace qualification
  JSONDocument doc;
  JSONDocumentBuilder builder(doc);

  // Use literal constants
  auto collection = jsonliteral::collection;
}
```

## Notes for Developers

### When to Use

Include this header in UconDB implementation files (`.cpp`) rather than including individual headers separately. This ensures:
- Consistent include ordering
- All required types are available
- Namespace aliases are defined
- TRACE configuration is applied

### When NOT to Use

- In header files (would create tight coupling and increase compile times)
- In non-UconDB code (use specific headers instead)
- In public API headers (would expose implementation details)

### Best Practices

1. Include this as the first artdaq-database header in implementation files
2. Override TRACE_NAME after including if a different module name is needed
3. Do not add new dependencies to this file without considering impact on all consumers

## See Also

- [provider_ucondb.h.md](./provider_ucondb.h.md) - Main provider header
- [ucondb_api.h.md](./ucondb_api.h.md) - REST API client
- [provider_ucondb_readwrite.cpp.md](./provider_ucondb_readwrite.cpp.md) - Uses this header
- [provider_mongodb_headers.h.md](../MongoDB/provider_mongodb_headers.h.md) - Similar pattern for MongoDB
