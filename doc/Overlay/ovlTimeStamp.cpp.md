# ovlTimeStamp.cpp

**Path:** `artdaq-database/Overlay/ovlTimeStamp.cpp`

**Implements:** [ovlTimeStamp.h](./ovlTimeStamp.h.md)

**Purpose:** Implements the `ovlTimeStamp` class, providing ISO8601 timestamp validation during construction, accessor methods for reading and writing timestamp values, serialization to JSON format, and maskable comparison for temporal fields.

## Implementation Overview

This file implements the timestamp overlay that wraps an ISO8601 timestamp string value. The key implementation detail is construction-time validation using the comma operator idiom to validate the timestamp before passing it to the base class constructor. The comparison operator respects the global timestamp mask for flexible testing.

## Key Algorithms

### Construction-Time Validation

**Steps:**
1. Unwrap the JSON value to get the underlying type using `unwrap()`
2. Extract the string value from the JSON using `value_as<std::string>()`
3. Validate the string as ISO8601 format via `confirm_iso8601_timestamp()`
4. Use comma operator to pass the validated reference to base class

**Why this approach:** The comma operator allows both validation and reference passing in a single initializer expression, ensuring validation occurs before the base class is initialized.

### Maskable Comparison

**Steps:**
1. Check if `DOCUMENT_COMPARE_MUTE_TIMESTAMPS` is set in the global compare mask using `useCompareMask()`
2. If masked, return `Success()` immediately without comparing
3. If not masked, delegate to base class `self() == other.self()` comparison

**Why this approach:** Allows flexible comparison behavior where timestamps can be ignored during testing or when comparing records that only differ in timing.

## Internal Functions

### Constructor with Validation

```cpp
ovlTimeStamp::ovlTimeStamp(object_t::key_type const& key, value_t& ts)
    : ovlKeyValue(key, (ts = confirm_iso8601_timestamp(unwrap(ts).value_as<std::string>()), ts)) {}
```

**Brief:** Constructs the timestamp overlay by validating the ISO8601 format before initializing the base class.

**Called by:** Client code creating timestamp overlays

**Purpose:** Ensures all timestamp overlays contain valid ISO8601 strings.

**Implementation Detail:** The expression `(ts = confirm_iso8601_timestamp(...), ts)` uses the comma operator to:
1. First, call `confirm_iso8601_timestamp()` which validates and returns the string
2. Assign the validated string back to `ts` (which updates the underlying JSON)
3. Return `ts` as the result of the comma expression
4. Pass `ts` to the base class constructor

### Accessor Methods

```cpp
std::string& ovlTimeStamp::timestamp() {
  return string_value();
}

std::string const& ovlTimeStamp::timestamp() const {
  return string_value();
}

std::string& ovlTimeStamp::timestamp(std::string const& ts) {
  confirm(!ts.empty());
  timestamp() = ts;
  return timestamp();
}
```

**Brief:** Provide access to the timestamp string, with the setter validating non-empty input.

**Called by:** Client code reading or updating timestamps

**Purpose:**
- Read access: Simple delegation to base class `string_value()`
- Write access: Validates non-empty before assignment

### Serialization

```cpp
std::string ovlTimeStamp::to_string() const {
  std::ostringstream oss;
  oss << "{" << quoted_(key()) << ":" << quoted_(timestamp()) << "}";
  return oss.str();
}
```

**Brief:** Produces a JSON representation of the timestamp with its key.

**Called by:** Debugging and logging code, parent overlay serialization

**Purpose:** Creates human-readable JSON output like `{"created": "2025-01-15T10:30:00Z"}`

### Comparison Operator

```cpp
result_t ovlTimeStamp::operator==(ovlTimeStamp const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_TIMESTAMPS) == DOCUMENT_COMPARE_MUTE_TIMESTAMPS)
         ? Success()
         : self() == other.self();
}
```

**Brief:** Compares timestamps, with optional masking that bypasses the actual comparison.

**Called by:** Parent overlay comparison operators

**Purpose:** Enables flexible comparison where timestamp differences can be ignored when the mask is set.

## Performance Considerations

- **Minimal overhead:** Simple string reference storage and delegation
- **Validation cost:** ISO8601 validation occurs only during construction
- **No dynamic allocation:** Uses references to existing JSON storage

## Error Handling Strategy

- Uses `confirm()` macro for precondition validation
- Uses `confirm_iso8601_timestamp()` for format validation
- Returns `result_t` pairs with success/failure status

## Testing Notes

- **Unit tests:** Located in `test/Overlay/` directory
- **Key test cases:**
  - Construction with valid ISO8601 timestamps
  - Construction with invalid timestamps (should fail)
  - Comparison with and without masking
  - Setter with empty string (should fail)

## Maintenance Notes

### Comma Operator Idiom

The constructor uses a C++ idiom where the comma operator allows multiple operations in a single expression:

```cpp
(ts = confirm_iso8601_timestamp(...), ts)
```

This is equivalent to:
```cpp
ts = confirm_iso8601_timestamp(...);
return ts;  // The comma operator returns the right-hand operand
```

This pattern is used because C++ requires constructor initializers to be single expressions.

### Validation Asymmetry

Note that validation behavior differs between construction and setting:
- **Construction:** Full ISO8601 format validation
- **Setter:** Only checks for non-empty

This is intentional because:
1. Construction creates a new overlay from potentially untrusted data
2. Setting typically involves programmatically generated timestamps that are already valid
3. Direct reference access via `timestamp()` is a deliberate bypass for performance

### Helper Functions Used

- `unwrap()` - Unwraps JSON value to access underlying typed value
- `confirm_iso8601_timestamp()` - Validates and returns ISO8601 timestamp string
- `quoted_()` - Wraps a string in double quotes for JSON output
- `string_value()` - Base class method returning reference to string value

## See Also

- [ovlTimeStamp.h](./ovlTimeStamp.h.md) - Class declaration
- [ovlKeyValue.h](./ovlKeyValue.h.md) - Base class providing `string_value()`
- [common.h](./common.h.md) - `confirm_iso8601_timestamp()` and mask constants
- [helper_functions.h](../SharedCommon/helper_functions.h.md) - `timestamp()` function for generating current time
