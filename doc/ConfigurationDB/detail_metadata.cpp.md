# detail_metadata.cpp

**Path:** `artdaq-database/ConfigurationDB/detail_metadata.cpp`

**Implements:** [dboperation_metadata.h](./dboperation_metadata.h.md)

**Purpose:** Contains the core implementation logic for database metadata operations including listing databases, listing collections, and retrieving database information. These operations provide introspection capabilities for discovering what data is available in the configuration database without accessing document content.

## Implementation Overview

This file provides the internal (`detail` namespace) implementations for metadata operations:
- **list_databases**: Retrieves a list of available databases
- **list_collections**: Retrieves a list of collections within a database
- **read_dbinfo**: Retrieves detailed information about a database

These are read-only operations that query the storage provider for structural metadata rather than document content.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | Common definitions and TRACE configuration |
| `artdaq-database/ConfigurationDB/dboperation_metadata.h` | Header being implemented |
| `artdaq-database/ConfigurationDB/shared_helper_functions.h` | Validation and helper functions |
| `artdaq-database/DataFormats/shared_literals.h` | String literal constants |
| `artdaq-database/SharedCommon/configuraion_api_literals.h` | API literal constants |
| `artdaq-database/ConfigurationDB/configuration_dbproviders.h` | Provider dispatch declarations |
| `artdaq-database/BasicTypes/basictypes.h` | Basic type definitions |
| `artdaq-database/DataFormats/Json/json_common.h` | JSON utilities |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | Document manipulation |
| `artdaq-database/JsonDocument/docrecord_literals.h` | Document record literals |
| `artdaq-database/ConfigurationDB/options_operations.h` | ManageDocumentOperation class |

## Type Definitions

### Provider Function Types

```cpp
using provider_listdatabases_t = std::vector<JSONDocument> (*)(const Options&, const JSONDocument&);
using provider_listcollections_t = std::vector<JSONDocument> (*)(const Options&, const JSONDocument&);
using provider_readdbinfo_t = JSONDocument (*)(const Options&, const JSONDocument&);
```

Function pointer types for storage provider dispatch operations:
- `provider_listdatabases_t`: For listing databases (returns vector)
- `provider_listcollections_t`: For listing collections (returns vector)
- `provider_readdbinfo_t`: For database info (returns single document)

## Internal Functions

### `list_databases(Options const&, std::string&) -> void`

**Brief:** Retrieves a list of all databases available in the storage provider.

**Parameters:**
- `options` - `Options const&` - Operation configuration with provider specification
- `configs` - `std::string&` - Output buffer for database list (must be empty)

**Preconditions:**
- `options.operation()` must be `listdatabases`
- `configs` must be empty
- Provider name must be valid

**Postconditions:**
- `configs` contains list of databases in requested format

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When format is unsupported (db, json, fhicl, xml, unknown) |

**Thread Safety:** Conditional - depends on underlying provider implementation

**Implementation Details:**

1. Validates operation type and empty output buffer
2. Validates provider name with `validate_dbprovider_name()`
3. Creates dispatch lambda to select the appropriate provider function
4. Dispatches to provider's `listDatabases` function with query filter
5. Formats results based on requested format

**Output Formats:**
- **gui**: `{"search": [<database objects>]}`
- **csv**: Comma-separated database names

**Provider Dispatch:**
```cpp
auto dispatch_persistence_provider = [](std::string const& name) {
  auto providers = std::map<std::string, provider_listdatabases_t>{
    {apiliteral::provider::mongo, cf::mongo::listDatabases},
    {apiliteral::provider::filesystem, cf::filesystem::listDatabases},
    {apiliteral::provider::ucon, cf::ucon::listDatabases}
  };
  return providers.at(name);
};
```

---

### `read_dbinfo(Options const&, std::string&) -> void`

**Brief:** Retrieves detailed information about the current database including statistics and metadata.

**Parameters:**
- `options` - `Options const&` - Operation configuration
- `filters` - `std::string&` - Output buffer for database info (must be empty)

**Preconditions:**
- `options.operation()` must be `readdbinfo`
- `filters` must be empty
- Provider name must be valid

**Postconditions:**
- `filters` contains database information in requested format

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When format conversion fails for unsupported formats |

**Thread Safety:** Conditional - depends on underlying provider implementation

**Implementation Details:**

1. Validates operation type and empty output buffer
2. Validates provider name
3. Dispatches to provider's `readDbInfo` function
4. Formats result based on requested format

**Format Handling:**

Unlike other metadata functions, `read_dbinfo` has different format behavior:
- **db, json, gui**: Returns result directly as-is
- **unknown, fhicl, xml**: Attempts `json_db_to_gui` conversion; throws if conversion fails

