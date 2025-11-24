# ovlTimeStamp.h

## File Overview

Overlay class for ISO8601 timestamp strings stored in JSON. Provides validation, accessor methods, and comparison support with masking capability for timestamp fields.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/ovlTimeStamp.h`

## Purpose

Wraps timestamp string values, ensuring they conform to ISO8601 format and providing selective comparison through the global timestamp mask.

## Class Definition

```cpp
class ovlTimeStamp final : public ovlKeyValue {
 public:
  ovlTimeStamp(object_t::key_type const& key, value_t& ts);

  // defaults
  ovlTimeStamp(ovlTimeStamp&&) = default;
  ~ovlTimeStamp() = default;

  // accessors
  std::string& timestamp();
  std::string const& timestamp() const;
  std::string& timestamp(std::string const& ts);

  // overrides
  std::string to_string() const override;

  // ops
  result_t operator==(ovlTimeStamp const&) const;
};
```

## Implementation Details

### Constructor (from .cpp)
```cpp
ovlTimeStamp::ovlTimeStamp(object_t::key_type const& key, value_t& ts)
    : ovlKeyValue(key, (ts = confirm_iso8601_timestamp(unwrap(ts).value_as<std::string>()), ts)) {}
```

**Behavior**: Validates timestamp on construction, ensuring ISO8601 format compliance.

### Accessors
```cpp
std::string& timestamp() { return string_value(); }
std::string const& timestamp() const { return string_value(); }

std::string& timestamp(std::string const& ts) {
  confirm(!ts.empty());
  timestamp() = ts;
  return timestamp();
}
```

### Serialization
```cpp
std::string to_string() const {
  std::ostringstream oss;
  oss << "{" << quoted_(key()) << ":" << quoted_(timestamp()) << "}";
  return oss.str();
}
```

**Example Output**: `{"created": "2025-01-15T10:30:00Z"}`

### Comparison
```cpp
result_t operator==(ovlTimeStamp const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_TIMESTAMPS) == DOCUMENT_COMPARE_MUTE_TIMESTAMPS)
         ? Success()
         : self() == other.self();
}
```

**Masking**: If `DOCUMENT_COMPARE_MUTE_TIMESTAMPS` is set, all timestamps are considered equal.

## Usage Examples

### Creating Timestamps
```cpp
value_t json;
json["created"] = artdaq::database::timestamp();  // Current time

ovlTimeStamp created("created", json.at("created"));
std::cout << created.timestamp() << std::endl;
```

### Updating Timestamps
```cpp
ovlTimeStamp modified("modified", json.at("modified"));
modified.timestamp(artdaq::database::timestamp());  // Update to now
```

### Comparison with Masking
```cpp
ovlTimeStamp ts1("created", json1.at("created"));
ovlTimeStamp ts2("created", json2.at("created"));

// Without mask: compares actual timestamp values
useCompareMask(0);
auto result = ts1 == ts2;  // Likely false if created at different times

// With mask: ignores timestamp differences
useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS);
result = ts1 == ts2;  // Always true (timestamps masked)
```

## Related Files

- **ovlKeyValue.h** - Base class
- **ovlTimeStamp.cpp** - Implementation
- **common.h** - DOCUMENT_COMPARE_MUTE_TIMESTAMPS constant
- **ovlBookkeeping.h** - Uses for tracking record creation
- **ovlOrigin.h** - Uses for document creation tracking

## Notes

- Validates ISO8601 format on construction
- Comparison respects global timestamp mask
- Used throughout module for temporal tracking
- Final class (cannot be derived from)
