# printStackTrace.cpp

## File Overview

This implementation file provides the actual implementation of stack trace capture, exception diagnostics, signal handling, and C++ ABI exception interception declared in `printStackTrace.h`. It's a critical debugging infrastructure component that helps diagnose crashes and exceptions in production environments.

**Location**: `/home/user/artdaq-database/artdaq-database/SharedCommon/printStackTrace.cpp`

## Dependencies

### System Headers
- `<cxxabi.h>` - C++ ABI for name demangling
- `<dlfcn.h>` - Dynamic linking (dlsym for finding __cxa_throw)
- `<execinfo.h>` - Stack trace support (backtrace functions)
- `<libgen.h>` - Path manipulation (basename)
- `<cerrno>` - Error numbers
- `<csignal>` - Signal handling
- `<cstdio>` - C I/O
- `<cstdlib>` - C standard library
- `<iostream>` - C++ I/O
- `<memory>` - Smart pointers
- `<ostream>` - Output streams
- `<regex>` - Regular expressions
- `<string>` - String class
- `<typeinfo>` - Type information

### Project Headers
- `"artdaq-database/SharedCommon/common.h"` - Common includes
- `"artdaq-database/SharedCommon/printStackTrace.h"` - Function declarations
- `"artdaq-database/SharedCommon/process_exit_codes.h"` - Exit codes

### Third-Party Libraries
- `<boost/exception/diagnostic_information.hpp>` - Exception diagnostics

## TRACE Configuration

```cpp
#ifndef TRACE_LVL
#define TRACE_LVL 11
#endif

#define TRACE_NAME "printStackTrace.cpp"
```

Sets default TRACE level to 11 and identifies this module for logging.

## Global State

### Stack Trace Storage

```cpp
namespace debug {
namespace stack {
void* last_frames[1024];
size_t last_size;
}  // namespace stack
}  // namespace debug
```

**Purpose**: Global storage for the most recent stack trace captured at exception throw.

**Structure**:
- `last_frames`: Array of 1024 void pointers (return addresses)
- `last_size`: Number of valid frames in array

**Thread Safety**: Not thread-safe. In multithreaded programs, traces from different threads could overwrite each other.

---

## C++ ABI Exception Interception

### __cxa_throw Implementation

```cpp
extern "C" {
#ifndef __clang__
void __cxa_throw(void* ex, void* info, void (*dest)(void*)) {
    debug::stack::last_size = backtrace(debug::stack::last_frames,
                                        sizeof debug::stack::last_frames / sizeof(void*));

    __cxa_throw_t* rethrow __attribute__((noreturn)) = (__cxa_throw_t*)dlsym(RTLD_NEXT, "__cxa_throw");

    rethrow(ex, info, dest);
}
#else
__attribute__((noreturn)) void __cxa_throw(void* ex, std::type_info* info, void (*dest)(void*)) {
    debug::stack::last_size = backtrace(debug::stack::last_frames,
                                        sizeof debug::stack::last_frames / sizeof(void*));

    auto* rethrow = (__cxa_throw_t*)dlsym(RTLD_NEXT, "__cxa_throw");

    rethrow(ex, info, dest);
}
#endif
}
```

**Purpose**: Intercept all C++ exception throws to capture stack trace.

**How It Works**:
1. **Capture Stack**: Uses `backtrace()` to capture current call stack
2. **Find Real __cxa_throw**: Uses `dlsym(RTLD_NEXT, "__cxa_throw")` to find the actual C++ runtime implementation
3. **Forward Call**: Calls the real __cxa_throw to continue normal exception handling

**Compiler Differences**:
- **GCC**: Uses `void*` for type_info parameter
- **Clang**: Uses `std::type_info*` and requires `noreturn` attribute

**Technical Details**:
- This relies on dynamic linking to interpose the C++ ABI function
- `RTLD_NEXT` finds the next occurrence of the symbol in the library search order
- Must be compiled with `-rdynamic` or similar to export symbols

**Limitations**:
- Only works with dynamic linking
- Stack trace storage is not thread-safe
- Adds small overhead to every exception throw

---

## Stack Trace Functions

### getStackTrace

```cpp
std::string getStackTrace() {
    debug::stack::last_size = backtrace(debug::stack::last_frames,
                                        sizeof debug::stack::last_frames / sizeof(void*));

    std::ostringstream os;
    os << "Stack trace [" << debug::stack::last_size << " frames]:\n";
    if (debug::stack::last_size != 0) {
        os << demangleStackTrace(debug::stack::last_frames, debug::stack::last_size);
    }

    return os.str();
}
```

