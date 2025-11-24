# JSONDocument.cpp

## File Overview

This file implements the core document tree manipulation methods of the `JSONDocument` class. It provides the path-based navigation and modification API that allows users to traverse, query, insert, replace, delete, and manipulate elements within JSON document structures.

**Location**: `/home/user/artdaq-database/artdaq-database/JsonDocument/JSONDocument.cpp`

## Dependencies

```cpp
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/DataFormats/Json/json_types_impl.h"
```

**Key Dependencies**:
- **JSONDocument.h** - Class definition
- **json_types_impl.h** - Implementation details for JSON types and operations

## TRACE Configuration

```cpp
#ifdef TRACE_NAME
#undef TRACE_NAME
#endif
#define TRACE_NAME "JSONDocument.cpp"
```

Sets the TRACE subsystem name for all log messages from this file.

## Using Declarations

```cpp
using artdaq::database::json::array_t;
using artdaq::database::json::object_t;
using artdaq::database::json::type_t;
using artdaq::database::json::value_t;
using artdaq::database::sharedtypes::unwrap;
using namespace artdaq::database;
using artdaq::database::docrecord::JSONDocument;
using artdaq::database::docrecord::split_path;

namespace dbdr = artdaq::database::docrecord;
```

## Helper Functions

### print_visitor

```cpp
std::string print_visitor(value_t const&);
```

Forward-declared utility function that converts a `value_t` to a human-readable string representation for debugging and logging.

### validate

```cpp
void validate(path_t const& path, std::string const& caller) {
  if (path.empty()) {
    throw invalid_argument("JSONDocument")
        << "Failed calling " << caller << "(): Invalid path; path is empty";
  }
}
```

**Purpose**: Validates that a path is not empty before attempting operations.

**Parameters**:
- `path` - The path to validate
- `caller` - Name of the calling function (for error messages)

**Throws**: `invalid_argument` if path is empty

### split_path_validate

```cpp
std::vector<path_t> split_path_validate(path_t const& path, std::string const& caller)
```

**Purpose**: Validates a path, splits it into components, and reverses the order for bottom-up traversal.

**Algorithm**:
1. Validates caller name is not empty
2. Validates path is not empty using `validate()`
3. Splits path using `split_path()` ("a.b.c" → ["a", "b", "c"])
4. Reverses the tokens (["a", "b", "c"] → ["c", "b", "a"])
5. Returns reversed tokens

**Why Reverse?**: The recursive traversal algorithms work from the deepest level up, so they need reversed paths.

**Example**:
- Input: `"document.data.field"`
- Split: `["document", "data", "field"]`
- Reversed: `["field", "data", "document"]`
- Traversal: Start at "field", work up through "data" to "document"

## Core Implementation Methods

### findChildValue (const version)

```cpp
value_t const& JSONDocument::findChildValue(path_t const& path) const
```

**Purpose**: Finds and returns a reference to the value at the specified path.

**Algorithm**:
1. Split and validate the path
2. Recursively traverse the document tree
3. At each level:
   - Verify current value is an object
   - Check if the path token exists as a key
   - If at final depth (depth 0), return the matched value
   - Otherwise, recurse deeper
4. Return reference to found value

**Recursive Lambda**:
```cpp
std::function<value_t const&(value_t const&, std::size_t)> recurse =
    [&](value_t const& childValue, std::size_t currentDepth) -> value_t const& {
  // Traversal logic
};
```

**Key Features**:
- **Bottom-up traversal** - Starts at highest depth, works down to 0
- **Type checking** - Ensures each node is an object
- **Detailed error messages** - Includes path, key, and value information
- **Reference return** - No copying of potentially large JSON structures

**Throws**: `notfound_exception` if:
- Path component doesn't exist
- Intermediate value is not an object
- Any step of traversal fails

**TRACE Points**:
- Level 20: Entry with path
- Level 21: Each recursion step
- Level 22: Successful find
- Level 23: Error during search

### findChild

```cpp
JSONDocument JSONDocument::findChild(path_t const& path) const
```

**Purpose**: Finds a child element and returns it wrapped in a `JSONDocument` with the path as the key.

**Algorithm**:
1. Validate path
2. Call `findChildValue()` to get the value
3. Create new empty document
4. Insert found value with path as key
5. Return the document

**Return Format**:
```json
{
  "path.to.element": <found_value>
}
```

**Example**:
```cpp
auto child = doc.findChild("server.port");
// Returns: {"server.port": 8080}
```

**TRACE Points**:
- Level 24: Entry
- Level 25: Found value
- Level 26: Success
- Level 27: Error

### findChildDocument

```cpp
JSONDocument JSONDocument::findChildDocument(path_t const& path) const
```

**Purpose**: Finds a child element and returns its value directly as a document (value must be an object).

