# provider_filedb_headers.h

## File Overview

Aggregator header file that includes all necessary headers for FileSystemDB provider implementation files. Reduces boilerplate includes and ensures consistent dependencies.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/FileSystemDB/provider_filedb_headers.h`

**Lines of Code**: 27

**Purpose**: Centralized header aggregation for FileSystemDB implementation

## Included Headers

### FileSystemDB Headers
- `provider_filedb.h` - Main provider class
- `provider_filedb_index.h` - Search index
- `common.h` - Common utilities

### Shared Headers
- `shared_literals.h` - String literal constants
- `sharedcommon_common.h` - Shared utilities
- `JSONDocumentBuilder.h` - JSON document construction

## Namespace Aliases

```cpp
namespace db = artdaq::database;
namespace dbfs = artdaq::database::filesystem;
namespace dbfsl = dbfs::literal;
namespace jsonliteral = artdaq::database::dataformats::literal;
```

**Purpose**: Shorter namespace paths for implementation files.

## Type Aliases

```cpp
using artdaq::database::filesystem::DBConfig;
using artdaq::database::filesystem::FileSystemDB;
using artdaq::database::filesystem::index::SearchIndex;
using artdaq::database::docrecord::JSONDocument;
using artdaq::database::docrecord::JSONDocumentBuilder;
```

**Purpose**: Convenient type names without full qualification.

## Usage

Include this header in FileSystemDB .cpp files:

```cpp
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb_headers.h"

// Now have access to all necessary types and utilities
```

---

**Documentation generated for artdaq-database FileSystemDB provider**
