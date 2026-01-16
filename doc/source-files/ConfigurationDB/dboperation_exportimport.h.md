# dboperation_exportimport.h

**Path:** `artdaq-database/ConfigurationDB/dboperation_exportimport.h`

**Purpose:** Declares functions for exporting and importing configurations, collections, and entire databases. These operations support bulk data transfer between the database and file system using compressed archive formats (tar.bz2 encoded as base64).


## Key Concepts

### Bulk Operations

This header defines hierarchical bulk operations for transferring configuration data:

| Level | Description | Functions |
|-------|-------------|-----------|
| **Configuration** | Single configuration with associated documents | `export_configuration`, `import_configuration` |
| **Collection** | All documents in a single collection | `export_collection`, `import_collection` |
| **Database** | All collections in a database | `export_database`, `import_database` |

### Archive Format

All bulk operations use a compressed archive format:
- **Container**: tar archive
- **Compression**: bzip2
- **Transport encoding**: base64 (for safe transmission over text-based protocols)
- **File extension**: `.artdaq`

### Two-Namespace Pattern

This header follows the ConfigurationDB two-namespace pattern:
- **opts namespace**: Takes `ManageDocumentOperation` objects directly for programmatic use
- **json namespace**: Takes JSON string payloads for REST API compatibility and includes additional read/write functions

## Thread Safety

- **Thread-safe:** Conditional
- **Concurrent access:** Multiple threads can call different export/import functions concurrently
- **Locking:** No internal locking; thread safety depends on the underlying storage provider and file system operations
- **File operations:** Uses temporary directories to avoid conflicts

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/SharedCommon/returned_result.h` | Provides `result_t` type for operation results |

## Forward Declarations

### `ManageDocumentOperation`

Forward-declared class from `artdaq::database::configuration` namespace that encapsulates operation parameters including source/destination file paths, collection names, and format settings.

## Functions

### Namespace: `opts`

---

#### `export_configuration(options) -> result_t`

**Brief:** Exports a configuration and its associated documents to a compressed archive file on the file system.

**Parameters:**
- `options` - `ManageDocumentOperation const&` - Export configuration containing:
  - `sourceFileName` - Output file path for the archive
  - `configuration` - Name of the configuration to export
  - `collection` - Collection containing the configuration
  - `provider` - Storage provider to read from

**Preconditions:**
- The configuration must exist in the database
- The output directory must be writable
- System compression utilities (tar, bzip2) must be available

**Returns:** `result_t` - A pair where:
- `first` (bool): `true` if export completed successfully
- `second` (string): On success, contains the export result; on failure, contains an error message

**Postconditions:**
- On success, a `.artdaq` archive file is created at the specified path
- The archive contains all documents associated with the configuration

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Function is `noexcept`; all exceptions are caught and returned as failures |

**Thread Safety:** Conditional - safe for concurrent exports to different files

**Side Effects:**
- Creates files on the file system
- Uses system calls for compression
- Creates and deletes temporary directories

---

#### `import_configuration(options) -> result_t`

**Brief:** Imports a configuration and its associated documents from a compressed archive file into the database.

**Parameters:**
- `options` - `ManageDocumentOperation const&` - Import configuration containing:
  - `sourceFileName` - Input archive file path
  - `configuration` - Name for the imported configuration
  - `collection` - Target collection
  - `provider` - Storage provider to write to

**Preconditions:**
- The source archive file must exist and be readable
- The archive must be in valid `.artdaq` format
- System decompression utilities must be available

**Returns:** `result_t` - A pair where:
- `first` (bool): `true` if import completed successfully
- `second` (string): On success, contains the import result; on failure, contains an error message

**Postconditions:**
- On success, the configuration and all associated documents are stored in the database
- Existing documents with the same identifiers may be updated or duplicated based on settings

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Function is `noexcept`; all exceptions are caught and returned as failures |

**Thread Safety:** Conditional - multiple imports can run concurrently to different collections

**Side Effects:**
- Writes documents to the database
- Uses system calls for decompression
- Creates and deletes temporary directories

---

#### `export_database(options) -> result_t`

**Brief:** Exports all collections in a database to a directory of archive files, one archive per collection.

**Parameters:**
- `options` - `ManageDocumentOperation const&` - Export configuration containing:
  - `sourceFileName` - Output directory path for archives

**Preconditions:**
- The output directory must exist and be writable
- System compression utilities must be available

**Returns:** `result_t` - A pair where:
- `first` (bool): `true` if all collections exported successfully
- `second` (string): On success, JSON array of exported file paths; on failure, error messages

**Postconditions:**
- On success, one `.artdaq` file is created for each collection in the database
- Files are named `<collection_name>.artdaq`

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Function is `noexcept`; all exceptions are caught and returned as failures |

**Thread Safety:** Not thread-safe - should not run concurrent database exports

**Side Effects:**
- Creates multiple files in the output directory
- Uses system calls for compression
- Lists all collections before export

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_exportimport.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include <iostream>

using namespace artdaq::database::configuration;

void backupDatabase() {
  ManageDocumentOperation opts{"exportdatabase"};
  opts.sourceFileName("/backup/my_database");

  auto result = opts::export_database(opts);
  if (result.first) {
    std::cout << "Backup complete: " << result.second << std::endl;
  } else {
    std::cerr << "Backup failed: " << result.second << std::endl;
  }
}
```

