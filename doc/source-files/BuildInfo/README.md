# BuildInfo Module

**Last Updated:** 2026-01-14

## Overview

The BuildInfo module provides version and build timestamp information for the artdaq-database package at runtime. It implements the standard artdaq pattern for embedding and accessing build metadata, allowing applications to query version numbers, build timestamps, and package identification without requiring external configuration files.

This module is essential for:
- Version checking and compatibility verification between artdaq packages
- Diagnostic logging and error reporting with version context
- Build tracking and debugging production issues
- User information display (e.g., `--version` command-line flags)

## Architecture

The BuildInfo module uses a compile-time code generation approach:

```
Source Files                    Build Process                     Runtime
+------------------------+     +-------------------+     +----------------------+
| GetPackageBuildInfo.hh |     |                   |     |                      |
| (API interface)        |---->|                   |     | getPackageBuildInfo()|
+------------------------+     |                   |     |         |            |
                               |  CMake            |     |         v            |
+------------------------+     |  configure_file() |     | PackageBuildInfo     |
| GetPackageBuildInfo    |---->|         |         |---->| - packageName        |
| .cc.in (template)      |     |         v         |     | - packageVersion     |
+------------------------+     | GetPackageBuildInfo.cc  | - buildTimestamp     |
                               | (generated)       |     +----------------------+
+------------------------+     |                   |
| CMakeLists.txt         |---->| git describe      |
| (build config)         |     | date -u           |
+------------------------+     +-------------------+
```

### Design Pattern

**Static Utility Class**: The `GetPackageBuildInfo` struct provides a single static method that returns build metadata. This pattern:
- Requires no object instantiation
- Is inherently thread-safe (no shared mutable state)
- Provides a simple, consistent API across all artdaq packages

### Build-Time Code Generation

CMake processes the `.cc.in` template file at configure time:
1. Runs `git describe --tags` to extract the version from the repository
2. Runs `date -u` to capture the UTC build timestamp
3. Substitutes `@variable@` placeholders with actual values
4. Generates the final `.cc` file in the build directory

## Key Classes

| Class | Purpose |
|-------|---------|
| `GetPackageBuildInfo` | Static utility class providing access to package build metadata via `getPackageBuildInfo()` |

## Key Concepts

### PackageBuildInfo Type

The `artdaq::PackageBuildInfo` type (from artdaq-core) serves as the return value. It provides:
- `getPackageName()` - Returns "artdaq-database"
- `getPackageVersion()` - Returns the git-based version string
- `getBuildTimestamp()` - Returns the UTC build time

### Version String Format

The version follows `git describe --tags` output format:
- **Release version**: `v1_05_00` (on a tag)
- **Development version**: `v1_05_00-N-gHASH` (N commits after tag)
- **Dirty build**: Appends `-dirty` if uncommitted changes exist

### Namespace Separation

The `artdaqdatabase` namespace differentiates this package's build info from other artdaq packages that use the same class name pattern.

## Dependencies

### Internal
- None (this is a leaf module with no dependencies on other artdaq-database modules)

### External
- **artdaq-core**: Provides the `artdaq::PackageBuildInfo` base type
- **CMake**: For template file processing during build
- **Git**: For version extraction at build time

## Thread Safety

The `getPackageBuildInfo()` method is fully thread-safe:
- Creates a new object on each call
- All data is embedded at compile time as string literals
- No shared mutable state exists

## Files

| File | Description |
|------|-------------|
| [GetPackageBuildInfo.hh](./GetPackageBuildInfo.hh.md) | Header defining the `GetPackageBuildInfo` struct and `getPackageBuildInfo()` method |
| [GetPackageBuildInfo.cc.in](./GetPackageBuildInfo.cc.in.md) | CMake template with placeholder variables for version and timestamp |
| CMakeLists.txt | Build configuration that extracts git version and processes the template |

## Usage Examples

### Basic Version Display

