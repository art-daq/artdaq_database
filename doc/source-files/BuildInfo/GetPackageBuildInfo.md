# GetPackageBuildInfo

## Overview

The GetPackageBuildInfo module provides version and build information for the artdaq-database package. It follows the artdaq convention for package build information, allowing runtime querying of version numbers and build timestamps.

## Files

### GetPackageBuildInfo.hh

**Location**: `/home/user/artdaq-database/artdaq-database/BuildInfo/GetPackageBuildInfo.hh`

**Type**: Header file

**Purpose**: Interface definition for build information access

### GetPackageBuildInfo.cc.in

**Location**: `/home/user/artdaq-database/artdaq-database/BuildInfo/GetPackageBuildInfo.cc.in`

**Type**: CMake template implementation file

**Purpose**: Implementation with CMake-substituted build values

---

## Header File (GetPackageBuildInfo.hh)

### Namespace

```cpp
namespace artdaqdatabase {
```

**Purpose**: Differentiates artdaq-database build info from other artdaq packages

### Class Definition

```cpp
struct GetPackageBuildInfo {
  static artdaq::PackageBuildInfo getPackageBuildInfo();
};
```

**Design Pattern**: Static utility class with single static method

---

## Implementation File (GetPackageBuildInfo.cc.in)

### Template Variables

The implementation uses CMake substitution variables:

| Variable | Description | Example |
|----------|-------------|---------|
| `@version_with_git@` | Version with git commit info | `v1_05_00-1-g5e0d18a` |
| `@utcDateTime@` | UTC build timestamp | `2025-11-13 15:30:45 UTC` |

### CMake Processing

During build:
1. CMake reads `GetPackageBuildInfo.cc.in`
2. Substitutes `@variable@` placeholders
3. Generates `GetPackageBuildInfo.cc`
4. Compiles into library

---

## API Documentation

### getPackageBuildInfo()

```cpp
static artdaq::PackageBuildInfo getPackageBuildInfo();
```

**Returns**: `artdaq::PackageBuildInfo` object containing:
- Package name: "artdaq-database"
- Version string with git info
- Build timestamp in UTC

**Usage Example**:

```cpp
#include "artdaq-database/BuildInfo/GetPackageBuildInfo.hh"

auto buildInfo = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();

std::cout << "Package: " << buildInfo.getPackageName() << "\n";
std::cout << "Version: " << buildInfo.getPackageVersion() << "\n";
std::cout << "Built: " << buildInfo.getBuildTimestamp() << "\n";
```

**Expected Output**:
```
Package: artdaq-database
Version: v1_05_00-1-g5e0d18a
Built: 2025-11-13 15:30:45 UTC
```

---

## Version String Format

### Standard Format

```
v{major}_{minor}_{patch}[-{commits}]-g{hash}
```

**Components**:
- **major**: Major version number
- **minor**: Minor version number
- **patch**: Patch version number
- **commits**: Commits since last tag (if not on tag)
- **hash**: Short git commit hash

### Examples

**Release Version** (on tag):
```
v1_05_00
```

**Development Version** (after tag):
```
v1_05_00-1-g5e0d18a
```
- 1 commit after v1_05_00 tag
- Commit hash: 5e0d18a

---

## Integration with artdaq

### PackageBuildInfo Type

Defined in `artdaq-core/Data/PackageBuildInfo.hh`

**Methods**:
- `std::string getPackageName()`
- `std::string getPackageVersion()`
- `std::string getBuildTimestamp()`

### Standard Pattern

All artdaq packages follow this pattern:
- **artdaq-core**: `artdaq::GetPackageBuildInfo`
- **artdaq**: `artdaq::GetPackageBuildInfo` (different namespace usage)
- **artdaq-database**: `artdaqdatabase::GetPackageBuildInfo`

This allows version checking across entire artdaq suite.

---

## Use Cases

### Version Reporting