---

#### `import_database(options) -> result_t`

**Brief:** Imports all collection archives from a directory into the database, restoring the entire database state.

**Parameters:**
- `options` - `ManageDocumentOperation const&` - Import configuration containing:
  - `sourceFileName` - Input directory path containing `.artdaq` files

**Preconditions:**
- The input directory must exist and contain valid `.artdaq` files
- System decompression utilities must be available

**Returns:** `result_t` - A pair where:
- `first` (bool): `true` if all collections imported successfully
- `second` (string): On success, JSON array of imported files; on failure, error messages

**Postconditions:**
- On success, all collections from the archives are restored in the database

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Function is `noexcept`; all exceptions are caught and returned as failures |

**Thread Safety:** Not thread-safe - should not run concurrent database imports

**Side Effects:**
- Writes many documents to the database
- Uses system calls for decompression
- Lists all archive files in directory

---

#### `export_collection(options) -> result_t`

**Brief:** Exports all documents in a single collection to a compressed archive file.

**Parameters:**
- `options` - `ManageDocumentOperation const&` - Export configuration containing:
  - `sourceFileName` - Output file path for the archive
  - `collection` - Name of the collection to export

**Preconditions:**
- The collection must exist and contain at least one document
- The output directory must be writable

**Returns:** `result_t` - A pair where:
- `first` (bool): `true` if export completed successfully
- `second` (string): On success, JSON with result file path; on failure, error message

**Postconditions:**
- On success, a `.artdaq` archive file is created containing all collection documents

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Function is `noexcept`; all exceptions are caught and returned as failures |

**Thread Safety:** Conditional - safe for concurrent exports of different collections

**Side Effects:**
- Creates files on the file system
- Uses system calls for compression
- Creates and deletes temporary directories

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_exportimport.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include <iostream>

using namespace artdaq::database::configuration;