```cpp
#include "artdaq-database/BuildInfo/GetPackageBuildInfo.hh"
#include <iostream>

void displayVersion() {
  auto info = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();

  std::cout << "Package:   " << info.getPackageName() << "\n";
  std::cout << "Version:   " << info.getPackageVersion() << "\n";
  std::cout << "Built:     " << info.getBuildTimestamp() << "\n";
}
```

**Output:**
```
Package:   artdaq-database
Version:   v1_05_00-1-g5e0d18a
Built:     13-Nov-2025 15:30:45 UTC
```

### Command-Line --version Flag

```cpp
#include "artdaq-database/BuildInfo/GetPackageBuildInfo.hh"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  if (argc > 1 && std::string(argv[1]) == "--version") {
    auto info = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();
    std::cout << info.getPackageName() << " "
              << info.getPackageVersion() << "\n";
    return 0;
  }
  // ... normal application logic
  return 0;
}
```

### Startup Logging

```cpp
#include "artdaq-database/BuildInfo/GetPackageBuildInfo.hh"
#include <iostream>

void logStartup() {
  auto info = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();

  std::clog << "[INFO] Starting " << info.getPackageName()
            << " " << info.getPackageVersion() << "\n";
  std::clog << "[INFO] Built: " << info.getBuildTimestamp() << "\n";
}
```

### Error Reporting with Version Context

```cpp
#include "artdaq-database/BuildInfo/GetPackageBuildInfo.hh"
#include <iostream>
#include <stdexcept>

void handleError(std::exception const& e) {
  auto info = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();

  std::cerr << "Error in " << info.getPackageName()
            << " " << info.getPackageVersion() << "\n";
  std::cerr << "Built: " << info.getBuildTimestamp() << "\n";
  std::cerr << "Message: " << e.what() << "\n";
}
```

### Cross-Package Version Check

```cpp
#include "artdaq-database/BuildInfo/GetPackageBuildInfo.hh"
#include "artdaq-core/BuildInfo/GetPackageBuildInfo.hh"
#include <iostream>

void checkPackageVersions() {
  auto dbInfo = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();
  auto coreInfo = artdaq::GetPackageBuildInfo::getPackageBuildInfo();

  std::cout << "artdaq-database: " << dbInfo.getPackageVersion() << "\n";
  std::cout << "artdaq-core:     " << coreInfo.getPackageVersion() << "\n";
}
```

## Integration with artdaq Ecosystem

All artdaq packages follow the same build info pattern:

| Package | Namespace | Class |
|---------|-----------|-------|
| artdaq-core | `artdaq` | `GetPackageBuildInfo` |
| artdaq | `artdaq` | `GetPackageBuildInfo` |
| artdaq-database | `artdaqdatabase` | `GetPackageBuildInfo` |
| artdaq-demo | `artdaqdemo` | `GetPackageBuildInfo` |

This consistent pattern enables:
- Uniform version checking across the ecosystem
- Common logging and diagnostic patterns
- Simplified tooling for version compatibility checks

## Limitations

### Build Timestamp Non-Reproducibility

The build timestamp changes on each build, breaking bit-for-bit reproducible builds. If reproducibility is required, consider:
- Using a fixed timestamp value
- Using `SOURCE_DATE_EPOCH` environment variable
- Using the commit timestamp instead of build timestamp

### Git Repository Requirement

Version extraction requires a git repository. When building from a tarball:
- The fallback uses `CETPKG_VERSION` environment variable
- Release tarballs should pre-configure the version

## See Also

- [SharedCommon](../SharedCommon/README.md) - Foundation utilities used across the project
- [External: artdaq-core](https://cdcvs.fnal.gov/redmine/projects/artdaq-core/repository) - Provides the PackageBuildInfo base type
- [External: CMake configure_file](https://cmake.org/cmake/help/latest/command/configure_file.html) - Template processing documentation
