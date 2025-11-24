# ovlTimeStamp.cpp

## File Overview

Implementation of the `ovlTimeStamp` class providing ISO8601 timestamp validation and accessor methods.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/ovlTimeStamp.cpp`

## Key Implementation

### Constructor with Validation
```cpp
ovlTimeStamp::ovlTimeStamp(object_t::key_type const& key, value_t& ts)
    : ovlKeyValue(key, (ts = confirm_iso8601_timestamp(unwrap(ts).value_as<std::string>()), ts)) {}
```

**Validation Flow**:
1. Unwraps the JSON value
2. Extracts as string
3. Validates ISO8601 format via `confirm_iso8601_timestamp()`
4. Assigns validated value back
5. Passes to base class constructor

**Important**: The comma operator is used to validate then pass the same reference.

### Accessor Methods
```cpp
std::string& ovlTimeStamp::timestamp() { return string_value(); }
std::string const& ovlTimeStamp::timestamp() const { return string_value(); }

std::string& ovlTimeStamp::timestamp(std::string const& ts) {
  confirm(!ts.empty());
  timestamp() = ts;
  return timestamp();
}
```

Simple delegation to base class string accessors with validation.

### Serialization
```cpp
std::string ovlTimeStamp::to_string() const {
  std::ostringstream oss;
  oss << "{" << quoted_(key()) << ":" << quoted_(timestamp()) << "}";
  return oss.str();
}
```

Produces clean JSON representation with quoted string values.

### Comparison with Masking
```cpp
result_t ovlTimeStamp::operator==(ovlTimeStamp const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_TIMESTAMPS) == DOCUMENT_COMPARE_MUTE_TIMESTAMPS)
         ? Success()
         : self() == other.self();
}
```

Checks global mask before delegating to base comparison.

## Related Files

- **ovlTimeStamp.h** - Class declaration
- **common.h** - Mask constants

## Notes

- ISO8601 validation occurs at construction time
- Empty timestamps not allowed (confirmed in setter)
- Comparison respects global timestamp masking
- Simple, focused implementation
