# ovlStringKeyValue.h

**Path:** `artdaq-database/Overlay/ovlStringKeyValue.h`

**Purpose:** Header-only template class providing a specialized overlay for JSON string values with automatic default value initialization and maskable comparison. Extends `ovlKeyValue` to handle string-valued fields, automatically setting empty strings to "not-provided" and supporting selective comparison through template-based mask parameters.

## Key Concepts

### String Field Handling

Many document fields are simple strings (versions, collections, configuration types). This template provides:
- Type-safe string access via the base class `string_value()` method
- Automatic default value for empty strings (set to "not-provided")
- Mask-aware comparison for ignoring string differences

### Default Value Pattern

Empty strings are automatically replaced with the literal "not-provided" during initialization. This:
- Distinguishes between "not set" and "intentionally empty"
- Provides consistent debugging output
- Prevents confusion when examining JSON documents

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not supported; instances hold mutable references to JSON data
- **Locking:** None; caller must synchronize access if used from multiple threads

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/Overlay/common.h` | Types, result_t, comparison flags, JSON literals |
| `artdaq-database/Overlay/ovlKeyValue.h` | Base class for key-value overlay |

## Classes/Structures

### `ovlStringKeyValue<mask>`

A template class extending `ovlKeyValue` for string-valued JSON fields. Automatically initializes empty strings to "not-provided" and supports mask-aware comparison.

**Template Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `mask` | `std::uint32_t` | Bitmask for comparison control from `DOCUMENT_COMPARE_FLAGS` |

**Thread Safety:** Not thread-safe

#### Methods

##### `ovlStringKeyValue(object_t::key_type const& key, value_t& value)`

**Brief:** Constructs an overlay for a JSON string field. If the string value is empty, it is automatically set to "not-provided".

**Parameters:**
- `key` - The JSON key identifying this field
- `value` - Reference to the JSON string value to overlay

**Preconditions:**
- `value` must be a primitive value type (type_t::VALUE), not an object or array

**Postconditions:**
- If the string was empty, it now contains "not-provided"

**Throws:**
| Exception | Condition |
|-----------|-----------|
| assertion failure | If `value` is not a primitive value type |

**Thread Safety:** Not thread-safe

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlStringKeyValue.h"

using namespace artdaq::database::overlay;

void initializeVersion() {
  value_t parent = object_t{};
  auto& obj = parent.value_as<object_t>();
  obj["version"] = "";  // Empty string

  // After construction, empty string becomes "not-provided"
  ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_VERSION> version("version", obj.at("version"));

  std::cout << version.string_value() << std::endl;  // Outputs: not-provided
}
```

##### `operator==(ovlStringKeyValue const& other) const -> result_t`

**Brief:** Compares this overlay with another for equality, respecting the global comparison mask. If the mask bit is set, the comparison is skipped.

**Parameters:**
- `other` - The overlay to compare against

**Returns:** `Success()` if the mask bit is set or values are equal; `Failure(message)` with difference description otherwise

**Thread Safety:** Not thread-safe

**Example:**
```cpp
ovlVersion v1("version", json1.at("version"));
ovlVersion v2("version", json2.at("version"));

v1.string_value() = "v1_0_0";
v2.string_value() = "v2_0_0";

// Without mask: versions are different
useCompareMask(0);
auto result = v1 == v2;
// result.first == false

// With mask: version differences ignored
useCompareMask(DOCUMENT_COMPARE_MUTE_VERSION);
result = v1 == v2;
// result.first == true
```

## Type Aliases

The following type aliases are defined throughout the codebase:

```cpp
// In ovlDatabaseRecord.h
using ovlVersion = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_VERSION>;
using ovlVersionUPtr_t = std::unique_ptr<ovlVersion>;

using ovlCollection = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_COLLECTION>;
using ovlCollectionUPtr_t = std::unique_ptr<ovlCollection>;

using ovlConfigurationType = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_CONFIGURATION>;
using ovlConfigurationTypeUPtr_t = std::unique_ptr<ovlConfigurationType>;

// In ovlOrigin.h
using ovlRawData = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_RAWDATA>;
```

## Usage Examples

### Version Field Management

