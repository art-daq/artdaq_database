# common.h

## File Overview

This is a minimal aggregator header file that provides access to common functionality from the SharedCommon module for all StorageProviders code. It serves as a convenience include to avoid repetitive header inclusions across the provider implementations.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/common.h`

**Lines of Code**: 7

**Purpose**: Central include point for common utilities used by storage provider implementations

## Dependencies

### Project Headers
- `"artdaq-database/SharedCommon/common.h"` - Standard library includes and common utilities

## File Structure

```cpp
#ifndef _ARTDAQ_DATABASE_PROVIDERS_COMMON_H_
#define _ARTDAQ_DATABASE_PROVIDERS_COMMON_H_

#include "artdaq-database/SharedCommon/common.h"

#endif /* _ARTDAQ_DATABASE_PROVIDERS_COMMON_H_ */
```

## Description

This header file is an aggregator that includes the SharedCommon's `common.h` header, which in turn provides:
- Standard C++ library includes (vector, string, map, etc.)
- Boost library includes
- TRACE debugging system

By including this single header, all StorageProvider implementation files gain access to:
- Standard containers (std::vector, std::string, std::map, etc.)
- Standard I/O (iostream, sstream, fstream)
- Standard algorithms and utilities
- Boost utilities (lexical_cast, demangle)
- TRACE macro for debugging

## Usage Pattern

Storage provider implementation files typically include this header for basic functionality:

```cpp
// In provider implementation files
#include "artdaq-database/StorageProviders/common.h"

// Now have access to all standard library includes
void some_function() {
    std::string name = "example";
    std::vector<int> data = {1, 2, 3};
    // ... etc
}
```

## Design Rationale

### Aggregator Pattern

This file follows the aggregator pattern used throughout artdaq-database:
1. Each module has a `common.h` that includes basic dependencies
2. Implementation files include the module's `common.h` once
3. Reduces boilerplate includes in every source file
4. Provides consistent set of available utilities

### Minimal Design

Unlike some other modules that have extensive aggregator headers, this file is deliberately minimal:
- Only includes SharedCommon's common.h
- Does not define any types or constants
- Pure include aggregation

This minimalism is appropriate because:
- Storage providers have more specific dependencies
- Provider-specific headers are included separately
- Keeps compilation dependencies lean

## Include Hierarchy

```
StorageProviders/common.h
  └─ SharedCommon/common.h
      ├─ Standard Library Headers (<vector>, <string>, <map>, etc.)
      ├─ Boost Headers (lexical_cast, demangle)
      └─ TRACE debugging system
```

## Namespace

This file does not define or open any namespaces. It is purely an include directive.

## Related Files

- **SharedCommon/common.h** - The actual source of common includes
- **storage_providers.h** - Provider template interface
- **FileSystemDB/provider_filedb.h** - Example usage in concrete provider
- **MongoDB/provider_mongodb.h** - Example usage in concrete provider
- **UconDB/provider_ucondb.h** - Example usage in concrete provider

## Compilation Impact

### Header Dependencies
Files that include this header will transitively include all of SharedCommon/common.h dependencies. Changes to SharedCommon/common.h will trigger recompilation of all provider files.

### Include Guards
The header guard `_ARTDAQ_DATABASE_PROVIDERS_COMMON_H_` prevents multiple inclusion within a single translation unit.

## Best Practices

### When to Include

Include this header in:
- Provider implementation (.cpp) files
- Provider private implementation files
- Files that need basic standard library functionality

### When NOT to Include

Avoid including in:
- Public header files that could be used by clients
- Files that don't need standard library functionality (rare)
- Files that only need specific headers (include them directly instead)

### Include Order

Recommended include order in provider files:
```cpp
// 1. Corresponding header
#include "provider_specific.h"

// 2. Common utilities
#include "artdaq-database/StorageProviders/common.h"

// 3. Other project headers
#include "artdaq-database/SharedCommon/helper_functions.h"

// 4. System/third-party headers
#include <mongocxx/client.hpp>
```

## Maintenance Notes

- Keep this file minimal - only include what ALL providers need
- Provider-specific includes belong in provider-specific headers
- Changes to this file affect all providers - modify with care
- Consider compilation time impact when adding new includes

## Historical Context

This aggregator pattern is consistent with other modules in artdaq-database:
- SharedCommon has sharedcommon_common.h
- JsonDocument has JSONDocument_common.h (likely)
- Each module maintains its own common.h namespace

The pattern improves:
- Code maintainability
- Compilation speed (fewer repeated includes)
- Consistency across the codebase

---

**Documentation generated for artdaq-database StorageProviders module**
