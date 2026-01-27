# common.h

**Path:** `artdaq-database/DataFormats/common.h`

**Purpose:** Provides a centralized configuration header for the DataFormats module. This file aggregates Boost exception handling utilities, includes the SharedCommon foundation layer, and configures the `boost::variant` library for relaxed access mode. All DataFormats submodules (Json, Xml, FHiCL, Conf) include this header to ensure consistent behavior.


## Key Concepts

### Relaxed Variant Access Mode

The `BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT` macro enables a more convenient way to work with `boost::variant` types:

- **Relaxed mode (enabled):** When calling `boost::get<T>(&variant)` with a pointer, it returns `nullptr` if the variant does not contain type T, rather than throwing an exception.
- **Strict mode (disabled):** Calling `boost::get<T>(variant)` throws `boost::bad_get` if types do not match.

This is particularly important because the DataFormats module extensively uses `boost::variant` to represent polymorphic data structures (JSON values that can be objects, arrays, strings, numbers, or booleans).

### Boost Exception Diagnostics

The `boost::diagnostic_information()` function provides detailed error information when exceptions occur, including:
- Exception type
- Error message
- File and line information (if available)
- Nested exception information

## Thread Safety

- **Thread-safe:** Yes
- **Concurrent access:** This header only defines macros and includes other headers; it introduces no shared state
- **Locking:** None required

## Dependencies

| Include | Purpose |
|---------|---------|
| `<boost/exception/diagnostic_information.hpp>` | Provides `boost::diagnostic_information()` for extracting detailed error messages from Boost exceptions |
| `"artdaq-database/SharedCommon/sharedcommon_common.h"` | Aggregates standard library headers, TRACE logging, and common utilities used throughout artdaq-database |

## Macro Definitions

### `BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT`

**Brief:** Enables relaxed pointer-based access to `boost::variant` types, returning `nullptr` instead of throwing exceptions when types do not match.

**Value:** `1` (enabled)

**Rationale:** The DataFormats module heavily uses `boost::variant` for representing values that can be multiple types (strings, numbers, objects, arrays). Relaxed mode allows cleaner conditional access patterns without try-catch blocks.

**Example:**
```cpp
#include "artdaq-database/DataFormats/common.h"

boost::variant<int, std::string, double> value = 42;

// With relaxed mode enabled (this file):
if (auto* intPtr = boost::get<int>(&value)) {
    // Handle integer case - this branch executes
    std::cout << "Integer: " << *intPtr << "\n";
} else if (auto* strPtr = boost::get<std::string>(&value)) {
    // Handle string case
    std::cout << "String: " << *strPtr << "\n";
}

// Without relaxed mode, you would need exception handling:
try {
    auto str = boost::get<std::string>(value);  // Throws boost::bad_get
} catch (boost::bad_get& e) {
    // Handle type mismatch
}
```

## Header Guard

```cpp
#ifndef _ARTDAQ_DATABASE_DATAFORMATS_FHICL_COMMON_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_FHICL_COMMON_H_
```

**Note:** The header guard name contains "FHICL" which is a legacy naming artifact. This is the common header for the entire DataFormats module, not just the FHiCL submodule. The guard functions correctly despite the misleading name.

## Relationship to Other Components

This header serves as the foundation for all DataFormats submodules:

```
DataFormats/common.h
    |
    +-- Json/json_common.h (includes this)
    +-- Xml/xml_common.h (includes this)
    +-- Fhicl/fhicl_common.h (includes this)
    +-- Conf/conf_common.h (includes this)
```

The header is transitively included by virtually all source files in the DataFormats module through the format-specific common headers.

## See Also

- [sharedcommon_common.h](../SharedCommon/sharedcommon_common.h.md) - Foundation header providing standard library includes and TRACE logging
- [shared_types.h](./shared_types.h.md) - Template types that use `boost::variant` with the relaxed access mode configured here
- [shared_literals.h](./shared_literals.h.md) - String constants used as keys throughout DataFormats
- [External: Boost.Variant](https://www.boost.org/doc/libs/release/doc/html/variant.html) - Documentation for boost::variant and get semantics
- [External: Boost.Exception](https://www.boost.org/doc/libs/release/libs/exception/doc/boost-exception.html) - Documentation for boost::diagnostic_information

## Notes for Developers

### Common Pitfalls

- **Pitfall 1:** Including this header in other header files can create excessive dependencies. Only include in `.cpp` implementation files when possible.
- **Pitfall 2:** The relaxed `boost::get` only works with pointer syntax (`boost::get<T>(&variant)`). Value syntax (`boost::get<T>(variant)`) still throws on type mismatch.

### Anti-patterns

```cpp
// DON'T do this - assumes relaxed mode works with value syntax:
auto str = boost::get<std::string>(variant);  // Still throws boost::bad_get!

// DO this instead - use pointer syntax for safe access:
if (auto* str = boost::get<std::string>(&variant)) {
    // Use *str
}
```

### Usage Pattern

```cpp
// In your .cpp file
#include "artdaq-database/DataFormats/MyModule/my_types.h"
#include "artdaq-database/DataFormats/common.h"

// Now all standard library, Boost, TRACE, and variant utilities are available
using namespace artdaq::database;

void processData(boost::variant<int, std::string> const& data) {
    if (auto* intVal = boost::get<int>(&data)) {
        TLOG(TLVL_DEBUG) << "Processing integer: " << *intVal;
    }
}
```

### Exception Diagnostics Usage

```cpp
#include "artdaq-database/DataFormats/common.h"

try {
    // Some operation that may throw
    parseJsonDocument(input);
} catch (...) {
    // Get detailed diagnostic information
    TLOG(TLVL_ERROR) << "Parse failed: "
                     << boost::current_exception_diagnostic_information();
    throw;
}
```
