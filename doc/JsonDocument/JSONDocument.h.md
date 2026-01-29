# JSONDocument.h

**Path:** `artdaq-database/JsonDocument/JSONDocument.h`

**Purpose:** This header file defines the `JSONDocument` class, which is the core abstraction for working with JSON documents in the artdaq-database system. It provides a high-level, type-safe interface for creating, manipulating, querying, and traversing JSON document structures using a path-based API with dot-notation navigation.


## Key Concepts

### JSON Document Abstraction
A `JSONDocument` wraps a JSON value (internally represented as `value_t`, a Boost variant type) and provides methods to navigate and modify the document tree. The class uses dot-notation paths like `"document.data.field"` to access nested elements, similar to how JavaScript accesses object properties.

### Value Types
The underlying JSON data is stored as `value_t`, which is a Boost variant that can hold:
- `object_t` - A JSON object (key-value pairs)
- `array_t` - A JSON array (ordered list of values)
- Primitive types (strings, numbers, booleans, null)

### Path-Based Navigation
All document operations use dot-notation paths:
- `"server.port"` accesses the `port` field inside `server`
- `"document.data.settings.enabled"` navigates deep into nested objects
- The path is split on `.` characters for tree traversal

### Lazy Serialization with Caching
The class uses a caching strategy with `_cached_json_buffer` and `_isDirty` flag to avoid redundant JSON serialization. The JSON string is only generated when needed and cached for subsequent reads.

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not safe for concurrent read/write operations. Even const methods like `to_string()` modify internal mutable state for caching.
- **Locking:** No internal locks; external synchronization required for multi-threaded use

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/JsonDocument/common.h` | Provides JSON data types (`value_t`, `object_t`, `array_t`, `type_t`), exception types (`notfound_exception`, `invalid_argument`), string literals, and shared utilities |

## Classes/Structures

### `JSONDocument`

A final class that wraps JSON data with a path-based manipulation API. Provides comprehensive functionality for creating, navigating, modifying, and serializing JSON documents. Declared as a friend of `JSONDocumentBuilder` and `JSONDocumentMigrator` to allow direct access to internal `_value` member.

**Thread Safety:** Not thread-safe. Mutable members (`_cached_json_buffer`, `_isDirty`) are modified without synchronization.

#### Constructors

##### `JSONDocument(JsonData const& data)`

**Brief:** Constructs a JSONDocument from a JsonData structure by parsing its JSON content.

**Parameters:**
- `data` - A JsonData structure containing JSON content as a string

**Preconditions:**
- The JsonData must contain valid, parseable JSON

**Postconditions:**
- Document contains parsed JSON structure
- `_isDirty` is set to true

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When JSON parsing fails or content is empty |

**Thread Safety:** safe (construction)

**Example:**
```cpp
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/BasicTypes/data_json.h"

using namespace artdaq::database::docrecord;
using namespace artdaq::database::basictypes;

void example() {
  JsonData data{R"({"name": "test", "value": 42})"};
  try {
    JSONDocument doc(data);
    std::cout << "Loaded document: " << doc.to_string() << std::endl;
  } catch (const invalid_argument& e) {
    std::cerr << "Failed to parse JSON: " << e.what() << std::endl;
  }
}
```

##### `JSONDocument(std::string const& json)`

**Brief:** Constructs a JSONDocument by parsing a JSON string.

**Parameters:**
- `json` - A string containing valid JSON

**Preconditions:**
- The string must contain valid, parseable JSON

**Postconditions:**
- Document contains parsed JSON structure
- `_isDirty` is set to true

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When JSON parsing fails or string is empty |

**Thread Safety:** safe (construction)

**Example:**
```cpp
#include "artdaq-database/JsonDocument/JSONDocument.h"

using namespace artdaq::database::docrecord;

