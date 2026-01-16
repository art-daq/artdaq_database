# common.h

**Path:** `artdaq-database/JsonDocument/common.h`

**Purpose:** This is a convenience aggregator header that provides all commonly used dependencies for the JsonDocument module. It serves as a central include point ensuring consistent usage of JSON data types, exceptions, literals, and shared utilities throughout the module, reducing boilerplate and simplifying dependency management.


## Key Concepts

### Aggregator Header Pattern

Rather than requiring each source file to include multiple individual headers, `common.h` bundles them together:
- **Reduces boilerplate** in implementation files
- **Ensures consistent imports** across the module
- **Simplifies maintenance** when dependencies change
- **Provides a clear module boundary**

### Module Dependency Organization

The header imports dependencies in a logical order:
1. **JSON Data Formats** - Core type definitions from DataFormats module
2. **Module Exceptions** - Error handling types specific to document records
3. **Module Literals** - String constants and action names
4. **Shared Utilities** - Common project-wide utilities

## Thread Safety

- **Thread-safe:** Yes (header-only, provides type definitions only)
- **Concurrent access:** All provided types are inherently thread-safe for reading
- **Locking:** No internal locks (no code execution)

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/Json/json_common.h` | Core JSON data type definitions (`value_t`, `object_t`, `array_t`, `type_t`), `JsonReader`, `JsonWriter`, type visitors |
| `artdaq-database/JsonDocument/docrecord_exceptions.h` | Document record-specific exceptions (`notfound_exception`, `readonly_exception`) |
| `artdaq-database/JsonDocument/docrecord_literals.h` | String literals, action names (`actions::` namespace), and template constants |
| `artdaq-database/SharedCommon/sharedcommon_common.h` | Project-wide utilities, result types (`result_t`, `Success()`, `Failure()`), path types (`path_t`), helper functions |

## What This Header Provides

### From json_common.h

**Brief:** Core JSON data types and utilities for variant-based JSON representation.

| Symbol | Type | Purpose |
|--------|------|---------|
| `value_t` | Type alias | Boost variant type for JSON values (string, number, bool, null, object, array) |
| `object_t` | Type alias | JSON object type (ordered key-value container) |
| `array_t` | Type alias | JSON array type (ordered element container) |
| `type_t` | Enumeration | Enumeration for runtime JSON type identification |
| `JsonReader` | Class | JSON string parsing utility |
| `JsonWriter` | Class | JSON string serialization utility |
| `print_visitor` | Functor | Variant visitor for debug output |
| `tostring_visitor` | Functor | Variant visitor for string conversion |

### From docrecord_exceptions.h

**Brief:** Custom exception types for document record error handling.

| Symbol | Type | Purpose |
|--------|------|---------|
| `notfound_exception` | Class | Thrown when paths or elements are not found in documents |
| `readonly_exception` | Class | Thrown when attempting to modify protected documents |

### From docrecord_literals.h

**Brief:** Compile-time string constants for document operations.

| Symbol | Type | Purpose |
|--------|------|---------|
| `actions::addAlias` | `constexpr` | Action constant for adding aliases |
| `actions::setentity` | `constexpr` | Action constant for setting entities |
| `actions::removeAlias` | `constexpr` | Action constant for removing aliases |
| `actions::addToGlobalConfig` | `constexpr` | Action constant for global configuration operations |
| `actions::setVersion` | `constexpr` | Action constant for version updates |
| `actions::markDeleted` | `constexpr` | Action constant for soft delete |
| `actions::markReadonly` | `constexpr` | Action constant for write protection |
| `template__empty_document` | `constexpr` | Empty document template string `"{}\n"` |

### From sharedcommon_common.h

**Brief:** Project-wide shared utilities and types.

| Symbol | Type | Purpose |
|--------|------|---------|
| `path_t` | Type alias | Path type alias for document navigation |
| `result_t` | Type alias | Operation result type for error handling |
| `Success()` | Function | Result factory for successful operations |
| `Failure()` | Function | Result factory for failed operations |
| `confirm()` | Macro | Assertion macro for preconditions |
| `quoted_()` | Function | String quoting utility |
| Helper functions | Various | Common utility functions |

## Relationship to Other Components

### Within the JsonDocument Module

All implementation files include this header:
- `JSONDocument.h` - Includes for type definitions
- `JSONDocument.cpp` - Includes for implementation
- `JSONDocumentBuilder.h` - Includes for types and literals
- `JSONDocumentBuilder.cpp` - Includes for implementation
- `JSONDocumentMigrator.h` - Includes for types
- `JSONDocumentMigrator.cpp` - Includes for implementation
- `JSONDocument_utils.cpp` - Includes for utilities

### Dependency Chain

```
common.h
  +-- DataFormats/Json/json_common.h
  |     +-- (Boost headers)
  |     +-- (Standard library)
  +-- JsonDocument/docrecord_exceptions.h
  |     +-- SharedCommon/shared_exceptions.h
  +-- JsonDocument/docrecord_literals.h
  |     (no dependencies)
  +-- SharedCommon/sharedcommon_common.h
        +-- (various shared headers)
