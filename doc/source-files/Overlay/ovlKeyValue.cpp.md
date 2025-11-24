# ovlKeyValue.cpp

## File Overview

This implementation file provides the concrete implementations for the `ovlKeyValue` base class methods and the global comparison mask utility. It handles JSON value access, serialization, comparison operations, and swap functionality.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/ovlKeyValue.cpp`

## Implementation Highlights

### Global Comparison Mask Function

```cpp
std::uint32_t artdaq::database::overlay::useCompareMask(std::uint32_t compareMask) {
  static std::uint32_t _compareMask = compareMask;
  return _compareMask;
}
```

**Purpose**: Manages a module-wide comparison mask using a static variable.

**Behavior**:
- First call initializes the static mask with the provided value
- Subsequent calls return the current mask value
- Default argument (0) allows querying without modification

**Thread Safety**: Not thread-safe; uses function-local static.

### JSON Serialization

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

**Algorithm**:
1. Creates temporary JSON object
2. Assigns key-value pair
3. Uses JsonWriter to serialize
4. Returns JSON string or error message

### Value Accessors

```cpp
value_t& ovlKeyValue::value(object_t::key_type const& key) {
  return objectValue(key);
}

value_t& ovlKeyValue::objectValue(object_t::key_type const& key) {
  using artdaq::database::sharedtypes::unwrap;
  return unwrap(_value).value<object_t>(key);
}
```

**Key Function**: `unwrap()` handles both wrapped and raw JSON values, providing uniform access.

### Array and Object Access

```cpp
array_t& ovlKeyValue::arrayValue() {
  return unwrap(_value).value_as<array_t>();
}

object_t& ovlKeyValue::objectValue() {
  return unwrap(_value).value_as<object_t>();
}
```

Direct type access assumes the value has the correct type.

### String Access

```cpp
std::string& ovlKeyValue::objectStringValue() {
  return unwrap(_value).value_as<std::string>();
}

std::string const& ovlKeyValue::objectStringValue() const {
  return unwrap(_value).value_as<std::string>();
}
```

Provides both mutable and const access to string values.

### Comparison Operator

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

**Comparison Logic**:
1. Check if keys match (must be identical)
2. Delegate to JSON comparison operator
3. If different, generate detailed error message with both values

### Swap Operation

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

**Safety**: Checks for null pointer and catches exceptions.

## Related Files

- **ovlKeyValue.h** - Class declaration
- **common.h** - Type definitions and constants used

## Notes

- Uses `unwrap()` consistently for safe JSON value access
- Provides detailed error messages for debugging
- Exception-safe swap operation
- Minimal implementation focusing on delegation to JSON utilities
