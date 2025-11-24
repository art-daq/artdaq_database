# fileststem_functions.cpp

## File Overview

This implementation file provides the actual implementation of filesystem utility functions declared in `fileststem_functions.h`. It uses Boost.Filesystem for cross-platform file operations and system commands for advanced operations like archiving and compression.

**Location**: `/home/user/artdaq-database/artdaq-database/SharedCommon/fileststem_functions.cpp`

**Note**: The filename contains a typo - "fileststem" should be "filesystem".

## Dependencies

### System Headers
- `<libgen.h>` - Unix path manipulation (basename, dirname)

### Boost Libraries
- `<boost/filesystem.hpp>` - Portable filesystem operations
- `<boost/range/iterator_range.hpp>` - Range utilities for iteration

### Project Headers
- `"artdaq-database/SharedCommon/common.h"` - Common includes and utilities
- `"artdaq-database/SharedCommon/configuraion_api_literals.h"` - String literals and constants
- `"artdaq-database/SharedCommon/fileststem_functions.h"` - Function declarations
- `"artdaq-database/SharedCommon/helper_functions.h"` - Helper utilities
- `"artdaq-database/SharedCommon/shared_exceptions.h"` - Exception types

## TRACE Configuration

```cpp
#define TRACE_NAME "fileststem_functions.cpp"
```

This sets the TRACE logging identifier for this compilation unit.

## Namespace Aliases

```cpp
using namespace artdaq::database;
namespace db = artdaq::database;
namespace apiliteral = db::configapi::literal;
```

## Function Implementations

### list_files

```cpp
std::vector<std::string> db::list_files(std::string const& path)
```

**Implementation Details**:
- Pre-allocates vector with 1024 capacity for efficiency
- Uses Boost.Filesystem directory_iterator for cross-platform compatibility
- Recursively traverses subdirectories
- Returns full absolute paths for all files

**Algorithm**:
1. Validate path is not empty (using `confirm()`)
2. Create empty vector with 1024 reserved capacity
3. Iterate through directory entries
4. For regular files: add path to vector
5. For directories: recursively call list_files and append results
6. Return complete file list

**Usage Example**:
```cpp
auto files = artdaq::database::list_files("/data/configs");
// Returns: ["/data/configs/file1.json", "/data/configs/subdir/file2.json", ...]
```

**Error Handling**: Will throw Boost.Filesystem exceptions if path doesn't exist or is inaccessible.

---

### collection_name_from_relative_path

```cpp
std::string db::collection_name_from_relative_path(std::string const& file_path_str)
```

**Implementation Details**:
- Uses Unix libgen functions (basename, dirname)
- Constructs collection name from parent directory and filename
- Joins components with dots

**Algorithm**:
1. Copy input path (basename/dirname modify their input)
2. Get basename (filename)
3. Get basename of parent directory
4. Concatenate with dots: `parentdir.filename`
5. Remove trailing dot

**Example Transformation**:
```
Input:  "SystemLayout/component1.json"
Steps:  basename = "component1.json"
        parent basename = "SystemLayout"
Output: "SystemLayout.component1.json"
```

**Note**: The commented-out code suggests this originally used 3 levels of hierarchy instead of 2.

---

### relative_path_from_collection_name

```cpp
std::string db::relative_path_from_collection_name(std::string const& collection_name)
```

**Implementation Details**:
- Converts dots to forward slashes
- Preserves the last dot (before file extension)
- Handles collections without dots

**Algorithm**:
1. Validate collection name is not empty
2. Copy collection name
3. Find position of last dot
4. If no dot found: return unchanged
5. Replace all dots before last dot with '/'
6. Return converted path

**Example Transformation**:
```
Input:  "SystemLayout.component1.json"
Find last dot at position (between "component1" and "json")
Replace earlier dots: "SystemLayout/component1.json"
Output: "SystemLayout/component1.json"
```

---

### mkdir

```cpp
bool db::mkdir(std::string const& path)
```

**Implementation Details**:
- Uses system command `mkdir -p`
- The `-p` flag creates parent directories as needed
- No-op if directory already exists

**Implementation**:
```cpp
auto cmd = std::string{"mkdir -p "};
cmd.append(path);
return 0 == system(cmd.c_str());
```

**Returns**: `true` if system command returns 0 (success), `false` otherwise

