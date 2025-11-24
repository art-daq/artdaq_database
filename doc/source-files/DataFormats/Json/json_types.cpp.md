# json_types.cpp

## File Overview

This implementation file provides the implementations for JSON type comparison operators, template specializations for unwrapper functions, and type conversion utilities. It includes sophisticated deep comparison logic that returns detailed error messages when objects differ.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Json/json_types.cpp`

## Dependencies

### Project Headers
- `"artdaq-database/DataFormats/Json/json_types.h"` - Type declarations
- `"artdaq-database/DataFormats/common.h"` - Common includes

## TRACE Configuration

```cpp
#define TRACE_NAME "json_types.cpp"
```

## Namespace Aliases

```cpp
using artdaq::database::json::array_t;
using artdaq::database::json::object_t;
using artdaq::database::json::value_t;
```

## Unwrapper Template Specializations

### For object_t and array_t

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

**Purpose**: Specializes the generic unwrapper template for JSON-specific types, enabling type-safe extraction from variants.

### Unwrapper Constructors

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

**Purpose**: Provides specialized constructors for both const and non-const JSON types.

## Comparison Implementation

### compare_visitor

```cpp
struct compare_visitor : public boost::static_visitor<bool>
```

**Purpose**: Visitor that performs type-aware comparison of JSON values.

**Overloads**:
- Compares same types using appropriate comparison logic
- Returns false when comparing different types

**Implementation**:
```cpp
bool operator()(object_t const& first, object_t const& second) const {
    return (first == second).first;
}
bool operator()(array_t const& first, array_t const& second) const {
    return (first == second).first;
}
bool operator()(std::string const& first, std::string const& second) const {
    return (first == second);
}
bool operator()(decimal const& first, decimal const& second) const {
    return (first == second);
}
bool operator()(integer const& first, integer const& second) const {
    return (first == second);
}
bool operator()(bool const& first, bool const& second) const {
    return (first == second);
}

// Different types always compare false
template <typename A1, typename A2>
bool operator()(A1 const&, A2 const&) const {
    return false;
}
```

### operator== (value_t)

```cpp
std::pair<bool, std::string> operator==(value_t const& first, value_t const& second)
```

**Algorithm**:
1. Check if variants hold same type using `which()`
2. If types differ, return false with descriptive error message
3. Apply compare_visitor to perform type-specific comparison
4. Return result with "Success" or detailed error message

**Error Messages**:
- Type mismatch: "Values have different types <type1, type2>"
- Value mismatch: "Values are different <value1, value2>"

### operator== (data_t)

```cpp
std::pair<bool, std::string> operator==(data_t const& first, data_t const& second)
```

**Algorithm**:
1. Compare keys (must match exactly)
2. If keys differ, return false with error
3. Compare values using compare_visitor
4. Return result with error message including key names

**Error Messages**:
- Key mismatch: "Keys are different <key1, key2>"
- Value mismatch: "Values are different <key1:value1, key2:value2>"

### operator== (array_t)

```cpp
std::pair<bool, std::string> operator==(array_t const& first, array_t const& second)
```

**Algorithm**:
1. Compare array sizes
2. If sizes differ, return false
3. Iterate through arrays in parallel
4. Compare each element pair using compare_visitor
5. Return on first mismatch or success after all elements

**Error Messages**:
- Size mismatch: "Arrays have different sizes <array1, array2>"
- Element mismatch: "Arrays are different <array1, array2>"

### operator== (object_t)

```cpp
std::pair<bool, std::string> operator==(object_t const& first, object_t const& second)
```

**Algorithm**:
1. Compare object sizes (number of key-value pairs)
2. If sizes differ, return false
3. Iterate through both objects in parallel
4. Compare each data_t pair using data_t::operator==
5. Return on first mismatch with key information

**Error Messages**:
- Size mismatch: "Objects have different sizes <object1, object2>"
- Pair mismatch: "Ojbects are different at key=\"<key>\" <details>"

**Note**: Typo in error message ("Ojbects" instead of "Objects") is in original code.

## Helper Functions

### to_string (type_t)

```cpp
std::string to_string(type_t t)
```

**Implementation**: Simple switch statement converting enum to string.

**Returns**:
- `type_t::NOTSET` → "NOTSET"
- `type_t::VALUE` → "VALUE"
- `type_t::DATA` → "DATA"
- `type_t::OBJECT` → "OBJECT"
- `type_t::ARRAY` → "ARRAY"
- Default → "NOTSET"

## Usage Examples

### Deep Comparison

```cpp
object_t obj1, obj2;
obj1["key"] = std::string("value");
obj2["key"] = std::string("different");

auto [equal, message] = obj1 == obj2;
// equal == false
// message == "Ojbects are different at key=\"key\" Values are different<...>"
```

### Type Debugging

```cpp
type_t t = type_t::OBJECT;
std::cout << to_string(t);  // Prints: "OBJECT"
```

### Unwrapper Usage

```cpp
using namespace artdaq::database::sharedtypes;

value_t v = object_t{};
auto wrapper = unwrap(v);
auto& obj = wrapper.value_as<object_t>();
```

## Design Considerations

**Error Messages**:
- Provide detailed context for debugging
- Include both sides of comparison
- Specify exact location of mismatch (key names, array positions)

**Performance**:
- Recursive comparisons can be expensive for deeply nested structures
- Early termination on first mismatch improves performance
- Size checks performed before element-wise comparison

**Type Safety**:
- Visitor pattern ensures type-safe comparisons
- Template specializations prevent invalid type combinations
- Boost variant provides runtime type checking

## Related Files

- **json_types.h** - Type declarations and visitor definitions
- **json_types_impl.h** - Additional template implementations
- **shared_types.h** - Base unwrapper template

## Notes

- All comparison operators return `std::pair<bool, std::string>` for detailed error reporting
- The `compare_visitor` recursively calls comparison operators for nested structures
- Unwrapper specializations enable clean syntax for accessing nested JSON values
- Error messages use print_visitor to generate human-readable type representations
