# provider_mongodb_headers.h

## File Overview

Aggregator header that includes all necessary headers for MongoDB provider implementation files. Provides mongocxx/bsoncxx includes, namespace aliases, and using directives.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/MongoDB/provider_mongodb_headers.h`

**Purpose**: Centralized header aggregation for MongoDB implementation

## Included Libraries

### MongoDB C++ Driver (mongocxx/bsoncxx)
- `<mongocxx/client.hpp>` - MongoDB client
- `<mongocxx/instance.hpp>` - Driver instance
- `<mongocxx/options/find.hpp>` - Query options
- `<mongocxx/pipeline.hpp>` - Aggregation pipeline
- `<bsoncxx/builder/stream/document.hpp>` - BSON document builder
- `<bsoncxx/json.hpp>` - JSON conversion
- `<bsoncxx/types.hpp>` - BSON types

### Project Headers
- `provider_mongodb.h` - MongoDB provider class
- `mongo_json.h` - JSON conversion utilities
- `JSONDocumentBuilder.h` - Document construction
- `basictypes.h` - Basic type definitions

## Namespace Aliases

```cpp
using namespace artdaq::database;
namespace db = artdaq::database;
namespace literal = db::mongo::literal;
namespace bbs = bsoncxx::builder::stream;
namespace jsonliteral = artdaq::database::dataformats::literal;
```

## Using Directives

```cpp
using bsoncxx::builder::concatenate_doc;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
```

**Purpose**: Stream-style BSON document construction.

## Usage

Include this header in MongoDB .cpp files:

```cpp
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb_headers.h"

// Now have access to all MongoDB types and utilities
```

---

**Documentation generated for artdaq-database MongoDB provider**
