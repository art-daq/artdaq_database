# returned_result.cpp

**Path:** `artdaq-database/SharedCommon/returned_result.cpp`

**Implements:** [returned_result.h](./returned_result.h.md)

**Purpose:** Implements the result creation and validation functions declared in `returned_result.h`. These are simple factory functions that create `result_t` pairs and a utility to convert failures to exceptions, providing the bridge between result-based and exception-based error handling patterns used throughout the artdaq-database library.

## Implementation Overview

This file provides minimal, efficient implementations of the result factory functions. The implementation is intentionally simple, as these functions are called frequently throughout the codebase. Each function creates a `std::pair<bool, std::string>` with the appropriate success/failure flag and message using C++11 aggregate initialization.

## Key Algorithms

### Aggregate Initialization

The implementations use C++11 aggregate initialization:
```cpp
return {false, msg};  // Creates std::pair<bool, std::string>
```
This is more concise than `std::make_pair(false, msg)` and equally efficient.

### Result-to-Exception Bridge

`ThrowOnFailure` allows mixing result-based and exception-based error handling in the same codebase, enabling seamless integration between different error handling strategies.

## Function Implementations

### `Failure(std::string const& msg) -> result_t`

**Brief:** Creates a failure result with a message string, indicating an operation did not succeed.

**Implementation:**
```cpp
result_t db::Failure(std::string const& msg) {
    return {false, msg};
}
```

**Returns:** `result_t{false, msg}`

**Preconditions:**
- None

**Postconditions:**
- Returned `result_t` has `first == false`
- Returned `result_t` has `second == msg`

**Thread Safety:** Thread-safe (uses only local variables and const parameters)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/returned_result.h"
#include <iostream>

using namespace artdaq::database;

result_t validateConfiguration(std::string const& config) {
    if (config.empty()) {
        return Failure("Configuration cannot be empty");
    }
    // ... validation logic ...
    return Success();
}

int main() {
    auto result = validateConfiguration("");
    if (!result.first) {
        std::cerr << "Error: " << result.second << std::endl;
    }
    return 0;
}
```

---

### `Success(std::string const& msg) -> result_t`

**Brief:** Creates a success result with a message string, indicating an operation completed successfully.

**Implementation:**
```cpp
result_t db::Success(std::string const& msg) {
    return {true, msg};
}
```

**Returns:** `result_t{true, msg}`

**Preconditions:**
- None

**Postconditions:**
- Returned `result_t` has `first == true`
- Returned `result_t` has `second == msg`

**Thread Safety:** Thread-safe (uses only local variables and const parameters)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/returned_result.h"
#include <iostream>

using namespace artdaq::database;

result_t storeDocument(std::string const& doc) {
    // ... storage logic ...
    return Success("{\"message\":\"Document stored successfully\"}");
}

int main() {
    auto result = storeDocument("test data");
    if (result.first) {
        std::cout << "Success: " << result.second << std::endl;
    }
    return 0;
}
```

---

### `Failure(std::ostringstream const& oss) -> result_t`

**Brief:** Creates a failure result from a string stream by extracting its content. Useful for building complex error messages incrementally.

**Implementation:**
```cpp
result_t db::Failure(std::ostringstream const& oss) {
    return {false, oss.str()};
}
```

Calls `.str()` to extract the string from the stream.

**Returns:** `result_t{false, oss.str()}`

**Preconditions:**
- None

**Postconditions:**
- Returned `result_t` has `first == false`
- Returned `result_t` has `second == oss.str()`

**Thread Safety:** Thread-safe (uses only local variables and const parameters)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/returned_result.h"
#include <sstream>
#include <iostream>

using namespace artdaq::database;

result_t processItems(int count) {
    if (count < 0) {
        std::ostringstream oss;
        oss << "Invalid item count: " << count << " (must be non-negative)";
        return Failure(oss);
    }
    return Success();
}

int main() {
    auto result = processItems(-5);
    if (!result.first) {
        std::cerr << result.second << std::endl;
        // Output: "Invalid item count: -5 (must be non-negative)"
    }
    return 0;
}
```

---

### `Success(std::ostringstream const& oss) -> result_t`

**Brief:** Creates a success result from a string stream by extracting its content. Useful for building complex success messages incrementally.

**Implementation:**
```cpp
result_t db::Success(std::ostringstream const& oss) {
    return {true, oss.str()};
}
```

**Returns:** `result_t{true, oss.str()}`

**Preconditions:**
- None

**Postconditions:**
- Returned `result_t` has `first == true`
- Returned `result_t` has `second == oss.str()`

**Thread Safety:** Thread-safe (uses only local variables and const parameters)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/returned_result.h"
#include <sstream>
#include <vector>
#include <iostream>

using namespace artdaq::database;

result_t processMultipleFiles(std::vector<std::string> const& files) {
    // ... processing logic ...
    std::ostringstream oss;
    oss << "Processed " << files.size() << " files successfully";
    return Success(oss);
}

int main() {
    std::vector<std::string> files = {"file1.txt", "file2.txt"};
    auto result = processMultipleFiles(files);
    if (result.first) {
        std::cout << result.second << std::endl;
        // Output: "Processed 2 files successfully"
    }
    return 0;
}
```

---

### `ThrowOnFailure(result_t const& result) -> void`

