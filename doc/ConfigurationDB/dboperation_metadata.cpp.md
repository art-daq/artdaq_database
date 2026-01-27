# dboperation_metadata.cpp

**Path:** `artdaq-database/ConfigurationDB/dboperation_metadata.cpp`

**Implements:** [dboperation_metadata.h](./dboperation_metadata.h.md)

**Purpose:** Implements the public wrapper functions for database metadata operations. These wrapper functions provide exception safety by catching all exceptions and converting them to `result_t` return values. The actual logic is delegated to `detail::` namespace functions in `detail_metadata.cpp`.

## Implementation Overview

This file implements a thin wrapper layer for retrieving database metadata. The implementation follows the standard ConfigurationDB pattern:

1. **Exception-safe wrappers** for all public functions using try-catch blocks
2. **Input validation** for JSON-based functions (empty payload checks)
3. **Delegation to `detail::` namespace** for actual implementation
4. **Consistent `result_t` returns** with success/failure status

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

- **Thread-safe:** Yes for all public functions
- **Concurrent access:** Multiple threads can safely call metadata functions concurrently
- **Locking:** No internal locking; relies on thread-safe detail implementations

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | Common definitions and TRACE macros |
| `artdaq-database/BasicTypes/basictypes.h` | Basic type wrappers (JsonData, etc.) |
| `artdaq-database/ConfigurationDB/dboperation_metadata.h` | Header being implemented |
| `artdaq-database/ConfigurationDB/options_operations.h` | ManageDocumentOperation class |
| `artdaq-database/ConfigurationDB/shared_helper_functions.h` | Helper utilities |
| `artdaq-database/DataFormats/shared_literals.h` | Error message literals (msg_EmptyFilter) |

## Internal Functions

### Forward Declarations in `detail` Namespace

The file declares (but does not define) the following internal functions implemented in `detail_metadata.cpp`:

---

#### `detail::list_databases(options, returnValue) -> void`

**Brief:** Internal implementation that queries the storage provider for available databases.

**Called by:** `opts::list_databases()`, `json::list_databases()`

**Purpose:** Dispatches to the appropriate storage provider and populates the return value with a JSON array of database names.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When storage provider operations fail |
| Various | Provider-specific exceptions |

---

#### `detail::read_dbinfo(options, returnValue) -> void`

**Brief:** Internal implementation that retrieves database metadata from the storage provider.

**Called by:** `opts::read_dbinfo()`, `json::read_dbinfo()`

**Purpose:** Queries the storage provider for database statistics and returns them as JSON.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When storage provider operations fail |
| Various | Provider-specific exceptions |

---

#### `detail::list_collections(options, returnValue) -> void`

**Brief:** Internal implementation that queries the storage provider for available collections.

**Called by:** `opts::list_collections()`, `json::list_collections()`

**Purpose:** Dispatches to the appropriate storage provider and populates the return value with collection names.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When storage provider operations fail |
| Various | Provider-specific exceptions |

## Key Algorithms

### Exception-Safe Wrapper Pattern

All public functions follow this pattern to ensure the `noexcept` guarantee:

**Steps:**
1. Enter try block
2. Create return value buffer (if needed)
3. Call corresponding `detail::` function
4. Wrap result in `Success()` and return
5. If any exception occurs, catch it and return `Failure()`

**Why this approach:**
- Ensures `noexcept` guarantee at the API boundary
- Preserves complete exception diagnostic information for debugging
- Provides consistent error handling across all API functions

### JSON Payload Processing

For functions in the `json::` namespace:

**Steps:**
1. Validate that payload is not empty (return `Failure(msg_EmptyFilter)` if empty)
2. Create a `ManageDocumentOperation` object with appropriate operation name
3. Parse JSON payload into the options object via `readJsonData()`
4. Call corresponding `detail::` implementation
5. Return result wrapped in `Success()` or `Failure()`

## Public Function Implementations

### `opts::list_databases(options, conf) -> result_t`

**Brief:** Wrapper that invokes the detail implementation for listing databases and handles exceptions.

