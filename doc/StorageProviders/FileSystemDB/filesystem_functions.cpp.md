# filesystem_functions.cpp

**Path:** `artdaq-database/StorageProviders/FileSystemDB/filesystem_functions.cpp`

**Implements:** [provider_filedb.h](./provider_filedb.h.md)

**Purpose:** Implements filesystem utility functions specific to the FileSystemDB storage provider, providing directory creation with permission validation, subdirectory discovery for collection enumeration, document file discovery, and file existence checking.

## Implementation Overview

This file provides a set of utility functions that abstract filesystem operations for the FileSystemDB provider. All functions handle the `filesystemdb://` URI scheme transparently, stripping the prefix before performing operations. The functions use Boost.Filesystem for cross-platform compatibility and include detailed error messages with TRACE logging for debugging.

## Key Algorithms

### URI Scheme Handling

All functions implement the same URI stripping pattern:

```cpp
if (dir.find(dbfsl::FILEURI) == 0) {
    dir = dir.substr(strlen(dbfsl::FILEURI));
}
```

This allows callers to pass either:
- Full URIs: `filesystemdb:///data/configs`
- Plain paths: `/data/configs`

**Why this approach:** The storage provider layer uses URIs for consistency with other backends (MongoDB), but filesystem operations require plain paths.

### Directory Creation with Validation

**Steps:**
1. Strip the URI scheme prefix if present
2. Check if the path already exists
3. If it exists as a directory, validate owner read/write permissions
4. If it exists but is not a directory, throw an error
5. If it does not exist, create the directory and all parent directories
6. Return the cleaned path (without URI prefix)

**Why this approach:** Permission validation catches configuration errors early, and recursive creation (`mkdir -p` style) simplifies caller code.

## Internal Functions

### `mkdir(std::string const& d) -> std::string`

**Brief:** Creates a directory and all parent directories, validating that existing directories have proper read/write permissions.

**Called by:** `provider_connection.cpp`, `provider_filedb_readwrite.cpp`, `provider_filedb.cpp`

**Purpose:** Provides a robust directory creation function that handles URI schemes, validates permissions, and creates the full directory hierarchy as needed.

**Parameters:**
- `d` - Directory path or URI to create. Must not be empty.

**Preconditions:**
- `d` must not be empty (enforced by `confirm()`)
- The parent directory must be writable (to create new directories)

**Returns:** The directory path with the URI scheme stripped (plain filesystem path).

**Postconditions:**
- The directory exists on the filesystem
- The directory has owner read and write permissions
- All parent directories in the path exist

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` (via `confirm()`) | `d` is empty |
| `runtime_error("FileSystemDB")` | Path exists but is not a directory |
| `runtime_error("FileSystemDB")` | Directory exists but lacks owner read/write permissions |
| `runtime_error("FileSystemDB")` | Directory creation fails (permissions, disk full, invalid path) |

**Thread Safety:** Conditional. Multiple threads can safely call with different paths. Calls with the same path may race, but directory creation is idempotent.

**Side Effects:**
- Creates directories on the filesystem
- Logs to TRACE at level 21

**Complexity:** O(p) where p is the number of path components to create.

**Example:**
```cpp
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"

void example() {
    try {
        // Both work identically:
        auto path1 = dbfs::mkdir("filesystemdb:///data/configs/MyCollection");
        // path1 = "/data/configs/MyCollection"

        auto path2 = dbfs::mkdir("/data/configs/AnotherCollection");
        // path2 = "/data/configs/AnotherCollection"
    } catch (const runtime_error& e) {
        std::cerr << "Failed to create directory: " << e.what() << std::endl;
    }
}
```

### `find_subdirs(std::string const& d) -> std::list<std::string>`

**Brief:** Finds all subdirectories within a given directory, returning their names (not full paths) for collection discovery.

**Called by:** `provider_filedb.cpp` for listing available collections

**Purpose:** Enables discovery of collections within a database. Each collection is stored as a subdirectory of the database root.

**Parameters:**
- `d` - Directory path to search for subdirectories. Must not be empty.

**Preconditions:**
- `d` must not be empty (enforced by `confirm()`)
- The directory must exist

**Returns:** A list of subdirectory names (not full paths). Empty list if no subdirectories exist.

**Postconditions:**
- The returned list contains only directory names, not files
- Each name is just the directory name, not a full path

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` (via `confirm()`) | `d` is empty |
| `runtime_error("FileSystemDB")` | Directory does not exist |
| `runtime_error("FileSystemDB")` | Path is not a directory |

