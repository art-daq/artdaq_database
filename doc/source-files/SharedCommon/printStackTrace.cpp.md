# printStackTrace.cpp

**Path:** `artdaq-database/SharedCommon/printStackTrace.cpp`

**Implements:** [printStackTrace.h](./printStackTrace.h.md)

**Purpose:** Implements stack trace capture, exception interception, and signal handling for debugging. The key feature is `__cxa_throw` interception, which captures stack traces at exception throw points rather than catch points, providing invaluable diagnostic information.

## Implementation Overview

This file provides the core debugging infrastructure for the artdaq-database library. It intercepts the C++ ABI's exception throwing mechanism to capture stack traces at throw points, installs signal handlers for fatal signals, and provides utility functions for name demangling and stack trace formatting. The implementation uses platform-specific features (glibc `backtrace()`, GCC/Clang ABI functions) to achieve this functionality.

## Key Algorithms

### __cxa_throw Interception

The C++ runtime calls `__cxa_throw` when throwing exceptions. Our implementation:

**Steps:**
1. Capture the current stack trace using `backtrace()` into global storage
2. Find the real `__cxa_throw` implementation using `dlsym(RTLD_NEXT, "__cxa_throw")`
3. Forward the call to the real implementation

```cpp
void __cxa_throw(void* ex, void* info, void (*dest)(void*)) {
    debug::stack::last_size = backtrace(debug::stack::last_frames, 1024);
    auto* rethrow = (__cxa_throw_t*)dlsym(RTLD_NEXT, "__cxa_throw");
    rethrow(ex, info, dest);
}
```

### Stack Trace Demangling (`demangleStackTrace`)

**Steps:**
1. Convert raw addresses to symbol strings using `backtrace_symbols()`
2. For each symbol, use regex to extract the mangled function name
3. Demangle each function name using `abi::__cxa_demangle()`
4. Format with frame numbers (highest first) for readability
5. Free the symbol memory

**Regex Pattern:** `[(](.*)[+]` - Extracts the function name between `(` and `+` in the backtrace symbol format.

### Signal Handler Flow

**Steps:**
1. Map signal number to human-readable name
2. Log the signal information via TRACE
3. Capture and log the current stack trace
4. Exit with the signal number as exit code

### Terminate Handler Flow

**Steps:**
1. Check if there's a current exception (`std::current_exception()`)
2. If so, retrieve the stack trace captured at throw time
3. Rethrow and catch to get exception type and message
4. Demangle the exception type name
5. Log all diagnostic information via TRACE
6. Capture and log the current stack trace
7. Exit with SIGTERM

## Internal Data Structures

### Global Stack Storage

```cpp
namespace debug {
namespace stack {
void* last_frames[1024];  // Array of return addresses
size_t last_size;         // Number of captured frames
}  // namespace stack
}  // namespace debug
```

Stores the most recent exception's stack trace. Not thread-safe - concurrent exceptions from multiple threads may overwrite each other's stack traces.

## Function Implementations

### `__cxa_throw(ex, info, dest) -> void`

**Brief:** Intercepts exception throws to capture stack traces before the real throw occurs.

**Implementation:**
- Captures stack with `backtrace()` into global storage
- Uses `dlsym(RTLD_NEXT, ...)` to find the real `__cxa_throw`
- Forwards all parameters to the real implementation
- Compiler-specific handling for GCC vs Clang ABI differences

**Thread Safety:** Not thread-safe (writes to global storage)

---

### `getStackTrace() -> std::string`

**Brief:** Captures the current call stack and formats it as a human-readable string.

**Implementation:**
```cpp
std::string getStackTrace() {
  debug::stack::last_size = backtrace(debug::stack::last_frames, 1024);
  std::ostringstream os;
  os << "Stack trace [" << debug::stack::last_size << " frames]:\n";
  if (debug::stack::last_size != 0) {
    os << demangleStackTrace(debug::stack::last_frames, debug::stack::last_size);
  }
  return os.str();
}
```

**Thread Safety:** Not thread-safe (writes to global storage)

---

### `getCxaThrowStack() -> std::string`

**Brief:** Returns the stack trace that was captured at the most recent exception throw point.

