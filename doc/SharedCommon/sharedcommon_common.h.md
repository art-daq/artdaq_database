# sharedcommon_common.h

**Path:** `artdaq-database/SharedCommon/sharedcommon_common.h`

**Purpose:** Master aggregator header that includes all SharedCommon module headers. Provides a single include point for code that needs the full SharedCommon infrastructure, simplifying dependency management and ensuring all module functionality is available with one include statement.


## Key Concepts

### Aggregator Pattern

One include provides access to the complete SharedCommon module:
- Type definitions (`path_t`, `object_id_t`, `result_t`)
- Exception types (`invalid_argument`, `runtime_error`, `runtime_exception`)
- Helper functions (`timestamp()`, `generate_oid()`, `to_json()`)
- Filesystem utilities (`list_files()`, `mkdir()`)
- Result functions (`Success()`, `Failure()`, `ThrowOnFailure()`)
- Debug infrastructure (`getStackTrace()`, `registerUngracefullExitHandlers()`)
- Standard library (via `common.h`)

### Inclusion Order

Headers are included in dependency order to ensure proper compilation:

1. `helper_functions.h` - Core utilities (depends only on standard library)
2. `returned_result.h` - Result type (depends only on standard library)
3. `shared_datatypes.h` - Type aliases (depends only on standard library)
4. `shared_exceptions.h` - Exception types (depends on cetlib)
5. `process_exit_codes.h` - Constants (no dependencies)
6. `printStackTrace.h` - Debug support (depends on system headers)
7. `fileststem_functions.h` - Filesystem (depends on Boost.Filesystem)
8. `common.h` - Standard library aggregator (last to avoid ordering issues)

The `// clang-format off/on` directives prevent clang-format from automatically reordering these includes, which would break the intentional dependency order.

## Thread Safety

- **Thread-safe:** Conditional
- **Concurrent access:** Thread safety depends on the specific functions and types used
- **Locking:** Most types are thread-safe for concurrent reads; functions that modify shared state require external synchronization

See individual header documentation for detailed thread safety information:
- `helper_functions.h` - Most functions are thread-safe
- `returned_result.h` - Thread-safe (stateless factory functions)
- `shared_datatypes.h` - Thread-safe for reads (type aliases)
- `shared_exceptions.h` - Thread-safe (exceptions are value types)
- `fileststem_functions.h` - Thread-safe (uses Boost.Filesystem)

## Dependencies

| Include | Purpose |
|---------|---------|
| `helper_functions.h` | String manipulation, time utilities, JSON helpers |
| `returned_result.h` | `result_t` type, `Success()`, `Failure()` functions |
| `shared_datatypes.h` | `path_t`, `object_id_t`, `timestamp_t` type aliases |
| `shared_exceptions.h` | Exception hierarchy (`invalid_argument`, `runtime_error`) |
| `process_exit_codes.h` | Exit codes (`SUCCESS`, `FAILURE`), trace modes |
| `printStackTrace.h` | Stack traces, signal handlers |
| `fileststem_functions.h` | File I/O, directory operations |
| `common.h` | Standard library, Boost, TRACE logging |

## Provided Functionality

This header does not define any classes, functions, or types itself. It aggregates all SharedCommon headers. After including this file, you have access to:

### Types (from shared_datatypes.h)

| Type | Description |
|------|-------------|
| `path_t` | Filesystem paths and URIs |
| `object_id_t` | Document unique identifiers |
| `timestamp_t` | ISO 8601 formatted timestamps |
| `string_pair_t` | Key-value string pairs |
| `result_t` | Success/failure pair with message |

### Exception Classes (from shared_exceptions.h)

| Class | Use Case |
|-------|----------|
| `exception` | Base class for all artdaq-database exceptions |
| `invalid_argument` | Invalid function parameters |
| `runtime_error` | Runtime failures (I/O, network, etc.) |
| `invalid_option_exception` | Bad CLI or configuration options |
| `runtime_exception` | General runtime errors with message |

### Result Functions (from returned_result.h)

| Function | Description |
|----------|-------------|
| `Success(msg)` | Create success result with message |
| `Failure(msg)` | Create failure result with message |
| `ThrowOnFailure(result)` | Convert failure to exception |

### Exit Codes (from process_exit_codes.h)

| Constant | Value | Use |
|----------|-------|-----|
| `process_exit_code::SUCCESS` | 0 | Normal completion |
| `process_exit_code::FAILURE` | 1 | Generic failure |
| `process_exit_code::INVALID_ARGUMENT` | 128 | Bad arguments |
| `process_exit_code::UNCAUGHT_EXCEPTION` | 144 | Unhandled exception |

## Relationship to Other Components

This header sits at the top of the SharedCommon module hierarchy:

```
sharedcommon_common.h (this file - aggregates everything)
    |
    +-- helper_functions.h
    +-- returned_result.h
    +-- shared_datatypes.h
    +-- shared_exceptions.h
    +-- process_exit_codes.h
    +-- printStackTrace.h
    +-- fileststem_functions.h
    +-- common.h
```

