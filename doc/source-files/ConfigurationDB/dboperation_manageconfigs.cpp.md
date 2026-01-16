# dboperation_manageconfigs.cpp

**Path:** `artdaq-database/ConfigurationDB/dboperation_manageconfigs.cpp`

**Implements:** [dboperation_manageconfigs.h](./dboperation_manageconfigs.h.md)

**Purpose:** Implements the public wrapper functions for global configuration management operations. These wrapper functions provide exception safety by catching all exceptions and converting them to `result_t` return values. The actual logic is delegated to `detail::` namespace functions.

## Implementation Overview

This file implements a thin wrapper layer for managing global configurations (compositions). The implementation:
1. Provides exception-safe wrappers for all public functions
2. Validates input parameters for JSON-based functions
3. Delegates to `detail::` namespace functions for actual implementation
4. Returns success/failure status via `result_t`

The pattern is consistent with other `dboperation_*.cpp` files:
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
| `artdaq-database/BasicTypes/basictypes.h` | Basic type wrappers |
| `artdaq-database/ConfigurationDB/dboperation_manageconfigs.h` | Header being implemented |
| `artdaq-database/ConfigurationDB/options_operations.h` | ManageDocumentOperation class |
| `artdaq-database/ConfigurationDB/shared_helper_functions.h` | Helper utilities |
| `artdaq-database/DataFormats/shared_literals.h` | Error message literals |
| `artdaq-database/SharedCommon/configuraion_api_literals.h` | API operation name literals |

## Key Algorithms

### Exception-Safe Wrapper Pattern

All public functions follow this pattern to ensure the `noexcept` guarantee:

**Steps:**
1. Enter try block
2. Create return value buffer (if needed)
3. Call corresponding `detail::` function
4. Wrap result in `Success()` and return
5. If any exception occurs, catch it and return `Failure()`

### JSON Payload Processing

For functions in the `json::` namespace:

**Steps:**
1. Validate payload is not empty (for some functions)
2. Create a `ManageDocumentOperation` object with appropriate operation name
3. Parse JSON payload into the options object via `readJsonData()`
4. Delegate to `detail::` implementation
5. Return result

### create_configuration Implementation

The `json::create_configuration` function differs from others:
- It passes the raw operations string directly to `detail::create_configuration`
- This allows batch processing of multiple assign operations
- The opts version is explicitly not implemented

## Internal Functions

### Forward Declarations in `detail` Namespace

The file declares (but does not define) the following internal functions:

#### `detail::find_configurations(options, returnValue) -> void`

**Brief:** Internal implementation of configuration search logic.

**Called by:** `opts::find_configurations()`, `json::find_configurations()`

**Purpose:** Queries the storage provider for matching global configurations.

---

#### `detail::configuration_composition(options, returnValue) -> void`

**Brief:** Internal implementation of composition retrieval.

**Called by:** `opts::configuration_composition()`, `json::configuration_composition()`

**Purpose:** Retrieves the complete member list for a global configuration.

---

#### `detail::assign_configuration(options, returnValue) -> void`

**Brief:** Internal implementation of configuration assignment.

**Called by:** `opts::assign_configuration()`, `json::assign_configuration()`

**Purpose:** Adds or updates a member in a global configuration.

---

#### `detail::remove_configuration(options, returnValue) -> void`

**Brief:** Internal implementation of configuration removal.

**Called by:** `opts::remove_configuration()`, `json::remove_configuration()`

**Purpose:** Removes a member from a global configuration.

---

#### `detail::create_configuration(operations, returnValue) -> void`

**Brief:** Internal implementation of batch configuration creation.

**Called by:** `json::create_configuration()`

**Purpose:** Creates a global configuration from a JSON array of operations.

**Note:** Unlike other detail functions, this takes a raw string instead of a ManageDocumentOperation object.

## Function Implementations

### opts::find_configurations