```

### What Is NOT Included

This header intentionally does not include:
- Implementation-specific headers (like `json_types_impl.h`)
- Boost headers directly (included transitively through json_common.h)
- Standard library headers directly (included transitively)
- External module headers (like Overlay module)
- Heavy template implementations

Implementation files that need additional dependencies must include them explicitly.

## Usage Examples

### In Header Files

```cpp
// JSONDocument.h
#ifndef _ARTDAQ_DATABASE_JSONDOCUMENT_H_
#define _ARTDAQ_DATABASE_JSONDOCUMENT_H_

#include "artdaq-database/JsonDocument/common.h"

namespace artdaq::database::docrecord {

class JSONDocument {
  // Can use value_t, object_t, array_t from json_common.h
  // Can use notfound_exception from docrecord_exceptions.h
  // Can use path_t, result_t from sharedcommon_common.h
};

}  // namespace artdaq::database::docrecord
#endif
```

### In Implementation Files

```cpp
// JSONDocument.cpp
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/common.h"  // Explicit for clarity

using namespace artdaq::database::docrecord;

// All symbols from common.h are available:
// - value_t, object_t, array_t, type_t
// - notfound_exception, readonly_exception
// - actions::markDeleted, template__empty_document
// - result_t, Success(), Failure()
```

## Notes for Developers

### Header Guard Naming

The guard uses `JSONUTILS` rather than `JSONDOCUMENT`:
```cpp
#ifndef _ARTDAQ_DATABASE_JSONUTILS_COMMON_H_
#define _ARTDAQ_DATABASE_JSONUTILS_COMMON_H_
```

This may be a historical artifact from when the module was called "JsonUtils".

### Best Practices

1. **Always include in .cpp files** - After the module's own header for implementation files
2. **Include in module headers** - For type definitions needed in the public interface
3. **Trust the aggregation** - Do not redundantly include what common.h already provides
4. **Use for internal implementation** - External users of the module should include specific public headers

### Design Benefits

1. **Centralized Dependencies** - All core dependencies in one place
2. **Consistent Imports** - All files have access to the same types
3. **Simplified Maintenance** - Update dependencies in one location
4. **Reduced Boilerplate** - Single include instead of multiple
5. **Clear Module Boundary** - Defines what is available module-wide
6. **Compilation Efficiency** - Precompiled header friendly

## See Also

- [docrecord_exceptions.h.md](./docrecord_exceptions.h.md) - Exception type definitions
- [docrecord_literals.h.md](./docrecord_literals.h.md) - String literal constants
- [JSONDocument.h.md](./JSONDocument.h.md) - Core document class
- [JSONDocumentBuilder.h.md](./JSONDocumentBuilder.h.md) - Builder class
- [../DataFormats/Json/json_common.h.md](../DataFormats/Json/json_common.h.md) - JSON type definitions
- [../SharedCommon/sharedcommon_common.h.md](../SharedCommon/sharedcommon_common.h.md) - Shared utilities
