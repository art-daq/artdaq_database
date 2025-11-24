# returned_result.h

## File Overview

This header file defines a simple but effective result type and utility functions for returning success/failure status along with descriptive messages. It provides a standardized way to communicate operation results throughout the artdaq-database project without using exceptions for expected failure cases.

**Location**: `/home/user/artdaq-database/artdaq-database/SharedCommon/returned_result.h`

## Dependencies

### Standard Library
- `<sstream>` - String stream for building messages
- `<string>` - String class
- `<utility>` - std::pair

## Namespace: artdaq::database

All types and functions are in the `artdaq::database` namespace.

## Type Definitions

### result_t

```cpp
using result_t = std::pair<bool, std::string>;
```

**Purpose**: A pair representing an operation result.

**Structure**:
- **first** (bool): Success flag
  - `true` = operation succeeded
  - `false` = operation failed
- **second** (std::string): Result message or error description

**Design Rationale**:
- Simple, lightweight alternative to exception handling
- Allows functions to return detailed error information
- Compatible with std::tie for easy unpacking
- Can be used in contexts where exceptions are not appropriate

**Usage Example**:
```cpp
result_t result = some_operation();
if (result.first) {
    std::cout << "Success: " << result.second << std::endl;
} else {
    std::cerr << "Failure: " << result.second << std::endl;
}
```

---

## Namespace: artdaq::database::result

Contains predefined result message constants.

### Message Constants

```cpp
constexpr auto msg_Missing = "{\"message\":\"Missing\"}";
constexpr auto msg_Ignored = "{\"message\":\"Ignored\"}";
constexpr auto msg_Updated = "{\"message\":\"Updated\"}";
constexpr auto msg_Added = "{\"message\":\"Added\"}";
constexpr auto msg_Removed = "{\"message\":\"Removed\"}";
constexpr auto msg_Success = "{\"message\":\"Success\"}";
constexpr auto msg_Failure = "{\"message\":\"Failure\"}";
constexpr auto msg_AlreadyExist = "{\"message\":\"AlreadyExist\"}";
```

**Purpose**: Standardized JSON-formatted result messages.

**Format**: All messages are JSON objects with a "message" field.

#### Message Meanings

| Constant | Value | Use Case |
|----------|-------|----------|
| `msg_Missing` | `"{\"message\":\"Missing\"}"` | Requested item not found |
| `msg_Ignored` | `"{\"message\":\"Ignored\"}"` | Operation skipped/ignored |
| `msg_Updated` | `"{\"message\":\"Updated\"}"` | Item successfully updated |
| `msg_Added` | `"{\"message\":\"Added\"}"` | Item successfully added |
| `msg_Removed` | `"{\"message\":\"Removed\"}"` | Item successfully removed |
| `msg_Success` | `"{\"message\":\"Success\"}"` | Generic success |
| `msg_Failure` | `"{\"message\":\"Failure\"}"` | Generic failure |
| `msg_AlreadyExist` | `"{\"message\":\"AlreadyExist\"}"` | Item already exists |

**Design Decision**: JSON format allows these messages to be:
- Easily parsed by clients
- Extended with additional fields
- Consistent with API response format

---

## Functions

### Failure (string overload)

```cpp
result_t Failure(std::string const& message = result::msg_Failure);
```

**Purpose**: Create a failure result with a message.

**Parameters**:
- `message` - Error description (default: generic failure message)

**Returns**: result_t with first=false and second=message

**Usage Example**:
```cpp
result_t validate_input(std::string const& input) {
    if (input.empty()) {
        return Failure("Input cannot be empty");
    }
    return Success();
}
```

---

### Success (string overload)

```cpp
result_t Success(std::string const& message = result::msg_Success);
```

**Purpose**: Create a success result with a message.

**Parameters**:
- `message` - Success description (default: generic success message)

**Returns**: result_t with first=true and second=message

**Usage Example**:
```cpp
result_t write_file(std::string const& data) {
    // ... write operation ...
    return Success("File written successfully");
}
```

---

### Failure (ostringstream overload)

```cpp
result_t Failure(std::ostringstream const& oss);
```

**Purpose**: Create a failure result from a string stream.

**Parameters**:
- `oss` - Ostringstream containing error message

**Returns**: result_t with first=false and second=oss.str()

**Usage Example**:
```cpp
result_t process_data(int value) {
    if (value < 0) {
        std::ostringstream oss;
        oss << "Invalid value: " << value << ", expected >= 0";
        return Failure(oss);
    }
    return Success();
}
```

**Rationale**: Allows building complex error messages efficiently.

---

### Success (ostringstream overload)

```cpp
result_t Success(std::ostringstream const& oss);
```

**Purpose**: Create a success result from a string stream.

**Parameters**:
- `oss` - Ostringstream containing success message

**Returns**: result_t with first=true and second=oss.str()

**Usage Example**:
```cpp
result_t import_data(std::vector<std::string> const& items) {
    // ... import operation ...
    std::ostringstream oss;
    oss << "Imported " << items.size() << " items successfully";
    return Success(oss);
}
```

---

### ThrowOnFailure

```cpp
void ThrowOnFailure(result_t const& result);
```

**Purpose**: Convert a failure result to an exception.

**Parameters**:
- `result` - Result to check

**Behavior**:
- If result.first is true: returns (no-op)
- If result.first is false: throws invalid_argument with result.second as message