```cpp
result_t opts::find_configurations(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};
    detail::find_configurations(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Creates a local string buffer for the return value
- Delegates to detail function
- Returns the populated buffer on success

---

### opts::configuration_composition

```cpp
result_t opts::configuration_composition(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};
    detail::configuration_composition(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Returns JSON object containing composition members
- Read-only operation, thread-safe

---

### opts::assign_configuration

```cpp
result_t opts::assign_configuration(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};
    detail::assign_configuration(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Modifies global configuration by adding/updating a member
- Not thread-safe for concurrent modifications

---

### opts::remove_configuration

```cpp
result_t opts::remove_configuration(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};
    detail::remove_configuration(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Removes a member from the global configuration
- Not thread-safe for concurrent modifications

---

### opts::create_configuration

```cpp
result_t opts::create_configuration(ManageDocumentOperation const& /*options*/) noexcept {
  return Failure("Not Implemented");
}
```

**Implementation notes:**
- Explicitly not implemented
- Parameters are intentionally unnamed
- Users should use `json::create_configuration()` instead

---

### json::find_configurations

```cpp
result_t json::find_configurations(std::string const& query_payload) noexcept {
  try {
    auto options = ManageDocumentOperation{apiliteral::operation::findconfigs};
    options.readJsonData({query_payload});
    auto returnValue = std::string{};
    detail::find_configurations(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Does NOT validate for empty payload (unlike assign/remove)
- Uses `apiliteral::operation::findconfigs` operation name

---

### json::configuration_composition

```cpp
result_t json::configuration_composition(std::string const& query_payload) noexcept {
  try {
    auto options = ManageDocumentOperation{apiliteral::operation::confcomposition};
    options.readJsonData({query_payload});
    auto returnValue = std::string{};
    detail::configuration_composition(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Does NOT validate for empty payload
- Uses `apiliteral::operation::confcomposition` operation name

---

### json::assign_configuration

```cpp
result_t json::assign_configuration(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }
    auto options = ManageDocumentOperation{apiliteral::operation::assignconfig};
    options.readJsonData({query_payload});
    auto returnValue = std::string{};
    detail::assign_configuration(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Validates that payload is not empty
- Uses `apiliteral::operation::assignconfig` operation name

---

### json::remove_configuration

```cpp
result_t json::remove_configuration(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }
    auto options = ManageDocumentOperation{apiliteral::operation::removeconfig};
    options.readJsonData({query_payload});
    auto returnValue = std::string{};
    detail::remove_configuration(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Validates that payload is not empty
- Uses `apiliteral::operation::removeconfig` operation name

---

### json::create_configuration

```cpp
result_t json::create_configuration(std::string const& operations) noexcept {
  try {
    if (operations.empty()) {
      return Failure(msg_EmptyFilter);
    }
    auto returnValue = std::string{};
    detail::create_configuration(operations, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Unlike other JSON functions, passes raw string directly to detail
- Does not create ManageDocumentOperation object
- Allows batch processing of multiple operations

---

### debug::ManageConfigs

```cpp
void dbcfg::debug::ManageConfigs() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);
  dbcfg::debug::detail::ManageConfigs();
  TLOG(10) << "artdaq::database::configuration::ManageConfigs trace_enable";
}
```

**Implementation notes:**
- Enables all TRACE levels
- Configures memory and slow modes
- Calls detail namespace debug function
- Logs confirmation message

## API Operation Names

The file uses these operation name literals from `configuraion_api_literals.h`:

| Literal | Value | Used by |
|---------|-------|---------|
| `apiliteral::operation::findconfigs` | "findconfigs" | `json::find_configurations` |
| `apiliteral::operation::confcomposition` | "confcomposition" | `json::configuration_composition` |
| `apiliteral::operation::assignconfig` | "assignconfig" | `json::assign_configuration` |
| `apiliteral::operation::removeconfig` | "removeconfig" | `json::remove_configuration` |

## Performance Considerations

- **Exception handling overhead**: Minimal since exceptions should be rare
- **JSON parsing**: Each JSON namespace function parses the payload on every call
- **Composition queries**: May be slow for configurations with many members

## Error Handling Strategy

1. **Input validation**: JSON functions check for empty payloads (for assign/remove/create)
2. **Exception capture**: All exceptions are caught and converted to error results
3. **Diagnostic preservation**: Full exception details are captured via `::debug::current_exception_diagnostic_information()`

### Error Messages

The file uses these predefined error messages:

| Message | Meaning |
|---------|---------|
| `msg_EmptyFilter` | Returned when required JSON payload is empty |
| `"Not Implemented"` | Returned by `opts::create_configuration` |

### Validation Patterns

Different functions have different validation:
- **find_configurations, configuration_composition**: No empty check
- **assign_configuration, remove_configuration, create_configuration**: Empty check required

## Testing Notes

- **Unit tests:** `test/ConfigurationDB/dboperation_manageconfigs_t.cc`
- **Key test cases:**
  - Find configurations with wildcards
  - Create multi-member configurations
  - Assign and remove individual members
  - Composition retrieval
  - Error handling for empty payloads

## Maintenance Notes

- **TRACE_NAME macro**: Defined as "dboperation_manageconfigs.cpp"
- **opts::create_configuration**: Intentionally not implemented; document if this changes
- **JSON conversion helpers**: `json_db_to_gui` and `json_gui_to_db` are declared but may be unused
- **Validation inconsistency**: `find_configurations` and `configuration_composition` don't check for empty payload

### Code Quality Notes

1. The `opts::create_configuration` function explicitly returns "Not Implemented" - this is intentional
2. Unlike document operations, find and composition queries don't validate empty payloads
3. The `json::create_configuration` passes raw JSON directly to detail, unlike other operations

## See Also

- [dboperation_manageconfigs.h.md](./dboperation_manageconfigs.h.md) - Header file with public API
- [detail_manageconfigs.cpp.md](./detail_manageconfigs.cpp.md) - Actual implementation logic
- [options_operations.h.md](./options_operations.h.md) - ManageDocumentOperation class
- [configurationdbifc.h.md](./configurationdbifc.h.md) - High-level API
- [returned_result.h.md](../SharedCommon/returned_result.h.md) - result_t and Success/Failure helpers

---

**Documentation generated for artdaq-database ConfigurationDB module**
