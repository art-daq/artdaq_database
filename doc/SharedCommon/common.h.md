# common.h

**Path:** `artdaq-database/SharedCommon/common.h`

**Purpose:** Convenience header that aggregates commonly used Standard C++ library and third-party headers. Including this single file provides access to containers, algorithms, I/O, chrono, Boost utilities, and the TRACE logging system. This reduces boilerplate and ensures consistent library usage across the codebase.


## Key Concepts

### Precompiled Header Pattern

This file is designed to be included in `.cpp` implementation files (not in headers) to:
- Reduce repetitive includes across multiple source files
- Enable precompiled header (PCH) optimization by compilers
- Ensure consistent standard library usage throughout the codebase

### TRACE Logging

The `trace.h` include provides the artdaq TRACE logging system, a high-performance tracing mechanism used throughout the ARTDAQ ecosystem:

```cpp
#include "artdaq-database/SharedCommon/common.h"
#include <iostream>

void example_function() {
    try {
        TLOG(10) << "Debug message at level 10";
        TLOG(TLVL_INFO) << "Info message";
        TLOG(TLVL_WARNING) << "Warning message";
    } catch (std::exception const& e) {
        std::cerr << "Logging error: " << e.what() << std::endl;
    }
}
```

TRACE provides:
- **Low overhead**: Messages can be compiled out in release builds
- **Configurable levels**: Runtime-adjustable verbosity
- **Memory buffering**: High-performance circular buffer for trace messages
- **Thread safety**: Safe for concurrent logging from multiple threads

## Thread Safety

- **Thread-safe:** Yes (for included headers)
- **Concurrent access:** All included standard library and Boost headers are thread-safe for read operations
- **Locking:** The TRACE logging system uses internal locking for thread-safe concurrent writes

## Dependencies

| Include | Purpose |
|---------|---------|
| `<algorithm>` | Standard algorithms (sort, find, copy, transform, etc.) |
| `<chrono>` | Time utilities, durations, and time points |
| `<cstddef>` | Standard size types (`size_t`, `ptrdiff_t`, `nullptr_t`) |
| `<cstdio>` | C-style I/O functions (`printf`, `fopen`, etc.) |
| `<fstream>` | File stream I/O (`ifstream`, `ofstream`) |
| `<functional>` | Function objects, `std::function`, `std::bind` |
| `<iomanip>` | I/O manipulators (`setw`, `setprecision`, `setfill`) |
| `<iostream>` | Standard I/O streams (`std::cin`, `std::cout`, `std::cerr`) |
| `<iterator>` | Iterator utilities and traits |
| `<memory>` | Smart pointers (`unique_ptr`, `shared_ptr`, `weak_ptr`) |
| `<regex>` | Regular expression support |
| `<sstream>` | String stream classes (`stringstream`, `ostringstream`) |
| `<streambuf>` | Stream buffer base classes |
| `<string>` | `std::string` class |
| `<tuple>` | `std::tuple` container |
| `<type_traits>` | Compile-time type information and transformations |
| `<vector>` | Dynamic array container |
| `<boost/core/demangle.hpp>` | C++ type name demangling for debugging output |
| `<boost/lexical_cast.hpp>` | Type conversion utilities (string to/from numeric) |
| `trace.h` | TRACE debugging/logging system |

## Provided Functionality

This header does not define any classes, functions, or types itself. It serves as an aggregator that makes the following available:

### Standard Library Components

After including `common.h`, you have access to:

- **Containers:** `std::vector`, `std::string`, `std::tuple`
- **Algorithms:** `std::sort`, `std::find`, `std::copy`, `std::transform`
- **I/O:** `std::cout`, `std::cerr`, `std::ifstream`, `std::ofstream`, `std::stringstream`
- **Memory:** `std::unique_ptr`, `std::shared_ptr`, `std::make_unique`, `std::make_shared`
- **Time:** `std::chrono::system_clock`, `std::chrono::duration`
- **Type Traits:** `std::is_same`, `std::enable_if`, `std::decay`

### Boost Components

- **`boost::core::demangle()`:** Converts mangled C++ type names to human-readable form
- **`boost::lexical_cast<T>()`:** Converts between string and numeric types

### TRACE Macros

