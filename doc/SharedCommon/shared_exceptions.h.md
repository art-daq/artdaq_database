# shared_exceptions.h

**Path:** `artdaq-database/SharedCommon/shared_exceptions.h`

**Purpose:** Defines the exception hierarchy for artdaq-database, built on top of the CET (Common Exception Type) framework from Fermilab's cetlib. These exceptions provide rich error context, stream-based message building, and consistent formatting across the project.


## Key Concepts

### CET Exception Framework

The exceptions inherit from `cet::exception`, which provides:
- **Stream operator (`<<`)**: Build error messages incrementally using familiar iostream syntax
- **Category string**: Identifies where the exception originated (typically the function name)
- **Formatted output**: Consistent error message format via `what()`
- **Exception nesting**: Chain exceptions for context preservation

### Exception Hierarchy

```
std::exception
    |
    +-- cet::exception
            |
            +-- artdaq::database::exception (base class)
                    |
                    +-- invalid_argument     (bad function parameters)
                    +-- runtime_error        (runtime failures)
                    +-- invalid_option_exception (bad CLI/configuration options)
                    +-- runtime_exception    (general runtime errors with message)
```

### When to Use Each Exception Type

| Exception Type | Use Case |
|----------------|----------|
| `invalid_argument` | Invalid function parameters, null pointers, out-of-range values |
| `runtime_error` | I/O errors, file not found, network failures, resource exhaustion |
| `invalid_option_exception` | Bad CLI arguments, invalid configuration options |
| `runtime_exception` | General runtime errors needing both category and initial message |

## Thread Safety

- **Thread-safe:** Yes
- **Concurrent access:** Exception objects are value types; each thread can throw and catch exceptions independently
- **Locking:** No internal locking required; exceptions are typically created on the stack

## Dependencies

| Include | Purpose |
|---------|---------|
| `<exception>` | Base `std::exception` class |
| `cetlib_except/coded_exception.h` | CET exception framework with category support and stream operators |

## Functions

### `debug::current_exception_diagnostic_information() -> std::string`

**Brief:** Returns diagnostic information about the current exception being handled within a catch block. Provides detailed exception type and message information useful for logging and debugging.

**Preconditions:**
- Must be called from within a catch block (active exception context)

**Returns:** A string containing the exception type name and message details for the currently active exception. Returns an empty or generic string if no exception is active.

**Postconditions:**
- The returned string contains human-readable diagnostic information

**Thread Safety:** Thread-safe (operates on thread-local exception state)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/shared_exceptions.h"
#include <iostream>

void riskyOperation() {
    throw artdaq::database::runtime_error("riskyOperation")
        << "Something went wrong";
}

int main() {
    try {
        riskyOperation();
    } catch (...) {
        std::string info = debug::current_exception_diagnostic_information();
        std::cerr << "Exception caught: " << info << std::endl;
    }
    return 0;
}
```

## Classes/Structures

### `exception`

**Brief:** Base class for all artdaq-database exceptions. Inherits from `cet::exception` to provide category-based error identification and stream-based message construction.

**Thread Safety:** Thread-safe (value type)

#### Constructors

##### `exception(Category const& category)`

**Brief:** Constructs an exception with only a category identifier. Use when the category alone sufficiently describes the error context.

**Parameters:**
- `category` - String identifying the source of the exception (typically the function or class name)

**Preconditions:**
- `category` should be non-empty and descriptive

**Postconditions:**
- Exception object is ready to be thrown or have additional context appended via `<<`

**Thread Safety:** Thread-safe

---

##### `exception(Category const& category, std::string const& message)`

**Brief:** Constructs an exception with both a category and initial message. Use when you have immediate context to provide.

**Parameters:**
- `category` - String identifying the source of the exception
- `message` - Initial error message describing the problem

**Preconditions:**
- `category` should be non-empty and descriptive

**Postconditions:**
- Exception object contains both category and initial message
- Additional context can still be appended via `<<`

**Thread Safety:** Thread-safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/shared_exceptions.h"
#include <iostream>

namespace db = artdaq::database;

void processDocument(std::string const& doc) {
    if (doc.empty()) {
        throw db::exception("processDocument")
            << "Document cannot be empty"
            << " (received " << doc.size() << " bytes)";
    }

    // Alternative with initial message
    if (doc.find("invalid") != std::string::npos) {
        throw db::exception("processDocument", "Invalid document content");
    }
}

int main() {
    try {
        processDocument("");
    } catch (db::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
```

---

### `invalid_argument`

**Brief:** Thrown when a function receives invalid parameters. Use this for input validation errors such as null pointers, empty strings when non-empty required, or out-of-range values.

**Thread Safety:** Thread-safe (value type)

#### Constructors

##### `invalid_argument(std::string const& category_)`

**Brief:** Constructs an invalid argument exception with a category. Additional context can be appended using the stream operator.

**Parameters:**
- `category_` - String identifying the source (typically the function name)

**Preconditions:**
- `category_` should be non-empty

**Postconditions:**
- Exception object is ready to be thrown or have context appended via `<<`

