# returned_result.h

**Path:** `artdaq-database/SharedCommon/returned_result.h`

**Purpose:** Defines a simple result type (`result_t`) and utility functions for returning success/failure status with descriptive messages. This provides a lightweight alternative to exceptions for operations where failure is expected or common, such as file I/O or database queries.


## Key Concepts

### result_t Pattern

Instead of throwing exceptions for expected failures, functions return a `result_t` pair:
- **first** (bool): Did the operation succeed?
- **second** (string): Success message or error description

This pattern is:
- **Faster than exceptions**: No stack unwinding overhead
- **Explicit**: Callers must check the result
- **Informative**: Always provides context via the message

### When to Use result_t vs Exceptions

| Use `result_t` when... | Use exceptions when... |
|------------------------|------------------------|
| Failure is expected/common | Failure is rare/exceptional |
| Performance is critical | Error must propagate up many call levels |
| Caller should explicitly check | Following RAII patterns |
| In loops returning many results | Simpler error handling desired |
| Database queries that may not find results | Invalid arguments that indicate bugs |

### Bridging result_t and Exceptions

The `ThrowOnFailure()` function converts a failure result to an exception, allowing you to:
- Use `result_t` internally for performance
- Convert to exceptions at API boundaries when needed

## Thread Safety

- **Thread-safe:** Yes
- **Concurrent access:** The `result_t` type is a simple value type (`std::pair`) with no shared state
- **Locking:** Factory functions `Success()` and `Failure()` are stateless and thread-safe

## Dependencies

| Include | Purpose |
|---------|---------|
| `<sstream>` | `std::ostringstream` for building messages |
| `<string>` | `std::string` for message storage |
| `<utility>` | `std::pair` for the `result_t` type |

## Type Aliases

### `result_t`

```cpp
using result_t = std::pair<bool, std::string>;
```

**Brief:** A pair where `first` indicates success (true) or failure (false), and `second` contains a descriptive message.

**Thread Safety:** Thread-safe (value type with no shared state)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/returned_result.h"
#include "artdaq-database/SharedCommon/shared_exceptions.h"
#include <iostream>

namespace db = artdaq::database;

db::result_t validateInput(std::string const& input) {
    if (input.empty()) {
        return db::Failure("Input cannot be empty");
    }
    if (input.size() > 1000) {
        return db::Failure("Input exceeds maximum length of 1000 characters");
    }
    return db::Success("Input is valid");
}

