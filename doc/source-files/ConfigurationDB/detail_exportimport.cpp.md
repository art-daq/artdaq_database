# detail_exportimport.cpp

**Path:** `artdaq-database/ConfigurationDB/detail_exportimport.cpp`

**Implements:** [dboperation_exportimport.h](./dboperation_exportimport.h.md)

**Purpose:** Implements file-based export and import operations for configuration documents. This file provides functionality to read documents from the database and write them to files, as well as read documents from files and store them in the database. These operations are essential for backup, migration, and offline configuration management.

## Implementation Overview

This file implements four main operations:
1. `read_document_file` - Reads a document from the database and writes it to a file
2. `write_document_file` - Reads a document from a file and writes it to the database
3. `export_configuration` - Bulk export operation (not yet implemented)
4. `import_configuration` - Bulk import operation (not yet implemented)

The file I/O operations wrap the core document read/write functionality with file handling.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | Common utilities and confirm macro |
| `artdaq-database/ConfigurationDB/dboperation_exportimport.h` | Operation interface declaration |
| `artdaq-database/ConfigurationDB/shared_helper_functions.h` | File I/O helper functions (`read_buffer_from_file`, `write_buffer_to_file`) |
| `artdaq-database/DataFormats/shared_literals.h` | JSON literal definitions |
| `artdaq-database/SharedCommon/configuraion_api_literals.h` | API literal constants |
| `artdaq-database/ConfigurationDB/configuration_dbproviders.h` | Provider validation |
| `artdaq-database/BasicTypes/basictypes.h` | Basic type definitions |
| `artdaq-database/DataFormats/Json/json_common.h` | JSON utilities |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | JSON document construction |
| `artdaq-database/ConfigurationDB/options_operations.h` | Operation options classes |

## Type Definitions

### Result Type

```cpp
using result_t = artdaq::database::configuration::result_t;
```

A result type (typically `std::pair<bool, std::string>`) used to return success/failure status with optional error message.

## Internal Function Declarations

The file references detail functions from `detail_managedocument.cpp`:

```cpp
void write_document(ManageDocumentOperation const&, std::string&);
void read_document(ManageDocumentOperation const&, std::string&);
```

These are used to interact with the database while this file handles file I/O.

## Internal Functions

### `read_document_file(ManageDocumentOperation const& options, std::string const& file_out_name) -> result_t`

**Brief:** Reads a configuration document from the database and exports it to a file.

**Called by:** Export operations in the CLI tools (`bulkdownloader`) and bulk operations

**Purpose:** Export a single document from the database to the filesystem for backup or transfer.

**Parameters:**
- `options` - `ManageDocumentOperation const&` containing database query parameters
- `file_out_name` - `std::string const&` path to the output file

**Preconditions:**
- `file_out_name` must not be empty (enforced via `confirm()`)
- Parent directory of output file must exist and be writable

**Returns:** `result_t` - Success() on success, Failure(message) on failure

**Postconditions:**
- On success, output file contains the document content
- On failure, file may not exist or be incomplete

**Throws:** None directly - all exceptions are caught and converted to Failure results

**Thread Safety:** Not thread-safe - file operations are not synchronized

**Side Effects:**
- Creates or overwrites the output file
- TRACE logging at level 21

**Implementation Details:**

1. Validates non-empty filename using `confirm()`
2. Calls `detail::read_document()` to retrieve the document from the database
3. Writes the document content to the specified file using `write_buffer_to_file()`
4. Returns Success() or Failure() based on result

---

### `write_document_file(ManageDocumentOperation const& options, std::string const& file_src_name) -> result_t`

**Brief:** Reads a configuration document from a file and stores it in the database.

**Called by:** Import operations in the CLI tools (`bulkloader`) and bulk operations

**Purpose:** Import a single document from the filesystem into the database.

**Parameters:**
- `options` - `ManageDocumentOperation const&` containing database storage parameters
- `file_src_name` - `std::string const&` path to the source file

**Preconditions:**
- `file_src_name` must not be empty (enforced via `confirm()`)
- Source file must exist and be readable

**Returns:** `result_t` - Success() on success, Failure(message) on failure

**Postconditions:**
- On success, document is stored in the database
- Original file remains unchanged

**Throws:** None directly - all exceptions are caught and converted to Failure results

**Thread Safety:** Not thread-safe - file and database operations are not synchronized

**Side Effects:**
- Modifies database by adding/updating document
- TRACE logging (when debug file writing enabled)

**Implementation Details:**

1. Validates non-empty filename using `confirm()`
2. Reads the file content into a buffer using `read_buffer_from_file()`
3. Calls `detail::write_document()` to store the document in the database
4. Returns Success() or Failure() based on result

