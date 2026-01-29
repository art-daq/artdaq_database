# common.h

**Path:** `artdaq-database/ConfigurationDB/common.h`

**Purpose:** Convenience header that includes shared common utilities and definitions from the SharedCommon module. This header serves as the foundational include for all ConfigurationDB module files, providing access to TRACE logging, exception classes, and utility functions.


## Key Concepts

This file implements the **Convenience Header Pattern** to simplify includes across the ConfigurationDB module. By providing a single header that brings in all commonly needed functionality, it reduces repetitive includes and makes it easier to maintain common dependencies module-wide.

### What This Header Provides

Through transitive inclusion from `sharedcommon_common.h`:

- **TLOG()** - TRACE logging macros for diagnostic output
- **trace_mode** - Enumeration for controlling trace verbosity levels
- **confirm()** - Debug assertion macro for validating preconditions
- **Exception types** - Common exception classes (`runtime_error`, `invalid_option_exception`, etc.)
- **Utility functions** - String manipulation, environment variable expansion

## Thread Safety

- **Thread-safe:** Yes
- **Concurrent access:** All utilities accessed through this header are thread-safe
- **Locking:** No internal locking required; relies on thread-safe implementations in SharedCommon

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/SharedCommon/sharedcommon_common.h` | Core shared utilities including TRACE logging, exception types, and common definitions |

## Classes/Structures

This header does not define any classes or structures of its own. It serves purely as an aggregation header.

## Functions

This header does not define any functions of its own.

## Macros and Definitions

Through transitive includes, this header provides access to:

### `TLOG(level)`

**Brief:** Macro for generating trace log messages at the specified verbosity level.

**Parameters:**
- `level` - Integer trace level (higher values = more verbose)

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/common.h"

void someFunction() {
  TLOG(10) << "Debug message at level 10";
  TLOG(20) << "More verbose message at level 20";
}
```

### `confirm(condition)`

**Brief:** Debug assertion macro that validates a condition and throws if false.

**Parameters:**
- `condition` - Boolean expression to validate

**Preconditions:**
- None

**Returns:** void

**Postconditions:**
- If the assertion passes, execution continues normally
- If the assertion fails in debug builds, an exception is thrown

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::logic_error` | When `condition` evaluates to false (debug builds only) |

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/common.h"

void processData(std::string const& data) {
  confirm(!data.empty());  // Asserts data is not empty
  // Process data...
}
```

## Relationship to Other Components

```
SharedCommon module
       ^
       |
       | includes
       |
common.h (this file)
       ^
       |
       | included by
       |
All ConfigurationDB .cpp files
```

- **SharedCommon module**: Directly includes `sharedcommon_common.h` to bring in all shared utilities
- **ConfigurationDB module**: All implementation files include this header as their foundation
- **Other modules**: Provides consistent access to shared utilities across the codebase

## See Also

- [sharedcommon_common.h](../SharedCommon/sharedcommon_common.h.md) - The actual implementation of shared utilities
- [shared_exceptions.h](../SharedCommon/shared_exceptions.h.md) - Exception class hierarchy
- [configuration_common.h](./configuration_common.h.md) - Higher-level aggregation header for ConfigurationDB

## Notes for Developers

### Include Order

This header should typically be the first project-specific include in ConfigurationDB files:

```cpp
#include "artdaq-database/ConfigurationDB/common.h"  // First

#include <string>     // Standard library
#include <vector>

#include "artdaq-database/ConfigurationDB/other.h"  // Other project includes
```

### Common Pitfalls

- **Pitfall 1:** Including this header in public API headers can create unnecessary dependencies. For minimal public interfaces, prefer direct includes of only what is needed.
- **Pitfall 2:** The `confirm()` macro may be disabled in release builds. Do not rely on it for production validation; use explicit exception throwing instead.

### Anti-patterns

```cpp
// DON'T do this: Using confirm() for user input validation
void setProvider(std::string const& provider) {
  confirm(!provider.empty());  // Only works in debug builds!
}

// DO this instead: Explicit validation with exceptions
void setProvider(std::string const& provider) {
  if (provider.empty()) {
    throw invalid_option_exception("setProvider") << "Provider cannot be empty";
  }
}
```

---

**Documentation generated for artdaq-database ConfigurationDB module**