**Throws:** Nothing (constructor is noexcept)

**Thread Safety:** Thread-safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/shared_exceptions.h"
#include <iostream>

namespace db = artdaq::database;

void setCount(int count) {
    if (count < 0) {
        throw db::invalid_argument("setCount")
            << "Count must be non-negative, got: " << count;
    }
    if (count > 1000000) {
        throw db::invalid_argument("setCount")
            << "Count exceeds maximum (1000000), got: " << count;
    }
    // ... use count
}

int main() {
    try {
        setCount(-5);
    } catch (db::invalid_argument const& e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
```

---

### `runtime_error`

**Brief:** Thrown for runtime errors that could not be detected at compile time. Use for I/O errors, file not found, network failures, database connection errors, and resource exhaustion.

**Thread Safety:** Thread-safe (value type)

#### Constructors

##### `runtime_error(std::string const& category_)`

**Brief:** Constructs a runtime error exception with a category. Additional context can be appended using the stream operator.

**Parameters:**
- `category_` - String identifying the source (typically the function name)

**Preconditions:**
- `category_` should be non-empty

**Postconditions:**
- Exception object is ready to be thrown or have context appended via `<<`

**Throws:** Nothing (constructor is noexcept)

**Thread Safety:** Thread-safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/shared_exceptions.h"
#include <fstream>
#include <iostream>
#include <cerrno>
#include <cstring>

namespace db = artdaq::database;

std::string readFile(std::string const& path) {
    std::ifstream file(path);
    if (!file) {
        throw db::runtime_error("readFile")
            << "Failed to open file: " << path
            << " (error: " << std::strerror(errno) << ")";
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    return content;
}

int main() {
    try {
        auto content = readFile("/nonexistent/file.txt");
    } catch (db::runtime_error const& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
```

---

### `invalid_option_exception`

**Brief:** Thrown for invalid command-line or configuration options. Use for unrecognized CLI arguments, missing required options, and invalid option values.

**Thread Safety:** Thread-safe (value type)

#### Constructors

##### `invalid_option_exception(Category const& category)`

**Brief:** Constructs an invalid option exception with only a category.

**Parameters:**
- `category` - String identifying the source

**Preconditions:**
- `category` should be non-empty

**Postconditions:**
- Exception object is ready to be thrown or have context appended via `<<`

**Thread Safety:** Thread-safe

---

##### `invalid_option_exception(Category const& category, std::string const& message)`

**Brief:** Constructs an invalid option exception with a category and initial message.

**Parameters:**
- `category` - String identifying the source
- `message` - Initial error message

**Preconditions:**
- `category` should be non-empty

**Postconditions:**
- Exception object contains both category and initial message

**Thread Safety:** Thread-safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/shared_exceptions.h"
#include "artdaq-database/SharedCommon/process_exit_codes.h"
#include <iostream>
#include <string>
#include <cstring>

namespace db = artdaq::database;

void parseArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg.substr(0, 2) == "--") {
            std::string option = arg.substr(2);
            if (option != "help" && option != "verbose" && option != "config") {
                throw db::invalid_option_exception("parseArgs")
                    << "Unrecognized option: " << arg
                    << ". Valid options are: --help, --verbose, --config";
            }
        } else {
            throw db::invalid_option_exception("parseArgs", "Invalid argument format")
                << " Expected --option, got: " << arg;
        }
    }
}

int main(int argc, char** argv) {
    try {
        parseArgs(argc, argv);
        return process_exit_code::SUCCESS;
    } catch (db::invalid_option_exception const& e) {
        std::cerr << "Option error: " << e.what() << std::endl;
        return process_exit_code::INVALID_ARGUMENT;
    }
}
```

---

### `runtime_exception`

**Brief:** Similar to `runtime_error` but with both constructor variants. Provides flexibility for general runtime errors when you want to provide an initial message in the constructor.

**Thread Safety:** Thread-safe (value type)

#### Constructors

##### `runtime_exception(Category const& category)`

**Brief:** Constructs a runtime exception with only a category.

**Parameters:**
- `category` - String identifying the source

**Preconditions:**
- `category` should be non-empty

**Postconditions:**
- Exception object is ready to be thrown or have context appended via `<<`

**Thread Safety:** Thread-safe

---

##### `runtime_exception(Category const& category, std::string const& message)`

**Brief:** Constructs a runtime exception with a category and initial message.

**Parameters:**
- `category` - String identifying the source
- `message` - Initial error message

**Preconditions:**
- `category` should be non-empty

**Postconditions:**
- Exception object contains both category and initial message

**Thread Safety:** Thread-safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/shared_exceptions.h"
#include <iostream>

namespace db = artdaq::database;

bool establishConnection(std::string const& uri) {
    // Simulated connection attempt
    return false;
}

void connectToDatabase(std::string const& uri) {
    if (!establishConnection(uri)) {
        throw db::runtime_exception("connectToDatabase", "Connection failed")
            << " URI: " << uri
            << " Timeout: 30s";
    }
}

int main() {
    try {
        connectToDatabase("mongodb://localhost:27017/test_db");
    } catch (db::runtime_exception const& e) {
        std::cerr << "Database error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
```