void example() {
  try {
    JSONDocument doc(R"({"server": {"port": 8080, "host": "localhost"}})");
    std::cout << "Document created successfully" << std::endl;
  } catch (const invalid_argument& e) {
    std::cerr << "Invalid JSON: " << e.what() << std::endl;
  }
}
```

##### `JSONDocument(value_t value)`

**Brief:** Constructs a JSONDocument from an existing JSON value_t (AST node).

**Parameters:**
- `value` - A value_t variant representing a JSON value

**Preconditions:**
- None

**Postconditions:**
- Document wraps the provided value (moved)
- `_isDirty` is set to true

**Throws:** None

**Thread Safety:** safe (construction)

##### `JSONDocument()`

**Brief:** Constructs an empty JSONDocument containing an empty JSON object.

**Parameters:** None

**Preconditions:** None

**Postconditions:**
- Document contains an empty JSON object `{}`
- `_isDirty` is set to true

**Throws:** None

**Thread Safety:** safe (construction)

#### Child Manipulation Methods

##### `insertChild(JSONDocument const& newChild, path_t const& path) -> JSONDocument`

**Brief:** Inserts a new child element at the specified path. Fails if the path already exists, preventing accidental overwrites.

**Parameters:**
- `newChild` - Document containing the value to insert (the value is extracted using the last path component as key)
- `path` - Dot-notation path where the child should be inserted (e.g., `"document.data.newfield"`)

**Preconditions:**
- Path must not be empty
- Parent path elements must exist
- Target path must not already exist

**Returns:** JSONDocument containing the inserted value

**Postconditions:**
- Document is modified to include the new child
- `_isDirty` is set to true

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When path is empty |
| `notfound_exception` | When parent path element doesn't exist |
| `notfound_exception` | When target path already exists (use replaceChild instead) |

**Thread Safety:** unsafe

**Example:**
```cpp
#include "artdaq-database/JsonDocument/JSONDocument.h"

using namespace artdaq::database::docrecord;

void example() {
  JSONDocument doc(R"({"server": {"port": 8080}})");
  JSONDocument newField(R"({"host": "localhost"})");

  try {
    auto inserted = doc.insertChild(newField, "server.host");
    std::cout << "Inserted: " << inserted.to_string() << std::endl;
    std::cout << "Document: " << doc.to_string() << std::endl;
  } catch (const notfound_exception& e) {
    std::cerr << "Insert failed: " << e.what() << std::endl;
  }
}
```

##### `replaceChild(JSONDocument const& newChild, path_t const& path) -> JSONDocument`

**Brief:** Replaces an existing child element at the specified path and returns the old value.

**Parameters:**
- `newChild` - Document containing the new value
- `path` - Dot-notation path to the element to replace

**Preconditions:**
- Path must not be empty
- Path must exist in the document

**Returns:** JSONDocument containing the old (replaced) value, wrapped with the path as key

**Postconditions:**
- Document is modified with the new value at the specified path
- `_isDirty` is set to true

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When path is empty |
| `notfound_exception` | When path doesn't exist |

**Thread Safety:** unsafe

**Example:**
```cpp
#include "artdaq-database/JsonDocument/JSONDocument.h"

using namespace artdaq::database::docrecord;

void example() {
  JSONDocument doc(R"({"server": {"port": 8080}})");
  JSONDocument newPort(R"({"port": 9090})");

  try {
    auto oldValue = doc.replaceChild(newPort, "server.port");
    std::cout << "Old value: " << oldValue.to_string() << std::endl;
    std::cout << "New document: " << doc.to_string() << std::endl;
  } catch (const notfound_exception& e) {
    std::cerr << "Replace failed: " << e.what() << std::endl;
  }
}
```

##### `deleteChild(path_t const& path) -> JSONDocument`

**Brief:** Deletes the child element at the specified path and returns the deleted value.

**Parameters:**
- `path` - Dot-notation path to the element to delete

**Preconditions:**
- Path must not be empty
- Path must exist in the document

**Returns:** JSONDocument containing the deleted value

**Postconditions:**
- Element is removed from the document
- `_isDirty` is set to true

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When path is empty |
| `notfound_exception` | When path doesn't exist |

**Thread Safety:** unsafe

**Example:**
```cpp
#include "artdaq-database/JsonDocument/JSONDocument.h"

using namespace artdaq::database::docrecord;

