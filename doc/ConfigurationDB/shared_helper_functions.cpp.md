# shared_helper_functions.cpp

**Path:** `artdaq-database/ConfigurationDB/shared_helper_functions.cpp`

**Implements:** [shared_helper_functions.h](./shared_helper_functions.h.md)

**Purpose:** Implementation of the data format conversion functions declared in `shared_helper_functions.h`. This file provides the bidirectional conversion between `data_format_t` enum values and their string representations.

## Implementation Overview

This implementation file provides two simple conversion functions:

1. `to_string()` - Converts enum to string using a switch statement
2. `to_data_format()` - Converts string to enum using if-else chain

Both functions are designed to be efficient and thread-safe through stateless operation.

## Dependencies

| Include | Purpose |
|---------|---------|
| `<libgen.h>` | POSIX basename/dirname utilities (unused in current implementation) |
| `<boost/filesystem.hpp>` | Filesystem operations (available for future extensions) |
| `<boost/range/iterator_range.hpp>` | Range utilities (available for future extensions) |
| `artdaq-database/ConfigurationDB/shared_helper_functions.h` | Header declarations |
| `artdaq-database/SharedCommon/sharedcommon_common.h` | Common utilities including `confirm()` macro |

## Namespace Aliases

```cpp
namespace cf = artdaq::database::configuration;
using artdaq::database::configuration::options::data_format_t;
```

## Internal Functions

### `cf::to_string(data_format_t const& f) -> std::string`

**Brief:** Converts a `data_format_t` enum value to its lowercase string representation.

**Implementation:**
```cpp
std::string cf::to_string(data_format_t const& f) {
  switch (f) {
    default:
    case data_format_t::unknown:
      return "unknown";
    case data_format_t::fhicl:
      return "fhicl";
    case data_format_t::json:
      return "json";
    case data_format_t::gui:
      return "gui";
    case data_format_t::db:
      return "db";
    case data_format_t::xml:
      return "xml";
    case data_format_t::origin:
      return "origin";
    case data_format_t::csv:
      return "csv";
  }
}
```

**Called by:** Any code needing to display or serialize a format value

**Design Notes:**
- Uses `default` case falling through to `unknown` for safety
- Returns static string literals (no allocation)
- O(1) complexity via switch statement

---

### `cf::to_data_format(std::string const& f) -> data_format_t`

**Brief:** Converts a string representation to a `data_format_t` enum value.

**Implementation:**
```cpp
data_format_t cf::to_data_format(std::string const& f) {
  confirm(!f.empty());

  if (f == "fhicl") {
    return data_format_t::fhicl;
  } else if (f == "json") {
    return data_format_t::json;
  } else if (f == "gui") {
    return data_format_t::gui;
  } else if (f == "db") {
    return data_format_t::db;
  } else if (f == "xml") {
    return data_format_t::xml;
  } else if (f == "origin") {
    return data_format_t::origin;
  } else if (f == "csv") {
    return data_format_t::csv;
  }

  return data_format_t::unknown;
}
```

**Called by:** Command-line parsers, configuration readers

**Design Notes:**
- Uses `confirm(!f.empty())` for debug-mode assertion on empty input
- Case-sensitive string comparison (lowercase expected)
- Returns `unknown` for unrecognized strings (safe default)
- O(n) complexity where n is number of formats (currently 7 comparisons worst case)

## Key Algorithms

### String-to-Enum Conversion

The conversion uses a simple if-else chain rather than a map for several reasons:

1. **Small Set**: Only 7 format types - map overhead not justified
2. **Static Values**: No need for dynamic lookup
3. **No Allocation**: String literals compared directly
4. **Predictable**: First match wins, common formats checked first

**Why not std::map?**
```cpp
// Alternative approach (NOT used - more overhead for small set)
static const std::map<std::string, data_format_t> format_map = {
  {"json", data_format_t::json},
  {"fhicl", data_format_t::fhicl},
  // ...
};
auto it = format_map.find(f);
return (it != format_map.end()) ? it->second : data_format_t::unknown;
```

The if-else chain is simpler and equally efficient for this small set.

### Enum-to-String Conversion

Uses a switch statement which compiles to a jump table for efficient O(1) lookup.

## Performance Considerations

### Memory
- No dynamic memory allocation
- Returns string literals (static storage)
- No caching required

### Thread Safety
- Both functions are pure functions with no shared state
- Safe to call from any thread without synchronization

### Optimization
- `to_string()` compiles to an efficient jump table
- `to_data_format()` short-circuits on first match
- Most common formats (json, fhicl) checked early in the chain

## Error Handling Strategy

### Empty String Input
```cpp
confirm(!f.empty());  // Debug assertion only
```

In debug builds, an empty string triggers an assertion failure. In release builds, the function continues and returns `unknown`.

### Invalid Format String
Returns `data_format_t::unknown` rather than throwing - callers must check for this value.

**Design Rationale:** This allows callers to provide appropriate error messages with context, rather than catching generic exceptions.

## Testing Notes

- **Unit tests:** `test/ConfigurationDB/shared_helper_functions_t.cc` (if exists)
- **Key test cases:**
  - Roundtrip: `to_string(to_data_format("json"))` should equal `"json"`
  - Unknown handling: `to_data_format("invalid")` should return `unknown`
  - Case sensitivity: `to_data_format("JSON")` should return `unknown`
  - Empty string: Should trigger assertion in debug mode

## Maintenance Notes

### Adding a New Format

When adding a new data format:

1. Add enum value to `data_format_t` in `shared_helper_functions.h`
2. Add case to `to_string()` switch statement (this file)
3. Add if-else branch to `to_data_format()` (this file)
4. Consider placement in if-else chain (common formats first)

**Example: Adding "yaml" format**
```cpp
// In to_string():
case data_format_t::yaml:
  return "yaml";

// In to_data_format():
} else if (f == "yaml") {
  return data_format_t::yaml;
}
```

### String Convention

All format strings must be:
- Lowercase only
- Single word (no spaces or special characters)
- Unique across all formats

## See Also

- [shared_helper_functions.h](./shared_helper_functions.h.md) - Header with declarations and enum definition
- [options_operation_base.h](./options_operation_base.h.md) - Uses these functions for format parsing
- [sharedcommon_common.h](../SharedCommon/sharedcommon_common.h.md) - Provides the `confirm()` macro

---

**Documentation generated for artdaq-database ConfigurationDB module**