**Brief:** Converts a failure result to an exception, bridging result-based and exception-based error handling. If the result indicates success, this function does nothing.

**Implementation:**
```cpp
void db::ThrowOnFailure(result_t const& result) {
    if (result.first) {
        return;
    }
    throw invalid_argument(result.second);
}
```

**Steps:**
1. Check if the result indicates success (`result.first == true`)
2. If successful, return immediately (no-op)
3. If failed, throw an `invalid_argument` exception with the error message

**Preconditions:**
- None

**Postconditions:**
- If the function returns normally, the result indicated success (`result.first == true`)
- If `result.first == false`, the function throws and never returns

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | When `result.first` is `false`; exception message is `result.second` |

**Thread Safety:** Thread-safe (uses only local variables and const parameters)

**Called by:** Any code that needs to enforce success, particularly in initialization or critical operations where failure cannot be recovered from.

**Example:**
```cpp
#include "artdaq-database/SharedCommon/returned_result.h"
#include "artdaq-database/SharedCommon/shared_exceptions.h"
#include "artdaq-database/SharedCommon/process_exit_codes.h"
#include <iostream>

using namespace artdaq::database;

result_t connectToDatabase() {
    // ... connection logic ...
    return Success("Connected to database");
}

void initializeDatabase() {
    result_t result = connectToDatabase();

    try {
        ThrowOnFailure(result);  // Throws if connection failed
        std::cout << "Database connected successfully\n";
    } catch (invalid_argument const& e) {
        std::cerr << "Failed to connect: " << e.what() << "\n";
        throw;  // Re-throw or handle as appropriate
    }
}

int main() {
    try {
        initializeDatabase();
        return process_exit_code::SUCCESS;
    } catch (invalid_argument const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return process_exit_code::FAILURE;
    }
}
```

## Performance Considerations

- **Success/Failure creation:** ~10 ns (pair construction + string copy)
- **ThrowOnFailure on success:** ~5 ns (bool check only)
- **ThrowOnFailure on failure:** ~1000-10000 ns (exception throw overhead)
- All factory functions use aggregate initialization which is efficient
- The `ostringstream` overloads call `.str()` which may allocate a new string

## Error Handling Strategy

The file uses a simple conditional check in `ThrowOnFailure` to determine whether to throw. The exception type used is `invalid_argument` from `shared_exceptions.h`, which is the library's standard exception for invalid inputs or failed operations.

## Thread Safety

All functions are thread-safe - they use only local variables and const parameters. There is no shared state or global variables accessed by these functions.

## Dependencies

| Include | Purpose |
|---------|---------|
| `returned_result.h` | Function declarations and `result_t` type |
| `shared_exceptions.h` | `invalid_argument` exception type |

## TRACE Configuration

The file defines `TRACE_NAME` as `"returned_result.cpp"` for use with the TRACE logging system, though no TRACE calls are currently made in this implementation. These functions are called very frequently, and adding TRACE logging would create excessive noise. Errors are communicated through the result message instead.

## Relationship to Other Components

- **returned_result.h** - Declares the functions implemented here
- **shared_exceptions.h** - Provides `invalid_argument` thrown by `ThrowOnFailure`
- **ConfigurationDB** - Uses these functions extensively for operation results
- **StorageProviders** - Uses these functions for database operation results
- **Utilities** - Uses these functions in CLI tools

## Testing Notes

- **Unit tests:** See tests that exercise configuration operations
- **Key test cases:**
  - Success creation with default and custom messages
  - Failure creation with default and custom messages
  - Success creation from ostringstream
  - Failure creation from ostringstream
  - ThrowOnFailure with success result (should not throw)
  - ThrowOnFailure with failure result (should throw `invalid_argument`)
  - Exception message contains the original failure message

## Example

```cpp
#include "artdaq-database/SharedCommon/returned_result.h"
#include "artdaq-database/SharedCommon/shared_exceptions.h"
#include "artdaq-database/SharedCommon/process_exit_codes.h"
#include <iostream>
#include <sstream>

namespace db = artdaq::database;

// Multi-step operation demonstrating result chaining
db::result_t step1() {
    return db::Success("Step 1 complete");
}

db::result_t step2() {
    // Simulate a failure
    return db::Failure("Step 2 failed: resource unavailable");
}

db::result_t step3() {
    return db::Success("Step 3 complete");
}

db::result_t performOperation() {
    auto result1 = step1();
    if (!result1.first) return result1;

    auto result2 = step2();
    if (!result2.first) return result2;

    auto result3 = step3();
    if (!result3.first) return result3;

    return db::Success("All steps completed");
}

int main() {
    try {
        // Direct result checking
        auto result = performOperation();
        if (!result.first) {
            std::cerr << "Operation failed: " << result.second << std::endl;
            return process_exit_code::FAILURE;
        }

        // Or use ThrowOnFailure for cleaner code
        auto result2 = performOperation();
        db::ThrowOnFailure(result2);

        std::cout << "Operation succeeded" << std::endl;
        return process_exit_code::SUCCESS;

    } catch (db::invalid_argument const& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return process_exit_code::FAILURE;
    }
}
```

## See Also

- [returned_result.h](./returned_result.h.md) - Declares these functions and the `result_t` type
- [shared_exceptions.h](./shared_exceptions.h.md) - Provides `invalid_argument` thrown by `ThrowOnFailure`
