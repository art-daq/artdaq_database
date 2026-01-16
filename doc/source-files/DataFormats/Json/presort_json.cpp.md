# presort_json.cpp

**Path:** `artdaq-database/DataFormats/Json/presort_json.cpp`

**Implements:** [presort_json.h](./presort_json.h.md)

**Purpose:** Implementation of the GUI JSON pre-sorting utility that sorts "search" array entries alphabetically by their "name" field. Uses the internal JSON AST for efficient parsing, manipulation, and serialization with comprehensive error handling.

## Implementation Overview

The implementation follows a conservative, fail-safe approach:
1. Parse input JSON string to internal AST representation
2. Locate the "search" array within the parsed object
3. Sort array elements using a custom comparator that extracts "name" fields
4. Serialize the modified AST back to JSON string
5. Return original input on any failure (parsing, missing keys, serialization)

This design ensures the function never throws exceptions and always returns valid output.

## Key Algorithms

### JSON Pre-sort Algorithm

**Steps:**
1. **Parse JSON**: Convert input string to `object_t` AST using `JsonReader`
2. **Find Search Array**: Look up "search" key in the root object
3. **Validate Array**: Verify the value is an `array_t` type
4. **Extract Names**: For each array element, extract "name" field value
5. **Sort Elements**: Use `std::list::sort()` with custom comparator
6. **Serialize Result**: Convert modified AST back to string using `JsonWriter`

**Why this approach:**
- Uses existing JSON reader/writer infrastructure
- AST manipulation is type-safe through Boost.Variant
- `std::list::sort()` is stable and efficient for linked lists
- Graceful degradation on any error preserves input

### Name Extraction Algorithm

**Steps:**
1. Attempt to cast element to `object_t`
2. Search for "name" key in the object
3. Attempt to cast value to `std::string`
4. Return empty string on any failure

**Why this approach:**
- Defensive coding handles malformed data
- Empty string sort order puts unnamed elements first
- Exception handling prevents crashes on type mismatches

## Thread Safety

- **Thread-safe:** Yes
- **Concurrent access:** Safe for concurrent calls from multiple threads
- **Locking:** None required
- **Reentrancy:** Fully reentrant

