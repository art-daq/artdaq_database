# ovlKeyValue.h

**Path:** `artdaq-database/Overlay/ovlKeyValue.h`

**Purpose:** This header defines `ovlKeyValue`, the fundamental base class for all overlay objects in the Overlay module. It provides the core abstraction for wrapping JSON key-value pairs with type-safe C++ accessors, enabling the overlay pattern where C++ objects provide convenient access to underlying JSON data structures without data duplication.


## Key Concepts

### Reference-Based Design

The class stores a **reference** to JSON data, not a copy:
```cpp
private:
  object_t::key_type _key;   // The JSON key (stored by value)
  value_t& _value;           // Reference to JSON value (NOT a copy!)
```

**Implications:**
- Changes through overlay objects modify the underlying JSON directly
- Overlay object lifetime must not exceed JSON data lifetime
- No data duplication - memory efficient for large documents
- Multiple overlays can reference the same JSON structure

### Template Accessor Pattern

Type-safe access to nested values through template methods:
```cpp
template <typename T>
T& value_as(object_t::key_type const& key);
```

This pattern allows type-safe retrieval of nested JSON values without explicit casting at the call site.

### Factory Function Pattern

The `overlay<>()` template function creates overlay objects with automatic field initialization, creating missing fields with default values as needed.

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** References mutable JSON data and uses a static comparison mask
- **Locking:** None - callers must ensure single-threaded access

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/Overlay/common.h` | Module foundation types, constants, and comparison flags |

## Classes/Structures

### `ovlKeyValue`

**Brief:** Base class for all overlay objects, wrapping a JSON key-value pair with type-safe accessors and comparison support.

**Thread Safety:** Not thread-safe

#### Constructor

##### `ovlKeyValue(object_t::key_type key, value_t& value)`

**Brief:** Constructs an overlay wrapping a JSON key-value pair. The key is moved into storage for efficiency, and the value is stored by reference.

**Parameters:**
- `key` - The JSON key this overlay represents (moved into storage)
- `value` - Reference to the JSON value (stored by reference, not copied)

**Preconditions:**
- `value` must remain valid for the lifetime of the overlay
- `value` should be the appropriate JSON type for the intended use

**Postconditions:**
- Overlay wraps the given key-value pair
- Modifications through the overlay affect the original JSON

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Constructor does not throw |

**Thread Safety:** Unsafe

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlKeyValue.h"
#include <iostream>

using namespace artdaq::database::overlay;

void wrapJsonValue(value_t& jsonValue) {
  try {
    ovlKeyValue wrapper{"myKey", jsonValue};
    std::cout << "Created overlay for key: " << wrapper.key() << "\n";
    // wrapper now provides type-safe access to jsonValue
  } catch (const std::exception& e) {
    std::cerr << "Failed to create overlay: " << e.what() << "\n";
  }
}
```

#### Special Member Functions

##### `ovlKeyValue(ovlKeyValue&&) = default`

**Brief:** Move constructor, allowing overlays to be moved efficiently. The moved-from overlay should not be used after the move.

##### `virtual ~ovlKeyValue() = default`

**Brief:** Virtual destructor enabling proper cleanup when deleting derived classes through a base class pointer.

#### Methods

##### `value(object_t::key_type const& key) -> value_t&`

**Brief:** Returns a reference to a nested JSON value by key. This method treats the wrapped value as a JSON object and retrieves the specified field.

**Parameters:**
- `key` - The key of the nested value to retrieve

**Preconditions:**
- The wrapped value must be a JSON object
- The key must exist in the object

**Returns:** Reference to the nested JSON value

**Postconditions:**
- None (returns reference to existing data)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` | If key does not exist |
| `std::bad_cast` | If wrapped value is not an object |

**Thread Safety:** Unsafe

---

##### `value_as<T>(object_t::key_type const& key) -> T&`

**Brief:** Returns a typed reference to a nested value by key. This is a template method that provides type-safe access to nested JSON values.

**Template Parameters:**
- `T` - The expected C++ type of the nested value (e.g., `std::string`, `int`, `bool`, `object_t`, `array_t`)

**Parameters:**
- `key` - The key of the nested value to retrieve

**Preconditions:**
- The wrapped value must be a JSON object
- The key must exist in the object
- The nested value must be convertible to type T

**Returns:** Reference to the nested value cast to type T

**Postconditions:**
- None (returns reference to existing data)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If value cannot be converted to type T |
| `std::runtime_error` | If key does not exist |

**Thread Safety:** Unsafe

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlKeyValue.h"

using namespace artdaq::database::overlay;

void accessNestedValues(ovlKeyValue& wrapper) {
  try {
    auto& count = wrapper.value_as<int>("count");
    auto& name = wrapper.value_as<std::string>("name");
    auto& enabled = wrapper.value_as<bool>("enabled");

    std::cout << "Name: " << name << ", Count: " << count << "\n";
  } catch (const std::bad_cast& e) {
    std::cerr << "Type mismatch: " << e.what() << "\n";
  } catch (const std::runtime_error& e) {
    std::cerr << "Key not found: " << e.what() << "\n";
  }
}
```

