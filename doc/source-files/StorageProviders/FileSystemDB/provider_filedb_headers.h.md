# provider_filedb_headers.h

**Path:** `artdaq-database/StorageProviders/FileSystemDB/provider_filedb_headers.h`

**Purpose:** Aggregator header file that includes all necessary headers for FileSystemDB provider implementation files. This reduces boilerplate includes across implementation files and ensures consistent dependencies and namespace aliases throughout the FileSystemDB module.

## Key Concepts

### Header Aggregation
This file collects all commonly-needed includes in one place, so implementation files only need a single include statement to access all required types and utilities. This pattern improves code maintainability and reduces the chance of missing dependencies.

### Namespace Aliasing
Provides consistent short namespace aliases used throughout the FileSystemDB implementation files, ensuring uniform naming conventions across the module.

## Thread Safety

- **Thread-safe:** Yes (header-only, no mutable state)
- **Concurrent access:** No restrictions
- **Locking:** None required

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h` | Main provider class declarations |
| `artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h` | SearchIndex class for fast queries |
| `artdaq-database/StorageProviders/common.h` | Common utilities and macros |
| `artdaq-database/DataFormats/shared_literals.h` | String literal constants |
| `artdaq-database/SharedCommon/sharedcommon_common.h` | Shared utilities |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | JSON document construction |
| `artdaq-database/SharedCommon/configuraion_api_literals.h` | API literal constants |
| `<sstream>` | String stream for JSON construction |

## Namespace Aliases

| Alias | Full Namespace | Purpose |
|-------|----------------|---------|
| `db` | `artdaq::database` | Root database namespace |
| `apiliteral` | `artdaq::database::configapi::literal` | API string literals |
| `dbfs` | `artdaq::database::filesystem` | FileSystemDB namespace |
| `dbfsl` | `artdaq::database::filesystem::literal` | FileSystemDB literals |
| `jsonliteral` | `artdaq::database::dataformats::literal` | JSON format literals |

## Type Aliases (using declarations)

| Alias | Full Type | Purpose |
|-------|-----------|---------|
| `DBConfig` | `artdaq::database::filesystem::DBConfig` | Configuration structure |
| `FileSystemDB` | `artdaq::database::filesystem::FileSystemDB` | Provider class |
| `SearchIndex` | `artdaq::database::filesystem::index::SearchIndex` | Index class |
| `JSONDocument` | `artdaq::database::docrecord::JSONDocument` | Document type |
| `JSONDocumentBuilder` | `artdaq::database::docrecord::JSONDocumentBuilder` | Builder class |

## Relationship to Other Components

### Implementation Files Using This Header
- `provider_filedb.cpp` - Query operations
- `provider_filedb_readwrite.cpp` - Read/write operations
- `provider_filedb_index.cpp` - Index operations
- `provider_connection.cpp` - Connection management
- `filesystem_functions.cpp` - Filesystem utilities

### Included Components
```
provider_filedb_headers.h
  |
  +-- provider_filedb.h
  |     +-- storage_providers.h
  |     +-- boost/filesystem.hpp
  |
  +-- provider_filedb_index.h
  |     +-- json_common.h
  |     +-- storage_providers.h
  |
  +-- common.h
  |     +-- SharedCommon/common.h
  |
  +-- shared_literals.h
  +-- sharedcommon_common.h
  +-- JSONDocumentBuilder.h
  +-- configuraion_api_literals.h
  +-- <sstream>
```

## See Also

- [provider_filedb.h](./provider_filedb.h.md) - Main provider class
- [provider_filedb_index.h](./provider_filedb_index.h.md) - SearchIndex class
- [../common.h](../common.h.md) - StorageProviders common header

## Notes for Developers

### Usage
Include this header at the top of FileSystemDB implementation files:
```cpp
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb_headers.h"
```

### Benefits
- **Reduces code duplication:** Single include instead of many
- **Ensures consistency:** All files use same namespace aliases
- **Simplifies maintenance:** Add new common includes in one place
- **Improves readability:** Implementation files are more concise

### When to Modify
Add includes to this header when:
- A new dependency is needed by multiple FileSystemDB implementation files
- A new namespace alias would benefit multiple files
- A new common type alias is needed

### When NOT to Modify
Do not add includes that are:
- Only needed by a single implementation file
- Specific to a particular operation or feature
- Heavy dependencies that increase compile time significantly

### Include Guard
The header guard `_ARTDAQ_DATABASE_PROVIDER_FILESYSTEM_HEADERS_H_` prevents multiple inclusion.
