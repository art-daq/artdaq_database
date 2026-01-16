# mongo_json.h

**Path:** `artdaq-database/StorageProviders/MongoDB/mongo_json.h`

**Purpose:** Declares utility functions for converting between MongoDB BSON types and JSON strings. These functions bridge the gap between the mongocxx driver's BSON representation and the artdaq-database JSON-centric document model.


## Key Concepts

### BSON to JSON Conversion

MongoDB's native format is BSON (Binary JSON), which includes additional types not present in standard JSON (ObjectId, Date, Binary, etc.). The functions in this header convert BSON values to their JSON string representations for use within artdaq-database's JSON-based document model.

### Escaped vs Unescaped JSON

MongoDB's `bsoncxx::to_json()` escapes forward slashes as `\/` for JavaScript compatibility. Some applications need unescaped slashes (e.g., for file paths or URLs). The `to_json_unescaped()` variants handle this conversion by replacing `\/` with `/`.

### Compatibility Namespace

All functions are declared in `artdaq::database::mongo::compat` to indicate they provide compatibility wrappers around the bsoncxx library functions.

## Thread Safety

- **Thread-safe:** Yes (all functions are stateless)
- **Concurrent access:** Safe for concurrent calls
- **Locking:** None required

## Dependencies

| Include | Purpose |
|---------|---------|
| `<bsoncxx/document/value.hpp>` | BSON document value type |
| `<bsoncxx/json.hpp>` | Standard BSON/JSON conversion functions |
| `<bsoncxx/types/value.hpp>` | BSON type value wrappers |

## Namespace

All functions are declared in `artdaq::database::mongo::compat`. A namespace alias `compat` is provided at global scope for convenience:

```cpp
namespace compat = artdaq::database::mongo::compat;
```

## Functions

### `to_json(bsoncxx::document::view view) -> std::string`

**Brief:** Converts a BSON document view to its JSON string representation using the standard bsoncxx conversion.

**Parameters:**
- `view` - Read-only view of a BSON document

**Returns:** JSON string representing the document

**Thread Safety:** Safe

**Implementation:** Inline wrapper around `bsoncxx::to_json()`.

**Example:**
```cpp
#include "artdaq-database/StorageProviders/MongoDB/mongo_json.h"
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>

using bsoncxx::builder::basic::kvp;

void convertDocument() {
  auto doc = bsoncxx::builder::basic::document{};
  doc.append(kvp("name", "test"));
  doc.append(kvp("value", 42));

  std::string json = compat::to_json(doc.view());
  // json == R"({"name": "test", "value": 42})"
}
```

---

### `from_json(std::string json) -> bsoncxx::document::value`

**Brief:** Parses a JSON string into an owning BSON document value.

**Parameters:**
- `json` - JSON string to parse

**Returns:** Owning BSON document value

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `bsoncxx::exception` | When JSON is malformed or contains invalid syntax |

**Thread Safety:** Safe

**Implementation:** Inline wrapper around `bsoncxx::from_json()`.

**Example:**
```cpp
#include "artdaq-database/StorageProviders/MongoDB/mongo_json.h"
#include <iostream>

void parseJson() {
  try {
    auto doc = compat::from_json(R"({"run": 12345, "active": true})");
    auto view = doc.view();
    // Use document view...
  } catch (const bsoncxx::exception& e) {
    std::cerr << "Invalid JSON: " << e.what() << std::endl;
  }
}
```

---

### `to_json(const bsoncxx::types::bson_value::view& value) -> std::string`

**Brief:** Converts a BSON value (any type) to its JSON string representation.

**Parameters:**
- `value` - View of a BSON value (can be any BSON type: string, int, ObjectId, Date, etc.)

**Returns:** JSON string representing just the value (without surrounding braces or key)

**Thread Safety:** Safe

**Implementation Details:**
Wraps the value in a temporary document with a "dummy" key, then extracts just the value portion. This technique is necessary because `bsoncxx::to_json()` only works on complete documents, not individual values.

**Example:**
```cpp
#include "artdaq-database/StorageProviders/MongoDB/mongo_json.h"

void convertValue() {
  // Converting an ObjectId to JSON
  auto oid = bsoncxx::oid{};
  auto value = bsoncxx::types::bson_value::view{bsoncxx::types::b_oid{oid}};
  std::string json = compat::to_json(value);
  // json == R"({"$oid": "507f1f77bcf86cd799439011"})"
}
```

---

### `to_json(const bsoncxx::types::b_array& array) -> std::string`

**Brief:** Converts a BSON array to its JSON string representation.

**Parameters:**
- `array` - BSON array value

**Returns:** JSON string representing the array

**Thread Safety:** Safe

**Implementation Details:**
Uses the same dummy-document technique as the value conversion function.

**Example:**
```cpp
#include "artdaq-database/StorageProviders/MongoDB/mongo_json.h"
#include <bsoncxx/builder/basic/array.hpp>

void convertArray() {
  auto arr = bsoncxx::builder::basic::array{};
  arr.append(1, 2, 3);
  auto bson_array = bsoncxx::types::b_array{arr.view()};
  std::string json = compat::to_json(bson_array);
  // json == "[1, 2, 3]"
}
```

---

### `to_json_unescaped(const bsoncxx::types::bson_value::view& value) -> std::string`

**Brief:** Converts a BSON value to JSON with forward slashes unescaped (replaces `\/` with `/`).

**Parameters:**
- `value` - View of a BSON value

**Returns:** JSON string with `\/` converted to `/`

**Thread Safety:** Safe

**Use Case:** When JSON contains file paths or URLs that should not have escaped slashes.

**Example:**
```cpp
#include "artdaq-database/StorageProviders/MongoDB/mongo_json.h"

void convertPath() {
  // Standard to_json would produce: {"path": "\\/data\\/file.txt"}
  // to_json_unescaped produces: {"path": "/data/file.txt"}
}
```

---

### `to_json_unescaped(bsoncxx::document::view view) -> std::string`

**Brief:** Converts a BSON document to JSON with forward slashes unescaped (replaces `\/` with `/`).

**Parameters:**
- `view` - Read-only view of a BSON document

**Returns:** JSON string with `\/` converted to `/`

**Thread Safety:** Safe

**Use Case:** When storing or displaying documents that contain file system paths or URLs.

## Relationship to Other Components

### Used By

- `provider_mongodb.cpp` - For query result conversion
- `provider_mongodb_readwrite.cpp` - For document I/O
- `mongo_functions.cpp` - For error message formatting

### Provides Bridge Between

- **mongocxx driver** (BSON types) and
- **artdaq-database document model** (JSON strings)

## Common Pitfalls

- **Escaped slashes:** Remember that standard `to_json()` escapes forward slashes. Use `to_json_unescaped()` if you need literal slashes in the output.

- **Exception handling:** `from_json()` can throw on malformed input. Always wrap in try-catch when parsing untrusted JSON.

### Anti-patterns

```cpp
// DON'T do this - no error handling
auto doc = compat::from_json(userInput);  // May throw!

// DO this instead
try {
  auto doc = compat::from_json(userInput);
  // Process document...
} catch (const bsoncxx::exception& e) {
  // Handle parse error
  std::cerr << "Parse error: " << e.what() << std::endl;
}
```

## See Also

- [mongo_json.cpp.md](./mongo_json.cpp.md) - Implementation details
- [provider_mongodb_headers.h.md](./provider_mongodb_headers.h.md) - Includes this header
- [External: bsoncxx Documentation](https://mongocxx.org/api/current/namespacebsoncxx.html) - BSON library reference
