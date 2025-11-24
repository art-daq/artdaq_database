# shared_exceptions.h

## File Overview

This header file defines a hierarchy of exception types used throughout the artdaq-database project. It extends the CET (Common Exception Type) exception framework to provide domain-specific exception types with consistent formatting and error reporting.

**Location**: `/home/user/artdaq-database/artdaq-database/SharedCommon/shared_exceptions.h`

## Dependencies

### Standard Library
- `<exception>` - Base exception class

### External Libraries
- `"cetlib_except/coded_exception.h"` - CET exception framework (from Fermilab's cetlib)

## Header Guard

```cpp
#ifndef _ARTDAQ_DATABASE_EXCEPTIONS_H_
#define _ARTDAQ_DATABASE_EXCEPTIONS_H_
```

## External Declaration

```cpp
namespace debug {
std::string current_exception_diagnostic_information();
}
```

Forward declaration of the exception diagnostics function from `printStackTrace.h`.

## Namespace: artdaq::database

All exception classes are in the `artdaq::database` namespace.

## Exception Hierarchy

```
std::exception (standard)
    └─ cet::exception (CET framework)
        └─ artdaq::database::exception (base)
            ├─ artdaq::database::invalid_argument
            ├─ artdaq::database::runtime_error
            ├─ artdaq::database::invalid_option_exception
            └─ artdaq::database::runtime_exception
```

---

## Base Exception Class

### exception

```cpp
class exception : public cet::exception {
 public:
  using cet::exception::Category;

  explicit exception(Category const& category) : cet::exception{category} {}

  exception(Category const& category, std::string const& message)
      : cet::exception{category, message} {}
};
```

**Purpose**: Base class for all artdaq-database exceptions. Extends CET exception framework with project-specific behavior.

**Type Alias**:
- `Category` - Inherited from cet::exception, represents error category string

**Constructors**:

1. **Category-only constructor**:
   ```cpp
   explicit exception(Category const& category);
   ```
   - Creates exception with just a category name
   - Message can be added later using stream operators

2. **Category and message constructor**:
   ```cpp
   exception(Category const& category, std::string const& message);
   ```
   - Creates exception with category and initial message
   - Additional details can be appended using stream operators

**Inherited Functionality**:
- Stream operator (`<<`) for building error messages
- `what()` method returning formatted error string
- `category()` method returning error category
- Exception chaining support

**Usage Example**:
```cpp
throw artdaq::database::exception("database_error")
    << "Failed to connect to database"
    << " host=" << hostname
    << " port=" << port;
```

**Commented-Out Code**:
```cpp
/*
  template <typename W>
  exception& operator<<(W const& w) {
    this->append(w);
    return *this;
  }
*/
```
This stream operator is commented out because it's already provided by cet::exception base class.

---

## Derived Exception Classes

### invalid_argument

```cpp
class invalid_argument : public exception {
 public:
  explicit invalid_argument(std::string const& category_)
      : exception(category_) {}
};
```

**Purpose**: Indicates an invalid argument was passed to a function.

**Use Cases**:
- Invalid parameter values
- Out-of-range arguments
- Malformed input data
- Type mismatches

**Usage Example**:
```cpp
void set_count(int count) {
    if (count < 0) {
        throw invalid_argument("set_count")
            << "Count must be non-negative, got: " << count;
    }
}
```

**Standard Library Equivalent**: Similar to `std::invalid_argument`, but with CET exception features.

---

### runtime_error

```cpp
class runtime_error : public exception {
 public:
  explicit runtime_error(std::string const& category_)
      : exception(category_) {}
};
```

**Purpose**: Indicates a runtime error that couldn't be detected at compile time.

**Use Cases**:
- I/O errors
- File not found
- Network failures
- Resource exhaustion
- Unexpected system states

**Usage Example**:
```cpp
void read_file(std::string const& path) {
    std::ifstream file(path);
    if (!file) {
        throw runtime_error("read_file")
            << "Failed to open file: " << path;
    }
}
```

**Standard Library Equivalent**: Similar to `std::runtime_error`, but with CET exception features.

---

### invalid_option_exception

```cpp
class invalid_option_exception : public exception {
 public:
  using exception::Category;

  explicit invalid_option_exception(Category const& category)
      : exception{category} {}

  invalid_option_exception(Category const& category, std::string const& message)
      : exception{category, message} {}
};
```

**Purpose**: Indicates an invalid command-line option or configuration option.

**Type Alias**:
- `Category` - Re-exported from base class for convenience

**Constructors**: Provides both category-only and category-with-message constructors.

**Use Cases**:
- Invalid command-line arguments
- Unrecognized configuration options
- Missing required options
- Conflicting options
- Invalid option values

**Usage Example**:
```cpp
void parse_options(int argc, char** argv) {
    if (argv[1] == "--invalid-option") {
        throw invalid_option_exception("parse_options")
            << "Unrecognized option: " << argv[1];
    }

    if (missing_required_option) {
        throw invalid_option_exception("parse_options",
                                      "Missing required option: --config");
    }
}
```

**Typical Handling**:
```cpp
try {
    parse_options(argc, argv);
} catch (invalid_option_exception const& e) {
    std::cerr << e.what() << std::endl;
    print_usage();
    return process_exit_code::INVALID_ARGUMENT;
}
```

---

### runtime_exception

```cpp
class runtime_exception : public exception {
 public:
  using exception::Category;

  explicit runtime_exception(Category const& category)
      : exception{category} {}

  runtime_exception(Category const& category, std::string const& message)
      : exception{category, message} {}
};
```

**Purpose**: Similar to runtime_error but with both constructor variants.

**Type Alias**:
- `Category` - Re-exported from base class

**Constructors**: Provides both category-only and category-with-message constructors.

**Use Cases**: Same as runtime_error, but provides more flexible construction.

**Usage Example**:
```cpp
// With category only
throw runtime_exception("database_operation");

// With category and message
throw runtime_exception("database_operation", "Connection timeout");

// Building message with stream operators
throw runtime_exception("database_operation")
    << "Failed after " << retries << " retries";
```

**vs runtime_error**: Provides the same functionality as runtime_error but offers both constructor variants for convenience. Use whichever fits the use case better.

---

## CET Exception Framework Features

All exception classes inherit these features from `cet::exception`:

### Stream Operator

```cpp
exception& operator<<(T const& value);
```

**Purpose**: Append formatted values to exception message.

**Usage**:
```cpp
throw invalid_argument("function_name")
    << "Error details: "
    << "value=" << value
    << ", expected=" << expected;
```

### what() Method

```cpp
const char* what() const noexcept override;
```

**Returns**: Formatted error message including:
- Exception category
- All appended messages
- Stack trace (if available)
- Additional context

**Example Output**:
```
---- invalid_argument BEGIN
  function_name: Error details: value=42, expected=100
  ... additional context ...
---- invalid_argument END
```

### category() Method

```cpp
std::string const& category() const noexcept;
```

**Returns**: The category string passed to constructor.

### Exception Nesting

CET exceptions support nested exceptions for preserving error context:

```cpp
try {
    low_level_operation();
} catch (exception const& e) {
    throw runtime_error("high_level_operation")
        << "Failed during low-level operation"
        << e;  // Nests the caught exception
}
```

---

## Usage Patterns

### Basic Throwing

```cpp
if (error_condition) {
    throw invalid_argument("function_name")
        << "Detailed error message";
}
```

### With Context

```cpp
throw runtime_error("database_write")
    << "Failed to write document"
    << " id=" << doc_id
    << " collection=" << collection
    << " error=" << error_message;
```

### Catching and Handling

```cpp
try {
    database_operation();
} catch (invalid_argument const& e) {
    // Handle invalid arguments specifically
    std::cerr << "Invalid argument: " << e.what() << std::endl;
    return false;
} catch (runtime_error const& e) {
    // Handle runtime errors
    std::cerr << "Runtime error: " << e.what() << std::endl;
    return false;
} catch (exception const& e) {
    // Catch all artdaq::database exceptions
    std::cerr << "Database error: " << e.what() << std::endl;
    return false;
} catch (std::exception const& e) {
    // Catch any standard exception
    std::cerr << "Unexpected error: " << e.what() << std::endl;
    return false;
}
```

### Rethrowing with Context

```cpp
try {
    risky_operation();
} catch (exception const& e) {
    throw runtime_exception("wrapper_function")
        << "Operation failed with error: "
        << e.what();
}
```

### Converting from result_t

```cpp
auto result = operation_that_returns_result();
if (!result.first) {
    throw runtime_error("calling_function")
        << result.second;  // Use error message from result
}
```

---

## Design Rationale

### Why CET Exceptions?

1. **Consistent Formatting**: All exceptions format consistently across artdaq projects
2. **Rich Context**: Supports nested exceptions and detailed error context
3. **Stream Building**: Natural message construction using `<<` operator
4. **Integration**: Works with artdaq framework tools and logging
5. **Proven**: Battle-tested in large HEP (High Energy Physics) projects

### Why Multiple Exception Types?

Different exception types enable:

1. **Selective Catching**: Catch specific error types
   ```cpp
   catch (invalid_option_exception const& e) {
       print_usage();  // Only for option errors
   }
   ```

2. **Appropriate Error Codes**: Map to different exit codes
   ```cpp
   catch (invalid_argument const&) {
       return process_exit_code::INVALID_ARGUMENT;
   }
   ```

3. **Documentation**: Exception type documents error cause
   ```cpp
   void process(int value);  // What exceptions can this throw?
   /// @throws invalid_argument if value < 0
   /// @throws runtime_error if operation fails
   ```

4. **Different Handling**: Different recovery strategies
   ```cpp
   catch (invalid_argument const&) {
       // Fix input and retry
   } catch (runtime_error const&) {
       // Operation failed, can't retry
   }
   ```

### Category String Convention

By convention, use function/component name as category:

```cpp
void MyClass::my_method() {
    throw invalid_argument("MyClass::my_method")
        << "Error details";
}
```

This makes it easy to identify where exceptions originate.

---

## Comparison with Standard Exceptions

### Standard Library
```cpp
throw std::invalid_argument("Simple message");
```
- Simple message only
- No context building
- No nested exceptions
- Less formatting

### CET/artdaq-database
```cpp
throw invalid_argument("function_name")
    << "Detailed message"
    << " param=" << param
    << " context=" << context;
```
- Rich message building
- Nested exception support
- Consistent formatting
- Category identification

---

## Exception Safety Guarantees

Functions should document their exception safety guarantees:

### No-throw Guarantee
```cpp
void cleanup() noexcept {
    // Never throws
}
```

### Strong Guarantee
```cpp
void update(Data& data) {
    // Either succeeds completely or leaves data unchanged
    // May throw exceptions
}
```

### Basic Guarantee
```cpp
void partial_update(Data& data) {
    // May leave data in valid but modified state on exception
    // No leaks or corruption
}
```

### No Guarantee
```cpp
void unsafe_operation() {
    // May leave system in inconsistent state on exception
    // Avoid this!
}
```

---

## Best Practices

1. **Use Specific Types**: Choose the most specific exception type
   ```cpp
   // Good
   throw invalid_argument("parse_config");

   // Less specific
   throw exception("parse_config");
   ```

2. **Include Context**: Add relevant details to error messages
   ```cpp
   throw runtime_error("read_file")
       << "Failed to read configuration file"
       << " path=" << path
       << " error=" << strerror(errno);
   ```

3. **Use Function Name as Category**: Makes debugging easier
   ```cpp
   void MyClass::process() {
       throw invalid_argument("MyClass::process")
           << "Details";
   }
   ```

4. **Don't Throw in Destructors**: Destructors should be noexcept
   ```cpp
   ~MyClass() noexcept {
       // Clean up, never throw
   }
   ```

5. **Document Exceptions**: Tell users what can be thrown
   ```cpp
   /// @throws invalid_argument if path is empty
   /// @throws runtime_error if file cannot be read
   void load_config(path_t const& path);
   ```

6. **Catch by const Reference**: Prevents slicing and copying
   ```cpp
   catch (exception const& e) {  // Good
       // ...
   }

   catch (exception e) {  // Bad: copies exception
       // ...
   }
   ```

---

## Debugging Exception Issues

### Getting Stack Trace

If printStackTrace handlers are registered:
```cpp
try {
    operation();
} catch (exception const& e) {
    std::cerr << e.what() << std::endl;
    std::cerr << debug::getCxaThrowStack() << std::endl;
}
```

### Getting Diagnostic Information

```cpp
catch (...) {
    std::cerr << debug::current_exception_diagnostic_information();
}
```

### Enable Exception Logging

Configure TRACE to log exceptions:
```cpp
TLOG(TLVL_ERROR) << "Exception caught: " << e.what();
```

---

## Related Files

- **printStackTrace.h/cpp** - Provides exception diagnostics and stack traces
- **returned_result.h/cpp** - Alternative to exceptions for expected errors
- **helper_functions.h** - Uses exceptions for validation failures
- All implementation files - Throw these exceptions on errors

## Notes

- All exception types are lightweight (just category and message storage)
- Exception messages are built lazily (only formatted when what() is called)
- CET exceptions are thread-safe
- The framework supports exception chaining and nesting
- Category strings are stored, not copied (efficient)
- Stream operators work with any type that has `operator<<` defined
- Exception types mirror standard library naming for familiarity
