# JSONDocument.h

## File Overview

This header file defines the `JSONDocument` class, which is the core abstraction for working with JSON documents in the artdaq-database system. It provides a high-level, type-safe interface for creating, manipulating, querying, and traversing JSON document structures using a path-based API.

**Location**: `/home/user/artdaq-database/artdaq-database/JsonDocument/JSONDocument.h`

## Dependencies

```cpp
#include "artdaq-database/JsonDocument/common.h"
```

The file depends on the module's common header which provides:
- JSON data types (`value_t`, `object_t`, `array_t`, `type_t`)
- Exception types (`notfound_exception`, `readonly_exception`)
- String literals and constants
- Shared utilities

## Forward Declarations

```cpp
namespace artdaq::database::basictypes {
  struct JsonData;
}
```

The file forward-declares `JsonData` to avoid including the full header, reducing compilation dependencies.

## Namespace Structure

```cpp
namespace artdaq {
namespace database {
namespace docrecord {
  // JSONDocument class and utilities
}
}
}
```

## Type Aliases

```cpp
using artdaq::database::basictypes::JsonData;
using artdaq::database::json::object_t;
using artdaq::database::json::value_t;
```

These aliases make JSON types available without full qualification within the `docrecord` namespace.

## Forward Declarations of Related Classes

```cpp
class JSONDocumentBuilder;
class JSONDocumentMigrator;
```

These classes are forward-declared as friends of `JSONDocument`.

## JSONDocument Class

### Class Declaration

```cpp
class JSONDocument final {
  friend class JSONDocumentBuilder;
  friend class JSONDocumentMigrator;
  // ...
};
```

**Key Characteristics**:
- **final** - Cannot be inherited from (design is complete, prevent polymorphism overhead)
- **Friends** - `JSONDocumentBuilder` and `JSONDocumentMigrator` have special access to private members

### Constructors

```cpp
JSONDocument(JsonData const&);    // From JsonData structure
JSONDocument(std::string const&); // From JSON string
JSONDocument(value_t);            // From JSON value_t (AST)
JSONDocument();                   // Default (empty document)
```

**Constructor Purposes**:
1. **JsonData** - Interop with BasicTypes module
2. **String** - Parse JSON text into document
3. **value_t** - Wrap existing JSON AST structure
4. **Default** - Create empty document for building

### Document Tree Manipulation Methods

#### Child Element Operations

```cpp
// Insert new child (throws if exists)
JSONDocument insertChild(JSONDocument const&, path_t const&);

// Replace existing child (returns old value)
JSONDocument replaceChild(JSONDocument const&, path_t const&);

// Delete child (returns deleted value)
JSONDocument deleteChild(path_t const&);

// Find child (throws if not found)
JSONDocument findChild(path_t const&) const;

// Find child as document (throws if not found or not object)
JSONDocument findChildDocument(path_t const&) const;
```

**Path-Based API**: All operations use dot-notation paths like `"document.data.field"`.

**Return Values**: Most operations return `JSONDocument` objects:
- **insertChild** - Returns the newly inserted child
- **replaceChild** - Returns the old (replaced) child
- **deleteChild** - Returns the deleted child
- **findChild** - Returns the found child wrapped in a document with path as key
- **findChildDocument** - Returns the found child value directly as a document

#### Array Operations

```cpp
// Append child to array (returns added child)
JSONDocument appendChild(JSONDocument const&, path_t const&);

// Remove matching child from array (returns removed children)
JSONDocument removeChild(JSONDocument const&, path_t const&);
```

**Array Specifics**:
- **appendChild** - Adds element to end of array at specified path
- **removeChild** - Removes all matching elements from array (partial match supported)

### Value Extraction and Conversion

```cpp
// Extract underlying value_t (destructive, empties document)
value_t extract();

// Convert to JSON string
std::string to_string() const;

// Check if document is empty
bool empty() const;

// Implicit conversion to string
operator std::string() const;

// Get value as string
std::string value();

// Get value with type conversion
template <typename T>
T value_as(path_t const& path) const;
```

