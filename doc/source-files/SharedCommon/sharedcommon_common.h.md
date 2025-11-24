# sharedcommon_common.h

## File Overview

This is a convenience aggregator header file that includes all major SharedCommon module headers in one place. It provides a single include point for code that needs access to the full SharedCommon infrastructure, simplifying dependency management and ensuring consistent inclusion order.

**Location**: `/home/user/artdaq-database/artdaq-database/SharedCommon/sharedcommon_common.h`

## Dependencies

This file aggregates the following SharedCommon headers:

### Core Utilities
- `"artdaq-database/SharedCommon/helper_functions.h"` - Helper utilities for strings, time, JSON, etc.
- `"artdaq-database/SharedCommon/returned_result.h"` - Result type for error handling
- `"artdaq-database/SharedCommon/shared_datatypes.h"` - Common type aliases
- `"artdaq-database/SharedCommon/shared_exceptions.h"` - Exception type hierarchy

### System Utilities
- `"artdaq-database/SharedCommon/process_exit_codes.h"` - Exit codes and trace modes
- `"artdaq-database/SharedCommon/printStackTrace.h"` - Stack trace and debugging support
- `"artdaq-database/SharedCommon/fileststem_functions.h"` - Filesystem operations

### Base Headers
- `"artdaq-database/SharedCommon/common.h"` - Standard library includes and common types

## Header Guard

```cpp
#ifndef _ARTDAQ_DATABASE_SHAREDCOMMON_COMMON_H_
#define _ARTDAQ_DATABASE_SHAREDCOMMON_COMMON_H_
```

## Code Structure

```cpp
// clang-format off

#include "artdaq-database/SharedCommon/helper_functions.h"
#include "artdaq-database/SharedCommon/returned_result.h"
#include "artdaq-database/SharedCommon/shared_datatypes.h"
#include "artdaq-database/SharedCommon/shared_exceptions.h"
#include "artdaq-database/SharedCommon/process_exit_codes.h"
#include "artdaq-database/SharedCommon/printStackTrace.h"
#include "artdaq-database/SharedCommon/fileststem_functions.h"

#include "artdaq-database/SharedCommon/common.h"

// clang-format on
```

**Note**: Includes are wrapped in clang-format directives to prevent automatic reordering, as the order may be significant.

## Purpose and Design

### Aggregator Pattern

This header follows the **Aggregator Header** pattern:

**Benefits**:
1. **Single Include**: One line includes everything from SharedCommon
2. **Dependency Management**: Ensures all dependencies are included together
3. **Inclusion Order**: Guarantees correct order (common.h last)
4. **Consistency**: All code using SharedCommon gets the same includes
5. **Ease of Use**: Simpler for developers

**Trade-offs**:
1. **Compile Time**: Includes more than some files need
2. **Dependencies**: Changes to this file affect many compilation units
3. **Namespace Pollution**: Brings many symbols into scope

### Inclusion Order

The order is deliberate:

1. **helper_functions.h** - Core utilities, no dependencies on other SharedCommon files
2. **returned_result.h** - Simple types, minimal dependencies
3. **shared_datatypes.h** - Type aliases, no dependencies
4. **shared_exceptions.h** - Exception types, depends on common types
5. **process_exit_codes.h** - Constants, no dependencies
6. **printStackTrace.h** - Debugging support, uses exceptions and exit codes
7. **fileststem_functions.h** - Filesystem utilities, uses datatypes and helpers
8. **common.h** - Standard library includes, included last to provide foundation

**Rationale**: Dependencies are included before dependents. `common.h` is last because it provides standard library includes that everything uses.

---

## What You Get By Including This File

### Type Definitions

```cpp
// From shared_datatypes.h
artdaq::database::path_t
artdaq::database::string_pair_t
artdaq::database::object_id_t
artdaq::database::timestamp_t

// From returned_result.h
artdaq::database::result_t
```

### Exception Types

```cpp
// From shared_exceptions.h
artdaq::database::exception
artdaq::database::invalid_argument
artdaq::database::runtime_error
artdaq::database::invalid_option_exception
artdaq::database::runtime_exception
```

### Constants

