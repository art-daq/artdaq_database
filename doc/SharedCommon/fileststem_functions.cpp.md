# fileststem_functions.cpp

**Path:** `artdaq-database/SharedCommon/fileststem_functions.cpp`

**Implements:** [fileststem_functions.h](./fileststem_functions.h.md)

**Purpose:** Implements filesystem utilities using Boost.Filesystem for cross-platform operations and system commands for archiving/compression. Used by the FileSystemDB storage provider and database export/import operations.

**Note:** Filename has historical typo ("fileststem" instead of "filesystem") - preserved for backward compatibility.

## Implementation Overview

This file provides filesystem operations using a combination of Boost.Filesystem for cross-platform directory traversal and POSIX/shell commands for operations like directory creation and archive handling. The choice to use `system()` calls for certain operations (mkdir, tar, etc.) provides reliable behavior that matches shell expectations, though at the cost of reduced portability and security considerations.

## Key Algorithms

### Recursive Directory Listing (`list_files`)

**Steps:**
1. Pre-allocate vector capacity (1024 entries) for efficiency
2. Iterate over directory entries using Boost's `directory_iterator`
3. For each entry:
   - If it's a regular file, add its full path to the result
   - If it's a directory, recursively call `list_files` and append results
4. Return the accumulated file list

### Collection Name / Path Conversion

**`collection_name_from_relative_path`:**
1. Copy the input path string
2. Extract the filename using `basename()`
3. Extract the parent directory name using `dirname()` followed by `basename()`
4. Concatenate with dots as separator
5. Remove trailing dot

**`relative_path_from_collection_name`:**
1. Find the last dot in the string (file extension boundary)
2. Replace all dots before that position with slashes
3. Leave the file extension intact

### Archive Pipeline

**Export (dir_to_tarbzip2base64):**
```bash
tar cjf - dir/* -C dir 2>/dev/null | base64 --wrap=0 > output
```

**Import (tarbzip2base64_to_dir):**
```bash
cat input | base64 -d | tar xjf - -C dir
```

### Temporary Directory Generation

**Steps:**
1. Seed the random number generator with current time
2. Generate a 7-digit random number (1000000-9999999)
3. Create path as `/tmp/adb{random}`
4. Execute `mkdir -p` via system call

## Function Implementations

### `list_files(path) -> std::vector<std::string>`

**Brief:** Recursively collects all regular files in a directory tree.

**Implementation:**
```cpp
std::vector<std::string> db::list_files(std::string const& path) {
  confirm(!path.empty());
  auto files = std::vector<std::string>{};
  files.reserve(1024);

  for (auto& entry : boost::make_iterator_range(
           boost::filesystem::directory_iterator(path), {})) {
    if (boost::filesystem::is_regular_file(entry)) {
      files.push_back(entry.path().string());
    } else if (boost::filesystem::is_directory(entry.path())) {
      auto suddir_list = list_files(entry.path().string());
      std::copy(suddir_list.begin(), suddir_list.end(), std::back_inserter(files));
    }
  }
  return files;
}
```

**Performance Notes:**
- Pre-allocates 1024 capacity to reduce reallocations
- Returns absolute paths

**Thread Safety:** Safe (read-only filesystem operation)

---

### `collection_name_from_relative_path(path) -> std::string`

**Brief:** Converts a relative file path to a collection name by combining parent directory and filename with a dot.

**Implementation:**
- Uses POSIX `basename()` and `dirname()` functions
- Creates a string copy to avoid modifying the input
- Builds result by prepending directory names with dots

**Thread Safety:** Safe

---

### `relative_path_from_collection_name(name) -> std::string`

**Brief:** Converts a collection name to a relative path by replacing dots with slashes (except in the file extension).

**Implementation:**
- Finds the last dot position (assumed to be the file extension boundary)
- Uses `std::replace()` to convert dots to slashes up to that position

**Thread Safety:** Safe

---

### `mkdir(path) -> bool`

**Brief:** Creates a directory and all parent directories using shell command.

