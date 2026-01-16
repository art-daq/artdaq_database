# common.h

**Path:** `artdaq-database/BasicTypes/common.h`

**Purpose:** Provides common includes and utilities shared across all BasicTypes headers. This header establishes the foundational dependencies for the module, including TRACE logging for diagnostics and Boost.Core utilities for type introspection.


## Key Concepts

### Module Foundation Header

This header serves as the foundation for all BasicTypes headers by providing:

1. **TRACE Logging:** The artdaq ecosystem's high-performance logging framework for debug and diagnostic output
2. **Boost.Core Utilities:** Specifically `demangle.hpp` for converting mangled C++ type names to human-readable form
3. **Consistent Setup:** Ensures all BasicTypes headers have the same baseline dependencies

### TRACE Logging Framework

TRACE is the artdaq ecosystem's logging framework, providing:

- **Compile-time filtering:** Log statements below a threshold can be compiled out entirely
- **Runtime control:** Log levels can be adjusted at runtime without recompilation
- **High performance:** Minimal overhead when logging is disabled
- **Source identification:** Each file defines its own `TRACE_NAME` for filtering

### Type Demangling

The `boost::core::demangle()` function converts compiler-generated mangled type names into human-readable form, which is useful for debugging template-heavy code like BasicTypes.

## Thread Safety

- **Thread-safe:** N/A (header-only, defines no runtime state)
- **Concurrent access:** TRACE logging is thread-safe
- **Locking:** See TRACE documentation for internal synchronization behavior

## Dependencies

| Include | Purpose |
|---------|---------|
| `trace.h` | TRACE logging framework - provides `TLOG()` macro and `TraceStreamer` class for high-performance diagnostic output |
| `<boost/core/demangle.hpp>` | Converts mangled C++ type names to human-readable strings for debugging |

### Commented-Out Dependencies (Historical Reference)

The header contains commented-out includes that document dependencies used across the module but not needed in every file:

| Include | Where Used |
|---------|-----------|
| `<cassert>` | Assertions in debug builds |
| `<ctime>` | Time utilities (not currently used in BasicTypes) |
| `<iostream>` | Stream I/O in .cpp files |
| `<iterator>` | Iterator utilities in .cpp files |
| `<memory>` | Smart pointers (not currently used in BasicTypes) |
| `<regex>` | Regular expressions in conversion functions |
| `<string>` | String operations - included by individual headers as needed |
| `<tuple>` | Tuple utilities (not currently used in BasicTypes) |
| `<type_traits>` | Type introspection (included where needed) |
| `<vector>` | Dynamic arrays (not currently used in BasicTypes) |

These are included directly by the source files that need them, following the principle of minimal includes.

## Provided Facilities

### TRACE Logging

**Brief:** TRACE provides high-performance, filterable logging for debugging and diagnostics across all BasicTypes files.

All BasicTypes files use TRACE for logging with this pattern:

```cpp
// At the top of each file, after includes:
#ifdef TRACE_NAME
#undef TRACE_NAME
#endif
#define TRACE_NAME "data_fhicl.cpp"  // Unique per file

// In code:
TLOG(11) << "Converting JSON to FHICL, buffer size: " << buffer.size();
TLOG(12) << "Base64 encoded string: " << base64_str;
```

**Thread Safety:** TRACE logging operations are thread-safe.

**TRACE Levels Used in BasicTypes:**

| Level Range | Purpose |
|-------------|---------|
| TLOG(1-5) | Errors and important warnings |
| TLOG(10-21) | Detailed conversion steps for debugging |

**Example with Error Handling:**
```cpp
#include "artdaq-database/BasicTypes/common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif
#define TRACE_NAME "my_module.cpp"

void processConfiguration(const std::string& config_data) {
  try {
    TLOG(10) << "Processing configuration, size: " << config_data.size();

    if (config_data.empty()) {
      TLOG(1) << "Error: Empty configuration data received";
      throw std::invalid_argument("Configuration data cannot be empty");
    }

    // Process the configuration...
    TLOG(11) << "Configuration processed successfully";

  } catch (const std::exception& e) {
    TLOG(1) << "Exception during configuration processing: " << e.what();
    throw;
  }
}
```

