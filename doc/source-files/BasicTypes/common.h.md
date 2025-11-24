# common.h

## File Overview

**Location**: `/home/user/artdaq-database/artdaq-database/BasicTypes/common.h`

This header file provides common utilities and dependencies used throughout the BasicTypes module. It serves as a centralized location for shared includes that are needed by multiple files in the module.

**Purpose**: Provides common includes and utilities for the BasicTypes module, primarily for TRACE logging support and type introspection.

## Dependencies

### Active Dependencies

- `"trace.h"` - TRACE logging framework for debug and diagnostic output
- `<boost/core/demangle.hpp>` - Boost utility for demangling C++ type names (useful for debugging and logging)

### Commented Out Dependencies

The file contains several commented-out includes that were previously used or may be used in the future:

```cpp
// #include <cassert>      // Assertions
// #include <ctime>        // Time operations
// #include <iostream>     // Standard I/O
// #include <iterator>     // Iterator utilities
// #include <memory>       // Smart pointers
// #include <regex>        // Regular expressions
// #include <string>       // String class
// #include <tuple>        // Tuple utilities
// #include <type_traits>  // Type traits
// #include <vector>       // Vector container
```

These were likely consolidated elsewhere or found to be unnecessary in the common header.

## Key Types/Classes

No types or classes are defined in this header.

## Functions/Methods

No functions are defined in this header.

## Constants/Literals

None defined.

## TRACE Logging

The primary purpose of this header is to ensure TRACE support is available. TRACE is a lightweight logging framework used throughout the artdaq project.

### What is TRACE?

TRACE provides:
- Low-overhead logging
- Configurable log levels
- Runtime filtering of log messages
- Minimal impact on performance when disabled

### Usage in BasicTypes

All data type headers (`data_fhicl.h`, `data_json.h`, `data_xml.h`) include this header to get TRACE support. They typically:

1. Undefine any existing `TRACE_NAME`
2. Define their own `TRACE_NAME`
3. Use TRACE macros for logging

Example pattern seen in data_*.h files:
```cpp
#include "artdaq-database/BasicTypes/common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "data_json.h"
```

## Boost Demangle Utility

### Purpose of boost::core::demangle

The `boost::core::demangle` function converts mangled C++ type names into human-readable form:

```cpp
#include <boost/core/demangle.hpp>

// Mangled name: "N6artdaq8database10basictypes8JsonDataE"
// Demangled: "artdaq::database::basictypes::JsonData"
```

### Typical Usage

```cpp
template<typename T>
void logType() {
    std::string name = boost::core::demangle(typeid(T).name());
    TLOG(10) << "Type: " << name;
}
```

### Use Cases in BasicTypes

- Debugging type conversion issues
- Logging type information during data transformations
- Error messages that need to report type names
- Template instantiation diagnostics

## Usage Context

This header is included by:
- `basictypes.h` - Main module header
- `data_fhicl.h` - FHICL data type
- `data_json.h` - JSON data type
- `data_xml.h` - XML data type

It provides foundational utilities that all BasicTypes components depend on.

## Include Order

When using BasicTypes headers, the include chain is:

```
Your Code
    ↓
#include "basictypes.h" or specific data_*.h
    ↓
#include "common.h"
    ↓
#include "trace.h" and <boost/core/demangle.hpp>
```

## Header Guards

```cpp
#ifndef _ARTDAQ_DATABASE_BASICTYPES_COMMON_H_
#define _ARTDAQ_DATABASE_BASICTYPES_COMMON_H_
```

Standard include guard prevents multiple inclusion.

## Design Decisions

### Why Comment Out Includes?

The commented-out includes suggest an evolution in the codebase:
- **Original design**: Put all common includes here
- **Current design**: Only include what's truly needed everywhere
- **Benefit**: Faster compilation, clearer dependencies

### Why Keep Comments?

The comments serve as:
- Documentation of previously used dependencies
- Quick reference for developers who need to add includes
- History of what was considered "common"

## Notes for Developers

### Adding New Common Dependencies

When adding a new dependency that ALL BasicTypes components need:

1. Add it to this header
2. Update this documentation
3. Remove it from individual data_*.h files
4. Consider the compilation time impact

### Removing Common Dependencies

When removing an include:

1. Comment it out first (don't delete immediately)
2. Ensure no BasicTypes file depends on it transitively
3. After verification, the comment can be removed in a future cleanup

### TRACE Configuration

Each file defines its own `TRACE_NAME` after including this header:
- This allows filtering log messages by component
- Standard pattern throughout artdaq projects
- Makes debugging easier by identifying message source

## Best Practices

1. **Keep it minimal**: Only add truly common dependencies
2. **Document changes**: Update this file when modifying common.h
3. **Consider compilation time**: Every include here affects all BasicTypes files
4. **Use TRACE appropriately**: Follow artdaq logging guidelines

## Related Documentation

- TRACE documentation: See artdaq project documentation
- Boost demangle: https://www.boost.org/doc/libs/release/libs/core/doc/html/core/demangle.html
- artdaq logging standards: See project coding guidelines

## Examples

### Using TRACE (typical pattern in BasicTypes)

```cpp
#include "artdaq-database/BasicTypes/common.h"

#define TRACE_NAME "my_component"

void myFunction() {
    TLOG(10) << "Debug message";
    TLOG(5) << "Important message";
}
```

### Using boost::core::demangle

```cpp
#include "artdaq-database/BasicTypes/common.h"

template<typename T>
void processData(const T& data) {
    auto typeName = boost::core::demangle(typeid(T).name());
    TLOG(10) << "Processing type: " << typeName;
}
```
