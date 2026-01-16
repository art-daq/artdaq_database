# presort_json.h

**Path:** `artdaq-database/DataFormats/Json/presort_json.h`

**Purpose:** Provides a utility function to sort entries in GUI JSON documents by their "name" field. This preprocessing step ensures consistent ordering of configuration elements before display or comparison operations, improving user experience and enabling reliable diff operations.


## Key Concepts

### GUI JSON Pre-sorting
The GUI interface uses JSON documents containing arrays of configuration entries. Pre-sorting these entries by name provides:
- **Consistent Display**: Users see entries in alphabetical order regardless of insertion order
- **Reliable Comparisons**: Sorted data enables meaningful diff operations between versions
- **Improved Usability**: Easier navigation through large configuration sets
- **Deterministic Output**: Same input always produces same output ordering

### Sort Target
The function specifically targets the "search" array within GUI JSON documents. This array contains configuration entries that are displayed to users in the web interface.

**Before sorting:**
```json
{
    "search": [
        {"name": "zebra", "value": 1},
        {"name": "alpha", "value": 2},
        {"name": "beta", "value": 3}
    ]
}
```

**After sorting:**
```json
{
    "search": [
        {"name": "alpha", "value": 2},
        {"name": "beta", "value": 3},
        {"name": "zebra", "value": 1}
    ]
}
```

### Safe Operation Philosophy
The function is designed to never fail catastrophically. Any error condition results in returning the original JSON string unchanged, ensuring that the calling code always receives valid JSON output.

## Thread Safety

- **Thread-safe:** Yes
- **Concurrent access:** Multiple threads can safely call this function concurrently with different inputs
- **Locking:** None required - function operates on local copies
- **Reentrancy:** Fully reentrant

The function creates local copies of all data structures and does not modify global state, making it safe for concurrent use.

## Dependencies

| Include | Purpose |
|---------|---------|
| `<string>` | `std::string` return type |
| `<string_view>` | `std::string_view` for efficient input parameter passing |

## Functions

### `presort_gui_json_by_name(std::string_view json) -> std::string`

**Brief:** Sorts the "search" array entries in a GUI JSON document alphabetically by their "name" field, returning a new JSON string with sorted entries.

**Namespace:** `artdaq::database::json`

**Parameters:**
- `json` (`std::string_view`) - Input JSON string containing a "search" array. The JSON must be well-formed.

**Preconditions:**
- Input should be valid JSON (function handles invalid input gracefully by returning original)

**Returns:** The JSON string with the "search" array sorted by "name" field. Returns the original input string unchanged if:
- JSON parsing fails
- No "search" key exists
- "search" is not an array
- "search" array is empty
- JSON serialization fails

**Postconditions:**
- If successful, the returned JSON has the "search" array sorted alphabetically by "name"
- All other JSON content is preserved unchanged
- Output is always valid JSON if input was valid JSON

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Function never throws; returns original on any error |

**Thread Safety:** Thread-safe. Creates local copies and does not modify global state.

**Side Effects:** None. Creates a new string without modifying the input.

**Complexity:** O(n log n) where n is the number of elements in the "search" array

**Attributes:**
- `[[nodiscard]]` - Compiler warns if return value is ignored

**Example:**
```cpp
#include "artdaq-database/DataFormats/Json/presort_json.h"
#include <iostream>

void sortAndDisplay() {
  std::string gui_json = R"({
    "search": [
      {"name": "zebra", "value": 1},
      {"name": "alpha", "value": 2},
      {"name": "beta", "value": 3}
    ],
    "other_data": "preserved"
  })";

  // Sort the search array by name
  std::string sorted = artdaq::database::json::presort_gui_json_by_name(gui_json);

  // Result: entries ordered as alpha, beta, zebra
  // "other_data" field is preserved unchanged
  std::cout << "Sorted JSON:\n" << sorted << "\n";
}

// Handling edge cases - function always returns valid JSON
void handleEdgeCases() {
  // Missing "search" key - returns original unchanged
  std::string no_search = R"({"data": "value"})";
  auto result1 = artdaq::database::json::presort_gui_json_by_name(no_search);
  // result1 == no_search

  // Empty array - returns original unchanged
  std::string empty_array = R"({"search": []})";
  auto result2 = artdaq::database::json::presort_gui_json_by_name(empty_array);
  // result2 == empty_array

  // Invalid JSON - returns original unchanged
  std::string invalid = "not valid json";
  auto result3 = artdaq::database::json::presort_gui_json_by_name(invalid);
  // result3 == invalid
}
```

## Relationship to Other Components

```
GUI JSON Document (from web interface)
    |
    v
presort_gui_json_by_name()
    |
    +-- JsonReader::read() --> json::object_t (AST)
    +-- Find "search" array
    +-- Sort by "name" field using NameComparator
    +-- JsonWriter::write() --> Sorted JSON string
    |
    v
Sorted GUI JSON (for display/comparison)
```

### Integration Points
- **convertjson2guijson.cpp**: Uses this for GUI display preparation
- **Configuration comparison**: Ensures deterministic ordering for diffs
- **Web GUI**: Displays sorted entries to users

## Common Pitfalls

- **Ignoring return value:** The `[[nodiscard]]` attribute helps catch this, but remember the function returns a new string; it does not modify in place
- **Assuming modification:** The input is not modified; always use the returned value
- **Missing name fields:** Entries without a "name" field sort as empty string (come first)

### Anti-patterns

```cpp
// DON'T do this - ignoring return value:
std::string json = getGuiJson();
presort_gui_json_by_name(json);  // Warning! Return value ignored
displayJson(json);  // Still unsorted!

// DO this instead:
std::string json = getGuiJson();
std::string sorted = presort_gui_json_by_name(json);
displayJson(sorted);  // Correctly sorted

// Or modify in place:
std::string json = getGuiJson();
json = presort_gui_json_by_name(json);  // Assign result back
displayJson(json);
```

## Notes for Developers

- **Safe Operation**: Never throws; returns original on any error
- **Performance**: Uses `std::list::sort()` for efficient in-place sorting of the internal list representation
- **String View**: Uses `std::string_view` for efficient input without copying until needed
- **Namespace**: Function is in `artdaq::database::json` namespace
- **No Side Effects**: Creates new string rather than modifying input
- **Lexicographic Sorting**: Uses standard string comparison (`<` operator) for sorting

## See Also

- [presort_json.cpp](./presort_json.cpp.md) - Implementation details
- [json_reader.h](./json_reader.h.md) - JSON parsing used internally
- [json_writer.h](./json_writer.h.md) - JSON serialization used internally
- [convertjson2guijson.h](./convertjson2guijson.h.md) - Primary consumer of this functionality
- [json_types.h](./json_types.h.md) - AST types used internally (object_t, array_t, value_t)

---

**Documentation generated for artdaq-database DataFormats/Json module**
