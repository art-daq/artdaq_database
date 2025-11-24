# fhicl_common.h

## File Overview

Convenience aggregator header providing all FHiCL DataFormat functionality.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Fhicl/fhicl_common.h`

## Dependencies

### Project Headers
- `"artdaq-database/DataFormats/Fhicl/fhicl_reader.h"` - FHiCL parser
- `"artdaq-database/DataFormats/Fhicl/fhicl_writer.h"` - FHiCL generator
- `"artdaq-database/DataFormats/Fhicl/fhicl_types.h"` - Type definitions
- `"artdaq-database/DataFormats/common.h"` - Common infrastructure

## Purpose

Single include for:
- FHiCL parsing
- FHiCL generation
- FHiCL type system
- Common infrastructure

## Usage

```cpp
#include "artdaq-database/DataFormats/Fhicl/fhicl_common.h"

std::string fhicl = "parameter: value\n";
fhicl::table_t ast;

fhicl::FhiclReader{}.read(fhicl, ast);
```

## Related Files

- **fhicl_reader.h** - Parser
- **fhicl_writer.h** - Generator
- **fhicl_types.h** - Type system
