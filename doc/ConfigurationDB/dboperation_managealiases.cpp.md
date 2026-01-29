# dboperation_managealiases.cpp

**Path:** `artdaq-database/ConfigurationDB/dboperation_managealiases.cpp`

**Implements:** [dboperation_managealiases.h](./dboperation_managealiases.h.md)

**Purpose:** Implements the public wrapper functions for alias management operations. These wrapper functions provide exception safety by catching all exceptions and converting them to `result_t` return values. The actual logic is delegated to `detail::` namespace functions.

## Implementation Overview

This file implements a thin wrapper layer for managing version aliases and configuration aliases. The implementation:
1. Provides exception-safe wrappers for all public functions
2. Validates input parameters for JSON-based functions
3. Delegates to `detail::` namespace functions for actual implementation
4. Returns success/failure status via `result_t`

**Note:** The current implementation primarily includes the `json::` namespace functions. The `opts::` namespace functions declared in the header are not implemented in this file.

## Thread Safety

- **Thread-safe:** Conditional
- **Concurrent access:** Each function call is independent; the wrapper layer itself is thread-safe, but the underlying detail functions may not be
- **Locking:** No internal locking in the wrapper layer

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | Common definitions and TRACE macros |
| `artdaq-database/BasicTypes/basictypes.h` | Basic type wrappers |
| `artdaq-database/ConfigurationDB/dboperation_managealiases.h` | Header being implemented |
| `artdaq-database/ConfigurationDB/options_operations.h` | ManageAliasesOperation class |
| `artdaq-database/ConfigurationDB/shared_helper_functions.h` | Helper utilities |
| `artdaq-database/DataFormats/shared_literals.h` | Error message literals |
| `artdaq-database/SharedCommon/configuraion_api_literals.h` | API operation name literals |

## Key Algorithms

### Exception-Safe Wrapper Pattern

All public functions follow this pattern to ensure the `noexcept` guarantee:

**Steps:**
1. Enter try block
2. Validate that payload is not empty
3. Create a `ManageAliasesOperation` object with appropriate operation name
4. Parse JSON payload into the options object via `readJsonData()`
5. Call corresponding `detail::` function
6. Wrap result in `Success()` and return
7. If any exception occurs, catch it and return `Failure()`

### JSON Payload Processing