### Boost Demangle

**Brief:** Converts mangled type names to human-readable form for debugging template-heavy code.

**Thread Safety:** Thread-safe (read-only operation on static type information).

**Example:**
```cpp
#include "artdaq-database/BasicTypes/common.h"
#include <typeinfo>
#include <iostream>

template<typename T>
void printTypeName(const T& value) {
  // Without demangle: "N6artdaq8database10basictypes8JsonDataE"
  // With demangle: "artdaq::database::basictypes::JsonData"
  std::cout << "Type: " << boost::core::demangle(typeid(value).name()) << "\n";
}

void debugTypeInformation() {
  try {
    artdaq::database::basictypes::JsonData json(R"({"key": "value"})");
    printTypeName(json);  // Output: artdaq::database::basictypes::JsonData
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

## Relationship to Other Components

```
                common.h
                   |
    +--------------+--------------+
    |              |              |
data_json.h   data_fhicl.h   data_xml.h
    |              |              |
    +--------------+--------------+
                   |
             basictypes.h
```

Every BasicTypes header includes `common.h` to ensure:
- Consistent TRACE setup across all files
- Availability of Boost utilities
- Standard module structure

## See Also

- [basictypes.h](./basictypes.h.md) - Umbrella header including all types
- [data_json.h](./data_json.h.md) - Uses common.h for TRACE and defines TraceStreamer specialization
- [data_fhicl.h](./data_fhicl.h.md) - Uses common.h for TRACE and defines TraceStreamer specialization
- [data_xml.h](./data_xml.h.md) - Uses common.h for TRACE and defines TraceStreamer specialization
- [External: TRACE](https://cdcvs.fnal.gov/redmine/projects/trace) - TRACE logging documentation
- [External: Boost.Core](https://www.boost.org/doc/libs/release/libs/core/) - Boost.Core documentation including demangle

## Notes for Developers

### Common Pitfalls

- **Pitfall 1:** Forgetting to `#undef TRACE_NAME` before `#define TRACE_NAME`. This causes compiler warnings about macro redefinition.
- **Pitfall 2:** Defining `TRACE_NAME` in header files. This should only be done in source files to avoid conflicts when headers are included by multiple translation units.
- **Pitfall 3:** Using TRACE macros in header-only code without checking that TRACE_NAME is defined.

### TRACE_NAME Pattern

Every BasicTypes source file follows this pattern:

```cpp
// After all #includes:
#ifdef TRACE_NAME
#undef TRACE_NAME
#endif
#define TRACE_NAME "filename.cpp"

// Now TLOG() calls will identify this file
TLOG(10) << "Processing configuration...";
```

This ensures clean TRACE output identifying the source of each log message, which is essential for debugging complex conversion chains.

### Design Philosophy

The commented-out includes document what was historically considered "common" but are now included only where needed. This follows the principle:

1. **Minimal includes:** Only include what is truly needed everywhere
2. **Documented history:** Comments show what might be needed in specific files
3. **Faster compilation:** Fewer unnecessary includes reduces build time

### Adding New Common Dependencies

When adding a new dependency that ALL BasicTypes components need:

1. Add the include to common.h
2. Update this documentation
3. Remove duplicate includes from individual data_*.h files
4. Consider the compilation time impact - is it truly needed everywhere?

If the dependency is only needed by some files, include it directly in those files instead.

### Anti-patterns

```cpp
// DON'T do this - defining TRACE_NAME in a header:
// myheader.h
#define TRACE_NAME "myheader.h"  // Wrong! Will conflict when included by multiple .cpp files

// DO this instead - define TRACE_NAME only in .cpp files:
// mysource.cpp
#ifdef TRACE_NAME
#undef TRACE_NAME
#endif
#define TRACE_NAME "mysource.cpp"  // Correct!
```