void exportSingleCollection() {
  ManageDocumentOperation opts{"exportcollection"};
  opts.collection("DAQConfigs");
  opts.sourceFileName("/backup/DAQConfigs.artdaq");

  auto result = opts::export_collection(opts);
  if (result.first) {
    std::cout << "Export complete: " << result.second << std::endl;
  } else {
    std::cerr << "Export failed: " << result.second << std::endl;
  }
}
```

---

#### `import_collection(options) -> result_t`

**Brief:** Imports all documents from a compressed archive file into a collection.

**Parameters:**
- `options` - `ManageDocumentOperation const&` - Import configuration containing:
  - `sourceFileName` - Input archive file path
  - `collection` - Target collection name

**Preconditions:**
- The source archive file must exist and be valid
- System decompression utilities must be available

**Returns:** `result_t` - A pair where:
- `first` (bool): `true` if import completed successfully
- `second` (string): On success, JSON with source file path; on failure, error message

**Postconditions:**
- On success, all documents from the archive are stored in the specified collection

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Function is `noexcept`; all exceptions are caught and returned as failures |

**Thread Safety:** Conditional - safe for concurrent imports to different collections

**Side Effects:**
- Writes documents to the database
- Uses system calls for decompression
- Creates and deletes temporary directories

---

### Namespace: `json`

---

#### `write_configuration(task_payload, conf) -> result_t`

**Brief:** Writes configuration data provided as a base64-encoded tar.bz2 archive directly to the database, typically used for REST API uploads.

**Parameters:**
- `task_payload` - `std::string const&` - JSON string with operation parameters
- `conf` - `std::string const&` - Base64-encoded tar.bz2 archive containing configuration files

**Preconditions:**
- `task_payload` must be a valid, non-empty JSON string
- `conf` must be a valid base64-encoded tar.bz2 archive

**Returns:** `result_t` - A pair where:
- `first` (bool): `true` if write completed successfully
- `second` (string): On success, empty; on failure, error message

**Postconditions:**
- Configuration documents are written to the database

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Function is `noexcept`; all exceptions are caught and returned as failures |

**Thread Safety:** Conditional - safe for concurrent writes with different configurations

**Side Effects:**
- Creates temporary files for decompression
- Writes documents to the database
- Cleans up temporary files after completion

---

#### `read_configuration(task_payload, conf) -> result_t`

**Brief:** Reads a configuration from the database and returns it as a base64-encoded tar.bz2 archive, typically used for REST API downloads.

**Parameters:**
- `task_payload` - `std::string const&` - JSON string with query parameters
- `conf` - `std::string&` - Output buffer for the base64-encoded archive (must be empty)

**Preconditions:**
- `task_payload` must be a valid, non-empty JSON string
- `conf` must be an empty string

**Returns:** `result_t` - A pair where:
- `first` (bool): `true` if read completed successfully
- `second` (string): On success, contains the base64 archive; on failure, error message

**Postconditions:**
- On success, `conf` contains `{"collection.tar.bzip2.base64":"<base64_data>"}`

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Function is `noexcept`; all exceptions are caught and returned as failures |

**Thread Safety:** Conditional - safe for concurrent reads

**Side Effects:**
- Creates temporary files for compression
- Reads from the database
- Cleans up temporary files after completion

---

#### `export_configuration(task_payload) -> result_t`

**Brief:** JSON-based interface for exporting a configuration to an archive file.

**Parameters:**
- `task_payload` - `std::string const&` - JSON string with export parameters

**Returns:** `result_t` - Success with result or failure with error message

**Thread Safety:** Conditional

---

#### `import_configuration(task_payload) -> result_t`

**Brief:** JSON-based interface for importing a configuration from an archive file.

**Parameters:**
- `task_payload` - `std::string const&` - JSON string with import parameters

**Returns:** `result_t` - Success with result or failure with error message

**Thread Safety:** Conditional

---

#### `export_database(task_payload) -> result_t`

**Brief:** JSON-based interface for exporting all collections in a database.

**Parameters:**
- `task_payload` - `std::string const&` - JSON string with export parameters

**Returns:** `result_t` - Success with JSON array of exported files or failure with error message

**Thread Safety:** Not thread-safe

---

#### `import_database(task_payload) -> result_t`

**Brief:** JSON-based interface for importing all collections from archive files.

**Parameters:**
- `task_payload` - `std::string const&` - JSON string with import parameters

**Returns:** `result_t` - Success with JSON array of imported files or failure with error message

**Thread Safety:** Not thread-safe

---

#### `export_collection(task_payload) -> result_t`

**Brief:** JSON-based interface for exporting a single collection to an archive file.

**Parameters:**
- `task_payload` - `std::string const&` - JSON string with export parameters

**Returns:** `result_t` - Success with result or failure with error message

**Thread Safety:** Conditional

---

#### `import_collection(task_payload) -> result_t`

**Brief:** JSON-based interface for importing a collection from an archive file.

**Parameters:**
- `task_payload` - `std::string const&` - JSON string with import parameters

**Returns:** `result_t` - Success with result or failure with error message

**Thread Safety:** Conditional

---

### Namespace: `debug`

---

#### `ExportImport() -> void`

**Brief:** Enables TRACE logging for the export/import module at the public API level.

**Parameters:** None

**Returns:** void

**Postconditions:**
- TRACE logging is enabled for export/import operations

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Function does not throw |

**Thread Safety:** Not thread-safe - should be called once at initialization

---

#### `detail::ExportImport() -> void`

**Brief:** Enables TRACE logging for the internal detail implementation of export/import operations.

**Parameters:** None

**Returns:** void

**Postconditions:**
- TRACE logging is enabled for internal export/import implementation

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Function does not throw |

**Thread Safety:** Not thread-safe - should be called once at initialization

---

### Global Functions

---

#### `set_default_locale() -> void`

**Brief:** Sets the default locale for string processing in the database module, ensuring consistent character encoding handling.

**Parameters:** None

**Returns:** void

**Postconditions:**
- The default locale is configured for the process

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Function does not throw |

**Thread Safety:** Not thread-safe - should be called once at initialization

---

## Relationship to Other Components

```
dboperation_exportimport.h
        |
        +-- ManageDocumentOperation (operation parameters)
        |
        +-- dboperation_exportimport.cpp (public wrapper implementation)
        |
        +-- detail_exportimport.cpp (internal helper functions)
        |
        +-- dboperation_managedocument.h (document read/write)
        |
        +-- dboperation_metadata.h (list collections for database export)
        |
        +-- StorageProviders (actual database read/write)
        |
        +-- filesystem_functions (archive handling)