```cpp
// From process_exit_codes.h
process_exit_code::SUCCESS
process_exit_code::FAILURE
process_exit_code::INVALID_ARGUMENT
process_exit_code::UNCAUGHT_EXCEPTION
trace_mode::modeM
trace_mode::modeS
```

### Helper Functions

```cpp
// From helper_functions.h
artdaq::database::timestamp()
artdaq::database::quoted_()
artdaq::database::generate_oid()
artdaq::database::to_json()
// ... and many more
```

### Filesystem Functions

```cpp
// From fileststem_functions.h
artdaq::database::list_files()
artdaq::database::mkdir()
artdaq::database::read_buffer_from_file()
artdaq::database::write_buffer_to_file()
// ... and more
```

### Result Functions

```cpp
// From returned_result.h
artdaq::database::Success()
artdaq::database::Failure()
artdaq::database::ThrowOnFailure()
```

### Debug Functions

```cpp
// From printStackTrace.h
debug::getStackTrace()
debug::getCxaThrowStack()
debug::registerUngracefullExitHandlers()
// ... and more
```

### Standard Library

```cpp
// From common.h
std::vector, std::string, std::map, etc.
std::chrono types
boost::demangle, boost::lexical_cast
```

---

## Usage Patterns

### Application Main Files

```cpp
#include "artdaq-database/SharedCommon/sharedcommon_common.h"

int main(int argc, char* argv[]) {
    debug::registerUngracefullExitHandlers();

    try {
        // Use all SharedCommon functionality
        auto timestamp = artdaq::database::timestamp();
        auto id = artdaq::database::generate_oid();

        // ... application logic ...

        return process_exit_code::SUCCESS;

    } catch (artdaq::database::exception const& e) {
        std::cerr << e.what() << std::endl;
        return process_exit_code::UNCAUGHT_EXCEPTION;
    }
}
```

### Implementation Files

```cpp
#include "my_module.h"
#include "artdaq-database/SharedCommon/sharedcommon_common.h"

namespace db = artdaq::database;

db::result_t MyClass::process(db::path_t const& path) {
    if (path.empty()) {
        return db::Failure("Path cannot be empty");
    }

    try {
        auto files = db::list_files(path);
        // ... process files ...
        return db::Success();

    } catch (db::exception const& e) {
        return db::Failure(e.what());
    }
}
```

### When to Use This Header

**Use sharedcommon_common.h when**:
- Writing main() functions
- Need multiple SharedCommon components
- Implementing core database operations
- Don't want to manage individual includes

**Don't use (use specific headers instead) when**:
- Writing header files (minimize dependencies)
- Only need one or two specific utilities
- Working on compile-time-sensitive code
- Want to minimize namespace pollution

---

## Comparison: Aggregator vs Individual Includes

### Using Aggregator (sharedcommon_common.h)

```cpp
#include "artdaq-database/SharedCommon/sharedcommon_common.h"

// Everything available immediately
auto ts = artdaq::database::timestamp();
auto result = artdaq::database::list_files(path);
debug::registerAbortHandler();
```

**Pros**:
- One line to include everything
- Don't need to know dependencies
- Guaranteed correct order

**Cons**:
- Slower compilation
- More dependencies
- Larger precompiled header

### Using Individual Includes

```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"
#include "artdaq-database/SharedCommon/fileststem_functions.h"

// Only what you included is available
auto ts = artdaq::database::timestamp();
auto files = artdaq::database::list_files(path);
// debug:: functions not available
```

**Pros**:
- Faster compilation
- Fewer dependencies
- More explicit

**Cons**:
- More lines
- Must know dependencies
- Can get order wrong

---

## Design Considerations

### clang-format Directives

```cpp
// clang-format off
#include "..."
// clang-format on
```

**Purpose**: Prevents automatic reformatters from reordering includes.

**Why Important**: Some includes may have order dependencies, and auto-formatting tools might break those dependencies.

### Recursive Inclusion Protection

All included headers have include guards, preventing duplicate inclusion:

```cpp
// sharedcommon_common.h includes common.h
// common.h has include guard
// If common.h is included elsewhere, guard prevents duplication
```

### Forward vs Full Includes

This header uses full includes (not forward declarations) because:
- It's meant to provide full functionality
- Users expect complete types, not just declarations
- It's an implementation file include, not a header file include

---

## Compilation Impact

