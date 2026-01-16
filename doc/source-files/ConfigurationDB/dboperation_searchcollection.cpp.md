# dboperation_searchcollection.cpp

**Path:** `artdaq-database/ConfigurationDB/dboperation_searchcollection.cpp`

**Implements:** [dboperation_searchcollection.h](./dboperation_searchcollection.h.md)

**Purpose:** Implements the public API functions for searching documents within collections. This file provides the thin wrapper layer that catches exceptions and delegates to the internal detail implementation, converting exceptions to result_t failures.

## Implementation Overview

This file implements the two-namespace pattern for search operations:

1. **opts::search_collection**: Accepts a `ManageDocumentOperation` object and delegates to `detail::search_collection`
2. **json::search_collection**: Parses a JSON payload, creates a `ManageDocumentOperation`, then delegates to the detail implementation

Both functions wrap the detail implementation in try-catch blocks, ensuring all exceptions are converted to `result_t` failures with diagnostic information.

## Thread Safety

- **Thread-safe:** Conditional
- **Concurrent access:** Multiple threads can safely call search functions with different options objects
- **Locking:** No internal locking; relies on thread-safe detail and provider implementations

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | Common definitions and TRACE configuration |
| `artdaq-database/BasicTypes/basictypes.h` | Basic type definitions including JsonData |
| `artdaq-database/ConfigurationDB/dboperation_searchcollection.h` | Header for this implementation |
| `artdaq-database/ConfigurationDB/options_operations.h` | ManageDocumentOperation class |
| `artdaq-database/ConfigurationDB/shared_helper_functions.h` | Shared utility functions |
| `artdaq-database/DataFormats/shared_literals.h` | String literal constants including msg_EmptyFilter |

## Internal Functions

### Namespace: `detail`

---

#### `search_collection(options, results) -> void`

**Brief:** Internal implementation that performs the actual collection search by dispatching to the appropriate storage provider.

**Called by:** `opts::search_collection()`, `json::search_collection()`

**Purpose:** This function is declared in the detail namespace and implemented in `detail_searchcollection.cpp`. It contains the actual search logic including provider dispatch and result formatting.

**Parameters:**
- `options` - `ManageDocumentOperation const&` - Search configuration with collection, filter, and format settings
- `results` - `std::string&` - Output buffer for search results

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When storage provider operations fail |
| Various | Provider-specific exceptions |

---

## Public Function Implementations

### `opts::search_collection(options, results) -> result_t`

**Brief:** Wrapper that invokes the detail implementation and handles exceptions, returning results in the result_t format.

**Implementation:**
```cpp
result_t opts::search_collection(ManageDocumentOperation const& options, std::string& results) noexcept {
  try {
    detail::search_collection(options, results);
    return Success(results);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation Details:**
1. Calls `detail::search_collection(options, results)` in a try block
2. On success, returns `Success(results)` with the search results
3. On exception, catches all exceptions and returns `Failure()` with diagnostic information from `debug::current_exception_diagnostic_information()`

**Error Handling Strategy:** All exceptions are caught and converted to failure results. The exception diagnostic information is preserved in the failure message for debugging.

---

### `json::search_collection(query_payload) -> result_t`

**Brief:** JSON-based entry point that parses the query payload and executes a collection search.

**Implementation:**
```cpp
result_t json::search_collection(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }

    auto options = ManageDocumentOperation{apiliteral::operation::listdatabases};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::search_collection(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Implementation Details:**
1. Validates that `query_payload` is not empty; returns `Failure(msg_EmptyFilter)` if empty
2. Creates a `ManageDocumentOperation` with the `listdatabases` operation type
3. Parses the JSON payload into the options object via `readJsonData()`
4. Calls `detail::search_collection()` with the configured options
5. Returns `Success(returnValue)` on success or `Failure()` with diagnostics on exception

**Validation:**
- Empty payload check prevents unnecessary processing

**Note:** The `listdatabases` operation type used in `json::search_collection` is intentional for backward compatibility.

---

### `debug::SearchCollection() -> void`

**Brief:** Enables TRACE logging for the search collection module and its detail implementation.

**Implementation:**
```cpp
void dbcfg::debug::SearchCollection() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  dbcfg::debug::detail::SearchCollection();
  TLOG(10) << "artdaq::database::configuration::SearchCollection trace_enable";
}
```

**Implementation Details:**
1. Sets the TRACE name to "dboperation_searchcollection.cpp"
2. Enables all trace levels with `lvlset` (0xFFFFFFFFFFFFFFFFLL)
3. Configures trace modes (modeM and modeS)
4. Calls `detail::SearchCollection()` to enable logging in the detail layer
5. Logs an initialization message at level 10

**Trace Configuration:**
- Uses the high-performance TRACE logging system
- Enables full trace level bitmask (0xFFFFFFFFFFFFFFFFLL)

---

## Key Algorithms

### Exception-to-Result Conversion Pattern

All public functions follow this pattern:

```cpp
result_t function(args) noexcept {
  try {
    // Perform operation
    detail::implementation(args);
    return Success(result);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

**Why this approach:**
- Ensures `noexcept` guarantee at the API boundary
- Preserves complete exception diagnostic information for debugging
- Provides consistent error handling across all API functions

---

## Performance Considerations

- **Minimal overhead**: The wrapper layer adds minimal overhead (try-catch block)
- **Single dispatch**: Options parsing happens once at the json:: level
- **No copying**: Results are passed by reference
- **Provider-dependent**: Actual search performance depends on the storage provider

## Error Handling Strategy

1. **Empty payload validation**: `json::search_collection` validates input before processing
2. **Exception capture**: All exceptions are caught at the API boundary
3. **Diagnostic preservation**: `current_exception_diagnostic_information()` captures full exception details including nested exceptions
4. **No partial results**: On failure, the result_t contains only the error message

### Error Messages

| Error | Condition |
|-------|-----------|
| `msg_EmptyFilter` | JSON payload is empty |
| Exception diagnostic | Any exception thrown during operation |

## Testing Notes

- **Unit tests:** `test/ConfigurationDB/SearchCollection_t.cc`
- **Key test cases:**
  - Empty payload handling
  - Valid search with matching results
  - Search with no matching results
  - Provider dispatch verification
  - Error handling for invalid filters

## Maintenance Notes

- **TRACE_NAME macro**: Defined as "dboperation_searchcollection.cpp"
- The detail implementation is in a separate file (`detail_searchcollection.cpp`) to allow independent modification
- Trace levels should remain consistent with other dboperation_*.cpp files
- The `listdatabases` operation type used in `json::search_collection` is intentional for backward compatibility

## See Also

- [dboperation_searchcollection.h.md](./dboperation_searchcollection.h.md) - Header file
- [detail_searchcollection.cpp.md](./detail_searchcollection.cpp.md) - Internal implementation
- [options_operation_managedocument.h.md](./options_operation_managedocument.h.md) - ManageDocumentOperation class
- [returned_result.h.md](../SharedCommon/returned_result.h.md) - Result type definition

---

**Documentation generated for artdaq-database ConfigurationDB module**