Display version at startup:
```cpp
void reportVersion() {
  auto info = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();
  std::cout << "artdaq-database version: "
            << info.getPackageVersion() << "\n";
}
```

### Compatibility Checking

Verify version requirements:
```cpp
bool checkVersion() {
  auto info = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();
  auto version = info.getPackageVersion();

  // Parse version and check
  if (parseVersion(version) < requiredVersion) {
    std::cerr << "Error: Requires artdaq-database >= "
              << requiredVersionStr << "\n";
    return false;
  }
  return true;
}
```

### Build Tracking

Log build information for debugging:
```cpp
void logBuildInfo() {
  auto info = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();

  logger->info("Package: {}", info.getPackageName());
  logger->info("Version: {}", info.getPackageVersion());
  logger->info("Built: {}", info.getBuildTimestamp());
}
```

### Diagnostic Output

Include in error reports:
```cpp
void reportError(std::string const& error) {
  auto info = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();

  std::cerr << "Error in " << info.getPackageName()
            << " " << info.getPackageVersion() << "\n";
  std::cerr << "Built: " << info.getBuildTimestamp() << "\n";
  std::cerr << "Error: " << error << "\n";
}
```

---

## CMake Integration

### Template Processing

In `CMakeLists.txt`:

```cmake
# Get git version
execute_process(
  COMMAND git describe --tags --always --dirty
  OUTPUT_VARIABLE GIT_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get build timestamp
string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S UTC" UTC)

# Configure file
configure_file(
  BuildInfo/GetPackageBuildInfo.cc.in
  BuildInfo/GetPackageBuildInfo.cc
  @ONLY
)
```

### Generated File

The generated `GetPackageBuildInfo.cc`:

```cpp
#include "artdaq-database/BuildInfo/GetPackageBuildInfo.hh"

#include <iostream>
#include <string>

namespace artdaqdatabase {
artdaq::PackageBuildInfo GetPackageBuildInfo::getPackageBuildInfo() {
  artdaq::PackageBuildInfo pkg;
  pkg.setPackageName("artdaq-database");
  pkg.setPackageVersion("v1_05_00-1-g5e0d18a");  // Substituted
  pkg.setBuildTimestamp("2025-11-13 15:30:45 UTC");  // Substituted
  return pkg;
}
}
```

---

## Build-Time Information Capture

### Version Capture

**Source**: Git repository
**Command**: `git describe --tags --always --dirty`

**Flags**:
- `--tags`: Use tag names for version
- `--always`: Show commit hash if no tag
- `--dirty`: Append "-dirty" if uncommitted changes

### Timestamp Capture

**Source**: Build system time
**Format**: ISO 8601 with UTC timezone
**Precision**: Seconds

### Build Reproducibility

**Note**: Timestamps make builds non-reproducible
- Same source can have different timestamps
- Useful for tracking when binary was built
- Can be disabled for reproducible builds if needed

---

## Runtime Usage Patterns

### Command-Line Tools

```cpp
int main(int argc, char* argv[]) {
  if (argc > 1 && std::string(argv[1]) == "--version") {
    auto info = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();
    std::cout << info.getPackageVersion() << "\n";
    return 0;
  }
  // ... rest of program
}
```

### Library Initialization

```cpp
class DatabaseManager {
  DatabaseManager() {
    auto info = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();
    logInfo("Initialized " + info.getPackageName() +
            " " + info.getPackageVersion());
  }
};
```

### Version Mismatch Detection

```cpp
void checkCompatibility() {
  auto dbInfo = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();
  auto coreInfo = artdaq::GetPackageBuildInfo::getPackageBuildInfo();

  if (!areCompatible(dbInfo.getPackageVersion(),
                     coreInfo.getPackageVersion())) {
    throw std::runtime_error(
      "Incompatible versions: artdaq-database " +
      dbInfo.getPackageVersion() +
      " with artdaq-core " +
      coreInfo.getPackageVersion()
    );
  }
}
```

---

