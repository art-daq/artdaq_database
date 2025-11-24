# mongo_json.cpp

## File Overview

Implementation of BSON to JSON conversion functions with special handling for MongoDB types.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/MongoDB/mongo_json.cpp`

**Purpose**: BSON ↔ JSON conversion implementation

## Key Functions Implemented

### to_json (bson_value)

Converts MongoDB BSON values to JSON strings with proper type handling:

**Supported Types**:
- **ObjectId**: Converts to `{"$oid": "..."}`
- **Date**: Converts to ISO 8601 timestamp
- **Binary**: Base64 encoded with subtype
- **Arrays**: Recursive conversion
- **Embedded Documents**: Recursive conversion
- **Primitives**: String, Number, Boolean, Null

### to_json (b_array)

Converts BSON arrays to JSON arrays:
```cpp
std::string to_json(const bsoncxx::types::b_array& array);
```

**Implementation**: Iterates array elements, converts each to JSON.

### to_json_unescaped

Produces human-readable JSON without escaping:

**Use Cases**:
- Logging and debugging
- Display to users
- Configuration file generation

**Difference from to_json**:
- `to_json()`: Escaped for programmatic use
- `to_json_unescaped()`: Readable for humans

---

**Documentation generated for artdaq-database MongoDB provider**
