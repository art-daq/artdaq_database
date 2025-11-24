# printStackTrace.h

## File Overview

This header file declares functions and handlers for debugging and diagnostic purposes, including stack trace generation, exception handling hooks, signal handlers, and C++ ABI exception interception. It provides critical debugging infrastructure for the artdaq-database project.

**Location**: `/home/user/artdaq-database/artdaq-database/SharedCommon/printStackTrace.h`

## Dependencies

### Standard Library
- `<string>` - String handling

### System Headers
- None explicitly in header (included in .cpp)

## Header Guard

```cpp
#ifndef PRINTSTACKTRACE_H
#define PRINTSTACKTRACE_H
```

## External C Declarations

The file declares C++ ABI exception throwing functions to allow interception:

### __cxa_throw Declaration

```cpp
extern "C" {

#ifndef __clang__
typedef void(__cxa_throw_t)(void*, void*, void (*)(void*));
void __cxa_throw(void*, void*, void (*)(void*));
#else   //__clang__
typedef __attribute__((noreturn)) void(__cxa_throw_t)(void*, std::type_info*, void (*)(void*));
__attribute__((noreturn)) void __cxa_throw(void*, std::type_info*, void (*)(void*));
#endif  //__clang__

}
```

**Purpose**: Declare the C++ ABI's exception throwing mechanism to allow interception.

**Compiler Differences**:
- **GCC**: Uses `void*` for type_info parameter
- **Clang**: Uses `std::type_info*` and marks as `noreturn`

**Function Signature**:
- First parameter: Pointer to exception object
- Second parameter: Type information (GCC: void*, Clang: std::type_info*)
- Third parameter: Destructor function pointer

**Usage Context**: This is implemented in printStackTrace.cpp to capture stack traces at exception throw points.

---

## Namespace: debug

All debugging utilities are in the `debug` namespace.

### Handler Registration Functions

#### registerAbortHandler
```cpp
void registerAbortHandler();
```

**Purpose**: Register signal handlers for fatal signals (SIGABRT, SIGSEGV, SIGBUS, etc.).

**Behavior**:
- Installs handlers for multiple fatal signals
- Handlers print stack trace before terminating
- Also calls std::set_terminate()

**Signals Handled**:
- SIGABRT - Abort signal
- SIGSEGV - Segmentation fault
- SIGBUS - Bus error
- SIGILL - Illegal instruction
- SIGFPE - Floating-point exception
- SIGQUIT - Quit signal
- SIGSTKFLT - Stack fault

**Usage Example**:
```cpp
int main() {
    debug::registerAbortHandler();
    // ... rest of program ...
}
```

**Important**: Should be called early in main() before any code that might crash.

---

#### registerTerminateHandler
```cpp
void registerTerminateHandler();
```

**Purpose**: Register a handler for std::terminate() calls.

**Behavior**:
- Installs custom terminate handler
- Prints exception information and stack trace
- Calls std::set_terminate()

**When Invoked**:
- Exception thrown with no handler
- Exception thrown during stack unwinding
- Destructor throws during exception handling
- std::terminate() called explicitly

**Usage Example**:
```cpp
debug::registerTerminateHandler();
```

---

#### registerUncaughtExceptionHandler
```cpp
void registerUncaughtExceptionHandler();
```

**Purpose**: Register handler for uncaught exceptions (currently a no-op).

**Note**: The implementation is commented out:
```cpp
/*std::set_unexpected(uncaughtExceptionHandler);*/
```

**Historical Context**: std::set_unexpected() was deprecated in C++11 and removed in C++17, explaining why this is disabled.

---

#### registerUngracefullExitHandlers
```cpp
void registerUngracefullExitHandlers();
```

**Purpose**: One-call registration of all debugging handlers.

**Behavior**:
- Calls trace_enable()
- Calls registerAbortHandler()
- Calls registerTerminateHandler()
- Calls registerUncaughtExceptionHandler()

**Usage Example**:
```cpp
int main() {
    debug::registerUngracefullExitHandlers();  // Register all handlers at once
    // ... rest of program ...
}
```

**Best Practice**: Call this at the very beginning of main() to ensure all crashes are properly diagnosed.

---

### TRACE Configuration

#### trace_enable
```cpp
void trace_enable();
```

**Purpose**: Enable and configure the TRACE logging system.

**Behavior**:
- Sets TRACE name
- Configures log levels
- Sets trace modes from process_exit_codes.h

**Usage Example**:
```cpp
debug::trace_enable();
TLOG(10) << "Logging is now enabled";
```

---

### Exception Information Functions

#### current_exception_diagnostic_information
```cpp
std::string current_exception_diagnostic_information();
```

**Purpose**: Get detailed diagnostic information about the current exception.

**Returns**: String containing:
- Stack trace from exception throw point
- Boost diagnostic information
- Exception type and message

**Usage Context**: Called from catch blocks to get detailed error information.

**Usage Example**:
```cpp
try {
    risky_operation();
} catch (...) {
    std::cerr << debug::current_exception_diagnostic_information();
    throw;
}
```

---

### Symbol Demangling Functions

C++ compilers "mangle" symbol names to encode type information. These functions reverse that process for human-readable output.

#### demangle (const char*)
```cpp
std::string demangle(const char* mangled_name);
```

**Purpose**: Convert mangled C++ symbol name to readable form.

**Parameters**:
- `mangled_name` - Mangled symbol name from typeid or backtrace

**Returns**: Human-readable symbol name

**Usage Example**:
```cpp
const char* mangled = typeid(std::vector<int>).name();
// mangled might be: "St6vectorIiSaIiEE"
std::string readable = debug::demangle(mangled);
// readable: "std::vector<int, std::allocator<int> >"
```

