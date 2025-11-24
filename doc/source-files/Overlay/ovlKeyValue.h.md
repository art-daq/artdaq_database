# ovlKeyValue.h

## File Overview

This header defines `ovlKeyValue`, the fundamental base class for all overlay objects in the Overlay module. It provides the core abstraction for wrapping JSON key-value pairs with type-safe C++ accessors, enabling the overlay pattern where C++ objects provide convenient access to underlying JSON data structures without data duplication.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/ovlKeyValue.h`

## Purpose

`ovlKeyValue` serves as the foundation for the entire Overlay module by:

1. **Wrapping JSON Data** - Provides object-oriented access to JSON values
2. **Reference Semantics** - Maintains references to JSON AST, enabling zero-copy operations
3. **Type-Safe Accessors** - Template methods for accessing typed values
4. **Comparison Operations** - Implements deep equality comparison
5. **Serialization** - Converts overlay objects back to JSON strings
6. **Swap Operations** - Enables efficient value exchange

## Class Definition

```cpp
class ovlKeyValue {
 public:
  ovlKeyValue(object_t::key_type /*key*/, value_t& /*value*/);

  // defaults
  ovlKeyValue(ovlKeyValue&&) = default;
  virtual ~ovlKeyValue() = default;

  // Value accessors
  value_t& value(object_t::key_type const& /*key*/);

  template <typename T>
  T& value_as(object_t::key_type const& /*key*/);

  template <typename T>
  T const& value_as(object_t::key_type const& /*key*/) const;

  // Direct accessors
  array_t& array_value();
  object_t& object_value();
  object_t::key_type& key();
  object_t::key_type const& key() const;
  value_t& value();
  value_t const& value() const;
  std::string& string_value();
  std::string const& string_value() const;

  // Self reference
  ovlKeyValue const& self() const;
  ovlKeyValue& self();

  // virtuals
  virtual std::string to_string() const;

  // ops
  result_t operator==(ovlKeyValue const&) const;
  result_t swap(ovlKeyValue*);

 private:
  object_t::key_type _key;
  value_t& _value;
};
```

## Core Concepts

### Reference-Based Design

The class stores a **reference** to JSON data, not a copy:

```cpp
private:
  object_t::key_type _key;   // The JSON key (stored by value)
  value_t& _value;            // Reference to JSON value (NOT a copy!)
```

**Implications**:
- Changes through overlay object modify the underlying JSON
- Overlay object lifetime must not exceed JSON data lifetime
- No data duplication - memory efficient
- Multiple overlays can reference the same JSON

### Template Accessor Methods

```cpp
template <typename T>
T& value_as(object_t::key_type const& key);
```

Type-safe access to nested values:

```cpp
// Access string value
std::string& name = overlay.value_as<std::string>("name");

// Access integer value
integer& count = overlay.value_as<integer>("count");

// Access boolean value
bool& flag = overlay.value_as<bool>("enabled");
```

## Public Interface

### Construction

```cpp
ovlKeyValue(object_t::key_type key, value_t& value);
```

**Parameters**:
- `key` - The JSON key this overlay represents
- `value` - Reference to the JSON value

**Example**:
```cpp
object_t jsonObj;
jsonObj["metadata"] = object_t{};
auto& metadataValue = jsonObj.at("metadata");

ovlKeyValue overlay("metadata", metadataValue);
```

### Value Access Methods

#### Generic Value Access
```cpp
value_t& value(object_t::key_type const& key);
```
Access nested value by key (returns JSON variant type).

#### Typed Value Access
```cpp
template <typename T>
T& value_as(object_t::key_type const& key);

