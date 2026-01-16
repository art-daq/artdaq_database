# process_exit_codes.h

**Path:** `artdaq-database/SharedCommon/process_exit_codes.h`

**Purpose:** Defines standardized process exit codes and TRACE logging mode settings. Using named constants instead of magic numbers makes code more readable and ensures consistent behavior across all command-line tools in the project.


## Key Concepts

### Unix Exit Code Conventions

In Unix/Linux, programs return an integer exit code when they terminate:
- **0** = Success (everything worked correctly)
- **1-127** = Various error conditions (application-defined)
- **128+N** = Terminated by signal N (e.g., 137 = killed by SIGKILL, signal 9)

This file defines application-specific codes that follow and extend these conventions, ensuring scripts and automation can reliably detect error conditions.

### TRACE Logging System

TRACE is a high-performance logging system used throughout the ARTDAQ ecosystem. The `trace_mode` namespace configures how trace messages are buffered and displayed:
- **Memory mode (modeM)**: Fast circular buffer for high-frequency logging
- **Slow mode (modeS)**: Immediate output for debugging

## Thread Safety

- **Thread-safe:** Yes
- **Concurrent access:** All values are `constexpr` constants, evaluated at compile time
- **Locking:** No locking needed; constants cannot be modified at runtime

## Dependencies

| Include | Purpose |
|---------|---------|
| (none) | Pure constant definitions - no external dependencies |

## Constants

### Namespace: `process_exit_code`

Exit codes for command-line tools and applications.

| Constant | Value | Purpose |
|----------|-------|---------|
| `SUCCESS` | `0` | Program completed successfully |
| `FAILURE` | `1` | Generic failure - something went wrong |
| `HELP` | `1` | Displayed help message and exited |
| `INVALID_ARGUMENT` | `128` | Invalid command-line arguments |
| `UNCAUGHT_EXCEPTION` | `144` | Program crashed due to uncaught exception |

---

#### `SUCCESS`

```cpp
constexpr int SUCCESS = 0;
```

**Brief:** Indicates the program completed successfully. This is the standard Unix convention where 0 means no errors occurred.

**Thread Safety:** Thread-safe (compile-time constant)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/process_exit_codes.h"
#include <iostream>

int main() {
    try {
        // Perform operations...
        std::cout << "Operation completed successfully" << std::endl;
        return process_exit_code::SUCCESS;
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return process_exit_code::FAILURE;
    }
}
```

---

#### `FAILURE`

```cpp
constexpr int FAILURE = 1;
```

**Brief:** Indicates generic failure - something went wrong but no more specific error code applies. Use when the operation failed but the cause is not a specific known category.

**Thread Safety:** Thread-safe (compile-time constant)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/process_exit_codes.h"
#include <iostream>

bool initialize();

int main() {
    try {
        if (!initialize()) {
            std::cerr << "Initialization failed" << std::endl;
            return process_exit_code::FAILURE;
        }
        // ... rest of program
        return process_exit_code::SUCCESS;
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return process_exit_code::FAILURE;
    }
}
```

---

#### `HELP`

```cpp
constexpr int HELP = 1;
```

**Brief:** Indicates the help message was displayed and the program exited. Uses value 1 to indicate "no actual work was performed" (not success, but not an error either).

**Thread Safety:** Thread-safe (compile-time constant)

**Note:** Some conventions use 0 for help; this project uses 1 to distinguish "help shown" from "operation succeeded".

**Example:**
```cpp
#include "artdaq-database/SharedCommon/process_exit_codes.h"
#include <iostream>
#include <cstring>

void printUsage(char const* program) {
    std::cout << "Usage: " << program << " [options] <command>\n"
              << "Options:\n"
              << "  --help    Show this help message\n"
              << "  --version Show version information\n";
}

int main(int argc, char** argv) {
    try {
        if (argc == 1) {
            printUsage(argv[0]);
            return process_exit_code::HELP;
        }

        for (int i = 1; i < argc; ++i) {
            if (std::strcmp(argv[i], "--help") == 0 ||
                std::strcmp(argv[i], "-h") == 0) {
                printUsage(argv[0]);
                return process_exit_code::HELP;
            }
        }

        // ... normal execution
        return process_exit_code::SUCCESS;
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return process_exit_code::FAILURE;
    }
}
```