All data structures are local to the function. The internal helpers in the anonymous namespace are also thread-safe as they operate only on their parameters.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/Json/presort_json.h` | Function declaration |
| `artdaq-database/DataFormats/Json/json_reader.h` | `JsonReader` for JSON parsing |
| `artdaq-database/DataFormats/Json/json_writer.h` | `JsonWriter` for JSON serialization |
| `<algorithm>` | Standard algorithm utilities |
| `<string>` | `std::string` type |
| `<string_view>` | `std::string_view` for efficient parameter passing |

## Internal Functions

### `extract_name_from_ast(object_t const& obj) -> std::string`

**Brief:** Extracts the "name" field value from a JSON object AST node, returning empty string if not found or not a string type.

**Namespace:** Anonymous (internal linkage)

**Called by:** `NameComparator::operator()`

**Purpose:** Provides safe extraction of the "name" field with graceful handling of missing or wrong-typed values.

**Parameters:**
- `obj` (`object_t const&`) - JSON object AST node to extract from

**Returns:** The string value of the "name" field, or empty string if:
- "name" key does not exist
- "name" value is not a string type

**Thread Safety:** Thread-safe (operates only on parameters)

**Implementation:**
```cpp
[[nodiscard]] std::string extract_name_from_ast(object_t const& obj) {
  auto it = obj.find("name");
  if (it == obj.end()) {
    return "";
  }

  try {
    return boost::get<std::string>(it->value);
  } catch (boost::bad_get const&) {
    return "";
  }
}
```

### `struct NameComparator`

**Brief:** Functor for comparing JSON array elements by their "name" field, enabling lexicographic sorting.

**Namespace:** Anonymous (internal linkage)

**Called by:** `presort_gui_json_by_name()` via `search_array->values.sort()`

**Purpose:** Provides a comparison function object for `std::list::sort()` that compares array elements based on their "name" field values.

**Thread Safety:** Thread-safe (stateless functor)

**Implementation:**
```cpp
struct NameComparator {
  bool operator()(value_t const& a, value_t const& b) const {
    std::string name_a, name_b;

    try {
      name_a = extract_name_from_ast(boost::get<object_t>(a));
    } catch (boost::bad_get const&) {
      name_a = "";
    }

    try {
      name_b = extract_name_from_ast(boost::get<object_t>(b));
    } catch (boost::bad_get const&) {
      name_b = "";
    }

    return name_a < name_b;
  }
};
```

**Behavior:**
- Elements that are not objects compare as empty string
- Elements without "name" field compare as empty string
- Comparison is lexicographic (standard string `<` operator)
- Empty strings sort before non-empty strings

## Error Handling Strategy

The implementation uses a defensive strategy where any error results in returning the original input unchanged:

| Error Condition | Detection | Behavior |
|-----------------|-----------|----------|
| Parse failure | `JsonReader::read()` returns false | Return original JSON |
| No "search" key | `ast.find()` returns `end()` | Return original JSON |
| "search" not array | `boost::get<array_t>` throws `bad_get` | Return original JSON |
| Empty array | `search_array->empty()` | Return original JSON |
| Serialization failure | `JsonWriter::write()` returns false | Return original JSON |
| Elements not objects | `boost::get<object_t>` throws | Treat as empty name |
| Missing "name" field | `obj.find()` returns `end()` | Treat as empty name |

This approach ensures:
- No exceptions propagate to callers
- Output is always valid if input was valid
- Worst case is a no-op (original returned unchanged)

## Performance Considerations

- **Parsing overhead:** Full JSON parse required even for small arrays
- **Memory:** Creates copy of entire AST in memory
- **Sort complexity:** O(n log n) for n elements in search array
- **Serialization:** Full re-serialization of entire document
- **String copies:** Multiple string copies during processing

For large documents with small search arrays, the overhead may be significant relative to actual sorting work.

**Optimization opportunities:**
- Consider streaming approach for very large documents
- Cache parsed AST if sorting multiple times
- Use string_view where possible to reduce copies

## Testing Notes

- **Unit tests:** Should test with various JSON structures
- **Key test cases:**
  - Empty input
  - No "search" key
  - Empty "search" array
  - Array with single element
  - Array with multiple elements (verify sort order)
  - Elements without "name" field
  - Elements with non-string "name" field
  - Malformed JSON input
  - Unicode in name values

## Maintenance Notes

- **Anonymous Namespace:** Helper functions are in anonymous namespace for internal linkage; they should not be exposed
- **Exception Safety:** Uses try/catch for `boost::get` to handle type mismatches gracefully
- **List Sort:** Uses `values.sort()` directly since `array_t` uses `std::list` internally; do not change to `std::sort` which requires random access iterators
- **No TRACE:** This utility doesn't use TRACE logging; consider adding for debugging complex issues
- **String View Conversion:** Converts `string_view` to `string` for `JsonReader` compatibility; JsonReader may be updated to accept string_view directly

## Relationship to Other Components

```
presort_json.cpp
    |
    +-- Uses json_reader.h (JsonReader class)
    |       +-- read() method parses JSON to AST
    |
    +-- Uses json_writer.h (JsonWriter class)
    |       +-- write() method serializes AST to string
    |
    +-- Uses json_types.h (implicitly via json_reader/writer)
            +-- object_t, array_t, value_t types
            +-- object_t::find() method
            +-- array_t::values (std::list container)
```

## Common Pitfalls

- **Assuming in-place modification:** Function returns new string; original is not modified
- **Expecting exceptions:** Function never throws; check return value against input if error detection needed
- **Large documents:** Full parse/serialize for every call; consider caching for repeated operations

## See Also

- [presort_json.h](./presort_json.h.md) - Public interface and usage documentation
- [json_reader.h](./json_reader.h.md) - JSON parsing implementation
- [json_reader.cpp](./json_reader.cpp.md) - JsonReader implementation details
- [json_writer.h](./json_writer.h.md) - JSON serialization implementation
- [json_writer.cpp](./json_writer.cpp.md) - JsonWriter implementation details
- [json_types.h](./json_types.h.md) - AST type definitions (object_t, array_t, value_t)
- [External: Boost.Variant](https://www.boost.org/doc/libs/release/doc/html/variant.html) - boost::get and boost::bad_get documentation

---

**Documentation generated for artdaq-database DataFormats/Json module**
