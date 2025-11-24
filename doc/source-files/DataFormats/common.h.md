# common.h

## File Overview

This is a convenience header file that aggregates commonly used includes for the DataFormats module. It serves as a central point to include both Boost exception handling utilities and the SharedCommon module's comprehensive header collection.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/common.h`

## Dependencies

### Third-Party Libraries

**Boost Libraries**:
- `<boost/exception/diagnostic_information.hpp>` - Utilities for extracting diagnostic information from Boost exceptions

### Project Headers

- `"artdaq-database/SharedCommon/sharedcommon_common.h"` - Aggregates all common SharedCommon utilities, standard library headers, and trace functionality

## Header Guard

The file uses a traditional include guard:
```cpp
#ifndef _ARTDAQ_DATABASE_DATAFORMATS_FHICL_COMMON_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_FHICL_COMMON_H_
...
#endif
```

**Note**: Despite the header guard name containing "FHICL", this is actually the common header for the entire DataFormats module, not just the Fhicl submodule.

## Macro Definitions

### BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT

```cpp
#define BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT 1
```

**Purpose**: Enables relaxed variant access with `boost::get<T>()`.

**Behavior**:
- **Relaxed mode (enabled)**: `boost::get<T>(variant)` returns nullptr if the variant doesn't contain type T
- **Strict mode (disabled)**: `boost::get<T>(variant)` throws `boost::bad_get` exception if types don't match

**Rationale**: The DataFormats module extensively uses `boost::variant` for handling different data types (JSON, XML, FHiCL, etc.). Relaxed mode provides more convenient error handling by allowing null checks instead of exception handling.

**Usage Example**:
```cpp
boost::variant<int, std::string> v = 42;

// With relaxed mode:
if (auto* str = boost::get<std::string>(&v)) {
    // Handle string case
} else if (auto* num = boost::get<int>(&v)) {
    // Handle int case - this branch executes
}

// Without relaxed mode, you'd need try-catch:
try {
    auto str = boost::get<std::string>(v);
} catch (boost::bad_get&) {
    // Handle wrong type
}
```

## Usage Context

This header is included by most implementation files in the DataFormats module and its submodules (Json, Xml, Conf, Fhicl). It provides:

1. **Exception diagnostics** - Using `boost::diagnostic_information()` for detailed error reporting
2. **Standard library components** - Via the SharedCommon aggregator
3. **TRACE logging** - Via the SharedCommon aggregator
4. **Boost variant utilities** - Configured for relaxed access mode

### Common Include Pattern

Source files in DataFormats typically use this pattern:
```cpp
#include "artdaq-database/DataFormats/common.h"

// Now all standard library, boost, and trace components are available
using namespace artdaq::database;

boost::variant<int, std::string> data;
auto* value = boost::get<int>(&data);  // Relaxed mode enabled
```

## Design Rationale

**Benefits**:
1. **Centralized configuration** - The BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT setting applies uniformly across all DataFormats code
2. **Reduces boilerplate** - Source files don't need to individually include Boost exception and SharedCommon headers
3. **Consistency** - Ensures all DataFormats modules use the same boost::variant behavior
4. **Easier maintenance** - Changes to common dependencies only require updating this one file

**Considerations**:
1. **Compilation dependencies** - Changes to this file trigger recompilation of many DataFormats files
2. **Namespace pollution** - Includes more than some files might strictly need

## Related Files

- **sharedcommon_common.h** - The aggregator header that provides standard library includes, Boost utilities, and TRACE functionality
- **shared_types.h** - Defines type aliases and template structures that use boost::variant extensively
- **Json/Xml/Conf/Fhicl submodules** - All include this header for consistent configuration

## Best Practices

When using this header:

1. **Include it first** in your .cpp files (after your module's own header):
   ```cpp
   #include "artdaq-database/DataFormats/MyModule/my_types.h"
   #include "artdaq-database/DataFormats/common.h"
   ```

2. **Don't include in headers** - Only include in .cpp implementation files to minimize dependencies

3. **Use relaxed boost::get** - Take advantage of the relaxed mode:
   ```cpp
   // Preferred pattern with relaxed mode
   if (auto* ptr = boost::get<Type>(&variant)) {
       // Use *ptr
   }
   ```

4. **Exception diagnostics** - Use boost::diagnostic_information for detailed error messages:
   ```cpp
   try {
       // Some operation
   } catch (...) {
       TLOG(TLVL_ERROR) << boost::current_exception_diagnostic_information();
       throw;
   }
   ```

## Notes

- This is a pure aggregator header with no code implementation
- The header guard naming ("FHICL") appears to be a legacy naming issue but doesn't affect functionality
- The relaxed variant mode is essential for the DataFormats module's extensive use of boost::variant for handling polymorphic data structures
- Boost exception diagnostics provide much more detailed error information than standard C++ exceptions, including stack traces and context