```cpp
#include "artdaq-database/Overlay/ovlStringKeyValue.h"

using namespace artdaq::database::overlay;

// Type alias for clarity
using ovlVersion = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_VERSION>;

void manageVersion() {
  value_t recordJson = object_t{};
  auto& record = recordJson.value_as<object_t>();
  record["version"] = std::string{};  // Empty initially

  ovlVersion version("version", record.at("version"));

  // Initially "not-provided"
  std::cout << "Initial: " << version.string_value() << std::endl;

  // Set actual version
  version.string_value() = "v1_0_0";
  std::cout << "Updated: " << version.string_value() << std::endl;
}
```

### Collection Field

```cpp
#include "artdaq-database/Overlay/ovlStringKeyValue.h"

using namespace artdaq::database::overlay;
using ovlCollection = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_COLLECTION>;

void handleCollection() {
  value_t docJson = object_t{};
  auto& doc = docJson.value_as<object_t>();
  doc["collection"] = "configurations";

  ovlCollection collection("collection", doc.at("collection"));

  // Read collection name
  std::cout << "Collection: " << collection.string_value() << std::endl;

  // Modify if needed
  collection.string_value() = "detector_configs";
}
```

### Derived Class Pattern

```cpp
// In ovlChangeLog.h - extending the template
class ovlChangeLog final : public ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_CHANGELOG> {
 public:
  ovlChangeLog(object_t::key_type const& key, value_t& changelog)
    : ovlStringKeyValue(key, changelog) {}

  // Additional changelog-specific methods
  std::string& buffer() {
    return string_value();
  }

  std::string& append(std::string const& entry) {
    auto& buf = buffer();
    if (!buf.empty() && buf != jsonliteral::notprovided) {
      buf += "\n";
    } else {
      buf = "";
    }
    buf += entry;
    return buf;
  }
};
```

## Common Use Cases

| Type Alias | Mask | Purpose |
|------------|------|---------|
| `ovlVersion` | `DOCUMENT_COMPARE_MUTE_VERSION` | Database record version strings |
| `ovlCollection` | `DOCUMENT_COMPARE_MUTE_COLLECTION` | Collection identifiers |
| `ovlConfigurationType` | `DOCUMENT_COMPARE_MUTE_CONFIGURATION` | Configuration type labels |
| `ovlRawData` | `DOCUMENT_COMPARE_MUTE_RAWDATA` | Raw data content |
| (derived) `ovlChangeLog` | `DOCUMENT_COMPARE_MUTE_CHANGELOG` | Human-readable change descriptions |

## Relationship to Other Components

This template is used throughout the Overlay module for string-valued metadata fields:
- `ovlDatabaseRecord` uses it for version and collection fields
- `ovlOrigin` uses it for raw data fields
- `ovlChangeLog` extends it with additional methods

It differs from `ovlKeyValueWithMask` in that:
- It expects primitive string values, not objects
- It provides automatic default value initialization
- It uses `string_value()` for type-safe string access

## See Also

- [ovlKeyValue.h](./ovlKeyValue.h.md) - Base class providing core overlay functionality
- [ovlChangeLog.h](./ovlChangeLog.h.md) - Derived class extending this template
- [ovlKeyValueWithMask.h](./ovlKeyValueWithMask.h.md) - Similar template for object values
- [common.h](./common.h.md) - Defines `DOCUMENT_COMPARE_FLAGS` enum and `jsonliteral::notprovided`

## Notes for Developers

### Common Pitfalls

- **Value must be primitive:** The constructor asserts that the value type is `type_t::VALUE` (a primitive), not an object or array. Using this with object fields will fail.
- **Default literal:** The "not-provided" literal comes from `jsonliteral::notprovided`. This is a fixed string constant.

### Design Notes

- This is a header-only template with no corresponding .cpp file
- The `_initOK` member tracks successful initialization (useful for debugging)
- Exception-safe initialization with try-catch block
- Different template instantiations create incompatible types

### Anti-patterns

```cpp
// DON'T: Use with object values
value_t objValue = object_t{};
ovlStringKeyValue<MASK> field("key", objValue);  // Will assert!

// DO: Use with string values
value_t strValue = std::string{"hello"};
ovlStringKeyValue<MASK> field("key", strValue);  // OK

// DON'T: Use ovlStringKeyValue directly
ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_VERSION> version(...);  // Unclear intent

// DO: Use type aliases
using ovlVersion = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_VERSION>;
ovlVersion version(...);  // Clear semantic meaning
```

### Performance Considerations

- Header-only implementation allows inlining
- Minimal overhead: just initialization check and mask comparison
- Each mask value creates a separate type (template instantiation)
