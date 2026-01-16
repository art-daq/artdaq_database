# conftool.cpp

**Path:** `artdaq-database/SWIGBindings/python/conftool/conftool.cpp`

**Implements:** [conftool.h](./conftool.h.md)

**Purpose:** This implementation file provides the wrapper functions that bridge the Python SWIG bindings to the underlying artdaq-database C++ library. Each function sets the locale for consistent number formatting, prepares any necessary output variables, and delegates to the appropriate library function in the `artdaq::database::configuration::json` namespace. The file also includes optional FHiCL-to-JSON conversion functions when compiled with FHiCL support.

## Implementation Overview

The implementation follows a consistent delegation pattern:
1. Call `set_default_locale()` to ensure consistent number formatting
2. Prepare any output variables if required by the library function signature
3. Delegate to the corresponding library function in `artdaq::database::configuration::json`
4. Return the result directly as a `result_t` (pair of bool and string)

All functions are thin wrappers that add locale management and adapt the library's API to the SWIG-compatible interface defined in `conftool.h`.

## Key Algorithms

### Locale Management Pattern

Every wrapper function follows this pattern to ensure consistent number formatting:

**Steps:**
1. Call `set_default_locale()` at function entry
2. Execute library operation
3. Return result

**Why this approach:** The locale setting ensures that floating-point numbers in JSON are formatted with `.` as the decimal separator, regardless of the system's locale settings. This is critical for Python/C++ interoperability where data is exchanged as JSON strings.

### Output Variable Preparation

Some library functions require output parameters. The wrapper prepares these:

