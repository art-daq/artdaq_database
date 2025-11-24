# common.h

## File Overview

This is a convenience header file that aggregates commonly used Standard C++ library headers and third-party library includes. It serves as a precompiled header or common include to reduce repetition across the codebase and ensures consistent library usage throughout the SharedCommon module.

**Location**: `/home/user/artdaq-database/artdaq-database/SharedCommon/common.h`

## Dependencies

### Standard C++ Libraries

The file includes a comprehensive set of standard library headers:

**Algorithms and Iterators**:
- `<algorithm>` - Standard algorithms (sort, find, etc.)
- `<iterator>` - Iterator definitions and utilities

**Containers**:
- `<vector>` - Dynamic array container
- `<tuple>` - Fixed-size heterogeneous container

**String Handling**:
- `<string>` - String class
- `<sstream>` - String stream classes
- `<streambuf>` - Stream buffer classes
- `<regex>` - Regular expression support

**Memory Management**:
- `<memory>` - Smart pointers (unique_ptr, shared_ptr)

**I/O Operations**:
- `<iostream>` - Standard input/output streams
- `<fstream>` - File input/output streams
- `<iomanip>` - I/O manipulators (setw, setprecision)

**Time and Chrono**:
- `<chrono>` - Time utilities and duration types

**Type Utilities**:
- `<type_traits>` - Type trait templates
- `<functional>` - Function objects and utilities
- `<cstddef>` - Size_t, nullptr_t, etc.
- `<cstdio>` - C standard I/O (printf, etc.)

### Third-Party Libraries

**Boost Libraries**:
- `<boost/core/demangle.hpp>` - Utilities for demangling C++ type names
- `<boost/lexical_cast.hpp>` - Type conversion utilities

**Trace Library**:
- `"trace.h"` - TRACE debugging/logging facility (artdaq tracing system)

## Header Guard

The file uses a traditional include guard:
```cpp
#ifndef _ARTDAQ_DATABASE_COMMON_H_
#define _ARTDAQ_DATABASE_COMMON_H_
...
#endif
```

## Constants/Literals

The file contains a commented-out macro definition:
```cpp
// #define extra_traces 0
```
This suggests there's an option for enabling extra trace output that is currently disabled.

## Usage Context

This header is included by most `.cpp` files in the SharedCommon module and serves as a foundation for:

1. **String manipulation** - Using std::string, sstream, regex
2. **Container operations** - Using vector, iterators, algorithms
3. **Type introspection** - Using boost::demangle for readable type names
4. **Memory management** - Using smart pointers
5. **Time operations** - Using std::chrono for timestamps
6. **Debugging** - Using TRACE macros for logging

### Common Include Pattern

Many source files in the project use this pattern:
```cpp
#include "artdaq-database/SharedCommon/common.h"

// Now all standard library components are available
std::vector<std::string> data;
std::stringstream ss;
auto ptr = std::make_unique<MyClass>();
```

### Files That Include common.h

Based on the codebase exploration, the following SharedCommon files include this header:
- `fileststem_functions.cpp`
- `helper_functions.cpp`
- `printStackTrace.cpp`
- And likely many others throughout the project

## Design Rationale

**Benefits of this approach**:
1. **Reduces boilerplate** - Source files don't need to individually include common headers
2. **Consistency** - Ensures all modules use the same standard library components
3. **Faster compilation** - Can be used as a precompiled header
4. **Easier maintenance** - Adding a commonly-needed header only requires one change

**Potential drawbacks**:
1. **Compilation dependencies** - Changes to this file trigger recompilation of many files
2. **Namespace pollution** - Includes more than some files might need
3. **Longer initial compile** - First compilation includes everything

## Best Practices

When using this header:

1. **Include it first** in your .cpp files (after your own module's header):
   ```cpp
   #include "my_module.h"
   #include "artdaq-database/SharedCommon/common.h"
   ```

2. **Don't include it in other headers** - Only include in .cpp implementation files to minimize dependencies

3. **Add specific includes in headers** - If your header file needs a type, include it explicitly

4. **Use TRACE macros for debugging**:
   ```cpp
   TLOG(10) << "Debug message";
   ```

## Related Files

- **sharedcommon_common.h** - A higher-level aggregator that includes this file along with other SharedCommon utilities
- **trace.h** - The tracing/logging system included by this file

## Notes

- This is a pure aggregator header with no actual code implementation
- The boost libraries are used for type demangling (showing readable C++ type names) and type conversions
- The TRACE system is the artdaq-standard logging mechanism
- Including chronos support suggests time-stamped operations are common in the codebase