**Implementation:**
- Returns `"None"` if `last_size == 0` (no exception thrown yet)
- Otherwise formats the stored stack frames using `demangleStackTrace()`

**Thread Safety:** Not thread-safe (reads global storage)

---

### `demangle(const char* name) -> std::string`

**Brief:** Converts a mangled C++ symbol name to human-readable form.

**Implementation:**
```cpp
std::string demangle(const char* name) {
  int status;
  std::unique_ptr<char, void (*)(void*)> realname(
      abi::__cxa_demangle(name, nullptr, nullptr, &status), &std::free);
  return status != 0 ? name : &*realname;
}
```

**Note:** Uses `unique_ptr` with custom deleter to ensure memory is freed.

**Thread Safety:** Safe

---

### `demangle(std::string const& name) -> std::string`

**Brief:** Convenience overload that delegates to the `const char*` version.

**Thread Safety:** Safe

---

### `demangleStackTrace(trace, size) -> std::string`

**Brief:** Converts raw backtrace addresses to a formatted, readable stack trace.

**Implementation:**
- Uses `backtrace_symbols()` to convert addresses to symbol strings
- Applies regex to extract function names
- Demangles each function name
- Formats with frame numbers (reverse order - highest frame number first)
- Frees symbol memory with `free()`

**Thread Safety:** Safe

---

### `signalHandler(signum) -> void`

**Brief:** Signal handler that logs diagnostic information before terminating.

**Implementation:**
```cpp
void signalHandler(int signum) {
  const char* name = nullptr;
  switch (signum) {
    case SIGABRT: name = "SIGABRT"; break;
    case SIGSEGV: name = "SIGSEGV"; break;
    case SIGBUS:  name = "SIGBUS";  break;
    case SIGILL:  name = "SIGILL";  break;
    case SIGFPE:  name = "SIGFPE";  break;
    case SIGTERM: name = "SIGTERM"; break;
    case SIGQUIT: name = "SIGQUIT"; break;
    case SIGSTKFLT: name = "SIGSTKFLT"; break;
  }

  TLOG(TRACE_LVL) << "Caught signal " << signum << " (" << name << ")";
  TLOG(TRACE_LVL) << getStackTrace();
  exit(signum);
}
```

**Thread Safety:** Not thread-safe (signal handlers are inherently not thread-safe)

---

### `terminateHandler() -> void`

**Brief:** Custom terminate handler that provides detailed exception diagnostics.

**Implementation:**
1. Checks for current exception using `std::current_exception()`
2. If exception exists:
   - Retrieves the throw stack trace
   - Rethrows and catches to get type and message
   - Demangles the exception type name
   - Logs all information via TRACE
3. Logs the current stack trace
4. Exits with SIGTERM

**Thread Safety:** Not thread-safe

---

### `uncaughtExceptionHandler() -> void`

**Brief:** Handler for unexpected exceptions (largely unused due to C++17 deprecation of `std::set_unexpected`).

**Implementation:**
- Logs a message indicating unexpected handler was called
- Logs the throw stack trace
- Delegates to `terminateHandler()`

**Thread Safety:** Not thread-safe

---

### `registerAbortHandler() -> void`

**Brief:** Installs signal handlers for fatal signals.

**Implementation:**
```cpp
void registerAbortHandler() {
  signal(SIGABRT, signalHandler);
  signal(SIGSEGV, signalHandler);
  signal(SIGBUS, signalHandler);
  signal(SIGILL, signalHandler);
  signal(SIGFPE, signalHandler);
  signal(SIGQUIT, signalHandler);
  signal(SIGSTKFLT, signalHandler);

  std::set_terminate(terminateHandler);
}
```

**Thread Safety:** Not thread-safe (modifies global handlers)

---

### `registerTerminateHandler() -> void`

**Brief:** Installs the custom terminate handler.

**Implementation:**
```cpp
void registerTerminateHandler() {
  std::set_terminate(terminateHandler);
}
```

**Thread Safety:** Not thread-safe

---

### `registerUncaughtExceptionHandler() -> void`

**Brief:** Placeholder for unexpected exception handler (deprecated in C++17).

**Implementation:** Currently empty (commented-out `std::set_unexpected` call).

**Thread Safety:** Not thread-safe

---

### `trace_enable() -> void`

**Brief:** Enables and configures the TRACE logging system.