int main() {
    try {
        // Traditional checking
        db::result_t result = validateInput("test data");
        if (result.first) {
            std::cout << "Success: " << result.second << std::endl;
        } else {
            std::cerr << "Failed: " << result.second << std::endl;
        }

        // C++17 structured bindings
        auto [success, message] = validateInput("");
        if (!success) {
            std::cerr << "Validation failed: " << message << std::endl;
        }

        return 0;
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

## Constants

### Namespace: `artdaq::database::result`

**Brief:** Predefined JSON-formatted messages for common results. These provide consistent messaging across the API.

| Constant | Value | Purpose |
|----------|-------|---------|
| `msg_Success` | `"{\"message\":\"Success\"}"` | Generic success result |
| `msg_Failure` | `"{\"message\":\"Failure\"}"` | Generic failure result |
| `msg_Added` | `"{\"message\":\"Added\"}"` | Item was created/added |
| `msg_Updated` | `"{\"message\":\"Updated\"}"` | Item was modified/updated |
| `msg_Removed` | `"{\"message\":\"Removed\"}"` | Item was deleted/removed |
| `msg_Missing` | `"{\"message\":\"Missing\"}"` | Item was not found |
| `msg_AlreadyExist` | `"{\"message\":\"AlreadyExist\"}"` | Duplicate item exists |
| `msg_Ignored` | `"{\"message\":\"Ignored\"}"` | Operation was skipped |

## Functions

### `Success(std::string const& msg) -> result_t`

**Brief:** Creates a success result with a custom message. Use this to indicate an operation completed successfully.

**Parameters:**
- `msg` - Success description (default: `result::msg_Success`)

**Preconditions:**
- None

**Returns:** `result_t{true, msg}` - A pair with `first=true` and `second` containing the message

**Postconditions:**
- Returned `result_t` has `first == true`

**Thread Safety:** Thread-safe (stateless function)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/returned_result.h"
#include <iostream>

namespace db = artdaq::database;

// With custom message
db::result_t writeFile(std::string const& data) {
    // ... write operation ...
    return db::Success("File written successfully");
}

// With default message
db::result_t simpleOperation() {
    return db::Success();  // Uses msg_Success
}

// With predefined message constant
db::result_t addItem(std::string const& item) {
    // ... add logic ...
    return db::Success(db::result::msg_Added);
}

int main() {
    auto result = writeFile("data");
    if (result.first) {
        std::cout << result.second << std::endl;
    }
    return 0;
}
```

---

### `Success(std::ostringstream const& oss) -> result_t`

**Brief:** Creates a success result from an ostringstream. Useful for building messages with formatted data.

**Parameters:**
- `oss` - String stream containing the success message

**Preconditions:**
- None

**Returns:** `result_t{true, oss.str()}` - Extracts the string from the stream

**Postconditions:**
- Returned `result_t` has `first == true`

**Thread Safety:** Thread-safe (stateless function)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/returned_result.h"
#include <sstream>
#include <iostream>

namespace db = artdaq::database;

db::result_t importData(int count, double duration) {
    std::ostringstream oss;
    oss << "Imported " << count << " items in " << duration << " seconds";
    return db::Success(oss);
}

int main() {
    auto result = importData(100, 2.5);
    std::cout << result.second << std::endl;  // "Imported 100 items in 2.5 seconds"
    return 0;
}
```

---

### `Failure(std::string const& msg) -> result_t`

**Brief:** Creates a failure result with a custom error message. Use this to indicate an operation failed.

**Parameters:**
- `msg` - Error description (default: `result::msg_Failure`)

**Preconditions:**
- None

**Returns:** `result_t{false, msg}` - A pair with `first=false` and `second` containing the error message

**Postconditions:**
- Returned `result_t` has `first == false`

**Thread Safety:** Thread-safe (stateless function)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/returned_result.h"
#include "artdaq-database/SharedCommon/shared_exceptions.h"
#include <iostream>

namespace db = artdaq::database;

db::result_t validateInput(std::string const& input) {
    if (input.empty()) {
        return db::Failure("Input cannot be empty");
    }
    if (input.find('\0') != std::string::npos) {
        return db::Failure("Input contains null characters");
    }
    return db::Success();
}

// Using predefined message
db::result_t findDocument(std::string const& id) {
    bool found = false;
    // ... search logic ...
    if (!found) {
        return db::Failure(db::result::msg_Missing);
    }
    return db::Success();
}

int main() {
    auto result = validateInput("");
    if (!result.first) {
        std::cerr << "Validation error: " << result.second << std::endl;
    }
    return 0;
}
```

---

### `Failure(std::ostringstream const& oss) -> result_t`

**Brief:** Creates a failure result from an ostringstream. Useful for building detailed error messages with context.

**Parameters:**
- `oss` - String stream containing the error message

**Preconditions:**
- None

**Returns:** `result_t{false, oss.str()}` - Extracts the string from the stream

**Postconditions:**
- Returned `result_t` has `first == false`

**Thread Safety:** Thread-safe (stateless function)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/returned_result.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <cerrno>
#include <cstring>

namespace db = artdaq::database;

db::result_t openFile(std::string const& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::ostringstream oss;
        oss << "Failed to open file '" << filename << "'"
            << ": " << std::strerror(errno)
            << " (errno=" << errno << ")";
        return db::Failure(oss);
    }
    return db::Success("File opened successfully");
}

int main() {
    auto result = openFile("/nonexistent/file.txt");
    if (!result.first) {
        std::cerr << result.second << std::endl;
    }
    return 0;
}
```

---

### `ThrowOnFailure(result_t const& result) -> void`

**Brief:** Converts a failure result to an exception. Provides a bridge between result-based and exception-based error handling.

**Parameters:**
- `result` - Result to check

**Preconditions:**
- None

**Returns:** Nothing (void)

**Postconditions:**
- If `result.first == true`, function returns normally
- If `result.first == false`, function throws (never returns)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When `result.first == false`; exception message is `result.second` |

**Thread Safety:** Thread-safe (stateless function)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/returned_result.h"
#include "artdaq-database/SharedCommon/shared_exceptions.h"
#include "artdaq-database/SharedCommon/process_exit_codes.h"
#include <iostream>

namespace db = artdaq::database;

db::result_t initializeResource() {
    // ... initialization logic ...
    return db::Success("Resource initialized");
}

void setupApplication() {
    // Convert result to exception if failed
    db::result_t result = initializeResource();
    db::ThrowOnFailure(result);  // Throws invalid_argument if result.first is false

    // This code only runs if initialization succeeded
    std::cout << "Application ready" << std::endl;
}

int main() {
    try {
        setupApplication();
        return process_exit_code::SUCCESS;
    } catch (db::invalid_argument const& e) {
        std::cerr << "Initialization failed: " << e.what() << std::endl;
        return process_exit_code::FAILURE;
    }
}
```

## Relationship to Other Components

- **returned_result.cpp** - Implementation file containing the factory function definitions
- **shared_exceptions.h** - Defines `invalid_argument` exception thrown by `ThrowOnFailure()`
- **configuraion_api_literals.h** - Contains additional result message constants
- Used throughout ConfigurationDB and StorageProviders for operation results
- Result messages use JSON format for compatibility with the REST-like API

## Example

```cpp
#include "artdaq-database/SharedCommon/returned_result.h"
#include "artdaq-database/SharedCommon/shared_exceptions.h"
#include "artdaq-database/SharedCommon/process_exit_codes.h"
#include <iostream>
#include <fstream>

namespace db = artdaq::database;

// Function returning result_t
db::result_t processDocument(std::string const& doc) {
    // Validate input
    if (doc.empty()) {
        return db::Failure("Document is empty");
    }

    if (doc.size() > 1000000) {
        std::ostringstream oss;
        oss << "Document too large: " << doc.size() << " bytes (max 1MB)";
        return db::Failure(oss);
    }

    // Process the document...
    return db::Success("Document processed successfully");
}

// Multi-step operation with early return on failure
db::result_t performStep1() { return db::Success("Step 1 complete"); }
db::result_t performStep2() { return db::Success("Step 2 complete"); }
db::result_t performStep3() { return db::Success("Step 3 complete"); }

db::result_t multiStepOperation() {
    auto step1 = performStep1();
    if (!step1.first) return step1;  // Propagate failure

    auto step2 = performStep2();
    if (!step2.first) return step2;

    auto step3 = performStep3();
    if (!step3.first) return step3;

    return db::Success("All steps completed successfully");
}

// Using structured bindings (C++17)
void processWithStructuredBindings() {
    auto [success, message] = processDocument("test data");
    if (success) {
        std::cout << "Result: " << message << std::endl;
    } else {
        std::cerr << "Error: " << message << std::endl;
    }
}

// Converting to exceptions at API boundary
void publicApiFunction() {
    auto result = multiStepOperation();
    db::ThrowOnFailure(result);  // Throws if any step failed
}

int main() {
    try {
        // Direct result checking
        auto result = processDocument("test data");
        if (result.first) {
            std::cout << "Success: " << result.second << std::endl;
        } else {
            std::cerr << "Error: " << result.second << std::endl;
            return process_exit_code::FAILURE;
        }

        // Using ThrowOnFailure for cleaner code
        publicApiFunction();
        std::cout << "Operation completed" << std::endl;
        return process_exit_code::SUCCESS;

    } catch (db::invalid_argument const& e) {
        std::cerr << "Operation failed: " << e.what() << std::endl;
        return process_exit_code::FAILURE;
    }
}
```

## Notes for Developers

### Common Usage Pattern

```cpp
// Multi-step operation with early return on failure
db::result_t complexOperation() {
    auto result1 = step1();
    if (!result1.first) return result1;  // Propagate failure immediately

    auto result2 = step2();
    if (!result2.first) return result2;

    auto result3 = step3();
    if (!result3.first) return result3;

    return db::Success("All steps completed");
}
```

### Provide Detailed Error Messages

```cpp
// GOOD - includes context for debugging
return db::Failure("Failed to open '/path/to/file': Permission denied (errno=13)");

// BAD - no useful information for debugging
return db::Failure("Error");
```

### Using Predefined Messages for Standard Outcomes

```cpp
// For standard outcomes, use predefined message constants
if (itemExists) {
    return db::Failure(db::result::msg_AlreadyExist);
}
return db::Success(db::result::msg_Added);
```

### Performance Comparison

`result_t` is approximately 10-100x faster than throwing exceptions for expected failure cases because:
- No stack unwinding
- No RTTI overhead
- Simple value copy

This makes `result_t` ideal for:
- File I/O operations that may fail
- Database queries that may not find results
- Validation functions with frequent failures
- Operations in tight loops

### Converting Between result_t and Exceptions

```cpp
// result_t to exception (when failure is unexpected at call site)
db::ThrowOnFailure(result);

// Exception to result_t (when wrapping exception-throwing code)
db::result_t safeWrapper() {
    try {
        riskyOperation();
        return db::Success();
    } catch (std::exception const& e) {
        return db::Failure(e.what());
    }
}
```

### Common Pitfalls

- **Pitfall 1:** Ignoring the result - always check `first`:
  ```cpp
  // BAD - ignoring the result
  processDocument(doc);  // Result discarded!

  // GOOD - check the result
  auto result = processDocument(doc);
  if (!result.first) {
      // Handle error
  }
  ```

- **Pitfall 2:** Using exceptions for expected failures wastes performance:
  ```cpp
  // BAD - exceptions are slow for common cases
  for (auto const& file : files) {
      try {
          process(file);  // Throws on every invalid file
      } catch (...) { }
  }

  // GOOD - result_t is fast for expected failures
  for (auto const& file : files) {
      auto result = process(file);
      if (!result.first) continue;  // Fast path for invalid files
  }
  ```

- **Pitfall 3:** Not providing context in error messages:
  ```cpp
  // BAD - no context
  return db::Failure("Failed");

  // GOOD - includes context
  return db::Failure("Failed to read document ID=" + doc_id);
  ```

### Anti-patterns

```cpp
// DON'T throw exceptions for expected failures
void badProcess(std::string const& input) {
    if (input.empty()) {
        throw std::invalid_argument("empty input");  // Slow!
    }
}

// DO use result_t for expected failures
db::result_t goodProcess(std::string const& input) {
    if (input.empty()) {
        return db::Failure("Input cannot be empty");  // Fast!
    }
    return db::Success();
}

// DON'T discard results
void badUsage() {
    validateInput(data);  // Result ignored!
}

// DO check results
void goodUsage() {
    auto result = validateInput(data);
    if (!result.first) {
        handleError(result.second);
    }
}
```

## See Also

- [shared_exceptions.h](./shared_exceptions.h.md) - Exception types used with `ThrowOnFailure()`
- [configuraion_api_literals.h](./configuraion_api_literals.h.md) - Additional result message constants
- [returned_result.cpp](./returned_result.cpp.md) - Implementation file