---

#### `INVALID_ARGUMENT`

```cpp
constexpr int INVALID_ARGUMENT = 128;
```

**Brief:** Indicates invalid command-line arguments were provided. Value 128 follows shell conventions for distinguishing argument errors from other failures.

**Thread Safety:** Thread-safe (compile-time constant)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/process_exit_codes.h"
#include "artdaq-database/SharedCommon/shared_exceptions.h"
#include <iostream>

namespace db = artdaq::database;

void parseArguments(int argc, char** argv);

int main(int argc, char** argv) {
    try {
        parseArguments(argc, argv);
        // ... program logic
        return process_exit_code::SUCCESS;
    } catch (db::invalid_argument const& e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        return process_exit_code::INVALID_ARGUMENT;
    } catch (db::invalid_option_exception const& e) {
        std::cerr << "Invalid option: " << e.what() << std::endl;
        return process_exit_code::INVALID_ARGUMENT;
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return process_exit_code::FAILURE;
    }
}
```

---

#### `UNCAUGHT_EXCEPTION`

```cpp
constexpr int UNCAUGHT_EXCEPTION = 144;
```

**Brief:** Indicates the program crashed due to an uncaught exception. Value 144 is chosen to be distinct from signal-related codes (128-143, where 128+signal_number would be used for signals 1-15).

**Thread Safety:** Thread-safe (compile-time constant)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/process_exit_codes.h"
#include <iostream>
#include <exception>

void runApplication();

int main() {
    try {
        runApplication();
        return process_exit_code::SUCCESS;
    } catch (std::exception const& e) {
        std::cerr << "Uncaught exception: " << e.what() << std::endl;
        return process_exit_code::UNCAUGHT_EXCEPTION;
    } catch (...) {
        std::cerr << "Unknown exception caught" << std::endl;
        return process_exit_code::UNCAUGHT_EXCEPTION;
    }
}
```

---

### Namespace: `trace_mode`

TRACE logging configuration constants.

| Constant | Value | Purpose |
|----------|-------|---------|
| `modeM` | `1LL` | TRACE memory buffering mode (fast) |
| `modeS` | `0LL` | TRACE slow-path mode |

---

#### `modeM`

```cpp
constexpr auto modeM = 1LL;
```

**Brief:** TRACE memory buffering mode. Value 1 enables buffered writes to a circular memory buffer for high-performance logging with minimal overhead.

**Thread Safety:** Thread-safe (compile-time constant)

---

#### `modeS`

```cpp
constexpr auto modeS = 0LL;
```

**Brief:** TRACE slow-path mode. Currently set to 0 in both debug and release builds. When enabled (non-zero), provides immediate output at the cost of performance.

**Thread Safety:** Thread-safe (compile-time constant)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/process_exit_codes.h"

void initializeTracing() {
    // Configure TRACE modes
    TRACE_CNTL("modeM", trace_mode::modeM);
    TRACE_CNTL("modeS", trace_mode::modeS);
}
```

## Relationship to Other Components

- **printStackTrace.h/cpp** - Uses exit codes in signal handlers and terminate handlers
- **Utilities/** - All CLI tools (`conftool`, `bulkloader`, etc.) return these codes from `main()`
- **shared_exceptions.h** - Exception types often map to specific exit codes
- **Shell scripts** - Check these codes to determine program outcome and handle errors

## Example

```cpp
#include "artdaq-database/SharedCommon/process_exit_codes.h"
#include "artdaq-database/SharedCommon/shared_exceptions.h"
#include <iostream>
#include <string>

namespace db = artdaq::database;

bool executeCommand(std::string const& command);

