# common.h

**Path:** `artdaq-database/StorageProviders/common.h`

**Purpose:** Minimal aggregator header that provides access to common functionality from the SharedCommon module for all StorageProviders code. This serves as a convenience include point to avoid repetitive header inclusions across the provider implementations, ensuring consistent access to standard library utilities and debugging macros.

## Key Concepts

### Aggregator Pattern
This file follows the aggregator pattern used throughout artdaq-database where each module has a central `common.h` that includes basic dependencies. This reduces boilerplate includes in every source file and provides a consistent set of available utilities.

### Minimal Design
Unlike some other modules with extensive aggregator headers, this file is deliberately minimal, containing only a single include directive. This keeps compilation dependencies lean while storage providers use their own specific headers for additional functionality.

## Thread Safety

- **Thread-safe:** Yes
- **Concurrent access:** This is a header-only file with no mutable state
- **Locking:** None required

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/SharedCommon/common.h` | Provides standard library includes (vector, string, map), Boost utilities, and TRACE debugging system |

## Classes/Structures

This file does not define any classes, functions, or constants. It is purely an include aggregator.

## Include Hierarchy

```
StorageProviders/common.h
  |
  +-- SharedCommon/common.h
        |
        +-- Standard Library Headers (<vector>, <string>, <map>, etc.)
        +-- Boost Headers (lexical_cast, demangle)
        +-- TRACE debugging system
```

## Relationship to Other Components

- **SharedCommon module**: Provides the actual common functionality through `SharedCommon/common.h`
- **storage_providers.h**: The main provider template interface that uses these common utilities
- **FileSystemDB/provider_filedb.h**: Includes this header for filesystem provider implementation
- **MongoDB/provider_mongodb.h**: Includes this header for MongoDB provider implementation
- **UconDB/provider_ucondb.h**: Includes this header for UconDB provider implementation

## See Also

- [SharedCommon/common.h](../SharedCommon/common.h.md) - The actual common utilities
- [storage_providers.h](./storage_providers.h.md) - Main provider template interface
- [FileSystemDB/provider_filedb.h](./FileSystemDB/provider_filedb.h.md) - FileSystemDB provider

## Notes for Developers

### When to Include
- Provider implementation (.cpp) files
- Provider private implementation files
- Files that need basic standard library functionality

### When NOT to Include
- Public header files that could be used by clients (include specific headers instead)
- Files that only need specific headers (include them directly to minimize dependencies)

### Compilation Impact
Files that include this header will transitively include all of `SharedCommon/common.h` dependencies. Changes to `SharedCommon/common.h` will trigger recompilation of all provider files.

### Include Guard
The header guard `_ARTDAQ_DATABASE_PROVIDERS_COMMON_H_` prevents multiple inclusion within a single translation unit.