void example() {
  JSONDocument doc(R"({"server": {"port": 8080, "host": "localhost"}})");

  try {
    auto deleted = doc.deleteChild("server.host");
    std::cout << "Deleted: " << deleted.to_string() << std::endl;
    std::cout << "Remaining: " << doc.to_string() << std::endl;
  } catch (const notfound_exception& e) {
    std::cerr << "Delete failed: " << e.what() << std::endl;
  }
}
```

##### `findChild(path_t const& path) const -> JSONDocument`

**Brief:** Finds and returns a child element at the specified path, wrapped in a document with the full path as key.

**Parameters:**
- `path` - Dot-notation path to the element to find

**Preconditions:**
- Path must not be empty
- Path must exist in the document

**Returns:** JSONDocument in format `{"path.to.element": <found_value>}`

**Postconditions:**
- Original document is unchanged

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When path is empty |
| `notfound_exception` | When path doesn't exist |

**Thread Safety:** safe (const method)

**Example:**
```cpp
#include "artdaq-database/JsonDocument/JSONDocument.h"

using namespace artdaq::database::docrecord;

void example() {
  JSONDocument doc(R"({"server": {"port": 8080}})");

  try {
    auto child = doc.findChild("server.port");
    // Returns: {"server.port": 8080}
    std::cout << "Found: " << child.to_string() << std::endl;
  } catch (const notfound_exception& e) {
    std::cerr << "Not found: " << e.what() << std::endl;
  }
}
```

##### `findChildDocument(path_t const& path) const -> JSONDocument`

**Brief:** Finds and returns a child element at the specified path as a standalone document. The child must be an object type.

**Parameters:**
- `path` - Dot-notation path to the element to find

**Preconditions:**
- Path must not be empty
- Path must exist in the document
- Value at path must be an object type

**Returns:** JSONDocument containing the found value directly (not wrapped with path key)

**Postconditions:**
- Original document is unchanged

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When path is empty |
| `notfound_exception` | When path doesn't exist |
| `notfound_exception` | When value at path is not an object type |

**Thread Safety:** safe (const method)

**Example:**
```cpp
#include "artdaq-database/JsonDocument/JSONDocument.h"

using namespace artdaq::database::docrecord;

void example() {
  JSONDocument doc(R"({"server": {"port": 8080, "host": "localhost"}})");

  try {
    auto serverDoc = doc.findChildDocument("server");
    // Returns: {"port": 8080, "host": "localhost"}
    std::cout << "Server config: " << serverDoc.to_string() << std::endl;
  } catch (const notfound_exception& e) {
    std::cerr << "Not found: " << e.what() << std::endl;
  }
}
```

#### Array Manipulation Methods

##### `appendChild(JSONDocument const& newChild, path_t const& path) -> JSONDocument`

**Brief:** Appends a child element to an array at the specified path.

**Parameters:**
- `newChild` - Document containing the value to append (payload is extracted)
- `path` - Dot-notation path to the array

**Preconditions:**
- Path must not be empty
- Path must exist and point to an array

**Returns:** JSONDocument containing the appended value

**Postconditions:**
- New element is added to the end of the array
- `_isDirty` is set to true

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When path is empty |
| `notfound_exception` | When path doesn't exist |
| `std::exception` | When value at path is not an array |

**Thread Safety:** unsafe

**Example:**
```cpp
#include "artdaq-database/JsonDocument/JSONDocument.h"

using namespace artdaq::database::docrecord;

void example() {
  JSONDocument doc(R"({"items": [{"name": "item1"}]})");
  JSONDocument newItem(R"({"name": "item2"})");

  try {
    auto added = doc.appendChild(newItem, "items");
    std::cout << "Added: " << added.to_string() << std::endl;
    std::cout << "Array: " << doc.to_string() << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Append failed: " << e.what() << std::endl;
  }
}
```

##### `removeChild(JSONDocument const& delChild, path_t const& path) -> JSONDocument`

**Brief:** Removes all matching child elements from an array at the specified path using partial matching.

**Parameters:**
- `delChild` - Document containing the pattern to match for deletion
- `path` - Dot-notation path to the array

**Preconditions:**
- Path must not be empty
- Path must exist and point to an array

**Returns:** JSONDocument containing an array of all removed elements under key "0"

**Postconditions:**
- All matching elements are removed from the array
- `_isDirty` is set to true

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When path is empty |
| `notfound_exception` | When path doesn't exist |
| `std::exception` | When value at path is not an array |

**Thread Safety:** unsafe

**Example:**
```cpp
#include "artdaq-database/JsonDocument/JSONDocument.h"