**value_as Template**:
```cpp
template <typename T>
T JSONDocument::value_as(path_t const& path) const {
  return boost::lexical_cast<T>(findChild(path).value());
}
```

Converts the value at the given path to type `T` using `boost::lexical_cast`.

**Example**:
```cpp
int port = doc.value_as<int>("server.port");
std::string name = doc.value_as<std::string>("server.hostname");
```

### Comparison Operations

```cpp
bool equals(JSONDocument const&) const;

bool operator==(JSONDocument const& other) const { return equals(other); }
bool operator!=(JSONDocument const& other) const { return !(*this == other); }
```

Compares two documents for equality based on their JSON content.

### File I/O Utilities

```cpp
// Save document to file (instance method)
bool saveToFile(std::string const&);

// Load document from file (static method)
static JSONDocument loadFromFile(std::string const&);
```

**File Operations**:
- **saveToFile** - Writes JSON to file, creates `.bak` backup if file exists
- **loadFromFile** - Reads JSON from file, returns new document

### Static Utility Methods

```cpp
// Extract string value from document
static std::string value(JSONDocument const&);

// Extract value at array index
static std::string value_at(JSONDocument const&, std::size_t);
```

### Special Member Functions

```cpp
// Defaults
~JSONDocument() = default;                        // Destructor
JSONDocument(JSONDocument const&) = default;      // Copy constructor
JSONDocument& operator=(JSONDocument&&) = default; // Move assignment
JSONDocument(JSONDocument&&) = default;           // Move constructor

// Deleted
JSONDocument& operator=(JSONDocument const&) = delete; // Copy assignment DELETED
```

**Design Choice**: Copy construction is allowed but copy assignment is deleted. This enforces explicit copying while allowing return value optimization and passing by value when needed.

### Private Methods

```cpp
private:
  // JSON parsing and serialization
  value_t readJson(std::string const&);
  std::string writeJson() const;

  // Caching
  std::string const& cached_json_buffer() const;

  // Value access helpers
  value_t const& getPayloadValueForKey(object_t::key_type const& key) const;
  value_t& findChildValue(path_t const&);
  value_t const& findChildValue(path_t const&) const;

  // Self-reference helpers
  JSONDocument& self() { return *this; }
  JSONDocument const& self() const { return *this; }
```

### Private Member Variables

```cpp
private:
  value_t _value;                      // JSON AST (variant type)
  mutable std::string _cached_json_buffer; // Cached JSON string
  mutable bool _isDirty;               // Cache invalidation flag
```

**Data Members Explained**:
- **_value** - The actual JSON data as an abstract syntax tree (variant type)
- **_cached_json_buffer** - Cached string representation to avoid repeated serialization
- **_isDirty** - Tracks if document was modified since last serialization (mutable for lazy caching)

## Free Functions and Utilities

### Version Comparison

```cpp
bool compareDocumentVersions(JSONDocument const&, JSONDocument const&);
```

Compares two documents based on their version fields (lexicographic comparison).

### Debug Functions

```cpp
namespace debug {
  void JSONDocument();
  void JSONDocumentUtils();
}
```

Enable detailed TRACE logging for debugging document operations.

### Stream Output Operator

```cpp
std::ostream& operator<<(std::ostream&, JSONDocument const&);
```

Allows streaming `JSONDocument` to output streams:
```cpp
std::cout << doc << std::endl;
TLOG(10) << "Document: " << doc;
```

### Path Utilities

```cpp
std::vector<path_t> split_path(path_t const&);
```

Splits a dot-notation path into individual components:
- Input: `"document.data.field"`
- Output: `["document", "data", "field"]`

## TRACE Integration

The file includes a specialization for TRACE streaming:

```cpp
namespace {
template <>
inline TraceStreamer& TraceStreamer::operator<<(
    const artdaq::database::docrecord::JSONDocument& r) {
  std::ostringstream s;
  s << r;
  msg_append(s.str().c_str());
  return *this;
}
}
```

This allows `JSONDocument` objects to be used directly in TRACE/TLOG statements.

## Usage Examples

### Creating Documents

```cpp
// From JSON string
JSONDocument doc(R"({"name":"test", "value":42})");

// Empty document
JSONDocument doc;

// From value_t
value_t val = /* ... */;
JSONDocument doc(val);
```