**Debug Feature:**
When `_WRITE_DEBUG_FILE_` is defined, the function also writes a debug copy of the imported document to a temporary file.

---

### `export_configuration(ManageDocumentOperation const&, std::string&) -> void`

**Brief:** Placeholder for bulk configuration export functionality (not yet implemented).

**Purpose:** Reserved for future bulk export operations that export entire configurations.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Always - function is not implemented |

**Note:** For bulk export operations, use the `bulkdownloader` utility which calls `read_document_file()` repeatedly.

---

### `import_configuration(ManageDocumentOperation const&, std::string&) -> void`

**Brief:** Placeholder for bulk configuration import functionality (not yet implemented).

**Purpose:** Reserved for future bulk import operations that import entire configurations.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Always - function is not implemented |

**Note:** For bulk import operations, use the `bulkloader` utility which calls `write_document_file()` repeatedly.

---

### `cftd::ExportImport() -> void`

**Brief:** Enables TRACE debugging for the export/import module by configuring trace output modes.

**Called by:** Debug initialization code

**Purpose:** Configure TRACE logging infrastructure for debugging export/import operations.

**Preconditions:** None

**Postconditions:**
- TRACE name set to "detail_exportimport.cpp"
- All trace levels enabled
- Trace modes configured per `trace_mode::modeM` and `trace_mode::modeS`

**Throws:** None

**Thread Safety:** Not thread-safe - should be called during initialization only

---

## Key Algorithms

### Document Export to File

**Steps:**
1. Validate the output filename is not empty
2. Call `detail::read_document()` to retrieve the document from the database
3. Write the document content to the specified file using `write_buffer_to_file()`
4. Return success or failure result

**Why this approach:** Separates file I/O concerns from database operations, allowing the same document read logic to be used for both API responses and file exports.

### Document Import from File

**Steps:**
1. Validate the source filename is not empty
2. Read the file content into a buffer using `read_buffer_from_file()`
3. Call `detail::write_document()` to store the document in the database
4. Return success or failure result

**Why this approach:** Enables loading configurations from files during system setup or recovery scenarios.

---

## Performance Considerations

- **Memory:** Entire document is loaded into memory before file I/O
- **Large files:** Memory usage scales with document size
- **Buffering:** Standard C++ file I/O buffering applies
- **No streaming:** Documents are fully loaded/stored, not streamed

## Error Handling Strategy

Errors are handled through a try-catch pattern that:
1. Catches all exceptions using `catch (...)`
2. Converts exceptions to `result_t` failures using `::debug::current_exception_diagnostic_information()`
3. Returns a Failure result with diagnostic information

```cpp
} catch (...) {
  return Failure(::debug::current_exception_diagnostic_information());
}
```

This approach ensures that:
- No exceptions propagate to callers
- Error information is preserved for diagnostics
- Both file and database errors are handled uniformly

## Testing Notes

- **Unit tests:** `test/ConfigurationDB/ConfigurationDB_t.cc`
- **Key test cases:**
  - Export existing document to file
  - Import valid JSON file to database
  - Handle missing source file gracefully
  - Handle write permission errors

## Maintenance Notes

The `_WRITE_DEBUG_FILE_` preprocessor flag enables additional debug file output during development. This code path is normally disabled but can be enabled for troubleshooting import operations.

```cpp
#ifdef _WRITE_DEBUG_FILE_
  std::cout << "Returned buffer:\n" << result.second << "\n";
  // ... write debug file ...
#endif
```

The bulk export/import functions (`export_configuration`, `import_configuration`) are placeholders for future implementation. The single-document operations (`read_document_file`, `write_document_file`) should be used via the `bulkloader` and `bulkdownloader` utilities for bulk operations.

## Common Pitfalls

- **Empty filename**: Will cause assertion failure via `confirm()`
- **File permissions**: Ensure write permissions for export, read permissions for import
- **Memory usage**: Large documents consume equivalent memory
- **Not implemented**: The bulk functions always throw; use CLI utilities instead

## See Also

- [dboperation_exportimport.h](./dboperation_exportimport.h.md) - Public interface
- [shared_helper_functions.h](./shared_helper_functions.h.md) - File I/O utilities
- [detail_managedocument.cpp](./detail_managedocument.cpp.md) - Core document read/write operations
- [Utilities/bulkloader.cc](../Utilities/bulkloader.cc.md) - Bulk import CLI tool
- [Utilities/bulkdownloader.cc](../Utilities/bulkdownloader.cc.md) - Bulk export CLI tool

---

**Documentation generated for artdaq-database ConfigurationDB module**
