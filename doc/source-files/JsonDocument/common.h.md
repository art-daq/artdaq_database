# common.h

## File Overview

This is a convenience header file that aggregates commonly used headers and dependencies for the JsonDocument module. It serves as a central include point to ensure consistent usage of JSON data types, exceptions, literals, and shared utilities throughout the JsonDocument module.

**Location**: `/home/user/artdaq-database/artdaq-database/JsonDocument/common.h`

## Dependencies

The file includes four key headers that provide the foundational components for JSON document manipulation:

### External Module Dependencies

**JSON Data Formats**:
- `artdaq-database/DataFormats/Json/json_common.h` - Core JSON data type definitions and utilities

**JsonDocument Module Components**:
- `artdaq-database/JsonDocument/docrecord_exceptions.h` - Document record-specific exception types
- `artdaq-database/JsonDocument/docrecord_literals.h` - String literals and constants for document operations

**Shared Common Utilities**:
- `artdaq-database/SharedCommon/sharedcommon_common.h` - Project-wide common utilities and types

## Header Guard

The file uses a traditional include guard with a descriptive name:
```cpp
#ifndef _ARTDAQ_DATABASE_JSONUTILS_COMMON_H_
#define _ARTDAQ_DATABASE_JSONUTILS_COMMON_H_
...
#endif
```

**Note**: The guard name uses `JSONUTILS` rather than `JSONDOCUMENT`, which may be a historical naming convention from when this module was called "JsonUtils".

## Purpose and Design

This aggregator header simplifies the include structure for JsonDocument module files by providing:

1. **Centralized Dependencies** - All core dependencies in one place
2. **Consistent Imports** - Ensures all JsonDocument files have access to the same types
3. **Simplified Maintenance** - Common dependencies only need to be updated in one location
4. **Reduced Boilerplate** - Implementation files can include just this header

## What This Header Provides

By including `common.h`, JsonDocument implementation files gain access to:

- **JSON Data Types**: `value_t`, `object_t`, `array_t`, `type_t` from the DataFormats module
- **Exception Types**: `notfound_exception`, `readonly_exception` for error handling
- **String Literals**: Action names, field names, and template strings
- **Shared Utilities**: Common types like `path_t`, `result_t`, and helper functions

## Usage Pattern

Files in the JsonDocument module typically follow this include pattern:

```cpp
#include "artdaq-database/JsonDocument/JSONDocument.h"  // Own header
#include "artdaq-database/JsonDocument/common.h"         // Module common
```

Or for the implementation files:

```cpp
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/common.h"
```

## Files That Include common.h

Based on the module structure, the following files include this header:
- `JSONDocument.h` - Main document class
- `JSONDocumentBuilder.h` - Document builder/modifier class
- `JSONDocumentMigrator.h` - Document migration utilities
- `JSONDocumentBuilder.cpp` - Builder implementation
- `JSONDocumentMigrator.cpp` - Migrator implementation
- `JSONDocument_utils.cpp` - Utility functions

## Related Files

- **docrecord_exceptions.h** - Defines exception types included by this header
- **docrecord_literals.h** - Defines string constants included by this header
- **json_common.h** - Provides JSON data type definitions
- **sharedcommon_common.h** - Provides project-wide utilities

## Design Notes

1. **Minimal Interface** - This header only aggregates other headers, containing no code itself
2. **Module Boundary** - Acts as the internal API boundary for the JsonDocument module
3. **Dependency Management** - Centralizes external dependencies for easier tracking
4. **Include Order** - Dependencies are ordered from specific (JSON types) to general (shared utilities)

## Best Practices

When working with JsonDocument module files:

1. **Always include this header** in .cpp files after your module's own header
2. **Don't include in public headers** - Only use in implementation files to reduce compile dependencies
3. **Use forward declarations** in headers when possible
4. **Trust the aggregation** - Don't redundantly include what common.h already provides