**Security Note**: This uses system() which can be a security risk if path contains special characters. In production, should use Boost.Filesystem instead.

---

### mkdirfile

```cpp
bool db::mkdirfile(std::string const& file)
```

**Implementation Details**:
- Extracts directory path from file path
- Creates the directory using `mkdir()`

**Implementation**:
```cpp
auto tmp = std::string{file};
return mkdir(dirname(const_cast<char*>(tmp.c_str())));
```

**Example**:
```cpp
mkdirfile("/path/to/output/file.json");
// Creates: /path/to/output/
```

**Note**: Uses dirname() which modifies its input (hence the copy and const_cast).

---

### write_buffer_to_file

```cpp
bool db::write_buffer_to_file(std::string const& buffer,
                              std::string const& file_out_name)
```

**Implementation Details**:
- Creates parent directory if needed
- Writes buffer contents to file using ofstream
- Uses exception handling with try-catch

**Algorithm**:
1. Validate filename is not empty
2. Create parent directory using `mkdirfile()`
3. Throw exception if directory creation fails
4. Open output file stream
5. Copy buffer to file using ostream_iterator
6. Close file
7. Return true on success

**Error Handling**:
- Throws `runtime_error` with detailed message on failure
- Includes filename in error message
- Includes diagnostic information from `current_exception_diagnostic_information()`

**Usage Example**:
```cpp
std::string json = "{\"config\": \"data\"}";
write_buffer_to_file(json, "/data/configs/newfile.json");
```

---

### read_buffer_from_file

```cpp
bool db::read_buffer_from_file(std::string& buffer,
                               std::string const& file_in_name)
```

**Implementation Details**:
- Requires buffer to be empty (precondition check)
- Reads entire file into memory
- Uses stringstream for efficient reading

**Algorithm**:
1. Validate filename is not empty
2. Confirm buffer is empty (precondition)
3. Open input file stream
4. Check if file opened successfully
5. Read entire file into stringstream using rdbuf()
6. Convert stringstream to string
7. Store in output buffer parameter
8. Return true on success

**Error Handling**:
- Throws `invalid_argument` if file doesn't exist or can't be opened
- Throws `runtime_error` with diagnostic info on other failures
- Includes filename in error messages

**Usage Example**:
```cpp
std::string content;
read_buffer_from_file(content, "/data/configs/existing.json");
// content now contains file contents
```

---

### make_temp_dir

```cpp
std::string db::make_temp_dir()
```

**Implementation Details**:
- Uses prefix `/tmp/adb` (from apiliteral::tmpdirprefix)
- Appends random 7-digit number (1000000-9999999)
- Creates directory using system command

**Algorithm**:
1. Start with prefix `/tmp/adb`
2. Seed random number generator with current time
3. Generate random number between 1000000 and 9999999
4. Append to prefix
5. Execute `mkdir -p` system command
6. Throw exception if creation fails
7. Return directory path

**Example Output**: `/tmp/adb3741829`

**Thread Safety**: Uses `rand()` which is not thread-safe. Seeding with `time(nullptr)` means threads started in the same second could generate same temp dir.

---

### delete_temp_dir

```cpp
void db::delete_temp_dir(std::string const& tmp_dir_name)
```

**Implementation Details**:
- Safety check: only deletes paths starting with `/tmp/adb`
- Uses `rm -rf` system command

**Algorithm**:
1. Validate tmp_dir_name is not empty
2. Confirm path starts with expected prefix using `std::equal()`
3. Build system command: `rm -rf <path>`
4. Execute system command
5. Throw exception if deletion fails

**Safety Features**:
```cpp
auto prefix = std::string{apiliteral::tmpdirprefix};  // "/tmp/adb"
confirm(std::equal(first, last, tmp_dir_name.begin()));
```

This prevents accidental deletion of non-temporary directories.

**Usage Example**:
```cpp
auto tmp = make_temp_dir();        // "/tmp/adb1234567"
// ... use directory ...
delete_temp_dir(tmp);               // Safely deletes only /tmp/adb* paths
```

---

### dir_to_tarbzip2base64

```cpp
std::string const& db::dir_to_tarbzip2base64(
    std::string const& tmp_dir_name,
    std::string const& bzip2base64)
```

**Implementation Details**:
- Creates tar.bzip2 archive of directory
- Encodes result as base64
- Saves to output file

