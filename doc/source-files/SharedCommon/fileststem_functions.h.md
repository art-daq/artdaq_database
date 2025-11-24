# fileststem_functions.h

## File Overview

This header file declares a set of utility functions for filesystem operations used throughout the artdaq-database project. It provides high-level abstractions for common file and directory operations including recursive directory listing, path conversions, compression/archiving, and file I/O.

**Location**: `/home/user/artdaq-database/artdaq-database/SharedCommon/fileststem_functions.h`

**Note**: The filename contains a typo - "fileststem" should be "filesystem".

## Dependencies

### Standard Library
- `<list>` - List container
- `<string>` - String class
- `<vector>` - Vector container

### Project Headers
- `"artdaq-database/SharedCommon/shared_datatypes.h"` - Common type definitions

## Namespace

All functions are declared in the `artdaq::database` namespace.

## Functions

### Directory Operations

#### list_files
```cpp
std::vector<std::string> list_files(std::string const& path);
```

**Purpose**: Recursively list all files in a directory and its subdirectories.

**Parameters**:
- `path` - The directory path to search

**Returns**: A vector of strings containing full paths to all files found

**Usage Example**:
```cpp
auto files = artdaq::database::list_files("/path/to/config/dir");
for (const auto& file : files) {
    std::cout << file << std::endl;
}
```

#### mkdir
```cpp
bool mkdir(std::string const& path);
```

**Purpose**: Create a directory path, including all parent directories if they don't exist (similar to `mkdir -p`).

**Parameters**:
- `path` - The directory path to create

**Returns**: `true` if successful, `false` otherwise

**Usage Example**:
```cpp
if (artdaq::database::mkdir("/tmp/my/nested/path")) {
    // Directory created successfully
}
```

#### mkdirfile
```cpp
bool mkdirfile(std::string const& file);
```

**Purpose**: Create the parent directory for a given file path. Useful before writing a file to ensure its directory exists.

**Parameters**:
- `file` - Full path to a file

**Returns**: `true` if successful, `false` otherwise

**Usage Example**:
```cpp
// Create /path/to/output/ before writing /path/to/output/file.json
mkdirfile("/path/to/output/file.json");
```

### Path Conversion Functions

#### collection_name_from_relative_path
```cpp
std::string collection_name_from_relative_path(std::string const& path);
```

**Purpose**: Convert a relative file path to a collection name by replacing directory separators with dots.

**Parameters**:
- `path` - Relative path to convert

**Returns**: Collection name string

**Usage Example**:
```cpp
// Input: "SystemLayout/component1.json"
// Output: "SystemLayout.component1.json"
auto collection = collection_name_from_relative_path("SystemLayout/component1.json");
```

#### relative_path_from_collection_name
```cpp
std::string relative_path_from_collection_name(std::string const& name);
```

**Purpose**: Convert a collection name to a relative file path by replacing dots with directory separators (inverse of above).

**Parameters**:
- `name` - Collection name to convert

**Returns**: Relative path string

**Usage Example**:
```cpp
// Input: "SystemLayout.component1.json"
// Output: "SystemLayout/component1.json"
auto path = relative_path_from_collection_name("SystemLayout.component1.json");
```

### File I/O Functions

#### write_buffer_to_file
```cpp
bool write_buffer_to_file(std::string const& buffer,
                         std::string const& file_out_name);
```

**Purpose**: Write a string buffer to a file, creating parent directories if needed.

**Parameters**:
- `buffer` - String content to write
- `file_out_name` - Output file path

**Returns**: `true` if successful, `false` on error

**Behavior**:
- Automatically creates parent directories
- Overwrites existing file
- Throws exception on failure

**Usage Example**:
```cpp
std::string json_data = "{\"config\": \"value\"}";
write_buffer_to_file(json_data, "/path/to/output.json");
```

#### read_buffer_from_file
```cpp
bool read_buffer_from_file(std::string& buffer,
                          std::string const& file_in_name);
```

**Purpose**: Read entire file contents into a string buffer.

**Parameters**:
- `buffer` - Output string to store file contents (must be empty on input)
- `file_in_name` - Input file path

**Returns**: `true` if successful, `false` on error

**Behavior**:
- Requires buffer to be empty initially (assertion check)
- Reads entire file into memory
- Throws exception on failure

**Usage Example**:
```cpp
std::string content;
if (read_buffer_from_file(content, "/path/to/input.json")) {
    // Process content
}
```

### Temporary Directory Functions