This allows GUI-ready data to be returned without conversion for common cases.

---

### `list_collections(Options const&, std::string&) -> void`

**Brief:** Retrieves a list of all collections in the current database.

**Parameters:**
- `options` - `Options const&` - Operation configuration with optional collection filter
- `collections` - `std::string&` - Output buffer for collection list (must be empty)

**Preconditions:**
- `options.operation()` must be `listcollections`
- `collections` must be empty
- Provider name must be valid

**Postconditions:**
- `collections` contains list of collection names in requested format

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When format is unsupported (db, json, fhicl, xml, unknown) |

**Thread Safety:** Conditional - depends on underlying provider implementation

**Implementation Details:**

1. Validates operation type and empty output buffer
2. Validates provider name
3. Dispatches to provider's `listCollections` function with collection filter
4. Formats results based on requested format

**Note:** Unlike `list_databases`, this function passes `collection_to_JsonData()` as the filter, allowing optional filtering by collection name pattern.

**Output Formats:**
- **gui**: `{"search": [<collection objects>]}`
- **csv**: Comma-separated collection names

---

### `debug::detail::Metadata() -> void`

**Brief:** Enables TRACE logging for the detail implementation of metadata operations.

**Purpose:** Configure TRACE logging infrastructure for debugging metadata operations.

**Postconditions:**
- TRACE name set to "detail_metadata.cpp"
- All trace levels enabled
- Trace modes configured per `trace_mode::modeM` and `trace_mode::modeS`

**Thread Safety:** Not thread-safe - should be called during initialization only

---

## Key Algorithms

### GUI Format Output Construction

For list operations returning multiple items, the GUI format is constructed as:

```cpp
std::ostringstream oss;
oss << "{ \"search\": [\n";
for (auto const& search_result : search_results) {
  oss << search_result << ",";
}
oss.seekp(-1, oss.cur);  // Remove trailing comma
oss << "] }";
returnValue = oss.str();
```

### CSV Format Output Construction

For CSV format, only the name field is extracted:

```cpp
std::ostringstream oss;
for (auto const& search_result : search_results) {
  oss << search_result.value_as<std::string>(apiliteral::name) << ",";
}
returnValue = oss.str();
if (returnValue.back() == ',') {
  returnValue.pop_back();  // Remove trailing comma
}
```

---

## Performance Considerations

- **Lightweight operations**: Metadata queries are typically fast as they only return structural information
- **Caching potential**: Results could be cached but currently are not
- **Provider-specific performance**: MongoDB provides native metadata queries; FileSystemDB scans directories
- **Output size**: Large databases with many collections may produce large result sets

## Error Handling Strategy

1. **Precondition assertions**: Uses `confirm()` macro for internal consistency checks
2. **Validation**: Validates provider name before dispatch
3. **Exception propagation**: Exceptions from providers propagate to caller
4. **Format validation**: Throws `runtime_error` for unsupported formats

## Provider-Specific Behavior

| Operation | MongoDB | FileSystemDB | UconDB |
|-----------|---------|--------------|--------|
| list_databases | Native API | Directory listing | REST API |
| list_collections | Native API | Subdirectory listing | REST API |
| read_dbinfo | db.stats() | File system stats | REST API |

## Testing Notes

- **Unit tests:** `test/ConfigurationDB/Metadata_t.cc`
- **Key test cases:**
  - List databases returns expected databases
  - List collections returns expected collections
  - Read db info returns valid statistics
  - Empty database/collection handling

## Maintenance Notes

- Trace level 21 is used for begin/end logging; level 22-23 for specific operations
- The log message at line 67 ("list_databases: end") is misplaced inside the dispatch lambda
- Format handling in `read_dbinfo` differs from other functions (allows more formats)
- The `list_collections` function uses `collection_to_JsonData()` for filtering

## Common Pitfalls

- **Format support**: Not all output formats are supported for all operations
- **Empty results**: Empty databases return empty arrays, not errors
- **Provider availability**: Operations fail if the specified provider is not available
- **Output buffer**: Must be empty before calling; non-empty buffers will cause assertion failures

## See Also

- [dboperation_metadata.h](./dboperation_metadata.h.md) - Header file
- [configuration_dbproviders.h](./configuration_dbproviders.h.md) - Provider declarations
- [dispatch_mongodb.h](./dispatch_mongodb.h.md) - MongoDB provider implementation
- [dispatch_filedb.h](./dispatch_filedb.h.md) - FileSystemDB provider implementation

---

**Documentation generated for artdaq-database ConfigurationDB module**
