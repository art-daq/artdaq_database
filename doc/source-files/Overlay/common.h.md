# common.h

## File Overview

This header file serves as the common foundation for the Overlay module, providing essential type definitions, namespace aliases, constants, and the enumeration of document comparison flags used throughout the module. It aggregates dependencies from JSON handling, shared types, and establishes the overlay namespace infrastructure.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/common.h`

## Purpose

The Overlay module provides object-oriented wrappers around JSON data structures representing database records. This common header:

1. **Imports JSON Types** - Brings in JSON handling types and implementations
2. **Defines Namespace Aliases** - Creates convenient shortcuts for deeply nested namespaces
3. **Declares Constants** - Provides standard error messages
4. **Defines Comparison Flags** - Establishes a comprehensive flag system for selective document comparison
5. **Declares Utilities** - Forward-declares the comparison mask utility function

## Dependencies

### JSON and Data Format Headers
```cpp
#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/DataFormats/Json/json_types_impl.h"
#include "artdaq-database/DataFormats/shared_literals.h"
```
- `json_common.h` - Core JSON type definitions and utilities
- `json_types_impl.h` - JSON type implementation details
- `shared_literals.h` - String literal constants used across the database

### Shared Common Headers
```cpp
#include "artdaq-database/SharedCommon/sharedcommon_common.h"
```
- Provides common utilities, types, and standard library includes

## Namespace Structure

```cpp
namespace artdaq {
namespace database {
namespace overlay {
  // Overlay module code
}}}
```

### Type Aliases

The file imports JSON types into the overlay namespace for convenience:

```cpp
using artdaq::database::json::array_t;    // JSON array type
using artdaq::database::json::object_t;   // JSON object type
using artdaq::database::json::type_t;     // JSON type enumeration
using artdaq::database::json::value_t;    // JSON value variant type
```

### Namespace Aliases

```cpp
namespace jsonliteral = artdaq::database::dataformats::literal;
```
Provides short access to JSON field name literals (e.g., `jsonliteral::name`, `jsonliteral::timestamp`)

```cpp
using result_t = artdaq::database::result_t;
```
Standard result type for error handling (a pair of bool and string message)

## Constants

### Error Messages

```cpp
constexpr auto msg_InvalidArgument = "{\"message\":\"Invalid Argument\"}";
constexpr auto msg_IsReadonly = "{\"message\":\"Document is readonly\"}";
constexpr auto msg_ConvertionError = "{\"message\":\"Conversion error\"}";
```

These JSON-formatted error messages are used throughout the Overlay module when operations fail.

## Document Comparison Flags

The `DOCUMENT_COMPARE_FLAGS` enumeration provides fine-grained control over which aspects of database records should be compared when checking equality. Each flag represents a specific component that can be selectively ignored during comparison.

```cpp
enum DOCUMENT_COMPARE_FLAGS {
  DOCUMENT_COMPARE_MUTE_TIMESTAMPS       = (1 << 0),   // Ignore all timestamps
  DOCUMENT_COMPARE_MUTE_OUIDS            = (1 << 1),   // Ignore object unique IDs
  DOCUMENT_COMPARE_MUTE_COMMENTS         = (1 << 2),   // Ignore comments
  DOCUMENT_COMPARE_MUTE_BOOKKEEPING      = (1 << 3),   // Ignore bookkeeping metadata
  DOCUMENT_COMPARE_MUTE_ORIGIN           = (1 << 4),   // Ignore origin information
  DOCUMENT_COMPARE_MUTE_VERSION          = (1 << 5),   // Ignore version
  DOCUMENT_COMPARE_MUTE_CHANGELOG        = (1 << 6),   // Ignore changelog
  DOCUMENT_COMPARE_MUTE_UPDATES          = (1 << 7),   // Ignore update history
  DOCUMENT_COMPARE_MUTE_CONFIGENTITY     = (1 << 8),   // Ignore configuration entities
  DOCUMENT_COMPARE_MUTE_CONFIGURATION    = (1 << 9),   // Ignore configurations
  DOCUMENT_COMPARE_MUTE_ALIAS            = (1 << 10),  // Ignore aliases
  DOCUMENT_COMPARE_MUTE_ALIAS_HISTORY    = (1 << 11),  // Ignore alias history
  DOCUMENT_COMPARE_MUTE_RUN              = (1 << 12),  // Ignore run information
  DOCUMENT_COMPARE_MUTE_ATTACHMENT       = (1 << 13),  // Ignore attachments
  DOCUMENT_COMPARE_MUTE_SEARCH           = (1 << 14),  // Ignore search metadata
  DOCUMENT_COMPARE_MUTE_RAWDATA          = (1 << 15),  // Ignore raw data
  DOCUMENT_COMPARE_MUTE_UPDATE_VALUES    = (1 << 16),  // Ignore update values

  // Broad scope flags - use with caution
  DOCUMENT_COMPARE_MUTE_COLLECTION       = (1 << 28),  // Ignore collection
  DOCUMENT_COMPARE_MUTE_DATA             = (1 << 29),  // Ignore user data
  DOCUMENT_COMPARE_MUTE_METADATA         = (1 << 30)   // Ignore metadata
};
```

### Flag Usage Patterns

**Bitwise Operations**: Flags are combined using bitwise OR:
```cpp
auto mask = DOCUMENT_COMPARE_MUTE_TIMESTAMPS | DOCUMENT_COMPARE_MUTE_OUIDS;
```

**Selective Comparison**: Enable comparison of content while ignoring metadata:
```cpp
auto mask = DOCUMENT_COMPARE_MUTE_BOOKKEEPING |
            DOCUMENT_COMPARE_MUTE_TIMESTAMPS |
            DOCUMENT_COMPARE_MUTE_ORIGIN;