**Algorithm**:
1. Validate path
2. Call `findChildValue()` to get the value
3. Verify the value is of type `OBJECT`
4. Create new document from the value
5. Return the document

**Difference from findChild**:
- `findChild`: Returns `{"path": value}`
- `findChildDocument`: Returns `value` directly

**Use Cases**:
- When you know the value is an object
- When you want to work with the object directly
- For extracting nested document structures

**Throws**: `notfound_exception` if:
- Path doesn't exist (from `findChildValue`)
- Found value is not an object type

**TRACE Points**:
- Level 28: Entry
- Level 29: Success
- Level 30: Error

### replaceChild

```cpp
JSONDocument JSONDocument::replaceChild(JSONDocument const& newChild, path_t const& path)
```

**Purpose**: Replaces an existing child element with a new value and returns the old value.

**Algorithm**:
1. Split and validate path
2. Extract new value from `newChild` using path token
3. Recursively traverse to target
4. At target (depth 0):
   - Swap old value with new value
   - Return old value
5. Mark document as dirty
6. Return old value wrapped in document

**Key Operation**:
```cpp
matchedValue.swap(newValue);  // Efficient swap instead of copy
return newValue;               // newValue now holds the old value after swap
```

**Modifies**: The document's internal structure (sets `_isDirty = true`)

**Returns**: Document containing the old (replaced) value

**Throws**: `notfound_exception` if:
- Path doesn't exist
- Intermediate value is not an object

**TRACE Points**:
- Level 31: Entry
- Level 32: New value
- Level 33: Each recursion
- Level 34: Success
- Level 35: Error

### insertChild

```cpp
JSONDocument JSONDocument::insertChild(JSONDocument const& newChild, path_t const& path)
```

**Purpose**: Inserts a new child element at the specified path. Throws if element already exists.