using namespace artdaq::database::docrecord;

void example() {
  JSONDocument doc(R"({"items": [{"name": "old"}, {"name": "new"}, {"name": "old"}]})");
  JSONDocument pattern(R"({"name": "old"})");

  try {
    auto removed = doc.removeChild(pattern, "items");
    // Returns: {"0": [{"name":"old"}, {"name":"old"}]}
    std::cout << "Removed: " << removed.to_string() << std::endl;
    std::cout << "Remaining: " << doc.to_string() << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Remove failed: " << e.what() << std::endl;
  }
}
```

#### Value Extraction Methods

##### `extract() -> value_t`

**Brief:** Extracts and returns the underlying JSON value, leaving the document empty.

**Parameters:** None

**Preconditions:** None

**Returns:** The internal `value_t` that was held by the document

**Postconditions:**
- Document is left with an empty object
- `_isDirty` is set to true

**Throws:** None

**Thread Safety:** unsafe

##### `to_string() const -> std::string`

**Brief:** Converts the document to a JSON string, using cached result if available and document is not dirty.

**Parameters:** None

**Preconditions:** None

**Returns:** JSON string representation of the document

**Postconditions:**
- Result is cached in `_cached_json_buffer`
- `_isDirty` is set to false

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When internal value is not an object type |

**Thread Safety:** conditional (modifies mutable members `_cached_json_buffer` and `_isDirty`)

**Example:**
```cpp
JSONDocument doc(R"({"key": "value"})");
std::string json = doc.to_string();
// Multiple calls return cached result efficiently
std::string json2 = doc.to_string();
```

##### `empty() const -> bool`

**Brief:** Checks if the document's internal value is empty.

**Parameters:** None

**Preconditions:** None

**Returns:** `true` if the document is empty, `false` otherwise

**Postconditions:** None

**Throws:** None

**Thread Safety:** safe (const method, no mutable state modified)

##### `operator std::string() const`

**Brief:** Implicit conversion operator to JSON string, equivalent to calling to_string().

**Parameters:** None

**Returns:** JSON string representation of the document

**Throws:** Same as `to_string()`

**Thread Safety:** conditional (modifies mutable members)

##### `value() -> std::string`

**Brief:** Returns the document's value as a string by extracting the payload.

**Parameters:** None

**Returns:** String representation of the document's payload value

**Throws:** None

**Thread Safety:** unsafe

#### Comparison Methods

##### `equals(JSONDocument const& other) const -> bool`

**Brief:** Compares this document with another for structural equality using deep comparison.

**Parameters:**
- `other` - The document to compare with

**Preconditions:** None

**Returns:** `true` if documents are structurally equal, `false` otherwise

**Postconditions:** None

**Throws:** None

**Thread Safety:** safe (const method)

##### `operator==(JSONDocument const& other) const -> bool`

**Brief:** Equality comparison operator, delegates to equals() method.

**Parameters:**
- `other` - The document to compare with

**Returns:** `true` if documents are equal, `false` otherwise

**Thread Safety:** safe (const method)

##### `operator!=(JSONDocument const& other) const -> bool`

**Brief:** Inequality comparison operator.

**Parameters:**
- `other` - The document to compare with

**Returns:** `true` if documents are not equal, `false` otherwise

**Thread Safety:** safe (const method)

##### `value_as<T>(path_t const& path) const -> T`

**Brief:** Template method that finds a value at the specified path and converts it to the requested type using boost::lexical_cast.

**Parameters:**
- `path` - Dot-notation path to the value

**Preconditions:**
- Path must exist
- Value must be convertible to type T via boost::lexical_cast

**Returns:** Value at path converted to type T

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `notfound_exception` | When path doesn't exist |
| `boost::bad_lexical_cast` | When value cannot be converted to T |

**Thread Safety:** safe (const method)

**Example:**
```cpp
JSONDocument doc(R"({"port": 8080, "name": "server", "enabled": true})");

