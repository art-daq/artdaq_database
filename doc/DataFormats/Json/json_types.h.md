# json_types.h

**Path:** `artdaq-database/DataFormats/Json/json_types.h`

**Purpose:** Defines the core data structures for representing JSON documents as Abstract Syntax Trees (AST). This header provides type definitions for JSON objects, arrays, values, and key-value pairs, along with visitor patterns for type-safe operations on JSON data structures. These types form the foundation for all JSON manipulation in artdaq-database.


## Key Concepts

### JSON Abstract Syntax Tree (AST)

The JSON AST represents parsed JSON documents as a tree of C++ objects. This approach allows:
- Type-safe manipulation of JSON data
- Efficient traversal and transformation
- Integration with C++ type system

### Recursive Variant Types

JSON structures are inherently recursive (objects can contain objects, arrays can contain arrays). This is handled using `boost::recursive_wrapper` which allows variants to contain types that include the variant itself.

### Boost.Fusion Adaptation

The `data_t` structure is adapted for Boost.Fusion, enabling automatic parser/generator creation with Boost.Spirit. This powers the JSON reader and writer implementations.

## Thread Safety

- **Thread-safe:** No (mutable data structures)
- **Concurrent access:** Multiple readers are safe; any writer requires exclusive access
- **Locking:** No internal locking; callers must synchronize

## Dependencies

| Include | Purpose |
|---------|---------|
| `<boost/fusion/adapted/struct/adapt_struct.hpp>` | Boost.Fusion structure adaptation for Boost.Spirit integration |
| `<boost/fusion/include/adapt_struct.hpp>` | Boost.Fusion includes |
| `artdaq-database/DataFormats/common.h` | Common infrastructure (boost::variant configuration) |
| `artdaq-database/DataFormats/shared_types.h` | Shared template types (`table_of`, `vector_of`, `variant_value_of`) |

## Type Definitions

### Forward Declarations

```cpp
struct object_t;
struct array_t;
```

**Brief:** Forward declarations allowing recursive type definitions.

---

### `variant_value_t`

```cpp
using variant_value_t = sharedtypes::variant_value_of<object_t, array_t>;
```

**Brief:** A boost::variant that can hold any JSON value type including nested objects and arrays.

**Contained Types:**
- `boost::recursive_wrapper<object_t>` - Nested JSON object
- `boost::recursive_wrapper<array_t>` - Nested JSON array
- `std::string` - String value
- `decimal` (double) - Floating-point number
- `integer` (int64_t) - Integer number
- `bool` - Boolean value

---

### `key_t`

```cpp
using key_t = sharedtypes::basic_key_t;
```

**Brief:** Type alias for JSON object keys (std::string).

---

### `value_t`

```cpp
using value_t = variant_value_t;
```

**Brief:** Type alias for JSON values, equivalent to `variant_value_t`.

---

### `data_t`

```cpp
using data_t = sharedtypes::kv_pair_of<key_t, value_t>;
```

**Brief:** A key-value pair representing a single entry in a JSON object.

**Members:**
- `key` - The string key
- `value` - The variant value

---

### `type_t`

```cpp
enum struct type_t {
  NOTSET = 0,
  VALUE,
  DATA,
  OBJECT,
  ARRAY
};
```

**Brief:** Enumeration for categorizing JSON node types.

| Value | Description |
|-------|-------------|
| `NOTSET` | Type not yet determined |
| `VALUE` | Primitive value (string, number, bool) |
| `DATA` | Key-value pair |
| `OBJECT` | JSON object (collection of key-value pairs) |
| `ARRAY` | JSON array (ordered list of values) |

## Classes/Structures

### `object_t`

```cpp
struct object_t : sharedtypes::table_of<data_t> {};
```

**Brief:** Represents a JSON object as an ordered collection of key-value pairs.

**Thread Safety:** Not thread-safe

**Characteristics:**
- Maintains insertion order (unlike std::map)
- Allows duplicate keys (for FHiCL compatibility)
- Provides map-like interface (`at()`, `operator[]`, `count()`)

**Example:**
```cpp
#include "artdaq-database/DataFormats/Json/json_types.h"

void createJsonObject() {
  using namespace artdaq::database::json;

  object_t config;
  config["name"] = std::string("detector");
  config["threshold"] = static_cast<integer>(100);
  config["enabled"] = true;

  // Access value
  auto& name = boost::get<std::string>(config.at("name"));
}
```

---

### `array_t`

```cpp
struct array_t : sharedtypes::vector_of<value_t> {};
```

**Brief:** Represents a JSON array as a list of values.

**Thread Safety:** Not thread-safe

**Characteristics:**
- Provides vector-like interface (`push_back()`, `size()`, iteration)
- Elements can be of different types (heterogeneous)
- Supports nesting (arrays of arrays, arrays of objects)

