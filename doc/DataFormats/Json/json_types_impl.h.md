# json_types_impl.h

**Path:** `artdaq-database/DataFormats/Json/json_types_impl.h`

**Purpose:** Provides template implementations for the `unwrapper` helper class specialized for JSON types. This header enables convenient extraction of typed values from variant structures with proper error handling and type checking, supporting both direct extraction and nested key-based access patterns.


## Key Concepts

### Unwrapper Pattern

The `unwrapper` class template provides a fluent interface for extracting typed values from boost::variant types. Instead of writing verbose `boost::get<T>()` calls, you can use:

```cpp
auto& value = unwrap(variant).value_as<TargetType>();
```

This pattern:
- Reduces boilerplate code
- Provides consistent error handling
- Supports chained access for nested structures

### Const-Correctness

The implementation provides separate specializations for const and non-const variants. Const versions include static assertions to ensure the template argument is also const-qualified, preventing accidental mutable access to const data.

## Thread Safety

- **Thread-safe:** Yes (stateless operations)
- **Concurrent access:** Safe for read operations; modifications require exclusive access
- **Locking:** No internal locking

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/Json/json_types.h` | JSON type definitions (`object_t`, `array_t`, `value_t`) |

## Template Specializations

### `unwrapper<object_t>::value_as<T>(name) -> T&`

**Brief:** Extracts a typed value from a JSON object by key name.

**Template Parameters:**
- `T` - The expected type of the value

**Parameters:**
- `name` - The key name to look up in the object

**Preconditions:**
- `name` must not be empty
- The key must exist in the object
- The value at the key must be of type T

**Returns:** Reference to the value of type T at the specified key

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::out_of_range` | When key is not found in the object |
| `boost::bad_get` | When the value type does not match T |

**Thread Safety:** Safe for read; requires exclusive access for modification

**Example:**
```cpp
#include "artdaq-database/DataFormats/Json/json_types_impl.h"

void accessObjectValue() {
  using namespace artdaq::database::json;
  using namespace artdaq::database::sharedtypes;

  object_t config;
  config["threshold"] = static_cast<integer>(100);

  try {
    auto& threshold = unwrap(config).value_as<integer>("threshold");
    std::cout << "Threshold: " << threshold << "\n";
  } catch (const std::out_of_range& e) {
    std::cerr << "Key not found\n";
  } catch (const boost::bad_get& e) {
    std::cerr << "Type mismatch\n";
  }
}
```

---

### `unwrapper<const object_t>::value_as<T>(name) -> T&`

**Brief:** Extracts a typed value from a const JSON object by key name.

**Template Parameters:**
- `T` - The expected type of the value (should be const-qualified)

**Parameters:**
- `name` - The key name to look up

**Returns:** Reference to the const value

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::out_of_range` | When key is not found |
| `boost::bad_get` | When type does not match |

---

### `unwrapper<value_t>::value_as<T>(name) -> T&`

**Brief:** Extracts a typed value from a variant containing an object, accessing by key name.

This specialization enables chained access: it first extracts the `object_t` from the `value_t`, then looks up the key.

**Template Parameters:**
- `T` - The expected type of the nested value

**Parameters:**
- `name` - The key name to look up

**Preconditions:**
- `name` must not be empty
- The variant must contain an `object_t`
- The key must exist in that object
- The value at the key must be of type T

**Returns:** Reference to the value of type T

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `boost::bad_get` | When variant doesn't contain object_t |
| `std::out_of_range` | When key is not found |
| `boost::bad_get` | When value type doesn't match T |

**Example:**
```cpp
#include "artdaq-database/DataFormats/Json/json_types_impl.h"

void accessNestedValue() {
  using namespace artdaq::database::json;
  using namespace artdaq::database::sharedtypes;

  object_t inner;
  inner["value"] = static_cast<integer>(42);

  value_t outer = inner;

  try {
    auto& val = unwrap(outer).value_as<integer>("value");
    std::cout << "Value: " << val << "\n";
  } catch (...) {
    std::cerr << "Access failed\n";
  }
}
```

---

### `unwrapper<const value_t>::value_as<T>(name) -> T&`

**Brief:** Const version of value_as with key access for value_t variants.

**Static Assertion:** Enforces that T is const-qualified:
```cpp
static_assert(std::is_const<T>(), "Template argument T is not a const-qualified type.");
```

**Template Parameters:**
- `T` - Must be a const-qualified type

**Returns:** Reference to the const value

---

### `unwrapper<value_t>::value_as<T>() -> T&`

**Brief:** Extracts a typed value directly from a value_t variant without key lookup.

**Template Parameters:**
- `T` - The expected type of the variant's current value

**Returns:** Reference to the value of type T

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `boost::bad_get` | When the variant doesn't contain type T |

**Example:**
```cpp
#include "artdaq-database/DataFormats/Json/json_types_impl.h"

