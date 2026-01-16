# GetPackageBuildInfo.cc.in

**Path:** `artdaq-database/BuildInfo/GetPackageBuildInfo.cc.in`

**Implements:** [GetPackageBuildInfo.hh](./GetPackageBuildInfo.hh.md)

**Purpose:** This CMake template file provides the implementation of the `GetPackageBuildInfo::getPackageBuildInfo()` static method. During the CMake configuration phase, placeholder variables (`@version_with_git@` and `@utcDateTime@`) are substituted with actual build-time values, generating the final `GetPackageBuildInfo.cc` source file that embeds version and timestamp information directly into the compiled binary.

## Implementation Overview

This file uses CMake's template processing mechanism to embed build-time information into the compiled binary. The `.in` extension signals to CMake that this is a template file. When CMake's `configure_file()` command processes this template, it replaces `@variable@` placeholders with their corresponding CMake variable values, producing the actual `.cc` source file in the build directory.

### Build-Time Information Flow

```
GetPackageBuildInfo.cc.in  -->  CMake configure_file()  -->  GetPackageBuildInfo.cc
   (with @placeholders@)           (substitution)             (with actual values)
                                        ^
                                        |
                               +--------+--------+
                               |                 |
                          git describe      date -u
                          --tags             (UTC)
```

## Key Algorithms

### Template Variable Substitution

**Steps:**
1. CMake runs `git describe --tags` to extract version from repository
2. CMake runs `date -u "+%d-%b-%Y %H:%M:%S %Z"` to capture the current UTC timestamp
3. CMake's `configure_file()` reads `GetPackageBuildInfo.cc.in`
4. All `@variable@` patterns are replaced with corresponding CMake variable values
5. The processed file is written to the build directory as `GetPackageBuildInfo.cc`
6. The generated file is compiled into the library

**Why this approach:** Embedding version information at compile time ensures:
- Version info is always available without external files
- No runtime file I/O is required
- The version cannot be accidentally modified after compilation
- Git commit information is captured exactly at build time

### Placeholder Variables

| Placeholder | Source Command | CMake Variable | Example Value |
|-------------|----------------|----------------|---------------|
| `@version_with_git@` | `git describe --tags` | `version_with_git` | `v1_05_00-1-g5e0d18a` |
| `@utcDateTime@` | `date -u "+%d-%b-%Y %H:%M:%S %Z"` | `utcDateTime` | `13-Nov-2025 15:30:45 UTC` |

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/BuildInfo/GetPackageBuildInfo.hh` | Header file declaring the `GetPackageBuildInfo` struct and `getPackageBuildInfo()` method |
| `<iostream>` | Standard I/O stream support (included for potential debugging, not directly used in current implementation) |
| `<string>` | Standard C++ string support for version and timestamp strings |

## Internal Functions

### `GetPackageBuildInfo::getPackageBuildInfo() -> artdaq::PackageBuildInfo`

**Brief:** Creates and returns a populated `PackageBuildInfo` object with the package name, version string, and build timestamp. This method implements the interface declared in the header file.

**Called by:** Any code that includes the header and calls `artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo()`

**Purpose:** This is the implementation of the static method declared in the header. It:
1. Creates a new `artdaq::PackageBuildInfo` object
2. Populates it with the package name ("artdaq-database")
3. Sets the version using the CMake-substituted value
4. Sets the timestamp using the CMake-substituted value
5. Returns the populated object by value

**Implementation (before substitution):**
```cpp
artdaq::PackageBuildInfo GetPackageBuildInfo::getPackageBuildInfo() {
  artdaq::PackageBuildInfo pkg;
  pkg.setPackageName("artdaq-database");
  pkg.setPackageVersion("@version_with_git@");
  pkg.setBuildTimestamp("@utcDateTime@");
  return pkg;
}
```

**Implementation (after CMake substitution):**
```cpp
artdaq::PackageBuildInfo GetPackageBuildInfo::getPackageBuildInfo() {
  artdaq::PackageBuildInfo pkg;
  pkg.setPackageName("artdaq-database");
  pkg.setPackageVersion("v1_05_00-1-g5e0d18a");
  pkg.setBuildTimestamp("13-Nov-2025 15:30:45 UTC");
  return pkg;
}
```

## Performance Considerations

- **Memory usage:** Each call creates a new `PackageBuildInfo` object (approximately 100-200 bytes depending on string lengths)
- **Caching behavior:** No caching - a new object is created on each call. For performance-critical code that calls this frequently, consider caching the result locally
- **Known bottlenecks:** None - this is a lightweight operation with no I/O or computation

## Error Handling Strategy

This implementation has no error paths:
- All data is embedded at compile time as string literals
- No external resources are accessed at runtime
- No dynamic allocation can fail in practice (string assignments are from literals)
- The method never throws exceptions

## Testing Notes

- **Unit tests:** The build info is typically tested indirectly through integration tests
- **Key test cases:**
  - Verify package name is "artdaq-database"
  - Verify version string starts with "v" and follows expected format
  - Verify timestamp string is non-empty and contains "UTC"

**Example test:**
```cpp
#include "artdaq-database/BuildInfo/GetPackageBuildInfo.hh"
#include <cassert>
#include <string>