**Example:**
```cpp
#include "artdaq-database/DataFormats/Json/json_types.h"

void createJsonArray() {
  using namespace artdaq::database::json;

  array_t numbers;
  numbers.push_back(static_cast<integer>(1));
  numbers.push_back(static_cast<integer>(2));
  numbers.push_back(static_cast<integer>(3));

  // Heterogeneous array
  array_t mixed;
  mixed.push_back(std::string("text"));
  mixed.push_back(static_cast<integer>(42));
  mixed.push_back(true);
}
```

---

### `print_visitor`

```cpp
struct print_visitor : public boost::static_visitor<std::string>
```

**Brief:** Visitor that converts variant values to debug-friendly string representations.

**Thread Safety:** Thread-safe (stateless)

#### Methods

##### `operator()(object_t const&) -> std::string`

**Brief:** Returns `"object(...)"` for objects.

##### `operator()(array_t const&) -> std::string`

**Brief:** Returns `"array(...)"` for arrays.

##### `operator()(std::string const& val) -> std::string`

**Brief:** Returns `"std::string(<value>)"` for strings.

##### `operator()(decimal const& val) -> std::string`

**Brief:** Returns `"decimal(<value>)"` for floating-point numbers.

##### `operator()(integer const& val) -> std::string`

**Brief:** Returns `"integer(<value>)"` for integers.

##### `operator()(bool const& val) -> std::string`

**Brief:** Returns `"bool(true)"` or `"bool(false)"` for booleans.

**Example:**
```cpp
value_t v = std::string("hello");
std::string debug = boost::apply_visitor(print_visitor(), v);
// debug == "std::string(hello)"
```

---

### `tostring_visitor`

```cpp
struct tostring_visitor : public boost::static_visitor<std::string>
```

**Brief:** Visitor that converts variant values to their actual string representations.

**Thread Safety:** Thread-safe (stateless)

#### Methods

##### `operator()(object_t const&) -> std::string`

**Brief:** Returns `"object(...)"` for objects (cannot be fully stringified).

##### `operator()(array_t const&) -> std::string`

**Brief:** Returns `"array(...)"` for arrays (cannot be fully stringified).

##### `operator()(std::string const& val) -> std::string`

**Brief:** Returns the string value directly.

##### `operator()(decimal const& val) -> std::string`

**Brief:** Returns the numeric string representation.

##### `operator()(integer const& val) -> std::string`

**Brief:** Returns the numeric string representation.

##### `operator()(bool const& val) -> std::string`

**Brief:** Returns `"true"` or `"false"`.

**Example:**
```cpp
value_t v = static_cast<integer>(42);
std::string str = boost::apply_visitor(tostring_visitor(), v);
// str == "42"
```

---

### `type_visitor`

```cpp
struct type_visitor : public boost::static_visitor<type_t>
```

**Brief:** Visitor that determines the `type_t` enumeration value for a variant.

**Thread Safety:** Thread-safe (stateless)

#### Methods

All overloads return the appropriate `type_t` value:
- `object_t` -> `type_t::OBJECT`
- `array_t` -> `type_t::ARRAY`
- `std::string`, `decimal`, `integer`, `bool` -> `type_t::VALUE`

Provides both const and non-const overloads for flexibility.

## Functions

### `to_string(t) -> std::string`

**Brief:** Converts a `type_t` enumeration value to its string representation.

**Parameters:**
- `t` - The type enumeration value

**Returns:** String representation ("NOTSET", "VALUE", "DATA", "OBJECT", "ARRAY")

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | This function does not throw |

**Thread Safety:** Safe

---

### `type<T>(var) -> type_t`

**Brief:** Template function that determines the type of a variant value.

**Parameters:**
- `var` - Reference to the variant value

**Preconditions:**
- `var` must not be empty

**Returns:** The `type_t` enumeration value for the variant's current type

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Uses `confirm()` macro for precondition |

**Thread Safety:** Safe

**Example:**
```cpp
value_t v = object_t{};
type_t t = type(v);  // t == type_t::OBJECT
```

## Comparison Operators

All comparison operators return `std::pair<bool, std::string>`:
- `.first`: `true` if equal, `false` otherwise
- `.second`: `"Success"` if equal, detailed error message if not equal

This design provides detailed diagnostic information when comparisons fail, useful for testing and debugging.

### `operator==(value_t const&, value_t const&) -> std::pair<bool, std::string>`

**Brief:** Compares two JSON values for equality.

**Returns:** Pair indicating equality and error message if different

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | This function does not throw |

**Comparison Semantics:**
- Different variant types -> not equal
- Same type -> type-specific deep comparison

---

### `operator==(data_t const&, data_t const&) -> std::pair<bool, std::string>`

**Brief:** Compares two key-value pairs for equality.

**Returns:** Pair indicating equality and error message if different

**Comparison Semantics:**
- Keys must match exactly
- Values compared using value_t comparison

---

### `operator==(array_t const&, array_t const&) -> std::pair<bool, std::string>`

**Brief:** Compares two JSON arrays for equality.

**Returns:** Pair indicating equality and error message if different

**Comparison Semantics:**
- Sizes must match
- Elements compared in order using value_t comparison

---