int port = doc.value_as<int>("port");           // 8080
std::string name = doc.value_as<std::string>("name");  // "server"
```

#### File I/O Methods

##### `saveToFile(std::string const& filename) -> bool`

**Brief:** Saves the document to a file, creating a backup (.bak) of any existing file first.

**Parameters:**
- `filename` - Path to the output file

**Preconditions:**
- Filename must not be empty
- Directory must be writable

**Returns:** `true` on success

**Postconditions:**
- File contains JSON representation of document
- If file existed, backup is created with .bak extension

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When filename is empty |
| `runtime_error` | When file write fails |

**Thread Safety:** unsafe

**Side Effects:**
- Writes to filesystem
- Creates backup file if original exists

**Example:**
```cpp
#include "artdaq-database/JsonDocument/JSONDocument.h"

using namespace artdaq::database::docrecord;

void example() {
  JSONDocument doc(R"({"config": "data"})");

  try {
    if (doc.saveToFile("/path/to/config.json")) {
      std::cout << "Saved successfully" << std::endl;
    }
  } catch (const std::exception& e) {
    std::cerr << "Save failed: " << e.what() << std::endl;
  }
}
```

##### `loadFromFile(std::string const& filename) -> JSONDocument` [static]

**Brief:** Static method that loads and parses a JSON document from a file.

**Parameters:**
- `filename` - Path to the JSON file

**Preconditions:**
- File must exist and be readable
- File must contain valid JSON

**Returns:** JSONDocument containing the parsed content

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When file cannot be opened |
| `runtime_error` | When parsing fails |

**Thread Safety:** safe (static method)

**Example:**
```cpp
#include "artdaq-database/JsonDocument/JSONDocument.h"

using namespace artdaq::database::docrecord;

