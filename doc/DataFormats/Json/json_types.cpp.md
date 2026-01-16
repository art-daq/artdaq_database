# json_types.cpp

**Path:** `artdaq-database/DataFormats/Json/json_types.cpp`

**Implements:** [json_types.h](./json_types.h.md)

**Purpose:** Implements the comparison operators for JSON types, template specializations for unwrapper functions, and type conversion utilities. This file provides sophisticated deep comparison logic that returns detailed error messages when objects differ, making debugging and testing significantly easier.

## Implementation Overview

The implementation provides:

1. **Unwrapper specializations:** Template specializations enabling type-safe value extraction from JSON variants
2. **Comparison operators:** Deep recursive comparison for all JSON types with detailed error reporting
3. **Type utilities:** Helper function for type enumeration to string conversion

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/Json/json_types.h` | Type declarations |
| `artdaq-database/DataFormats/common.h` | Common infrastructure |

## TRACE Configuration

```cpp
#define TRACE_NAME "json_types.cpp"
```

## Key Algorithms

### Deep Comparison Strategy

All comparison operators follow a consistent pattern:
1. Check structural compatibility (types match, sizes match)
2. If incompatible, return `{false, "descriptive error"}`
3. Recursively compare nested elements
4. Return `{true, "Success"}` only if all checks pass

This approach provides detailed diagnostics when comparisons fail, specifying exactly where and why the difference was found.

---

### compare_visitor

**Brief:** A Boost static visitor that performs type-aware comparison of JSON values.

```cpp
struct compare_visitor : public boost::static_visitor<bool>
```

**Overloads:**

| Types | Behavior |
|-------|----------|
| `object_t, object_t` | Delegates to `object_t::operator==` |
| `array_t, array_t` | Delegates to `array_t::operator==` |
| `std::string, std::string` | Direct string comparison |
| `decimal, decimal` | Direct numeric comparison |
| `integer, integer` | Direct numeric comparison |
| `bool, bool` | Direct boolean comparison |
| `A1, A2` (different types) | Returns `false` |

**Template catch-all:**
```cpp
template <typename A1, typename A2>
bool operator()(A1 const&, A2 const&) const {
  return false;  // Different types are never equal
}
```

---

### Value Comparison (operator==)

**Brief:** Compares two `value_t` variants for equality with type checking.

**Algorithm:**
1. Compare variant type indices using `which()`
2. If indices differ, return false with type mismatch message
3. Apply `compare_visitor` to compare actual values
4. Return result with appropriate success/error message

**Error Message Examples:**
- Type mismatch: `"Values have different types <std::string(hello), integer(42)>"`
- Value mismatch: `"Values are different <std::string(hello), std::string(world)>"`

---

### Data Comparison (operator==)

**Brief:** Compares two `data_t` key-value pairs for equality.

**Algorithm:**
1. Compare keys using standard string comparison
2. If keys differ, return false with key mismatch message
3. Compare values using `compare_visitor`
4. Return result with key context in error messages

**Error Message Examples:**
- Key mismatch: `"Keys are different <config, settings>"`
- Value mismatch: `"Values are different <config:std::string(v1), config:std::string(v2)>"`

---

### Array Comparison (operator==)

**Brief:** Compares two `array_t` arrays for element-wise equality.

**Algorithm:**
1. Compare array sizes
2. If sizes differ, return false with size mismatch message
3. Iterate through arrays in parallel
4. Compare each element pair using `compare_visitor`
5. Return on first mismatch or success after all elements

**Error Message Examples:**
- Size mismatch: `"Arrays have different sizes <array(...), array(...)>"`
- Element mismatch: `"Arrays are different <array(...), array(...)>"`

---

### Object Comparison (operator==)

**Brief:** Compares two `object_t` objects for pair-wise equality.

**Algorithm:**
1. Compare object sizes (number of key-value pairs)
2. If sizes differ, return false with size mismatch message
3. Iterate through both objects in parallel
4. Compare each `data_t` pair using `data_t::operator==`
5. Return on first mismatch with key context

**Error Message Examples:**
- Size mismatch: `"Objects have different sizes <object(...), object(...)>"`
- Pair mismatch: `"Ojbects are different at key=\"threshold\" Values are different..."`

**Note:** There is a typo in the error message ("Ojbects" instead of "Objects") which is preserved for compatibility.

## Unwrapper Template Specializations

### Constructor Specializations

```cpp
template<>
unwrapper<value_t>::unwrapper(value_t& a) : any(a) {}

