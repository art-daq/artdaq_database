# mongo_json.cpp

**Path:** `artdaq-database/StorageProviders/MongoDB/mongo_json.cpp`

**Implements:** [mongo_json.h](./mongo_json.h.md)

**Purpose:** Implements utility functions for converting between MongoDB BSON types and JSON strings, including specialized handling for extracting values from documents and unescaping forward slashes in JSON output.

## Implementation Overview

This file implements the non-inline BSON/JSON conversion functions declared in `mongo_json.h`. The key technique used is wrapping individual BSON values in temporary documents to leverage the standard `bsoncxx::to_json()` function, then extracting just the value portion from the result.

## Key Algorithms

### Value Extraction via Dummy Document

The BSON library's `to_json()` function only works on complete documents, not individual values. To convert a single value, we:

1. Create a document with a "dummy" key
2. Assign the target value to this key
3. Convert the whole document to JSON
4. Extract the substring after the first `:` and before the final `}`

**Steps:**
1. Build document: `{"dummy": <value>}`
2. Convert to JSON: `{"dummy": <json_value>}`
3. Find position after first `:` using `buff.find(':') + 1`
4. Find position of last `}` using `buff.rfind('}')`
5. Extract substring and trim whitespace using `artdaq::database::trim()`

**Why this approach:** The bsoncxx library does not provide direct value-to-JSON conversion. This workaround leverages the existing document conversion infrastructure.

## Functions

### `compat::to_json(const bsoncxx::types::bson_value::view& value) -> std::string`

**Brief:** Converts any BSON value type to its JSON string representation by wrapping it in a temporary document.

**Parameters:**
- `value` - View of a BSON value (any BSON type: string, int, ObjectId, etc.)

**Returns:** JSON string representing just the value (without surrounding braces or key)

**Implementation:**
```cpp
std::string compat::to_json(const bsoncxx::types::bson_value::view& value) {
  using bsoncxx::builder::basic::kvp;
  auto doc = bsoncxx::builder::basic::document{};
  doc.append(kvp("dummy", value));
  std::string buff = bsoncxx::to_json(doc.view());
  auto fpos = buff.find(':') + 1;
  auto nchars = buff.rfind('}') - fpos;
  return artdaq::database::trim(buff.substr(fpos, nchars));
}
```

**Example Results:**
| Input BSON Type | Output JSON |
|----------------|-------------|
| String `"hello"` | `"hello"` |
| Int32 `42` | `42` |
| ObjectId | `{"$oid": "..."}` |
| Date | `{"$date": ...}` |
| Boolean `true` | `true` |
| Array `[1,2,3]` | `[1, 2, 3]` |

---

### `compat::to_json(const bsoncxx::types::b_array& array) -> std::string`

**Brief:** Converts a BSON array to its JSON string representation using the dummy-document technique.

**Parameters:**
- `array` - BSON array value

**Returns:** JSON array string

**Implementation:**
```cpp
std::string compat::to_json(const bsoncxx::types::b_array& array) {
  using bsoncxx::builder::basic::kvp;
  auto doc = bsoncxx::builder::basic::document{};
  doc.append(kvp("dummy", array));
  std::string buff = bsoncxx::to_json(doc.view());
  auto fpos = buff.find(':') + 1;
  auto nchars = buff.rfind('}') - fpos;
  return artdaq::database::trim(buff.substr(fpos, nchars));
}
```

---

### `compat::to_json_unescaped(const bsoncxx::types::bson_value::view& value) -> std::string`

**Brief:** Converts a BSON value to JSON with forward slashes unescaped by replacing `\/` with `/`.

**Parameters:**
- `value` - View of a BSON value

**Returns:** JSON string with `\/` replaced by `/`

**Implementation:**
```cpp
std::string compat::to_json_unescaped(const bsoncxx::types::bson_value::view& value) {
  std::stringstream ss;
  ss << compat::to_json(value);
  auto retValue = ss.str();
  return artdaq::database::replace_all(retValue, "\\/", "/");
}
```

**Use Case:**
- File paths: `/data/config.json` instead of `\/data\/config.json`
- URLs: `http://example.com/path` instead of `http:\/\/example.com\/path`

---

### `compat::to_json_unescaped(bsoncxx::document::view view) -> std::string`

**Brief:** Converts a BSON document to JSON with forward slashes unescaped.

**Parameters:**
- `view` - Read-only view of a BSON document

**Returns:** Complete JSON document string with unescaped slashes

**Implementation:**
```cpp
std::string compat::to_json_unescaped(bsoncxx::document::view view) {
  std::stringstream ss;
  ss << compat::to_json(view);
  auto retValue = ss.str();
  return artdaq::database::replace_all(retValue, "\\/", "/");
}
```

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/StorageProviders/MongoDB/mongo_json.h` | Header declarations |
| `artdaq-database/SharedCommon/helper_functions.h` | `trim()` and `replace_all()` utilities |
| `<bsoncxx/builder/basic/document.hpp>` | BSON document builder |
| `<bsoncxx/builder/basic/kvp.hpp>` | Key-value pair helper |

## Performance Considerations

- **String operations:** Each conversion creates temporary strings and performs substring extraction. For high-frequency conversions, consider caching results.

- **Slash replacement:** The `replace_all()` function performs a full string scan. For documents with many slashes, this adds linear overhead.

- **Memory allocation:** Multiple string copies are made during conversion. For very large documents, consider streaming alternatives.

## Error Handling Strategy

These functions assume valid BSON input and do not perform extensive error checking:
- Invalid BSON values may produce undefined behavior
- The caller is responsible for ensuring input validity
- String extraction assumes valid JSON output from `bsoncxx::to_json()`

## Testing Notes

- **Unit tests:** Test with various BSON types (strings, numbers, ObjectIds, arrays, nested documents)
- **Key test cases:**
  - Simple value types (string, int, bool)
  - Complex types (ObjectId, Date, Binary)
  - Arrays and nested documents
  - Values containing forward slashes
  - Empty documents and arrays

## Maintenance Notes

- The dummy-document technique is a workaround for bsoncxx API limitations. If future versions of bsoncxx add direct value serialization, consider updating this implementation.

- The `trim()` function from helper_functions is used to clean up whitespace from the extracted value substring.

- The `stringstream` usage in `to_json_unescaped()` is unnecessary but harmless; it could be simplified to direct string assignment.

## See Also

- [mongo_json.h.md](./mongo_json.h.md) - Header declarations and inline functions
- [helper_functions.h.md](../../SharedCommon/helper_functions.h.md) - String utilities used by this implementation
- [provider_mongodb_readwrite.cpp.md](./provider_mongodb_readwrite.cpp.md) - Primary consumer of these functions
