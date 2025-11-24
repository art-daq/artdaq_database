# provider_mongodb_readwrite.cpp

## File Overview

Implements core read and write operations for MongoDB provider using mongocxx driver.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/MongoDB/provider_mongodb_readwrite.cpp`

**Purpose**: MongoDB document I/O operations

## Template Specializations

### readDocument
```cpp
std::vector<JSONDocument> StorageProvider<JSONDocument, MongoDB>::readDocument(JSONDocument const& arg);
```

**Algorithm**:
1. Extract collection name and filter from arg
2. Convert JSON filter to BSON query
3. Execute MongoDB find() query
4. Convert BSON results to JSON documents
5. Return vector of JSON documents

**Features**:
- Full MongoDB query language support
- Efficient indexed queries
- Projection support
- Sort and limit support

### writeDocument
```cpp
object_id_t StorageProvider<JSONDocument, MongoDB>::writeDocument(JSONDocument const& arg);
```

**Algorithm**:
1. Extract document and collection
2. Check for existing _id
3. If _id exists: update existing document
4. If no _id: insert new document with generated ObjectId
5. Return ObjectId as string

**Operations**:
- **Insert**: New documents get MongoDB ObjectId
- **Update**: Existing documents updated by _id
- **Upsert**: Optional upsert mode

## BSON Conversion

Uses `mongo_json.h` utilities for bidirectional conversion:
- JSON → BSON: `compat::from_json()`
- BSON → JSON: `compat::to_json()`

## Error Handling

- MongoDB exceptions caught and re-thrown as `runtime_error`
- Connection failures handled gracefully
- Invalid queries logged and reported

---

**Documentation generated for artdaq-database MongoDB provider**
