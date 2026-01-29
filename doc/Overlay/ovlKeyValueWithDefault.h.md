# ovlKeyValueWithDefault.h

**Path:** `artdaq-database/Overlay/ovlKeyValueWithDefault.h`

**Purpose:** Template class extending `ovlKeyValue` with automatic default value initialization and maskable comparison. This overlay ensures that a JSON field exists with at least an empty object if not already present, preventing access errors on optional fields while supporting selective comparison through masking.

## Key Concepts

### Auto-initialization Pattern

This template addresses a common challenge: accessing optional JSON fields that may not exist. Rather than requiring callers to check for field existence before access, this overlay automatically creates an empty object at the specified key if one does not exist.

### Comparison Masking

The template parameter `mask` controls whether this field should be compared during equality checks. When the global comparison mask has this bit set, differences in this field are ignored.

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

### `ovlKeyValueWithDefault<mask>`

A template class extending `ovlKeyValue` that automatically creates an empty JSON object at the specified key if the field does not exist. This prevents null access errors for optional fields.

**Template Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `mask` | `std::uint32_t` | Bitmask for comparison control from `DOCUMENT_COMPARE_FLAGS` |

**Thread Safety:** Not thread-safe

#### Methods

##### `ovlKeyValueWithDefault(object_t::key_type const& key, value_t& object)`

**Brief:** Constructs an overlay for a JSON object field, creating an empty object at the specified key if the field does not exist.

**Parameters:**
- `key` - The JSON key identifying this field within the parent object
- `object` - Reference to the parent JSON value (must be an object)

**Preconditions:**
- `object` must be a JSON object (type_t::OBJECT)

**Postconditions:**
- The field specified by `key` exists in the parent object (created as empty object if missing)

**Throws:**
| Exception | Condition |
|-----------|-----------|
| assertion failure | If `object` is not a JSON object |

**Thread Safety:** Not thread-safe

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlKeyValueWithDefault.h"

using namespace artdaq::database::overlay;

void accessOptionalField() {
  value_t parent = object_t{};
  // Note: "metadata" field does not exist yet

  try {
    // Using mask for bookkeeping fields
    ovlKeyValueWithDefault<DOCUMENT_COMPARE_MUTE_BOOKKEEPING>
      metadata("metadata", parent);

    // Field now exists as empty object
    auto& obj = metadata.object_value();
    obj["created"] = "2025-01-15";  // Can now add fields
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}
```

##### `operator==(ovlKeyValueWithDefault const& other) const -> result_t`

**Brief:** Compares this overlay with another for equality, respecting the global comparison mask.

**Parameters:**
- `other` - The overlay to compare against

**Returns:** `Success()` if the mask bit is set (comparison skipped) or if values are equal; `Failure(message)` with difference description otherwise

**Thread Safety:** Not thread-safe

**Example:**
```cpp
// With mask set, comparison is always successful
useCompareMask(DOCUMENT_COMPARE_MUTE_BOOKKEEPING);
auto result = field1 == field2;  // Always Success()

// Without mask, actual comparison is performed
useCompareMask(0);
result = field1 == field2;  // Compares JSON values
```

## Comparison with Similar Templates

| Template | Initialization | Use Case |
|----------|---------------|----------|
| `ovlKeyValueWithDefault` | Creates empty object if missing | Optional object fields |
| `ovlKeyValueWithMask` | No initialization | Required existing fields |
| `ovlStringKeyValue` | Sets "not-provided" for empty strings | String fields |

## Usage Examples

### Pattern for Optional Metadata Fields

```cpp
#include "artdaq-database/Overlay/ovlKeyValueWithDefault.h"

using namespace artdaq::database::overlay;

// Define a type alias for optional bookkeeping
using ovlOptionalBookkeeping = ovlKeyValueWithDefault<DOCUMENT_COMPARE_MUTE_BOOKKEEPING>;

void handleOptionalField() {
  value_t documentJson = object_t{};

  // Field "bookkeeping" may or may not exist
  // This overlay will create it if missing
  auto bookkeeping = std::make_unique<ovlOptionalBookkeeping>("bookkeeping", documentJson);

  // Now safe to access and modify
  auto& obj = bookkeeping->object_value();
  obj["lastModified"] = timestamp();
  obj["modifiedBy"] = "user123";
}
```

### Derived Class Pattern

```cpp
// Extending with additional functionality
template <std::uint32_t mask>
class MyOptionalField : public ovlKeyValueWithDefault<mask> {
 public:
  using ovlKeyValueWithDefault<mask>::ovlKeyValueWithDefault;

  // Additional methods specific to this field type
  void setCustomProperty(std::string const& value) {
    this->object_value()["custom"] = value;
  }
};
```

## Relationship to Other Components

This template provides a base pattern for optional fields that should exist as objects. It is a simpler alternative to `ovlKeyValueTimeStamp` when:
- No timestamp tracking is needed
- The field should always exist (even if empty)
- Default initialization is sufficient

The pattern is useful for metadata and bookkeeping sections of documents where the section should always be present but may have varying content.

## See Also

- [ovlKeyValue.h](./ovlKeyValue.h.md) - Base class providing key-value overlay functionality
- [ovlKeyValueWithMask.h](./ovlKeyValueWithMask.h.md) - Similar template without default initialization
- [ovlBookkeeping.h](./ovlBookkeeping.h.md) - Uses this pattern for bookkeeping fields
- [common.h](./common.h.md) - Defines `DOCUMENT_COMPARE_FLAGS` enum

## Notes for Developers

### Common Pitfalls

- **Parent must be object:** The constructor asserts that the parent value is a JSON object, not an array or primitive value.
- **Creates nested objects:** The initialization creates an empty object `{}`, which is suitable for object fields but not for array or primitive fields.

### Design Notes

- This is a header-only template with no corresponding .cpp file
- The `_initOK` member tracks successful initialization (useful for debugging)
- Exception-safe initialization with try-catch block
- The created empty object can be populated after construction

### Anti-patterns

```cpp
// DON'T: Use with array parent
value_t arrayParent = array_t{};
ovlKeyValueWithDefault<MASK> field("key", arrayParent);  // Will assert!

// DO: Use with object parent
value_t objectParent = object_t{};
ovlKeyValueWithDefault<MASK> field("key", objectParent);  // OK

// DON'T: Use for string/primitive fields (creates object, not string)
// This template creates an empty object {}, not an empty string

// DO: Use ovlStringKeyValue for string fields with defaults
```