**Thread Safety:** Safe for reads. Unsafe if directories are being created/deleted concurrently.

**Side Effects:**
- Reads from filesystem
- Logs to TRACE at level 21

**Complexity:** O(n) where n is the number of entries in the directory.

**Example:**
```cpp
auto collections = dbfs::find_subdirs("/data/configs");
// collections = ["ComponentConfigs", "RunConfigurations", "SystemMetadata"]

for (const auto& name : collections) {
    std::cout << "Found collection: " << name << std::endl;
}
```

### `find_siblingdirs(std::string const& d) -> std::list<std::string>`

**Brief:** Finds sibling directories of the given directory by listing subdirectories of its parent directory.

**Called by:** Database discovery operations

**Purpose:** Enables discovery of sibling databases or collections at the same level in the directory hierarchy.

**Parameters:**
- `d` - Reference directory path. Must not be empty.

**Preconditions:**
- `d` must not be empty (enforced by `confirm()`)
- The parent directory of `d` must exist

**Returns:** A list of directory names at the same level as `d`, including `d` itself.

**Postconditions:**
- The returned list contains directory names from the parent of `d`

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` (via `confirm()`) | `d` is empty |
| `runtime_error("FileSystemDB")` | Parent directory does not exist |

**Thread Safety:** Safe for reads. Unsafe if directories are being created/deleted concurrently.

**Side Effects:**
- Reads from filesystem
- Logs to TRACE at level 21

**Complexity:** O(n) where n is the number of entries in the parent directory.

**Example:**
```cpp
// Given: /data/configs/db1, /data/configs/db2, /data/configs/db3
auto siblings = dbfs::find_siblingdirs("/data/configs/db1");
// siblings = ["db1", "db2", "db3"]
```

### `find_documents(std::string const& d) -> std::list<object_id_t>`

**Brief:** Finds all document files in a directory by listing regular files and returning their stems (filenames without extensions) as object IDs.

**Called by:** Collection scanning operations, index rebuilding

**Purpose:** Enables enumeration of all documents within a collection without relying on the search index.

**Parameters:**
- `d` - Directory path (collection path) to search. Must not be empty.

**Preconditions:**
- `d` must not be empty (enforced by `confirm()`)
- The directory must exist

**Returns:** A list of object IDs (filenames without the `.json` extension).

**Postconditions:**
- Each returned ID corresponds to a file `<id>.json` in the directory
- The list excludes subdirectories

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` (via `confirm()`) | `d` is empty |
| `runtime_error("FileSystemDB")` | Directory does not exist |
| `runtime_error("FileSystemDB")` | Path is not a directory |

**Thread Safety:** Safe for reads. Unsafe if files are being created/deleted concurrently.

**Side Effects:**
- Reads from filesystem
- Logs to TRACE at level 21

**Complexity:** O(n) where n is the number of entries in the directory.

**Example:**
```cpp
auto oids = dbfs::find_documents("/data/configs/ComponentConfigs");
// oids = ["507f1f77bcf86cd799439011", "507f1f77bcf86cd799439012", ...]

for (const auto& oid : oids) {
    auto filepath = "/data/configs/ComponentConfigs/" + oid + ".json";
    // Read document from filepath
}
```

### `list_files_in_directory(boost::filesystem::path const& path, std::string const& ext) -> file_paths_t`

**Brief:** Lists all regular files in a directory, optionally filtered by extension, returning their full paths.

**Called by:** Bulk operations, index rebuilding

**Purpose:** Provides a general-purpose file listing function that returns full paths suitable for file I/O operations.

**Parameters:**
- `path` - Directory path to list
- `ext` - File extension filter (currently unused in implementation, marked `[[gnu::unused]]`)

**Preconditions:**
- The directory must exist

**Returns:** A vector of `boost::filesystem::path` objects for each regular file found.

