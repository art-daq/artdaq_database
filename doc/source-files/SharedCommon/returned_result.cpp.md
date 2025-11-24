# returned_result.cpp

## File Overview

This implementation file provides the concrete implementations of the result creation and validation functions declared in `returned_result.h`. The implementations are straightforward, focusing on creating properly formatted result_t pairs and converting failures to exceptions when needed.

**Location**: `/home/user/artdaq-database/artdaq-database/SharedCommon/returned_result.cpp`

## Dependencies

### Project Headers
- `"artdaq-database/SharedCommon/returned_result.h"` - Function declarations and type definitions
- `"artdaq-database/SharedCommon/shared_exceptions.h"` - Exception types (invalid_argument)

## TRACE Configuration

```cpp
#define TRACE_NAME "returned_result.cpp"
```

Sets the TRACE logging identifier for this compilation unit.

## Namespace Aliases

```cpp
namespace db = artdaq::database;
using artdaq::database::result_t;
```

Simplifies type names within the implementation.

## Function Implementations

### Failure (string overload)

```cpp
result_t db::Failure(std::string const& msg) {
    return {false, msg};
}
```

**Implementation Details**:
- Uses aggregate initialization of std::pair
- Returns pair with first=false (failure flag)
- Second element contains the error message

**Efficiency**: Direct initialization, no intermediate objects.

**Example Usage**:
```cpp
if (file_not_found) {
    return Failure("File not found: " + filename);
}
```

---

### Success (string overload)

```cpp
result_t db::Success(std::string const& msg) {
    return {true, msg};
}
```

**Implementation Details**:
- Uses aggregate initialization of std::pair
- Returns pair with first=true (success flag)
- Second element contains the success message

**Efficiency**: Direct initialization, no intermediate objects.

**Example Usage**:
```cpp
return Success("Configuration loaded successfully");
```

---

### Failure (ostringstream overload)

```cpp
result_t db::Failure(std::ostringstream const& oss) {
    return {false, oss.str()};
}
```

**Implementation Details**:
- Calls .str() on the ostringstream to extract the string
- Creates result_t pair with failure flag
- Allows building complex error messages

**String Extraction**: The .str() method creates a copy of the stream's contents.

**Example Usage**:
```cpp
std::ostringstream oss;
oss << "Failed to process " << count << " items. ";
oss << "Expected range: [" << min << ", " << max << "]";
return Failure(oss);
```

**Performance Note**: The string copy from oss.str() is unavoidable but typically not a concern for error paths.

---

### Success (ostringstream overload)

```cpp
result_t db::Success(std::ostringstream const& oss) {
    return {true, oss.str()};
}
```

**Implementation Details**:
- Calls .str() on the ostringstream to extract the string
- Creates result_t pair with success flag
- Useful for detailed success messages with statistics

**Example Usage**:
```cpp
std::ostringstream oss;
oss << "Successfully processed " << processed << " items. ";
oss << "Skipped: " << skipped << ", Failed: " << failed;
return Success(oss);
```

---

### ThrowOnFailure

```cpp
void db::ThrowOnFailure(result_t const& result) {
    if (result.first) {
        return;
    }

    throw invalid_argument(result.second);
}
```

**Implementation Details**:

1. **Check Success**: If result.first is true, immediately return
2. **Throw on Failure**: If false, throw invalid_argument exception
3. **Preserve Message**: Exception message is the result's second element

**Exception Type**: Uses `artdaq::database::invalid_argument` from shared_exceptions.h

**Control Flow**:
```
result.first == true  → return (no exception)
result.first == false → throw invalid_argument(result.second)
```

**Usage Example**:

```cpp
// Must-succeed operation
void critical_initialization() {
    auto result = load_configuration();
    ThrowOnFailure(result);  // Throws if load failed

    // Only reached if load succeeded
    apply_configuration();
}

// Caller can catch the exception
try {
    critical_initialization();
} catch (const artdaq::database::invalid_argument& e) {
    std::cerr << "Initialization failed: " << e.what() << std::endl;
    exit(1);
}
```

**Design Pattern**: This function implements the "result to exception" conversion pattern, allowing code to mix result-based and exception-based error handling.

---

## Implementation Patterns

### Aggregate Initialization

All creation functions use C++11 aggregate initialization:
```cpp
return {false, msg};  // Instead of: return std::make_pair(false, msg);
```

**Benefits**:
- More concise
- Easier to read
- Same performance (no extra copies)
- Works with auto return type deduction

### Early Return Pattern

ThrowOnFailure uses early return for the success case:
```cpp
if (result.first) {
    return;  // Early return on success
}
// Only failure path continues
throw invalid_argument(result.second);
```

**Benefits**:
- Reduces nesting
- Makes success path clear
- Failure path is at the end (less common case)

### String Extraction Pattern

Ostringstream overloads extract string once:
```cpp
return {false, oss.str()};  // Single str() call
```

Alternative (less efficient):
```cpp
std::string msg = oss.str();
return {false, msg};  // Extra copy
```

The direct approach avoids an intermediate variable and potential extra copy.

## Error Handling Strategy

This file demonstrates a dual-mode error handling approach:

### 1. Result-Based (Primary)
```cpp
result_t result = operation();
if (!result.first) {
    // Handle error using result.second
}
```

**Use When**:
- Caller can reasonably handle failure
- Multiple operations might fail
- Performance is critical
- Failure is expected/common

### 2. Exception-Based (via ThrowOnFailure)
```cpp
auto result = operation();
ThrowOnFailure(result);  // Convert to exception
// Continue with success path
```