```

- **ManageDocumentOperation**: Provides operation parameters for all export/import functions
- **dboperation_exportimport.cpp**: Contains the implementation of all public functions
- **detail_exportimport.cpp**: Contains internal helper functions
- **dboperation_managedocument.h**: Used for individual document read/write
- **dboperation_metadata.h**: Used to list collections for database export
- **StorageProviders**: Actual database read/write operations
- **filesystem_functions**: File system operations for archive handling

## Notes for Developers

### Common Pitfalls

- **System Availability**: Functions use `system()` to invoke tar and bzip2; ensure these utilities are available
- **Empty Collections**: Exporting an empty collection returns a failure
- **Buffer State**: `read_configuration` requires `conf` to be empty; non-empty buffers cause failure
- **Temporary Files**: Operations create temporary directories that are cleaned up on success; failures may leave orphan directories

### Anti-patterns

```cpp
// DON'T do this: Providing non-empty output buffer
std::string conf = "existing data";
json::read_configuration(payload, conf);  // Will fail!

// DO this instead: Ensure buffer is empty
std::string conf;
auto result = json::read_configuration(payload, conf);
if (result.first) {
  // Process conf
}
```

### Performance Considerations

1. **Large Collections**: Exporting large collections may be slow due to compression overhead
2. **Disk I/O**: Operations are I/O bound; use SSDs for better performance
3. **Memory Usage**: Large archives are processed in memory during base64 encoding/decoding
4. **Parallel Exports**: Multiple collection exports can run in parallel for faster database backups

### File Format Details

The `.artdaq` archive format:
- Inner format: tar archive containing JSON files (one per document)
- Compression: bzip2
- Transport encoding: base64 (for `read_configuration`/`write_configuration`)
- Document naming: `<object_ouid>.json`

### Typical Usage Pattern

```cpp
#include "artdaq-database/ConfigurationDB/dboperation_exportimport.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include <iostream>

using namespace artdaq::database::configuration;

void performBackup() {
  // Export entire database
  ManageDocumentOperation exportOpts{"exportdatabase"};
  exportOpts.sourceFileName("/backup/2024-01-15");

  auto result = opts::export_database(exportOpts);
  if (!result.first) {
    std::cerr << "Backup failed: " << result.second << std::endl;
    return;
  }

  std::cout << "Backup complete. Files: " << result.second << std::endl;
}

void restoreFromBackup() {
  // Import entire database
  ManageDocumentOperation importOpts{"importdatabase"};
  importOpts.sourceFileName("/backup/2024-01-15");

  auto result = opts::import_database(importOpts);
  if (!result.first) {
    std::cerr << "Restore failed: " << result.second << std::endl;
    return;
  }

  std::cout << "Restore complete. Files: " << result.second << std::endl;
}
```

## See Also

- [dboperation_exportimport.cpp.md](./dboperation_exportimport.cpp.md) - Implementation details
- [detail_exportimport.cpp.md](./detail_exportimport.cpp.md) - Internal helper functions
- [dboperation_managedocument.h.md](./dboperation_managedocument.h.md) - Individual document operations
- [options_operation_managedocument.h.md](./options_operation_managedocument.h.md) - ManageDocumentOperation class definition
- [storage_providers.h.md](../StorageProviders/storage_providers.h.md) - Storage provider interface
- [returned_result.h.md](../SharedCommon/returned_result.h.md) - Result type definition

---

**Documentation generated for artdaq-database ConfigurationDB module**