## Relationship to Other Components

- **printStackTrace.h/cpp** - Provides stack traces when exceptions are thrown; `registerUngracefullExitHandlers()` can catch uncaught exceptions
- **returned_result.h** - `ThrowOnFailure()` throws `invalid_argument` when converting a failure result to an exception
- **process_exit_codes.h** - Defines exit codes used when exceptions are caught in main()
- Used throughout ConfigurationDB, StorageProviders, and Utilities for error reporting

## Example

```cpp
#include "artdaq-database/SharedCommon/shared_exceptions.h"
#include "artdaq-database/SharedCommon/process_exit_codes.h"
#include <iostream>
#include <fstream>

namespace db = artdaq::database;

// Function demonstrating exception usage
db::result_t loadConfiguration(std::string const& path) {
    if (path.empty()) {
        throw db::invalid_argument("loadConfiguration")
            << "Configuration path cannot be empty";
    }

    std::ifstream file(path);
    if (!file) {
        throw db::runtime_error("loadConfiguration")
            << "Failed to open configuration file: " << path;
    }

    // Process file...
    return db::Success("Configuration loaded");
}

int main(int argc, char** argv) {
    try {
        if (argc < 2) {
            throw db::invalid_option_exception("main")
                << "Usage: " << argv[0] << " <config_path>";
        }

        auto result = loadConfiguration(argv[1]);
        std::cout << "Result: " << result.second << std::endl;
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

    } catch (db::exception const& e) {
        std::cerr << "Database error: " << e.what() << std::endl;
        return process_exit_code::FAILURE;

    } catch (std::exception const& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return process_exit_code::UNCAUGHT_EXCEPTION;
    }
}
```

## Notes for Developers

### Category Convention

Use the function or method name as the category string for easy error tracing:

```cpp
void MyClass::process() {
    throw invalid_argument("MyClass::process")
        << "Error details here";
}

// For free functions
void processData() {
    throw runtime_error("processData")
        << "Processing failed";
}
```

### Catching Exceptions

Always catch by const reference to avoid slicing and unnecessary copies:

```cpp
try {
    operation();
} catch (db::invalid_argument const& e) {
    // Handle invalid arguments specifically
    std::cerr << "Invalid argument: " << e.what() << std::endl;
    return process_exit_code::INVALID_ARGUMENT;

} catch (db::runtime_error const& e) {
    // Handle runtime errors
    std::cerr << "Runtime error: " << e.what() << std::endl;
    return process_exit_code::FAILURE;

} catch (db::exception const& e) {
    // Handle any artdaq-database exception
    std::cerr << "Database error: " << e.what() << std::endl;
    return process_exit_code::FAILURE;

} catch (std::exception const& e) {
    // Handle any standard exception
    std::cerr << "Exception: " << e.what() << std::endl;
    return process_exit_code::UNCAUGHT_EXCEPTION;
}
```

### Including Context

Add relevant details to help debugging:

```cpp
throw runtime_error("database_write")
    << "Failed to write document"
    << " id=" << doc_id
    << " collection=" << collection
    << " reason=" << error_message;
```

### Common Pitfalls

- **Pitfall 1:** Catching by value instead of const reference causes object slicing:
  ```cpp
  // BAD: Slices the exception object
  catch (db::exception e) { ... }

  // GOOD: Preserves full exception information
  catch (db::exception const& e) { ... }
  ```

- **Pitfall 2:** Forgetting to include context makes debugging difficult:
  ```cpp
  // BAD: No context
  throw runtime_error("process");

  // GOOD: Includes useful context
  throw runtime_error("process")
      << "Failed at step " << step << " with input: " << input;
  ```

- **Pitfall 3:** Using generic category names makes error tracing harder:
  ```cpp
  // BAD: Generic category
  throw runtime_error("error");

  // GOOD: Specific function name
  throw runtime_error("MyClass::processDocument");
  ```

### result_t vs Exceptions

| Use `result_t` when... | Use exceptions when... |
|------------------------|------------------------|
| Failure is expected/common | Failure is rare/exceptional |
| Performance is critical | Error must propagate up many call levels |
| Caller should explicitly check | Following RAII patterns |
| In loops returning many results | Simpler error handling desired |

### Anti-patterns

```cpp
// DON'T catch and rethrow without adding context
try {
    operation();
} catch (db::exception& e) {
    throw;  // BAD: No added context
}

// DO add context when rethrowing
try {
    operation();
} catch (db::exception& e) {
    e << " while processing item " << item_id;
    throw;  // GOOD: Added context
}
```

## See Also

- [returned_result.h](./returned_result.h.md) - Result type that can convert to exceptions via `ThrowOnFailure()`
- [printStackTrace.h](./printStackTrace.h.md) - Stack trace support for exceptions
- [process_exit_codes.h](./process_exit_codes.h.md) - Exit codes to use when exceptions are caught
- [External: cetlib exceptions](https://cdcvs.fnal.gov/redmine/projects/cetlib/wiki) - CET exception framework documentation
