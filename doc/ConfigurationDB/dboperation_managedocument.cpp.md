# dboperation_managedocument.cpp

**Path:** `artdaq-database/ConfigurationDB/dboperation_managedocument.cpp`

**Implements:** [dboperation_managedocument.h](./dboperation_managedocument.h.md)

**Purpose:** Implements the public wrapper functions for document management operations. These wrapper functions provide exception safety by catching all exceptions and converting them to `result_t` return values. The actual logic is delegated to `detail::` namespace functions.

## Implementation Overview

This file implements a thin wrapper layer that:
1. Catches all exceptions and converts them to failure results
2. Validates input parameters for JSON-based functions
3. Delegates to `detail::` namespace functions for actual implementation
4. Returns success/failure status via `result_t`

The implementation follows a consistent pattern across all functions:
```cpp
result_t function(params) noexcept {
  try {
    detail::function_impl(params);
    return Success(result);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

## Thread Safety

- **Thread-safe:** Conditional
- **Concurrent access:** Each function call is independent; the wrapper layer itself is thread-safe, but the underlying detail functions may not be
- **Locking:** No internal locking in the wrapper layer

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | Common definitions and TRACE macros |
| `artdaq-database/BasicTypes/basictypes.h` | Basic type wrappers (JsonData, FhiclData, etc.) |
| `artdaq-database/ConfigurationDB/dboperation_managedocument.h` | Header being implemented |
| `artdaq-database/ConfigurationDB/options_operations.h` | ManageDocumentOperation class |
| `artdaq-database/ConfigurationDB/shared_helper_functions.h` | Helper utilities |
| `artdaq-database/DataFormats/shared_literals.h` | Error message literals (msg_EmptyFilter, msg_EmptyDocument) |
| `artdaq-database/SharedCommon/configuraion_api_literals.h` | API operation name literals |

## Key Algorithms

### Exception-Safe Wrapper Pattern

All public functions follow this pattern to ensure `noexcept` guarantee:

**Steps:**
1. Enter try block
2. Validate input parameters (for JSON functions)
3. Call corresponding `detail::` function
4. Wrap result in `Success()` and return
5. If any exception occurs, catch it
6. Extract diagnostic information and return `Failure()`

**Why this approach:** This pattern ensures that:
- The API is exception-safe and suitable for FFI/SWIG bindings
- All error information is preserved in the result string
- Callers can use simple success/failure checks

### JSON Payload Processing

For functions in the `json::` namespace:

**Steps:**
1. Check if payload is empty (return early with error if so)
2. Create a `ManageDocumentOperation` object with appropriate operation name
3. Parse JSON payload into the options object via `readJsonData()`
4. Delegate to `detail::` implementation
5. Return result

**Why this approach:** Separates payload parsing from business logic, allowing the same implementation to serve both programmatic and REST API interfaces.

## Internal Functions

### Forward Declarations in `detail` Namespace

The file declares (but does not define) the following internal functions:

#### `detail::write_document(options, conf) -> void`

**Brief:** Internal implementation of document writing logic.

**Called by:** `opts::write_document()`, `json::write_document()`

**Purpose:** Performs the actual write operation by dispatching to the appropriate storage provider.

---

#### `detail::read_document(options, conf) -> void`

**Brief:** Internal implementation of document reading logic.

**Called by:** `opts::read_document()`, `json::read_document()`

**Purpose:** Retrieves document from storage provider and populates the output buffer.

---

#### `detail::find_versions(options, returnValue) -> void`

**Brief:** Internal implementation of version search.

**Called by:** `opts::find_versions()`, `json::find_versions()`

**Purpose:** Queries the storage provider for all matching versions.

---

#### `detail::find_entities(options, returnValue) -> void`

**Brief:** Internal implementation of entity search.

**Called by:** `opts::find_entities()`, `json::find_entities()`

**Purpose:** Queries the storage provider for all matching entities.

---

#### `detail::add_entity(options, returnValue) -> void`

**Brief:** Internal implementation of entity addition.

**Called by:** `opts::add_entity()`, `json::add_entity()`

**Purpose:** Adds an entity association to an existing configuration.

---

#### `detail::remove_entity(options, returnValue) -> void`

**Brief:** Internal implementation of entity removal.

**Called by:** `opts::remove_entity()`, `json::remove_entity()`

**Purpose:** Removes an entity association from a configuration.

---

#### `detail::mark_document_readonly(options, returnValue) -> void`

**Brief:** Internal implementation of read-only marking.

**Called by:** `opts::mark_document_readonly()`, `json::mark_document_readonly()`

**Purpose:** Updates document metadata to mark it as read-only.

---

#### `detail::mark_document_deleted(options, returnValue) -> void`

**Brief:** Internal implementation of soft delete.

**Called by:** `opts::mark_document_deleted()`, `json::mark_document_deleted()`

**Purpose:** Updates document metadata to mark it as deleted.

## Function Implementations

### opts::write_document

```cpp
result_t opts::write_document(ManageDocumentOperation const& options, std::string& conf) noexcept {
  try {
    detail::write_document(options, conf);
    return Success(conf);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Passes `conf` by reference, allowing the detail function to modify it
- Returns the (potentially modified) `conf` on success

---

### opts::read_document

```cpp
result_t opts::read_document(ManageDocumentOperation const& options, std::string& conf) noexcept {
  try {
    detail::read_document(options, conf);
    return Success(conf);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Output buffer `conf` is populated by the detail function
- Returns the populated buffer on success

---

### opts::find_versions

```cpp
result_t opts::find_versions(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};
    detail::find_versions(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Creates a local buffer for results
- Pattern used by all query operations (find_versions, find_entities)

---

### json::write_document

```cpp
result_t json::write_document(std::string const& query_payload, std::string const& conf) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }
    if (conf.empty()) {
      return Failure(msg_EmptyDocument);
    }
    auto options = ManageDocumentOperation{apiliteral::operation::writedocument};
    options.readJsonData({query_payload});
    auto database_format = std::string(conf);
    detail::write_document(options, database_format);
    return Success(database_format);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Validates both payload and document content
- Creates a copy of `conf` for processing (preserves original)
- Uses `apiliteral::operation::writedocument` operation name

---

### json::read_document

```cpp
result_t json::read_document(std::string const& query_payload, std::string& conf) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }
    auto options = ManageDocumentOperation{apiliteral::operation::readdocument};
    options.readJsonData({query_payload});
    auto database_format = std::string{};
    detail::read_document(options, database_format);
    conf = database_format;
    return Success(conf);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Only validates payload (document content is output)
- Assigns result to output parameter `conf`

---

### json::add_entity

```cpp
result_t json::add_entity(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }
    TLOG(10) << "add_entity 1";
    auto options = ManageDocumentOperation{apiliteral::operation::addentity};
    TLOG(10) << "add_entity 2";
    options.readJsonData({query_payload});
    TLOG(10) << "add_entity 2";
    auto returnValue = std::string{};
    detail::add_entity(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Contains debug TLOG statements for troubleshooting
- Note: Second TLOG(10) message is duplicated (likely copy-paste artifact)

---

### json::mark_document_deleted

```cpp
result_t json::mark_document_deleted(std::string const& query_payload) noexcept {
  try {
    // Note: Empty check is commented out
    auto options = ManageDocumentOperation{apiliteral::operation::markdeleted};
    options.readJsonData({query_payload});
    auto returnValue = std::string{};
    detail::mark_document_deleted(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Unlike other JSON functions, empty payload check is commented out
- This may allow marking documents deleted with default/empty criteria

---

### debug::ManageDocuments

```cpp
void dbcfg::debug::ManageDocuments() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);
  dbcfg::debug::detail::ManageDocuments();
  TLOG(10) << "artdaq::database::configuration::ManageDocuments trace_enable";
}
```

**Implementation notes:**
- Enables all TRACE levels (0xFFFFFFFFFFFFFFFFLL masks)
- Configures memory and slow modes
- Calls detail namespace debug function
- Logs confirmation message

## API Operation Names

The file uses these operation name literals from `configuraion_api_literals.h`:

| Literal | Value | Used by |
|---------|-------|---------|
| `apiliteral::operation::writedocument` | "writedocument" | `json::write_document` |
| `apiliteral::operation::readdocument` | "readdocument" | `json::read_document` |
| `apiliteral::operation::findversions` | "findversions" | `json::find_versions` |
| `apiliteral::operation::findentities` | "findentities" | `json::find_entities` |
| `apiliteral::operation::addentity` | "addentity" | `json::add_entity` |
| `apiliteral::operation::rmentity` | "rmentity" | `json::remove_entity` |
| `apiliteral::operation::markreadonly` | "markreadonly" | `json::mark_document_readonly` |
| `apiliteral::operation::markdeleted` | "markdeleted" | `json::mark_document_deleted` |

## Performance Considerations

- **Exception handling overhead**: Each function has try/catch blocks, but since exceptions should be rare, the overhead is minimal
- **String copying**: Some functions copy strings unnecessarily; this could be optimized
- **JSON parsing**: The `readJsonData()` call parses JSON on every call to `json::` functions

## Error Handling Strategy

1. **Input validation**: JSON functions check for empty payloads before processing
2. **Exception capture**: All exceptions are caught and converted to error results
3. **Diagnostic preservation**: `::debug::current_exception_diagnostic_information()` captures full exception details including nested exceptions and stack traces (when available)

### Error Messages

The file uses these predefined error messages from `shared_literals.h`:

| Message | Meaning |
|---------|---------|
| `msg_EmptyFilter` | Returned when the JSON payload is empty |
| `msg_EmptyDocument` | Returned when the document content is empty (write operations) |

## Testing Notes

- **Unit tests:** `test/ConfigurationDB/dboperation_managedocument_t.cc`
- **Key test cases:**
  - Write then read document round-trip
  - Version searching
  - Entity addition and removal
  - Read-only and delete marking
  - Error handling for invalid inputs

## Maintenance Notes

- **TRACE_NAME macro**: Must be undefined and redefined to get proper trace context
- **JSON conversion helpers**: `json_db_to_gui` and `json_gui_to_db` are declared but their usage appears to be commented out
- **Consistent pattern**: When adding new operations, follow the existing wrapper pattern
- **Duplicate TLOG**: In `json::add_entity`, there are duplicate TLOG messages that could be cleaned up

### Code Quality Notes

1. The `json::mark_document_deleted` function has a commented-out empty check, which differs from the pattern used by other functions
2. Debug TLOG statements in `json::add_entity` have duplicate messages
3. Forward declarations for `json_db_to_gui` and `json_gui_to_db` exist but are not used in this file

## See Also

- [dboperation_managedocument.h.md](./dboperation_managedocument.h.md) - Header file with public API
- [detail_managedocument.cpp.md](./detail_managedocument.cpp.md) - Actual implementation logic
- [options_operations.h.md](./options_operations.h.md) - ManageDocumentOperation class
- [returned_result.h.md](../SharedCommon/returned_result.h.md) - result_t and Success/Failure helpers
- [configuraion_api_literals.h.md](../SharedCommon/configuraion_api_literals.h.md) - API operation name literals

---

**Documentation generated for artdaq-database ConfigurationDB module**
