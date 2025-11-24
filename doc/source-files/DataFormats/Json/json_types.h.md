# json_types.h

## File Overview

This header file defines the core data structures for representing JSON documents as Abstract Syntax Trees (AST). It provides type definitions for JSON objects, arrays, values, and key-value pairs, along with visitor patterns for type-safe operations on JSON data structures.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Json/json_types.h`

## Dependencies

### Third-Party Libraries

**Boost Libraries**:
- `<boost/fusion/adapted/struct/adapt_struct.hpp>` - Boost.Fusion structure adaptation for introspection
- `<boost/fusion/include/adapt_struct.hpp>` - Boost.Fusion includes

### Project Headers

- `"artdaq-database/DataFormats/common.h"` - Common DataFormats headers and boost::variant configuration
- `"artdaq-database/DataFormats/shared_types.h"` - Shared template types (table_of, vector_of, variant_value_of)

## Header Guard

```cpp
#ifndef _ARTDAQ_DATABASE_JSONTYPES_H_
#define _ARTDAQ_DATABASE_JSONTYPES_H_
...
#endif
```

## Namespace

```cpp
artdaq::database::json
```

**Namespace Alias**:
```cpp
namespace jsn = artdaq::database::json;
```

## Core Type Definitions

### Forward Declarations

```cpp
struct object_t;
struct array_t;
```

These are forward-declared because they participate in a recursive type definition with `variant_value_t`.

### Variant Value Type

```cpp
using variant_value_t = sharedtypes::variant_value_of<object_t, array_t>;
```

A variant that can hold any JSON value type:
- `boost::recursive_wrapper<object_t>` - Nested JSON object
- `boost::recursive_wrapper<array_t>` - Nested JSON array
- `std::string` - String value
- `decimal` (double) - Floating-point number
- `integer` (int64_t) - Integer number
- `bool` - Boolean value

### Basic Types

```cpp
using key_t = sharedtypes::basic_key_t;         // std::string
using value_t = variant_value_t;
using data_t = sharedtypes::kv_pair_of<key_t, value_t>;
```

| Type | Description |
|------|-------------|
| `key_t` | JSON object key (string) |
| `value_t` | JSON value (variant of all possible types) |
| `data_t` | JSON key-value pair |

### Composite Types

```cpp
struct object_t : sharedtypes::table_of<data_t> {};
struct array_t : sharedtypes::vector_of<value_t> {};
```

**object_t**: Represents a JSON object as an ordered collection of key-value pairs
- Inherits from `table_of<data_t>`, providing map-like interface
- Maintains insertion order
- Allows duplicate keys (for FHiCL compatibility)

**array_t**: Represents a JSON array as a list of values
- Inherits from `vector_of<value_t>`, providing vector-like interface
- Elements can be of different types (heterogeneous)

## Type Enumeration

```cpp
enum struct type_t {
    NOTSET = 0,
    VALUE,
    DATA,
    OBJECT,
    ARRAY
};
```

**Values**:
- `NOTSET` - Type not yet determined
- `VALUE` - Primitive value (string, number, bool)
- `DATA` - Key-value pair
- `OBJECT` - JSON object
- `ARRAY` - JSON array

**Helper Function**:
```cpp
std::string to_string(type_t t);
```

Converts type enum to string representation.

## Visitor Patterns

### print_visitor

```cpp
struct print_visitor : public boost::static_visitor<std::string>
```

Converts variant values to debug-friendly string representations.

**Overloads**:
```cpp
std::string operator()(object_t const&) const;       // Returns: "object(...)"
std::string operator()(array_t const&) const;        // Returns: "array(...)"
std::string operator()(std::string const& val) const; // Returns: "std::string(<val>)"
std::string operator()(decimal const& val) const;     // Returns: "decimal(<val>)"
std::string operator()(integer const& val) const;     // Returns: "integer(<val>)"
std::string operator()(bool const& val) const;        // Returns: "bool(true/false)"
```

**Usage**:
```cpp
value_t v = std::string("hello");
std::string debug = boost::apply_visitor(print_visitor(), v);
// debug == "std::string(hello)"
```

### tostring_visitor

```cpp
struct tostring_visitor : public boost::static_visitor<std::string>
```

Converts variant values to their actual string representations (not debug format).

**Overloads**:
```cpp
std::string operator()(object_t const&) const;       // Returns: "object(...)"
std::string operator()(array_t const&) const;        // Returns: "array(...)"
std::string operator()(std::string const& val) const; // Returns: val
std::string operator()(decimal const& val) const;     // Returns: std::to_string(val)
std::string operator()(integer const& val) const;     // Returns: std::to_string(val)
std::string operator()(bool const& val) const;        // Returns: "true" or "false"
```

**Usage**:
```cpp
value_t v = 42;
std::string str = boost::apply_visitor(tostring_visitor(), v);
// str == "42"
```

### type_visitor

```cpp
struct type_visitor : public boost::static_visitor<type_t>
```

Determines the type_t enum value for a variant.

**Overloads** (both const and non-const):
```cpp
type_t operator()(object_t const&) const;    // Returns: type_t::OBJECT
type_t operator()(array_t const&) const;     // Returns: type_t::ARRAY
type_t operator()(std::string const&) const; // Returns: type_t::VALUE
type_t operator()(decimal const&) const;     // Returns: type_t::VALUE
type_t operator()(integer const&) const;     // Returns: type_t::VALUE
type_t operator()(bool const&) const;        // Returns: type_t::VALUE
```

**Helper Template**:
```cpp
template <typename T>
type_t type(T& var);
```

Convenience function that applies type_visitor to a variant.

**Usage**:
```cpp
value_t v = object_t{};
type_t t = type(v);  // t == type_t::OBJECT
```

## Comparison Operators

All comparison operators return `std::pair<bool, std::string>`:
- `.first`: true if equal, false otherwise
- `.second`: "Success" if equal, detailed error message if not equal

```cpp
std::pair<bool, std::string> operator==(value_t const&, value_t const&);
std::pair<bool, std::string> operator==(data_t const&, data_t const&);
std::pair<bool, std::string> operator==(array_t const&, array_t const&);
std::pair<bool, std::string> operator==(object_t const&, object_t const&);
```

**Comparison Semantics**:

1. **value_t**: Type-sensitive comparison
   - Different types → not equal
   - Same type → deep comparison

2. **data_t**: Compares both key and value
   - Keys must match exactly
   - Values compared recursively

3. **array_t**: Element-wise comparison
   - Sizes must match
   - Elements compared in order

4. **object_t**: Pair-wise comparison
   - Sizes must match
   - Pairs compared in order (order matters!)

**Usage Example**:
```cpp
object_t obj1, obj2;
// ... populate objects ...