- **`TLOG(level)`:** Log a message at the specified trace level
- **`TLVL_*`:** Predefined trace levels (INFO, WARNING, ERROR, etc.)

## Relationship to Other Components

This file serves as the foundation for standard library access in the SharedCommon module:

```
common.h (this file)
    ^
    |
sharedcommon_common.h (includes this + all SharedCommon utilities)
    ^
    |
Implementation files (.cpp) throughout the project
```

**Used by:**
- `sharedcommon_common.h` - Higher-level aggregator that includes this file
- Most `.cpp` files in SharedCommon: `helper_functions.cpp`, `fileststem_functions.cpp`, `printStackTrace.cpp`
- Implementation files throughout the project

**Important:** This file should only be included in implementation files (`.cpp`), not in headers (`.h`), to avoid excessive dependency propagation.

## Example

```cpp
#include "artdaq-database/SharedCommon/common.h"
#include <iostream>

void demonstrateAvailableFeatures() {
    try {
        // Standard library containers and utilities
        std::vector<std::string> data{"item1", "item2", "item3"};
        std::stringstream ss;
        auto ptr = std::make_unique<int>(42);

        // Boost utilities for type demangling (useful for debugging)
        auto type_name = boost::core::demangle(typeid(std::vector<int>).name());
        std::cout << "Type name: " << type_name << std::endl;

        // Boost lexical_cast for string conversion
        auto num = boost::lexical_cast<int>("42");
        auto str = boost::lexical_cast<std::string>(3.14);
        std::cout << "Converted: " << num << ", " << str << std::endl;

        // TRACE logging
        TLOG(10) << "Processing " << data.size() << " items";

    } catch (boost::bad_lexical_cast const& e) {
        std::cerr << "Conversion error: " << e.what() << std::endl;
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    demonstrateAvailableFeatures();
    return 0;
}
```

## Notes for Developers

### Best Practices

```cpp
// GOOD: In .cpp files - include common.h for standard library access
#include "artdaq-database/SharedCommon/common.h"

void processData() {
    std::vector<std::string> data;
    std::stringstream ss;
    auto ptr = std::make_unique<MyClass>();
    // ...
}
```

```cpp
// BAD: In .h files - include specific headers only
// Don't do this in headers:
// #include "artdaq-database/SharedCommon/common.h"

// DO this instead in headers:
#include <string>
#include <vector>
// Include only what's needed for the interface
```

### Common Pitfalls

- **Pitfall 1:** Including this file in header files creates excessive compile-time dependencies and slows down builds. Always include in `.cpp` files only.

- **Pitfall 2:** The TRACE macros require proper initialization. Ensure TRACE is configured before use in main programs.

- **Pitfall 3:** `boost::lexical_cast` throws `boost::bad_lexical_cast` on conversion failure. Always wrap in try-catch when converting user input:
  ```cpp
  // GOOD: Handle conversion errors
  try {
      int value = boost::lexical_cast<int>(user_input);
  } catch (boost::bad_lexical_cast const& e) {
      std::cerr << "Invalid number: " << user_input << std::endl;
  }

  // BAD: Unhandled exception on invalid input
  int value = boost::lexical_cast<int>(user_input);  // May throw!
  ```

### Anti-patterns

```cpp
// DON'T do this in a header file:
#ifndef MY_HEADER_H
#define MY_HEADER_H
#include "artdaq-database/SharedCommon/common.h"  // Wrong! Too many dependencies
class MyClass { /* ... */ };
#endif

// DO this instead:
#ifndef MY_HEADER_H
#define MY_HEADER_H
#include <string>  // Only include what's needed
class MyClass {
    std::string name_;
};
#endif
```

## See Also

- [sharedcommon_common.h](./sharedcommon_common.h.md) - Module aggregator that includes this file plus all SharedCommon utilities
- [External: Boost.Core demangle](https://www.boost.org/doc/libs/release/libs/core/doc/html/core/demangle.html) - Documentation for type name demangling
- [External: Boost.Lexical_Cast](https://www.boost.org/doc/libs/release/libs/lexical_cast/) - Documentation for type conversion
- [External: TRACE](https://cdcvs.fnal.gov/redmine/projects/trace/wiki) - ARTDAQ TRACE logging system documentation
