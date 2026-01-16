# docrecord_exceptions.h

**Path:** `artdaq-database/JsonDocument/docrecord_exceptions.h`

**Purpose:** This header file defines custom exception types specific to the JsonDocument module's document record operations. It provides specialized exceptions for handling common error conditions when working with JSON documents: `notfound_exception` for missing elements and `readonly_exception` for write-protection violations.


## Key Concepts

### Exception Hierarchy

The exceptions inherit from the project's base `exception` class, providing:
- **Stream-style error message composition** via `operator<<`
- **Category tracking** for error organization and filtering
- **Standard `std::exception` compatibility** via `what()` method
- **Consistent error formatting** across the project

### Semantic Exception Types

Two distinct exception types serve different error conditions:

| Exception | Semantic Meaning | Use Case |
|-----------|------------------|----------|
| `notfound_exception` | Element or path does not exist | Navigation failures, missing fields |
| `readonly_exception` | Write operation blocked | Protected or archived documents |

Using specific exception types allows callers to handle different errors appropriately rather than parsing error messages.

## Thread Safety

- **Thread-safe:** Yes (exception construction is thread-safe)
- **Concurrent access:** Exception objects should not be shared between threads
- **Locking:** No internal locks

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/SharedCommon/shared_exceptions.h` | Provides the base `exception` class with stream operators and category support |

## Classes/Structures

### `notfound_exception`

**Brief:** Exception class thrown when a requested JSON element, path, or child document cannot be found in a document structure.

A final exception class that inherits from `artdaq::database::exception`. Used throughout the JsonDocument module to signal that a path traversal or element lookup has failed.

**Thread Safety:** Thread-safe for construction; exception objects should not be shared between threads.

#### Constructor

##### `notfound_exception(std::string const& category_)`

**Brief:** Constructor that creates a notfound_exception with a category string for error categorization and filtering.

**Parameters:**
- `category_` - Category string identifying the source of the exception (e.g., "JSONDocument", "JSONDocumentBuilder")

**Throws:** None (noexcept constructor)

**Thread Safety:** safe (construction)

**Example:**
```cpp
#include "artdaq-database/JsonDocument/docrecord_exceptions.h"

using namespace artdaq::database::docrecord;

value_t const& findChildValue(object_t const& obj, std::string const& key) {
  if (obj.count(key) == 0) {
    throw notfound_exception("JSONDocument")
        << "Failed calling findChildValue(): Key not found; key=" << key;
  }
  return obj.at(key);
}
```

### `readonly_exception`

**Brief:** Exception class thrown when attempting to modify a document or field that has been marked as read-only through bookkeeping flags.

A final exception class that inherits from `artdaq::database::exception`. Used to enforce write protection on archived, locked, or immutable documents.

**Thread Safety:** Thread-safe for construction; exception objects should not be shared between threads.

#### Constructor

##### `readonly_exception(std::string const& category_)`

**Brief:** Constructor that creates a readonly_exception with a category string for error categorization.

**Parameters:**
- `category_` - Category string identifying the source of the exception (e.g., "JSONDocumentBuilder", "ovlBookkeeping")

**Throws:** None (noexcept constructor)

**Thread Safety:** safe (construction)

**Example:**
```cpp
#include "artdaq-database/JsonDocument/docrecord_exceptions.h"

using namespace artdaq::database::docrecord;

void modifyDocument(JSONDocument& doc, bool isReadonly) {
  if (isReadonly) {
    throw readonly_exception("JSONDocumentBuilder")
        << "Cannot modify document: marked as readonly";
  }
  // Proceed with modification
}
```

## Exception Hierarchy

```
std::exception
    |
    +-- artdaq::database::exception (from shared_exceptions.h)
            |
            +-- notfound_exception
            |
            +-- readonly_exception
```

## When Exceptions Are Thrown

### notfound_exception

Thrown in the following scenarios:

| Location | Condition |
|----------|-----------|
| `JSONDocument::findChildValue()` | Path traversal fails to find element |
| `JSONDocument::findChild()` | Requested child element does not exist |
| `JSONDocument::findChildDocument()` | Document at specified path is missing |
| `JSONDocument::replaceChild()` | Target element to replace does not exist |
| `JSONDocument::insertChild()` | Parent path for insertion is invalid |
| `JSONDocument::deleteChild()` | Element to delete is not found |
| `JSONDocumentBuilder::_importUserData()` | Optional field lookup (caught internally) |

### readonly_exception

Thrown in the following scenarios:

| Location | Condition |
|----------|-----------|
| `JSONDocumentBuilder` methods | When `bookkeeping.isreadonly` flag is true |
| `ovlBookkeeping` overlay | When enforcing write protection |
| Configuration updates | When document is archived or locked |

## Common Use Cases

### notfound_exception Use Cases

- Searching for a non-existent path in a JSON document
- Attempting to find a child element that does not exist
- Querying for missing metadata or configuration fields
- Path traversal failures during document navigation

### readonly_exception Use Cases

- Attempting to modify a document marked as readonly via bookkeeping flags
- Trying to update soft-deleted documents
- Modifying archived or locked configurations
- Write operations on immutable document records

## Error Handling Patterns

### Specific Exception Handling

```cpp
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/docrecord_exceptions.h"

