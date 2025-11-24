# process_exit_codes.h

## File Overview

This header file defines standardized exit codes and trace mode settings used throughout the artdaq-database project. It provides consistent return values for processes and configures trace logging behavior.

**Location**: `/home/user/artdaq-database/artdaq-database/SharedCommon/process_exit_codes.h`

## Dependencies

No external dependencies - this is a pure constant definition header.

## Namespace: process_exit_code

Defines standardized process exit codes following Unix conventions and extending them for application-specific needs.

### Exit Code Constants

```cpp
constexpr int INVALID_ARGUMENT = 128;
constexpr int UNCAUGHT_EXCEPTION = 144;
constexpr int SUCCESS = 0;
constexpr int FAILURE = 1;
constexpr int HELP = 1;
```

#### SUCCESS
**Value**: `0`

**Purpose**: Indicates successful program execution.

**Usage**: Return from main() when program completes without errors.

**Example**:
```cpp
int main() {
    // ... successful execution ...
    return process_exit_code::SUCCESS;
}
```

#### FAILURE
**Value**: `1`

**Purpose**: Generic failure exit code.

**Usage**: Return from main() when program encounters a non-specific error.

**Example**:
```cpp
int main() {
    if (!initialize()) {
        return process_exit_code::FAILURE;
    }
    // ...
}
```

#### HELP
**Value**: `1`

**Purpose**: Exit code when displaying help message and exiting.

**Usage**: Typically used when user requests help (--help, -h flags).

**Note**: Some Unix conventions use 0 for help display, but this uses 1 to indicate "no work was performed".

**Example**:
```cpp
if (argc == 1 || show_help) {
    print_usage();
    return process_exit_code::HELP;
}
```

#### INVALID_ARGUMENT
**Value**: `128`

**Purpose**: Indicates invalid command-line arguments or parameters.

**Rationale**: Value 128 is traditionally used for invalid arguments (Bash uses 128+ for signal-related exits).

**Usage**: Return when command-line parsing fails or arguments are invalid.

**Example**:
```cpp
try {
    parse_arguments(argc, argv);
} catch (const std::invalid_argument& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return process_exit_code::INVALID_ARGUMENT;
}
```

#### UNCAUGHT_EXCEPTION
**Value**: `144`

**Purpose**: Indicates program terminated due to an uncaught exception.

**Rationale**: Value 144 is chosen to be distinct from standard Unix exit codes (0-127) and signal codes (128-192).

**Usage**: Used in exception handlers at the top level of main().

**Example**:
```cpp
int main() {
    try {
        // ... program logic ...
    } catch (const std::exception& e) {
        std::cerr << "Uncaught exception: " << e.what() << std::endl;
        return process_exit_code::UNCAUGHT_EXCEPTION;
    }
}
```

## Namespace: trace_mode

Defines constants for configuring the TRACE logging system behavior.

### Trace Mode Constants

```cpp
constexpr auto modeM = 1LL;

#ifndef NDEBUG
constexpr auto modeS = 0LL;
#else
constexpr auto modeS = 0LL;
#endif
```

#### modeM
**Value**: `1LL` (long long 1)

**Purpose**: TRACE mode M setting - controls memory buffering mode.

**Usage**: Passed to TRACE_CNTL to configure trace buffering behavior.

**Technical Details**:
- modeM=1: Traces are written to memory buffer first
- Improves performance by avoiding immediate I/O for each trace

**Example**:
```cpp
TRACE_CNTL("modeM", trace_mode::modeM);
```

#### modeS
**Value**: `0LL` (long long 0)

**Purpose**: TRACE mode S setting - controls slow-path tracing.

**Build Configuration**:
- Debug builds (NDEBUG not defined): `0LL`
- Release builds (NDEBUG defined): `0LL`

**Note**: Currently both debug and release use the same value (0), but the conditional compilation structure allows for future differentiation.

**Technical Details**:
- modeS=0: Standard trace path
- Could be used to enable/disable slow-path debugging

**Usage**: Passed to TRACE_CNTL to configure trace behavior.

