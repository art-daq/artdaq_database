# ovlKeyValue.cpp

**Path:** `artdaq-database/Overlay/ovlKeyValue.cpp`

**Implements:** [ovlKeyValue.h](./ovlKeyValue.h.md)

**Purpose:** Implementation file for the ovlKeyValue base class, providing the constructor, value accessor implementations, serialization, comparison operator, swap operation, and the global comparison mask management function.

## Implementation Overview

This file implements the core functionality for the overlay base class. Key aspects include:

1. **Global Comparison Mask** - A static variable that controls which document components are compared
2. **Value Access Delegation** - All value access methods delegate to the `unwrap()` utility
3. **JSON Serialization** - The `to_string()` method creates temporary JSON and serializes it
4. **Deep Comparison** - The equality operator performs deep JSON value comparison

## Key Algorithms

### Comparison Mask Management

The `useCompareMask()` function manages a static variable for comparison masking:

```cpp
std::uint32_t useCompareMask(std::uint32_t compareMask) {
  static std::uint32_t _compareMask = compareMask;
  return _compareMask;
}
```

**Brief:** Gets or sets the global comparison mask that controls overlay comparisons.

**Steps:**
1. Static variable is initialized with the first call's argument
2. Subsequent calls return the stored value without modification
3. Default parameter (0) allows querying without changing

**Why this approach:** This provides a simple global state mechanism that does not require threading support or complex initialization. The trade-off is that the mask cannot be changed after first initialization.

**Thread Safety Note:** C++11 guarantees thread-safe initialization of static local variables. However, subsequent reads are not synchronized, so concurrent access from multiple threads is unsafe.

### JSON Serialization

The `to_string()` method serializes the key-value pair:

**Brief:** Serializes key-value pair to JSON string format.

**Steps:**
1. Create a temporary JSON object
2. Assign the key-value pair to the temporary object
3. Use JsonWriter to serialize to string
4. Return the JSON string or error message on failure

**Implementation:**
```cpp
std::string ovlKeyValue::to_string() const {
  auto retValue = std::string{};
  auto tmpAST = object_t{};

  tmpAST[_key] = _value;

  using artdaq::database::json::JsonWriter;

  if (JsonWriter().write(tmpAST, retValue)) {
    return retValue;
  }
  { return msg_ConvertionError; }
}
```

### Deep Value Comparison

The comparison operator performs deep JSON comparison with detailed error reporting:

**Brief:** Compares two overlays for equality with detailed error reporting.

**Steps:**
1. Compare keys for exact match
2. Delegate to `artdaq::database::json::operator==` for value comparison
3. Generate detailed error message if values differ
4. Return result_t with success/failure and message

**Implementation:**
```cpp
result_t ovlKeyValue::operator==(ovlKeyValue const& other) const {
  if (_key != other._key) {
    return {false, "Keys are different: self,other=" + _key + "," + other._key + "."};
  }

  auto result = artdaq::database::json::operator==(_value, other._value);

  if (result.first) {
    return result;
  }

  using artdaq::database::json::print_visitor;

  std::ostringstream oss;
  oss << "\n  Values disagree.";
  oss << "\n  Key: " << quoted_(_key);
  oss << "\n  Self  value: " << to_string();
  oss << "\n  Other value: " << other.to_string();

  return Failure(oss);
}
```

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/Overlay/ovlKeyValue.h` | Class declaration and template methods |
| `<utility>` | `std::move` for constructor efficiency |

## Internal Functions

### `useCompareMask(std::uint32_t) -> std::uint32_t`

**Brief:** Gets or sets the global comparison mask that controls overlay comparisons. The mask is initialized on first call and cannot be changed afterward.

**Called by:** All overlay `operator==` implementations

**Purpose:** Provides a single point of control for comparison behavior across all overlay types.

**Parameters:**
- `compareMask` - Value to initialize the mask (only used on first call, default: 0)

**Returns:** The current comparison mask value

**Thread Safety:** Initialization is thread-safe (C++11), but subsequent reads are not synchronized

---

### Constructor

```cpp
ovlKeyValue::ovlKeyValue(object_t::key_type key, value_t& value)
    : _key(std::move(key)), _value(value) {}