**Implementation Details**:
- Captures current stack using `backtrace()`
- Stores in global `last_frames` array
- Formats with frame count header
- Calls `demangleStackTrace()` for human-readable output

**Output Example**:
```
Stack trace [15 frames]:
 15 main (artdaq_database_tool.cpp:123)
 14 process_command (command_processor.cpp:456)
 13 execute_operation (operations.cpp:789)
  ...
```

---

### getCxaThrowStack

```cpp
std::string getCxaThrowStack() {
    if (debug::stack::last_size == 0) {
        return "None";
    }

    std::ostringstream os;
    os << "Stack trace [" << debug::stack::last_size << " frames] @ __cxa_throw():\n";
    os << demangleStackTrace(debug::stack::last_frames, debug::stack::last_size) << "\n";

    return os.str();
}
```

**Implementation Details**:
- Returns stack captured by __cxa_throw interception
- Returns "None" if no exception has been thrown
- Includes special marker "@ __cxa_throw()" in header

**Use Case**: Shows where an exception was originally thrown, not just where it was caught.

---

### demangle (const char*)

```cpp
std::string demangle(const char* name) {
    int status;
    std::unique_ptr<char, void (*)(void*)> realname(
        abi::__cxa_demangle(name, nullptr, nullptr, &status),
        &std::free);
    return status != 0 ? name : &*realname;
}
```

**Implementation Details**:
- Uses C++ ABI `__cxa_demangle()` function
- Wraps result in unique_ptr with custom deleter (std::free)
- Returns original name if demangling fails

**Parameters**:
- `name` - Mangled C++ symbol name

**Status Codes**:
- 0: Success
- -1: Memory allocation failure
- -2: Invalid mangled name
- -3: Invalid arguments

**Example Transformation**:
```
Input:  "_ZNSt6vectorIiSaIiEE9push_backERKi"
Output: "std::vector<int, std::allocator<int> >::push_back(int const&)"
```

---

### demangle (std::string)

```cpp
std::string demangle(std::string const& name) {
    return debug::demangle(name.c_str());
}
```

**Purpose**: String overload that forwards to const char* version.

---

### demangleStackTrace

```cpp
std::string demangleStackTrace(void* const* trace, int size) {
    char** symbols = backtrace_symbols(trace, size);
    std::ostringstream os;

    auto ex = std::regex{"[(](.*)[+]"};

    auto make_readable = [&ex](char* sym) {
        auto val = std::string{basename(sym)};
        auto m = std::smatch();

        if (std::regex_search(val, m, ex)) {
            if (m.size() > 1) {
                val.replace(m.position(1), m.length(1), demangle(m[1]));
            }
        }

        return val;
    };

    for (int i = size; i > 0; i--) {
        os << std::setw(3) << i << " " << make_readable(symbols[i - 1]) << "\n";
    }

    free(symbols);

    return os.str();
}
```

**Implementation Details**:

1. **Get Symbols**: `backtrace_symbols()` converts addresses to symbol information
2. **Parse Format**: Symbol format is typically: `path/to/binary(symbol+offset) [address]`
3. **Extract Function**: Regex `[(](.*)[+]` extracts function name between '(' and '+'
4. **Demangle**: Converts mangled name to readable form
5. **Format Output**: Numbers frames in reverse order (innermost last)
6. **Cleanup**: Frees memory allocated by backtrace_symbols

**Regex Pattern**: `[(](.*)[+]`
- `[(]` - Literal opening parenthesis
- `(.*)` - Capture group: any characters (function name)
- `[+]` - Literal plus sign

**Output Format**:
```
  5 artdaq_database_tool(main+0x123) [0x400abc]
  4 libdatabase.so(artdaq::database::operation::execute()+0x45) [0x7f1234]
  3 libdatabase.so(_ZN6artdaq8database9operation7executeEv+0x45) [0x7f1234]
```
After processing:
```
  5 main
  4 artdaq::database::operation::execute()
  3 artdaq::database::operation::execute()
```

---

## Signal Handler

### signalHandler

