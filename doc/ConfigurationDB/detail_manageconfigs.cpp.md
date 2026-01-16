# detail_manageconfigs.cpp

**Path:** `artdaq-database/ConfigurationDB/detail_manageconfigs.cpp`

**Implements:** [dboperation_manageconfigs.h](./dboperation_manageconfigs.h.md)

**Purpose:** Contains the core implementation logic for configuration management operations including assigning, removing, finding, and creating configurations. A "configuration" in this context is a named collection of documents that belong together as a logical unit (e.g., a complete detector configuration for a specific run or experiment setup).

## Implementation Overview

This file provides the internal (`detail` namespace) implementations for configuration-level operations:
- **assign_configuration**: Associates a document with a named configuration
- **remove_configuration**: Disassociates a document from a configuration
- **create_configuration**: Creates a new configuration by associating multiple documents
- **find_configurations**: Searches for existing configurations
- **configuration_composition**: Retrieves the list of documents that make up a configuration

All functions dispatch to the appropriate storage provider (MongoDB, FileSystemDB, or UconDB) based on the configured provider name.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | Common definitions and TRACE configuration |
| `artdaq-database/ConfigurationDB/dboperation_manageconfigs.h` | Header being implemented |
| `artdaq-database/ConfigurationDB/shared_helper_functions.h` | Validation and helper functions |
| `artdaq-database/DataFormats/shared_literals.h` | String literal constants |
| `artdaq-database/SharedCommon/configuraion_api_literals.h` | API literal constants |
| `artdaq-database/ConfigurationDB/configuration_dbproviders.h` | Provider dispatch declarations |
| `artdaq-database/BasicTypes/basictypes.h` | Basic type definitions |
| `artdaq-database/DataFormats/Json/json_common.h` | JSON parsing utilities |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | Document manipulation |
| `artdaq-database/JsonDocument/docrecord_literals.h` | Document record literals |
| `artdaq-database/ConfigurationDB/options_operations.h` | ManageDocumentOperation class |

## Type Definitions

### Provider Function Types

```cpp
using provider_call_returnslist_t = std::vector<JSONDocument> (*)(const Options&, const JSONDocument&);
using provider_call_t = JSONDocument (*)(const Options&, const JSONDocument&);
```

Function pointer types for storage provider dispatch operations:
- `provider_call_returnslist_t`: For operations returning multiple documents (e.g., find)
- `provider_call_t`: For operations returning a single document (e.g., assign, remove)

## Internal Functions

### `assign_configuration(Options const&, std::string&) -> void`

**Brief:** Assigns a document to a named configuration, creating an association between the document and the configuration name.

**Parameters:**
- `options` - `Options const&` - Operation configuration containing configuration name and document identifier
- `configs` - `std::string&` - Output buffer for the result (must be empty)

**Preconditions:**
- `options.operation()` must be `assignconfig`
- `configs` must be empty
- Provider name must be valid

**Postconditions:**
- The document is associated with the specified configuration
- `configs` contains the result in the requested format

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When format is unsupported (db, json, fhicl, unknown) |

**Thread Safety:** Conditional - depends on underlying provider implementation

**Implementation Details:**

1. Validates operation type and empty output buffer
2. Validates provider name using `validate_dbprovider_name()`
3. Dispatches to provider's `assignConfiguration` function with query filter
4. Formats result based on requested format (only `gui` format is supported)

**Supported Formats:**
- **gui**: Returns the result document as-is

---

### `remove_configuration(Options const&, std::string&) -> void`

**Brief:** Removes the association between a document and a configuration name, effectively excluding the document from the configuration.

**Parameters:**
- `options` - `Options const&` - Operation configuration containing configuration name and document identifier
- `configs` - `std::string&` - Output buffer for the result (must be empty)

**Preconditions:**
- `options.operation()` must be `removeconfig`
- `configs` must be empty
- Provider name must be valid

**Postconditions:**
- The document is no longer associated with the specified configuration
- `configs` contains the result in the requested format

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When format is unsupported |

**Thread Safety:** Conditional - depends on underlying provider implementation

**Implementation Details:**

1. Validates operation type and empty output buffer
2. Validates provider name
3. Dispatches to provider's `removeConfiguration` function
4. Returns result in requested format (gui only)

---

### `create_configuration(std::string const&, std::string&) -> void`

**Brief:** Creates a new configuration by processing a list of operations that assign multiple documents to a configuration name in sequence.

**Parameters:**
- `operations` - `std::string const&` - JSON string containing an array of assign operations
- `configs` - `std::string&` - Output buffer for the result (must be empty)

**Preconditions:**
- `operations` must not be empty
- `configs` must be empty

**Postconditions:**
- All specified documents are assigned to the configuration
- `configs` contains the final operation result

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When JSON parsing fails |

**Thread Safety:** Not thread-safe - processes operations sequentially

**Implementation Details:**

1. Validates non-empty operations string
2. Parses the JSON operations string using `JsonReader`
3. Extracts the operations array from the JSON object
4. For each operation in the array:
   - Converts to JSON string
   - Creates a `ManageDocumentOperation` with the operation data
   - Calls `assign_configuration()` to process the assignment
5. Returns the result of the last operation

**JSON Operations Format:**
```json
{
  "operations": [
    {"operation": "assignconfig", "configuration": "myconfig", ...},
    {"operation": "assignconfig", "configuration": "myconfig", ...}
  ]
}
```

---

### `find_configurations(Options const&, std::string&) -> void`

**Brief:** Searches for configurations matching the query filter and returns a list of configuration names.

