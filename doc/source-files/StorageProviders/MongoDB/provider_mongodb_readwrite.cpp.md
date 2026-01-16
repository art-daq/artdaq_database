# provider_mongodb_readwrite.cpp

**Path:** `artdaq-database/StorageProviders/MongoDB/provider_mongodb_readwrite.cpp`

**Implements:** [provider_mongodb.h](./provider_mongodb.h.md)

**Purpose:** Implements the core document read and write operations for the MongoDB storage provider. These are the fundamental CRUD operations that enable storing and retrieving configuration documents in MongoDB.

## Implementation Overview

This file provides template specializations of `StorageProvider<JSONDocument, MongoDB>` for document I/O:
- `readDocument()` - Query and retrieve documents from MongoDB
- `writeDocument()` - Insert new documents or update existing ones

The implementation handles collection selection, BSON conversion, duplicate detection, and object ID management.

## Key Algorithms

### Document Write Algorithm (`writeDocument`)

The write operation implements an insert-or-update pattern with semantic duplicate detection.

**Steps:**
1. Extract user document and optional filter from the input
2. Determine target collection (from document, filter, or explicit parameter)
3. Check if this is a new document or an update based on filter presence
4. For new documents:
   a. Generate a new object ID
   b. Check for semantic duplicates (same version and entity)
   c. If duplicate found, throw an error
   d. Insert the document
5. For updates:
   a. Verify filter matches exactly one document
   b. Insert new version of document
   c. Delete the old version matching the filter
6. Return the object ID of the inserted/updated document

**Why this approach:** The insert-then-delete pattern for updates ensures atomic updates in MongoDB. Semantic duplicate checking prevents multiple documents with the same version/entity combination, which would violate data integrity.

### Document Read Algorithm (`readDocument`)

The read operation performs a filtered query on a MongoDB collection.

**Steps:**
1. Extract filter document and collection name from input
2. Convert filter to BSON format
3. Execute find query on the collection
4. Convert each matching document from BSON to JSON
5. Return vector of matching JSON documents

## Template Specializations

### `StorageProvider<JSONDocument, MongoDB>::readDocument`

**Brief:** Retrieves documents from MongoDB matching the specified filter criteria.

**Parameters:**
- `arg` - JSON document containing filter criteria and collection name

**Returns:** Vector of JSON documents matching the filter

**Preconditions:**
- Collection name must be specified (in filter document or arg)
- Filter document must be valid JSON

**Postconditions:**
- All matching documents are returned as unescaped JSON

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `confirm` assertion failure | When collection name is empty |
| `mongocxx::exception` | When MongoDB query fails |

**Input Format:**
```json
{
  "collection": "collection_name",
  "filter": {
    "version": "1.0.0",
    "entities.name": "my_entity"
  }
}
```

**Example:**
```cpp
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"

using namespace artdaq::database;
using namespace artdaq::database::mongo;

void readConfigurations() {
  DBConfig config("mongodb://localhost:27017/artdaq_db");
  auto db = MongoDB::create(config);

  // Create read query
  JSONDocument query(R"({
    "collection": "Configurations",
    "filter": {
      "version": "1.0.0"
    }
  })");

  try {
    auto provider = StorageProvider<JSONDocument, MongoDB>::create(db);
    auto results = provider->readDocument(query);

    for (const auto& doc : results) {
      std::cout << "Found: " << doc << std::endl;
    }
  } catch (const mongocxx::exception& e) {
    std::cerr << "MongoDB error: " << e.what() << std::endl;
  }
}
```

### `StorageProvider<JSONDocument, MongoDB>::writeDocument`

**Brief:** Writes a document to MongoDB, either as a new insert or as an update to an existing document.

**Parameters:**
- `arg` - JSON document containing the user document, optional filter, and collection name

**Returns:** Object ID of the written document

**Preconditions:**
- Collection name must be specified
- User document must be valid JSON
- For updates, filter must match exactly one document

**Postconditions:**
- Document is stored in MongoDB
- Object ID is assigned and returned
- For updates, previous version is deleted

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `confirm` assertion failure | When collection name is empty |
| `runtime_error("MongoDB")` | When semantic duplicate exists (same version + entity) |
| `runtime_error("MongoDB")` | When filter matches more than one document |
| `mongocxx::exception` | When MongoDB operation fails |