void testBuildInfo() {
  auto info = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();

  // Verify package name
  assert(info.getPackageName() == "artdaq-database");

  // Verify version format (starts with 'v')
  std::string version = info.getPackageVersion();
  assert(!version.empty());
  assert(version[0] == 'v');

  // Verify timestamp includes UTC
  std::string timestamp = info.getBuildTimestamp();
  assert(!timestamp.empty());
  assert(timestamp.find("UTC") != std::string::npos);
}
```

## Maintenance Notes

### Version String Format

The version string from `git describe --tags` follows this format:
- **On a tag**: `v1_05_00`
- **After a tag**: `v1_05_00-N-gHASH` where N is commits since tag and HASH is the short commit hash
- **Dirty working directory**: Appends `-dirty` if there are uncommitted changes

### Fallback Behavior

From CMakeLists.txt, if `git describe` fails (e.g., building from a tarball without git history):
```cmake
if(${GIT_DESCRIBE_RESULT} EQUAL 0)
    set(version_with_git ${GIT_DESCRIBE_VERSION})
else()
    set(version_with_git $ENV{CETPKG_VERSION})
endif()
```
The fallback uses the `CETPKG_VERSION` environment variable, which is typically set by the Fermilab UPS/Spack build system.

### Timestamp Format

The timestamp uses the format: `DD-Mon-YYYY HH:MM:SS UTC`
- Generated by: `date -u "+%d-%b-%Y %H:%M:%S %Z"`
- Always in UTC for consistency across build environments
- Example: `30-Dec-2025 15:30:45 UTC`

### Build Reproducibility Note

Because the timestamp changes on each build, this approach does not support bit-for-bit reproducible builds. If reproducibility is required, the timestamp generation could be modified to use:
- A fixed value
- The `SOURCE_DATE_EPOCH` environment variable (standard for reproducible builds)
- The commit timestamp instead of build timestamp

### File Locations

| File | Location |
|------|----------|
| Template (source) | `<source>/artdaq-database/BuildInfo/GetPackageBuildInfo.cc.in` |
| Generated (build) | `<build>/artdaq-database/BuildInfo/GetPackageBuildInfo.cc` |

The generated file is compiled from the build directory, not the source directory. The original template remains unchanged in the source tree.

### Debugging Build Information

To verify the substituted values, inspect the generated file in the build directory:
```bash
cat <build-dir>/artdaq-database/BuildInfo/GetPackageBuildInfo.cc
```

Or query at runtime:
```cpp
#include "artdaq-database/BuildInfo/GetPackageBuildInfo.hh"
#include <iostream>

void debugBuildInfo() {
  auto info = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();
  std::cout << "Package:   " << info.getPackageName() << "\n";
  std::cout << "Version:   " << info.getPackageVersion() << "\n";
  std::cout << "Timestamp: " << info.getBuildTimestamp() << "\n";
}
```

### CMake Integration Details

The CMakeLists.txt in the BuildInfo directory performs these operations:

1. **Capture UTC timestamp:**
   ```cmake
   execute_process(COMMAND "date" "-u" "+%d-%b-%Y %H:%M:%S %Z" OUTPUT_VARIABLE rawUTCDate)
   string(STRIP ${rawUTCDate} utcDateTime)
   ```

2. **Extract git version:**
   ```cmake
   execute_process(COMMAND git describe --tags
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                   RESULT_VARIABLE GIT_DESCRIBE_RESULT
                   OUTPUT_VARIABLE GIT_DESCRIBE_VERSION
                   ERROR_QUIET
                   OUTPUT_STRIP_TRAILING_WHITESPACE)
   ```

3. **Apply fallback if needed:**
   ```cmake
   if(${GIT_DESCRIBE_RESULT} EQUAL 0)
       set(version_with_git ${GIT_DESCRIBE_VERSION})
   else()
       set(version_with_git $ENV{CETPKG_VERSION})
   endif()
   ```

4. **Generate source file:**
   ```cmake
   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/GetPackageBuildInfo.cc.in
                  ${CMAKE_CURRENT_BINARY_DIR}/GetPackageBuildInfo.cc @ONLY)
   ```

The `@ONLY` flag ensures only `@variable@` syntax is substituted, not `${variable}` syntax.

## See Also

- [GetPackageBuildInfo.hh.md](./GetPackageBuildInfo.hh.md) - Header file with API documentation
- [README.md](./README.md) - BuildInfo module overview
- [External: CMake configure_file](https://cmake.org/cmake/help/latest/command/configure_file.html) - CMake documentation for template processing
- [External: git describe](https://git-scm.com/docs/git-describe) - Git documentation for version extraction

---

*Last updated: 2025-12-30*
