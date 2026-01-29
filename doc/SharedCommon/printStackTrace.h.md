# printStackTrace.h

**Path:** `artdaq-database/SharedCommon/printStackTrace.h`

**Purpose:** Declares debugging infrastructure for stack traces, signal handling, and exception interception. Enables detailed diagnostics when crashes or exceptions occur by capturing stack traces at throw points and installing handlers for fatal signals.

## Key Concepts

### __cxa_throw Interception

The C++ ABI function `__cxa_throw` is called by the runtime whenever an exception is thrown. By providing our own implementation that wraps the real one, we capture the stack trace at the throw point (not just at the catch point). This is invaluable for debugging because it shows exactly where exceptions originate.

```cpp
extern "C" void __cxa_throw(void*, void*, void (*)(void*));
```

### Signal Handlers

Fatal signals (SIGSEGV, SIGABRT, etc.) normally terminate the process without useful diagnostic information. Custom handlers print stack traces before termination, making it much easier to diagnose crashes in production environments.

### Name Demangling

C++ compilers "mangle" function and type names to encode namespaces, template parameters, and overloading information. For example, `_ZNSt6vectorIiSaIiEE` represents `std::vector<int, std::allocator<int>>`. Demangling converts these cryptic names back to human-readable form for stack traces.

## Thread Safety

- **Thread-safe:** No
- **Notes:** Stack trace capture uses global state (`debug::stack::last_frames`, `debug::stack::last_size`). Signal handlers modify global state. Handler registration is not thread-safe and should be done once at program startup before spawning threads.

## Dependencies

| Include | Purpose |
|---------|---------|
| `<string>` | String class for return values |

## Types

### `__cxa_throw_t`

**Brief:** Function pointer type for the C++ ABI exception throwing function. Used to call the real `__cxa_throw` after capturing the stack trace.

```cpp
// GCC
typedef void(__cxa_throw_t)(void*, void*, void (*)(void*));

// Clang
typedef __attribute__((noreturn)) void(__cxa_throw_t)(void*, std::type_info*, void (*)(void*));
```

**Note:** The type differs slightly between GCC and Clang compilers due to ABI differences.

## Functions

### Handler Registration Functions

#### `registerUngracefullExitHandlers() -> void`

**Brief:** One-call registration of all debugging handlers. This is the recommended function to call at program startup for comprehensive debugging support.

**Thread Safety:** Not thread-safe (should be called once at startup before creating threads)

**Side Effects:**
- Enables TRACE logging
- Installs signal handlers for fatal signals
- Installs terminate handler

**Example:**
```cpp
#include "artdaq-database/SharedCommon/printStackTrace.h"

int main(int argc, char** argv) {
    debug::registerUngracefullExitHandlers();
    // ... rest of application ...
    return 0;
}
```

---

#### `registerAbortHandler() -> void`

**Brief:** Installs signal handlers for fatal signals that print stack traces before termination.

**Handles Signals:**
- `SIGABRT` - Abort signal (e.g., from `abort()` or failed assertions)
- `SIGSEGV` - Segmentation fault
- `SIGBUS` - Bus error
- `SIGILL` - Illegal instruction
- `SIGFPE` - Floating-point exception
- `SIGQUIT` - Quit signal
- `SIGSTKFLT` - Stack fault

**Thread Safety:** Not thread-safe (modifies global signal handlers)

**Side Effects:**
- Replaces default signal handlers

---

#### `registerTerminateHandler() -> void`

**Brief:** Installs a custom handler for `std::terminate()` that prints diagnostic information including the stack trace.

**Called When:**
- Uncaught exception propagates out of `main()`
- Exception thrown during exception handling (double exception)
- `std::terminate()` called explicitly
- Exception thrown from `noexcept` function

**Thread Safety:** Not thread-safe (modifies global terminate handler)

**Side Effects:**
- Replaces default terminate handler

---

#### `registerUncaughtExceptionHandler() -> void`

**Brief:** Installs a handler for unexpected exceptions. Currently a placeholder as `std::set_unexpected` is deprecated in C++17.

**Thread Safety:** Not thread-safe

---

#### `trace_enable() -> void`

**Brief:** Enables and configures the TRACE logging system for the debugging infrastructure.

**Thread Safety:** Not thread-safe (modifies global TRACE state)

**Side Effects:**
- Modifies TRACE logging configuration

---

### Stack Trace Functions

#### `getStackTrace() -> std::string`

**Brief:** Captures and returns the current call stack as a formatted, human-readable string. Useful for debugging and error reporting.

**Returns:** Formatted stack trace with frame numbers and demangled function names

**Thread Safety:** Not thread-safe (uses global stack frame storage)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/printStackTrace.h"
#include <iostream>

void debugFunction() {
    std::cerr << "Current stack:\n" << debug::getStackTrace();
}
```

**Example Output:**
```
Stack trace [10 frames]:
 10 main (tool.cpp:45)
  9 process_request (handler.cpp:123)
  8 execute_operation (operation.cpp:67)
  ...