---

##### `value_as<T>(object_t::key_type const& key) const -> T const&`

**Brief:** Returns a const typed reference to a nested value by key. This is the const version of the template accessor.

**Template Parameters:**
- `T` - The expected C++ type of the nested value

**Parameters:**
- `key` - The key of the nested value to retrieve

**Preconditions:**
- The wrapped value must be a JSON object
- The key must exist in the object

**Returns:** Const reference to the nested value cast to type T

**Postconditions:**
- None (returns reference to existing data)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If value cannot be converted to type T |
| `std::runtime_error` | If key does not exist |

**Thread Safety:** Unsafe

---

##### `array_value() -> array_t&`

**Brief:** Treats the wrapped value as a JSON array and returns a reference to it. Use this when the overlay wraps an array-type JSON value.

**Preconditions:**
- The wrapped value must be a JSON array

**Returns:** Reference to the value as an array

**Postconditions:**
- None (returns reference to existing data)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If value is not a JSON array |

**Thread Safety:** Unsafe

---

##### `object_value() -> object_t&`

**Brief:** Treats the wrapped value as a JSON object and returns a reference to it. Use this when the overlay wraps an object-type JSON value.

**Preconditions:**
- The wrapped value must be a JSON object

**Returns:** Reference to the value as an object

**Postconditions:**
- None (returns reference to existing data)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If value is not a JSON object |

**Thread Safety:** Unsafe

---

##### `key() -> object_t::key_type&`

**Brief:** Returns a mutable reference to the overlay's key. This allows changing the key associated with the overlay.

**Preconditions:**
- None

**Returns:** Reference to the key string

**Postconditions:**
- None (returns reference to existing data)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | This method does not throw |

**Thread Safety:** Unsafe

---

##### `key() const -> object_t::key_type const&`

**Brief:** Returns a const reference to the overlay's key for read-only access.

**Preconditions:**
- None

**Returns:** Const reference to the key string

**Postconditions:**
- None

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | This method does not throw |

**Thread Safety:** Unsafe

---

##### `value() -> value_t&`

**Brief:** Returns a reference to the raw wrapped JSON value. This provides direct access to the underlying JSON without type conversion.

**Preconditions:**
- None

**Returns:** Reference to the underlying JSON value

**Postconditions:**
- None (returns reference to existing data)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | This method does not throw |

**Thread Safety:** Unsafe

---

##### `value() const -> value_t const&`

**Brief:** Returns a const reference to the raw wrapped JSON value for read-only access.

**Preconditions:**
- None

**Returns:** Const reference to the underlying JSON value

**Postconditions:**
- None

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | This method does not throw |

**Thread Safety:** Unsafe

---

##### `string_value() -> std::string&`

**Brief:** Treats the wrapped value as a string and returns a reference to it. Use this when the overlay wraps a string-type JSON value.

**Preconditions:**
- The wrapped value must be a JSON string

**Returns:** Reference to the value as a string

**Postconditions:**
- None (returns reference to existing data)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If value is not a string |

**Thread Safety:** Unsafe

---

##### `string_value() const -> std::string const&`

**Brief:** Returns a const reference to the wrapped value as a string for read-only access.

**Preconditions:**
- The wrapped value must be a JSON string

**Returns:** Const reference to the value as a string

**Postconditions:**
- None

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If value is not a string |

**Thread Safety:** Unsafe

---

##### `self() -> ovlKeyValue&`

**Brief:** Returns a reference to this overlay object. This is useful for comparison operations where you need to compare at the base class level.

**Preconditions:**
- None

**Returns:** Reference to `*this`

**Postconditions:**
- None

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | This method does not throw |

**Thread Safety:** Unsafe

---

##### `self() const -> ovlKeyValue const&`

**Brief:** Returns a const reference to this overlay object.

**Preconditions:**
- None

**Returns:** Const reference to `*this`

**Postconditions:**
- None

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | This method does not throw |

**Thread Safety:** Unsafe

---

##### `to_string() const -> std::string` [virtual]

**Brief:** Serializes the key-value pair to a JSON string representation. This method creates a temporary JSON object containing the key-value pair and serializes it.

