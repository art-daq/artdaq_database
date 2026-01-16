# common.h

**Path:** `artdaq-database/Overlay/common.h`

**Purpose:** This header file serves as the common foundation for the Overlay module, providing essential type definitions, namespace aliases, constants, and the enumeration of document comparison flags used throughout the module. It aggregates dependencies from JSON handling, shared types, and establishes the overlay namespace infrastructure.


## Key Concepts

### The Overlay Pattern Foundation

This header provides the building blocks for the Overlay Pattern implementation:
- **JSON Types** - Brings in JSON handling types (`array_t`, `object_t`, `value_t`, `type_t`) from the JSON module
- **Namespace Aliases** - Creates convenient shortcuts for deeply nested namespaces
- **Error Constants** - Provides standard JSON-formatted error messages for consistent error handling
- **Comparison Flags** - Establishes a comprehensive bit flag system for selective document comparison

### Comparison Masking System

The module provides fine-grained comparison control through bit flags. Each flag represents a specific component that can be selectively ignored during equality comparison. Flags can be combined using bitwise OR operations to create custom comparison behaviors for different use cases (testing, migration, synchronization).

### Result Type Pattern

The `result_t` type (a pair of bool and string) provides:
- **Success/Failure Indication** - Boolean first element indicates operation outcome
- **Error Messages** - String second element explains failures in human-readable form
- **Composability** - Results can be aggregated and combined across multiple operations

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** The `useCompareMask()` function uses a static variable without synchronization
- **Locking:** None - callers must ensure single-threaded access to comparison operations

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/Json/json_common.h` | Core JSON type definitions and utilities |
| `artdaq-database/DataFormats/Json/json_types_impl.h` | JSON type implementation details including `unwrap()` |
| `artdaq-database/DataFormats/shared_literals.h` | String literal constants for JSON field names |
| `artdaq-database/SharedCommon/sharedcommon_common.h` | Common utilities, types, and standard library includes |

## Type Aliases

### JSON Types (at file scope)

```cpp
using artdaq::database::json::array_t;    // JSON array type (vector-like container)
using artdaq::database::json::object_t;   // JSON object type (map-like container)
using artdaq::database::json::type_t;     // JSON type enumeration (OBJECT, ARRAY, STRING, etc.)
using artdaq::database::json::value_t;    // JSON value variant type (holds any JSON value)
```

**Brief:** These type aliases bring JSON types into the overlay namespace for convenient access throughout the module.

### Namespace Alias

```cpp
namespace jsonliteral = artdaq::database::dataformats::literal;
```

**Brief:** Provides short access to JSON field name literals (e.g., `jsonliteral::name`, `jsonliteral::timestamp`, `jsonliteral::bookkeeping`).

### Result Type

```cpp
using result_t = artdaq::database::result_t;
```

**Brief:** Standard result type (pair of bool and string) used for operation outcomes throughout the overlay module.

## Constants

| Constant | Value | Purpose |
|----------|-------|---------|
| `msg_InvalidArgument` | `"{\"message\":\"Invalid Argument\"}"` | Returned when invalid parameters are passed to a function |
| `msg_IsReadonly` | `"{\"message\":\"Document is readonly\"}"` | Returned when attempting to modify a readonly document |
| `msg_ConvertionError` | `"{\"message\":\"Conversion error\"}"` | Returned on serialization or type conversion failure |

**Brief:** These JSON-formatted error message constants provide consistent error reporting across all overlay operations.

## Enumerations

### `DOCUMENT_COMPARE_FLAGS`

**Brief:** Bit flags that control which document components are compared during equality operations. Combine flags with bitwise OR to ignore multiple components.

```cpp
enum DOCUMENT_COMPARE_FLAGS {
  // Individual component flags
  DOCUMENT_COMPARE_MUTE_TIMESTAMPS       = (1 << 0),   // Ignore all timestamp fields
  DOCUMENT_COMPARE_MUTE_OUIDS            = (1 << 1),   // Ignore object unique IDs
  DOCUMENT_COMPARE_MUTE_COMMENTS         = (1 << 2),   // Ignore comment entries
  DOCUMENT_COMPARE_MUTE_BOOKKEEPING      = (1 << 3),   // Ignore entire bookkeeping section
  DOCUMENT_COMPARE_MUTE_ORIGIN           = (1 << 4),   // Ignore origin/provenance information
  DOCUMENT_COMPARE_MUTE_VERSION          = (1 << 5),   // Ignore version field
  DOCUMENT_COMPARE_MUTE_CHANGELOG        = (1 << 6),   // Ignore changelog string
  DOCUMENT_COMPARE_MUTE_UPDATES          = (1 << 7),   // Ignore update history array
  DOCUMENT_COMPARE_MUTE_CONFIGENTITY     = (1 << 8),   // Ignore configuration entities
  DOCUMENT_COMPARE_MUTE_CONFIGURATION    = (1 << 9),   // Ignore configurations
  DOCUMENT_COMPARE_MUTE_ALIAS            = (1 << 10),  // Ignore alias assignments
  DOCUMENT_COMPARE_MUTE_ALIAS_HISTORY    = (1 << 11),  // Ignore alias assignment history
  DOCUMENT_COMPARE_MUTE_RUN              = (1 << 12),  // Ignore run information
  DOCUMENT_COMPARE_MUTE_ATTACHMENT       = (1 << 13),  // Ignore attachments
  DOCUMENT_COMPARE_MUTE_SEARCH           = (1 << 14),  // Ignore search metadata
  DOCUMENT_COMPARE_MUTE_RAWDATA          = (1 << 15),  // Ignore raw data payloads
  DOCUMENT_COMPARE_MUTE_UPDATE_VALUES    = (1 << 16),  // Ignore update values (keep events)