**Input Format (New Document):**
```json
{
  "document": {
    "collection": "Configurations",
    "version": "1.0.0",
    "entities": [{"name": "my_entity"}],
    "data": {...}
  }
}
```

**Input Format (Update Existing):**
```json
{
  "document": {
    "collection": "Configurations",
    "version": "1.0.1",
    "data": {...}
  },
  "filter": {
    "_id": {"$oid": "507f1f77bcf86cd799439011"}
  }
}
```

**Example:**
```cpp
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"

using namespace artdaq::database;
using namespace artdaq::database::mongo;

void storeConfiguration() {
  DBConfig config("mongodb://localhost:27017/artdaq_db");
  auto db = MongoDB::create(config);

  // Create document to store
  JSONDocument writeDoc(R"({
    "document": {
      "collection": "Configurations",
      "version": "1.0.0",
      "entities": [{"name": "detector_front_end"}],
      "data": {
        "threshold": 100,
        "enabled": true
      }
    }
  })");

  try {
    auto provider = StorageProvider<JSONDocument, MongoDB>::create(db);
    auto objectId = provider->writeDocument(writeDoc);

    std::cout << "Stored with ID: " << objectId << std::endl;
  } catch (const runtime_error& e) {
    std::cerr << "Write failed: " << e.what() << std::endl;
  }
}
```

## Internal Logic

### Collection Name Resolution

The collection name is resolved in priority order:
1. From the filter document's `collection` field
2. From the user document's `collection` field
3. From the top-level `collection` field in the input argument

### Object ID Management

- **New documents:** A new OID is generated using `generate_oid()`
- **Updates:** The existing OID from the filter is extracted and reused
- **Invalid OID:** Represented by `ouid_invalid` constant

### Semantic Duplicate Detection

Before inserting a new document, the system checks for existing documents with the same:
- `version` field value
- `entities.name` field value

If a duplicate is found, the operation fails with an error message identifying the conflict.

## Debug Functions

### `artdaq::database::mongo::debug::ReadWrite()`

**Brief:** Enables TRACE debugging for read and write operations.

**Implementation:**
- Sets TRACE control parameters for this file
- Enables verbose logging of all read/write operations
- Logs confirmation message at TRACE level 10

## Performance Considerations

- **Duplicate checking:** The semantic duplicate check adds an extra query before each insert. For bulk operations, consider using `insertMany` with appropriate indexes.

- **Update pattern:** The insert-then-delete pattern involves two operations. Consider using MongoDB's native `replaceOne` for single-document updates if atomicity is not critical.

- **Index requirements:** Ensure indexes exist on `version` and `entities.name` fields for efficient duplicate detection.

## Error Handling Strategy

1. **Precondition validation:** Uses `confirm()` assertions for required fields
2. **Try-catch wrapping:** Catches exceptions from document field extraction
3. **Semantic validation:** Throws `runtime_error` for business logic violations (duplicates, wide filters)
4. **Driver errors:** Allows `mongocxx::exception` to propagate

## Testing Notes

- **Unit tests:** See `test/StorageProviders/MongoDB/` for read/write tests
- **Key test cases:**
  - Insert new document
  - Update existing document by filter
  - Duplicate detection rejection
  - Wide filter rejection (matches multiple documents)
  - Collection name resolution from different locations

## Maintenance Notes

- The `#pragma GCC diagnostic ignored "-Wdeprecated-declarations"` suppresses warnings from deprecated mongocxx APIs. Monitor driver updates for migration guidance.

- The try-catch blocks around field extraction are intentional to handle optional fields gracefully without crashing.

## See Also

- [provider_mongodb.h.md](./provider_mongodb.h.md) - Header declarations
- [provider_mongodb.cpp.md](./provider_mongodb.cpp.md) - Query operations
- [provider_mongodb_headers.h.md](./provider_mongodb_headers.h.md) - Common includes
- [JSONDocumentBuilder.h.md](../../JsonDocument/JSONDocumentBuilder.h.md) - Document construction

---

**Last Updated:** 2026-01-14