**Preconditions:**
- None

**Returns:** JSON string of the form `{"key": value}`, or `msg_ConvertionError` on serialization failure

**Postconditions:**
- None (creates temporary data for serialization)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Errors are returned as the `msg_ConvertionError` string |

**Thread Safety:** Unsafe

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlKeyValue.h"
#include <iostream>

using namespace artdaq::database::overlay;

void printOverlay(ovlKeyValue const& wrapper) {
  std::string jsonStr = wrapper.to_string();
  if (jsonStr == msg_ConvertionError) {
    std::cerr << "Failed to serialize overlay\n";
  } else {
    std::cout << "Overlay content: " << jsonStr << "\n";
  }
}
```

---

##### `operator==(ovlKeyValue const& other) const -> result_t`

**Brief:** Compares this overlay with another for equality, returning detailed difference information. Compares both keys and values.

**Parameters:**
- `other` - The overlay to compare against

**Preconditions:**
- None

**Returns:** `result_t` pair where:
- `first` is `true` if equal, `false` otherwise
- `second` contains a detailed description of differences if not equal, empty string if equal

**Postconditions:**
- None (read-only comparison)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Exceptions are caught internally |

**Thread Safety:** Unsafe

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlKeyValue.h"
#include <iostream>

using namespace artdaq::database::overlay;

void compareOverlays(ovlKeyValue const& wrapper1, ovlKeyValue const& wrapper2) {
  auto result = wrapper1 == wrapper2;
  if (result.first) {
    std::cout << "Overlays are equal\n";
  } else {
    std::cerr << "Overlays differ: " << result.second << "\n";
  }
}
```

---

##### `swap(ovlKeyValue* other) -> result_t`

**Brief:** Swaps the key and value with another overlay object. Both the key and value references are exchanged.

**Parameters:**
- `other` - Pointer to the overlay to swap with (must not be null)

**Preconditions:**
- `other` must not be null

**Returns:** `result_t` with success/failure status:
- On success: `{true, ""}`
- On failure: `{false, error_message}`

**Postconditions:**
- On success: This overlay has other's key/value, and other has this overlay's original key/value

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Exceptions are caught and converted to Failure result |

**Thread Safety:** Unsafe

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlKeyValue.h"
#include <iostream>

using namespace artdaq::database::overlay;

void swapOverlays(ovlKeyValue& first, ovlKeyValue& second) {
  auto result = first.swap(&second);
  if (result.first) {
    std::cout << "Swap successful\n";
  } else {
    std::cerr << "Swap failed: " << result.second << "\n";
  }
}
```

## Functions

### `overlay<OVL, T>(value_t& parent, object_t::key_type const& self_key) -> std::unique_ptr<OVL>`

**Brief:** Factory function that creates an overlay object for a field within a parent JSON object. If the field does not exist, it is created with a default value of type T.

**Template Parameters:**
- `OVL` - The overlay class to create (e.g., `ovlDocument`, `ovlBookkeeping`)
- `T` - The expected JSON type for the field (default: `object_t`)

**Parameters:**
- `parent` - Parent JSON object containing the target field
- `self_key` - Key name of the field within the parent

**Preconditions:**
- `self_key` must not be empty
- `parent` must be a JSON object (type_t::OBJECT)

**Returns:** `std::unique_ptr<OVL>` to the created overlay

**Postconditions:**
- Field exists in parent (created with default value if missing)
- Returned overlay wraps the field
- Overlay is valid for the lifetime of the parent JSON

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` | If `self_key` is empty |
| `std::runtime_error` | If `parent` is not a JSON object |

**Thread Safety:** Unsafe

**Side Effects:**
- May modify `parent` by adding a new field if it does not exist

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlKeyValue.h"
#include "artdaq-database/Overlay/ovlBookkeeping.h"
#include <iostream>

using namespace artdaq::database::overlay;

void createOverlayWithFactory(value_t& recordJson) {
  try {
    // Creates "bookkeeping" field if missing, returns overlay
    auto bookkeepingOverlay = overlay<ovlBookkeeping>(recordJson, "bookkeeping");

    // Now bookkeepingOverlay wraps recordJson["bookkeeping"]
    if (!bookkeepingOverlay->isReadonly()) {
      std::cout << "Record is modifiable\n";
    }
  } catch (const std::runtime_error& e) {
    std::cerr << "Failed to create overlay: " << e.what() << "\n";
  }
}