**Algorithm**:
1. Split and validate path
2. Extract new value from `newChild`
3. Recursively traverse to target
4. At each level, check:
   - If at target depth (0) and element exists → error (use replace instead)
   - If not at target and element missing → error (parent doesn't exist)
   - If at target and element missing → insert and return
5. Mark document as dirty

**Insert vs. Replace**:
- **Insert**: Fails if element exists (prevents accidental overwrites)
- **Replace**: Requires element to exist (prevents accidental creates)

**Returns**: Document containing the inserted value

**Throws**: `notfound_exception` if:
- Element already exists at target path
- Parent path doesn't exist
- Intermediate value is not an object

**TRACE Points**:
- Level 36: Entry
- Level 37: New value
- Level 38: Each recursion
- Level 39: Success
- Level 40: Error

### deleteChild

```cpp
JSONDocument JSONDocument::deleteChild(path_t const& path)
```

**Purpose**: Deletes a child element at the specified path and returns the deleted value.

**Algorithm**:
1. Split and validate path
2. Recursively traverse to target
3. At target (depth 0):
   - Call `childDocument.delete_at(path_token)`
   - Return deleted value
4. Mark document as dirty

**Key Operation**:
```cpp
return childDocument.delete_at(path_token);  // Erase and return value
```

**Modifies**: The document's internal structure (sets `_isDirty = true`)

**Returns**: Document containing the deleted value

**Throws**: `notfound_exception` if:
- Path doesn't exist
- Intermediate value is not an object

**TRACE Points**:
- Level 41: Entry with JSON buffer
- Level 42: Entry with path
- Level 43: Each recursion
- Level 44: Success
- Level 45: Error

### appendChild

```cpp
JSONDocument JSONDocument::appendChild(JSONDocument const& newChild, path_t const& path)
```

**Purpose**: Appends an element to an array at the specified path.

**Algorithm**:
1. Extract new value from `newChild` (using "null" as key)
2. Find the array at the specified path
3. Unwrap the value as an `array_t`
4. Push the new value to the end of the array
5. Mark document as dirty
6. Return the appended value

**Array Operation**:
```cpp
auto& valueArray = unwrap(findChildValue(path)).value_as<array_t>();
valueArray.push_back(newValue);
```

**Requirements**:
- Path must point to an array
- Array must already exist

**Returns**: Document containing the appended value

**Throws**: Exception if path is not an array or doesn't exist

**TRACE Points**:
- Level 46: Entry
- Level 47: New value
- Level 48: Success
- Level 49: Error

### removeChild

```cpp
JSONDocument JSONDocument::removeChild(JSONDocument const& delChild, path_t const& path)
```

**Purpose**: Removes all matching elements from an array at the specified path.

**Algorithm**:
1. Extract delete pattern from `delChild`
2. Find the array at the specified path
3. Create result document with array for deleted values
4. Iterate through array:
   - If element matches pattern → add to deleted list, remove from array
   - If element doesn't match → keep in array
5. Mark document as dirty
6. Return document with deleted values

**Matching**:
Uses the `matches()` function which supports partial matching for objects.

**Multiple Removals**:
Removes **all** matching elements, not just the first one.

**Returns**: Document containing array of removed values under key "0"

**Example**:
```cpp
// Remove all items with name="old"
JSONDocument pattern(R"({"name":"old"})");
auto removed = doc.removeChild(pattern, "items");
// Returns: {"0": [<array of removed items>]}
```

**TRACE Points**:
- Level 50: Entry
- Level 51: Delete pattern
- Level 53: Success
- Level 54: Error

### extract

```cpp
value_t JSONDocument::extract()
```

**Purpose**: Extracts the internal `value_t` from the document, leaving the document empty.

**Algorithm**:
```cpp
value_t tmp = jsn::object_t{};
std::swap(tmp, _value);
_isDirty = true;
return tmp;
```

**Characteristics**:
- **Destructive** - Empties the source document
- **Efficient** - Uses swap, no copying
- **Invalidates cache** - Sets dirty flag

**Use Case**: Transfer ownership of the JSON AST without copying.

## Version Comparison

### compareDocumentVersions

```cpp
bool dbdr::compareDocumentVersions(JSONDocument const& a, JSONDocument const& b) {
  return a.value_as<std::string>(version) < b.value_as<std::string>(version);
}
```

**Purpose**: Compares two documents by their version fields (lexicographic comparison).

**Use Case**: Sorting documents by version, finding newest/oldest version.

**Example**:
```cpp
std::vector<JSONDocument> docs = /* ... */;
std::sort(docs.begin(), docs.end(), compareDocumentVersions);
// Now sorted by version
```

## Debug Function

### debug::JSONDocument

```cpp
void dbdr::debug::JSONDocument() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);
  TLOG(55) << "artdaq::database::JSONDocument trace_enable";
}
```

**Purpose**: Enables maximum TRACE logging for debugging JSONDocument operations.

**Usage**:
```cpp
artdaq::database::docrecord::debug::JSONDocument();
// Now all TRACE calls will output
```

## Implementation Patterns

### Recursive Tree Traversal

Most methods use a common pattern:
1. **Split path** into tokens (reversed for bottom-up)
2. **Define recursive lambda** that:
   - Takes current value and depth
   - Validates value is an object
   - Checks if key exists
   - If at target depth (0), performs operation
   - Otherwise, recurses to next level
3. **Invoke lambda** with root value and starting depth
4. **Mark dirty** if modified

### Error Handling

Consistent error handling:
```cpp
try {
  // Operation
  TLOG(N) << "Success message";
  return result;
} catch (std::exception& ex) {
  TLOG(N+1) << "Error message: " << ex.what();
  throw;  // Re-throw
}
```

### TRACE Logging Levels

- **20-29**: findChildValue and findChild operations
- **30-39**: replaceChild and insertChild operations
- **40-49**: deleteChild, appendChild operations
- **50-55**: removeChild, debug operations

## Performance Considerations

1. **Reference Returns**: `findChildValue` returns references to avoid copying
2. **Swap Operations**: `replaceChild` uses swap instead of copy
3. **Move Semantics**: Return values leverage RVO and move semantics
4. **Lazy Cache**: `_isDirty` flag prevents unnecessary serialization
5. **Bottom-up Traversal**: Efficient path navigation

## Thread Safety

Not thread-safe:
- Modifies `_value` and `_isDirty` without synchronization
- Multiple readers safe only if no writers
- Requires external synchronization for concurrent access

## Error Messages

All error messages follow a consistent format:
```
"JSONDocument: Failed calling <method>(): <reason>; <details>"
```

Example:
```
"JSONDocument: Failed calling findChildValue(): Search failed for JSON element name=field, search path =<document.data.field>."
```

## Related Files

- **JSONDocument.h** - Class definition
- **JSONDocument_utils.cpp** - Additional utility implementations
- **json_types_impl.h** - JSON type operations

## Best Practices

1. **Check paths** before operations to provide better error messages
2. **Use findChild** when you need the path preserved in the result
3. **Use findChildDocument** when you want to work with the object directly
4. **Use insertChild** for new elements to prevent accidental overwrites
5. **Use replaceChild** for existing elements to ensure they exist
6. **Handle notfound_exception** for optional fields
7. **Enable debug traces** when investigating path traversal issues

## Notes

- All tree navigation happens recursively using lambdas
- Paths are validated before any traversal begins
- Type checking occurs at every level of traversal
- The `_isDirty` flag is set after any modification
- Extensive TRACE logging available at multiple levels for debugging
- The implementation uses the visitor pattern extensively for JSON value operations