  // Broad scope flags - use with caution
  DOCUMENT_COMPARE_MUTE_COLLECTION       = (1 << 28),  // Ignore collection name
  DOCUMENT_COMPARE_MUTE_DATA             = (1 << 29),  // Ignore entire user data section
  DOCUMENT_COMPARE_MUTE_METADATA         = (1 << 30)   // Ignore entire metadata section
};
```

## Functions

### `useCompareMask(std::uint32_t) -> std::uint32_t`

**Brief:** Gets or sets the global comparison mask that controls which document components are compared during overlay equality operations. The mask value is initialized on the first call and cannot be changed afterward.

**Parameters:**
- `compareMask` - Optional mask value to set on first call (default: 0). This value is only used during initialization; subsequent calls ignore this parameter.

**Preconditions:**
- None

**Returns:** The current comparison mask value (always returns the value set during first initialization)

**Postconditions:**
- First call: Initializes the static comparison mask to the provided value
- All calls: Returns the current mask value

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | This function does not throw |

**Thread Safety:** Unsafe - uses a static variable without synchronization. C++11 guarantees thread-safe initialization of static locals, but subsequent reads are not synchronized.

**Side Effects:**
- First call initializes a static variable that persists for program lifetime
- The mask value cannot be changed after first initialization

**Example:**
```cpp
#include "artdaq-database/Overlay/common.h"
#include <iostream>

using namespace artdaq::database::overlay;

void setupComparisonMask() {
  // Set mask to ignore timestamps, bookkeeping, and origin
  // IMPORTANT: This must be done before any comparisons
  auto mask = DOCUMENT_COMPARE_MUTE_TIMESTAMPS |
              DOCUMENT_COMPARE_MUTE_BOOKKEEPING |
              DOCUMENT_COMPARE_MUTE_ORIGIN;

  // Initialize the mask (only works on first call)
  useCompareMask(mask);

  // Query current mask value
  auto currentMask = useCompareMask();
  std::cout << "Current mask: " << currentMask << "\n";
}