template <typename T>
T const& value_as(object_t::key_type const& key) const;
```

Access nested value with automatic type casting.

**Supported Types**:
- `std::string` - String values
- `integer` - Integer values
- `bool` - Boolean values
- `object_t` - Nested objects
- `array_t` - Arrays

#### Direct Type Access

```cpp
array_t& array_value();          // Treat value as array
object_t& object_value();        // Treat value as object
std::string& string_value();     // Treat value as string
```

These assume the value is of the specified type.

#### Key and Value Access

```cpp
object_t::key_type& key();              // Get the key
object_t::key_type const& key() const;
value_t& value();                        // Get the value reference
value_t const& value() const;
```

### Self Reference

```cpp
ovlKeyValue const& self() const;
ovlKeyValue& self();
```

Returns reference to the object itself. Used for:
- Comparison operations in derived classes
- Chaining operations
- Type erasure patterns

### Serialization

```cpp
virtual std::string to_string() const;
```

Converts the overlay back to JSON string representation.

**Example Output**:
```json
{"metadata": {"name": "config1", "version": "1.0"}}
```

### Comparison

```cpp
result_t operator==(ovlKeyValue const& other) const;
```

Deep comparison of overlay objects:
- Compares keys (must match)
- Compares underlying JSON values
- Returns `result_t` pair: {success: bool, message: string}

**Example**:
```cpp
result_t result = overlay1 == overlay2;
if (result.first) {
  // Equal
} else {
  std::cout << "Difference: " << result.second << std::endl;
}
```

### Swap Operation

```cpp
result_t swap(ovlKeyValue* other);
```

Swaps the key and value references with another overlay.

**Use Case**: Efficient replacement of one overlay's data with another's.

## Overlay Factory Function

The header provides a template factory function for creating overlay objects:

```cpp
template <typename OVL, typename T = object_t>
std::unique_ptr<OVL> overlay(value_t& parent,
                              object_t::key_type const& self_key);
```

**Purpose**: Creates an overlay of specified type, initializing it with default values if needed.

**Template Parameters**:
- `OVL` - The overlay class to create (e.g., `ovlDocument`, `ovlBookkeeping`)
- `T` - The expected JSON type (default: `object_t`)

**Parameters**:
- `parent` - Parent JSON object containing the target field
- `self_key` - Key name within parent

**Example**:
```cpp
value_t recordJson = /* ... */;

// Create document overlay
auto document = overlay<ovlDocument>(recordJson, "document");

// Create bookkeeping overlay
auto bookkeeping = overlay<ovlBookkeeping>(recordJson, "bookkeeping");
```

**Behavior**:
- Checks that parent is an object
- Uses `unwrap()` to access the underlying JSON structure
- Calls the template method `value<object_t, T>(self_key)` which creates the field if it doesn't exist
- Constructs the overlay object with the key and value reference
- Returns a unique_ptr to the overlay

## Implementation Details (from .cpp)

### Comparison Mask Management

```cpp
std::uint32_t useCompareMask(std::uint32_t compareMask);
```

Manages a static comparison mask used globally:

```cpp
static std::uint32_t _compareMask = compareMask;
return _compareMask;
```

### String Conversion

```cpp
std::string ovlKeyValue::to_string() const {
  auto tmpAST = object_t{};
  tmpAST[_key] = _value;

  using artdaq::database::json::JsonWriter;
  std::string retValue;

  if (JsonWriter().write(tmpAST, retValue)) {
    return retValue;
  }
  return msg_ConvertionError;
}
```

Creates a temporary JSON object and serializes it.

### Value Access Implementation

```cpp
value_t& ovlKeyValue::value(object_t::key_type const& key) {
  return objectValue(key);
}

value_t& ovlKeyValue::objectValue(object_t::key_type const& key) {
  using artdaq::database::sharedtypes::unwrap;
  return unwrap(_value).value<object_t>(key);
}
```

Uses `unwrap()` utility to access the underlying JSON structure.

### Comparison Implementation

```cpp
result_t ovlKeyValue::operator==(ovlKeyValue const& other) const {
  if (_key != other._key) {
    return {false, "Keys are different: self,other=" + _key + "," + other._key + "."};
  }

  auto result = artdaq::database::json::operator==(_value, other._value);

  if (result.first) {
    return result;
  }

  std::ostringstream oss;
  oss << "\n  Values disagree.";
  oss << "\n  Key: " << quoted_(_key);
  oss << "\n  Self  value: " << to_string();
  oss << "\n  Other value: " << other.to_string();

  return Failure(oss);
}
```

Provides detailed error messages showing where values differ.

## Usage Patterns

### Basic Wrapper Creation

```cpp
object_t config;
config["metadata"] = object_t{};
config["metadata"]["name"] = "myconfig";