template<>
unwrapper<const value_t>::unwrapper(const value_t& a) : any(a) {}

template<>
unwrapper<object_t>::unwrapper(object_t& a) : any(a) {}

template<>
unwrapper<const object_t>::unwrapper(const object_t& a) : any(a) {}
```

**Brief:** Provides specialized constructors for both const and non-const JSON types.

---

### Value Extraction Specializations

```cpp
template<>
template<>
object_t& unwrapper<object_t&>::value_as() {
  return boost::get<object_t&>(any);
}

template<>
template<>
array_t& unwrapper<array_t&>::value_as() {
  return boost::get<array_t&>(any);
}
```

**Brief:** Specializes the generic unwrapper template for JSON-specific types.

## Helper Functions

### to_string (type_t)

**Brief:** Converts a `type_t` enumeration value to its string representation.

```cpp
std::string to_string(type_t t) {
  switch (t) {
    case type_t::NOTSET: return "NOTSET";
    case type_t::VALUE:  return "VALUE";
    case type_t::DATA:   return "DATA";
    case type_t::OBJECT: return "OBJECT";
    case type_t::ARRAY:  return "ARRAY";
  }
  return "NOTSET";
}
```

## Error Handling Strategy

### Informative Error Messages

All comparison operators return detailed error messages that include:
- The type of mismatch (key, value, size, type)
- String representations of both values being compared
- Context information (which key caused the failure)

This design supports:
- Efficient debugging when documents don't match
- Automated testing with meaningful failure messages
- Logging and diagnostics

### No Exception Throwing

Comparison operators do not throw exceptions; instead, they return failure status with error details. This allows callers to handle mismatches gracefully without try-catch blocks.

## Usage Examples

### Deep Comparison with Error Reporting

```cpp
#include "artdaq-database/DataFormats/Json/json_types.h"
#include <iostream>

void compareDocuments() {
  using namespace artdaq::database::json;

  object_t doc1, doc2;
  doc1["name"] = std::string("config");
  doc1["version"] = static_cast<integer>(1);

  doc2["name"] = std::string("config");
  doc2["version"] = static_cast<integer>(2);  // Different!

  auto [equal, message] = doc1 == doc2;

  if (!equal) {
    std::cerr << "Documents differ: " << message << "\n";
    // Output: Documents differ: Ojbects are different at key="version" Values are different...
  }
}
```

### Type Debugging

```cpp
#include "artdaq-database/DataFormats/Json/json_types.h"

void debugType() {
  using namespace artdaq::database::json;

  type_t t = type_t::OBJECT;
  std::cout << "Type: " << to_string(t) << "\n";  // Output: Type: OBJECT
}
```

### Using Unwrapper

```cpp
#include "artdaq-database/DataFormats/Json/json_types.h"

void extractValue() {
  using namespace artdaq::database::json;
  using namespace artdaq::database::sharedtypes;

  value_t v = object_t{};
  auto& obj = unwrap(v).value_as<object_t>();

  obj["key"] = std::string("value");
}
```

## Performance Considerations

- **Recursive comparison:** Deep structures may cause significant recursion; stack overflow possible for extremely nested documents
- **Early termination:** Comparisons return on first mismatch, optimizing for the unequal case
- **Size checks first:** Size comparisons are O(1) and performed before element-wise comparison
- **String allocation:** Error messages allocate strings; in hot paths, consider checking `.first` before accessing `.second`

## Testing Notes

- **Unit tests:** `test/DataFormats/Json/json_types_t.cc`
- **Key test cases:**
  - Value comparison for all primitive types
  - Nested object and array comparison
  - Type mismatch detection
  - Size mismatch detection
  - Error message content verification

## Maintenance Notes

- The "Ojbects" typo in error messages is intentional for backward compatibility with existing tests and logs
- The `noerror` constant (`"Success"`) is used for successful comparison results
- Unwrapper specializations must match those in `json_types_impl.h`

## See Also

- [json_types.h](./json_types.h.md) - Type declarations and visitor definitions
- [json_types_impl.h](./json_types_impl.h.md) - Additional template implementations
- [shared_types.h](../shared_types.h.md) - Base unwrapper template definition