**Used by:**
- ConfigurationDB module implementation files
- StorageProviders module implementation files
- Utilities command-line tools
- Test files

**Important:** This header should only be included in `.cpp` implementation files, not in headers, to avoid excessive dependency propagation.

## Example

```cpp
#include "artdaq-database/SharedCommon/sharedcommon_common.h"
#include <iostream>

namespace db = artdaq::database;

int main() {
    // Register signal handlers for clean crash reporting
    debug::registerUngracefullExitHandlers();

    try {
        // Types available from shared_datatypes.h
        db::path_t config_path = "/etc/artdaq/config.json";
        db::object_id_t doc_id;
        db::timestamp_t created;

        // Helper functions from helper_functions.h
        created = db::timestamp();
        doc_id = db::generate_oid();

        std::cout << "Generated ID: " << doc_id << std::endl;
        std::cout << "Timestamp: " << created << std::endl;

        // Result handling from returned_result.h
        db::result_t result = db::Success("Operation completed");
        if (!result.first) {
            std::cerr << "Operation failed: " << result.second << std::endl;
            return process_exit_code::FAILURE;
        }

        // File operations from fileststem_functions.h
        auto files = db::list_files(config_path);

        // Convert result to exception if needed
        db::ThrowOnFailure(result);

        return process_exit_code::SUCCESS;

    } catch (db::invalid_argument const& e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        return process_exit_code::INVALID_ARGUMENT;

    } catch (db::runtime_error const& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        return process_exit_code::FAILURE;

    } catch (std::exception const& e) {
        std::cerr << "Uncaught exception: " << e.what() << std::endl;
        std::cerr << debug::current_exception_diagnostic_information() << std::endl;
        return process_exit_code::UNCAUGHT_EXCEPTION;
    }
}
```

## Notes for Developers

### When to Use

```cpp
// GOOD: Use in .cpp implementation files for full SharedCommon access
#include "artdaq-database/SharedCommon/sharedcommon_common.h"
namespace db = artdaq::database;
```

### When NOT to Use

```cpp
// BAD: Don't use in .h header files - too many dependencies
// This would slow down compilation and create unnecessary coupling

// GOOD: In headers, include specific headers only:
#include "artdaq-database/SharedCommon/shared_datatypes.h"
#include "artdaq-database/SharedCommon/returned_result.h"
```

### Available Namespaces After Including

| Namespace | Contents |
|-----------|----------|
| `artdaq::database` | Types, helper functions, result utilities |
| `artdaq::database::result` | Result message constants (`msg_Success`, `msg_Failure`, etc.) |
| `debug` | Stack traces, exception diagnostics |
| `process_exit_code` | Exit code constants (`SUCCESS`, `FAILURE`, etc.) |
| `trace_mode` | TRACE logging mode settings |

### Namespace Alias Convention

```cpp
// Recommended namespace alias for brevity
namespace db = artdaq::database;
namespace apiliteral = artdaq::database::configapi::literal;

// Usage
db::result_t result = db::Success();
db::path_t path = "/path/to/file";
```

### Common Pitfalls

- **Pitfall 1:** Including in header files creates excessive compile-time dependencies. Always use in `.cpp` files only.

- **Pitfall 2:** Forgetting to call `registerUngracefullExitHandlers()` in main programs means crashes will not generate stack traces.

- **Pitfall 3:** Not catching exceptions in main() leads to unclean termination:
  ```cpp
  // BAD: Exceptions cause abort()
  int main() {
      riskyOperation();
      return 0;
  }

  // GOOD: Catch and return appropriate exit codes
  int main() {
      try {
          riskyOperation();
          return process_exit_code::SUCCESS;
      } catch (db::invalid_argument const& e) {
          std::cerr << e.what() << std::endl;
          return process_exit_code::INVALID_ARGUMENT;
      } catch (std::exception const& e) {
          std::cerr << e.what() << std::endl;
          return process_exit_code::UNCAUGHT_EXCEPTION;
      }
  }
  ```

### Anti-patterns

```cpp
// DON'T include in header files
#ifndef MY_CLASS_H
#define MY_CLASS_H
#include "artdaq-database/SharedCommon/sharedcommon_common.h"  // BAD!
class MyClass { };
#endif

// DO include only specific headers in .h files
#ifndef MY_CLASS_H
#define MY_CLASS_H
#include "artdaq-database/SharedCommon/shared_datatypes.h"  // GOOD
class MyClass {
    artdaq::database::object_id_t id_;
};
#endif
```

## See Also

- [common.h](./common.h.md) - Standard library aggregator
- [helper_functions.h](./helper_functions.h.md) - Utility functions
- [returned_result.h](./returned_result.h.md) - Result type and factories
- [shared_datatypes.h](./shared_datatypes.h.md) - Type aliases
- [shared_exceptions.h](./shared_exceptions.h.md) - Exception hierarchy
- [process_exit_codes.h](./process_exit_codes.h.md) - Exit codes
- [printStackTrace.h](./printStackTrace.h.md) - Debug infrastructure
- [fileststem_functions.h](./fileststem_functions.h.md) - File operations
