# filesystem_functions.cpp

## File Overview

This file implements filesystem utility functions specific to the FileSystemDB provider. It provides directory operations, file searching, and file existence checking with robust error handling.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/FileSystemDB/filesystem_functions.cpp`

**Purpose**: Filesystem operations for document storage

## Key Functions

### mkdir
```cpp
std::string dbfs::mkdir(std::string const& d)
```

**Purpose**: Create directory and all parent directories (like `mkdir -p`).

**Algorithm**:
1. Strip `filesystemdb://` URI prefix if present
2. Check if directory exists
3. If exists: Verify read/write permissions
4. If not: Create directory and parents
5. Return directory path

**Permission Checks**:
- Requires owner read and write permissions
- Throws exception if permissions insufficient

### find_subdirs
```cpp
std::list<std::string> dbfs::find_subdirs(std::string const& d)
```

**Purpose**: Find all subdirectories in a directory.

**Returns**: List of subdirectory names (not full paths)

**Usage**: Discover collections (each collection is a subdirectory).

**Error Handling**:
- Throws if directory doesn't exist
- Throws if path is not a directory

### find_siblingdirs
```cpp
std::list<std::string> dbfs::find_siblingdirs(std::string const& d)
```

**Purpose**: Find sibling directories (at same level as given directory).

**Usage**: Discover databases (sibling directories of current database).

### find_documents
```cpp
std::list<object_id_t> dbfs::find_documents(std::string const& d)
```

**Purpose**: Find all JSON document files in a directory.

**Returns**: List of object IDs (filenames without `.json` extension).

**Filter**: Only includes `.json` files, excludes `index.json`.

### check_if_file_exists
```cpp
bool dbfs::check_if_file_exists(std::string const& f)
```

**Purpose**: Check if a file exists.

**Returns**: true if file exists, false otherwise.

### list_files_in_directory
```cpp
file_paths_t dbfs::list_files_in_directory(boost::filesystem::path const& path, std::string const& ext)
```

**Purpose**: List all files with specific extension.

**Parameters**:
- `path` - Directory to search
- `ext` - File extension filter (e.g., ".json")

**Returns**: Vector of file paths.

---

## URI Handling

All functions automatically strip `filesystemdb://` URI prefix:

```cpp
if (dir.find(dbfsl::FILEURI) == 0) {
    dir = dir.substr(strlen(dbfsl::FILEURI));
}
```

This allows passing either URIs or plain paths.

---

## Error Handling

### Permission Errors
```cpp
if ((boost::filesystem::status(path).permissions() & mask) != mask) {
    throw runtime_error("FileSystemDB")
        << "Directory has wrong permissions; needs owner readable and writable";
}
```

### Filesystem Errors
```cpp
boost::system::error_code ec;
if (!boost::filesystem::create_directories(path, ec)) {
    if (ec != boost::system::errc::success) {
        throw runtime_error("FileSystemDB")
            << "Failed creating directory: " << ec.message();
    }
}
```

---

## TRACE Debugging

All functions use TRACE logging (level 21):

```cpp
TLOG(21) << "StorageProvider::FileSystemDB mkdir dir=<" << dir << ">";
```

Enable with: `artdaq::database::filesystem::debug::enable();`

---

## Related Files

- **provider_filedb.h** - Function declarations
- **provider_filedb.cpp** - Uses these functions
- **provider_filedb_readwrite.cpp** - Uses for file I/O

---

**Documentation generated for artdaq-database FileSystemDB provider**