### `operator==(object_t const&, object_t const&) -> std::pair<bool, std::string>`

**Brief:** Compares two JSON objects for equality.

**Returns:** Pair indicating equality and error message if different

**Comparison Semantics:**
- Sizes must match
- Pairs compared in order (order matters!)

**Example:**
```cpp
object_t obj1, obj2;
obj1["key"] = std::string("value");
obj2["key"] = std::string("value");

auto [equal, message] = obj1 == obj2;
if (equal) {
  std::cout << "Objects are equal\n";
} else {
  std::cout << "Objects differ: " << message << "\n";
}
```

## Boost.Fusion Adaptation

```cpp
BOOST_FUSION_ADAPT_STRUCT(jsn::data_t,
    (jsn::key_t, key)
    (jsn::value_t, value))
```

**Brief:** Adapts `data_t` for use with Boost.Fusion and Boost.Spirit parsers/generators.

**Purpose:**
- Enables automatic parser/generator creation
- Allows tuple-like access to struct members
- Integrates with Boost.Spirit Qi (parser) and Karma (generator)

## Namespace Alias

```cpp
namespace jsn = artdaq::database::json;
```

**Brief:** Convenience alias for the `artdaq::database::json` namespace.

## Usage Examples

### Creating JSON Structures

```cpp
#include "artdaq-database/DataFormats/Json/json_types.h"

void createComplexJson() {
  using namespace artdaq::database::json;

  // Create nested structure
  object_t config;
  config["name"] = std::string("detector_config");
  config["version"] = static_cast<integer>(1);

  object_t parameters;
  parameters["threshold"] = static_cast<integer>(100);
  parameters["enabled"] = true;
  config["parameters"] = parameters;

  array_t channels;
  channels.push_back(static_cast<integer>(0));
  channels.push_back(static_cast<integer>(1));
  channels.push_back(static_cast<integer>(2));
  config["channels"] = channels;
}
```

### Type Checking

```cpp
#include "artdaq-database/DataFormats/Json/json_types.h"

void checkValueType(const value_t& v) {
  using namespace artdaq::database::json;

  switch (type(v)) {
    case type_t::OBJECT:
      std::cout << "Value is an object\n";
      break;
    case type_t::ARRAY:
      std::cout << "Value is an array\n";
      break;
    case type_t::VALUE:
      std::cout << "Value is a primitive\n";
      break;
    default:
      std::cout << "Unknown type\n";
  }
}
```

### Safe Value Extraction

```cpp
#include "artdaq-database/DataFormats/Json/json_types.h"

void safeExtraction(const value_t& v) {
  using namespace artdaq::database::json;

  // Safe extraction using pointer
  if (auto* str = boost::get<std::string>(&v)) {
    std::cout << "String value: " << *str << "\n";
  } else if (auto* num = boost::get<integer>(&v)) {
    std::cout << "Integer value: " << *num << "\n";
  } else {
    std::cout << "Other type\n";
  }
}
```

## Relationship to Other Components

This header is foundational to the JSON DataFormats module:

- **json_reader.h/cpp**: Parser produces these types from JSON text
- **json_writer.h/cpp**: Generator converts these types to JSON text
- **json_types_impl.h**: Template implementations for unwrapper functions
- **convertfhicl2jsondb.h**: Uses these types for FHiCL to JSON conversion
- **JsonDocument**: Uses these types for document representation

## See Also

- [json_types.cpp](./json_types.cpp.md) - Implementation of comparison operators
- [json_types_impl.h](./json_types_impl.h.md) - Template implementations for unwrapper
- [json_reader.h](./json_reader.h.md) - JSON parser
- [json_writer.h](./json_writer.h.md) - JSON generator
- [shared_types.h](../shared_types.h.md) - Base templates used by these types
- [External: Boost.Variant](https://www.boost.org/doc/libs/release/doc/html/variant.html) - Variant type documentation

## Notes for Developers

### Common Pitfalls

- **Type casting for integers:** Use `static_cast<integer>()` when assigning integer literals to avoid ambiguity
- **Order sensitivity:** `object_t` comparison is order-sensitive, which differs from standard JSON semantics
- **Recursive structures:** Deep nesting can cause stack overflow during operations

### Anti-patterns

```cpp
// DON'T: Assign integer literal directly (ambiguous)
object["count"] = 42;  // May become decimal instead of integer

// DO: Cast explicitly
object["count"] = static_cast<integer>(42);

// DON'T: Assume map-like key uniqueness
object["key"] = "first";
object["key"] = "second";  // Creates duplicate, doesn't overwrite!

// DO: Check before inserting if uniqueness needed
if (object.count("key") == 0) {
  object["key"] = "value";
}
```

### Performance Considerations

1. **Variant access:** Use `boost::get<T>()` with pointer syntax for safe, fast access
2. **Comparison overhead:** Deep comparison is recursive and can be expensive
3. **Memory:** `boost::recursive_wrapper` adds pointer indirection
4. **Iteration:** `object_t` uses list-based storage, slower random access than map
