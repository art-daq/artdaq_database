# ovlStringKeyValue.h

## File Overview

This header-only template class provides a specialized overlay for JSON string values with optional comparison masking. It extends `ovlKeyValue` to handle string-valued fields while supporting selective comparison through template-based mask parameters.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/ovlStringKeyValue.h`

## Purpose

`ovlStringKeyValue` simplifies working with JSON string fields by:

1. **String-Specific Overlay** - Wraps string values with type-safe access
2. **Default Value Initialization** - Provides default "not-provided" value for empty strings
3. **Maskable Comparison** - Template parameter enables selective comparison

## Class Definition

```cpp
template <std::uint32_t mask>
class ovlStringKeyValue : public ovlKeyValue {
 public:
  ovlStringKeyValue(object_t::key_type const& key, value_t& value);

  // defaults
  ovlStringKeyValue(ovlStringKeyValue&&) = default;
  ~ovlStringKeyValue() = default;

  // ops
  result_t operator==(ovlStringKeyValue const&) const;

 private:
  bool init(value_t& parent);
  bool _initOK;
};
```

## Template Parameter

```cpp
template <std::uint32_t mask>
```

**mask**: Comparison flag from `DOCUMENT_COMPARE_FLAGS` enum that determines if this field should be ignored during comparisons.

**Example Usage**:
```cpp
// Version that can be muted in comparisons
using ovlVersion = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_VERSION>;

// Changelog that can be muted
using ovlChangeLog = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_CHANGELOG>;

// Collection that can be muted
using ovlCollection = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_COLLECTION>;
```

## Implementation

### Constructor

```cpp
template <std::uint32_t mask>
ovlStringKeyValue<mask>::ovlStringKeyValue(object_t::key_type const& key,
                                            value_t& value)
    : ovlKeyValue(key, value), _initOK(init(value)) {}
```

**Behavior**:
1. Calls base class constructor
2. Initializes the value with defaults if needed
3. Stores initialization status

### Initialization

```cpp
template <std::uint32_t mask>
bool ovlStringKeyValue<mask>::init(value_t& parent) try {
  confirm(type(parent) == type_t::VALUE);

  auto& value = string_value();

  if (value.empty()) value = std::string{jsonliteral::notprovided};

  return true;
} catch (...) {
  confirm(false);
  throw;
}
```

**Logic**:
1. Confirms the value type is a simple value (not object/array)
2. Gets reference to the string
3. If empty, sets to "not-provided" literal
4. Returns success status

**Safety**: Exception-safe with try-catch and assertions.

### Comparison Operator

```cpp
template <std::uint32_t mask>
result_t ovlStringKeyValue<mask>::operator==(ovlStringKeyValue const& other) const {
  return ((useCompareMask() & mask) == mask) ? Success() : self() == other.self();
}
```

**Behavior**:
- If the global comparison mask has the template mask bit set, returns success (ignores differences)
- Otherwise, delegates to base class comparison (`self() == other.self()`)

## Type Aliases in ovlDatabaseRecord.h

The template is used to create concrete types:

```cpp
// In ovlDatabaseRecord.h
using ovlVersion = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_VERSION>;
using ovlVersionUPtr_t = std::unique_ptr<ovlVersion>;

using ovlCollection = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_COLLECTION>;
using ovlCollectionUPtr_t = std::unique_ptr<ovlCollection>;

using ovlConfigurationType = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_CONFIGURATION>;
using ovlConfigurationTypeUPtr_t = std::unique_ptr<ovlConfigurationType>;
```

Also used in other files:

```cpp
// In ovlChangeLog.h
class ovlChangeLog final : public ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_CHANGELOG> {
  // ...
};

// In ovlOrigin.h
using ovlRawData = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_RAWDATA>;
```

## Usage Examples

### Creating a Version Overlay

```cpp
value_t recordJson;
auto& versionValue = recordJson["version"];

// Initialize with empty string creates "not-provided"
versionValue = std::string{};

ovlVersion version("version", versionValue);
// version.string_value() == "not-provided"

// Set actual version
version.string_value() = "v1_0_0";
```

### Comparison with Masking

```cpp
ovlVersion v1("version", json1.at("version"));
ovlVersion v2("version", json2.at("version"));

v1.string_value() = "v1_0_0";
v2.string_value() = "v2_0_0";

// Without mask: versions are different
useCompareMask(0);
auto result = v1 == v2;
// result.first == false

// With mask: versions ignored
useCompareMask(DOCUMENT_COMPARE_MUTE_VERSION);
result = v1 == v2;
// result.first == true (version differences ignored)
```

### Derived Class Pattern

```cpp
class ovlChangeLog final : public ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_CHANGELOG> {
 public:
  ovlChangeLog(object_t::key_type const& key, value_t& changelog)
    : ovlStringKeyValue(key, changelog) {}

  // Additional methods specific to changelog
  std::string& buffer();
  std::string& append(std::string const& entry);
};
```

## Design Rationale

### Template-Based Masking

**Why template parameter?**
- **Compile-Time Configuration** - Mask is part of the type
- **Type Safety** - Different string fields are different types
- **Zero Runtime Cost** - Mask checking can be optimized by compiler
- **Clear Intent** - Type alias names indicate maskable fields

### Default "not-provided" Value

**Why provide default?**
- **Consistency** - All string fields have a value, never truly empty
- **Validation** - Can distinguish between "not set" and "empty"
- **Debugging** - Clear indication of uninitialized fields

### String Validation

The `init()` method assumes the value is of type `type_t::VALUE` (primitive), not `type_t::OBJECT` or `type_t::ARRAY`.

## Common Use Cases

### Version Fields
```cpp
using ovlVersion = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_VERSION>;
```
Database record version strings that should be ignored when comparing configurations.

### Collection Names
```cpp
using ovlCollection = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_COLLECTION>;
```
Collection identifiers that may differ between environments.

### Configuration Types
```cpp
using ovlConfigurationType = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_CONFIGURATION>;
```
Configuration type labels that can be selectively ignored.

### Changelogs
```cpp
using ovlChangeLog = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_CHANGELOG>;
```
Human-readable change descriptions that don't affect functional equality.

## Performance Considerations

- **Header-Only** - No separate compilation, allows inlining
- **Template Instantiation** - Each mask value creates a separate type
- **Minimal Overhead** - Adds only initialization check and mask comparison

## Thread Safety

Same as base `ovlKeyValue`:
- Not thread-safe
- References mutable JSON data
- No internal synchronization needed (read-only after init)

## Related Files

- **ovlKeyValue.h** - Base class
- **common.h** - Mask constants and types
- **ovlDatabaseRecord.h** - Uses this template for version, collection, etc.
- **ovlChangeLog.h** - Derives from this template
- **ovlOrigin.h** - Uses for raw data fields

## Best Practices

1. **Use Type Aliases** - Don't use `ovlStringKeyValue<mask>` directly; use named aliases
2. **Choose Appropriate Masks** - Match mask to the semantic meaning of the field
3. **Initialize Properly** - Let the template handle default initialization
4. **Check _initOK** - Verify initialization succeeded in debug builds

## Notes

- This is a header-only template (no .cpp file)
- The "not-provided" literal comes from `jsonliteral::notprovided`
- Type checking is strict: value must be `type_t::VALUE`, not object or array
- The template parameter makes different instantiations incompatible types
- Comparison operator checks the global mask, enabling runtime control of comparison behavior