```

**Complete Muting**: The broad flags at the end can ignore entire sections (use cautiously).

## Utility Functions

```cpp
std::uint32_t useCompareMask(std::uint32_t = 0);
```

This function manages the comparison mask as a static variable:
- **First call with argument**: Sets the comparison mask
- **Subsequent calls**: Returns the current mask
- **Default parameter**: Allows querying current mask without changing it

Implementation is in `ovlKeyValue.cpp`.

## Header Guard

```cpp
#ifndef _ARTDAQ_DATABASE_OVERLAY_COMMON_H_
#define _ARTDAQ_DATABASE_OVERLAY_COMMON_H_
// ... content ...
#endif
```

Traditional include guard prevents multiple inclusion.

## Usage Context

This header is included by virtually every file in the Overlay module:

### Typical Include Pattern in Overlay Files
```cpp
#include "artdaq-database/Overlay/common.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"

using namespace artdaq::database::overlay;
using namespace artdaq::database::result;
```

### Files That Include common.h
All overlay implementation files include this header:
- `ovlKeyValue.h/.cpp` - Base key-value wrapper
- `ovlBookkeeping.h/.cpp` - Bookkeeping metadata
- `ovlDocument.h/.cpp` - Document wrapper
- `ovlDatabaseRecord.h/.cpp` - Complete database record
- All other overlay files

## Design Rationale

### Centralized Type Definitions
By centralizing type imports and aliases, the module ensures:
- **Consistency** - All files use the same type definitions
- **Maintainability** - Changes to type aliases need only occur in one place
- **Clarity** - Shorter, more readable type names throughout the codebase

### Flexible Comparison System
The flag-based comparison system provides:
- **Granularity** - Fine-grained control over what aspects matter in comparisons
- **Composability** - Flags can be combined for complex comparison scenarios
- **Testing** - Enables focused testing of specific record components
- **Versioning** - Allows comparing records while ignoring version metadata

### JSON Format for Errors
Error messages in JSON format enable:
- **Structured Error Handling** - Errors can be parsed and processed programmatically
- **Consistency** - All errors follow the same format
- **Extensibility** - Additional error fields can be added as needed

## Common Usage Patterns

### Setting a Comparison Mask
```cpp
using namespace artdaq::database::overlay;

// Compare only data, ignore all metadata
auto mask = DOCUMENT_COMPARE_MUTE_BOOKKEEPING |
            DOCUMENT_COMPARE_MUTE_ORIGIN |
            DOCUMENT_COMPARE_MUTE_TIMESTAMPS |
            DOCUMENT_COMPARE_MUTE_COMMENTS;

useCompareMask(mask);
```

### Checking Equality with Mask
```cpp
ovlDatabaseRecord record1{json1};
ovlDatabaseRecord record2{json2};

auto result = record1 == record2;  // Uses global mask set by useCompareMask()
if (result.first) {
  // Records are equal (considering mask)
} else {
  // Records differ: result.second contains explanation
}
```

## Related Files

- **JSONDocumentOverlay.h** - High-level include aggregator for the module
- **ovlKeyValue.h/.cpp** - Base class using these types and flags
- **ovlDatabaseRecord.h/.cpp** - Main record type using comparison flags
- **DataFormats/shared_literals.h** - Defines the string literals used as JSON keys

## Architecture Notes

### Overlay Pattern
The Overlay module implements the **Overlay Pattern**:
- JSON data remains as the underlying storage
- C++ objects provide type-safe, convenient access
- No data duplication - overlays reference the JSON AST directly
- Changes to overlay objects modify the underlying JSON

### Result Type Pattern
The `result_t` type (a pair of bool and string) provides:
- **Success/Failure Indication** - Boolean first element
- **Error Messages** - String second element explains failures
- **Composability** - Results can be aggregated and combined

### Static Mask Management
The `useCompareMask()` function uses a static variable to maintain global comparison state:
- **Thread Consideration** - Not thread-safe; assumes single-threaded comparison operations
- **Global State** - Affects all comparisons until mask is changed
- **Testing** - Tests should reset mask before comparison operations

## Best Practices

1. **Always include this header first** when creating Overlay module files
2. **Use namespace aliases** (`jsonliteral`, not full `artdaq::database::dataformats::literal`)
3. **Set comparison masks explicitly** in test code to avoid dependencies on default state
4. **Combine flags with bitwise OR**, never with addition
5. **Use error constants** (`msg_IsReadonly`, etc.) for consistency
6. **Document which flags affect your comparisons** when implementing `operator==`

## Notes

- The comparison flag enum uses bit positions (1 << N) to allow bitwise combination
- Flags at positions 17-27 are unused and reserved for future expansion
- Flags at positions 28-30 are marked "use with caution" due to their broad scope
- The module is designed for read-heavy workloads with occasional updates
- All overlay classes assume valid JSON structure; invalid JSON causes undefined behavior
