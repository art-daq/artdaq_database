# JSONDocument.cpp

**Path:** `artdaq-database/JsonDocument/JSONDocument.cpp`

**Implements:** [JSONDocument.h](./JSONDocument.h.md)

**Purpose:** This file implements the core document tree manipulation methods of the `JSONDocument` class, providing path-based navigation and modification operations that allow users to traverse, query, insert, replace, delete, and manipulate elements within JSON document structures using recursive tree traversal algorithms.

## Implementation Overview

The implementation uses recursive lambda functions to traverse the JSON document tree based on dot-notation paths. Each path is split into tokens, reversed for bottom-up traversal, and the recursion navigates through nested objects to find, insert, replace, or delete the target element.

## Key Algorithms

### Path Validation and Splitting

**Function:** `split_path_validate()`

Validates that a path is non-empty and splits it into individual tokens for tree traversal.

**Steps:**
1. Validate caller is not empty (confirm macro)
2. Validate path is not empty (throws `invalid_argument` if empty)
3. Call `split_path()` to tokenize on `.` delimiter
4. Validate tokens are not empty (throws `invalid_argument` if empty)
5. Reverse the token order for bottom-up recursion
6. Return reversed token vector

**Why this approach:** Reversing the path allows the recursive traversal to work from `currentDepth = tokens.size() - 1` down to 0, making it easier to check when the target depth is reached.

### Recursive Tree Traversal Pattern

All modification operations use the same traversal pattern:

**Steps:**
1. Split and validate path into tokens
2. Create recursive lambda with current depth tracking
3. At each depth:
   - Verify current value is an object type (throw if not)
   - Look up the path token in the object (throw if not found)
   - If at target depth (0), perform the operation
   - Otherwise, recurse into the matched value
4. Set `_isDirty` flag after successful modification
5. Return the affected value wrapped in a JSONDocument
6. Log and re-throw any exceptions

### findChildValue Algorithm

**Purpose:** Navigate to a path and return a const reference to the value found there.

**Pseudocode:**
```
recurse(value, depth):
  if value is not OBJECT: throw notfound_exception
  if path_token not in value: throw notfound_exception
  if depth == 0: return value[path_token]
  return recurse(value[path_token], depth - 1)
```

### replaceChild Algorithm

**Purpose:** Replace an existing value at a path with a new value.

**Pseudocode:**
```
recurse(value, depth):
  if value is not OBJECT: throw notfound_exception
  if path_token not in value: throw notfound_exception
  if depth == 0:
    swap(value[path_token], newValue)
    return newValue (which now contains old value)
  return recurse(value[path_token], depth - 1)
```

### insertChild Algorithm

**Purpose:** Insert a new value at a path (fails if path exists).

**Pseudocode:**
```
recurse(value, depth):
  if value is not OBJECT: throw notfound_exception
  count = value.count(path_token)
  if depth == 0 and count != 0: throw (exists, use replace)
  if depth != 0 and count == 0: throw notfound_exception
  if depth == 0 and count == 0:
    value[path_token] = newValue
    return newValue
  return recurse(value[path_token], depth - 1)
```

### deleteChild Algorithm

**Purpose:** Delete an element at a path and return the deleted value.