**Usage Example**:
```cpp
result_t result = risky_operation();
ThrowOnFailure(result);  // Throws if operation failed
// Continues here only if operation succeeded
```

**Use Case**: Bridge between result-based and exception-based error handling.

---

## Usage Patterns

### Basic Success/Failure Pattern

```cpp
result_t do_operation() {
    if (/* error condition */) {
        return Failure("Operation failed: reason");
    }

    // ... perform work ...

    return Success("Operation completed");
}

// Caller:
auto result = do_operation();
if (!result.first) {
    // Handle error
    log_error(result.second);
    return;
}
// Continue on success
```

### Chaining Operations

```cpp
result_t multi_step_operation() {
    auto step1 = perform_step1();
    if (!step1.first) return step1;  // Propagate failure

    auto step2 = perform_step2();
    if (!step2.first) return step2;  // Propagate failure

    return Success("All steps completed");
}
```

### Converting to Exception

```cpp
void must_succeed_operation() {
    auto result = operation_that_might_fail();
    ThrowOnFailure(result);  // Converts failure to exception
    // Code here only runs if operation succeeded
}
```

### Structured Binding (C++17)

```cpp
auto [success, message] = do_operation();
if (success) {
    std::cout << message << std::endl;
} else {
    std::cerr << message << std::endl;
}
```

### Using Predefined Messages

```cpp
result_t add_item(std::string const& item) {
    if (item_exists(item)) {
        return Failure(result::msg_AlreadyExist);
    }

    insert_item(item);
    return Success(result::msg_Added);
}
```

### Building Complex Messages

```cpp
result_t validate_config(Config const& cfg) {
    std::vector<std::string> errors;

    if (!cfg.valid_field1) errors.push_back("field1 invalid");
    if (!cfg.valid_field2) errors.push_back("field2 invalid");

    if (!errors.empty()) {
        std::ostringstream oss;
        oss << "Validation failed: ";
        for (size_t i = 0; i < errors.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << errors[i];
        }
        return Failure(oss);
    }

    return Success();
}
```

## Design Decisions

### Why std::pair Instead of Custom Type?

**Advantages**:
- No additional type definition needed
- Works with standard library algorithms
- Supports structured bindings in C++17
- Zero overhead (std::pair is a simple struct)

**Disadvantages**:
- Less self-documenting than named fields
- .first and .second less clear than .success and .message

**Decision**: Simplicity and standard library compatibility outweigh the naming clarity issues.

### Why Provide Both String and Ostringstream Overloads?

- **String overload**: For simple, static messages
- **Ostringstream overload**: For dynamically built messages with formatting

This eliminates the need to call .str() explicitly, reducing boilerplate.

### Why JSON-formatted Messages?

- Consistency with API responses
- Easy to parse programmatically
- Extensible (can add more fields without breaking parsing)
- Human-readable

### When to Use result_t vs Exceptions?

**Use result_t when**:
- Failure is expected/common
- Performance is critical (no exception overhead)
- Need to return multiple results in a loop
- Caller should explicitly check result

**Use exceptions when**:
- Failure is exceptional/rare
- Error should propagate up multiple levels
- Simpler error handling logic
- Following RAII patterns

## Performance Considerations

1. **No Exception Overhead**: Using result_t avoids the cost of exception throwing and stack unwinding
2. **String Copy**: The string message is copied when creating result_t
3. **Return Value Optimization**: Modern compilers optimize away the pair copy

**Benchmark** (approximate):
- Exception throw/catch: ~1000-10000 ns
- result_t return: ~10-100 ns

For operations that fail frequently, result_t provides 10-100x better performance.

## Comparison with Other Approaches

### vs std::optional
```cpp
std::optional<Data> get_data();  // Can't include error message
result_t get_data();             // Includes error message
```

### vs std::expected (C++23)
```cpp
std::expected<Data, Error> get_data();  // Similar, but not available in C++17
result_t get_data();                     // Available now, simpler
```

### vs Error Codes
```cpp
int get_data(Data* out);  // C-style, error-prone
result_t get_data();      // Modern C++, safer
```

## Related Files

- **returned_result.cpp** - Implementation of these functions
- **shared_exceptions.h** - Exception types used by ThrowOnFailure
- All API operation functions use result_t for return values

## Best Practices

1. **Check Results**: Always check result.first before using result.second
   ```cpp
   auto result = operation();
   if (result.first) {
       // Use result.second for success info
   } else {
       // Use result.second for error info
   }
   ```

2. **Provide Detailed Messages**: Include context in failure messages
   ```cpp
   // Good
   return Failure("Failed to open file '/path/to/file': Permission denied");

   // Bad
   return Failure("Error");
   ```

3. **Use Predefined Messages**: For standard results, use the constants
   ```cpp
   return Success(result::msg_Added);  // Clear intent
   ```

4. **Early Return**: Fail fast and propagate failures
   ```cpp
   auto r1 = step1();
   if (!r1.first) return r1;

   auto r2 = step2();
   if (!r2.first) return r2;
   ```

5. **Convert to Exceptions When Appropriate**: Use ThrowOnFailure for must-succeed operations
   ```cpp
   ThrowOnFailure(initialize_critical_resource());
   ```

## Notes

- The JSON message format is consistent with the broader artdaq-database API design
- result_t is particularly useful in filesystem and database operations where failures are common
- Consider adding a custom result type with named fields if .first/.second become too confusing
- C++23's std::expected provides similar functionality with better ergonomics