**Implementation:**
```cpp
bool db::mkdir(std::string const& path) {
  confirm(!path.empty());
  auto cmd = std::string{"mkdir -p "};
  cmd.append(path);
  return 0 == system(cmd.c_str());
}
```

**Thread Safety:** Not thread-safe (uses `system()`)

---

### `mkdirfile(file) -> bool`

**Brief:** Creates the parent directory for a file path.

**Implementation:**
```cpp
bool db::mkdirfile(std::string const& file) {
  confirm(!file.empty());
  auto tmp = std::string{file};
  return mkdir(dirname(const_cast<char*>(tmp.c_str())));
}
```

**Note:** Creates a copy of the string because `dirname()` may modify its argument.

**Thread Safety:** Not thread-safe (uses `system()`)

---

### `write_buffer_to_file(buffer, filepath) -> bool`

**Brief:** Writes a string buffer to a file, creating parent directories as needed.

**Implementation:**
1. Validates file path is not empty
2. Creates parent directory with `mkdirfile()`
3. Opens output file stream
4. Writes buffer using `std::copy` with `ostream_iterator`
5. Closes stream and returns success
6. Catches exceptions and rethrows with diagnostic information

**Thread Safety:** Not thread-safe (file write operation)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/fileststem_functions.h"
#include <iostream>

void saveJson(const std::string& json, const std::string& path) {
    try {
        artdaq::database::write_buffer_to_file(json, path);
        std::cout << "Saved to: " << path << "\n";
    } catch (const artdaq::database::runtime_error& e) {
        std::cerr << "Save failed: " << e.what() << "\n";
    }
}
```

---

### `read_buffer_from_file(buffer, filepath) -> bool`

**Brief:** Reads entire file contents into a string buffer.

**Implementation:**
1. Validates file path is not empty and buffer is empty
2. Opens input file stream
3. Checks stream status; throws `invalid_argument` if file cannot be opened
4. Reads entire file using `rdbuf()` into a stringstream
5. Assigns result to buffer
6. Catches exceptions and rethrows with diagnostic information

**Thread Safety:** Safe for reading (may race with concurrent writers)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/fileststem_functions.h"
#include <iostream>

void loadJson(const std::string& path) {
    std::string content;
    try {
        artdaq::database::read_buffer_from_file(content, path);
        processJson(content);
    } catch (const artdaq::database::invalid_argument& e) {
        std::cerr << "File not found: " << e.what() << "\n";
    } catch (const artdaq::database::runtime_error& e) {
        std::cerr << "Read error: " << e.what() << "\n";
    }
}
```

---

### `make_temp_dir() -> std::string`

**Brief:** Creates a temporary directory with a random name.

**Implementation:**
```cpp
std::string db::make_temp_dir() {
  auto tmp_dir_name = std::string{apiliteral::tmpdirprefix};
  srand(time(nullptr));
  tmp_dir_name.append(std::to_string(rand() % 9000000 + 1000000));

  auto system_cmd = std::string{"mkdir -p "};
  system_cmd += tmp_dir_name;

  if (0 != system(system_cmd.c_str())) {
    throw runtime_error("make_temp_dir") << "...";
  }
  return tmp_dir_name;
}
```

**Notes:**
- Random number range: 1000000-9999999 (7 digits)
- Not thread-safe due to `rand()` usage

**Thread Safety:** Not thread-safe (uses `rand()` and `system()`)

---

### `delete_temp_dir(path) -> void`

**Brief:** Recursively deletes a temporary directory with safety checks.

**Implementation:**
```cpp
void db::delete_temp_dir(std::string const& tmp_dir_name) {
  confirm(!tmp_dir_name.empty());

  auto prefix = std::string{apiliteral::tmpdirprefix};
  auto first(std::begin(prefix)), last(std::end(prefix));
  confirm(std::equal(first, last, tmp_dir_name.begin()));

  auto system_cmd = std::string{"rm -rf "}.append(tmp_dir_name);

  if (0 != system(system_cmd.c_str())) {
    throw runtime_error("delete_temp_dir") << "...";
  }
}
```

