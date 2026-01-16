# detail_findcompositions.cpp

**Path:** `artdaq-database/ConfigurationDB/detail_findcompositions.cpp`

**Implements:** Provider dispatch for [dboperation_findcompositions.h](./dboperation_findcompositions.h.md)

**Purpose:** Contains the internal implementation of the `find_compositions_containing` operation, which searches for global configurations (compositions) that include a specific configuration version. This file handles provider dispatch and result formatting.

---

## Implementation Overview

This file implements the core search logic for finding compositions that contain a specific configuration member. It:

1. Validates the configured storage provider
2. Dispatches to the appropriate provider implementation (FileSystemDB, MongoDB, or UconDB)
3. Formats the search results according to the requested output format (GUI JSON or CSV)

---

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | Common utilities and TRACE logging |
| `artdaq-database/ConfigurationDB/dboperation_findcompositions.h` | Public API header |
| `artdaq-database/ConfigurationDB/dispatch_filedb.h` | FileSystemDB provider functions |
| `artdaq-database/ConfigurationDB/dispatch_mongodb.h` | MongoDB provider functions |
| `artdaq-database/ConfigurationDB/dispatch_ucondb.h` | UconDB provider functions |
| `artdaq-database/ConfigurationDB/options_operations.h` | Operation options classes |
| `artdaq-database/BasicTypes/basictypes.h` | Data type wrappers |
| `artdaq-database/DataFormats/shared_literals.h` | JSON field name literals |
| `artdaq-database/JsonDocument/JSONDocument.h` | JSON document manipulation |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | JSON document construction |
| `artdaq-database/SharedCommon/helper_functions.h` | Utility functions |

---

## TRACE Configuration

```cpp
#define TRACE_NAME "detail_findcompositions.cpp"
```

**Brief:** Sets the TRACE logging identifier for this translation unit.

---

## Type Aliases

### `Options`

```cpp
using Options = cf::ManageDocumentOperation;
```

**Brief:** Shorter alias for the operation options class.

### `provider_call_returnslist_t`

```cpp
using provider_call_returnslist_t = std::vector<JSONDocument> (*)(Options const&, JSONDocument const&);
```

**Brief:** Function pointer type for provider dispatch. All storage provider implementations must match this signature to return a vector of matching compositions.

---

## Internal Functions

### `validate_dbprovider_name`

```cpp
namespace {
void validate_dbprovider_name(std::string const& name);
}
```

**Brief:** Validates that the given provider name is one of the supported values (mongo, filesystem, ucon). Throws `runtime_error` for unknown providers.

**Parameters:**
- `name` - Provider name to validate

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When name is not "mongo", "filesystem", or "ucon" |

**Called by:** `detail::find_compositions_containing()`

---

## Main Implementation

### `detail::find_compositions_containing`

```cpp
void find_compositions_containing(Options const& options, std::string& results);
```

**Brief:** Core implementation that dispatches to storage providers and formats results. This function coordinates the entire search operation.

**Parameters:**
- `options` - Operation configuration containing:
  - `operation()` - Must be "findcompositionscontaining"
  - `provider()` - Storage provider name
  - `collection()` - Configuration type to search for
  - `version()` - Version to match (from query filter)
  - `entity()` - Entity to match (optional, from query filter)
  - `format()` - Output format (gui or csv)
- `results` - Output parameter receiving formatted results

**Preconditions:**
- `results` must be empty
- `options.operation()` must be "findcompositionscontaining"

**Postconditions:**
- `results` contains formatted search results

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Unsupported data format requested |
| `runtime_error` | Unknown provider name |
| `runtime_error` | Results were not set (internal error) |

---

## Key Algorithms

### Provider Dispatch Pattern

The function uses a map-based dispatch pattern to route to the correct storage provider:

```cpp
auto dispatch_persistence_provider = [](std::string const& name) -> provider_call_returnslist_t {
  auto providers = std::map<std::string, provider_call_returnslist_t>{
    {apiliteral::provider::mongo, cf::mongo::findCompositionsContaining},
    {apiliteral::provider::filesystem, cf::filesystem::findCompositionsContaining},
    {apiliteral::provider::ucon, cf::ucon::findCompositionsContaining}
  };
  return providers.at(name);
};
```

**Steps:**
1. Create map of provider name to function pointer
2. Look up function by provider name
3. Call returned function pointer with options and filter

**Why this approach:** Provides runtime provider selection with compile-time type safety. Adding new providers requires only adding an entry to the map.

### Result Formatting

Results are formatted based on the requested output format:

**GUI Format (`data_format_t::gui`):**
```cpp
oss << "{ \"search\": [\n";
for (auto const& result : search_results) {
  oss << result;  // JSONDocument serialization
}
oss << "\n] }";
```

Output:
```json
{
  "search": [
    {"name": "GlobalConfig1"},
    {"name": "GlobalConfig2"}
  ]
}
```

**CSV Format (`data_format_t::csv`):**
```cpp
for (auto const& result : search_results) {
  auto name_node = result.findChild(apiliteral::name);
  oss << JSONDocument::value(name_node.value()) << ",";
}
// Remove trailing comma
```

Output:
```
GlobalConfig1,GlobalConfig2,GlobalConfig3
```

---

## Supported Formats

| Format | Support | Output |
|--------|---------|--------|
| `gui` | Yes | JSON array of search results |
| `csv` | Yes | Comma-separated composition names |
| `json` | No | Throws runtime_error |
| `fhicl` | No | Throws runtime_error |
| `xml` | No | Throws runtime_error |
| `db` | No | Throws runtime_error |

---

## Error Handling Strategy

- **Input validation:** Uses `confirm()` assertions for preconditions
- **Provider validation:** Explicit check with descriptive error message
- **Format validation:** Switch-based with explicit throw for unsupported formats
- **Result validation:** Ensures results were actually set before returning

---

## Performance Considerations

- **Memory:** Search results stored in vector; memory scales with result count
- **String operations:** Uses ostringstream for efficient string building
- **Provider calls:** Single database query per search operation

---

## Testing Notes

- **Unit tests:** `test/ConfigurationDB/detail_findcompositions_t.cc`
- **Key test cases:**
  - Each supported format produces correct output
  - Unsupported formats throw appropriate exceptions
  - Empty result sets handled correctly
  - Provider dispatch works for all three providers

---

## Maintenance Notes

### Adding New Output Formats

1. Add case to the switch statement
2. Implement formatting logic
3. Set `returnValueChanged = true`
4. Update the Supported Formats table above

### Adding New Storage Providers

1. Add dispatch entry in the lambda's map
2. Ensure new provider implements `findCompositionsContaining()` with matching signature
3. Update `validate_dbprovider_name()` to accept new name

---

## See Also

- [dboperation_findcompositions.h.md](./dboperation_findcompositions.h.md) - Public API
- [dispatch_filedb.h.md](./dispatch_filedb.h.md) - FileSystemDB provider
- [dispatch_mongodb.h.md](./dispatch_mongodb.h.md) - MongoDB provider
- [dispatch_ucondb.h.md](./dispatch_ucondb.h.md) - UconDB provider

---

**Documentation generated for artdaq-database ConfigurationDB module**
