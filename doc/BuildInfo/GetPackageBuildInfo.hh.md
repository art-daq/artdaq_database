# GetPackageBuildInfo.hh

**Path:** `artdaq-database/BuildInfo/GetPackageBuildInfo.hh`

**Purpose:** This header file defines the interface for accessing artdaq-database package build information at runtime. It provides a static utility class within the `artdaqdatabase` namespace that follows the standard artdaq pattern for exposing version numbers, build timestamps, and package identification through a simple, thread-safe API.


## Key Concepts

### Build Information Pattern

The artdaq ecosystem uses a standard pattern where each package provides a `GetPackageBuildInfo` class with a static method that returns build metadata. This allows applications to query version information at runtime without external configuration files. The `artdaqdatabase` namespace differentiates this package's build info from other artdaq packages (like artdaq-core or artdaq) that use similar class names.

### Static Utility Class Design

The `GetPackageBuildInfo` struct uses a static method design pattern, meaning:
- No object instantiation is required to access build information
- The method is thread-safe since it creates and returns a new object each call
- The API is simple: just call `artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo()`

### PackageBuildInfo Type

The return type `artdaq::PackageBuildInfo` (from artdaq-core) is a data container with three properties:
- **Package Name**: The name of the package ("artdaq-database")
- **Package Version**: The version string with git information (e.g., "v1_05_00-1-g5e0d18a")
- **Build Timestamp**: When the package was built (e.g., "13-Nov-2025 15:30:45 UTC")

## Thread Safety

- **Thread-safe:** Yes
- **Concurrent access:** Safe - method creates a new object on each call
- **Locking:** None required - all data is embedded at compile time with no shared mutable state

## Dependencies

| Include | Purpose |
|---------|---------|
| `<string>` | Standard C++ string support for return values and internal operations |
| `artdaq-core/Data/PackageBuildInfo.hh` | Provides the `artdaq::PackageBuildInfo` class that serves as the return type for build information |

## Classes/Structures

### `GetPackageBuildInfo`

A static utility structure providing access to package build metadata. This struct exists in the `artdaqdatabase` namespace to differentiate it from similar structures in other artdaq packages.

**Thread Safety:** Thread-safe (stateless, no shared mutable state)

#### Methods

##### `getPackageBuildInfo() -> artdaq::PackageBuildInfo` [static]

**Brief:** Retrieves the version number, build timestamp, and package name for artdaq-database as a `PackageBuildInfo` object. This static method can be called without instantiating the struct.

**Parameters:** None

**Preconditions:**
- None - this method can be called at any time after library initialization

**Returns:** An `artdaq::PackageBuildInfo` object containing:
- `getPackageName()` returns `"artdaq-database"`
- `getPackageVersion()` returns the git-based version string (e.g., "v1_05_00-1-g5e0d18a")
- `getBuildTimestamp()` returns the UTC build time (e.g., "13-Nov-2025 15:30:45 UTC")

**Postconditions:**
- Returned object contains valid, non-empty strings for all three properties

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | This method does not throw exceptions |

**Thread Safety:** Safe - creates a new object on each call, no shared state

**Side Effects:**
- None - this is a pure accessor function

**Complexity:** O(1) - constant time string copy operations

**Example:**
```cpp
#include "artdaq-database/BuildInfo/GetPackageBuildInfo.hh"
#include <iostream>

void displayVersion() {
  // Get build information - no error handling needed as this never throws
  auto buildInfo = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();

  // Access individual properties
  std::cout << "Package: " << buildInfo.getPackageName() << "\n";
  std::cout << "Version: " << buildInfo.getPackageVersion() << "\n";
  std::cout << "Built:   " << buildInfo.getBuildTimestamp() << "\n";
}
```

**Expected Output:**
```
Package: artdaq-database
Version: v1_05_00-1-g5e0d18a
Built:   13-Nov-2025 15:30:45 UTC
```

## Relationship to Other Components

### Within the BuildInfo Module

- **GetPackageBuildInfo.cc.in**: The corresponding CMake template that provides the implementation with placeholder variables for version and timestamp
- **CMakeLists.txt**: Processes the template and defines build variables by running git commands

### In the Larger System

- **artdaq-core**: Provides the `artdaq::PackageBuildInfo` base type used as the return value
- **Other artdaq packages**: Follow the same pattern (artdaq, artdaq-demo, etc.) enabling consistent version checking across the ecosystem
- **Command-line tools**: Use this interface to implement `--version` flags
- **Diagnostic systems**: Query build info for error reporting and logging

### Cross-Package Usage

```cpp
#include "artdaq-database/BuildInfo/GetPackageBuildInfo.hh"
#include "artdaq-core/BuildInfo/GetPackageBuildInfo.hh"

void checkVersions() {
  auto dbInfo = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();
  auto coreInfo = artdaq::GetPackageBuildInfo::getPackageBuildInfo();

  std::cout << "artdaq-database: " << dbInfo.getPackageVersion() << "\n";
  std::cout << "artdaq-core:     " << coreInfo.getPackageVersion() << "\n";
}
```

## See Also

- [GetPackageBuildInfo.cc.in.md](./GetPackageBuildInfo.cc.in.md) - Implementation template with CMake variable substitution
- [README.md](./README.md) - BuildInfo module overview
- [External: artdaq-core PackageBuildInfo](https://cdcvs.fnal.gov/redmine/projects/artdaq-core/repository) - The base type definition

## Notes for Developers

### Header Guard Convention

The header guard follows artdaq naming conventions: `artdaq_database_BuildInfo_GetPackageBuildInfo_hh`, which mirrors the directory structure with underscores replacing path separators.

### Namespace Purpose

The `artdaqdatabase` namespace (note: no underscore or hyphen) is specifically chosen to differentiate this package's `GetPackageBuildInfo` from identically-named classes in other artdaq packages. Each artdaq package uses its own namespace for this purpose.

### Version String Format

The version string follows this format:
- **Release version** (on a tag): `v1_05_00`
- **Development version** (commits after tag): `v1_05_00-N-gHASH` where N is commits since tag and HASH is the short commit hash
- **Dirty working directory**: Appends `-dirty` if there are uncommitted changes

### Common Pitfalls

- **Pitfall 1:** Confusing namespaces - `artdaqdatabase::GetPackageBuildInfo` is different from `artdaq::GetPackageBuildInfo`. Make sure to use the correct namespace for the package you want version info from.
- **Pitfall 2:** Version parsing - The version string format includes git metadata; if you need to compare versions, you must parse the string appropriately (extracting the semantic version part before the hyphen-separated git info).

### Anti-patterns

```cpp
// DON'T do this - using wrong namespace:
auto info = artdaq::GetPackageBuildInfo::getPackageBuildInfo();
// This gives artdaq-core version, not artdaq-database version

// DO this instead - use artdaqdatabase namespace:
auto info = artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo();
// This gives the correct artdaq-database version
```

---

*Last updated: 2025-12-30*