---

#### demangle (std::string)
```cpp
std::string demangle(std::string const& mangled_name);
```

**Purpose**: String overload of demangle function.

**Parameters**:
- `mangled_name` - Mangled symbol name as string

**Returns**: Human-readable symbol name

**Usage Example**:
```cpp
std::string mangled = "St6vectorIiSaIiEE";
std::string readable = debug::demangle(mangled);
```

---

### Stack Trace Functions

#### getStackTrace
```cpp
std::string getStackTrace();
```

**Purpose**: Capture and return the current call stack.

**Returns**: Formatted string with:
- Number of frames
- Each frame with:
  - Frame number
  - Function name (demangled)
  - File and line information (if available)

**Usage Example**:
```cpp
void problematic_function() {
    std::cerr << "Current stack:\n" << debug::getStackTrace();
}
```

**Output Format**:
```
Stack trace [10 frames]:
 10 main (artdaq_database_tool.cpp:45)
  9 process_request (request_handler.cpp:123)
  8 execute_operation (operation.cpp:67)
  ...
```

---

#### getCxaThrowStack
```cpp
std::string getCxaThrowStack();
```

**Purpose**: Get the stack trace from the most recent exception throw point.

**Returns**: Formatted stack trace captured at __cxa_throw()

**Usage Context**: Called in exception handlers to see where exception originated.

**Usage Example**:
```cpp
try {
    operation();
} catch (const std::exception& e) {
    std::cerr << "Exception thrown at:\n" << debug::getCxaThrowStack();
}
```

**How It Works**: The __cxa_throw() interception (defined in .cpp) captures the stack before throwing, and this function retrieves it.

**Output Format**:
```
Stack trace [8 frames] @ __cxa_throw():
  8 artdaq::database::some_function (module.cpp:156)
  7 artdaq::database::caller (caller.cpp:89)
  ...
```

---

#### demangleStackTrace
```cpp
std::string demangleStackTrace(void* const* trace, int size);
```

**Purpose**: Convert array of raw stack frame addresses to readable stack trace.

**Parameters**:
- `trace` - Array of void pointers (return addresses from backtrace())
- `size` - Number of frames in trace

**Returns**: Formatted, human-readable stack trace

**Usage Context**: Internal helper for getStackTrace() and getCxaThrowStack().

**Implementation Note**: Uses backtrace_symbols() and demangle() to convert addresses to readable function names.

---

## Usage Patterns

### Basic Setup

```cpp
#include "artdaq-database/SharedCommon/printStackTrace.h"

int main(int argc, char* argv[]) {
    // Register all debugging handlers
    debug::registerUngracefullExitHandlers();

    try {
        // Your program logic
        run_application(argc, argv);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        std::cerr << debug::current_exception_diagnostic_information();
        return 1;
    }
}
```

### Selective Handler Registration

```cpp
int main() {
    // Only register specific handlers
    debug::trace_enable();           // Enable logging
    debug::registerTerminateHandler();  // Catch std::terminate

    // Run program
}
```

### Manual Stack Trace Capture

```cpp
void diagnose_issue() {
    TLOG(TLVL_ERROR) << "Problem detected!";
    TLOG(TLVL_ERROR) << debug::getStackTrace();
}
```

### Exception Throw Point Diagnosis

```cpp
try {
    complex_operation();
} catch (const artdaq::database::exception& e) {
    std::cerr << "Exception message: " << e.what() << "\n";
    std::cerr << "Thrown from:\n" << debug::getCxaThrowStack() << "\n";
    std::cerr << "Caught at:\n" << debug::getStackTrace() << "\n";
}
```

## Design Rationale

### Why Intercept __cxa_throw?
- Captures stack trace at exception throw point
- Standard catch blocks only show stack from catch point
- Enables debugging of exception origin

### Why Multiple Handlers?
- Different failure modes need different handling
- Signals vs. exceptions vs. terminate conditions
- Provides comprehensive debugging coverage

### Why Custom Demangling?
- Makes stack traces human-readable
- Essential for C++ template-heavy code
- Aids in debugging without debugger attached

## Platform Dependencies

- **__cxa_throw**: GCC/Clang ABI specific
- **backtrace()**: POSIX/glibc specific
- **Signal handling**: Unix/POSIX specific

**Portability**: This code is Unix/Linux specific and would need significant changes for Windows.

## Performance Considerations

1. **Handler Registration**: Minimal overhead, done once at startup
2. **Stack Trace Capture**: Expensive operation, only done on errors
3. **__cxa_throw Interception**: Adds small overhead to every exception throw
4. **Demangling**: Can be slow for complex template names

**Recommendation**: The debugging benefits far outweigh the minimal performance cost in production.

## Related Files

- **printStackTrace.cpp** - Implementation of these functions
- **process_exit_codes.h** - Exit codes used by handlers
- **helper_functions.h** - Uses getStackTrace() in confirm()
- **shared_exceptions.h** - Exception types that benefit from this infrastructure

## Best Practices

1. **Always call registerUngracefullExitHandlers()** early in main()
2. **Use getCxaThrowStack()** in exception handlers for better diagnostics
3. **Enable TRACE** before registering handlers
4. **Include stack traces in error logs** for production debugging
5. **Demangle symbols** when displaying to users

## Notes

- The std::set_unexpected mechanism is deprecated and disabled
- Stack traces work best with debug symbols (-g compiler flag)
- Optimized builds (-O2, -O3) may have inaccurate stack traces due to inlining
- Stack trace depth is limited (typically 1024 frames)
