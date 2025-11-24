# basictypes.h

## File Overview

**Location**: `/home/user/artdaq-database/artdaq-database/BasicTypes/basictypes.h`

This is the main convenience header file for the BasicTypes module. It serves as a single point of inclusion for all the major data type headers in the module, making it easy for client code to access all BasicTypes functionality with a single `#include`.

**Purpose**: Provides a unified include point for all basic data types (FHICL, JSON, and XML) used throughout the artdaq-database system.

## Dependencies

This header includes:
- `artdaq-database/BasicTypes/common.h` - Common utilities and trace support
- `artdaq-database/BasicTypes/data_fhicl.h` - FHICL data type
- `artdaq-database/BasicTypes/data_json.h` - JSON data type
- `artdaq-database/BasicTypes/data_xml.h` - XML data type

## Key Types/Classes

This header does not define any types directly. It provides access to the following types through its includes:

### Available Types (via includes)
- `artdaq::database::basictypes::FhiclData` - FHICL configuration data wrapper
- `artdaq::database::basictypes::JsonData` - JSON data wrapper
- `artdaq::database::basictypes::XmlData` - XML data wrapper

## Functions/Methods

No functions are defined in this header. It is purely an aggregation header.

## Constants/Literals

None defined.

## Usage Context

### When to Use This Header

Use `basictypes.h` when:
1. You need multiple data types (FHICL, JSON, XML) in the same file
2. You want convenience over granular includes
3. You're writing high-level code that works with multiple configuration formats

### When to Use Individual Headers

Use individual headers (`data_fhicl.h`, `data_json.h`, `data_xml.h`) when:
1. You only need one specific data type
2. You want to minimize compilation dependencies
3. Compile time is a concern for large projects

### Usage Example

```cpp
#include "artdaq-database/BasicTypes/basictypes.h"

using namespace artdaq::database::basictypes;

void processConfiguration() {
    // Now you have access to all three types:
    FhiclData fhicl("parameter: value");
    JsonData json("{\"parameter\": \"value\"}");
    XmlData xml("<config><parameter>value</parameter></config>");

    // Convert between formats
    JsonData fromFhicl = fhicl;  // FHICL → JSON
    FhiclData backToFhicl = fromFhicl;  // JSON → FHICL
}
```

## Files That Include This Header

Common use cases throughout the artdaq-database project:
- Configuration management modules
- Database interface layers
- Data conversion utilities
- Test files that exercise multiple formats

## Include Structure

```
basictypes.h
├── common.h
│   ├── trace.h (TRACE logging)
│   └── boost/core/demangle.hpp
├── data_fhicl.h
│   └── common.h
├── data_json.h
│   └── common.h
└── data_xml.h
    └── common.h
```

## Design Pattern

This header follows the **Aggregation Header** pattern:
- **Benefit**: Single include for entire module
- **Trade-off**: Includes more than might be needed
- **Use case**: Convenience for users who need multiple types

## Header Guards

```cpp
#ifndef _ARTDAQ_DATABASE_BASICTYPES_H_
#define _ARTDAQ_DATABASE_BASICTYPES_H_
```

Standard include guard prevents multiple inclusion.

## Notes for Developers

1. **Maintenance**: When adding new basic types to the module, add their headers here
2. **Compilation Impact**: Including this header pulls in FHICL, JSON, and XML processing capabilities
3. **Namespace**: All types are in `artdaq::database::basictypes` namespace
4. **Design Philosophy**: This follows the standard library pattern (e.g., `<iostream>` includes multiple stream types)

## Related Documentation

For detailed information about each data type, see:
- `data_fhicl.h.md` - FHICL data type documentation
- `data_json.h.md` - JSON data type documentation
- `data_xml.h.md` - XML data type documentation
- `common.h.md` - Common utilities documentation

## Best Practices

1. **In header files**: Prefer including specific headers (`data_json.h`) to minimize dependencies
2. **In source files**: Feel free to use this header for convenience
3. **In libraries**: Avoid exposing this in public APIs to reduce coupling
4. **In applications**: This is the preferred way to include BasicTypes functionality
