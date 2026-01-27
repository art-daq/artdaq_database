# provider_mongodb_headers.h

**Path:** `artdaq-database/StorageProviders/MongoDB/provider_mongodb_headers.h`

**Purpose:** Aggregates all common includes and namespace declarations needed by MongoDB provider implementation files. This header centralizes dependencies to ensure consistency and reduce code duplication across the MongoDB provider implementation files.


## Key Concepts

### Header Aggregation Pattern

Rather than repeating the same set of includes in each `.cpp` file, this header consolidates all common dependencies. Implementation files simply include this one header to get all necessary declarations.

### Namespace Aliases

The header establishes namespace aliases used throughout the MongoDB implementation, ensuring consistent naming across all source files.

## Thread Safety

- **Thread-safe:** N/A (header-only, no executable code)
- **Concurrent access:** N/A
- **Locking:** N/A

## Dependencies

### Internal Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/BasicTypes/basictypes.h` | Basic data type definitions (`JsonData`, `XmlData`, etc.) |
| `artdaq-database/StorageProviders/MongoDB/mongo_json.h` | BSON/JSON conversion utilities |
| `artdaq-database/StorageProviders/MongoDB/provider_mongodb.h` | Main MongoDB provider declarations |
| `artdaq-database/StorageProviders/common.h` | Common storage provider utilities |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | Builder pattern for constructing JSON documents |
| `artdaq-database/SharedCommon/configuraion_api_literals.h` | API literal string constants |
| `artdaq-database/SharedCommon/sharedcommon_common.h` | Common shared utilities |

### Standard Library Dependencies

| Include | Purpose |
|---------|---------|
| `<sstream>` | String stream operations for building JSON strings |

### MongoDB Driver Dependencies

| Include | Purpose |
|---------|---------|
| `<bsoncxx/builder/basic/helpers.hpp>` | BSON builder helper functions |
| `<bsoncxx/builder/stream/array.hpp>` | Stream-style BSON array construction |
| `<bsoncxx/builder/stream/document.hpp>` | Stream-style BSON document construction |
| `<bsoncxx/document/value.hpp>` | Owning BSON document values |
| `<bsoncxx/document/view.hpp>` | Non-owning BSON document views |
| `<bsoncxx/json.hpp>` | JSON to/from BSON conversion |
| `<bsoncxx/stdx/optional.hpp>` | Optional type for BSON values |
| `<bsoncxx/stdx/string_view.hpp>` | String view type from BSON library |
| `<bsoncxx/types.hpp>` | BSON type definitions |
| `<bsoncxx/types/value.hpp>` | BSON type value wrappers |
| `<mongocxx/client.hpp>` | MongoDB client for database connections |
| `<mongocxx/instance.hpp>` | MongoDB driver instance singleton |
| `<mongocxx/options/find.hpp>` | Find operation options |
| `<mongocxx/pipeline.hpp>` | Aggregation pipeline construction |

## Namespace Aliases

| Alias | Target Namespace | Purpose |
|-------|------------------|---------|
| `db` | `artdaq::database` | Short alias for main database namespace |
| `literal` | `db::mongo::literal` | MongoDB literal constants |
| `bbs` | `bsoncxx::builder::stream` | Stream-style BSON builder |
| `jsonliteral` | `artdaq::database::dataformats::literal` | JSON field name literals |
| `apiliteral` | `artdaq::database::configapi::literal` | API operation literals |

## Using Declarations

| Declaration | Source | Purpose |
|-------------|--------|---------|
| `concatenate_doc` | `bsoncxx::builder` | Concatenate BSON documents |
| `close_array` | `bsoncxx::builder::stream` | Close BSON array in stream builder |
| `close_document` | `bsoncxx::builder::stream` | Close BSON document in stream builder |
| `document` | `bsoncxx::builder::stream` | Start BSON document in stream builder |
| `finalize` | `bsoncxx::builder::stream` | Finalize BSON document construction |
| `open_array` | `bsoncxx::builder::stream` | Open BSON array in stream builder |
| `open_document` | `bsoncxx::builder::stream` | Open nested BSON document |
| `JSONDocument` | `artdaq::database::docrecord` | JSON document class |
| `JSONDocumentBuilder` | `artdaq::database::docrecord` | JSON document builder class |
| `DBConfig` | `artdaq::database::mongo` | MongoDB configuration structure |
| `MongoDB` | `artdaq::database::mongo` | MongoDB provider class |

## Relationship to Other Components

### Used By

- `provider_mongodb.cpp` - Query operations implementation
- `provider_mongodb_readwrite.cpp` - Read/write operations implementation

### Provides Access To

- All MongoDB provider types and utilities
- BSON/JSON conversion functions
- Document builder infrastructure
- Literal constants for field names and operations

## Common Pitfalls

- **Include order:** This header should be included first in implementation files, as it sets up namespace aliases used by subsequent code.

- **Namespace pollution:** The `using namespace artdaq::database;` declaration brings the entire database namespace into scope. Be aware of potential naming conflicts.

## Notes for Developers

This is a convenience header for implementation files only. It should not be included in other header files to avoid exposing implementation details and creating circular dependencies.

### Usage Example

```cpp
// In an implementation file
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb_headers.h"

// Now all aliases are available
namespace artdaq {
namespace database {

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, MongoDB>::customOperation(JSONDocument const& arg) {
  // Use bbs::document for BSON building
  bbs::document query;
  query << jsonliteral::name << "value";

  // Use the provider
  auto collection = _provider->connection().collection("MyCollection");

  // Use compat namespace for JSON conversion
  auto json = compat::to_json(query.view());

  return {};
}

}  // namespace database
}  // namespace artdaq
```

## See Also

- [provider_mongodb.cpp.md](./provider_mongodb.cpp.md) - Uses this header for query implementations
- [provider_mongodb_readwrite.cpp.md](./provider_mongodb_readwrite.cpp.md) - Uses this header for I/O implementations
- [mongo_json.h.md](./mongo_json.h.md) - BSON/JSON conversion utilities included by this header

---

**Last Updated:** 2026-01-14