```

---

#### `getCxaThrowStack() -> std::string`

**Brief:** Returns the stack trace captured at the most recent exception throw point. This shows where the exception was thrown, not where it was caught.

**Returns:** Formatted stack trace from throw point, or `"None"` if no exception has been thrown

**Thread Safety:** Not thread-safe (reads global stack frame storage)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/printStackTrace.h"
#include <iostream>

void handleException() {
    try {
        operationThatMayThrow();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        std::cerr << "Thrown at:\n" << debug::getCxaThrowStack();
    }
}
```

---

#### `demangleStackTrace(void* const* trace, int size) -> std::string`

**Brief:** Converts raw backtrace addresses to a formatted, human-readable stack trace with demangled function names.

**Parameters:**
- `trace` - Array of void pointers from `backtrace()`
- `size` - Number of frames in the trace

**Returns:** Formatted stack trace string

**Thread Safety:** Safe

---

### Utility Functions

#### `demangle(const char* name) -> std::string`

**Brief:** Converts a mangled C++ symbol name to its human-readable form.

**Parameters:**
- `name` - Mangled symbol name (e.g., `"_ZNSt6vectorIiSaIiEE"`)

**Returns:** Demangled name (e.g., `"std::vector<int, std::allocator<int> >"`) or original name if demangling fails

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/printStackTrace.h"

auto readable = debug::demangle("_ZNSt6vectorIiSaIiEE");
// Returns: "std::vector<int, std::allocator<int> >"
```

---

#### `demangle(std::string const& name) -> std::string`

**Brief:** Overload that accepts a std::string parameter for convenience.

**Parameters:**
- `name` - Mangled symbol name as std::string

**Returns:** Demangled name or original if demangling fails

**Thread Safety:** Safe

---

#### `current_exception_diagnostic_information() -> std::string`

**Brief:** Returns comprehensive diagnostic information about the current exception, including the stack trace from the throw point and Boost exception details.

**Returns:** Multi-line string with exception diagnostics

**Thread Safety:** Not thread-safe (reads global stack state)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/printStackTrace.h"
#include <iostream>

void safeOperation() {
    try {
        riskyOperation();
    } catch (...) {
        std::cerr << debug::current_exception_diagnostic_information();
        throw;  // Re-throw after logging
    }
}
```

---

## External Functions (C Linkage)

### `__cxa_throw(void* ex, void*/std::type_info* info, void (*dest)(void*)) -> void`

**Brief:** Override of the C++ ABI exception throwing function. Captures the stack trace before delegating to the real implementation.

**Parameters:**
- `ex` - Pointer to the exception object
- `info` - Type information (void* for GCC, std::type_info* for Clang)
- `dest` - Destructor function for the exception object

**Thread Safety:** Not thread-safe (writes to global stack frame storage)

**Note:** This function is automatically called by the C++ runtime when exceptions are thrown. It should not be called directly.

---

## Relationship to Other Components

- [printStackTrace.cpp](./printStackTrace.cpp.md) - Implementation file
- [helper_functions.h](./helper_functions.h.md) - Uses `getStackTrace()` in `confirm()` assertion
- [shared_exceptions.h](./shared_exceptions.h.md) - Exception types benefit from this infrastructure
- [process_exit_codes.h](./process_exit_codes.h.md) - Exit codes used by signal handlers

## Notes for Developers

### Platform Specificity

This code is Unix/Linux specific and relies on:
- `__cxa_throw` - GCC/Clang C++ ABI function
- `backtrace()` - glibc function for stack trace capture
- Signal handling - POSIX signals
- `abi::__cxa_demangle` - GCC/Clang demangling function

### Performance Considerations

| Operation | Overhead |
|-----------|----------|
| Handler registration | Minimal (done once at startup) |
| `__cxa_throw` interception | Small overhead on every exception throw |
| Stack trace capture | Expensive (only on errors, acceptable) |

### Best Practices

1. **Call early:** Call `registerUngracefullExitHandlers()` at the very beginning of `main()` before any other operations
2. **Use in catch blocks:** Use `getCxaThrowStack()` in exception handlers to see where exceptions originated
3. **Compile with debug info:** Use `-g` compiler flag for best stack trace quality with source file and line information
4. **Optimization effects:** High optimization levels (`-O2`, `-O3`) may affect stack trace accuracy due to function inlining

### Example Integration

```cpp
#include "artdaq-database/SharedCommon/printStackTrace.h"
#include <iostream>

int main(int argc, char** argv) {
    // Register all debugging handlers at startup
    debug::registerUngracefullExitHandlers();

    try {
        // Application code
        run_application(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        std::cerr << debug::getCxaThrowStack();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
```

## Common Pitfalls

- **Late registration:** Registering handlers after threads are spawned may cause race conditions
- **Missing `-g` flag:** Without debug symbols, stack traces only show addresses
- **Inlining:** Aggressive optimization may hide function calls in stack traces
- **Thread safety:** The global stack storage means concurrent exceptions may overwrite each other's traces

## See Also

- [printStackTrace.cpp](./printStackTrace.cpp.md) - Implementation details
- [helper_functions.h](./helper_functions.h.md) - `confirm()` assertion that uses stack traces
- [process_exit_codes.h](./process_exit_codes.h.md) - Exit codes for signal handlers