**Postconditions:**
- Each returned path is a full path to a regular file
- Directories and special files are excluded
- The vector is pre-allocated for approximately 100 entries

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error("FileSystemDB")` | Directory does not exist |

**Thread Safety:** Safe for reads. Unsafe if files are being created/deleted concurrently.

**Side Effects:**
- Reads from filesystem
- Logs to TRACE at levels 21-22

**Complexity:** O(n) where n is the number of entries in the directory.

**Note:** The `ext` parameter is currently unused (marked with `[[gnu::unused]]`). The function returns all regular files regardless of extension.

### `check_if_file_exists(std::string const& f) -> bool`

**Brief:** Checks whether a file exists and is accessible at the given path by attempting to open it for reading.

**Called by:** `writeDocument()` for duplicate detection, various validation checks

**Purpose:** Provides a simple existence check that also verifies read accessibility.

**Parameters:**
- `f` - File path to check. Must not be empty.

**Preconditions:**
- `f` must not be empty (enforced by `confirm()`)

**Returns:** `true` if the file exists and can be opened for reading, `false` otherwise.

**Postconditions:** None (read-only operation)

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` (via `confirm()`) | `f` is empty |

**Thread Safety:** Safe. Opening a file for read is a non-modifying operation.

**Side Effects:** Briefly opens and closes the file if it exists.

**Complexity:** O(1)

**Example:**
```cpp
auto filepath = "/data/configs/ComponentConfigs/507f1f77bcf86cd799439011.json";

if (dbfs::check_if_file_exists(filepath)) {
    std::cout << "Document exists" << std::endl;
} else {
    std::cout << "Document not found" << std::endl;
}
```

## Performance Considerations

| Operation | Complexity | Notes |
|-----------|------------|-------|
| `mkdir` | O(p) | p = path components to create |
| `find_subdirs` | O(n) | n = directory entries |
| `find_siblingdirs` | O(n) | n = parent directory entries |
| `find_documents` | O(n) | n = directory entries |
| `list_files_in_directory` | O(n) | n = directory entries, pre-allocates for 100 |
| `check_if_file_exists` | O(1) | Single file open attempt |

For large directories (>1000 files), consider caching results or using the search index instead of `find_documents`.

## Error Handling Strategy

All functions use a consistent error reporting pattern:

1. **Precondition validation**: Use `confirm()` for programmer errors (empty parameters)
2. **Filesystem validation**: Check existence and type before operations
3. **Error propagation**: Wrap Boost.Filesystem errors in `runtime_error("FileSystemDB")`

Error messages include:
- The operation being performed
- The path involved
- The specific error condition

**Example error messages:**
```
FileSystemDB: Directory </data/configs> has wrong permissions; needs owner readable and writable
FileSystemDB: Failed searching for subdirectories, directory does not exist path=</nonexistent>
FileSystemDB: Failed creating a directory, something in the way path=</data/configs/file.json>
```

## Testing Notes

- **Unit tests:** `test/StorageProviders/FileSystemDB/`
- **Key test cases:**
  - Create nested directories
  - Permission validation on existing directories
  - Find subdirs in directory with mixed files and directories
  - Find documents excludes index.json
  - Empty directory handling
  - Non-existent path handling

## Maintenance Notes

### Extension Filter Not Implemented
The `list_files_in_directory` function accepts an `ext` parameter but does not use it (marked `[[gnu::unused]]`). If extension filtering is needed, this parameter should be implemented or removed.

### Typo in Error Message
Line 49 contains the typo "sometging" (should be "something"). This affects the error message for non-directory paths.

### TRACE Logging Levels
All functions log at TRACE level 21, except `list_files_in_directory` which also uses level 22. Enable debug output with:
```cpp
artdaq::database::filesystem::debug::enable();
```

## See Also

- [provider_filedb.h](./provider_filedb.h.md) - Function declarations
- [provider_filedb.cpp](./provider_filedb.cpp.md) - Main provider implementation using these functions
- [provider_connection.cpp](./provider_connection.cpp.md) - Database initialization using `mkdir`
- [External: Boost.Filesystem](https://www.boost.org/doc/libs/release/libs/filesystem/) - Underlying filesystem library

---

**Last Updated:** 2026-01-14