```cpp
result_t json::operation(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }
    auto options = ManageAliasesOperation{apiliteral::operation::name};
    options.readJsonData({query_payload});
    auto returnValue = std::string{};
    detail::operation(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

### ManageAliasesOperation vs ManageDocumentOperation

This file uses `ManageAliasesOperation` instead of `ManageDocumentOperation` for the JSON functions. The `ManageAliasesOperation` class is specialized for alias operations and may have different field handling.

## Internal Functions

### Forward Declarations in `detail` Namespace

The file declares (but does not define) the following internal functions:

#### `detail::add_version_alias(options, conf) -> void`

**Brief:** Internal implementation of version alias creation.

**Called by:** `json::add_version_alias()`

**Purpose:** Creates a new alias pointing to the specified version.

---

#### `detail::remove_version_alias(options, conf) -> void`

**Brief:** Internal implementation of version alias removal.

**Called by:** `json::remove_version_alias()`

**Purpose:** Removes an existing version alias.

---

#### `detail::find_version_aliases(options, conf) -> void`

**Brief:** Internal implementation of version alias search.

**Called by:** `json::find_version_aliases()`

**Purpose:** Queries the storage provider for matching aliases.

## Function Implementations

### json::add_version_alias

```cpp
result_t json::add_version_alias(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }
    auto options = ManageAliasesOperation{apiliteral::operation::addversionalias};
    options.readJsonData({query_payload});
    auto returnValue = std::string{};
    detail::add_version_alias(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Uses `ManageAliasesOperation` instead of `ManageDocumentOperation`
- Validates that payload is not empty
- Uses `apiliteral::operation::addversionalias` operation name

---

### json::remove_version_alias

```cpp
result_t json::remove_version_alias(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }
    auto options = ManageAliasesOperation{apiliteral::operation::rmversionalias};
    options.readJsonData({query_payload});
    auto returnValue = std::string{};
    detail::remove_version_alias(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Uses `apiliteral::operation::rmversionalias` operation name

---

### json::find_version_aliases

```cpp
result_t json::find_version_aliases(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }
    auto options = ManageAliasesOperation{apiliteral::operation::rmversionalias};
    options.readJsonData({query_payload});
    auto returnValue = std::string{};
    detail::find_version_aliases(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- **Potential bug**: Uses `rmversionalias` operation name instead of expected `findversionalias`
- This may be intentional for internal dispatch, but should be verified
- The operation name is used for logging/tracking, actual behavior is determined by the detail function called

---

### debug::ManageAliases

```cpp
void dbcfg::debug::ManageAliases() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);
  dbcfg::debug::detail::ManageAliases();
  TLOG(10) << "artdaq::database::configuration::ManageAliases trace_enable";
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
| `apiliteral::operation::addversionalias` | "addversionalias" | `json::add_version_alias` |
| `apiliteral::operation::rmversionalias` | "rmversionalias" | `json::remove_version_alias`, `json::find_version_aliases` |

## Missing Implementations

The following functions are declared in the header but not implemented in this file:

**opts namespace:**
- `opts::add_version_alias()`
- `opts::remove_version_alias()`
- `opts::find_version_aliases()`
- `opts::add_configuration_alias()`
- `opts::remove_configuration_alias()`

**json namespace:**
- `json::add_configuration_alias()`
- `json::remove_configuration_alias()`

These may be implemented in a separate file or may be pending implementation.

## Performance Considerations

- **Exception handling overhead**: Minimal since exceptions should be rare
- **JSON parsing**: Each function parses the JSON payload on every call
- **Alias lookups**: May require index scans depending on storage provider

## Error Handling Strategy

1. **Input validation**: All JSON functions check for empty payloads
2. **Exception capture**: All exceptions are caught and converted to error results
3. **Diagnostic preservation**: Full exception details are captured via `::debug::current_exception_diagnostic_information()`

### Error Messages

The file uses these predefined error messages:

| Message | Meaning |
|---------|---------|
| `msg_EmptyFilter` | Returned when the JSON payload is empty |

## Testing Notes

- **Unit tests:** `test/ConfigurationDB/dboperation_managealiases_t.cc`
- **Key test cases:**
  - Add and find version aliases
  - Remove version aliases
  - Verify alias uniqueness
  - Error handling for non-existent versions
  - Error handling for empty payloads

## Maintenance Notes

- **TRACE_NAME macro**: Defined as "dboperation_managealiases.cpp"
- **ManageAliasesOperation**: Uses a specialized operation class for alias management
- **Operation name in find_version_aliases**: Uses `rmversionalias` - verify this is intentional or a bug
- **Missing implementations**: Several opts namespace functions are not implemented
- **Configuration aliases**: Functions for configuration aliases are declared but not implemented

### Code Quality Notes

1. The `json::find_version_aliases` function uses the wrong operation name (`rmversionalias` instead of expected `findversionalias`)
2. Only three of the declared JSON functions are implemented
3. None of the opts namespace functions are implemented
4. Configuration alias functions are completely missing

### Recommended Fixes

If the operation name in `find_version_aliases` is incorrect, it should be changed to:
```cpp
auto options = ManageAliasesOperation{apiliteral::operation::findversionalias};
```

Or a new literal should be added if one doesn't exist.

## See Also

- [dboperation_managealiases.h.md](./dboperation_managealiases.h.md) - Header file with public API
- [detail_managealiases.cpp.md](./detail_managealiases.cpp.md) - Actual implementation logic
- [options_operations.h.md](./options_operations.h.md) - ManageAliasesOperation class
- [configurationdbifc.h.md](./configurationdbifc.h.md) - High-level API
- [returned_result.h.md](../SharedCommon/returned_result.h.md) - result_t and Success/Failure helpers

---

**Documentation generated for artdaq-database ConfigurationDB module**
