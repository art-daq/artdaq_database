# provider_ucondb_headers.h

## File Overview

Aggregator header for UConDB provider implementation files.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/UconDB/provider_ucondb_headers.h`

**Purpose**: Centralized header aggregation for UConDB implementation

## Included Headers

- `provider_ucondb.h` - UConDB provider class
- `ucondb_api.h` - REST API client
- `common.h` - Common utilities
- `JSONDocumentBuilder.h` - Document construction
- HTTP client libraries

## Namespace Aliases

```cpp
namespace db = artdaq::database;
namespace dbucon = db::ucon;
namespace literal = dbucon::literal;
```

## Usage

```cpp
#include "artdaq-database/StorageProviders/UconDB/provider_ucondb_headers.h"

// All UConDB types and utilities available
```

---

**Documentation generated for artdaq-database UConDB provider**