```cpp
void signalHandler(int signum) {
    const char* name = nullptr;
    switch (signum) {
        case SIGABRT:   name = "SIGABRT"; break;
        case SIGSEGV:   name = "SIGSEGV"; break;
        case SIGBUS:    name = "SIGBUS"; break;
        case SIGILL:    name = "SIGILL"; break;
        case SIGFPE:    name = "SIGFPE"; break;
        case SIGTERM:   name = "SIGTERM"; break;
        case SIGQUIT:   name = "SIGQUIT"; break;
        case SIGSTKFLT: name = "SIGSTKFLT"; break;
    }

    if (name != nullptr) {
        TLOG(TRACE_LVL) << __func__ << ": Caught signal " << signum << " (" << name << ")";
    } else {
        TLOG(TRACE_LVL) << __func__ << ": Caught signal " << signum;
    }

    TLOG(TRACE_LVL) << __func__ << ": " << getStackTrace();

    exit(signum);
}
```

**Purpose**: Handle fatal signals by logging stack trace before terminating.

**Signals Handled**:
- `SIGABRT` (6) - Abort signal from abort()
- `SIGSEGV` (11) - Segmentation fault
- `SIGBUS` (7) - Bus error (alignment, non-existent memory)
- `SIGILL` (4) - Illegal instruction
- `SIGFPE` (8) - Floating-point exception
- `SIGTERM` (15) - Termination signal
- `SIGQUIT` (3) - Quit from keyboard (Ctrl-\)
- `SIGSTKFLT` (16) - Stack fault on coprocessor

**Behavior**:
1. Map signal number to name
2. Log signal number and name
3. Capture and log stack trace
4. Exit with signal number as exit code

**Usage Context**: Installed by `registerAbortHandler()`.

---

## Terminate Handler

### terminateHandler

```cpp
void terminateHandler() {
    std::exception_ptr exptr = std::current_exception();
    if (exptr != nullptr) {
        auto pending = getCxaThrowStack();
        try {
            std::rethrow_exception(exptr);
        } catch (std::exception const& ex) {
            size_t funcnamesize = 1024;
            char funcname[1024];
            int status = 0;

            TLOG(TRACE_LVL) << __func__ << ": Terminate called after throwing an instance of \'"
                            << abi::__cxa_demangle(typeid(ex).name(), funcname, &funcnamesize, &status) << "\'";

            TLOG(TRACE_LVL) << __func__ << ": what(): " << ex.what();
            TLOG(TRACE_LVL) << __func__ << ": details: " << pending;

        } catch (...) {
            TLOG(TRACE_LVL) << __func__ << ": Terminate called after throwing an instance of unknown exception";
        }
    } else {
        TLOG(TRACE_LVL) << __func__ << ": Terminate called";
    }

    TLOG(TRACE_LVL) << __func__ << ": " << getStackTrace();

    exit(SIGTERM);
}
```

**Purpose**: Handle std::terminate() calls with detailed diagnostics.

**Implementation Details**:

1. **Check for Active Exception**: `std::current_exception()` returns exception_ptr if called during exception handling
2. **Get Throw Stack**: Retrieve stack from __cxa_throw interception
3. **Rethrow and Catch**: Rethrow to access exception object
4. **Demangle Type**: Show human-readable exception type
5. **Log Details**: Show what() message and throw stack
6. **Current Stack**: Show stack at terminate point
7. **Exit**: Terminate with SIGTERM code

**Scenarios Handled**:
- Known exception types (std::exception)
- Unknown exception types (catch(...))
- No exception (terminate called directly)

**Output Example**:
```
terminateHandler: Terminate called after throwing an instance of 'artdaq::database::invalid_argument'
terminateHandler: what(): Invalid collection name
terminateHandler: details: Stack trace [12 frames] @ __cxa_throw():
  12 artdaq::database::validate_name (validator.cpp:45)
  ...
terminateHandler: Stack trace [8 frames]:
  8 terminate (terminate.cpp:123)
  ...
```

---

## Uncaught Exception Handler

### uncaughtExceptionHandler

```cpp
void uncaughtExceptionHandler() {
    TLOG(TRACE_LVL) << __func__ << ": Unexpected handler function called.";
    TLOG(TRACE_LVL) << __func__ << ": " << getCxaThrowStack();
    terminateHandler();
}
```

**Purpose**: Handle unexpected exceptions (deprecated mechanism).

**Note**: This is rarely called in modern C++ as std::set_unexpected is deprecated.

---

## Handler Registration Functions

### registerAbortHandler

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

**Purpose**: Register handlers for fatal signals and terminate condition.

**Behavior**: Installs `signalHandler` for 7 different fatal signals and sets terminate handler.

