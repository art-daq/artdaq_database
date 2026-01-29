# dboperation_findcompositions.cpp

**Path:** `artdaq-database/ConfigurationDB/dboperation_findcompositions.cpp`

**Implements:** [dboperation_findcompositions.h](./dboperation_findcompositions.h.md)

**Purpose:** Implements the public API functions for finding global configuration compositions that contain a specific configuration version. This file provides the thin wrapper layer that handles exception safety and delegates to the detail implementation.

---

## Implementation Overview

This file implements the two-namespace API pattern used throughout ConfigurationDB:

1. **opts namespace**: Takes strongly-typed `ManageDocumentOperation` objects
2. **json namespace**: Takes JSON string payloads for REST API compatibility

Both implementations follow a simple delegation pattern - validate inputs, call the detail implementation, and wrap any exceptions in result types.

---

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | Common utilities and TRACE logging |
| `artdaq-database/ConfigurationDB/dboperation_findcompositions.h` | Header being implemented |
| `artdaq-database/ConfigurationDB/options_operations.h` | ManageDocumentOperation class |
| `artdaq-database/BasicTypes/basictypes.h` | JsonData type for JSON parsing |
| `artdaq-database/DataFormats/shared_literals.h` | JSON field name literals |
| `artdaq-database/JsonDocument/JSONDocument.h` | JSON document manipulation |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | JSON document construction |

---

## TRACE Configuration

```cpp
#define TRACE_NAME "dboperation_findcompositions.cpp"
```

**Brief:** Sets the TRACE logging identifier for this translation unit to enable targeted debug output filtering.

---

## Forward Declarations

### `detail::find_compositions_containing`

```cpp
namespace detail {
void find_compositions_containing(ManageDocumentOperation const&, std::string&);
}
```

**Brief:** Forward declaration of the detail implementation function. The actual implementation resides in `detail_findcompositions.cpp`.

---

## Function Implementations

### `opts::find_compositions_containing`

```cpp
result_t opts::find_compositions_containing(ManageDocumentOperation const& options, std::string& results) noexcept
```

**Brief:** Implements the options-based API by delegating to the detail implementation with full exception handling.

**Implementation Steps:**
1. Create local return value string
2. Call `detail::find_compositions_containing()` with options
3. Swap results into output parameter on success
4. Return Success with results string
5. On any exception, return Failure with diagnostic information

**Error Handling Strategy:**
- Uses try-catch to guarantee noexcept contract
- Captures exception diagnostic information using `debug::current_exception_diagnostic_information()`
- Returns structured Failure result on any exception

**Called by:** `json::find_compositions_containing()`, `ConfigurationInterface::findGlobalConfigurationsContaining()`

---

### `json::find_compositions_containing`

```cpp
result_t cf::json::find_compositions_containing(std::string const& task_payload) noexcept
```

**Brief:** Implements the JSON-based API by parsing the payload and delegating to the options-based implementation.

**Implementation Steps:**
1. Confirm task_payload is non-empty (debug assertion)
2. Parse JSON payload into `JsonData` object
3. Construct `ManageDocumentOperation` from JSON data
4. Call `opts::find_compositions_containing()` with parsed options
5. Return success result with search results or propagate failure

**Error Handling Strategy:**
- Uses try-catch for noexcept guarantee
- JSON parsing errors caught and returned as failures
- Invalid payload structure caught and returned as failures

**Called by:** REST API handlers, CLI tools

---

### `debug::FindCompositions`

```cpp
void cf::debug::FindCompositions() {
  TLOG(10) << "artdaq::database::configuration::debug::FindCompositions trace_enable";
}
```

**Brief:** Enables TRACE logging for the findcompositions module at the public API level. Call this function to activate trace messages at level 10.

**Usage:**
```cpp
// Enable tracing for findcompositions operations
artdaq::database::configuration::debug::FindCompositions();
```

---

### `debug::detail::FindCompositions`

```cpp
void cf::debug::detail::FindCompositions() {
  TLOG(11) << "artdaq::database::configuration::debug::detail::FindCompositions trace_enable";
}
```

**Brief:** Enables TRACE logging for the findcompositions detail implementation at level 11. Provides more verbose tracing than the public API level.

**Usage:**
```cpp
// Enable detailed tracing
artdaq::database::configuration::debug::detail::FindCompositions();
```

---

## Key Algorithms

### JSON Payload Processing

**Steps:**
1. Validate payload is non-empty using `confirm()` assertion
2. Wrap raw JSON string in `basictypes::JsonData`
3. Pass `JsonData` to `ManageDocumentOperation` constructor
4. Constructor parses JSON and populates operation fields

**Why this approach:** The `ManageDocumentOperation` class handles all JSON parsing internally, providing consistent error handling and field validation across all JSON-based APIs.

---

## Performance Considerations

- **Memory:** Minimal allocation - uses move semantics via `swap()` for result strings
- **No copying:** Results are swapped, not copied, avoiding large string copies
- **Exception handling:** All exceptions caught and converted to result types

---

## Error Handling Strategy

All public functions are marked `noexcept` and use the following pattern:

```cpp
result_t function(...) noexcept {
  try {
    // Implementation
    return Success(result);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

This ensures:
- No exceptions escape to callers
- All errors are reported via the result type
- Full diagnostic information is captured for debugging

---

## Testing Notes

- **Unit tests:** `test/ConfigurationDB/findcompositions_t.cc`
- **Key test cases:**
  - Valid search with matching compositions
  - Valid search with no matches (empty result)
  - Invalid provider name
  - Malformed JSON payload
  - Empty payload handling

---

## Maintenance Notes

The separation of `dboperation_*.cpp` (public API) and `detail_*.cpp` (implementation) follows the ConfigurationDB convention for:
- Clear separation of concerns
- Independent testing of API layer vs implementation
- Ability to change implementation without affecting API

---

## See Also

- [dboperation_findcompositions.h.md](./dboperation_findcompositions.h.md) - Header documentation
- [detail_findcompositions.cpp.md](./detail_findcompositions.cpp.md) - Implementation details
- [options_operations.h.md](./options_operations.h.md) - ManageDocumentOperation class

---

**Documentation generated for artdaq-database ConfigurationDB module**