**Example**:
```cpp
TRACE_CNTL("modeS", trace_mode::modeS);
```

## Usage Context

These constants are used throughout the artdaq-database project in several contexts:

### 1. Main Function Return Values

```cpp
int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            print_usage();
            return process_exit_code::HELP;
        }

        auto result = parse_arguments(argc, argv);
        if (!result) {
            return process_exit_code::INVALID_ARGUMENT;
        }

        execute_operation();

        return process_exit_code::SUCCESS;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return process_exit_code::UNCAUGHT_EXCEPTION;
    }
}
```

### 2. TRACE Configuration

```cpp
#include "artdaq-database/SharedCommon/process_exit_codes.h"

void initialize_tracing() {
    TRACE_CNTL("name", "my_module");
    TRACE_CNTL("modeM", trace_mode::modeM);
    TRACE_CNTL("modeS", trace_mode::modeS);
    TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
}
```

### 3. Shell Script Integration

Shell scripts can check exit codes to determine what happened:

```bash
#!/bin/bash
artdaq_database_tool --operation=load

case $? in
    0)
        echo "Success"
        ;;
    1)
        echo "Generic failure"
        ;;
    128)
        echo "Invalid arguments provided"
        exit 128
        ;;
    144)
        echo "Program crashed with uncaught exception"
        exit 144
        ;;
    *)
        echo "Unknown exit code"
        ;;
esac
```

## Exit Code Conventions

The file follows these conventions:

| Range | Meaning | Example |
|-------|---------|---------|
| 0 | Success | Normal completion |
| 1 | Generic failure | General errors, help display |
| 2-127 | Reserved | Available for application-specific codes |
| 128 | Invalid argument | Bad command-line parameters |
| 129-143 | Available | Could be used for more specific errors |
| 144 | Uncaught exception | Program crash |
| 145-255 | Available | Could be used for more error types |

**Unix Tradition**:
- 0: Success
- 1-127: Various error conditions
- 128+N: Terminated by signal N
- 255: Exit status out of range

**This Implementation**:
- 0: Success (standard)
- 1: Generic failure and help (standard)
- 128: Invalid argument (traditional for shells)
- 144: Uncaught exception (custom choice)

## Design Rationale

### Why constexpr?
- Compile-time constants have zero runtime overhead
- Can be used in template parameters and constant expressions
- Compiler can optimize switches on constexpr values

### Why namespace instead of enum?
- Namespaces allow same type (int) for all codes
- Compatible with standard exit() and return conventions
- No need for explicit casting
- Can be extended without modifying original definition

### Exit Code Selection
- **128**: Chosen for INVALID_ARGUMENT because bash uses 128+ for signal terminations, and 128 specifically is often used for invalid arguments
- **144**: Chosen for UNCAUGHT_EXCEPTION to be distinct from signal codes (128-192) but still in extended range

## Related Files

- **printStackTrace.h/cpp** - Uses these exit codes in signal handlers
- **Common main() functions** - Return these codes to indicate status
- Shell scripts and process monitors - Check these codes to determine program outcome

## Best Practices

1. **Always use named constants** instead of magic numbers:
   ```cpp
   // Good
   return process_exit_code::INVALID_ARGUMENT;

   // Bad
   return 128;
   ```

2. **Handle all exit paths** in main():
   ```cpp
   int main() {
       try {
           // Normal execution
           return process_exit_code::SUCCESS;
       } catch (const std::invalid_argument&) {
           return process_exit_code::INVALID_ARGUMENT;
       } catch (...) {
           return process_exit_code::UNCAUGHT_EXCEPTION;
       }
   }
   ```

3. **Document exit codes** in program help/documentation so users and scripts know what to expect.

4. **Initialize TRACE early** using the trace_mode constants for consistent logging behavior.

## Notes

- The modeS constant has the same value in both debug and release builds (both 0LL), but the conditional compilation structure allows for future differentiation
- Exit codes above 128 should be used carefully as they can conflict with signal-based terminations
- The TRACE system is part of the artdaq framework and these constants configure its behavior