**Safety Check:** Uses `confirm()` to verify the path starts with the expected prefix before executing `rm -rf`.

**Thread Safety:** Not thread-safe (uses `system()`)

---

### `dir_to_tarbzip2base64(dir, output) -> std::string const&`

**Brief:** Compresses a directory to a base64-encoded tar.bzip2 archive.

**Command Generated:**
```bash
tar cjf - tmp_dir_name/* -C tmp_dir_name 2>/dev/null | base64 --wrap=0 > output
```

**Implementation Notes:**
- Creates parent directory for output file
- Uses `-C` option to change directory before archiving
- Suppresses tar warnings with `2>/dev/null`
- Uses `--wrap=0` to prevent line wrapping in base64 output

**Thread Safety:** Not thread-safe (uses `system()`)

---

### `tarbzip2base64_to_dir(input, dir) -> std::string const&`

**Brief:** Extracts a base64-encoded tar.bzip2 archive to a directory.

**Command Generated:**
```bash
cat input | base64 -d | tar xjf - -C output_dir
```

**Implementation Notes:**
- Creates destination directory before extraction
- Uses pipeline to decode and extract in one operation

**Thread Safety:** Not thread-safe (uses `system()`)

---

### `extract_collectionname_from_filename(filename, collection) -> bool`

**Brief:** Extracts collection name from an export filename.

**Implementation:**
```cpp
bool db::extract_collectionname_from_filename(
    std::string const& file_name, std::string& collection_name) {
  confirm(!file_name.empty());
  boost::filesystem::path p(file_name);

  if (p.extension().string() != apiliteral::dbexport_extension) {
    return false;
  }

  collection_name = p.stem().string();
  return true;
}
```

**Notes:**
- Uses Boost.Filesystem `path` class for parsing
- Checks for expected export file extension
- Returns `false` if extension doesn't match

**Thread Safety:** Safe

---

## Performance Considerations

- `list_files()` pre-allocates vector capacity for efficiency
- `system()` calls have subprocess overhead
- Archive operations involve I/O and compression - can be slow for large directories

## Error Handling Strategy

- Functions use `confirm()` assertions for precondition validation
- File I/O functions wrap operations in try-catch blocks
- Exceptions include diagnostic information from `current_exception_diagnostic_information()`
- `system()` calls check return value and throw `runtime_error` on failure

## Dependencies

| Include | Purpose |
|---------|---------|
| `<libgen.h>` | POSIX `basename()`, `dirname()` |
| `<boost/filesystem.hpp>` | Cross-platform filesystem operations |
| `<boost/range/iterator_range.hpp>` | Range-based iteration |
| `common.h` | TRACE logging macros |
| `configuraion_api_literals.h` | `tmpdirprefix`, `dbexport_extension` |
| `helper_functions.h` | `confirm()` assertion |
| `shared_exceptions.h` | Exception types |

## TRACE Configuration

The file defines `TRACE_NAME` as `"fileststem_functions.cpp"` for use with the TRACE logging system.

## Thread Safety

- `make_temp_dir()` uses `rand()` seeded with `time(nullptr)` - not thread-safe. Threads starting within the same second could generate the same temp directory name.
- `system()` calls are not thread-safe
- File I/O operations may have race conditions with concurrent access

## Security Considerations

- `system()` calls are vulnerable to command injection if paths contain special characters
- `delete_temp_dir()` includes a safety check to only delete paths with the expected prefix
- Other functions do not validate paths for malicious content

## Testing Notes

- **Unit tests:** FileSystemDB provider tests exercise these functions
- **Key test cases:** Directory listing, file I/O, path conversions, archive operations

## See Also

- [fileststem_functions.h](./fileststem_functions.h.md) - Function declarations
- [configuraion_api_literals.h](./configuraion_api_literals.h.md) - Constants used (tmpdirprefix, dbexport_extension)
- [helper_functions.h](./helper_functions.h.md) - `confirm()` assertion function
