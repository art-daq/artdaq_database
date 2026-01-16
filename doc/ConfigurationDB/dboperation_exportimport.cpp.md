# dboperation_exportimport.cpp

**Path:** `artdaq-database/ConfigurationDB/dboperation_exportimport.cpp`

**Implements:** [dboperation_exportimport.h](./dboperation_exportimport.h.md)

**Purpose:** Implements the public API functions for exporting and importing configurations, collections, and databases. This file provides the bulk data transfer operations using compressed archive formats (tar.bz2 with base64 encoding), handling file format detection, temporary file management, and provider dispatch.

## Implementation Overview

This file implements comprehensive export/import operations across three levels:

1. **Database level**: Export/import all collections
2. **Collection level**: Export/import all documents in a collection
3. **Configuration level**: Export/import configuration files with format detection

The implementation follows these patterns:
- **opts namespace functions**: Delegate to json namespace or detail functions
- **json namespace functions**: Parse JSON payloads, set up options, and call detail implementations
- **Error handling**: All functions catch exceptions and return `result_t` failures

## Thread Safety

- **Thread-safe:** Conditional
- **Concurrent access:** Safe for concurrent exports/imports to different files/collections
- **Locking:** No internal locking; relies on file system and database thread safety
- **Temporary directories:** Uses unique temp directories to avoid conflicts

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | Common definitions and TRACE configuration |
| `artdaq-database/BasicTypes/basictypes.h` | Basic type definitions |
| `artdaq-database/ConfigurationDB/dboperation_exportimport.h` | Header for this implementation |
| `artdaq-database/ConfigurationDB/dboperation_managedocument.h` | Document read/write operations |
| `artdaq-database/ConfigurationDB/dboperation_metadata.h` | List collections operation |
| `artdaq-database/ConfigurationDB/options_operations.h` | ManageDocumentOperation class |
| `artdaq-database/ConfigurationDB/shared_helper_functions.h` | Utility functions |
| `artdaq-database/DataFormats/shared_literals.h` | String literal constants |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | Document building utilities |
| `artdaq-database/SharedCommon/configuraion_api_literals.h` | API literal constants |
| `artdaq-database/DataFormats/Json/json_common.h` | JSON parsing utilities |

## Internal Functions

### Namespace: `detail`

---

#### `export_configuration(options, returnValue) -> void`

**Brief:** Internal implementation that exports a configuration to a file by reading documents and creating an archive.

**Called by:** `opts::export_configuration()`, `json::export_configuration()`

---

#### `import_configuration(options, returnValue) -> void`

**Brief:** Internal implementation that imports a configuration from an archive file.

**Called by:** `opts::import_configuration()`, `json::import_configuration()`

---

#### `export_database(options, returnValue) -> void`

**Brief:** Internal implementation that exports all collections in a database.

**Called by:** Indirectly via `json::export_database()`

---

#### `import_database(options, returnValue) -> void`

**Brief:** Internal implementation that imports all collections from archive files.

**Called by:** Indirectly via `json::import_database()`

---

#### `import_collection(options, returnValue) -> void`

**Brief:** Internal implementation that imports documents from an archive into a collection.

**Called by:** Indirectly via `json::import_collection()`

---

#### `export_collection(options, returnValue) -> void`

**Brief:** Internal implementation that exports all documents in a collection to an archive.

**Called by:** Indirectly via `json::export_collection()`

---

#### `configuration_composition(options, returnValue) -> void`

**Brief:** Retrieves the composition of a configuration (list of associated documents).

**Called by:** `json::read_configuration()`

---

#### `write_documents(options, document_list) -> void`

**Brief:** Writes multiple documents to the database through the configured provider.

**Called by:** Collection import operations

---

#### `read_documents(options, document_list) -> void`

**Brief:** Reads multiple documents from the database through the configured provider.

**Called by:** Collection export operations

---

#### `read_document_file(options, file_name) -> result_t`

**Brief:** Reads a document from the database and writes it to a file.

**Called by:** `json::read_configuration()`

---

#### `write_document_file(options, file_name) -> result_t`

**Brief:** Reads a document from a file and writes it to the database.

**Called by:** `json::write_configuration()`

