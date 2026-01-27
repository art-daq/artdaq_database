# provider_ucondb.cpp

**Path:** `artdaq-database/StorageProviders/UconDB/provider_ucondb.cpp`

**Implements:** [provider_ucondb.h](./provider_ucondb.h.md)

**Purpose:** Implements query-related template specializations for the UconDB storage provider. Currently, most query operations are not implemented and throw `runtime_error` as placeholders, since UconDB primarily supports basic read/write operations rather than complex queries.

## Implementation Overview

This file provides template specializations of `StorageProvider<JSONDocument, UconDB>` for various query operations. The UconDB provider is designed for simple key-value storage of configurations via REST API, so many of the advanced query operations inherited from the `StorageProvider` interface are intentionally unimplemented.

**Note:** The primary read/write operations (`readDocument`, `writeDocument`) are implemented in `provider_ucondb_readwrite.cpp`.

## Key Algorithms

### Query Operation Pattern

All query methods follow the same pattern:

**Steps:**
1. Validate that the query payload is not empty using `confirm(!query_payload.empty())`
2. Initialize an empty return collection
3. Log the operation and parameters via TLOG at the appropriate trace level
4. Throw `runtime_error("UconDB")` with "not implemented" message

**Why this approach:** UconDB is a simple conditions database that does not support the full query semantics of MongoDB. These operations are stubbed out to maintain interface compatibility while clearly indicating they are not available.

## Template Specializations

All specializations follow the pattern:
```cpp
template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, UconDB>::methodName(JSONDocument const& payload)
```

### `findConfigurations(JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Placeholder for finding configurations matching a query. Currently unimplemented.

**Parameters:**
- `query_payload` - JSON document containing query criteria

**Preconditions:**
- `query_payload` must not be empty

**Returns:** Never returns (always throws)

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error("UconDB")` | Always (operation not implemented) |

**TLOG Level:** 15

---

### `configurationComposition(JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Placeholder for retrieving configuration composition. Currently unimplemented.

**Parameters:**
- `query_payload` - JSON document containing query criteria

**Preconditions:**
- `query_payload` must not be empty

**Returns:** Never returns (always throws)

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error("UconDB")` | Always (operation not implemented) |

**TLOG Level:** 16

---

### `findVersions(JSONDocument const& filter) -> std::vector<JSONDocument>`

**Brief:** Placeholder for finding version information. Currently unimplemented.

**Parameters:**
- `filter` - JSON document containing filter criteria

**Preconditions:**
- `filter` must not be empty

**Returns:** Never returns (always throws)

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error("UconDB")` | Always (operation not implemented) |

**TLOG Level:** 15

---

### `findEntities(JSONDocument const& filter) -> std::vector<JSONDocument>`

**Brief:** Placeholder for finding entities matching a filter. Currently unimplemented.

**Parameters:**
- `filter` - JSON document containing filter criteria

**Preconditions:**
- `filter` must not be empty

**Returns:** Never returns (always throws)

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error("UconDB")` | Always (operation not implemented) |

**TLOG Level:** 19

---

### `listCollections(JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Placeholder for listing collections (folders) in the database. Currently unimplemented.

**Parameters:**
- `query_payload` - JSON document containing query parameters

**Preconditions:**
- `query_payload` must not be empty

**Returns:** Never returns (always throws)

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error("UconDB")` | Always (operation not implemented) |

**TLOG Level:** 22

---

### `addConfiguration(JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Placeholder for adding a configuration. Currently unimplemented.

**Parameters:**
- `query_payload` - JSON document containing configuration data

**Preconditions:**
- `query_payload` must not be empty

**Returns:** Never returns (always throws)

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error("UconDB")` | Always (operation not implemented) |

**TLOG Level:** 15

---

### `listDatabases(JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Placeholder for listing available databases. Currently unimplemented.

**Parameters:**
- `query_payload` - JSON document containing query parameters

**Preconditions:**
- `query_payload` must not be empty

**Returns:** Never returns (always throws)

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error("UconDB")` | Always (operation not implemented) |

**TLOG Level:** 19

---

### `databaseMetadata(JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Placeholder for retrieving database metadata. Currently unimplemented.

**Parameters:**
- `query_payload` - JSON document (marked as `[[gnu::unused]]`)

**Preconditions:**
- `query_payload` must not be empty

**Returns:** Never returns (always throws)

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error("UconDB")` | Always (operation not implemented) |

---

### `searchCollection(JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Placeholder for searching within a collection. Currently unimplemented.

**Parameters:**
- `query_payload` - JSON document containing search criteria

**Preconditions:**
- `query_payload` must not be empty

**Returns:** Never returns (always throws)

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error("UconDB")` | Always (operation not implemented) |

**TLOG Level:** 15

## Internal Functions

### `debug::enable() -> void`

**Brief:** Enables all TRACE debugging for UconDB provider components.

**Called by:** Application code requesting debug output

**Purpose:** Configures TRACE logging at maximum verbosity for troubleshooting

**Side Effects:**
- Sets TRACE name to "provider_ucondb.cpp"
- Enables all trace levels (0xFFFFFFFFFFFFFFFFLL)
- Calls `debug::UconDBAPI()` to enable API tracing
- Calls `debug::ReadWrite()` to enable read/write tracing

**Implementation:**
```cpp
void enable() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::ucon trace_enable";
  artdaq::database::ucon::debug::UconDBAPI();
  artdaq::database::ucon::debug::ReadWrite();
}
```

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/StorageProviders/UconDB/provider_ucondb.h` | Header declarations |
| `artdaq-database/BasicTypes/basictypes.h` | Basic type definitions |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | Document construction |
| `artdaq-database/SharedCommon/common.h` | Common utilities |
| `artdaq-database/StorageProviders/UconDB/ucondb_api.h` | REST API client |

## Performance Considerations

- Since all query operations throw exceptions immediately, there is no performance overhead beyond exception handling
- These methods should not be called in production code; use FileSystemDB or MongoDB providers for query operations
- The `confirm()` macro check adds minimal overhead

## Error Handling Strategy

All query methods use the same error handling pattern:
1. Validate input with `confirm()` macro (assertion failure if empty)
2. Log the attempt with TLOG at the appropriate trace level
3. Throw `runtime_error("UconDB")` with descriptive "not implemented" message

This approach ensures:
- Clear error messages indicating the limitation
- Consistent behavior across all unimplemented operations
- Easy identification of UconDB-specific errors in logs

## Testing Notes

- **Unit tests:** Limited applicability (operations always throw)
- **Key test cases:** Verify that calling these methods throws the expected `runtime_error` with the correct message

## Maintenance Notes

If UconDB REST API is extended to support query operations in the future, these template specializations should be updated to:
1. Build appropriate REST API requests using `ucondb_api.h` functions
2. Parse JSON responses into `JSONDocument` objects
3. Handle HTTP errors appropriately
4. Remove the throw statement and return actual results

## See Also

- [provider_ucondb.h.md](./provider_ucondb.h.md) - Header file with class declarations
- [provider_ucondb_readwrite.cpp.md](./provider_ucondb_readwrite.cpp.md) - Implemented read/write operations
- [ucondb_api.cpp.md](./ucondb_api.cpp.md) - REST API client implementation