void example() {
  try {
    auto doc = JSONDocument::loadFromFile("/path/to/config.json");
    std::cout << "Loaded: " << doc.to_string() << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Load failed: " << e.what() << std::endl;
  }
}
```

#### Static Utility Methods

##### `value(JSONDocument const& document) -> std::string` [static]

**Brief:** Static utility method that extracts the payload value from a document as a string.

**Parameters:**
- `document` - The document to extract value from

**Returns:** String representation of the document's payload value

**Thread Safety:** safe (static method)

##### `value_at(JSONDocument const& document, std::size_t index) -> std::string` [static]

**Brief:** Static utility method that extracts a value at a specific index from an array document.

**Parameters:**
- `document` - The document containing an array
- `index` - Zero-based index into the array

**Preconditions:**
- Document must contain an array
- Index must be within bounds

**Returns:** String representation of the value at the specified index

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When array is empty or index is out of bounds |

**Thread Safety:** safe (static method)

## Functions

### `compareDocumentVersions(JSONDocument const& a, JSONDocument const& b) -> bool`

**Brief:** Compares two documents by their version fields for sorting purposes using lexicographic string comparison.

**Parameters:**
- `a` - First document to compare
- `b` - Second document to compare

**Preconditions:**
- Both documents must have a "version" field

**Returns:** `true` if a's version is less than b's version

**Thread Safety:** safe

**Example:**
```cpp
std::vector<JSONDocument> docs = loadDocuments();
std::sort(docs.begin(), docs.end(), compareDocumentVersions);
```

### `split_path(path_t const& path) -> std::vector<path_t>`

**Brief:** Splits a dot-notation path into its component parts.

**Parameters:**
- `path` - Dot-notation path string (e.g., `"document.data.field"`)

**Returns:** Vector of path components (e.g., `["document", "data", "field"]`)

**Thread Safety:** safe

**Example:**
```cpp
auto parts = split_path("document.data.field");
// parts = {"document", "data", "field"}
```

### `operator<<(std::ostream& os, JSONDocument const& doc) -> std::ostream&`

**Brief:** Stream output operator for JSONDocument, outputs JSON string representation.

**Parameters:**
- `os` - Output stream
- `doc` - Document to output

**Returns:** Reference to the output stream

**Thread Safety:** conditional (calls to_string which modifies mutable state)

### `debug::JSONDocument()`

**Brief:** Enables TRACE logging for JSONDocument class operations at maximum verbosity.

**Parameters:** None

**Returns:** None

**Side Effects:**
- Configures TRACE logging for JSONDocument debugging

**Thread Safety:** safe

### `debug::JSONDocumentUtils()`

**Brief:** Enables TRACE logging for JSONDocument utility functions at maximum verbosity.

**Parameters:** None

**Returns:** None

**Side Effects:**
- Configures TRACE logging for utility function debugging

**Thread Safety:** safe

## Copy/Move Semantics

- **Copy construction:** Allowed (default)
- **Copy assignment:** Deleted - use move assignment or explicit copy
- **Move construction:** Allowed (default)
- **Move assignment:** Allowed (default)

**Rationale:** Deleting copy assignment prevents accidental copies of potentially large documents while allowing explicit copy construction when needed.

## Relationship to Other Components

### Within the JsonDocument Module
- **JSONDocumentBuilder** - Uses `JSONDocument` as its internal storage, declared as a friend class to access private `_value` member for efficient document construction
- **JSONDocumentMigrator** - Declared as a friend class to access document internals during format migration
- **JSONDocument_utils.cpp** - Implements utility functions and some `JSONDocument` methods including constructors, file I/O, and value extraction

### Dependencies
- **DataFormats/Json** - Provides the underlying JSON types (`value_t`, `object_t`, `array_t`, `type_t`) and reader/writer implementations
- **BasicTypes** - Provides `JsonData` structure for interoperability
- **SharedCommon** - Provides base exception class and common utilities

### Used By
- **StorageProviders** - MongoDB, FileSystemDB providers use `JSONDocument` for data storage
- **ConfigurationDB** - Higher-level APIs wrap documents for user interaction
- **Overlay Module** - Provides structured access to document fields via overlays

## See Also

- [JSONDocument.cpp.md](./JSONDocument.cpp.md) - Implementation details for tree manipulation
- [JSONDocument_utils.cpp.md](./JSONDocument_utils.cpp.md) - Utility function implementations
- [JSONDocumentBuilder.h.md](./JSONDocumentBuilder.h.md) - Builder pattern for database documents
- [common.h.md](./common.h.md) - Common includes and type definitions
- [docrecord_exceptions.h.md](./docrecord_exceptions.h.md) - Exception types
- [External: Boost.Variant](https://www.boost.org/doc/libs/release/doc/html/variant.html) - Underlying variant type

## Notes for Developers

### Common Pitfalls

- **Using insertChild for existing paths:** `insertChild` will throw if the path exists. Use `replaceChild` for existing paths or check first.
- **Forgetting to handle notfound_exception:** Always wrap path operations in try-catch when the path may not exist.
- **Thread safety assumptions:** The class uses mutable members for caching, so even const methods like `to_string()` modify internal state.
- **Empty paths:** All path-based methods throw on empty paths. Validate paths before calling.
- **Dangling references:** The non-const `findChildValue` returns a reference to internal data; modifications to the document may invalidate it.

### Anti-patterns

```cpp
// DON'T do this - insertChild throws if path exists:
doc.insertChild(newValue, "existing.path");  // Throws notfound_exception!

// DO this instead - check or use replaceChild:
try {
  doc.findChild("some.path");  // Check if exists
  doc.replaceChild(newValue, "some.path");  // Replace existing
} catch (const notfound_exception&) {
  doc.insertChild(newValue, "some.path");  // Insert new
}

// DON'T rely on thread-safety for const methods:
// Thread 1:
std::string s1 = doc.to_string();  // Modifies _cached_json_buffer
// Thread 2:
std::string s2 = doc.to_string();  // Race condition!

// DO use external synchronization:
std::mutex docMutex;
{
  std::lock_guard<std::mutex> lock(docMutex);
  std::string s = doc.to_string();
}
```
