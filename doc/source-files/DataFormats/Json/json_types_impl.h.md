# json_types_impl.h

## File Overview

This header provides template implementations for the `unwrapper` helper class specialized for JSON types. It enables convenient extraction of typed values from variant structures with proper error handling and type checking.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Json/json_types_impl.h`

## Dependencies

### Project Headers
- `"artdaq-database/DataFormats/Json/json_types.h"` - JSON type definitions

## Type Aliases

```cpp
using artdaq::database::json::array_t;
using artdaq::database::json::object_t;
using artdaq::database::json::value_t;
```

## Template Specializations

### unwrapper<object_t>::value_as

```cpp
template<>
template<typename T>
T& unwrapper<object_t>::value_as(std::string const& name)
```

**Purpose**: Extracts a typed value from an object by key name.

**Returns**: Reference to value of type T at the specified key.

**Throws**:
- `std::out_of_range` if key not found
- `boost::bad_get` if type mismatch

### unwrapper<const object_t>::value_as

```cpp
template<>
template<typename T>
T& unwrapper<const object_t>::value_as(std::string const& name)
```

**Purpose**: Const version for read-only access.

### unwrapper<value_t>::value_as (with name)

```cpp
template<>
template<typename T>
T& unwrapper<value_t>::value_as(std::string const& name)
```

**Purpose**: Extracts a value from a variant that contains an object, then gets the specified key.

**Algorithm**:
1. Extract object from value_t variant
2. Get value at key from object
3. Extract type T from that value

### unwrapper<const value_t>::value_as (with name)

```cpp
template<>
template<typename T>
T& unwrapper<const value_t>::value_as(std::string const& name)
```

**Purpose**: Const version with static assertion ensuring T is const-qualified.

**Static Assert**: `"Template argument T is not a const-qualified type."`

### unwrapper<value_t>::value_as (no name)

```cpp
template<>
template<typename T>
T& unwrapper<value_t>::value_as()
```

**Purpose**: Extracts type T directly from value_t variant.

### unwrapper<const value_t>::value_as (no name)

```cpp
template<>
template<typename T>
T& unwrapper<const value_t>::value_as()
```

**Purpose**: Const version with static assertion.

## Usage Examples

### Direct Value Extraction

```cpp
value_t v = std::string("hello");
auto& str = unwrap(v).value_as<std::string>();
// str == "hello"
```

### Nested Access

```cpp
object_t obj;
obj["config"] = object_t{};

value_t v = obj;
auto& nested = unwrap(v).value_as<object_t>("config");
```

### Const-Correct Access

```cpp
const value_t v = 42;
auto& num = unwrap(v).value_as<const integer>();
// Compiler error if T is not const-qualified
```

## Design Patterns

**Type Safety**: Static assertions enforce const-correctness for const variants.

**Error Propagation**: Exceptions from `boost::get` and `at()` are re-thrown for debugging.

**Convenience**: Enables chaining with `unwrap()` factory function.

## Related Files

- **json_types.h** - Declares unwrapper template
- **json_types.cpp** - Implements unwrapper constructors
- **shared_types.h** - Base unwrapper template definition

## Notes

- All specializations use try-catch to allow exception propagation
- Static assertions ensure type safety for const operations
- Template design allows extracting nested values in one expression
