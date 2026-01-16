# ovlComment.cpp

**Path:** `artdaq-database/Overlay/ovlComment.cpp`

**Implements:** [ovlComment.h](./ovlComment.h.md)

**Purpose:** Provides the implementation for the `ovlComment` overlay class, including accessor methods for line number and text, JSON serialization, and direct comparison for configuration comments with line number tracking. The implementation delegates field access to the base class `value_as<T>()` template method for type-safe JSON field retrieval.

## Implementation Overview

The implementation is straightforward, delegating field access to the base class `value_as<T>()` template method. The class uses JSON literal constants (`jsonliteral::linenum` and `jsonliteral::value`) for field names to ensure consistency across the codebase. This design ensures that field names are defined in a single location and any changes propagate automatically.

## Key Algorithms

### Field Access Pattern

All accessor methods follow the same pattern:

**Steps:**
1. Call base class `value_as<T>()` with the appropriate JSON literal constant
2. Return the reference (mutable or const depending on method signature)

**Why this approach:** Centralizes type-safe field access in the base class, reducing code duplication and ensuring consistent behavior.

### Serialization

The `to_string()` method produces a JSON-formatted string representation:

**Steps:**
1. Open JSON object with `{`
2. Output quoted `linenum` key with integer value (no quotes around number)
3. Output quoted `value` key with quoted string value
4. Close JSON object with `}`

**Why this approach:** Provides human-readable output for debugging while maintaining valid JSON syntax that can be parsed by standard JSON tools.

### Comparison Logic

The `operator==` uses the simplest possible comparison:

**Steps:**
1. Call `self()` on both objects to get underlying JSON values
2. Compare the JSON values directly
3. Return result wrapped in `result_t`

**Why this approach:** Comments need exact matching; masking is handled at the list level.

## Internal Functions

### Constructor

```cpp
ovlComment::ovlComment(object_t::key_type const& key, value_t& comment)
    : ovlKeyValue(key, comment) {}
```

**Brief:** Simple pass-through to base class constructor; no additional initialization required. The base class stores the reference to the JSON value.

**Called by:** Factory functions, direct construction, list containers when iterating

**Purpose:** Establishes the overlay relationship between the C++ object and the underlying JSON.

### Mutable Accessor Implementations

```cpp
integer& ovlComment::linenum() {
  return value_as<integer>(jsonliteral::linenum);
}

std::string& ovlComment::text() {
  return value_as<std::string>(jsonliteral::value);
}
```

**Brief:** Delegates to base class `value_as<T>()` for type-safe field access. Returns mutable references allowing modification of the underlying JSON.

**Called by:** All code that needs to read or modify comment properties

**Purpose:** Provides type-safe access to comment fields with automatic type checking.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If the field exists but has wrong type |
| `std::out_of_range` | If the field does not exist |

### Const Accessor Implementations

```cpp
integer const& ovlComment::linenum() const {
  return value_as<integer>(jsonliteral::linenum);
}

std::string const& ovlComment::text() const {
  return value_as<std::string>(jsonliteral::value);
}
```

**Brief:** Const versions of accessors for read-only access. These are called when the `ovlComment` object is const.

**Called by:** Code that has const reference to comment, comparison operators

**Purpose:** Enables read-only access without risk of accidental modification.

### Serialization Implementation

```cpp
std::string ovlComment::to_string() const {
  std::ostringstream oss;
  oss << "{" << quoted_(jsonliteral::linenum) << ":" << linenum() << ",";
  oss << quoted_(jsonliteral::value) << ":" << quoted_(text()) << "}";
  return oss.str();
}
```

**Brief:** Produces JSON representation for debugging and logging. Uses the `quoted_()` helper to properly escape strings.

**Output format:** `{"linenum": 42, "value": "Configuration comment"}`

**Called by:** Debugging code, logging, comparison error messages

**Purpose:** Provides human-readable representation of comment state.

### Comparison Implementation

```cpp
result_t ovlComment::operator==(ovlComment const& other) const {
  return self() == other.self();
}
```

**Brief:** Performs direct comparison of underlying JSON values without masking. This is the simplest form of comparison.

**Called by:** `ovlComments` list comparison, test code

**Purpose:** Comments are compared directly at the individual level; masking is applied at the `ovlComments` list level through the `DOCUMENT_COMPARE_MUTE_COMMENTS` flag.

**Returns:** `result_t` where `first` is `true` if equal, `false` otherwise; `second` contains difference details.

## Performance Considerations

- **Accessor Methods:** O(1) - direct field lookup in JSON object using hash map
- **Serialization:** O(n) where n is length of comment text; string concatenation dominates
- **Comparison:** O(n) where n is complexity of JSON comparison (includes string comparison for text field)
- **Construction:** O(1) - only stores reference, no copying

## Error Handling Strategy

The implementation relies on:
- Base class `value_as<T>()` for type checking and field existence validation
- No explicit exception handling in this file - exceptions propagate from base class
- Assumes fields exist (no null checks) - caller must ensure valid JSON structure

If fields are missing or have wrong types, exceptions will be thrown by `value_as<T>()`. Callers should use try-catch blocks when working with comments that may have invalid structure.

## Testing Notes

- **Unit tests:** Part of Overlay module tests in `test/Overlay/`
- **Key test cases:**
  - Create comment with valid JSON containing both fields
  - Read line number and text via const and non-const accessors
  - Modify line number and text via mutable accessors
  - Serialize to string and verify JSON format
  - Compare equal comments (same linenum and text)
  - Compare unequal comments (different linenum or text)
  - Verify exceptions when fields are missing

## Maintenance Notes

### Field Name Constants

Uses `jsonliteral::linenum` and `jsonliteral::value` from the shared literals header. If field names need to change:
1. Update the literal constants in `artdaq-database/DataFormats/shared_literals.h`
2. No changes needed to this file
3. All overlays using these literals will automatically use the new names

### Minimal Implementation

This is intentionally a minimal implementation with no validation:
- No validation of line numbers (can be negative, zero, or any integer)
- No validation of text content (can be empty or contain any characters)
- No auto-initialization of missing fields
- No default values

This keeps the overlay simple and fast, with validation responsibility at higher levels (e.g., when storing documents).

### JSON Literal Dependencies

| Literal | Expected Value | Used For |
|---------|----------------|----------|
| `jsonliteral::linenum` | `"linenum"` | Line number field key |
| `jsonliteral::value` | `"value"` | Comment text field key |

### Helper Function Dependencies

| Function | Purpose |
|----------|---------|
| `quoted_(str)` | Wraps string in quotes for JSON output |
| `value_as<T>(key)` | Type-safe JSON field access from base class |
| `self()` | Returns underlying JSON value for comparison |

## See Also

- [ovlComment.h](./ovlComment.h.md) - Class declaration with public interface
- [ovlKeyValue.h](./ovlKeyValue.h.md) - Base class providing `value_as<T>()` template
- [ovlKeyValue.cpp](./ovlKeyValue.cpp.md) - Base class implementation
- [common.h](./common.h.md) - JSON literal constants and comparison flags
- [shared_literals.h](../DataFormats/shared_literals.h.md) - JSON field name definitions