**Algorithm**:
1. Validate inputs are not empty
2. Create parent directory for output file
3. Build complex shell pipeline:
   ```bash
   tar cjf - <dir>/* -C <dir> 2>/dev/null | base64 --wrap=0 > <output>
   ```
4. Execute system command
5. Throw exception on failure
6. Return output file path

**Command Breakdown**:
- `tar cjf -` - Create bzip2-compressed tar, output to stdout
- `<dir>/*` - Include all files in directory
- `-C <dir>` - Change to directory first
- `2>/dev/null` - Suppress error messages
- `| base64 --wrap=0` - Encode as base64 without line wrapping
- `> <output>` - Write to output file

**Usage Example**:
```cpp
dir_to_tarbzip2base64("/tmp/adb123456",
                      "/exports/backup.tar-bzip2-base64");
```

---

### tarbzip2base64_to_dir

```cpp
std::string const& db::tarbzip2base64_to_dir(
    std::string const& bzip2base64,
    std::string const& tmp_dir_name)
```

**Implementation Details**:
- Decodes base64 file
- Decompresses bzip2
- Extracts tar archive to directory

**Algorithm**:
1. Validate inputs are not empty
2. Create target directory
3. Build shell pipeline:
   ```bash
   cat <input> | base64 -d | tar xjf - -C <output>
   ```
4. Execute system command
5. Throw exception on failure
6. Return extraction directory path

**Command Breakdown**:
- `cat <input>` - Read input file
- `| base64 -d` - Decode base64
- `| tar xjf -` - Extract bzip2-compressed tar from stdin
- `-C <output>` - Extract to specified directory

**Usage Example**:
```cpp
tarbzip2base64_to_dir("/exports/backup.tar-bzip2-base64",
                      "/tmp/adb789012");
```

---

### extract_collectionname_from_filename

```cpp
bool db::extract_collectionname_from_filename(
    std::string const& file_name,
    std::string& collection_name)
```

**Implementation Details**:
- Uses Boost.Filesystem path parsing
- Checks for specific file extension
- Extracts filename stem

**Algorithm**:
1. Validate filename is not empty
2. Parse path using Boost.Filesystem
3. Check if extension is `.tar-bzip2-base64`
4. If not, return false
5. Extract stem (filename without extension)
6. Store in output parameter
7. Return true

**Example**:
```cpp
std::string collection;
extract_collectionname_from_filename(
    "MyCollection.tar-bzip2-base64",
    collection);
// collection = "MyCollection"
// returns true

extract_collectionname_from_filename(
    "regular_file.txt",
    collection);
// returns false (wrong extension)
```

**Usage Context**: Used during database import to determine which collection an archive belongs to.

## Error Handling Strategy

The file uses multiple error handling approaches:

1. **Assertions** (`confirm()`):
   - Validates preconditions
   - Throws in release builds, asserts in debug builds

2. **Return Values**:
   - Boolean returns for success/failure
   - Reference returns for function chaining

3. **Exceptions**:
   - `runtime_error` for system operation failures
   - `invalid_argument` for invalid inputs
   - Includes context (function name, parameters) in exceptions

4. **Diagnostic Information**:
   - Uses `debug::current_exception_diagnostic_information()` for detailed error context
   - Includes stack traces in error messages

## Performance Considerations

1. **Memory Allocation**:
   - `list_files()` pre-allocates vector with 1024 capacity to reduce reallocations

2. **System Calls**:
   - Uses system() for operations - relatively expensive but portable
   - Could be optimized using Boost.Filesystem or native APIs

3. **String Operations**:
   - Frequent string copying in path manipulation functions
   - Trade-off between safety (copies) and performance

## Security Considerations

1. **System Command Injection**: Functions using `system()` are potentially vulnerable to command injection if paths contain special characters
   - Should sanitize inputs or use library functions instead

2. **Path Traversal**: Limited validation of path strings could allow directory traversal attacks
   - `delete_temp_dir()` has safety check but others don't

3. **Temporary Files**: Predictable temp directory names (based on time-seeded rand()) could be exploited

## Related Files

- **fileststem_functions.h** - Function declarations
- **helper_functions.h/cpp** - Provides `confirm()` and other utilities
- **shared_exceptions.h** - Exception type definitions
- **configuraion_api_literals.h** - Constants like tmpdirprefix, dbexport_extension
