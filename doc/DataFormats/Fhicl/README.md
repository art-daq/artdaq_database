# Fhicl Submodule Documentation

## Overview

The Fhicl submodule provides comprehensive support for FHiCL (Fermilab Hierarchical Configuration Language) with full metadata preservation including comments and annotations.

## Files

### Core Types
- **[fhicl_types.h](fhicl_types.h.md)** - FHiCL AST with comment and annotation support
- **[fhicl_types.cpp](fhicl_types.cpp.md)** - Type implementations

### Parsing and Generation
- **[fhicl_reader.h](fhicl_reader.h.md)** - Boost.Spirit Qi parser for FHiCL
- **[fhicl_reader.cpp](fhicl_reader.cpp.md)** - FhiclReader implementation
- **[fhicl_writer.h](fhicl_writer.h.md)** - Boost.Spirit Karma generator for FHiCL
- **[fhicl_writer.cpp](fhicl_writer.cpp.md)** - FhiclWriter implementation

### Conversion Utilities
- **[convertfhicl2jsondb.h](convertfhicl2jsondb.h.md)** - FHiCL ↔ database JSON with metadata
- **[convertfhicl2jsondb.cpp](convertfhicl2jsondb.cpp.md)** - Conversion implementation
- **[fhicljsondb.h](fhicljsondb.h.md)** - High-level database interface
- **[fhicljsondb.cpp](fhicljsondb.cpp.md)** - Database interface implementation

### Utilities
- **[helper_functions.h](helper_functions.h.md)** - FHiCL-specific utility functions
- **[helper_functions.cpp](helper_functions.cpp.md)** - Utility implementations
- **[fhiclcpplib_includes.h](fhiclcpplib_includes.h.md)** - fhiclcpp library integration

### Convenience
- **[fhicl_common.h](fhicl_common.h.md)** - Aggregator header for all FHiCL functionality

## Key Features

### Metadata Preservation

**Comments** (attached to keys):
```fhicl
# This is a comment
timeout: 30
```

**Annotations** (attached to values):
```fhicl
port: 8080  # default port
```

### Rich Type System

```cpp
key_t      // Key with optional comment
value_t    // Value with optional annotation
atom_t     // Key-value pair
table_t    // FHiCL table (like JSON object with metadata)
sequence_t // FHiCL sequence (like JSON array with metadata)
```

## FHiCL Syntax

```fhicl
# Configuration file
server: {
    # Network settings
    host: "localhost"  # server hostname
    port: 8080

    options: [
        "verbose",
        "debug"
    ]
}
```

## Usage Example

```cpp
std::string fhicl = R"(
    # Server configuration
    timeout: 30  # seconds
)";

fhicl::table_t ast;
fhicl::FhiclReader{}.read(fhicl, ast);
// Comments and annotations preserved in AST
```

## Related Files

- [../shared_types.h.md](../shared_types.h.md) - Base templates extended by FHiCL
- [../Json/json_types.h.md](../Json/json_types.h.md) - Similar structure, simpler types
