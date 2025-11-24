# docrecord_exceptions.h

## File Overview

This header file defines custom exception types specific to the JsonDocument module's document record operations. It provides specialized exceptions for handling common error conditions when working with JSON documents, particularly for missing elements and write-protection violations.

**Location**: `/home/user/artdaq-database/artdaq-database/JsonDocument/docrecord_exceptions.h`

## Dependencies

```cpp
#include "artdaq-database/SharedCommon/shared_exceptions.h"
```

The file depends on the shared exceptions module which provides the base `exception` class used throughout the artdaq-database project.

## Header Guard

```cpp
#ifndef _ARTDAQ_DATABASE_DOCRECORD_EXCEPTIONS_H_
#define _ARTDAQ_DATABASE_DOCRECORD_EXCEPTIONS_H_
...
#endif
```

**Note**: The closing guard comment references `_ARTDAQ_DATABASE_JSONRECORD_EXCEPTIONS_H_` (JSONRECORD instead of DOCRECORD), which is a minor inconsistency but doesn't affect functionality.

## Namespace Structure

```cpp
namespace artdaq {
namespace database {
namespace docrecord {
  // Exception classes defined here
}
}
}
```

All exception classes are defined in the `artdaq::database::docrecord` namespace, which is the primary namespace for JSON document record operations.

## Using Declaration

```cpp
using artdaq::database::exception;
```

This brings the base `exception` class into the `docrecord` namespace, making it available for inheritance without fully qualifying the name.

## Exception Classes

### 1. notfound_exception

**Purpose**: Thrown when a requested JSON element, path, or child document cannot be found.

**Definition**:
```cpp
class notfound_exception : public exception {
 public:
  explicit notfound_exception(std::string const& category_) : exception(category_) {}
};
```

**Characteristics**:
- Inherits from `artdaq::database::exception`
- Takes a category string in constructor for error categorization
- Uses `explicit` keyword to prevent implicit conversions

**Common Use Cases**:
- Searching for a non-existent path in a JSON document
- Attempting to find a child element that doesn't exist
- Querying for missing metadata or configuration fields
- Path traversal failures during document navigation

**Example Usage in Code**:
```cpp
// From JSONDocument.cpp
if (childDocument.count(path_token) == 0) {
  throw notfound_exception("JSONDocument")
    << "Failed calling findChildValue(): Search failed for JSON element name="
    << path_token << ", search path =<" << path << ">.";
}
```

### 2. readonly_exception

**Purpose**: Thrown when attempting to modify a document or field that has been marked as read-only.

**Definition**:
```cpp
class readonly_exception : public exception {
 public:
  explicit readonly_exception(std::string const& category_) : exception(category_) {}
};
```

**Characteristics**:
- Inherits from `artdaq::database::exception`
- Takes a category string in constructor for error categorization
- Uses `explicit` keyword to prevent implicit conversions

**Common Use Cases**:
- Attempting to modify a document marked as readonly via bookkeeping flags
- Trying to update deleted documents
- Modifying archived or locked configurations
- Write operations on immutable document records

**Typical Scenarios**:
```cpp
// Hypothetical usage
if (document.isReadonly()) {
  throw readonly_exception("JSONDocumentBuilder")
    << "Cannot modify document: marked as readonly";
}
```

## Exception Base Class

Both exceptions inherit from `artdaq::database::exception`, which provides:

- **Stream-style error messages**: Using `operator<<` for composing error messages
- **Category tracking**: For organizing exceptions by component
- **Standard exception interface**: Compatible with `std::exception`
- **Formatted error output**: Consistent error message formatting

## Usage Pattern

The typical pattern for using these exceptions:

```cpp
try {
  auto child = document.findChild("some.path.to.element");
  // ... use child ...
} catch (notfound_exception const& ex) {
  // Handle missing element
  std::cerr << "Element not found: " << ex.what() << std::endl;
} catch (readonly_exception const& ex) {
  // Handle write-protection violation
  std::cerr << "Cannot modify: " << ex.what() << std::endl;
}
```

## Error Message Construction

Both exception types support the streaming operator for building descriptive error messages:

```cpp
throw notfound_exception("JSONDocument")
    << "Failed calling " << caller << "(): Invalid path; path=" << path;
```

This creates a message like: "JSONDocument: Failed calling insertChild(): Invalid path; path=document.data.field"

## Design Rationale

**Why Separate Exception Types?**

1. **Semantic Clarity** - Different exceptions for different error conditions
2. **Selective Handling** - Allows catching specific errors differently
3. **Error Recovery** - NotFound might be recoverable, ReadOnly usually isn't
4. **API Documentation** - Function signatures can document which exceptions they throw
5. **Debugging** - Stack traces immediately reveal the type of error

**Why Inherit from Base Exception?**

1. **Consistency** - All artdaq-database exceptions work the same way
2. **Shared Functionality** - Stream-style message building
3. **Polymorphic Handling** - Can catch base class to handle all database exceptions
4. **Error Formatting** - Consistent error message format across the project

## Exception Hierarchy

```
std::exception
    └── artdaq::database::exception (from shared_exceptions.h)
            ├── notfound_exception
            └── readonly_exception
```

## Where These Exceptions Are Thrown

### notfound_exception Usage:
- `JSONDocument::findChildValue()` - When path traversal fails
- `JSONDocument::findChild()` - When element doesn't exist
- `JSONDocument::findChildDocument()` - When document at path is missing
- `JSONDocument::replaceChild()` - When target doesn't exist
- `JSONDocument::insertChild()` - When parent path is invalid
- `JSONDocument::deleteChild()` - When element to delete doesn't exist

### readonly_exception Usage:
- Likely used in `JSONDocumentBuilder` when checking bookkeeping flags
- Used by overlay classes when enforcing write protection
- Database operations that check readonly status

## Related Files

- **shared_exceptions.h** - Defines the base `exception` class
- **JSONDocument.cpp** - Primary user of `notfound_exception`
- **JSONDocumentBuilder.cpp** - Likely user of `readonly_exception`
- **Overlay classes** - Use these exceptions for validation

## Best Practices

When using these exceptions in your code:

1. **Always provide context** - Include the operation that failed and why
   ```cpp
   throw notfound_exception("MyClass") << "Failed doing X because Y";
   ```

2. **Include relevant data** - Add path names, field names, values
   ```cpp
   throw notfound_exception("JSONDocument") << "Path not found: " << path;
   ```

3. **Catch specifically when possible** - Don't just catch `exception` if you can handle specific types
   ```cpp
   catch (notfound_exception const& ex) { /* specific handling */ }
   ```

4. **Document thrown exceptions** - In function comments, note which exceptions can be thrown
   ```cpp
   /// @throws notfound_exception if path doesn't exist
   ```

5. **Use for exceptional conditions** - Not for normal control flow
   - Good: Throw when required configuration is missing
   - Bad: Throw to signal optional field isn't present

## Notes

- Both exception classes are lightweight wrappers around the base exception class
- The `explicit` constructor prevents accidental implicit conversions from strings
- Exception messages are built at throw-time using the streaming operator
- These exceptions integrate with the TRACE logging system for error tracking
- The category parameter helps organize log messages and stack traces