### Include Graph

```
sharedcommon_common.h
  ├─ helper_functions.h
  │   └─ shared_datatypes.h
  ├─ returned_result.h
  ├─ shared_datatypes.h (already included)
  ├─ shared_exceptions.h
  ├─ process_exit_codes.h
  ├─ printStackTrace.h
  │   └─ process_exit_codes.h (already included)
  ├─ fileststem_functions.h
  │   └─ shared_datatypes.h (already included)
  └─ common.h
      └─ (many standard library headers)
```

### Compile Time Impact

**First Compilation**:
- Parses all SharedCommon headers
- Parses all standard library headers from common.h
- Can be slow (1-2 seconds)

**Subsequent Compilations** (with precompiled headers):
- Reuses cached parse trees
- Much faster (0.1-0.2 seconds)

**Recommendation**: Use precompiled headers in production builds.

---

## Best Practices

### In Implementation Files (.cpp)

```cpp
// Good: Use aggregator in .cpp files
#include "my_module.h"
#include "artdaq-database/SharedCommon/sharedcommon_common.h"

void MyModule::process() {
    // All functionality available
}
```

### In Header Files (.h)

```cpp
// Bad: Don't use aggregator in headers
#include "artdaq-database/SharedCommon/sharedcommon_common.h"

class MyClass {
    // Exposes all SharedCommon to every includer
};
```

```cpp
// Good: Use specific headers and forward declarations
#include "artdaq-database/SharedCommon/shared_datatypes.h"

namespace artdaq::database {
    class exception;  // Forward declaration
}

class MyClass {
    artdaq::database::path_t path_;
    // Minimal dependencies exposed
};
```

### With Namespace Aliases

```cpp
#include "artdaq-database/SharedCommon/sharedcommon_common.h"

namespace db = artdaq::database;  // Convenient alias

db::result_t process(db::path_t const& path) {
    // Shorter, more readable
}
```

### In Precompiled Headers

```cpp
// stdafx.h or pch.h
#pragma once

#include "artdaq-database/SharedCommon/sharedcommon_common.h"
// Other commonly-used headers

// Then in source files:
#include "pch.h"  // Gets everything
```

---

## Alternative Approaches

### Modular Includes

Create smaller aggregators:

```cpp
// sharedcommon_types.h
#include "shared_datatypes.h"
#include "shared_exceptions.h"

// sharedcommon_utils.h
#include "helper_functions.h"
#include "fileststem_functions.h"

// sharedcommon_debug.h
#include "printStackTrace.h"
#include "process_exit_codes.h"
```

Users include only what they need.

### Individual Includes Only

Eliminate aggregators entirely:

```cpp
// Each source file manages its own includes
#include "artdaq-database/SharedCommon/helper_functions.h"
#include "artdaq-database/SharedCommon/returned_result.h"
// ... etc
```

More explicit but more verbose.

---

## Maintenance Guidelines

### Adding New SharedCommon Headers

When adding a new header to SharedCommon:

1. **Add to this file** if it's widely used
2. **Consider dependencies** - include after dependencies
3. **Test compilation** - ensure no circular dependencies
4. **Update documentation** - document what the new header provides

### Removing Headers

When deprecating a SharedCommon header:

1. **Remove from this file** first
2. **Update all direct includers** to include explicitly
3. **Deprecate the header** before removing
4. **Update documentation**

### Reordering Includes

If you need to reorder:

1. **Verify dependencies** - ensure new order respects them
2. **Test thoroughly** - compile all modules
3. **Update comments** - explain new order if non-obvious

---

## Related Files

- **common.h** - Base standard library includes (included by this file)
- All other SharedCommon headers - Aggregated by this file
- Build system (CMakeLists.txt) - May use this for precompiled headers

---

## Summary

This aggregator header provides a convenient single-include access point to the entire SharedCommon module infrastructure. It's designed for use in implementation files where compile time is less critical than developer convenience. For header files and compile-time-sensitive code, prefer individual includes.

**Key Points**:
- Includes all major SharedCommon functionality
- Maintains correct inclusion order
- Best for .cpp files, not .h files
- Trade-off: convenience vs compile time
- Protected against format tool reordering

**When in doubt**: Use this in .cpp files, specific headers in .h files.
