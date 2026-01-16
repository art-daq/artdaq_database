# ovlKeyValueWithMask.h

**Path:** `artdaq-database/Overlay/ovlKeyValueWithMask.h`

**Purpose:** A minimal template class that extends `ovlKeyValue` with maskable comparison support. Unlike `ovlKeyValueWithDefault`, this template does not initialize default values, making it suitable for overlaying existing complete JSON structures such as document data and metadata fields that are expected to already exist.

## Key Concepts

### Minimal Extension

This template provides the simplest possible extension of `ovlKeyValue` - it only adds mask-aware comparison without any initialization logic. This makes it ideal for fields that:
- Are required to exist before overlay creation
- Do not need default values
- Should support selective comparison masking

### Comparison Masking

Like other mask-enabled templates, the `mask` template parameter controls comparison behavior. When the corresponding bit is set in the global comparison mask, differences in this field are ignored during equality checks.

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

### `ovlKeyValueWithMask<mask>`

A template class extending `ovlKeyValue` with maskable comparison. Provides no additional initialization beyond the base class, suitable for required fields that already exist.

**Template Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `mask` | `std::uint32_t` | Bitmask for comparison control from `DOCUMENT_COMPARE_FLAGS` |

**Thread Safety:** Not thread-safe

#### Methods

##### `ovlKeyValueWithMask(object_t::key_type const& key, value_t& object)`

**Brief:** Constructs an overlay for an existing JSON field. Simply wraps the value without any initialization or default value creation.

**Parameters:**
- `key` - The JSON key identifying this field within the parent
- `object` - Reference to the JSON value to overlay

**Preconditions:**
- The field must already exist in the JSON structure

**Postconditions:**
- Overlay is ready to access the field value

**Thread Safety:** Not thread-safe

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlKeyValueWithMask.h"

using namespace artdaq::database::overlay;

void accessExistingField() {
  value_t documentJson = object_t{};
  auto& doc = documentJson.value_as<object_t>();

  // Field must exist before creating overlay
  doc["data"] = object_t{{"key", "value"}};

  ovlKeyValueWithMask<DOCUMENT_COMPARE_MUTE_DATA> data("data", doc.at("data"));

  // Can now access the existing data
  std::cout << data.to_string() << std::endl;
}
```

##### `operator==(ovlKeyValueWithMask const& other) const -> result_t`

**Brief:** Compares this overlay with another for equality, respecting the global comparison mask. If the mask bit is set, the comparison is skipped and returns success.

**Parameters:**
- `other` - The overlay to compare against

**Returns:** `Success()` if the mask bit is set (comparison skipped) or if values are equal; delegates to base class `self() == other.self()` otherwise

**Thread Safety:** Not thread-safe

**Example:**
```cpp
// When mask bit is set, comparison always succeeds
useCompareMask(DOCUMENT_COMPARE_MUTE_DATA);
auto result = data1 == data2;  // Always Success(), regardless of actual content

// When mask bit is not set, actual comparison is performed
useCompareMask(0);
result = data1 == data2;  // Compares actual JSON content
```

## Type Aliases

The following type aliases are defined in `ovlDocument.h`:

```cpp
using ovlData = ovlKeyValueWithMask<DOCUMENT_COMPARE_MUTE_DATA>;
using ovlMetadata = ovlKeyValueWithMask<DOCUMENT_COMPARE_MUTE_METADATA>;
```

## Usage Examples

### Document Data and Metadata

```cpp
#include "artdaq-database/Overlay/ovlKeyValueWithMask.h"

using namespace artdaq::database::overlay;

// Type aliases for clarity
using ovlData = ovlKeyValueWithMask<DOCUMENT_COMPARE_MUTE_DATA>;
using ovlMetadata = ovlKeyValueWithMask<DOCUMENT_COMPARE_MUTE_METADATA>;

void handleDocument() {
  value_t documentJson = object_t{};
  auto& doc = documentJson.value_as<object_t>();

  // Both fields must exist
  doc["data"] = object_t{{"threshold", 100}, {"enabled", true}};
  doc["metadata"] = object_t{{"version", "1.0"}, {"author", "system"}};

  ovlData data("data", doc.at("data"));
  ovlMetadata metadata("metadata", doc.at("metadata"));

  // Access values
  auto& dataObj = data.object_value();
  std::cout << "Threshold: " << dataObj["threshold"] << std::endl;
}
```

### Masked Comparison for Testing

```cpp
#include "artdaq-database/Overlay/ovlKeyValueWithMask.h"

using namespace artdaq::database::overlay;

bool compareDocumentsIgnoringData() {
  value_t doc1, doc2;
  // ... populate documents ...

  // Create overlays
  ovlData data1("data", doc1.at("data"));
  ovlData data2("data", doc2.at("data"));

  // Ignore data differences, focus on structure
  useCompareMask(DOCUMENT_COMPARE_MUTE_DATA);

  auto result = data1 == data2;
  return result.first;  // Always true since data is masked
}
```

## Comparison with Similar Templates

| Template | Initialization | Default Value | Primary Use Case |
|----------|---------------|---------------|------------------|
| `ovlKeyValueWithMask` | None | None | Required fields (data, metadata) |
| `ovlKeyValueWithDefault` | Creates empty object | `{}` | Optional object fields |
| `ovlStringKeyValue` | Sets default string | "not-provided" | Optional string fields |

## Relationship to Other Components

This template provides the simplest mask-aware overlay, used primarily in `ovlDocument` for wrapping the main "data" and "metadata" sections of database documents. These are:
- Required fields that must exist in valid documents
- Large structures where full comparison may be expensive
- Fields where masking provides flexibility in testing and validation

## See Also

- [ovlKeyValue.h](./ovlKeyValue.h.md) - Base class providing core overlay functionality
- [ovlDocument.h](./ovlDocument.h.md) - Primary user of this template (ovlData, ovlMetadata)
- [ovlKeyValueWithDefault.h](./ovlKeyValueWithDefault.h.md) - Similar template with default initialization
- [common.h](./common.h.md) - Defines `DOCUMENT_COMPARE_FLAGS` enum

## Notes for Developers

### Common Pitfalls

- **Field must exist:** Unlike `ovlKeyValueWithDefault`, this template does not create the field. The JSON structure must already contain the field before creating the overlay.
- **No validation:** The constructor performs no type validation on the wrapped value.

### Design Notes

- This is a header-only template with no corresponding .cpp file
- Minimal overhead - only adds mask checking to base class comparison
- No member variables beyond inherited ones
- Designed for high-performance scenarios where initialization is unnecessary

### When to Use Which Template

```cpp
// Use ovlKeyValueWithMask for required, pre-existing fields
// that need mask-aware comparison
ovlKeyValueWithMask<MASK> data("data", existingValue);

// Use ovlKeyValueWithDefault for optional fields that should
// be auto-created if missing
ovlKeyValueWithDefault<MASK> optional("optional", parentObject);

// Use ovlStringKeyValue for string fields with default values
ovlStringKeyValue<MASK> version("version", stringValue);
```