**Pseudocode:**
```
recurse(value, depth):
  if value is not OBJECT: throw notfound_exception
  if path_token not in value: throw notfound_exception
  if depth == 0:
    return value.delete_at(path_token)
  return recurse(value[path_token], depth - 1)
```

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/JsonDocument/JSONDocument.h` | Class definition for JSONDocument |
| `artdaq-database/DataFormats/Json/json_types_impl.h` | Implementation details for JSON types and operations, visitor functions like `print_visitor` |

## Internal Functions

### `validate(path_t const& path, std::string const& caller)`

**Brief:** Validates that a path is not empty before attempting operations, throwing an exception with the caller name if invalid.

**Called by:** `findChild()`, `removeChild()`

**Purpose:** Provides consistent path validation with informative error messages.

**Parameters:**
- `path` - The path to validate
- `caller` - Name of calling function for error messages

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When path is empty |

### `split_path_validate(path_t const& path, std::string const& caller) -> std::vector<path_t>`

**Brief:** Validates a path, splits it into components, and reverses the order for bottom-up traversal.

**Called by:** `replaceChild()`, `insertChild()`, `deleteChild()`, `findChildValue()`

**Purpose:** Centralizes path processing logic for all tree traversal operations.

**Parameters:**
- `path` - Dot-notation path to split
- `caller` - Name of calling function for error messages

**Returns:** Vector of path tokens in reversed order

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When path is empty or produces no tokens |

**Example:**
```cpp
// Input: "document.data.field"
// Split: ["document", "data", "field"]
// Reversed: ["field", "data", "document"]
```

### `print_visitor(value_t const&) -> std::string`

**Brief:** Converts a JSON value to a human-readable string for logging and error messages using Boost visitor pattern.

**Called by:** All recursive operations for TRACE logging

**Purpose:** Provides debug output of JSON values.

**Note:** Defined in `JSONDocument_utils.cpp` as `boost::apply_visitor(jsn::print_visitor(), value)`.

## Functions

### `JSONDocument::findChildValue(path_t const& path) const -> value_t const&`

**Brief:** Private method that finds and returns a const reference to the value at the specified path using recursive traversal.

**Parameters:**
- `path` - Dot-notation path to the element

**Preconditions:**
- Path must not be empty
- All path components must exist
- All intermediate values must be object types

**Returns:** Const reference to the value at the specified path

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When path is empty |
| `notfound_exception` | When path component doesn't exist |
| `notfound_exception` | When intermediate value is not an object |

**Thread Safety:** safe (const method)

**TRACE Levels:** 20-23

### `JSONDocument::findChild(path_t const& path) const -> JSONDocument`

**Brief:** Finds a child element and returns it wrapped in a JSONDocument with the full path as the key.

**Parameters:**
- `path` - Dot-notation path to the element

**Returns:** Document in format `{"path.to.element": <found_value>}`

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When path is empty |
| `notfound_exception` | When path doesn't exist |

**Thread Safety:** safe (const method)

**TRACE Levels:** 24-27

**Example:**
```cpp
auto doc = JSONDocument(R"({"server": {"port": 8080}})");
auto child = doc.findChild("server.port");
// Returns: {"server.port": 8080}
```

### `JSONDocument::findChildDocument(path_t const& path) const -> JSONDocument`

**Brief:** Finds a child element and returns its value directly as a document (value must be an object).

**Parameters:**
- `path` - Dot-notation path to the element

**Preconditions:**
- Value at path must be an object type

**Returns:** JSONDocument containing the found object value directly

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When path is empty |
| `notfound_exception` | When path doesn't exist |
| `notfound_exception` | When found value is not an object type |

**Thread Safety:** safe (const method)

**TRACE Levels:** 28-30

### `JSONDocument::replaceChild(JSONDocument const& newChild, path_t const& path) -> JSONDocument`

**Brief:** Replaces an existing child element with a new value and returns the old value using swap semantics.

**Parameters:**
- `newChild` - Document containing the new value
- `path` - Dot-notation path to the element to replace

**Returns:** Document containing the old (replaced) value wrapped with path as key

**Postconditions:**
- Document is modified with new value at path
- `_isDirty` is set to true

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When path is empty |
| `notfound_exception` | When path doesn't exist |

**Thread Safety:** unsafe

**TRACE Levels:** 31-35

### `JSONDocument::insertChild(JSONDocument const& newChild, path_t const& path) -> JSONDocument`

**Brief:** Inserts a new child element at the specified path. Fails if element already exists at that path.

**Parameters:**
- `newChild` - Document containing the value to insert
- `path` - Dot-notation path where to insert

**Returns:** Document containing the inserted value

**Postconditions:**
- New element exists at the specified path
- `_isDirty` is set to true

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When path is empty |
| `notfound_exception` | When parent path doesn't exist |
| `notfound_exception` | When target path already exists (message: "Child exists, call replace instead") |

**Thread Safety:** unsafe

**TRACE Levels:** 36-40

### `JSONDocument::deleteChild(path_t const& path) -> JSONDocument`

**Brief:** Deletes a child element at the specified path and returns the deleted value using object's `delete_at` method.

**Parameters:**
- `path` - Dot-notation path to the element to delete

**Returns:** Document containing the deleted value

**Postconditions:**
- Element no longer exists at the specified path
- `_isDirty` is set to true

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When path is empty |
| `notfound_exception` | When path doesn't exist |

**Thread Safety:** unsafe

**TRACE Levels:** 41-45

### `JSONDocument::appendChild(JSONDocument const& newChild, path_t const& path) -> JSONDocument`

**Brief:** Appends an element to an array at the specified path by getting the array via `findChildValue` and calling `push_back`.

**Parameters:**
- `newChild` - Document containing the value to append (payload extracted via `getPayloadValueForKey("null")`)
- `path` - Dot-notation path to the array

**Preconditions:**
- Path must point to an existing array

**Returns:** Document containing the appended value

**Postconditions:**
- New element is added to end of array
- `_isDirty` is set to true

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When path is empty |
| `notfound_exception` | When path doesn't exist |
| `std::exception` | When value at path is not an array |

**Thread Safety:** unsafe

**TRACE Levels:** 46-49

### `JSONDocument::removeChild(JSONDocument const& delChild, path_t const& path) -> JSONDocument`

**Brief:** Removes all matching elements from an array at the specified path using partial object matching via the `matches()` function.

**Parameters:**
- `delChild` - Document containing the pattern to match for deletion
- `path` - Dot-notation path to the array

**Returns:** Document containing array of removed values under key "0"

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

**TRACE Levels:** 50-54

**Example:**
```cpp
JSONDocument doc(R"({"items": [{"name":"old"}, {"name":"new"}, {"name":"old"}]})");
JSONDocument pattern(R"({"name":"old"})");
auto removed = doc.removeChild(pattern, "items");
// Returns: {"0": [{"name":"old"}, {"name":"old"}]}
// doc now: {"items": [{"name":"new"}]}
```

### `JSONDocument::extract() -> value_t`

**Brief:** Extracts the internal value_t from the document using swap, leaving the document with an empty object.

**Parameters:** None

**Returns:** The internal `value_t` that was held by the document

**Postconditions:**
- Document is left with an empty object `{}`
- `_isDirty` is set to true

**Thread Safety:** unsafe

### `compareDocumentVersions(JSONDocument const& a, JSONDocument const& b) -> bool`

**Brief:** Compares two documents by their "version" fields for sorting purposes using lexicographic string comparison.

**Parameters:**
- `a` - First document to compare
- `b` - Second document to compare

**Returns:** `true` if a's version is less than b's version

**Thread Safety:** safe

**Example:**
```cpp
std::sort(docs.begin(), docs.end(), compareDocumentVersions);
```

### `debug::JSONDocument()`

**Brief:** Enables maximum TRACE logging for debugging JSONDocument operations by setting all TRACE levels.

**Parameters:** None

**Returns:** None

**Side Effects:**
- Configures TRACE logging for JSONDocument debugging

**TRACE Level:** 55 (activation message)

## Performance Considerations

- **Recursion depth:** Limited by path depth; deeply nested documents require more stack frames
- **Reference returns:** `findChildValue` returns references to avoid copying large values
- **Swap operations:** `replaceChild` uses swap instead of copy for efficiency
- **Move semantics:** Return values leverage RVO and move semantics
- **Lazy caching:** The `_isDirty` flag prevents unnecessary re-serialization in `to_string()`

## Error Handling Strategy

All public methods use function-try blocks:
```cpp
JSONDocument JSONDocument::method(args) try {
  // implementation
} catch (std::exception& ex) {
  TLOG(level) << "method() failed; Error:" << ex.what();
  throw;
}
```

This pattern:
1. Logs all exceptions with TRACE including error message
2. Re-throws to preserve the original exception type and message
3. Provides consistent debugging output across all methods

## Testing Notes

- **Unit tests:** `test/JsonDocument/JSONDocument_t.cc`
- **Key test cases:**
  - Path navigation with various depths
  - Insert/replace/delete operations
  - Array append/remove operations
  - Error conditions (empty paths, missing elements)
  - Edge cases (empty documents, single-element documents)

## Maintenance Notes

### TRACE Logging Levels
The implementation uses incrementing TRACE levels organized by operation type:
- **20-29:** findChildValue and findChild operations
- **30-39:** replaceChild and insertChild operations
- **40-49:** deleteChild and appendChild operations
- **50-55:** removeChild and utility operations

### Implementation Patterns
All tree manipulation methods follow a consistent pattern:
1. Log entry with TRACE including path argument
2. Split and validate path using `split_path_validate()`
3. Define recursive lambda for traversal with depth tracking
4. Invoke lambda with root value `_value` and starting depth `tokens.size() - 1`
5. Mark `_isDirty = true` if document was modified
6. Return result wrapped in JSONDocument
7. Catch exceptions in function-try block, log error, re-throw

### Key Implementation Details

- **Path token reversal:** Paths are reversed so traversal goes from root to leaf while counting depth down to 0
- **Const correctness:** `findChildValue` has both const and non-const overloads; non-const uses const_cast internally
- **Value extraction:** `getPayloadValueForKey()` handles payload wrapping for child documents
- **Matching semantics:** `matches()` function performs partial object matching for `removeChild()`

## See Also

- [JSONDocument.h.md](./JSONDocument.h.md) - Header file with class declaration
- [JSONDocument_utils.cpp.md](./JSONDocument_utils.cpp.md) - Utility function implementations including constructors, file I/O, and `matches()`
- [common.h.md](./common.h.md) - Common includes and types
