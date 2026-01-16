# dboperation_findcompositions.h

**Path:** `artdaq-database/ConfigurationDB/dboperation_findcompositions.h`

**Purpose:** Declares the API for finding global configuration compositions that contain specific configuration versions. This provides reverse lookup capability to determine which global configurations reference a particular configuration type/version/entity combination.


---

## Key Concepts

### Composition Containment Search

This file provides the "reverse lookup" for global configurations. Given a specific configuration type, version, and optionally entity, it finds all global configurations (compositions) that include that member. This is useful for:

- Impact analysis before modifying a configuration version
- Finding dependent global configurations
- Auditing which runs used a specific configuration

### Two-Namespace Pattern

Like other dboperation headers, this file provides two API entry points:
- `opts::` namespace - Takes C++ operation objects directly
- `json::` namespace - Takes JSON string payloads for REST API compatibility

---

## Thread Safety

- **Thread-safe:** Yes
- **Concurrent access:** Functions are stateless and can be called concurrently
- **Locking:** No internal locks; thread safety provided by underlying storage providers

---

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/SharedCommon/returned_result.h` | Provides `result_t` type for operation results |

---

## Forward Declarations

### `ManageDocumentOperation`

**Brief:** Forward declaration of the operation options class used to configure search parameters.

---

## Type Aliases

### `result_t`

```cpp
using artdaq::database::result_t;
```

**Brief:** Result type alias imported from SharedCommon, representing a pair of (success boolean, message string).

---

## Functions

### `opts::find_compositions_containing`

```cpp
result_t find_compositions_containing(ManageDocumentOperation const& options, std::string& results) noexcept;
```

**Brief:** Finds all global configurations (compositions) that contain a specified configuration version. Returns matching composition names in the specified output format.

**Parameters:**
- `options` - Operation configuration specifying:
  - `collection` - Configuration type/collection name to search for
  - `version` - Version string to match
  - `entity` - (Optional) Entity name to filter by
  - `format` - Output format (gui, csv supported)
  - `provider` - Storage provider (filesystem, mongo, ucon)
- `results` - Output parameter receiving the search results as a formatted string

**Preconditions:**
- `options.operation()` must be `"findcompositionscontaining"`
- `options.collection()` must be non-empty
- `options.version()` must be non-empty
- `options.provider()` must be valid (filesystem, mongo, or ucon)

**Returns:** `result_t` - Pair of (success flag, message). On success, `results` contains the formatted output.

**Postconditions:**
- On success, `results` contains composition names in the requested format
- On failure, `results` is unchanged

**Throws:** Does not throw (noexcept). All exceptions are caught and returned as failure results.

**Thread Safety:** Safe - stateless function

**Side Effects:**
- Queries the configured storage provider database

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_findcompositions.h"
#include "artdaq-database/ConfigurationDB/options_operation_managedocument.h"

using namespace artdaq::database::configuration;

void findDependentConfigurations() {
  ManageDocumentOperation opts("caller");
  opts.operation("findcompositionscontaining");
  opts.collection("TriggerConfig");
  opts.version("v2.1");
  opts.entity("DAQ1");
  opts.format(options::data_format_t::gui);

  std::string results;
  auto result = opts::find_compositions_containing(opts, results);

  if (result.first) {
    std::cout << "Compositions containing TriggerConfig v2.1:\n";
    std::cout << results << "\n";
  } else {
    std::cerr << "Search failed: " << result.second << "\n";
  }
}
```

---

### `json::find_compositions_containing`

```cpp
result_t find_compositions_containing(std::string const& task_payload) noexcept;
```

**Brief:** JSON API version of the composition search. Parses operation parameters from a JSON string and returns results, providing REST API compatibility.

**Parameters:**
- `task_payload` - JSON string containing operation parameters:
  ```json
  {
    "operation": "findcompositionscontaining",
    "collection": "ConfigTypeName",
    "filter": {
      "version": "v1.0",
      "entities": "EntityName"
    },
    "format": "gui"
  }
  ```

**Preconditions:**
- `task_payload` must be non-empty
- `task_payload` must be valid JSON

**Returns:** `result_t` - Pair of (success flag, result JSON or error message)

**Postconditions:**
- On success, second element contains JSON results
- On failure, second element contains error description

**Throws:** Does not throw (noexcept)

**Thread Safety:** Safe - stateless function

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_findcompositions.h"

using namespace artdaq::database::configuration;

void restApiHandler(std::string const& requestBody) {
  auto result = json::find_compositions_containing(requestBody);

  if (result.first) {
    sendResponse(200, result.second);
  } else {
    sendResponse(500, result.second);
  }
}
```

---

## Debug Functions

### `debug::FindCompositions`

```cpp
void debug::FindCompositions();
```

**Brief:** Enables TRACE logging for the dboperation_findcompositions module by emitting a debug message at trace level 10.

**Thread Safety:** Safe

---

### `debug::detail::FindCompositions`

```cpp
void debug::detail::FindCompositions();
```

**Brief:** Enables TRACE logging for the detail_findcompositions implementation by emitting a debug message at trace level 11.

**Thread Safety:** Safe

---

## Relationship to Other Components

- **configurationdbifc.h**: Called by `ConfigurationInterface::findGlobalConfigurationsContaining()` method
- **options_operation_managedocument.h**: Uses `ManageDocumentOperation` class for parameters
- **detail_findcompositions.cpp**: Contains the actual implementation logic
- **dispatch_*.h**: Routes to storage provider implementations

---

## Output Formats

### GUI Format (data_format_t::gui)
```json
{
  "search": [
    {"name": "GlobalConfig1"},
    {"name": "GlobalConfig2"}
  ]
}
```

### CSV Format (data_format_t::csv)
```
GlobalConfig1,GlobalConfig2,GlobalConfig3
```

---

## See Also

- [dboperation_findcompositions.cpp.md](./dboperation_findcompositions.cpp.md) - Implementation details
- [detail_findcompositions.cpp.md](./detail_findcompositions.cpp.md) - Provider dispatch logic
- [dboperation_manageconfigs.h.md](./dboperation_manageconfigs.h.md) - Related configuration operations
- [configurationdbifc.h.md](./configurationdbifc.h.md) - High-level API using this function

---

## Notes for Developers

### Performance Considerations

The optimized path queries the storage provider directly for compositions containing the specified member. If this path is unavailable or fails, the system falls back to a brute-force approach that:
1. Lists all global configurations
2. Loads each one
3. Checks membership manually

The brute-force fallback is O(n * m) where n = number of compositions and m = average members per composition.

### Common Pitfalls

- **Unsupported formats:** Only `gui` and `csv` formats are supported. Using `json`, `fhicl`, `xml`, or `db` will throw an exception.
- **Empty results:** An empty result set is valid and indicates no compositions contain the specified member.

---

**Documentation generated for artdaq-database ConfigurationDB module**
