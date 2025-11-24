# convertfhicl2jsondb.cpp

## File Overview

Implementation of FHiCL ↔ database JSON conversion with full metadata preservation.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Fhicl/convertfhicl2jsondb.cpp`

## TRACE Configuration

```cpp
#define TRACE_NAME "convertfhicl2jsondb.cpp"
```

## Implementation

Sophisticated conversion handling:
- FHiCL table_t to JSON object_t
- Comment/annotation mapping
- Metadata structure generation
- Bidirectional transformation

## Related Files

- **convertfhicl2jsondb.h** - Function declarations
- **fhicl_types.h** - FHiCL AST
- **json_types.h** - JSON AST
- **helper_functions.h** - Conversion utilities