**Implementation:**
```cpp
void trace_enable() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);
}
```

**Thread Safety:** Not thread-safe (modifies global TRACE state)

---

### `registerUngracefullExitHandlers() -> void`

**Brief:** One-call registration of all debugging handlers.

**Implementation:**
```cpp
void registerUngracefullExitHandlers() {
  trace_enable();
  registerAbortHandler();
  registerTerminateHandler();
  registerUncaughtExceptionHandler();
}
```

**Thread Safety:** Not thread-safe (should be called once at startup)

---

### `current_exception_diagnostic_information() -> std::string`

**Brief:** Combines throw stack trace with Boost exception diagnostic information.

**Implementation:**
```cpp
std::string current_exception_diagnostic_information() {
  std::ostringstream os;
  os << "Process exited";
  os << getCxaThrowStack();
  os << "Error: " << boost::current_exception_diagnostic_information();
  return os.str();
}
```

**Thread Safety:** Not thread-safe (reads global stack state)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/printStackTrace.h"
#include <iostream>

void safeWrapper() {
    try {
        riskyOperation();
    } catch (...) {
        std::cerr << debug::current_exception_diagnostic_information();
        throw;
    }
}
```

---

## Performance Considerations

| Operation | Approximate Cost |
|-----------|-----------------|
| Handler registration | ~1 microsecond (done once) |
| `__cxa_throw` interception | ~10 microseconds per throw |
| Stack trace capture (`backtrace()`) | ~100 microseconds |
| Stack trace formatting | ~1 millisecond (depends on depth) |

Stack capture is expensive but only occurs during errors, so the debugging benefit outweighs the cost.

## Error Handling Strategy

- Signal handlers log diagnostics via TRACE before exiting
- Terminate handler catches and logs exception information
- All handlers capture stack traces for post-mortem analysis

## Dependencies

| Include | Purpose |
|---------|---------|
| `<cxxabi.h>` | Name demangling (`abi::__cxa_demangle`) |
| `<dlfcn.h>` | Dynamic linking (`dlsym`) |
| `<execinfo.h>` | Stack traces (`backtrace`, `backtrace_symbols`) |
| `<libgen.h>` | Path manipulation (`basename`) |
| `<cerrno>` | Error numbers |
| `<csignal>` | Signal handling |
| `<cstdio>` | Standard I/O |
| `<cstdlib>` | `exit()`, `free()` |
| `<iostream>` | Output streams |
| `<memory>` | `unique_ptr` |
| `<ostream>` | Stream operations |
| `<regex>` | Function name extraction |
| `<string>` | String class |
| `<typeinfo>` | Type information |
| `common.h` | TRACE macros |
| `printStackTrace.h` | Header declarations |
| `process_exit_codes.h` | Exit codes, trace modes |
| `<boost/exception/diagnostic_information.hpp>` | Exception diagnostics |

## TRACE Configuration

- `TRACE_NAME`: `"printStackTrace.cpp"`
- `TRACE_LVL`: 11 (default, can be overridden)

Used for logging signal and terminate handler diagnostics.

## Thread Safety

- Global `last_frames` and `last_size` are not thread-safe
- Concurrent exceptions from multiple threads may overwrite each other's stack traces
- Signal handlers are inherently not thread-safe
- Handler registration should be done once at startup before spawning threads

## Platform Requirements

- **Operating System:** Unix/Linux only
- **Compiler:** GCC or Clang (C++ ABI specific)
- **Library:** glibc (for `backtrace()`)
- **Compilation flags:**
  - `-rdynamic` for symbol export in executables
  - `-g` for meaningful stack traces
  - Note: High optimization (`-O2`, `-O3`) may affect accuracy due to inlining

## Testing Notes

- **Unit tests:** Debugging infrastructure is tested indirectly through exception handling
- **Key test cases:** Stack trace capture, exception interception, signal handling

## See Also

- [printStackTrace.h](./printStackTrace.h.md) - Function declarations
- [process_exit_codes.h](./process_exit_codes.h.md) - Exit codes and trace mode constants
- [helper_functions.h](./helper_functions.h.md) - `confirm()` assertion that uses `getStackTrace()`
- [common.h](./common.h.md) - TRACE logging macros