void createStringOverlay(value_t& recordJson) {
  try {
    // For string fields, specify std::string as the second template parameter
    auto changelogOverlay = overlay<ovlChangeLog, std::string>(recordJson, "changelog");
    std::cout << "Changelog: " << changelogOverlay->buffer() << "\n";
  } catch (const std::runtime_error& e) {
    std::cerr << "Failed to create overlay: " << e.what() << "\n";
  }
}
```

## Type Aliases

### `ovlKeyValueUPtr_t`

```cpp
using ovlKeyValueUPtr_t = std::unique_ptr<ovlKeyValue>;
```

**Brief:** Convenience type alias for unique pointer to ovlKeyValue. Used for managing overlay lifetime through smart pointers.

## Relationship to Other Components

### Class Hierarchy

```
ovlKeyValue (this class - base)
     ^
     |
     +-- ovlBookkeeping
     +-- ovlChangeLog
     +-- ovlComment
     +-- ovlDocument
     +-- ovlId
     +-- ovlOrigin
     +-- ovlTimeStamp
     +-- ovlUpdate
     +-- ovlKeyValueTimeStamp<>
     +-- ovlKeyValueWithDefault<>
     +-- ovlKeyValueWithMask<>
     +-- ovlStringKeyValue<>
     +-- ovlFixedList<>
     +-- ovlMovableList<>
     +-- ovlDatabaseRecord
```

### Usage Pattern

All derived classes use `ovlKeyValue` as their base:
```cpp
class ovlDocument : public ovlKeyValue {
 public:
  ovlDocument(object_t::key_type const& key, value_t& document)
    : ovlKeyValue(key, document),
      _data{overlay<ovlData>(document, "data")},
      _metadata{overlay<ovlMetadata>(document, "metadata")}
  {}
};
```

## See Also

- [ovlKeyValue.cpp](./ovlKeyValue.cpp.md) - Implementation file
- [common.h](./common.h.md) - Module foundation types
- [ovlBookkeeping.h](./ovlBookkeeping.h.md) - Derived class example
- [ovlDatabaseRecord.h](./ovlDatabaseRecord.h.md) - Root overlay class

## Notes for Developers

### Lifetime Management

```cpp
#include "artdaq-database/Overlay/ovlKeyValue.h"

using namespace artdaq::database::overlay;

// BAD: JSON destroyed before overlay
ovlKeyValue createOverlayBad() {
  value_t json = object_t{};  // Local variable
  return ovlKeyValue{"key", json};  // json destroyed on return!
}

// GOOD: JSON outlives overlay
void useOverlayGood(value_t& json) {
  ovlKeyValue overlay{"key", json};
  // Use overlay...
  // overlay destroyed first, json still valid
}

// GOOD: Store JSON and overlay together
struct DocumentHolder {
  value_t json;
  std::unique_ptr<ovlKeyValue> overlay;

  DocumentHolder() : json(object_t{}) {
    overlay = std::make_unique<ovlKeyValue>("key", json);
  }
};
```

### Performance Considerations

- **Value access:** O(1) reference access - no copying
- **Type casting:** No copies, just reference reinterpretation
- **Comparison:** O(n) in data size - deep JSON comparison
- **to_string():** O(n) - creates temporary JSON object and serializes

### Common Pitfalls

- **Lifetime:** Overlay must not outlive the JSON it references. This is the most common source of bugs.
- **Type Mismatch:** Using `value_as<T>` with wrong type causes `std::bad_cast` exception
- **Empty Key:** Factory function throws if key is empty
- **Null Pointer:** `swap()` returns failure if passed null pointer

### Anti-patterns

```cpp
// DON'T: Returning overlay to local JSON
ovlKeyValue bad() {
  value_t local = object_t{};
  return ovlKeyValue{"k", local};  // local dies after return!
}

// DO: Accept JSON by reference
void good(value_t& external) {
  ovlKeyValue overlay{"k", external};
  // Safe - external outlives overlay
}

// DON'T: Assume type without checking
auto& val = wrapper.value_as<int>("field");  // May throw if not int

// DO: Handle type errors
try {
  auto& val = wrapper.value_as<int>("field");
  // use val
} catch (const std::bad_cast& e) {
  // handle type mismatch
}
```

### Why Virtual Destructor?

```cpp
virtual ~ovlKeyValue() = default;
```

Allows proper cleanup when deleting derived classes through base pointer:
```cpp
ovlKeyValue* ptr = new ovlDocument(...);
delete ptr;  // Calls ovlDocument destructor correctly due to virtual
```

### Why result_t for Comparison?

Standard `operator==` returns `bool`, but `result_t` provides:
- Success/failure status
- Detailed error messages explaining the difference
- Composable comparisons across overlay hierarchies
- Better debugging with `result.second` containing difference details

This enables callers to understand why two overlays differ, not just that they differ.
