# fileststem_functions.h

**Path:** `artdaq-database/SharedCommon/fileststem_functions.h`

**Purpose:** Filesystem utility functions for directory operations, file I/O, path conversions, and archive/compression. Used by the FileSystemDB storage provider and database export/import operations.

**Note:** Filename has historical typo ("fileststem" instead of "filesystem") - preserved for backward compatibility.

## Key Concepts

### Collection Name to Path Conversion

Collection names use dots as separators while filesystem paths use slashes:
```
"SystemLayout.component1.json" <-> "SystemLayout/component1.json"
```

### Archive Format

Database exports use tar + bzip2 + base64 encoding for the following benefits:
- Portable across systems
- Text-safe (base64 encoding)
- Good compression ratio (bzip2)
- Single-file output for easy transfer

## Thread Safety

- **Thread-safe:** No
- **Notes:** Functions that call `system()` are not thread-safe. Functions using `rand()` for temp directory names are not thread-safe. File I/O operations may have race conditions if multiple processes access the same files.

## Dependencies

| Include | Purpose |
|---------|---------|
| `<list>` | List container for path components |
| `<string>` | String class |
| `<vector>` | Container for file lists |
| `shared_datatypes.h` | Type definitions |

## Functions

### Directory Listing

#### `list_files(std::string const& path) -> std::vector<std::string>`

**Brief:** Recursively lists all regular files in a directory and its subdirectories. Skips symbolic links and special files.

**Parameters:**
- `path` - Directory path to scan

**Preconditions:**
- `path` must not be empty
- `path` must point to an existing directory

**Returns:** Vector of full file paths for all regular files found

**Thread Safety:** Safe (read-only filesystem operation)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/fileststem_functions.h"
#include <iostream>