---

## Public Function Implementations

### `opts::export_database(options) -> result_t`

**Brief:** Wrapper that delegates to `json::export_database()` after converting options to JSON.

**Implementation:**
```cpp
result_t opts::export_database(ManageDocumentOperation const& options) noexcept {
  try {
    return json::export_database(options.to_JsonData());
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
```

---

### `json::export_database(query_payload) -> result_t`

**Brief:** Exports all collections in a database to individual archive files in a directory.

**Implementation Details:**
1. Validates payload is not empty
2. Checks `system()` is available for compression utilities
3. Creates `ManageDocumentOperation` and parses JSON payload
4. Lists all collections using `opts::list_collections()`
5. Iterates through collections, calling `opts::export_collection()` for each
6. Collects results and error messages
7. Returns JSON array of exported file paths or accumulated errors

**Key Algorithm:**
```cpp
for (auto const& collection : collections) {
  options.collection(collection);
  options.sourceFileName(dirname + "/" + collection + ".artdaq");
  auto result = opts::export_collection(options);
  // Accumulate results/errors
}
```

---

### `opts::import_database(options) -> result_t`

**Brief:** Wrapper that delegates to `json::import_database()`.

---

### `json::import_database(query_payload) -> result_t`

**Brief:** Imports all `.artdaq` files from a directory into the database.

**Implementation Details:**
1. Validates payload and system availability
2. Lists files in the source directory using `list_files()`
3. Extracts collection names from file names using `extract_collectionname_from_filename()`
4. Calls `opts::import_collection()` for each valid archive file
5. Returns JSON array of imported files or accumulated errors

---

### `opts::export_configuration(options) -> result_t`

**Brief:** Exports a configuration by delegating to `detail::export_configuration()`.

---

### `json::export_configuration(query_payload) -> result_t`

**Brief:** JSON interface for exporting a configuration.

---

### `json::import_configuration(query_payload) -> result_t`

**Brief:** JSON interface for importing a configuration.

---

### `opts::import_configuration(options) -> result_t`

**Brief:** Imports a configuration by delegating to `detail::import_configuration()`.

---

### `json::write_configuration(query_payload, tarbzip2_base64) -> result_t`

**Brief:** Writes configuration data from a base64-encoded archive directly to the database.

**Implementation Details:**
1. Validates inputs (non-empty payload, non-empty archive data, system available)
2. Creates temporary directory using `db::make_temp_dir()`
3. Writes base64 archive to a file
4. Extracts archive using `db::tarbzip2base64_to_dir()`
5. Iterates through extracted files, detecting format by extension:
   - `.fcl`, `.fhicl` -> FHiCL format
   - `.jsn`, `.json` -> JSON format
   - `.xml` -> XML format
6. Calls `detail::write_document_file()` for each file
7. Cleans up temporary directory
8. Returns success or accumulated error messages

**Format Detection Algorithm:**
```cpp
if (file_name.rfind(".fcl") != std::string::npos ||
    file_name.rfind(".fhicl") != std::string::npos) {
  options.format(options::data_format_t::fhicl);
} else if (file_name.rfind(".jsn") != std::string::npos ||
           file_name.rfind(".json") != std::string::npos) {
  options.format(options::data_format_t::json);
} else if (file_name.rfind(".xml") != std::string::npos) {
  options.format(options::data_format_t::xml);
}
```

---

### `json::read_configuration(query_payload, tarbzip2_base64) -> result_t`

**Brief:** Reads a configuration from the database and returns it as a base64-encoded archive.

**Implementation Details:**
1. Validates inputs (non-empty payload, empty output buffer, system available)
2. Retrieves configuration composition via `detail::configuration_composition()`
3. Parses the composition JSON to get list of documents
4. Creates temporary directory
5. For each document in the configuration:
   - Creates `ManageDocumentOperation` from query
   - Sets format to `origin` (preserves original format)
   - Calls `detail::read_document_file()` to write to temp directory
6. Compresses directory to base64 archive using `db::dir_to_tarbzip2base64()`
7. Formats result as `{"collection.tar.bzip2.base64":"<base64_data>"}`
8. Cleans up temporary directory

---

### `opts::export_collection(options) -> result_t`