### Navigating Documents

```cpp
// Find child element
auto child = doc.findChild("document.data");

// Find and convert
std::string name = doc.value_as<std::string>("configuration.name");
int version = doc.value_as<int>("version");

// Check existence
try {
  auto field = doc.findChild("optional.field");
  // Field exists, use it
} catch (notfound_exception const&) {
  // Field doesn't exist
}
```

### Modifying Documents

```cpp
// Insert new field
JSONDocument newField(R"({"field":"value"})");
doc.insertChild(newField, "document.data.field");

// Replace existing
JSONDocument newValue(R"({"field":"new_value"})");
auto old = doc.replaceChild(newValue, "document.data.field");

// Delete field
auto deleted = doc.deleteChild("document.data.field");
```

### Working with Arrays

```cpp
// Append to array
JSONDocument item(R"({"name":"item"})");
doc.appendChild(item, "items");

// Remove from array
JSONDocument toRemove(R"({"name":"old_item"})");
auto removed = doc.removeChild(toRemove, "items");
```

### File Operations

```cpp
// Load from file
auto doc = JSONDocument::loadFromFile("/path/to/config.json");

// Modify document
doc.replaceChild(newValue, "some.path");

// Save to file
doc.saveToFile("/path/to/config.json"); // Creates .bak backup
```

## Design Patterns

### Value Semantics with Move

The class supports both copying and moving, but copy assignment is deleted:
- Allows return by value (RVO)
- Supports move semantics for efficiency
- Forces explicit copying via copy constructor

### Path-Based API

All navigation uses dot-notation paths:
- Intuitive and readable
- Matches JSON structure
- Easy to construct programmatically
- Consistent with MongoDB-style queries

### Lazy Serialization with Caching

The `_cached_json_buffer` and `_isDirty` members implement lazy serialization:
- JSON string only generated when needed
- Cache invalidated on modifications
- Reduces redundant serialization
- Improves performance for read-heavy operations

### Friend-Based Privileged Access

`JSONDocumentBuilder` and `JSONDocumentMigrator` are friends:
- Can access private `_value` member
- Enables efficient document construction
- Maintains encapsulation for general users
- Builder pattern for complex document creation

## Error Handling

The class throws exceptions for error conditions:

**notfound_exception**:
- Path doesn't exist
- Element not found
- Invalid navigation

**invalid_argument**:
- Empty path
- Invalid JSON string
- Type mismatches

**readonly_exception**:
- Attempting to modify read-only documents (via Builder)

## Performance Considerations

1. **Caching** - String serialization is cached and only regenerated when dirty
2. **Move Semantics** - Efficient transfer of ownership
3. **Reference Returns** - Many internal methods return references to avoid copies
4. **Extract** - Allows extracting value without copying (destructive but efficient)

## Thread Safety

The class is **not thread-safe**:
- Mutable members (`_cached_json_buffer`, `_isDirty`) used without synchronization
- Intended for single-threaded use or external synchronization
- Modifications are not atomic

## Related Files

- **JSONDocument.cpp** - Implementation of member functions
- **JSONDocument_utils.cpp** - Utility function implementations
- **JSONDocumentBuilder.h** - Higher-level document construction API
- **JSONDocumentMigrator.h** - Document format migration utilities

## Best Practices

1. **Use Builder for Complex Documents** - `JSONDocumentBuilder` is better for constructing documents with metadata
2. **Catch Specific Exceptions** - Handle `notfound_exception` when searching for optional fields
3. **Prefer Move** - Use `std::move` when transferring ownership
4. **Check Empty** - Use `empty()` before operations on potentially empty documents
5. **Use value_as for Type Safety** - Better than string manipulation for typed values

## Notes

- The class is marked `final` preventing inheritance
- Copy assignment is deleted but copy construction is allowed (asymmetric)
- Most methods return `JSONDocument` by value (leverages RVO and move semantics)
- The internal representation uses `value_t` which is a variant type
- Paths are split on '.' characters for navigation
- Array operations support partial matching for element removal