void directExtraction() {
  using namespace artdaq::database::json;
  using namespace artdaq::database::sharedtypes;

  value_t v = std::string("hello");

  try {
    auto& str = unwrap(v).value_as<std::string>();
    std::cout << "String: " << str << "\n";
  } catch (const boost::bad_get& e) {
    std::cerr << "Not a string\n";
  }
}
```

---

### `unwrapper<const value_t>::value_as<T>() -> T&`

**Brief:** Const version of direct value extraction from value_t.

**Static Assertion:** Enforces that T is const-qualified.

**Template Parameters:**
- `T` - Must be a const-qualified type

**Returns:** Reference to the const value

**Example:**
```cpp
#include "artdaq-database/DataFormats/Json/json_types_impl.h"

void constAccess() {
  using namespace artdaq::database::json;
  using namespace artdaq::database::sharedtypes;

  const value_t v = static_cast<integer>(42);

  // Correct: const-qualified type
  auto& num = unwrap(v).value_as<const integer>();

  // Compiler error: non-const type with const variant
  // auto& bad = unwrap(v).value_as<integer>();
}
```

## Usage Examples

### Chained Access Pattern

```cpp
#include "artdaq-database/DataFormats/Json/json_types_impl.h"

void chainedAccess() {
  using namespace artdaq::database::json;
  using namespace artdaq::database::sharedtypes;

  // Create nested structure
  object_t config;
  object_t detector;
  detector["threshold"] = static_cast<integer>(100);
  detector["enabled"] = true;
  config["detector"] = detector;

  // Access nested values
  try {
    // Get detector object
    auto& det = unwrap(config).value_as<object_t>("detector");

    // Get threshold from detector
    auto& threshold = unwrap(det).value_as<integer>("threshold");

    std::cout << "Threshold: " << threshold << "\n";
  } catch (const std::exception& e) {
    std::cerr << "Access error: " << e.what() << "\n";
  }
}
```

### Safe Access with Type Checking

```cpp
#include "artdaq-database/DataFormats/Json/json_types_impl.h"

bool safeGetInteger(const object_t& obj, const std::string& key, integer& out) {
  using namespace artdaq::database::sharedtypes;

  try {
    out = unwrap(obj).value_as<const integer>(key);
    return true;
  } catch (...) {
    return false;
  }
}
```

## Relationship to Other Components

This header extends the unwrapper infrastructure for JSON-specific types:

```
shared_types.h          (base unwrapper template)
    |
    +-- json_types.h    (JSON type definitions)
    |       |
    |       +-- json_types_impl.h (this file - template implementations)
    |
    +-- json_types.cpp  (unwrapper constructor specializations)
```

## See Also

- [json_types.h](./json_types.h.md) - JSON type declarations
- [json_types.cpp](./json_types.cpp.md) - Unwrapper constructor implementations
- [shared_types.h](../shared_types.h.md) - Base unwrapper template definition

## Notes for Developers

### Common Pitfalls

- **Const-qualification:** When using const variants, the type parameter T must be const-qualified
- **Exception safety:** All specializations may throw; always use try-catch when the operation might fail
- **Empty keys:** The `confirm(!name.empty())` precondition will fail on empty key strings

### Anti-patterns

```cpp
// DON'T: Use non-const type with const variant
const value_t v = 42;
auto& bad = unwrap(v).value_as<integer>();  // Static assertion fails!

// DO: Use const-qualified type
auto& good = unwrap(v).value_as<const integer>();

// DON'T: Ignore exceptions in production code
auto& val = unwrap(obj).value_as<integer>("key");  // May throw!

// DO: Handle potential failures
try {
  auto& val = unwrap(obj).value_as<integer>("key");
} catch (const std::out_of_range&) {
  // Key doesn't exist
} catch (const boost::bad_get&) {
  // Type mismatch
}
```

### Design Rationale

The try-catch blocks in the implementation simply re-throw exceptions. This pattern:
1. Allows setting breakpoints in the catch block during debugging
2. Provides a consistent exception propagation point
3. Documents that exceptions are expected behavior, not errors