#### make_temp_dir
```cpp
std::string make_temp_dir();
```

**Purpose**: Create a temporary directory with a random name in `/tmp/`.

**Returns**: Path to the created temporary directory

**Behavior**:
- Uses prefix from `apiliteral::tmpdirprefix` (`/tmp/adb`)
- Appends random 7-digit number
- Creates directory using system call

**Usage Example**:
```cpp
auto tmp = make_temp_dir();  // Returns "/tmp/adb1234567"
// Use temp directory
delete_temp_dir(tmp);  // Clean up when done
```

#### delete_temp_dir
```cpp
void delete_temp_dir(std::string const& tmp_dir_name);
```

**Purpose**: Recursively delete a temporary directory created by `make_temp_dir`.

**Parameters**:
- `tmp_dir_name` - Path to temporary directory

**Safety**: Only deletes directories that start with the expected prefix (`/tmp/adb`) to prevent accidental deletion.

**Usage Example**:
```cpp
auto tmp = make_temp_dir();
// ... use temp directory ...
delete_temp_dir(tmp);
```

### Archive/Compression Functions

#### dir_to_tarbzip2base64
```cpp
std::string const& dir_to_tarbzip2base64(std::string const& tmp_dir_name,
                                          std::string const& bzip2base64);
```

**Purpose**: Compress a directory to a tar.bzip2 archive and encode it as base64.

**Parameters**:
- `tmp_dir_name` - Source directory to compress
- `bzip2base64` - Output file path for the base64-encoded archive

**Returns**: Reference to the output file path

**Process**:
1. Creates tar archive of directory
2. Compresses with bzip2
3. Encodes as base64
4. Writes to output file

**Usage Example**:
```cpp
auto archive = dir_to_tarbzip2base64("/tmp/adb123456",
                                     "/output/backup.tar-bzip2-base64");
```

#### tarbzip2base64_to_dir
```cpp
std::string const& tarbzip2base64_to_dir(std::string const& bzip2base64,
                                          std::string const& tmp_dir_name);
```

**Purpose**: Decode a base64-encoded tar.bzip2 archive and extract to a directory (inverse of above).

**Parameters**:
- `bzip2base64` - Input base64-encoded archive file
- `tmp_dir_name` - Target directory for extraction

**Returns**: Reference to the extraction directory path

**Process**:
1. Decodes base64
2. Decompresses bzip2
3. Extracts tar archive
4. Places contents in target directory

**Usage Example**:
```cpp
auto extracted = tarbzip2base64_to_dir("/input/backup.tar-bzip2-base64",
                                       "/tmp/adb789012");
```

### Utility Functions

#### extract_collectionname_from_filename
```cpp
bool extract_collectionname_from_filename(std::string const& file_name,
                                         std::string& collection_name);
```

**Purpose**: Extract collection name from an export file name.

**Parameters**:
- `file_name` - Input filename
- `collection_name` - Output parameter for extracted collection name

**Returns**: `true` if filename has correct extension and name was extracted, `false` otherwise

**Behavior**:
- Checks if file has `.tar-bzip2-base64` extension
- Extracts stem (filename without extension) as collection name

**Usage Example**:
```cpp
std::string collection;
if (extract_collectionname_from_filename("MyConfig.tar-bzip2-base64", collection)) {
    // collection now contains "MyConfig"
}
```

## Usage Context

These functions are used throughout the artdaq-database project for:

1. **Configuration File Management**: Reading and writing configuration files in filesystem-based storage
2. **Database Export/Import**: Compressing and archiving database contents
3. **Temporary Storage**: Managing temporary directories during operations
4. **Collection Storage**: Converting between collection names and file paths
5. **Batch Operations**: Listing all files for bulk processing

## Error Handling

Most functions use one or more of these error handling strategies:
- Return boolean success/failure status
- Throw exceptions (runtime_error, invalid_argument) with descriptive messages
- Use `confirm()` assertions for precondition checking
- Log errors using TRACE macros

## Implementation Notes

- Functions use `system()` calls for operations like mkdir, tar, bzip2
- The archive format (tar.bzip2.base64) is specifically designed for database export/import
- Path conversions support dot-notation collection names used in MongoDB-style databases
- Temporary directory names use random numbers to avoid collisions

## Related Files

- **fileststem_functions.cpp** - Implementation of these functions
- **configuraion_api_literals.h** - Defines constants like `tmpdirprefix`, `dbexport_extension`
- **shared_datatypes.h** - Type definitions used by these functions
