# JSONDocument_utils.cpp

**Path:** `artdaq-database/JsonDocument/JSONDocument_utils.cpp`

**Implements:** [JSONDocument.h](./JSONDocument.h.md), [JSONDocumentBuilder.h](./JSONDocumentBuilder.h.md)

**Purpose:** This file implements utility functions and supplementary methods for both the `JSONDocument` and `JSONDocumentBuilder` classes. It provides JSON serialization/deserialization, value matching for array operations, path splitting, file I/O operations, and the critical `createFromData` method that wraps user data in the proper database structure.

## Implementation Overview

This file contains a mix of free functions, `JSONDocument` methods (constructors, serialization, file I/O), and `JSONDocumentBuilder` methods (data import). The code implements lazy serialization with caching, recursive value matching, and flexible document structure handling.

## Key Algorithms

### Lazy Serialization with Caching

The `to_string()` method implements lazy serialization:
- Uses `_isDirty` flag to track if document was modified since last serialization
- Caches serialized JSON in `_cached_json_buffer`
- Only regenerates JSON when dirty flag is set
- Improves performance for read-heavy operations

### Value Matching for Arrays

The `matches()` function enables flexible array element matching:
- Supports partial object matching (template can have subset of fields)
- Recursive comparison for nested structures
- Used by `removeChild()` to find elements to remove from arrays

### Flexible Data Import