void compareDocumentsIgnoringMetadata(value_t& json1, value_t& json2) {
  // Assumes mask was set up before any comparisons
  ovlDatabaseRecord record1{"record", json1};
  ovlDatabaseRecord record2{"record", json2};

  auto result = record1 == record2;  // Uses global mask
  if (result.first) {
    std::cout << "Records are equal (ignoring masked components)\n";
  } else {
    std::cerr << "Records differ: " << result.second << "\n";
  }
}
```

## Relationship to Other Components

### Within the Overlay Module

- **Included by all overlay files** - Every `.h` and `.cpp` file in the Overlay module includes this header
- **Provides foundation types** - All overlay classes use the types defined here
- **Comparison infrastructure** - The flags and `useCompareMask()` function are used by all `operator==` implementations

### Comparison with JSONDocumentOverlay.h

| Aspect | common.h | JSONDocumentOverlay.h |
|--------|----------|----------------------|
| **Purpose** | Internal module foundation | External API entry point |
| **Audience** | Overlay module implementation files | External code using the overlay |
| **Contents** | Types, constants, flags, utilities | Just the main record interface |
| **Include Frequency** | Included by every overlay `.cpp` file | Included by external code |

### Files That Include common.h

All overlay implementation files include this header:
- `ovlKeyValue.h/.cpp` - Base key-value overlay
- `ovlBookkeeping.h/.cpp` - Bookkeeping metadata
- `ovlDocument.h/.cpp` - Document overlay
- `ovlDatabaseRecord.h/.cpp` - Complete database document record
- All other overlay files

## See Also

- [ovlKeyValue.h](./ovlKeyValue.h.md) - Base class using types from this header
- [JSONDocumentOverlay.h](./JSONDocumentOverlay.h.md) - External API entry point
- [json_common.h](../DataFormats/Json/json_common.h.md) - JSON type definitions
- [sharedcommon_common.h](../SharedCommon/sharedcommon_common.h.md) - Shared utilities

## Notes for Developers

### Using Comparison Flags

```cpp
#include "artdaq-database/Overlay/common.h"
#include <iostream>

using namespace artdaq::database::overlay;

void compareDocumentsWithMask(value_t& json1, value_t& json2) {
  // Compare content only, ignore all metadata
  // IMPORTANT: Set this BEFORE creating any overlay objects that will be compared
  auto mask = DOCUMENT_COMPARE_MUTE_BOOKKEEPING |
              DOCUMENT_COMPARE_MUTE_TIMESTAMPS |
              DOCUMENT_COMPARE_MUTE_ORIGIN |
              DOCUMENT_COMPARE_MUTE_COMMENTS |
              DOCUMENT_COMPARE_MUTE_CHANGELOG;

  useCompareMask(mask);

  try {
    ovlDatabaseRecord record1{"record", json1};
    ovlDatabaseRecord record2{"record", json2};

    auto result = record1 == record2;
    if (result.first) {
      std::cout << "Records are equal (considering mask)\n";
    } else {
      std::cerr << "Records differ: " << result.second << "\n";
    }
  } catch (const std::exception& e) {
    std::cerr << "Comparison failed: " << e.what() << "\n";
  }
}
```

### Common Pitfalls

- **Static Mask State:** The `useCompareMask()` function uses a static variable that is initialized on first call. The mask cannot be changed after initialization. Always set the mask before any comparison operations.
- **Flag Combination:** Use bitwise OR (`|`) to combine flags, never addition (`+`). While addition may work for non-overlapping flags, it fails for duplicate flags and is semantically incorrect.
- **Thread Safety:** Do not use comparison operations from multiple threads without external synchronization. While static initialization is thread-safe in C++11, concurrent reads/writes to the mask are not.

### Anti-patterns

```cpp
// DON'T: Use addition to combine flags
auto mask = DOCUMENT_COMPARE_MUTE_TIMESTAMPS + DOCUMENT_COMPARE_MUTE_BOOKKEEPING;  // Wrong!

// DO: Use bitwise OR
auto mask = DOCUMENT_COMPARE_MUTE_TIMESTAMPS | DOCUMENT_COMPARE_MUTE_BOOKKEEPING;  // Correct

// DON'T: Try to change mask after first initialization
useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS);  // First call - sets mask
useCompareMask(DOCUMENT_COMPARE_MUTE_BOOKKEEPING); // Ignored! Mask is already set

// DO: Set all desired flags in the first call
auto allFlags = DOCUMENT_COMPARE_MUTE_TIMESTAMPS | DOCUMENT_COMPARE_MUTE_BOOKKEEPING;
useCompareMask(allFlags);  // Set once with all needed flags
```

### Reserved Flag Positions

- Flags at positions 17-27 are unused and reserved for future expansion
- Flags at positions 28-30 are marked "use with caution" due to their broad scope (they ignore entire document sections)

### Best Practices

1. **Always include this header first** when creating Overlay module files
2. **Use namespace aliases** (`jsonliteral`, not full `artdaq::database::dataformats::literal`)
3. **Set comparison mask early** in your program, before any comparison operations
4. **Combine flags with bitwise OR**, never with addition
5. **Use error constants** (`msg_IsReadonly`, etc.) for consistency across error reporting
6. **Document which flags affect your comparisons** when implementing `operator==`
7. **Consider mask implications in tests** - tests may need specific mask configurations
