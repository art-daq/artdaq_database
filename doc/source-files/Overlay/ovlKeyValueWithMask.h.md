# ovlKeyValueWithMask.h

## File Overview

A minimal template class that extends `ovlKeyValue` with maskable comparison support. Unlike `ovlKeyValueWithDefault`, this template does not initialize default values, making it suitable for overlaying existing complete JSON structures.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/ovlKeyValueWithMask.h`

## Purpose

Provides comparison masking without default initialization, used for data and metadata fields where values should already exist.

## Class Definition

```cpp
template <std::uint32_t mask>
class ovlKeyValueWithMask : public ovlKeyValue {
 public:
  ovlKeyValueWithMask(object_t::key_type const& key, value_t& object);

  // defaults
  ovlKeyValueWithMask(ovlKeyValueWithMask&&) = default;
  ~ovlKeyValueWithMask() = default;

  // ops
  result_t operator==(ovlKeyValueWithMask const&) const;
};
```

## Implementation

### Constructor
```cpp
template <std::uint32_t mask>
ovlKeyValueWithMask<mask>::ovlKeyValueWithMask(object_t::key_type const& key,
                                                value_t& value)
    : ovlKeyValue(key, value) {}
```
Simple pass-through to base class, no initialization.

### Comparison
```cpp
template <std::uint32_t mask>
result_t ovlKeyValueWithMask<mask>::operator==(ovlKeyValueWithMask const& other) const {
  return ((useCompareMask() & mask) == mask) ? Success() : self() == other.self();
}
```
Checks mask bit and delegates to base comparison if not masked.

## Usage

### Type Aliases in ovlDocument.h
```cpp
using ovlData = ovlKeyValueWithMask<DOCUMENT_COMPARE_MUTE_DATA>;
using ovlMetadata = ovlKeyValueWithMask<DOCUMENT_COMPARE_MUTE_METADATA>;
```

### Example
```cpp
value_t documentJson;
documentJson["data"] = object_t{/* user data */};
documentJson["metadata"] = object_t{/* metadata */};

ovlData data("data", documentJson.at("data"));
ovlMetadata metadata("metadata", documentJson.at("metadata"));

// Compare with mask
useCompareMask(DOCUMENT_COMPARE_MUTE_DATA);
auto result = data == otherData;  // Always succeeds (data masked)
```

## Design Rationale

**No Initialization**: Unlike `ovlKeyValueWithDefault`, this assumes values exist. Used for required fields like data/metadata in documents.

**Template Mask**: Enables compile-time type differentiation and selective masking.

## Related Files

- **ovlKeyValue.h** - Base class
- **ovlDocument.h** - Primary user (ovlData, ovlMetadata)
- **ovlKeyValueWithDefault.h** - Similar but with default initialization
- **common.h** - Mask constants

## Notes

- Header-only template
- No .cpp file needed
- Minimal overhead over base class
- Used primarily for document data and metadata fields