**Use When**:
- Failure should propagate up
- Caller can't reasonably handle error
- Simpler control flow desired
- Failure is exceptional

### Hybrid Approach Example
```cpp
result_t complex_operation() {
    // Internal operations use results
    auto r1 = step1();
    if (!r1.first) return r1;  // Propagate failure

    auto r2 = step2();
    if (!r2.first) return r2;  // Propagate failure

    return Success("All steps completed");
}

void high_level_function() {
    // High-level code converts to exceptions
    auto result = complex_operation();
    ThrowOnFailure(result);  // Must succeed here
}
```

## Performance Analysis

### Benchmarks (Approximate)

**Success Case**:
```cpp
auto r = Success("message");  // ~10 ns
```
- Pair construction
- String copy/move

**Failure Case**:
```cpp
auto r = Failure("message");  // ~10 ns
```
- Same cost as success (just different bool value)

**ThrowOnFailure (Success)**:
```cpp
ThrowOnFailure(success_result);  // ~5 ns
```
- Just a bool check and return

**ThrowOnFailure (Failure)**:
```cpp
ThrowOnFailure(failure_result);  // ~1000-10000 ns
```
- Exception construction
- Stack unwinding
- Catch block execution

### Comparison: result_t vs Direct Exception

**With result_t**:
```cpp
// Caller code
auto r = operation();  // ~10 ns
if (!r.first) {
    handle_error();     // ~100 ns
}
// Total: ~110 ns (common case: failure is frequent)
```

**With direct exception**:
```cpp
try {
    operation();        // ~10 ns (success)
                        // ~5000 ns (failure)
} catch (...) {
    handle_error();     // ~100 ns
}
// Total: ~10 ns (success) or ~5100 ns (failure)
```

**Conclusion**: For operations where failure is common (>1%), result_t provides significantly better performance.

## Memory Management

### String Lifetime
```cpp
result_t r = Failure("Error message");
```
- String is copied into the result_t
- Result owns the string
- Safe to return from functions
- No dangling references

### String Stream Conversion
```cpp
std::ostringstream oss;
oss << "Message";
return Failure(oss);  // oss.str() creates new string
```
- .str() creates a copy
- Original oss can be destroyed
- String is owned by result_t

## Thread Safety

All functions are thread-safe:
- No shared state
- Only local variables
- const parameters
- Pure functions (same input → same output)

Multiple threads can safely call these functions concurrently.

## Exception Safety

### Strong Guarantee
All functions provide strong exception safety:
- If string allocation fails, no side effects
- Original data unchanged
- Can safely retry

### ThrowOnFailure
- Success case: No-throw guarantee
- Failure case: Throws by design (intended behavior)

## Design Advantages

1. **Simplicity**: Minimal implementation, easy to understand
2. **Efficiency**: Direct initialization, no unnecessary copies
3. **Flexibility**: Both string and ostringstream interfaces
4. **Composability**: Easy to chain and transform results
5. **Interoperability**: Works with both result and exception styles

## Limitations and Alternatives

### Current Limitations

1. **No Result Chaining**: Can't easily compose multiple results
2. **Binary Success/Failure**: No partial success states
3. **Single Error**: Can't accumulate multiple errors

### Potential Enhancements

Could add:
```cpp
// Chaining
result_t operator&&(result_t const& a, result_t const& b);

// Mapping
template<typename F>
result_t map(result_t const& r, F transform);

// Multiple errors
using multi_result_t = std::pair<bool, std::vector<std::string>>;
```

But current implementation is intentionally minimal and sufficient for artdaq-database needs.

## Testing Considerations

### Test Cases to Cover

1. **Success with default message**
   ```cpp
   auto r = Success();
   assert(r.first && r.second == result::msg_Success);
   ```

2. **Success with custom message**
   ```cpp
   auto r = Success("Custom");
   assert(r.first && r.second == "Custom");
   ```

3. **Failure with default message**
   ```cpp
   auto r = Failure();
   assert(!r.first && r.second == result::msg_Failure);
   ```

4. **Failure with custom message**
   ```cpp
   auto r = Failure("Error");
   assert(!r.first && r.second == "Error");
   ```

5. **Ostringstream overloads**
   ```cpp
   std::ostringstream oss;
   oss << "Message " << 42;
   auto r = Failure(oss);
   assert(!r.first && r.second == "Message 42");
   ```

6. **ThrowOnFailure with success**
   ```cpp
   auto r = Success();
   ThrowOnFailure(r);  // Should not throw
   // Test passes if no exception
   ```

7. **ThrowOnFailure with failure**
   ```cpp
   auto r = Failure("Error");
   try {
       ThrowOnFailure(r);
       assert(false);  // Should not reach here
   } catch (const invalid_argument& e) {
       assert(std::string(e.what()) == "Error");
   }
   ```

## Related Files

- **returned_result.h** - Function declarations and type definitions
- **shared_exceptions.h** - Provides invalid_argument exception type
- Database operation implementations - Primary users of result_t
- API layer - Converts result_t to HTTP responses

## Best Practices for Implementation

1. **Use aggregate initialization**: `{false, msg}` instead of `std::make_pair`
2. **Early return for success**: Makes code more readable
3. **Single string extraction**: Call .str() only once
4. **Minimal error handling**: Let exceptions propagate
5. **Const correctness**: All parameters are const references

## Notes

- Implementation is intentionally minimal and focused
- No TRACE logging in these functions (would add noise)
- String copies are acceptable for error path
- Exception from ThrowOnFailure is by design, not a bug
- Functions are inline candidates but not marked inline (let compiler decide)