**Parameters:**
- `options` - `Options const&` - Search configuration with query filter
- `configs` - `std::string&` - Output buffer for configuration list (must be empty)

**Preconditions:**
- `options.operation()` must be `findconfigs`
- `configs` must be empty
- Provider name must be valid

**Postconditions:**
- `configs` contains list of matching configurations in requested format

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When format is unsupported (db, json, fhicl, xml, unknown) |

**Thread Safety:** Conditional - depends on underlying provider implementation

**Implementation Details:**

1. Validates operation type and empty output buffer
2. Validates provider name
3. Dispatches to provider's `findConfigurations` function
4. Formats results based on requested format

**Output Formats:**
- **gui**: `{"search": [<configuration objects>]}`
- **csv**: Comma-separated configuration names

---

### `configuration_composition(Options const&, std::string&) -> void`

**Brief:** Retrieves the composition of a configuration, returning the list of documents that make up the configuration with their filter criteria.

**Parameters:**
- `options` - `Options const&` - Query configuration with configuration name
- `filters` - `std::string&` - Output buffer for document list (must be empty)

**Preconditions:**
- `options.operation()` must be `confcomposition`
- `filters` must be empty
- Provider name must be valid

**Postconditions:**
- `filters` contains the list of documents in the configuration

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When format conversion fails for non-gui formats |

**Thread Safety:** Conditional - depends on underlying provider implementation

**Implementation Details:**

1. Validates operation type and empty output buffer
2. Validates provider name
3. Dispatches to provider's `configurationComposition` function
4. For non-gui formats (db, json, fhicl, xml, unknown), attempts `json_db_to_gui` conversion
5. For gui format, returns result directly

**Note:** Unlike other functions, this one attempts format conversion for non-gui formats rather than throwing immediately, providing more flexible output options.

---

### `debug::detail::ManageConfigs() -> void`

**Brief:** Enables TRACE logging for the detail implementation of configuration management operations.

**Purpose:** Configure TRACE logging infrastructure for debugging configuration operations.

**Postconditions:**
- TRACE name set to "detail_manageconfigs.cpp"
- All trace levels enabled
- Trace modes configured per `trace_mode::modeM` and `trace_mode::modeS`

**Thread Safety:** Not thread-safe - should be called during initialization only

---

## Key Algorithms

### Storage Provider Dispatch

All operations use the same dispatch pattern as other detail files:

```cpp
auto dispatch_persistence_provider = [](std::string const& name) {
  auto providers = std::map<std::string, provider_call_t>{
    {apiliteral::provider::mongo, cf::mongo::function},
    {apiliteral::provider::filesystem, cf::filesystem::function},
    {apiliteral::provider::ucon, cf::ucon::function}
  };
  return providers.at(name);
};
```

### Batch Operation Processing

The `create_configuration` function processes multiple operations in sequence:

```cpp
for (auto const& operation_entry : operations_list) {
  auto buff = std::string{};
  JsonWriter{}.write(boost::get<object_t>(operation_entry), buff);

  auto addconfig = Options{apiliteral::operations};
  addconfig.readJsonData(buff);

  configs.clear();
  assign_configuration(addconfig, configs);
}
```

This allows creating a complete configuration with all its component documents in a single API call.

### Output Formatting

Results are formatted based on the requested format:

**GUI Format:**
```cpp
std::ostringstream oss;
oss << "{ \"search\": [\n";
for (auto const& search_result : search_results) {
  oss << search_result << ",";
}
oss.seekp(-1, oss.cur);  // Remove trailing comma
oss << "] }";
```

**CSV Format:**
```cpp
std::ostringstream oss;
for (auto const& search_result : search_results) {
  oss << search_result.value_as<std::string>(apiliteral::name) << ",";
}
// Remove trailing comma if present
```

---

## Performance Considerations

- **Batch operations**: `create_configuration` processes operations sequentially; could potentially be parallelized for independent documents
- **Provider dispatch**: Map lookup is O(1) but creates a new map on each call
- **JSON parsing**: Parsing the operations array adds overhead for batch operations
- **Output formatting**: GUI format construction involves string concatenation

## Error Handling Strategy

1. **Precondition assertions**: Uses `confirm()` macro for internal consistency checks
2. **Validation**: Validates provider name before dispatch
3. **Exception propagation**: Exceptions from providers propagate to caller
4. **Format validation**: Throws `runtime_error` for unsupported formats

## Testing Notes

- **Unit tests:** `test/ConfigurationDB/ManageConfigs_t.cc`
- **Key test cases:**
  - Assign/remove configuration round-trip
  - Create configuration with multiple documents
  - Find configurations with various filters
  - Configuration composition retrieval

## Maintenance Notes

- The `configs.clear()` in the batch loop ensures each operation starts fresh
- Format support is limited to `gui` and `csv` for most operations
- The `configuration_composition` function has special handling for non-gui formats
- Trace level 21 is used consistently for begin/end logging

## Common Pitfalls

- **Format support**: Most operations only support `gui` and `csv` formats
- **Empty output buffer**: Must be empty before calling; non-empty buffers will cause assertion failures
- **Batch operation errors**: If one operation fails in `create_configuration`, subsequent operations are skipped

## See Also

- [dboperation_manageconfigs.h](./dboperation_manageconfigs.h.md) - Header file
- [configuration_dbproviders.h](./configuration_dbproviders.h.md) - Provider declarations
- [detail_managedocument.cpp](./detail_managedocument.cpp.md) - Document-level operations
- [JSONDocumentBuilder.h](../JsonDocument/JSONDocumentBuilder.h.md) - Document manipulation

---

**Documentation generated for artdaq-database ConfigurationDB module**