**Brief:** Wrapper that delegates to `json::export_collection()`.

---

### `json::export_collection(query_payload) -> result_t`

**Brief:** Exports all documents in a collection to a compressed archive file.

**Implementation Details:**
1. Validates payload and system availability
2. Creates options with `exportcollection` operation
3. Reads all documents using `detail::read_documents()`
4. Checks collection is not empty
5. Creates temporary directory
6. Writes each document as `<OUID>.json` file in temp directory
7. Compresses to archive using `db::dir_to_tarbzip2base64()`
8. Cleans up temporary directory
9. Returns JSON with result file path

---

### `opts::import_collection(options) -> result_t`

**Brief:** Wrapper that delegates to `json::import_collection()`.

---

### `json::import_collection(query_payload) -> result_t`

**Brief:** Imports documents from an archive file into a collection.

**Implementation Details:**
1. Validates payload and system availability
2. Creates options with `importcollection` operation
3. Creates temporary directory
4. Extracts archive using `db::tarbzip2base64_to_dir()`
5. Reads each JSON file from temp directory into `JSONDocumentBuilder`
6. Checks document list is not empty
7. Writes all documents using `detail::write_documents()`
8. Cleans up temporary directory
9. Returns JSON with source file path

---

### `debug::ExportImport() -> void`

**Brief:** Enables TRACE logging for the export/import module.

**Implementation:**
```cpp
void dbcfg::debug::ExportImport() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);
  dbcfg::debug::detail::ExportImport();
  TLOG(10) << "artdaq::database::configuration::ExportImport trace_enable";
}
```

---

## Key Algorithms

### Error Message Accumulation

Used in database export/import to collect errors from multiple operations:

```cpp
std::ostringstream err;
auto noerror_pos = err.tellp();

for (auto const& item : items) {
  auto result = process(item);
  if (!result.first) {
    err << result.second << ", ";
  }
}

if (err.tellp() != noerror_pos) {
  return Failure(err);
}
```

### Error Message JSON Formatting

Inline lambda for consistent error JSON format:

```cpp
inline auto make_error_msg = [](auto msg) {
  return std::string(R"({"error":")").append(msg).append("\"}");
};
```

---

## Performance Considerations

- **Parallel potential**: Database export iterates collections sequentially; could be parallelized
- **Memory usage**: All documents in a collection are loaded into memory during export
- **I/O bound**: Operations are dominated by file system and database I/O
- **Compression overhead**: bzip2 compression adds CPU overhead but reduces network/storage costs

## Error Handling Strategy

1. **Early validation**: Validates inputs (empty checks, system availability)
2. **Accumulate errors**: Collects all errors during batch operations
3. **Cleanup on error**: Attempts to delete temporary directories on failure
4. **Diagnostic preservation**: Captures full exception information

### Error Messages

| Error | Condition |
|-------|-----------|
| `msg_EmptyFilter` | JSON payload is empty |
| `msg_EmptyDocument` | Archive data is empty |
| `msg_SystemCallFailed` | `system()` is not available |
| `make_error_msg(...)` | Various operation-specific errors |

## Testing Notes

- **Unit tests:** `test/ConfigurationDB/ExportImport_t.cc`
- **Key test cases:**
  - Empty collection export (should fail)
  - Round-trip export/import
  - Format detection by file extension
  - Error handling for missing files
  - Database backup and restore

## Maintenance Notes

- **TRACE_NAME macro**: Defined as "dboperation_exportimport.cpp"
- The `system()` call check is performed early to fail fast if compression tools are unavailable
- Temporary directories use unique names to avoid conflicts in concurrent operations
- The `make_error_msg` lambda creates consistent error JSON format

## See Also

- [dboperation_exportimport.h.md](./dboperation_exportimport.h.md) - Header file
- [detail_exportimport.cpp.md](./detail_exportimport.cpp.md) - Additional detail implementation
- [dboperation_metadata.h.md](./dboperation_metadata.h.md) - List collections operation
- [JSONDocumentBuilder.h.md](../JsonDocument/JSONDocumentBuilder.h.md) - Document building

---

**Documentation generated for artdaq-database ConfigurationDB module**