int main(int argc, char** argv) {
    try {
        // Check for help request
        if (argc < 2) {
            std::cout << "Usage: " << argv[0] << " <command>" << std::endl;
            return process_exit_code::HELP;
        }

        std::string command = argv[1];

        // Validate arguments
        if (command.empty()) {
            std::cerr << "Error: command cannot be empty" << std::endl;
            return process_exit_code::INVALID_ARGUMENT;
        }

        // Execute command
        if (!executeCommand(command)) {
            std::cerr << "Command execution failed" << std::endl;
            return process_exit_code::FAILURE;
        }

        return process_exit_code::SUCCESS;

    } catch (db::invalid_argument const& e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        return process_exit_code::INVALID_ARGUMENT;

    } catch (db::invalid_option_exception const& e) {
        std::cerr << "Invalid option: " << e.what() << std::endl;
        return process_exit_code::INVALID_ARGUMENT;

    } catch (db::runtime_error const& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        return process_exit_code::FAILURE;

    } catch (std::exception const& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return process_exit_code::UNCAUGHT_EXCEPTION;

    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
        return process_exit_code::UNCAUGHT_EXCEPTION;
    }
}
```

## Notes for Developers

### Always Use Named Constants

```cpp
// GOOD - clear intent, maintainable
return process_exit_code::INVALID_ARGUMENT;

// BAD - magic number, unclear meaning
return 128;
```

### Shell Script Integration

```bash
#!/bin/bash
conftool --operation=load --collection=my_config

case $? in
    0)
        echo "Success"
        ;;
    1)
        echo "Generic failure or help displayed"
        ;;
    128)
        echo "Invalid arguments - check command syntax"
        ;;
    144)
        echo "Uncaught exception - program crashed"
        ;;
    *)
        echo "Unknown exit code: $?"
        ;;
esac
```

### Exit Code Reference Table

| Code | Constant | Meaning | Typical Cause |
|------|----------|---------|---------------|
| 0 | `SUCCESS` | Normal completion | Operation succeeded |
| 1 | `FAILURE` / `HELP` | Generic error or help displayed | Various failures, or user requested help |
| 128 | `INVALID_ARGUMENT` | Bad command-line parameters | Missing required args, unknown options |
| 144 | `UNCAUGHT_EXCEPTION` | Program crashed | Unhandled exception propagated to main |

### Common Pitfalls

- **Pitfall 1:** Using raw numbers instead of constants makes code harder to understand:
  ```cpp
  // BAD
  return 128;

  // GOOD
  return process_exit_code::INVALID_ARGUMENT;
  ```

- **Pitfall 2:** Forgetting to catch exceptions in main() can result in unclean exits:
  ```cpp
  // BAD - uncaught exceptions cause abort()
  int main() {
      riskyOperation();  // May throw
      return 0;
  }

  // GOOD - catch and return appropriate code
  int main() {
      try {
          riskyOperation();
          return process_exit_code::SUCCESS;
      } catch (std::exception const& e) {
          std::cerr << e.what() << std::endl;
          return process_exit_code::UNCAUGHT_EXCEPTION;
      }
  }
  ```

- **Pitfall 3:** Not handling all exception types appropriately:
  ```cpp
  // BAD - all errors return same code
  } catch (...) {
      return 1;
  }

  // GOOD - specific exit codes for specific errors
  } catch (db::invalid_argument const& e) {
      return process_exit_code::INVALID_ARGUMENT;
  } catch (db::runtime_error const& e) {
      return process_exit_code::FAILURE;
  } catch (...) {
      return process_exit_code::UNCAUGHT_EXCEPTION;
  }
  ```

### Anti-patterns

```cpp
// DON'T use magic numbers
if (!result) return 1;

// DO use named constants
if (!result) return process_exit_code::FAILURE;

// DON'T ignore exceptions
int main() {
    doWork();  // BAD: unhandled exceptions
    return 0;
}

// DO catch and handle appropriately
int main() {
    try {
        doWork();
        return process_exit_code::SUCCESS;
    } catch (std::exception const& e) {
        std::cerr << e.what() << std::endl;
        return process_exit_code::UNCAUGHT_EXCEPTION;
    }
}
```

## See Also

- [shared_exceptions.h](./shared_exceptions.h.md) - Exception types used with these exit codes
- [printStackTrace.h](./printStackTrace.h.md) - Signal handlers that use these codes
- [External: Exit Status](https://www.gnu.org/software/bash/manual/html_node/Exit-Status.html) - Bash exit status conventions
