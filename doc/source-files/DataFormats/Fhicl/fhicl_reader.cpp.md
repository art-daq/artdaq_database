# fhicl_reader.cpp

## File Overview

Implementation of FHiCL parser using Boost.Spirit Qi framework with support for FHiCL-specific syntax features.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Fhicl/fhicl_reader.cpp`

## TRACE Configuration

```cpp
#define TRACE_NAME "fhicl_reader.cpp"
```

## Implementation

Implements FHiCL grammar including:
- Parameter assignments
- Tables and sequences
- Comments and annotations
- #include directives
- Quoted and unquoted strings
- Numbers, booleans, nil

## Related Files

- **fhicl_reader.h** - FhiclReader declaration
- **fhicl_types.h** - AST types
- **helper_functions.h** - Parsing utilities