```cpp
result_t read_document(std::string const& query_payload) {
  set_default_locale();
  auto conf = std::string{};           // Output variable
  return impl::read_document(query_payload, conf);  // Library populates conf
}
```

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/configurationdb.h` | Aggregate header providing access to all configuration database operations |
| `artdaq-database/DataFormats/Fhicl/fhicljsondb.h` | FHiCL-to-JSON conversion utilities (used conditionally with `ADFHICLCPP`) |
| `conftool.h` | Local header with function declarations and `result_t` type definition |

## Internal Functions

### Namespace Aliases

```cpp
namespace impl = artdaq::database::configuration::json;
namespace fjlib = artdaq::database::fhicljson;
```

**Brief:** Shorthand aliases for frequently used namespaces.

**Purpose:**
- `impl`: The JSON-based configuration database operations namespace
- `fjlib`: FHiCL/JSON conversion library namespace

---

### `set_default_locale() -> void`

**Brief:** Wrapper that delegates to the core library's locale management function.

**Called by:** All other wrapper functions

**Implementation:**
```cpp
void set_default_locale() { artdaq::database::set_default_locale(); }
```

**Why It's Needed:**
- Ensures consistent decimal separators (`.` not `,`) in JSON
- Prevents locale-dependent formatting issues
- Critical for Python/C++ interoperability

---

### `enable_trace() -> void`

**Brief:** Enables debug tracing in the underlying configuration library.

**Called by:** Python applications for debugging

**Implementation:**
```cpp
void enable_trace() { return impl::enable_trace(); }
```

---

## Document Management Implementations

### `read_document(query_payload) -> result_t`

**Brief:** Reads a document from the database by delegating to the library's read function.

**Called by:** Python via SWIG wrapper

**Implementation:**
```cpp
result_t read_document(std::string const& query_payload) {
  set_default_locale();
  auto conf = std::string{};
  return impl::read_document(query_payload, conf);
}
```

**Notes:**
- Prepares an empty string `conf` for the library to populate
- The library function signature differs from the wrapper (takes output parameter)

---

### `write_document(query_payload, json_document) -> result_t`

**Brief:** Writes a document to the database by delegating to the library's write function.

**Implementation:**
```cpp
result_t write_document(std::string const& query_payload, std::string const& json_document) {
  set_default_locale();
  return impl::write_document(query_payload, json_document);
}
```

**Notes:**
- Direct delegation - no output variable preparation needed
- Both parameters passed through to library

---

### `mark_document_readonly(query_payload) -> result_t`

**Brief:** Marks a document as read-only by delegating to the library function.

**Implementation:**
```cpp
result_t mark_document_readonly(std::string const& query_payload) {
  set_default_locale();
  return impl::mark_document_readonly(query_payload);
}
```

---

### `mark_document_deleted(query_payload) -> result_t`

**Brief:** Marks a document as deleted (soft delete) by delegating to the library function.

**Implementation:**
```cpp
result_t mark_document_deleted(std::string const& query_payload) {
  set_default_locale();
  return impl::mark_document_deleted(query_payload);
}
```

---

## Version Management Implementations

### `find_versions(query_payload) -> result_t`

**Brief:** Finds all versions of a document by delegating to the library function.

**Implementation:**
```cpp
result_t find_versions(std::string const& query_payload) {
  set_default_locale();
  return impl::find_versions(query_payload);
}
```

---

### `add_version_alias(query_payload) -> result_t`

**Brief:** Adds a version alias by delegating to the library function.

**Implementation:**
```cpp
result_t add_version_alias(std::string const& query_payload) {
  set_default_locale();
  return impl::add_version_alias(query_payload);
}
```

---

### `remove_version_alias(query_payload) -> result_t`

**Brief:** Removes a version alias by delegating to the library function.

**Implementation:**
```cpp
result_t remove_version_alias(std::string const& query_payload) {
  set_default_locale();
  return impl::remove_version_alias(query_payload);
}
```

---

### `find_version_aliases(query_payload) -> result_t`

**Brief:** Lists all version aliases by delegating to the library function.

**Implementation:**
```cpp
result_t find_version_aliases(std::string const& query_payload) {
  set_default_locale();
  return impl::find_version_aliases(query_payload);
}
```

---

## Entity Management Implementations

### `find_entities(query_payload) -> result_t`

**Brief:** Finds entities in a collection by delegating to the library function.

**Implementation:**
```cpp
result_t find_entities(std::string const& query_payload) {
  set_default_locale();
  return impl::find_entities(query_payload);
}
```

---

### `add_entity(query_payload) -> result_t`

**Brief:** Adds a new entity by delegating to the library function.

**Implementation:**
```cpp
result_t add_entity(std::string const& query_payload) {
  set_default_locale();
  return impl::add_entity(query_payload);
}
```

---

### `remove_entity(query_payload) -> result_t`

**Brief:** Removes an entity by delegating to the library function.

**Implementation:**
```cpp
result_t remove_entity(std::string const& query_payload) {
  set_default_locale();
  return impl::remove_entity(query_payload);
}
```

---

## Configuration Management Implementations

### `find_configurations(query_payload) -> result_t`

**Brief:** Lists available configurations by delegating to the library function.

**Implementation:**
```cpp
result_t find_configurations(std::string const& query_payload) {
  set_default_locale();
  return impl::find_configurations(query_payload);
}
```

---

### `configuration_composition(query_payload) -> result_t`

**Brief:** Gets the composition of a configuration by delegating to the library function.

**Implementation:**
```cpp
result_t configuration_composition(std::string const& query_payload) {
  set_default_locale();
  return impl::configuration_composition(query_payload);
}
```

---

### `create_configuration(query_payload) -> result_t`

**Brief:** Creates a new configuration by delegating to the library function.

**Implementation:**
```cpp
result_t create_configuration(std::string const& query_payload) {
  set_default_locale();
  return impl::create_configuration(query_payload);
}
```

---

### `assign_configuration(query_payload) -> result_t`

**Brief:** Assigns a configuration to a run or context by delegating to the library function.

**Implementation:**
```cpp
result_t assign_configuration(std::string const& query_payload) {
  set_default_locale();
  return impl::assign_configuration(query_payload);
}
```

---

### `remove_configuration(query_payload) -> result_t`

**Brief:** Removes a configuration by delegating to the library function.

**Implementation:**
```cpp
result_t remove_configuration(std::string const& query_payload) {
  set_default_locale();
  return impl::remove_configuration(query_payload);
}
```

---

### `read_configuration(query_payload) -> result_t`

**Brief:** Reads an entire configuration by delegating to the library function.

**Implementation:**
```cpp
result_t read_configuration(std::string const& query_payload) {
  set_default_locale();
  auto conf = std::string{};
  return impl::read_configuration(query_payload, conf);
}
```

**Notes:**
- Similar to `read_document()`, prepares output variable
- Library populates `conf` with configuration data

---

### `write_configuration(query_payload, json_document) -> result_t`

**Brief:** Writes an entire configuration by delegating to the library function.

**Implementation:**
```cpp
result_t write_configuration(std::string const& query_payload, std::string const& json_document) {
  set_default_locale();
  return impl::write_configuration(query_payload, json_document);
}
```

---

## Import/Export Implementations

### `export_configuration(query_payload) -> result_t`

**Brief:** Exports a configuration to JSON format by delegating to the library function.

**Implementation:**
```cpp
result_t export_configuration(std::string const& query_payload) {
  set_default_locale();
  return impl::export_configuration(query_payload);
}
```

---

### `import_configuration(query_payload) -> result_t`

**Brief:** Imports a configuration from JSON data by delegating to the library function.

**Implementation:**
```cpp
result_t import_configuration(std::string const& query_payload) {
  set_default_locale();
  return impl::import_configuration(query_payload);
}
```

---

### `export_database(query_payload) -> result_t`

**Brief:** Exports the entire database to JSON format by delegating to the library function.

**Implementation:**
```cpp
result_t export_database(std::string const& query_payload) {
  set_default_locale();
  return impl::export_database(query_payload);
}
```

---

### `import_database(query_payload) -> result_t`

**Brief:** Imports entire database contents from JSON by delegating to the library function.

**Implementation:**
```cpp
result_t import_database(std::string const& query_payload) {
  set_default_locale();
  return impl::import_database(query_payload);
}
```

---

### `export_collection(query_payload) -> result_t`

**Brief:** Exports a specific collection to JSON format by delegating to the library function.

**Implementation:**
```cpp
result_t export_collection(std::string const& query_payload) {
  set_default_locale();
  return impl::export_collection(query_payload);
}
```

---

### `import_collection(query_payload) -> result_t`

**Brief:** Imports a collection from JSON data by delegating to the library function.

**Implementation:**
```cpp
result_t import_collection(std::string const& query_payload) {
  set_default_locale();
  return impl::import_collection(query_payload);
}
```

---

## Search and Metadata Implementations

### `search_collection(query_payload) -> result_t`

**Brief:** Searches within a collection by delegating to the library function.

**Implementation:**
```cpp
result_t search_collection(std::string const& query_payload) {
  set_default_locale();
  return impl::search_collection(query_payload);
}
```

---

### `list_databases(query_payload) -> result_t`

**Brief:** Lists available databases by delegating to the library function.

**Implementation:**
```cpp
result_t list_databases(std::string const& query_payload) {
  set_default_locale();
  return impl::list_databases(query_payload);
}
```

---

### `read_dbinfo(query_payload) -> result_t`

**Brief:** Reads database information and metadata by delegating to the library function.

**Implementation:**
```cpp
result_t read_dbinfo(std::string const& query_payload) {
  set_default_locale();
  return impl::read_dbinfo(query_payload);
}
```

---

### `list_collections(query_payload) -> result_t`

**Brief:** Lists all collections in the database by delegating to the library function.

**Implementation:**
```cpp
result_t list_collections(std::string const& query_payload) {
  set_default_locale();
  return impl::list_collections(query_payload);
}
```

---

## FHiCL Utility Implementations (Conditional)

These functions are only compiled when `ADFHICLCPP` is defined.

### `fhicl_to_json(fcl, filename) -> result_t`

**Brief:** Converts FHiCL content to JSON format using the FHiCL/JSON conversion library.

**Implementation:**
```cpp
#ifdef ADFHICLCPP
result_t fhicl_to_json(std::string const& fcl, std::string const& filename) {
  set_default_locale();
  auto retValue = std::string{};
  auto result = fjlib::fhicl_to_json(fcl, filename, retValue);
  return {result, retValue};
}
#endif
```

**Notes:**
- Uses the `fhicljson` library namespace (`fjlib`)
- Prepares output variable `retValue` for the converted JSON
- Constructs the result pair manually from the boolean and output string

**Parameters:**
- `fcl`: FHiCL content as a string
- `filename`: Original filename (used for error messages and context)

---

### `json_to_fhicl(jsn, filename) -> result_t`

**Brief:** Converts JSON content to FHiCL format using the FHiCL/JSON conversion library.

**Implementation:**
```cpp
#ifdef ADFHICLCPP
result_t json_to_fhicl(std::string const& jsn, std::string& filename) {
  set_default_locale();
  auto retValue = std::string{};
  auto result = fjlib::json_to_fhicl(jsn, retValue, filename);
  return {result, retValue};
}
#endif
```

**Notes:**
- Converts JSON back to FHiCL format
- The `filename` parameter may be modified by the library
- Returns the FHiCL content in the result string

---

## Performance Considerations

- **Locale overhead:** Each call involves a locale check, which has minimal overhead
- **String passing:** Parameters are passed by const reference (no copy on input)
- **String results:** Results may involve copies across the SWIG boundary
- **Large documents:** Consider batch operations for large document sets

## Error Handling Strategy

The wrapper functions do not add additional error handling:
- Exceptions from the library propagate to SWIG
- SWIG catches exceptions and converts to Python errors
- The `result_t` return type already provides error information via the boolean and string

## Testing Notes

- **Unit tests:** Integration tests use the Python bindings directly
- **Key test cases:** Document round-trip, configuration lifecycle, import/export

## Maintenance Notes

### Adding New Functions

To add a new wrapped function:

1. Add declaration to `conftool.h`:
   ```cpp
   result_t new_function(std::string const& query_payload);
   ```

2. Add implementation to `conftool.cpp`:
   ```cpp
   result_t new_function(std::string const& query_payload) {
     set_default_locale();
     return impl::new_function(query_payload);
   }
   ```

3. SWIG will automatically wrap it (included via `%include "conftool.h"`)

### Thread Safety

- `set_default_locale()` is thread-safe
- Thread safety of library calls depends on ConfigurationDB implementation
- Python GIL provides some protection but not complete

### Debugging Tips

1. Enable tracing from Python:
   ```python
   conftoolp.enable_trace()
   ```

2. Check library logs for detailed error messages

3. Verify JSON payload format matches expected schema

4. Test with simple queries first before complex operations

## Relationship to Other Components

### Architecture Position

```
Python Application (imports conftoolp)
         |
         v
+-------------------+
| SWIG Wrapper      |  (Generated from conftool.i)
| conftoolp.so      |
+-------------------+
         |
         v
+-------------------+
| conftool.cpp      |  <-- This file
| (Wrapper layer)   |
+-------------------+
         |
         v
+-------------------+
| ConfigurationDB   |  artdaq::database::configuration::json::*
| (Core library)    |
+-------------------+
         |
         v
+-------------------+
| Database Backend  |  MongoDB or FileDB
+-------------------+
```

### Function Mapping

| conftool.cpp Function | Library Function |
|-----------------------|------------------|
| `read_document` | `impl::read_document` |
| `write_document` | `impl::write_document` |
| `find_versions` | `impl::find_versions` |
| `find_configurations` | `impl::find_configurations` |
| `export_configuration` | `impl::export_configuration` |
| `fhicl_to_json` | `fjlib::fhicl_to_json` |
| ... | ... |

## See Also

- [conftool.h.md](./conftool.h.md) - Header file with function declarations
- [conftool.i.md](./conftool.i.md) - SWIG interface file
- [ConfigurationDB/configurationdb.h.md](../ConfigurationDB/configurationdb.h.md) - Core library header