The `_importUserData()` method handles various document formats:
- Imports optional fields (metadata, changelog, origin, collection, attachments)
- Falls back to treating entire document as data if no specific structure found
- Uses try-catch to silently skip missing optional fields

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/BasicTypes/data_json.h` | JSON data type definitions, `JsonData` wrapper |
| `artdaq-database/JsonDocument/JSONDocument.h` | Document class definition |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | Builder class definition |
| `artdaq-database/JsonDocument/common.h` | Module utilities, types, and literals |
| `<boost/filesystem.hpp>` | File existence check and copy operations |
| `<utility>` | `std::move` for move semantics |

## Functions

### `print_visitor(value_t const& value) -> std::string`

**Brief:** Converts a JSON value to a human-readable string for debugging by applying `jsn::print_visitor` to the variant type.

**Parameters:**
- `value` - JSON value variant to convert

**Returns:** Human-readable string representation suitable for logging

**Thread Safety:** safe (pure function)

### `tostring_visitor(value_t const& value) -> std::string`

**Brief:** Converts a JSON value to its raw string representation, providing the unformatted value rather than debug output.

**Parameters:**
- `value` - JSON value variant to convert

**Returns:** Raw string representation of the value

**Thread Safety:** safe (pure function)

### `matches(value_t const& left, value_t const& right) -> bool`

**Brief:** Recursively compares two JSON values for equality with support for partial object matching, where the template can have more fields than the candidate.

**Parameters:**
- `left` - Template value (pattern to match against)
- `right` - Candidate value (value being tested)

**Returns:** `true` if values match (partial match for objects), `false` otherwise

**Thread Safety:** safe (pure function)

**Algorithm:**
1. **Type Check:** Return false if types differ
2. **Object Matching:** Uses partial matching where template can have more fields than candidate; recursively matches nested values
3. **Array Matching:** Requires same size; element-by-element comparison
4. **Primitive Matching:** Converts to strings and compares

**TRACE Level:** 20 (logs object differences)

**Example:**
```cpp
// Template: {"name": "test"}
// Candidate: {"name": "test", "value": 42}
// Result: true (partial match succeeds)
```

**Known Issue:** Contains bug on line 79 - uses `unwrap(left)` twice instead of `unwrap(right)` for the right array.

### `utl::split_path(std::string const& path) -> std::vector<std::string>`

**Brief:** Splits a dot-notation path into individual components by replacing dots with spaces and using string stream tokenization.

**Parameters:**
- `path` - Dot-notation path string (e.g., "document.data.field")

**Returns:** Vector of path components (e.g., `["document", "data", "field"]`)

**Thread Safety:** safe (pure function)

**Example:**
```cpp
auto parts = utl::split_path("document.data.field");
// parts = {"document", "data", "field"}
```

### `utl::operator<<(std::ostream& os, JSONDocument const& document) -> std::ostream&`

**Brief:** Stream output operator that writes the document's JSON string representation to an output stream.

**Parameters:**
- `os` - Output stream to write to
- `document` - Document to output

**Returns:** Reference to the output stream for chaining

**Thread Safety:** conditional (depends on stream thread safety)

### `JSONDocument::readJson(std::string const& json) -> value_t` [static]

**Brief:** Parses a JSON string into internal `value_t` representation using the JSON reader.

**Parameters:**
- `json` - JSON string to parse

**Preconditions:**
- JSON string must not be empty
- JSON must be syntactically valid

**Returns:** Parsed `value_t` containing the JSON structure

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When JSON string is empty |
| `invalid_argument` | When JSON is malformed or unparseable |

**Thread Safety:** safe (static method, no shared state)

### `JSONDocument::writeJson() const -> std::string`

**Brief:** Serializes the internal `value_t` to a JSON string using the JSON writer.

**Preconditions:**
- Internal value must be of type OBJECT

**Returns:** JSON string representation of the document

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When internal type is not OBJECT |
| `invalid_argument` | When serialization fails (invalid AST) |

**Thread Safety:** conditional (const but accesses mutable cache)

### `JSONDocument::to_string() const -> std::string`

**Brief:** Returns the JSON string representation with caching for performance, only regenerating the JSON when the dirty flag is set.

**Returns:** Cached or freshly serialized JSON string

**Postconditions:**
- Result is cached in `_cached_json_buffer`
- `_isDirty` flag is set to false

**Thread Safety:** conditional (modifies mutable members `_cached_json_buffer` and `_isDirty`)

### `JSONDocument::operator std::string() const`

**Brief:** Implicit conversion operator to string that delegates to `to_string()` method for seamless string conversion.

**Returns:** JSON string representation (identical to `to_string()`)

**Thread Safety:** conditional (same as `to_string()`)

### `JSONDocument::empty() const -> bool`

**Brief:** Checks if the document has no value by examining whether the internal value is empty.

**Returns:** `true` if document is empty, `false` otherwise

**Thread Safety:** safe (const method, no state modification)

### `JSONDocument::JSONDocument(JsonData const& data)`

**Brief:** Constructor that parses JSON content from a `JsonData` wrapper structure into the document.

**Parameters:**
- `data` - `JsonData` structure containing JSON string

**Postconditions:**
- Document contains parsed JSON structure
- `_isDirty` is set to true
- `_cached_json_buffer` is initialized to empty JSON literal

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When JSON in data is empty or malformed |

**Thread Safety:** safe (construction)

### `JSONDocument::JSONDocument(std::string const& json)`

**Brief:** Constructor that parses a JSON string directly into the document.

**Parameters:**
- `json` - JSON string to parse

**Postconditions:**
- Document contains parsed JSON structure
- `_isDirty` is set to true

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When JSON string is empty or malformed |

**Thread Safety:** safe (construction)

### `JSONDocument::JSONDocument(value_t value)`

**Brief:** Constructor that wraps an existing `value_t` variant in a document without parsing.

**Parameters:**
- `value` - JSON value variant to wrap (moved)

**Postconditions:**
- Document wraps the provided value
- `_isDirty` is set to true

**Thread Safety:** safe (construction)

### `JSONDocument::JSONDocument()`

**Brief:** Default constructor that creates an empty document containing an empty JSON object.

**Postconditions:**
- Document contains empty JSON object `{}`
- `_isDirty` is set to true

**Thread Safety:** safe (construction)

### `JSONDocument::cached_json_buffer() const -> std::string const&`

**Brief:** Returns a reference to the cached JSON string without triggering re-serialization.

**Returns:** Const reference to cached JSON string (may be stale if document was modified)

**Thread Safety:** safe (const method)

### `JSONDocument::getPayloadValueForKey(object_t::key_type const& key) const -> value_t const&`

**Brief:** Extracts the payload value from the document, handling different document formats with fallback logic.

**Parameters:**
- `key` - Key to look for in payload section

**Preconditions:**
- `key` must not be empty

**Returns:** Const reference to the extracted value

**Thread Safety:** safe (const method)

**TRACE Level:** 21

**Logic:**
1. If document has "payload" key with requested key inside: return that nested value
2. If document has "payload" key without requested key: return entire payload
3. If document has single key: return that value
4. Otherwise: return entire document value

### `JSONDocument::equals(JSONDocument const& other) const -> bool`

**Brief:** Compares two documents for structural equality by comparing their internal value representations.

**Parameters:**
- `other` - Document to compare with

**Returns:** `true` if documents are structurally equal, `false` otherwise

**Thread Safety:** safe (const method)

**TRACE Levels:** 22 (result), 23 (error details)

### `JSONDocument::loadFromFile(std::string const& fileName) -> JSONDocument` [static]

**Brief:** Static factory method that loads and parses a JSON document from a file.

**Parameters:**
- `fileName` - Path to the JSON file

**Returns:** `JSONDocument` containing the parsed file content

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When file cannot be opened or read |
| `runtime_error` | When JSON parsing fails |

**Thread Safety:** safe (static method)

**Side Effects:**
- Reads from filesystem

### `JSONDocument::saveToFile(std::string const& fileName) -> bool`

**Brief:** Saves the JSON document to a file, creating a backup (.bak) of any existing file first.

**Parameters:**
- `fileName` - Path to the output file

**Returns:** `true` on success

**Postconditions:**
- File contains JSON representation of document
- If file existed, backup created with `.bak` extension

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When fileName is empty |
| `runtime_error` | When file write fails |

**Thread Safety:** unsafe (filesystem operations)

**Side Effects:**
- Writes to filesystem
- Creates backup file if original exists
- Uses Boost.Filesystem for copy operations

### `JSONDocument::value(JSONDocument const& document) -> std::string` [static]

**Brief:** Static utility method that extracts the primary value from a document as a string.

**Parameters:**
- `document` - Document to extract value from

**Returns:** String representation of the document's payload value

**Thread Safety:** safe (static method)

**TRACE Level:** 39

### `JSONDocument::value_at(JSONDocument const& document, std::size_t index) -> std::string` [static]

**Brief:** Static utility method that extracts a value at a specific index from an array document.

**Parameters:**
- `document` - Document containing an array
- `index` - Zero-based index into the array

**Returns:** String representation of the value at the specified index

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When array is empty |
| `runtime_error` | When index exceeds array size |

**Thread Safety:** safe (static method)

**TRACE Levels:** 40 (document), 41 (index)

### `JSONDocument::findChildValue(path_t const& path) -> value_t&` [non-const overload]

**Brief:** Non-const version of findChildValue that returns a mutable reference by casting away constness from the const version.

**Parameters:**
- `path` - Dot-notation path to the child value

**Returns:** Mutable reference to the child value

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `notfound_exception` | When path does not exist |

**Thread Safety:** unsafe (returns mutable reference)

**TRACE Level:** 42 (error logging)

### `JSONDocumentBuilder::createFromData(JSONDocument doc) -> JSONDocumentBuilder&`

**Brief:** Creates a database document from user data by resetting the overlay, creating a template, importing user data fields, and refreshing the overlay structure.

**Parameters:**
- `doc` - User-provided document containing configuration data

**Returns:** Reference to this builder for method chaining

**Postconditions:**
- Document contains user data wrapped in database structure
- Overlay is refreshed to reflect new structure

**Thread Safety:** unsafe (modifies internal state)

**TRACE Level:** 24

**Process:**
1. Reset overlay to nullptr
2. Move user document to local variable
3. Create empty template using `template__empty_document`
4. Create initial overlay from template
5. Import user data via `_importUserData()`
6. Refresh overlay to reflect imported data
7. Return self reference for chaining

### `JSONDocumentBuilder::_importUserData(JSONDocument const& document)` [private]

**Brief:** Private method that imports optional user data fields into the document structure, using try-catch to silently skip missing optional fields.

**Parameters:**
- `document` - User document to import data from

**Thread Safety:** unsafe (modifies builder state)

**TRACE Levels:** 25-38

**Fields Imported (in order):**
1. `document.metadata` - Document metadata section
2. `changelog` - Change history
3. `origin` - Data origin information
4. `collection` - Collection assignment
5. `attachments` - File attachments
6. `document.data` - Primary configuration data
7. Fallback: entire document treated as data if no structure found

### `JSONDocumentBuilder::SaveUndo() -> result_t`

**Brief:** Placeholder method for saving undo state that currently always returns Success without actually preserving state.

**Returns:** `Success()` (always)

**Note:** Actual state preservation not implemented

### `JSONDocumentBuilder::CallUndo() noexcept -> result_t`

**Brief:** Placeholder method for restoring undo state that currently always returns Success without actually restoring state.

**Returns:** `Success()` (always, catches all exceptions internally)

**Note:** Actual state restoration not implemented

### `debug::JSONDocumentUtils()`

**Brief:** Enables maximum TRACE logging for debugging utility operations by configuring the TRACE subsystem.

**Side Effects:**
- Configures TRACE name to "JSONDocument_utils.cpp"
- Sets TRACE level to maximum (0xFFFFFFFFFFFFFFFFLL)
- Configures memory and slow modes

**TRACE Level:** 43 (activation message)

### `toJSONDocument<string_pair_t>(string_pair_t const& pair) -> JSONDocument` [template specialization]

**Brief:** Template specialization that converts a string pair to a JSON document with the format `{"first_value": "second_value"}`.

**Parameters:**
- `pair` - String pair (std::pair<std::string, std::string>) to convert

**Returns:** `JSONDocument` containing the converted pair

**Thread Safety:** safe (creates new document)

**Example:**
```cpp
string_pair_t pair("name", "value");
auto doc = toJSONDocument(pair);
// Result: {"name":"value"}
```

## Known Issues

1. **Array Matching Bug:** Line 79 uses `unwrap(left)` twice:
   ```cpp
   auto const& rightObj = unwrap(left).value_as<const array_t>();
   // Should be: unwrap(right).value_as<const array_t>();
   ```

2. **TRACE Message Inconsistencies:** Some log messages say "origin" when referring to other fields (lines 336, 346)

3. **Variable Name Typo:** Line 421 uses `myslef` instead of `myself` in `findChildValue()`

## TRACE Logging Levels

| Level | Function | Purpose |
|-------|----------|---------|
| 20 | `matches()` | Object differences |
| 21 | `getPayloadValueForKey()` | Document inspection |
| 22-23 | `equals()` | Comparison results |
| 24 | `createFromData()` | Builder operations |
| 25-38 | `_importUserData()` | Field import progress |
| 39 | `value()` | Value extraction |
| 40-41 | `value_at()` | Array access |
| 42 | `findChildValue()` | Error logging |
| 43 | `debug::JSONDocumentUtils()` | Debug activation |

## Performance Considerations

1. **Caching** - Lazy serialization reduces redundant JSON generation
2. **Move Semantics** - Used throughout for efficiency
3. **Reference Returns** - Avoids copies where possible
4. **Visitor Pattern** - Efficient type-safe variant access

## See Also

- [JSONDocument.h.md](./JSONDocument.h.md) - Class declaration
- [JSONDocument.cpp.md](./JSONDocument.cpp.md) - Tree manipulation methods
- [JSONDocumentBuilder.h.md](./JSONDocumentBuilder.h.md) - Builder class declaration
- [JSONDocumentBuilder.cpp.md](./JSONDocumentBuilder.cpp.md) - Builder implementation
- [common.h.md](./common.h.md) - Common includes and types
- [docrecord_exceptions.h.md](./docrecord_exceptions.h.md) - Exception types used