```

**Brief:** Constructs overlay by moving key and storing value reference.

**Called by:** Derived class constructors

**Purpose:** Efficiently stores key (by move) and value (by reference).

**Parameters:**
- `key` - JSON key (moved into storage)
- `value` - Reference to JSON value (stored by reference)

**Thread Safety:** Unsafe

---

### `to_string() const -> std::string`

**Brief:** Serializes key-value pair to JSON string format.

**Called by:** Client code for debugging and comparison error messages

**Purpose:** Provides human-readable representation of the overlay content.

**Returns:** JSON string or `msg_ConvertionError` on failure

**Thread Safety:** Unsafe

---

### `value(object_t::key_type const& key) -> value_t&`

**Brief:** Retrieves a nested value by key, delegating to objectValue.

**Called by:** Client code for accessing nested JSON fields

**Purpose:** Provides access to nested object fields.

**Parameters:**
- `key` - Key of the nested value to retrieve

**Returns:** Reference to the nested JSON value

**Throws:** `std::runtime_error` if key does not exist, `std::bad_cast` if not an object

**Thread Safety:** Unsafe

---

### `array_value() -> array_t&`

**Brief:** Returns the wrapped value as a JSON array.

**Called by:** Client code when value is an array type

**Purpose:** Provides type-safe access to array values.

**Returns:** Reference to the array

**Throws:** `std::bad_cast` if value is not an array

**Thread Safety:** Unsafe

---

### `object_value() -> object_t&`

**Brief:** Returns the wrapped value as a JSON object.

**Called by:** Client code when value is an object type

**Purpose:** Provides type-safe access to object values.

**Returns:** Reference to the object

**Throws:** `std::bad_cast` if value is not an object

**Thread Safety:** Unsafe

---

### `key() -> object_t::key_type&`

**Brief:** Returns mutable reference to the overlay's key.

**Called by:** Client code needing to access or modify the key

**Purpose:** Provides direct access to the key.

**Returns:** Reference to the key string

**Thread Safety:** Unsafe

---

### `key() const -> object_t::key_type const&`

**Brief:** Returns const reference to the overlay's key.

**Called by:** Client code needing read-only key access

**Purpose:** Provides const-correct key access.

**Returns:** Const reference to the key string

**Thread Safety:** Unsafe

---

### `value() -> value_t&`

**Brief:** Returns mutable reference to the underlying JSON value.

**Called by:** Client code needing direct JSON access

**Purpose:** Provides direct access to the wrapped value.

**Returns:** Reference to the JSON value

**Thread Safety:** Unsafe

---

### `value() const -> value_t const&`

**Brief:** Returns const reference to the underlying JSON value.

**Called by:** Client code needing read-only value access

**Purpose:** Provides const-correct value access.

**Returns:** Const reference to the JSON value

**Thread Safety:** Unsafe

---

### `string_value() -> std::string&`

**Brief:** Returns the wrapped value as a string.

**Called by:** Client code when value is a string type

**Purpose:** Provides type-safe string access.

**Returns:** Reference to the string

**Throws:** `std::bad_cast` if value is not a string

**Thread Safety:** Unsafe

---

### `string_value() const -> std::string const&`

**Brief:** Returns const reference to the wrapped value as a string.

**Called by:** Client code needing read-only string access

**Purpose:** Provides const-correct string access.

**Returns:** Const reference to the string

**Throws:** `std::bad_cast` if value is not a string

**Thread Safety:** Unsafe

---

### `self() -> ovlKeyValue&`

**Brief:** Returns reference to this overlay object.

**Called by:** Comparison operations in derived classes

**Purpose:** Enables base class comparison from derived types.

**Returns:** Reference to `*this`

**Thread Safety:** Unsafe

---

### `self() const -> ovlKeyValue const&`

**Brief:** Returns const reference to this overlay object.

**Called by:** Const comparison operations in derived classes

**Purpose:** Enables const base class comparison from derived types.

**Returns:** Const reference to `*this`

**Thread Safety:** Unsafe

---

### `objectValue(object_t::key_type const& key) -> value_t&`

**Brief:** Internal helper that retrieves a nested value by key using unwrap.

**Called by:** `value()` public method

**Purpose:** Encapsulates the unwrap call for consistent value access.

**Parameters:**
- `key` - Key of the nested value

**Returns:** Reference to the nested value

**Thread Safety:** Unsafe

---

### `arrayValue() -> array_t&`

**Brief:** Internal helper that retrieves the value as an array.

**Called by:** `array_value()` public method

**Purpose:** Provides type-safe array access through unwrap.

**Returns:** Reference to the array

**Thread Safety:** Unsafe

---

### `objectValue() -> object_t&`

**Brief:** Internal helper that retrieves the value as an object.

**Called by:** `object_value()` public method

**Purpose:** Provides type-safe object access through unwrap.

**Returns:** Reference to the object

**Thread Safety:** Unsafe

---

### `objectStringValue() -> std::string&`

**Brief:** Internal helper that retrieves the value as a string.

**Called by:** `string_value()` public methods

**Purpose:** Provides type-safe string access through unwrap.

**Returns:** Reference to the string

**Thread Safety:** Unsafe

---

### `objectStringValue() const -> std::string const&`

**Brief:** Internal const helper that retrieves the value as a string.

**Called by:** `string_value() const` public method

**Purpose:** Provides const-correct string access through unwrap.

**Returns:** Const reference to the string

**Thread Safety:** Unsafe

---

### `operator==(ovlKeyValue const& other) const -> result_t`

**Brief:** Compares two overlays for equality with detailed error reporting.

**Called by:** Derived class comparison operators and client code

**Purpose:** Provides deep comparison with human-readable difference descriptions.

**Parameters:**
- `other` - The overlay to compare against

**Returns:** result_t with success/failure and difference details

**Thread Safety:** Unsafe

---

### `swap(ovlKeyValue* other) -> result_t`

**Brief:** Swaps key and value with another overlay, with exception safety.

**Called by:** Client code for reordering operations

**Purpose:** Enables safe swapping of overlay contents.

**Parameters:**
- `other` - Pointer to the overlay to swap with (must not be null)

**Returns:** result_t with success/failure status

**Thread Safety:** Unsafe

**Implementation:**
```cpp
result_t ovlKeyValue::swap(ovlKeyValue* other) try {
  if (other == nullptr) {
    return Failure(msg_InvalidArgument);
  }

  _key.swap(other->_key);
  _value.swap(other->_value);

  return Success();
} catch (...) {
  return Failure();
}
```

## Performance Considerations

- **Value Access:** O(1) - simple reference forwarding through unwrap
- **to_string():** O(n) - creates temporary JSON object and serializes entire value
- **Comparison:** O(n) - deep comparison of JSON structures
- **Constructor:** O(1) - just stores key (moved) and value reference

## Error Handling Strategy

- **Constructor:** No error handling - assumes valid input
- **Value Access:** Relies on unwrap to handle type mismatches (throws `std::bad_cast`)
- **to_string():** Returns `msg_ConvertionError` constant on serialization failure instead of throwing
- **swap():** Uses function-try-catch to convert exceptions to Failure result
- **Comparison:** Builds detailed error message describing differences, does not throw

## Testing Notes

- **Unit tests:** `test/Overlay/ovlKeyValue_t.cc`
- **Key test cases:**
  - Constructor with valid/invalid values
  - Value access with correct/incorrect types
  - Comparison of equal/different overlays
  - Swap operation with valid/null pointers
  - Serialization success/failure scenarios

## Maintenance Notes

### Template Methods Location

Template methods (`value_as<T>`, `objectValue<T>`) are defined in the header file due to C++ template instantiation rules. The compiler needs to see the template definition at each instantiation point.

### Static Mask Initialization

The `useCompareMask()` function uses C++ static local variable initialization semantics:
- Initialization happens on first call with the passed argument
- The initialization is thread-safe in C++11 and later
- However, subsequent reads and writes are not synchronized
- **Important:** The mask value cannot be changed after first initialization

### The unwrap() Function

All value access methods use `artdaq::database::sharedtypes::unwrap()` consistently:
- Handles both wrapped and raw JSON values uniformly
- Provides consistent interface regardless of storage details
- Defined in `artdaq-database/DataFormats/Json/json_types_impl.h`

### Error Message Generation

The comparison operator builds detailed error messages:
```cpp
std::ostringstream oss;
oss << "\n  Values disagree.";
oss << "\n  Key: " << quoted_(_key);
oss << "\n  Self  value: " << to_string();
oss << "\n  Other value: " << other.to_string();
```

This allows debugging comparison failures by showing both values. The `quoted_()` helper ensures proper JSON string escaping.

### Exception Safety in swap()

The swap method uses a function-try-block pattern:
```cpp
result_t ovlKeyValue::swap(ovlKeyValue* other) try {
  // ... implementation ...
} catch (...) {
  return Failure();
}
```

This ensures:
1. Null pointer check happens first
2. Any exception during swap is caught
3. Failure is returned instead of propagating exception
4. Strong exception guarantee is maintained (either both swap or neither)

## See Also

- [ovlKeyValue.h](./ovlKeyValue.h.md) - Class declaration and template methods
- [common.h](./common.h.md) - Types, constants, and comparison flags
- [json_types_impl.h](../DataFormats/Json/json_types_impl.h.md) - The unwrap() utility