void processAllConfigs(const std::string& configDir) {
    try {
        auto files = artdaq::database::list_files(configDir);
        std::cout << "Found " << files.size() << " files\n";
        for (const auto& f : files) {
            processFile(f);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error listing files: " << e.what() << "\n";
    }
}
```

---

### Directory Creation

#### `mkdir(std::string const& path) -> bool`

**Brief:** Creates a directory path including all parent directories (equivalent to `mkdir -p`).

**Parameters:**
- `path` - Directory path to create

**Preconditions:**
- `path` must not be empty

**Returns:** `true` if directory was created successfully, `false` otherwise

**Thread Safety:** Not thread-safe (uses `system()`)

**Side Effects:**
- Creates directories on the filesystem

**Example:**
```cpp
#include "artdaq-database/SharedCommon/fileststem_functions.h"
#include <iostream>

void ensureDirectoryExists(const std::string& path) {
    if (artdaq::database::mkdir(path)) {
        std::cout << "Directory created: " << path << "\n";
    } else {
        std::cerr << "Failed to create directory: " << path << "\n";
    }
}
```

---

#### `mkdirfile(std::string const& file) -> bool`

**Brief:** Creates the parent directory structure for a file path, allowing the file to be written.

**Parameters:**
- `file` - Full file path whose parent directory should be created

**Preconditions:**
- `file` must not be empty

**Returns:** `true` if parent directory was created successfully, `false` otherwise

**Thread Safety:** Not thread-safe (uses `system()`)

**Side Effects:**
- Creates directories on the filesystem

**Example:**
```cpp
#include "artdaq-database/SharedCommon/fileststem_functions.h"

void prepareForWrite(const std::string& filepath) {
    if (artdaq::database::mkdirfile(filepath)) {
        // Parent directory now exists, safe to write file
        writeFile(filepath);
    }
}
```

---

### File I/O

#### `write_buffer_to_file(std::string const& buffer, std::string const& file_out_name) -> bool`

**Brief:** Writes a string buffer to a file, creating parent directories if needed.

**Parameters:**
- `buffer` - String content to write
- `file_out_name` - Full path to the output file

**Preconditions:**
- `file_out_name` must not be empty

**Returns:** `true` if file was written successfully

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Unable to create parent directory or write file |

**Thread Safety:** Not thread-safe (file write operation)

**Side Effects:**
- Creates/overwrites file on the filesystem
- Creates parent directories if they do not exist

**Example:**
```cpp
#include "artdaq-database/SharedCommon/fileststem_functions.h"
#include <iostream>

void saveConfiguration(const std::string& json, const std::string& path) {
    try {
        artdaq::database::write_buffer_to_file(json, path);
        std::cout << "Configuration saved to: " << path << "\n";
    } catch (const artdaq::database::runtime_error& e) {
        std::cerr << "Failed to save: " << e.what() << "\n";
    }
}
```

---

#### `read_buffer_from_file(std::string& buffer, std::string const& file_in_name) -> bool`

**Brief:** Reads the entire contents of a file into a string buffer.

**Parameters:**
- `buffer` - Output string to receive file contents (must be empty)
- `file_in_name` - Full path to the input file

**Preconditions:**
- `file_in_name` must not be empty
- `buffer` must be empty

**Returns:** `true` if file was read successfully

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_argument` | File could not be opened |
| `runtime_error` | Error during file read |

**Thread Safety:** Safe for reading (may race with concurrent writers)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/fileststem_functions.h"
#include <iostream>

void loadConfiguration(const std::string& path) {
    std::string content;
    try {
        artdaq::database::read_buffer_from_file(content, path);
        std::cout << "Loaded " << content.size() << " bytes\n";
        processConfiguration(content);
    } catch (const artdaq::database::invalid_argument& e) {
        std::cerr << "File not found: " << e.what() << "\n";
    } catch (const artdaq::database::runtime_error& e) {
        std::cerr << "Read error: " << e.what() << "\n";
    }
}
```

---

### Path Conversion

#### `collection_name_from_relative_path(std::string const& path) -> std::string`

**Brief:** Converts a relative file path to a collection name by replacing path separators with dots.

**Parameters:**
- `path` - Relative file path (e.g., `"Dir/file.json"`)

**Returns:** Collection name with dots (e.g., `"Dir.file.json"`)

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/fileststem_functions.h"

auto name = artdaq::database::collection_name_from_relative_path("SystemLayout/component1.json");
// name == "SystemLayout.component1.json"
```

---

#### `relative_path_from_collection_name(std::string const& collection_name) -> std::string`

**Brief:** Converts a collection name back to a relative file path by replacing dots with path separators (except for the file extension).

**Parameters:**
- `collection_name` - Collection name with dots (e.g., `"Dir.file.json"`)

**Preconditions:**
- `collection_name` must not be empty

**Returns:** Relative file path with slashes (e.g., `"Dir/file.json"`)

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/fileststem_functions.h"

auto path = artdaq::database::relative_path_from_collection_name("SystemLayout.component1.json");
// path == "SystemLayout/component1.json"
```

---

### Temporary Directory Management

#### `make_temp_dir() -> std::string`

**Brief:** Creates a temporary directory with a random name in the system temp location.

**Returns:** Full path to the created temporary directory (e.g., `/tmp/adb1234567`)

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Unable to create temporary directory |

**Thread Safety:** Not thread-safe (uses `rand()` and `system()`)

**Side Effects:**
- Creates a directory on the filesystem

**Example:**
```cpp
#include "artdaq-database/SharedCommon/fileststem_functions.h"
#include <iostream>

void processWithTempStorage() {
    std::string tmpDir;
    try {
        tmpDir = artdaq::database::make_temp_dir();
        std::cout << "Using temp directory: " << tmpDir << "\n";
        // ... use temp directory for processing ...
        artdaq::database::delete_temp_dir(tmpDir);
    } catch (const artdaq::database::runtime_error& e) {
        std::cerr << "Temp dir error: " << e.what() << "\n";
        if (!tmpDir.empty()) {
            artdaq::database::delete_temp_dir(tmpDir);
        }
    }
}
```

---

#### `delete_temp_dir(std::string const& tmp_dir_name) -> void`

**Brief:** Recursively deletes a temporary directory. For safety, only deletes directories with the expected temp directory prefix.

**Parameters:**
- `tmp_dir_name` - Full path to the temporary directory to delete

**Preconditions:**
- `tmp_dir_name` must not be empty
- `tmp_dir_name` must start with the expected temp directory prefix

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Unable to delete directory |

**Thread Safety:** Not thread-safe (uses `system()`)

**Side Effects:**
- Recursively deletes directory and all contents

**Example:**
```cpp
#include "artdaq-database/SharedCommon/fileststem_functions.h"

void cleanupTempDirectory(const std::string& tmpDir) {
    try {
        artdaq::database::delete_temp_dir(tmpDir);
    } catch (const artdaq::database::runtime_error& e) {
        // Log error but don't propagate - cleanup is best-effort
        std::cerr << "Warning: Failed to delete temp dir: " << e.what() << "\n";
    }
}
```

---

### Archive Operations

#### `dir_to_tarbzip2base64(std::string const& tmp_dir_name, std::string const& bzip2base64) -> std::string const&`

**Brief:** Compresses a directory into a tar archive, bzip2 compresses it, and encodes the result as base64 text. Used for database exports.

**Parameters:**
- `tmp_dir_name` - Source directory to compress
- `bzip2base64` - Output file path for the base64-encoded archive

**Preconditions:**
- `tmp_dir_name` must not be empty
- `bzip2base64` must not be empty

**Returns:** Reference to `bzip2base64` parameter

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Unable to create output directory or create archive |

**Thread Safety:** Not thread-safe (uses `system()`)

**Side Effects:**
- Creates file on filesystem
- Invokes external commands (tar, bzip2, base64)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/fileststem_functions.h"
#include <iostream>

void exportDatabase(const std::string& sourceDir, const std::string& outputFile) {
    try {
        artdaq::database::dir_to_tarbzip2base64(sourceDir, outputFile);
        std::cout << "Exported to: " << outputFile << "\n";
    } catch (const artdaq::database::runtime_error& e) {
        std::cerr << "Export failed: " << e.what() << "\n";
    }
}
```

---

#### `tarbzip2base64_to_dir(std::string const& bzip2base64, std::string const& tmp_dir_name) -> std::string const&`

**Brief:** Decodes a base64-encoded archive, decompresses it, and extracts the contents to a directory. Used for database imports.

**Parameters:**
- `bzip2base64` - Input file path containing base64-encoded archive
- `tmp_dir_name` - Destination directory for extracted files

**Preconditions:**
- `bzip2base64` must not be empty
- `tmp_dir_name` must not be empty

**Returns:** Reference to `tmp_dir_name` parameter

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Unable to create destination directory or extract archive |

**Thread Safety:** Not thread-safe (uses `system()`)

**Side Effects:**
- Creates directories and files on filesystem
- Invokes external commands (cat, base64, tar)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/fileststem_functions.h"
#include <iostream>

void importDatabase(const std::string& archiveFile, const std::string& destDir) {
    try {
        artdaq::database::tarbzip2base64_to_dir(archiveFile, destDir);
        std::cout << "Imported to: " << destDir << "\n";
    } catch (const artdaq::database::runtime_error& e) {
        std::cerr << "Import failed: " << e.what() << "\n";
    }
}
```

---

### File Metadata

#### `extract_collectionname_from_filename(std::string const& file_name, std::string& collection_name) -> bool`

**Brief:** Extracts the collection name from an export filename by removing the archive file extension.

**Parameters:**
- `file_name` - Export filename (e.g., `"MyCollection.tar-bzip2-base64"`)
- `collection_name` - Output parameter for the extracted collection name

**Preconditions:**
- `file_name` must not be empty

**Returns:** `true` if extraction was successful (file has correct extension), `false` otherwise

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/fileststem_functions.h"
#include <iostream>

void processExportFile(const std::string& filename) {
    std::string collection;
    if (artdaq::database::extract_collectionname_from_filename(filename, collection)) {
        std::cout << "Collection: " << collection << "\n";
    } else {
        std::cerr << "Invalid export file format: " << filename << "\n";
    }
}
```

---

## Relationship to Other Components

- [fileststem_functions.cpp](./fileststem_functions.cpp.md) - Implementation file
- **StorageProviders/FileSystemDB/** - Primary user for filesystem-based storage
- **ConfigurationDB/BulkOperations/** - Uses for export/import operations
- [configuraion_api_literals.h](./configuraion_api_literals.h.md) - Defines `tmpdirprefix`, `dbexport_extension`

## Notes for Developers

### Error Handling

Functions use multiple error handling strategies:
- Boolean return values for simple success/failure
- Exceptions (`runtime_error`, `invalid_argument`) for unexpected errors
- `confirm()` assertions for preconditions
- TRACE logging for debugging

### Security Considerations

- `delete_temp_dir()` only deletes paths starting with the expected temp directory prefix to prevent accidental deletion of important directories
- Archive operations use shell commands - ensure inputs are sanitized if accepting user-provided paths

### System Requirements

Archive operations (`dir_to_tarbzip2base64`, `tarbzip2base64_to_dir`) use `system()` calls to shell commands. The following commands must be available:
- `tar` - Archive creation/extraction
- `bzip2` / `bunzip2` - Compression
- `base64` - Base64 encoding/decoding

## Common Pitfalls

- **Filename typo:** The filename `fileststem` contains a historical typo. This is intentional for backward compatibility and should not be "fixed".
- **Thread safety:** Most functions use `system()` calls and are not thread-safe. Ensure proper synchronization when using in multi-threaded contexts.
- **Temp directory collisions:** `make_temp_dir()` uses `rand()` seeded with `time(nullptr)`, which may cause collisions if called multiple times within the same second.

## See Also

- [fileststem_functions.cpp](./fileststem_functions.cpp.md) - Implementation details
- [configuraion_api_literals.h](./configuraion_api_literals.h.md) - Filesystem-related constants