---

### registerTerminateHandler

```cpp
void registerTerminateHandler() {
    std::set_terminate(terminateHandler);
}
```

**Purpose**: Only register terminate handler without signal handlers.

---

### registerUncaughtExceptionHandler

```cpp
void registerUncaughtExceptionHandler() {
    /*std::set_unexpected(uncaughtExceptionHandler);*/
}
```

**Purpose**: Would register unexpected handler, but is commented out due to deprecation.

---

### trace_enable

```cpp
void trace_enable() {
    TRACE_CNTL("name", TRACE_NAME);
    TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
    TRACE_CNTL("modeM", trace_mode::modeM);
    TRACE_CNTL("modeS", trace_mode::modeS);
}
```

**Purpose**: Configure TRACE logging system.

**Settings**:
- Set module name to "printStackTrace.cpp"
- Enable all trace levels (all bits set)
- Configure memory mode
- Configure slow-path mode

---

### registerUngracefullExitHandlers

```cpp
void registerUngracefullExitHandlers() {
    trace_enable();
    registerAbortHandler();
    registerTerminateHandler();
    registerUncaughtExceptionHandler();
}
```

**Purpose**: One-call registration of all debugging infrastructure.

**Order of Operations**:
1. Enable TRACE (so handlers can log)
2. Register abort/signal handlers
3. Register terminate handler
4. Register uncaught exception handler (no-op)

---

## Exception Diagnostics

### current_exception_diagnostic_information

```cpp
std::string current_exception_diagnostic_information() {
    std::ostringstream os;

    os << "Process exited";
    os << getCxaThrowStack();
    os << "Error: " << boost::current_exception_diagnostic_information();

    return os.str();
}
```

**Purpose**: Gather comprehensive exception diagnostic information.

**Includes**:
1. Generic header
2. Stack trace from throw point
3. Boost diagnostic information (includes type, message, nested exceptions)

**Usage Context**: Called from catch blocks and exception handlers.

---

## Design Patterns and Techniques

### RAII for Symbol Names
```cpp
std::unique_ptr<char, void (*)(void*)> realname(
    abi::__cxa_demangle(...),
    &std::free);
```
Uses unique_ptr with custom deleter to ensure demangled string is freed.

### Lambda for Local Processing
```cpp
auto make_readable = [&ex](char* sym) {
    // ... processing logic ...
};
```
Encapsulates symbol processing logic in demangleStackTrace.

### Dynamic Symbol Resolution
```cpp
auto* rethrow = (__cxa_throw_t*)dlsym(RTLD_NEXT, "__cxa_throw");
```
Finds next symbol in library chain to avoid infinite recursion.

## Performance Considerations

1. **Stack Capture**: `backtrace()` is relatively expensive (~100 microseconds)
2. **Symbol Resolution**: `backtrace_symbols()` does filesystem I/O
3. **Demangling**: Can be slow for complex template names
4. **Global State**: Non-thread-safe last_frames storage

**Mitigation**: These costs are only incurred during errors/crashes, so acceptable for debugging.

## Thread Safety

**Not Thread-Safe**:
- `last_frames` and `last_size` are global
- Multiple threads throwing exceptions simultaneously will race
- Signal handlers are process-wide

**Recommendation**: In multithreaded code, consider thread-local storage for stack traces.

## Limitations

1. **Dynamic Linking Required**: Symbol interposition needs dynamic linking
2. **Unix/Linux Only**: Uses POSIX-specific APIs
3. **Debug Symbols**: Stack traces are most useful with -g flag
4. **Optimization**: Inlining from -O2/-O3 can obscure call chains
5. **Single Trace**: Only stores most recent exception throw stack

## Related Files

- **printStackTrace.h** - Function declarations
- **process_exit_codes.h** - Exit codes and trace modes
- **helper_functions.h** - Uses getStackTrace() in confirm()
- **shared_exceptions.h** - Exception types that trigger these handlers

## Best Practices

1. **Initialize Early**: Call registerUngracefullExitHandlers() at program start
2. **Compile with Symbols**: Use -g flag for meaningful stack traces
3. **Link Dynamically**: Use -rdynamic for symbol interposition
4. **Check Logs**: Always check TRACE logs after crashes

## Notes

- Comment typo on line 122: "aosociate" should be "associate"
- The implementation is production-ready and battle-tested in artdaq projects
- Stack trace depth limited to 1024 frames (sufficient for most use cases)