## Dependencies

### Header Dependencies
- `<string>` - String handling
- `artdaq-core/Data/PackageBuildInfo.hh` - PackageBuildInfo type

### Build Dependencies
- CMake (for template processing)
- Git (for version extraction)

### No Runtime Dependencies
- All information embedded at compile time
- No external files or network access needed

---

## Testing

### Verification

```cpp
#include "artdaq-database/BuildInfo/GetPackageBuildInfo.hh"
#include <cassert>

void testBuildInfo() {
  auto info = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();

  // Verify package name
  assert(info.getPackageName() == "artdaq-database");

  // Verify version format (basic check)
  auto version = info.getPackageVersion();
  assert(!version.empty());
  assert(version[0] == 'v');  // Should start with 'v'

  // Verify timestamp format (basic check)
  auto timestamp = info.getBuildTimestamp();
  assert(!timestamp.empty());
  assert(timestamp.find("UTC") != std::string::npos);

  std::cout << "Build info tests passed\n";
}
```

### Example Output

```cpp
void displayBuildInfo() {
  auto info = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();

  std::cout << "===== Build Information =====\n";
  std::cout << "Package:   " << info.getPackageName() << "\n";
  std::cout << "Version:   " << info.getPackageVersion() << "\n";
  std::cout << "Built:     " << info.getBuildTimestamp() << "\n";
  std::cout << "============================\n";
}
```

Output:
```
===== Build Information =====
Package:   artdaq-database
Version:   v1_05_00-1-g5e0d18a
Built:     2025-11-13 15:30:45 UTC
============================
```

---

## Best Practices

### 1. Version Checking

Always check version in critical applications:
```cpp
void ensureVersion(std::string const& minVersion) {
  auto info = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();
  if (compareVersions(info.getPackageVersion(), minVersion) < 0) {
    throw std::runtime_error("Requires artdaq-database >= " + minVersion);
  }
}
```

### 2. Logging

Log version at application start:
```cpp
int main() {
  auto info = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();
  std::clog << "Starting with " << info.getPackageName()
            << " " << info.getPackageVersion() << "\n";
  // ... application code
}
```

### 3. Error Reporting

Include version in error messages:
```cpp
void handleError(std::exception const& e) {
  auto info = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();
  std::cerr << "Error in " << info.getPackageName()
            << " " << info.getPackageVersion() << ":\n"
            << e.what() << "\n";
}
```

### 4. Diagnostic Files

Write version to diagnostic logs:
```cpp
void writeDiagnostics(std::ostream& out) {
  auto info = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();
  out << "Software: " << info.getPackageName() << "\n";
  out << "Version: " << info.getPackageVersion() << "\n";
  out << "Built: " << info.getBuildTimestamp() << "\n";
  // ... more diagnostics
}
```

---

## Maintenance Notes

### Updating Version

Version automatically updated by git:
- Tags in format `vX_YY_ZZ`
- Commit count auto-incremented
- Hash auto-updated

### Manual Version Override

If needed, edit CMakeLists.txt:
```cmake
set(ARTDAQ_DATABASE_VERSION "v1_05_00" CACHE STRING "Version override")
```

### Build Timestamp

Timestamp generated at CMake configure time:
- Reconfigure to update timestamp
- Clean build not necessary for timestamp update

---

## Related Files

- **CMakeLists.txt**: Build configuration with version extraction
- **artdaq-core/Data/PackageBuildInfo.hh**: Base type definition
- **Version control**: Git tags define official versions

---

## Summary

The GetPackageBuildInfo module provides a simple, standard interface for accessing artdaq-database version and build information at runtime. It follows artdaq conventions and integrates with the broader artdaq ecosystem for version checking and compatibility verification.

**Key Points**:
- Template-based build information embedding
- CMake-time version and timestamp capture
- Runtime queryable through static method
- Compatible with artdaq PackageBuildInfo standard
- Useful for version checking, logging, and diagnostics