using namespace artdaq::database::docrecord;

void processDocument(JSONDocument& doc) {
  try {
    auto child = doc.findChild("configuration.settings");
    // Process child
  } catch (notfound_exception const& ex) {
    // Handle missing element - maybe use defaults
    std::cerr << "Element not found: " << ex.what() << std::endl;
  } catch (readonly_exception const& ex) {
    // Handle write-protection violation
    std::cerr << "Cannot modify: " << ex.what() << std::endl;
  }
}
```

### Generic Exception Handling

```cpp
try {
  // Document operations
} catch (artdaq::database::exception const& ex) {
  // Handles all database exceptions including notfound and readonly
  std::cerr << "Database error: " << ex.what() << std::endl;
}
```

### Optional Field Pattern

```cpp
// Pattern used in _importUserData() for optional fields
try {
  auto metadata = document.findChild("document.metadata");
  // Use metadata
} catch (notfound_exception const&) {
  // Field is optional - silently continue without it
}
```

## Relationship to Other Components

### Within the JsonDocument Module

- **JSONDocument.cpp** - Primary user of `notfound_exception` for path operations
- **JSONDocument_utils.cpp** - Uses `notfound_exception` in `_importUserData()`
- **JSONDocumentBuilder.cpp** - Uses `readonly_exception` when checking bookkeeping
- **common.h** - Includes this header for module-wide availability

### In Overlay Module

- **ovlBookkeeping** - Throws `readonly_exception` when write protection is active

### Dependencies

- **shared_exceptions.h** - Provides base `exception` class with:
  - Stream operator (`operator<<`) for message building
  - Category tracking for error filtering
  - Standard `std::exception` interface (`what()` method)

## Notes for Developers

### Error Message Construction

Both exception types support streaming for descriptive error messages:

```cpp
throw notfound_exception("JSONDocument")
    << "Failed calling " << __func__ << "(): "
    << "Invalid path; path=" << path
    << ", element=" << element;
```

Creates message like: `"JSONDocument: Failed calling findChild(): Invalid path; path=document.data.field, element=field"`

### Best Practices

1. **Always provide context** - Include operation name and reason:
   ```cpp
   throw notfound_exception("MyClass") << "Failed doing X because Y";
   ```

2. **Include relevant data** - Add paths, field names, and values:
   ```cpp
   throw notfound_exception("JSONDocument") << "Path not found: " << path;
   ```

3. **Catch specifically when possible** - Handle each exception type appropriately:
   ```cpp
   catch (notfound_exception const& ex) { /* handle missing */ }
   catch (readonly_exception const& ex) { /* handle protected */ }
   ```

4. **Document thrown exceptions** - Use comments or documentation:
   ```cpp
   /// @throws notfound_exception if path does not exist
   /// @throws readonly_exception if document is protected
   value_t& findAndModify(path_t const& path);
   ```

5. **Use for exceptional conditions** - Not for normal control flow:
   - **Good:** Throw when required configuration is missing
   - **Bad:** Throw to signal optional field is not present (use try-catch internally)

### Header Guard Note

The closing comment references a different guard name than the opening:
```cpp
#ifndef _ARTDAQ_DATABASE_DOCRECORD_EXCEPTIONS_H_
// ...
#endif /* _ARTDAQ_DATABASE_JSONRECORD_EXCEPTIONS_H_ */
```

This is a minor inconsistency (`JSONRECORD` vs `DOCRECORD`) but does not affect functionality.

## See Also

- [common.h.md](./common.h.md) - Aggregator header that includes this file
- [JSONDocument.h.md](./JSONDocument.h.md) - Primary user of `notfound_exception`
- [JSONDocumentBuilder.h.md](./JSONDocumentBuilder.h.md) - User of `readonly_exception`
- [../SharedCommon/shared_exceptions.h.md](../SharedCommon/shared_exceptions.h.md) - Base exception class
- [../Overlay/ovlBookkeeping.h.md](../Overlay/ovlBookkeeping.h.md) - Uses `readonly_exception`