ovlKeyValue wrapper("metadata", config.at("metadata"));
std::string& name = wrapper.value_as<std::string>("name");
name = "updated_config";  // Modifies underlying JSON
```

### Derived Class Pattern

Most overlay classes derive from `ovlKeyValue`:

```cpp
class ovlDocument : public ovlKeyValue {
 public:
  ovlDocument(object_t::key_type const& key, value_t& document)
    : ovlKeyValue(key, document),
      _data{overlay<ovlData>(document, "data")},
      _metadata{overlay<ovlMetadata>(document, "metadata")}
  {}

 private:
  ovlDataUPtr_t _data;
  ovlMetadataUPtr_t _metadata;
};
```

### Comparison with Masking

```cpp
// Set mask to ignore timestamps
useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS);

result_t result = overlay1 == overlay2;
// Comparison will ignore timestamp differences
```

## Design Rationale

### Why References?

**Reference-based design** provides:
1. **Zero Copy** - No data duplication
2. **Direct Modification** - Changes affect original JSON
3. **Memory Efficiency** - Multiple overlays can reference same data
4. **Consistency** - All overlays see the same data

### Why Virtual Destructor?

```cpp
virtual ~ovlKeyValue() = default;
```

Allows proper cleanup when deleting derived classes through base pointer:

```cpp
ovlKeyValue* ptr = new ovlDocument(...);
delete ptr;  // Calls ovlDocument destructor
```

### Why result_t for Comparison?

Standard `operator==` returns `bool`, but `result_t` provides:
- Success/failure status
- Detailed error messages
- Composable comparisons
- Better debugging

## Thread Safety

**Not thread-safe**:
- References mutable JSON data
- Static comparison mask
- No internal synchronization

**Thread-Safety Recommendations**:
- Use one overlay per thread
- Don't share overlays across threads
- Synchronize access to underlying JSON

## Performance Considerations

### Efficient Operations
- **Value access**: O(1) reference access
- **Type casting**: No copies, just reinterpret
- **Comparison**: O(n) in data size

### Expensive Operations
- **to_string()**: Requires JSON serialization
- **Deep comparison**: Must traverse entire structure

### Optimization Tips
- Cache overlay objects when accessing same data repeatedly
- Use direct accessors (`value_as`) when type is known
- Minimize `to_string()` calls (only for debugging/serialization)

## Related Files

- **common.h** - Defines types and constants used by ovlKeyValue
- **ovlKeyValue.cpp** - Implementation of methods
- **ovlDocument.h** - Example derived class
- **ovlBookkeeping.h** - Another derived class
- **DataFormats/Json/json_types_impl.h** - Underlying JSON type system

## Best Practices

1. **Lifetime Management** - Ensure JSON outlives all overlay objects
2. **Const Correctness** - Use const accessors when not modifying
3. **Type Safety** - Use `value_as<T>()` with correct type
4. **Error Checking** - Check result_t values from operations
5. **Virtual Methods** - Override `to_string()` in derived classes for better output
6. **Move Semantics** - Use move constructor (defaulted) for efficiency

## Common Pitfalls

### Dangling References
```cpp
// BAD: JSON destroyed before overlay
ovlKeyValue createOverlay() {
  value_t json = parseJSON("...");
  return ovlKeyValue("key", json);  // json destroyed!
}
```

### Wrong Type Access
```cpp
// BAD: Value is string, not integer
integer& count = overlay.value_as<integer>("name");  // Runtime error
```

### Comparison Without Mask Setup
```cpp
// MAY BE PROBLEMATIC: Uses default mask (0)
result_t result = overlay1 == overlay2;
// Better: Set mask explicitly
useCompareMask(desired_mask);
result = overlay1 == overlay2;
```

## Notes

- This is a polymorphic base class (virtual destructor)
- All derived classes use move semantics (default move constructor)
- The class is designed for short-lived objects wrapping long-lived JSON
- Comparison operator returns result_t, not bool (non-standard but more informative)
- The unwrap() utility handles both wrapped and unwrapped JSON values