**Implementation:**
```cpp
result_t opts::list_databases(ManageDocumentOperation const& options, std::string& conf) noexcept {
  try {
    detail::list_databases(options, conf);
    return Success(conf);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Takes `conf` by reference for output
- Delegates directly to detail function
- Returns the populated `conf` on success

---

### `opts::read_dbinfo(options, conf) -> result_t`

**Brief:** Wrapper that invokes the detail implementation for reading database info and handles exceptions.

**Implementation:**
```cpp
result_t opts::read_dbinfo(ManageDocumentOperation const& options, std::string& conf) noexcept {
  try {
    detail::read_dbinfo(options, conf);
    return Success(conf);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Similar pattern to list_databases
- Takes `conf` by reference for output

---

### `opts::list_collections(options) -> result_t`

**Brief:** Wrapper that invokes the detail implementation for listing collections and handles exceptions.

**Implementation:**
```cpp
result_t opts::list_collections(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};
    detail::list_collections(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Creates local buffer for return value (different from list_databases/read_dbinfo)
- Does not take output parameter by reference
- Result is contained entirely in the returned `result_t`

---

### `json::list_databases(query_payload) -> result_t`

**Brief:** JSON-based entry point for listing databases that parses the payload and delegates to detail implementation.

**Implementation:**
```cpp
result_t json::list_databases(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }
    auto options = ManageDocumentOperation{apiliteral::operation::listdatabases};
    options.readJsonData({query_payload});
    auto returnValue = std::string{};
    detail::list_databases(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Validates payload is not empty
- Uses `apiliteral::operation::listdatabases` operation name

---

### `json::read_dbinfo(query_payload) -> result_t`

**Brief:** JSON-based entry point for reading database info that parses the payload and delegates to detail implementation.

**Implementation:**
```cpp
result_t json::read_dbinfo(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }
    auto options = ManageDocumentOperation{apiliteral::operation::readdbinfo};
    options.readJsonData({query_payload});
    auto returnValue = std::string{};
    detail::read_dbinfo(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Uses `apiliteral::operation::readdbinfo` operation name

---

### `json::list_collections(query_payload) -> result_t`

**Brief:** JSON-based entry point for listing collections that parses the payload and delegates to detail implementation.

**Implementation:**
```cpp
result_t json::list_collections(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }
    auto options = ManageDocumentOperation{apiliteral::operation::listcollections};
    options.readJsonData({query_payload});
    auto returnValue = std::string{};
    detail::list_collections(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation notes:**
- Uses `apiliteral::operation::listcollections` operation name

---

### `debug::Metadata() -> void`

**Brief:** Enables TRACE logging for the metadata module and its detail implementation.

**Implementation:**
```cpp
void dbcfg::debug::Metadata() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);
  dbcfg::debug::detail::Metadata();
  TLOG(10) << "artdaq::database::configuration::Metadata trace_enable";
}
```

**Implementation notes:**
- Sets TRACE_NAME to "dboperation_metadata.cpp"
- Enables all TRACE levels (0xFFFFFFFFFFFFFFFFLL masks)
- Configures memory and slow modes
- Calls detail namespace debug function
- Logs confirmation message at level 10

## API Operation Names

The file uses these operation name literals from `apiliteral::operation`:

| Literal | Value | Used By |
|---------|-------|---------|
| `listdatabases` | "listdatabases" | `json::list_databases()` |
| `readdbinfo` | "readdbinfo" | `json::read_dbinfo()` |
| `listcollections` | "listcollections" | `json::list_collections()` |

## Signature Differences

Note the signature differences between opts functions:

| Function | Signature | Notes |
|----------|-----------|-------|
| `opts::list_databases` | `(options, conf&)` | Takes output by reference |
| `opts::read_dbinfo` | `(options, conf&)` | Takes output by reference |
| `opts::list_collections` | `(options)` | Returns output in result only |

This inconsistency may be historical or intentional for API compatibility.

## Performance Considerations

- **Exception handling overhead**: Minimal since exceptions should be rare
- **JSON parsing**: Each JSON namespace function parses the payload on every call
- **Collection listing**: May be slow for databases with many collections
- **Database info**: Performance depends on storage provider implementation

## Error Handling Strategy

1. **Input validation**: JSON functions check for empty payloads and return `Failure(msg_EmptyFilter)`
2. **Exception capture**: All exceptions are caught via catch-all (`catch (...)`)
3. **Diagnostic preservation**: Full exception details are captured via `::debug::current_exception_diagnostic_information()`
4. **Consistent returns**: All functions return `result_t` with clear success/failure indication

### Error Messages

| Error | Condition |
|-------|-----------|
| `msg_EmptyFilter` | JSON payload is empty |
| Exception diagnostic | Any exception thrown during operation |

## Testing Notes

- **Unit tests:** `test/ConfigurationDB/Metadata_t.cc`
- **Key test cases:**
  - List available databases
  - List collections in a database
  - Read database info
  - Error handling for empty payloads
  - Provider-specific behavior (FileSystemDB vs MongoDB)

## Maintenance Notes

- **TRACE_NAME macro**: Defined as "dboperation_metadata.cpp" - used for logging identification
- **Signature inconsistency**: `list_collections` differs from `list_databases` and `read_dbinfo` in taking output by reference
- **Read-only operations**: All metadata operations are read-only; no write safety concerns
- **No configuraion_api_literals.h**: This file does not include that header; literals come from shared_literals.h

## See Also

- [dboperation_metadata.h.md](./dboperation_metadata.h.md) - Header file with public API
- [detail_metadata.cpp.md](./detail_metadata.cpp.md) - Actual implementation logic
- [options_operation_managedocument.h.md](./options_operation_managedocument.h.md) - ManageDocumentOperation class
- [configurationdbifc.h.md](./configurationdbifc.h.md) - High-level API

---

**Documentation generated for artdaq-database ConfigurationDB module**