auto result = obj1 == obj2;
if (result.first) {
    std::cout << "Objects are equal\n";
} else {
    std::cout << "Objects differ: " << result.second << "\n";
}
```

## Boost.Fusion Adaptation

```cpp
BOOST_FUSION_ADAPT_STRUCT(jsn::data_t,
    (jsn::key_t, key)
    (jsn::value_t, value))
```

**Purpose**: Adapts `data_t` for use with Boost.Fusion and Boost.Spirit parsers/generators.

**Benefits**:
- Enables automatic parser/generator creation
- Allows tuple-like access to struct members
- Integrates with Boost.Spirit Qi (parser) and Karma (generator)

## Usage Examples

### Creating JSON Structures

```cpp
using namespace artdaq::database::json;

// Create JSON object
object_t config;
config["host"] = std::string("localhost");
config["port"] = static_cast<integer>(8080);
config["enabled"] = true;

// Create JSON array
array_t numbers;
numbers.push_back(static_cast<integer>(1));
numbers.push_back(static_cast<integer>(2));
numbers.push_back(static_cast<integer>(3));

// Nest structures
object_t root;
root["config"] = config;
root["numbers"] = numbers;
```

### Type Checking

```cpp
value_t v = /* ... */;

if (type(v) == type_t::OBJECT) {
    auto& obj = boost::get<object_t>(v);
    // Work with object
} else if (type(v) == type_t::ARRAY) {
    auto& arr = boost::get<array_t>(v);
    // Work with array
}
```

### Visiting Values

```cpp
value_t v = 3.14;

// Debug output
std::cout << boost::apply_visitor(print_visitor(), v);
// Output: "decimal(3.140000)"

// Value as string
std::cout << boost::apply_visitor(tostring_visitor(), v);
// Output: "3.140000"
```

### Comparing Structures

```cpp
object_t obj1, obj2;
obj1["key"] = std::string("value");
obj2["key"] = std::string("value");

auto [equal, message] = obj1 == obj2;
if (equal) {
    // Objects are identical
}
```

## Design Patterns

### Recursive Structures

The use of `boost::recursive_wrapper` allows infinite nesting:

```cpp
object_t root;
object_t nested;
nested["depth"] = static_cast<integer>(2);
root["nested"] = nested;  // Object contains another object
```

### Heterogeneous Arrays

Arrays can hold mixed types:

```cpp
array_t mixed;
mixed.push_back(std::string("text"));
mixed.push_back(static_cast<integer>(42));
mixed.push_back(true);
mixed.push_back(object_t{});  // Nested object
```

### Ordered Object Keys

Unlike standard JSON semantics, `object_t` maintains insertion order:

```cpp
object_t obj;
obj["z"] = 1;
obj["a"] = 2;
obj["m"] = 3;
// Iteration order: z, a, m (insertion order, not alphabetical)
```

## Performance Considerations

1. **Variant Access**: Use `boost::get<T>()` with pointer syntax for safe access:
   ```cpp
   if (auto* str = boost::get<std::string>(&v)) {
       // Use *str
   }
   ```

2. **Comparison Overhead**: Comparison operators perform deep recursive comparisons, which can be expensive for large structures

3. **Memory**: `boost::recursive_wrapper` adds pointer indirection overhead

4. **Iteration**: `object_t` uses `std::list` internally, providing stable iterators but slower random access

## Related Files

- **json_types.cpp** - Implements comparison operators and unwrapper specializations
- **json_types_impl.h** - Template implementations for unwrapper functions
- **json_reader.h** - Parser that produces these types from JSON text
- **json_writer.h** - Generator that converts these types to JSON text
- **common.h** - Configures boost::variant relaxed mode
- **shared_types.h** - Base templates used by these types

## Notes

- The types represent JSON in a format suitable for AST manipulation and transformation
- Order preservation in objects supports FHiCL compatibility (which allows duplicate keys and cares about order)
- The visitor pattern provides type-safe operations without explicit type checking
- Boost.Fusion adaptation enables declarative parser/generator definitions
- All comparison operators return detailed error messages, useful for debugging and testing
