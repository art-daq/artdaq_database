# mongo_json.h

## File Overview

This header provides JSON conversion utilities for MongoDB BSON types. It bridges the gap between MongoDB's native BSON format and JSON strings, with special handling for escaped and unescaped output.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/MongoDB/mongo_json.h`

**Purpose**: BSON to JSON conversion utilities

## Namespace: artdaq::database::mongo::compat

## Functions

### to_json (document)
```cpp
inline std::string to_json(bsoncxx::document::view view);
```

**Purpose**: Convert BSON document to JSON string (uses bsoncxx library).

**Returns**: JSON string with escaped special characters.

---

### from_json
```cpp
inline bsoncxx::document::value from_json(std::string json);
```

**Purpose**: Convert JSON string to BSON document.

**Parameters**: JSON string

**Returns**: BSON document value

---

### to_json (BSON value)
```cpp
std::string to_json(const bsoncxx::types::bson_value::view& value);
```

**Purpose**: Convert BSON value to JSON string.

**Handles**: ObjectId, Date, Binary, and other BSON types.

---

### to_json (array)
```cpp
std::string to_json(const bsoncxx::types::b_array& array);
```

**Purpose**: Convert BSON array to JSON array string.

---

### to_json_unescaped (value)
```cpp
std::string to_json_unescaped(const bsoncxx::types::bson_value::view& value);
```

**Purpose**: Convert BSON value to JSON without escaping special characters.

**Use Case**: Human-readable output, logging.

---

### to_json_unescaped (document)
```cpp
std::string to_json_unescaped(bsoncxx::document::view view);
```

**Purpose**: Convert BSON document to unescaped JSON.

## Usage Example

```cpp
#include "artdaq-database/StorageProviders/MongoDB/mongo_json.h"

// BSON to JSON
bsoncxx::document::view bson_doc = ...;
std::string json = compat::to_json(bson_doc);

// JSON to BSON
std::string json_str = "{\"key\": \"value\"}";
auto bson = compat::from_json(json_str);

// Unescaped for logging
std::string readable = compat::to_json_unescaped(bson_doc);
std::cout << readable << std::endl;
```

## Special BSON Types

Handles MongoDB-specific types:
- **ObjectId**: `{"$oid": "507f1f77bcf86cd799439011"}`
- **Date**: `{"$date": "2025-11-13T10:30:00Z"}`
- **Binary**: `{"$binary": {"base64": "...", "subType": "00"}}`

---

**Documentation generated for artdaq-database MongoDB provider**
