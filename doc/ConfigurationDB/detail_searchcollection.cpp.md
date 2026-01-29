# detail_searchcollection.cpp

**Path:** `artdaq-database/ConfigurationDB/detail_searchcollection.cpp`

**Implements:** [dboperation_searchcollection.h](./dboperation_searchcollection.h.md)

**Purpose:** Implements the core search collection operation that queries the configuration database and returns matching documents. This file provides the detail-level implementation that dispatches to the appropriate storage provider (MongoDB, FileSystemDB, or UconDB) based on the configured provider.

## Implementation Overview

This file implements the `search_collection` function which:
1. Validates the operation parameters and provider name
2. Dispatches to the appropriate storage provider's search function
3. Formats the results as a JSON array
4. Returns the formatted results to the caller

The search operation is provider-agnostic at this level, using a dispatch map to route to the correct backend implementation.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | Common utilities and confirm macro |
| `artdaq-database/ConfigurationDB/dboperation_searchcollection.h` | Operation interface declaration |
| `artdaq-database/ConfigurationDB/shared_helper_functions.h` | Shared validation functions |
| `artdaq-database/DataFormats/shared_literals.h` | JSON literal definitions |
| `artdaq-database/SharedCommon/configuraion_api_literals.h` | API literal constants |
| `artdaq-database/ConfigurationDB/configuration_dbproviders.h` | Provider validation functions |
| `artdaq-database/BasicTypes/basictypes.h` | Basic type definitions |
| `artdaq-database/DataFormats/Json/json_common.h` | JSON utilities |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | JSON document construction |
| `artdaq-database/JsonDocument/docrecord_literals.h` | Document record literals |
| `artdaq-database/ConfigurationDB/options_operations.h` | Operation options classes |

## Type Definitions

### Provider Function Type

```cpp
using provider_searchcollection_t = std::vector<JSONDocument> (*)(const Options&, const JSONDocument&);
```

Function pointer type for provider search operations that return a vector of documents.

## Internal Functions

### `search_collection(Options const& options, std::string& results) -> void`

**Brief:** Executes a collection search across the configured storage provider and returns matching documents as a JSON array.

**Called by:** Higher-level API functions in the configuration module

**Purpose:** Core implementation of the search collection operation that handles provider dispatch and result formatting.

**Parameters:**
- `options` - `ManageDocumentOperation const&` containing search parameters including provider name, collection, and query filters
- `results` - `std::string&` output parameter that receives the formatted JSON results

**Preconditions:**
- `results` must be empty (enforced via `confirm()`)
- `options.operation()` must be `apiliteral::operation::searchcollection`
- Provider name in options must be valid (mongo, filesystem, or ucon)

**Postconditions:**
- `results` contains a JSON object with a "search" array of matching documents
- If no format conversion is performed, `results` remains empty

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Unsupported data format (only JSON format is supported) |
| `std::out_of_range` | Unknown provider name in dispatch map |

**Thread Safety:** Conditional - depends on underlying provider implementation

**Side Effects:**
- TRACE logging at level 23

**Implementation Details:**

1. Validates preconditions using `confirm()` macro
2. Validates provider name with `validate_dbprovider_name()`
3. Creates a dispatch lambda that maps provider names to functions
4. Calls the selected provider's `searchCollection` function
5. Formats results based on requested format (only JSON is supported)

**Supported Formats:**
- **json**: `{"search": [<document objects>]}`

**Unsupported Formats:**
- db, gui, fhicl, csv, xml, unknown - all throw `runtime_error`

---

### `cftd::SearchCollection() -> void`

**Brief:** Enables TRACE debugging for the search collection module by configuring trace output modes.

**Called by:** Debug initialization code

**Purpose:** Configure TRACE logging infrastructure for debugging search collection operations.

**Preconditions:** None

**Postconditions:**
- TRACE name set to "detail_searchcollection.cpp"
- All trace levels enabled
- Trace modes configured per `trace_mode::modeM` and `trace_mode::modeS`

**Throws:** None

**Thread Safety:** Not thread-safe - should be called during initialization only

---

## Key Algorithms

### Provider Dispatch Pattern

The implementation uses a lambda-based dispatch pattern to select the appropriate provider:

```cpp
auto dispatch_persistence_provider = [](std::string const& name) {
  auto providers = std::map<std::string, provider_searchcollection_t>{
    {apiliteral::provider::mongo, cf::mongo::searchCollection},
    {apiliteral::provider::filesystem, cf::filesystem::searchCollection},
    {apiliteral::provider::ucon, cf::ucon::searchCollection}
  };
  return providers.at(name);
};
```

**Steps:**
1. Create a map of provider names to function pointers
2. Look up the provider name from the options
3. Call the corresponding provider function with the search parameters
4. Format the results as JSON

**Why this approach:** This pattern allows runtime provider selection without compile-time dependencies on all providers, enabling clean separation of concerns.

### Search Result Formatting

Search results from providers are formatted into a JSON array structure:

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

The resulting JSON format:
```json
{
  "search": [
    { /* result 1 */ },
    { /* result 2 */ }
  ]
}
```

---

## Performance Considerations

- **Memory:** Results are accumulated in a string stream before being swapped to the output
- **Caching:** No caching at this level; caching handled by providers
- **Known bottlenecks:** String stream manipulation for large result sets
- **Map creation:** The dispatch map is created on each call (could be optimized with static map)

## Error Handling Strategy

Errors are handled through:
1. **Precondition checks:** The `confirm()` macro validates that results are empty and operation type is correct
2. **Provider validation:** `validate_dbprovider_name()` throws if provider is unknown
3. **Format validation:** A `runtime_error` is thrown for unsupported data formats
4. **Map lookup:** `std::out_of_range` thrown if provider not in dispatch map

## Testing Notes

- **Unit tests:** `test/ConfigurationDB/ConfigurationDB_t.cc`
- **Key test cases:**
  - Search with valid provider returns results
  - Search with invalid provider throws exception
  - Empty search results handled correctly
  - Format validation (only JSON supported)

## Maintenance Notes

The `dispatch_persistence_provider` lambda creates a new map on each call. This is intentional to avoid static initialization order issues, but could be optimized if performance becomes a concern.

**Note:** This function only supports JSON output format, unlike other detail functions that support GUI and CSV formats. This is because search results typically contain full documents that are not suitable for simple CSV output.

## Common Pitfalls

- **Format support**: Only JSON format is supported; other formats throw exceptions
- **Empty output buffer**: Must be empty before calling
- **Provider name**: Must exactly match one of the registered providers

## See Also

- [dboperation_searchcollection.h](./dboperation_searchcollection.h.md) - Public interface
- [dispatch_filedb.h](./dispatch_filedb.h.md) - FileSystemDB provider dispatch
- [dispatch_mongodb.h](./dispatch_mongodb.h.md) - MongoDB provider dispatch
- [configuration_dbproviders.h](./configuration_dbproviders.h.md) - Provider validation

---

**Documentation generated for artdaq-database ConfigurationDB module**
