# ConfigurationInterface Tutorial

## Complete Guide to Using the artdaq-database Configuration API

**Document Version:** 2.1

---

## Table of Contents

1. [Introduction](#1-introduction)
2. [Core Concepts](#2-core-concepts)
3. [Data Format Types](#3-data-format-types)
4. [Environment Setup](#4-environment-setup)
5. [Getting Started](#5-getting-started)
6. [Working with Configuration Versions](#6-working-with-configuration-versions)
7. [Working with Global Configurations](#7-working-with-global-configurations)
8. [Making Your Configuration Serializable](#8-making-your-configuration-serializable)
9. [Error Handling and Exception Types](#9-error-handling-and-exception-types)
10. [Safe vs Throwing Methods](#10-safe-vs-throwing-methods)
11. [Multi-threaded Operations](#11-multi-threaded-operations)
12. [Complete Working Examples](#12-complete-working-examples)
13. [Real-World Integration Pattern (OtsDaq)](#13-real-world-integration-pattern-otsdaq)
14. [Troubleshooting Guide](#14-troubleshooting-guide)
15. [Best Practices](#15-best-practices)
16. [Quick Reference](#16-quick-reference)
- [Appendix A: Internal Types and Helpers](#appendix-a-internal-types-and-helpers)
- [Appendix B: Complete Code Template](#appendix-b-complete-code-template)
- [Appendix C: Exception Hierarchy](#appendix-c-exception-hierarchy)
- [Appendix D: Complete API Reference](#appendix-d-complete-api-reference)

---

## 1. Introduction

### What is ConfigurationInterface?

`ConfigurationInterface` is the primary C++ class for storing and retrieving configurations in the artdaq-database system. Think of it as your gateway to a configuration database - it allows you to:

- **Store** configuration data with version control
- **Load** configurations by version
- **List** available versions and configurations
- **Create** global configurations (groups of related configurations)
- **Search** for configurations containing specific versions
- **Protect** configurations by marking them read-only

### Why Use ConfigurationInterface?

In data acquisition (DAQ) systems, you often need to:
- Keep track of different configuration versions for experiments
- Group related configurations together for specific runs
- Retrieve exact configurations used in past experiments
- Protect production configurations from accidental changes

`ConfigurationInterface` solves all these problems with a clean, type-safe API.

### Supported Storage Backends

The same API works with three different storage backends:
- **FileSystemDB** - Stores configurations as JSON files on disk
- **MongoDB** - Stores configurations in a MongoDB database
- **UconDB** - Stores configurations via Fermilab's UconDB REST service

You select the backend via the `ARTDAQ_DATABASE_URI` environment variable.

### Source Files

The ConfigurationInterface is defined in:
- **Header**: `artdaq-database/ConfigurationDB/configurationdbifc.h`
- **Base Types**: `artdaq-database/ConfigurationDB/configurationdbifc_base.h`
- **Data Formats**: `artdaq-database/BasicTypes/basictypes.h`

---

## 2. Core Concepts

Before diving into code, let's understand the key concepts:

### 2.1 Configuration

A **configuration** is a C++ object containing settings for a DAQ component. For example:
- Trigger settings
- Board reader parameters
- Event builder configuration

### 2.2 Collection (Configuration Type)

A **collection** groups configurations of the same type. The collection name is typically derived from your C++ class name:
- `TriggerConfig` collection contains all trigger configurations
- `BoardReaderConfig` collection contains all board reader configurations

**Important:** The collection name is determined by the `configurationNameImpl()` method in your `MakeSerializable` specialization.

### 2.3 Version

A **version** is a unique identifier for a specific configuration state. Common versioning schemes:
- Numeric: `"1"`, `"2"`, `"3"`
- Semantic: `"v1.0.0"`, `"v1.0.1"`
- Date-based: `"2025-01-15"`, `"2025-01-16"`

**Constraint:** Version strings cannot be empty. The API will reject empty version strings with an error.

### 2.4 Entity

An **entity** identifies a specific instance of a configuration. For example:
- `"DAQ1"` - First DAQ component
- `"DAQ2"` - Second DAQ component
- `"BoardReader_01"` - First board reader

**Entity Parameter Behavior:**

| Context | Empty Entity Behavior |
|---------|----------------------|
| `storeVersion`, `loadVersion`, etc. | Entity option is **not set** (omitted from query) |
| `VersionInfo` for global configurations | **NOT ALLOWED** - `validate()` throws exception |
| `findGlobalConfigurationsContaining` | Empty means "match any entity" |

**Implementation Detail:** When you pass an empty string to version methods, the entity is not set at all:
```cpp
// From the implementation:
if (!entity.empty()) opts.entity(entity);  // Only sets if non-empty
```

This means an empty entity is **not the same** as setting entity to an empty string explicitly - the option is simply omitted from the database query. The exact behavior depends on the storage provider.

**Best Practice:** Always use non-empty, consistent entity names to avoid ambiguity.

### 2.5 Global Configuration (Composition)

A **global configuration** groups multiple configuration versions together. For example, "Run12345" might include:
- TriggerConfig v2.0 for entity "Trigger1"
- BoardReaderConfig v1.5 for entity "BoardReader_01"
- BoardReaderConfig v1.5 for entity "BoardReader_02"

**Key Point:** A global configuration is also called a "composition" in the codebase. Both terms are used interchangeably.

### 2.6 VersionInfo

A `VersionInfo` struct holds the three pieces needed to identify a configuration:

```cpp
// Defined in artdaq-database/ConfigurationDB/configurationdbifc_base.h
struct VersionInfo {
    std::string configuration;  // Collection/type name (e.g., "TriggerConfig")
    std::string version;        // Version string (e.g., "v1.0")
    std::string entity;         // Entity name (e.g., "DAQ1")

    // Validates that all fields are non-empty
    // THROWS: artdaq::database::invalid_option_exception if any field is empty
    void validate() const;
};
```

**The `validate()` Method:**

```cpp
void VersionInfo::validate() const {
    if (configuration.empty())
        throw artdaq::database::invalid_option_exception("VersionInfo")
            << "Configuration name is empty";

    if (version.empty())
        throw artdaq::database::invalid_option_exception("VersionInfo")
            << "Version is empty";

    if (entity.empty())
        throw artdaq::database::invalid_option_exception("VersionInfo")
            << "Entity is empty";
}
```

**Important:** When storing global configurations, each `VersionInfo` is validated. All three fields must be non-empty.

### 2.7 result_t

Most methods return `result_t`, which is a type alias for `std::pair<bool, std::string>`.

**Important - Two Definitions Exist:**

```cpp
// Definition 1: In artdaq-database/ConfigurationDB/dispatch_signatures.h
namespace artdaq::database::configuration {
    using result_t = std::pair<bool, std::string>;  // Used by ConfigurationInterface
}

// Definition 2: In artdaq-database/SharedCommon/returned_result.h
namespace artdaq::database {
    using result_t = std::pair<bool, std::string>;  // Used by storage providers
}
```

Both definitions are equivalent (`std::pair<bool, std::string>`), but exist in different namespaces. The `ConfigurationInterface` uses `artdaq::database::configuration::result_t`.

```cpp
// result.first  = success (true) or failure (false)
// result.second = message (success message or error description)
```

**Accessing result_t in your code:**

```cpp
namespace db = artdaq::database::configuration;

// Method 1: Use namespace alias (RECOMMENDED)
db::result_t result = ifc.storeVersion(...);

// Method 2: Direct std::pair usage (always works, avoids namespace issues)
std::pair<bool, std::string> result = ifc.storeVersion(...);

// Method 3: C++17 structured bindings (recommended for readability)
auto [success, message] = ifc.storeVersion(...);
```

**Error Message Format - IMPORTANT:**

**The error message format varies by method.** Different methods return different formats:

| Methods | Error Format | Example |
|---------|--------------|---------|
| `storeVersion`, `loadVersion`, `overwriteVersion`, `markVersionReadonly` | **Raw exception message** | `"ConfigurationInterface::storeVersion: Version is empty"` |
| `storeGlobalConfiguration`, `storeGlobalConfiguration_mt` | `{error:"..."}` format | `{error:"Version info list is empty"}` |
| All `*_safe` methods | `{error:"..."}` format | `{error:"Exception message"}` |

**Why the inconsistency?** The core version methods catch exceptions and return `e.what()` directly:
```cpp
// In storeVersion, loadVersion, overwriteVersion, markVersionReadonly:
} catch (std::exception const& e) {
    return {false, {e.what()}};  // Raw exception message
}
```

The global configuration and safe methods use the `make_error_msg()` helper:
```cpp
// In storeGlobalConfiguration, *_safe methods:
} catch (std::exception const& e) {
    return {false, make_error_msg(e.what())};  // Wrapped in {error:"..."}
}
```

**Best Practice - Handle Both Formats:**
```cpp
auto [success, message] = ifc.storeVersion(...);
if (!success) {
    // Don't parse the message format - just log or display as-is
    std::cerr << "Operation failed: " << message << "\n";
    // Or use TLOG:
    TLOG(TLVL_ERROR) << "Operation failed: " << message;
}
```

**Note:** Storage providers may also return messages in `{"message":"..."}` format (from `artdaq-database/SharedCommon/returned_result.h`), which differs from both formats above.

### 2.8 VersionInfoList_t

A type alias for lists of `VersionInfo`:

```cpp
using VersionInfoList_t = std::list<VersionInfo>;
```

This is used for global configuration operations.

---

## 3. Data Format Types

### 3.1 Overview

The ConfigurationInterface supports two data formats for serialization:
- **JsonData** - JSON format (recommended for most use cases)
- **FhiclData** - FHiCL format (Fermilab Hierarchical Configuration Language)

Both types are defined in `artdaq-database/BasicTypes/basictypes.h`.

### 3.2 JsonData

```cpp
// Defined in artdaq-database/BasicTypes/data_json.h
namespace artdaq::database::basictypes {

struct JsonData final {
    // Constructor - initialize with JSON string (REQUIRED - no default constructor)
    JsonData(std::string);

    // The actual JSON content - this is where your serialized data goes
    std::string json_buffer;

    // Conversion methods (for internal use, rarely needed directly)
    template <typename TYPE>
    bool convert_to(TYPE&) const;

    template <typename TYPE>
    bool convert_from(TYPE const&);

    // String conversion operators (for implicit conversions)
    operator std::string const&() const;  // Read-only access
    operator std::string&();               // Read-write access

    // Check if buffer is empty
    bool empty() const;

    // Version identifier
    static constexpr auto type_version() { return "V1.0.0"; }
};

// Stream operators for I/O
std::ostream& operator<<(std::ostream&, JsonData const&);
std::istream& operator>>(std::istream&, JsonData&);

} // namespace
```

**Important:** `JsonData` has **NO default constructor**. You must always initialize it with a string:

```cpp
JsonData data("{}");      // Empty JSON object - OK
JsonData data2;           // COMPILE ERROR - no default constructor!
```

**Usage Example:**

```cpp
using artdaq::database::basictypes::JsonData;

// Create from JSON string (required - no default constructor)
JsonData data("{\"key\": \"value\"}");

// Access the buffer directly (most common usage)
std::cout << data.json_buffer << "\n";

// Use implicit string conversion
std::string str = data;  // Converts to string

// Use with streams (for ConfigurationInterface internal use)
std::ostringstream oss;
oss << data;  // Uses operator<<
```

### 3.3 FhiclData

```cpp
// Defined in artdaq-database/BasicTypes/data_fhicl.h
namespace artdaq::database::basictypes {

struct FhiclData final {
    // Constructors
    FhiclData(std::string);       // From FHiCL string
    FhiclData() = default;        // Default constructor (unlike JsonData!)

    // Conversion from JsonData (automatic conversion)
    FhiclData(JsonData const&);

    // Conversion operators
    operator JsonData() const;         // Convert to JsonData
    operator std::string const&() const;  // Read-only string access (NO non-const version!)

    // The FHiCL content
    std::string fhicl_buffer = "";

    // Optional source file name (for tracking/debugging)
    std::string fhicl_file_name = "notprovided";

    // Version identifier
    static constexpr auto type_version() { return "V100"; }
};

// Stream operators for I/O
std::ostream& operator<<(std::ostream&, FhiclData const&);
std::istream& operator>>(std::istream&, FhiclData&);

} // namespace
```

**Key Differences from JsonData:**
- `FhiclData` HAS a default constructor; `JsonData` does NOT
- `FhiclData` only provides `operator std::string const&()` (read-only); `JsonData` also has `operator std::string&()` (read-write)
- `FhiclData` can be constructed from `JsonData` and converted back to `JsonData`

**Usage Example:**

```cpp
using artdaq::database::basictypes::FhiclData;
using artdaq::database::basictypes::JsonData;

// Create from FHiCL string
FhiclData data("param1: 42\nparam2: \"test\"");

// Default construction is allowed (unlike JsonData)
FhiclData emptyData;  // OK - fhicl_buffer will be ""

// Access the buffer directly
std::cout << data.fhicl_buffer << "\n";

// Convert to JsonData (for storage or interop)
JsonData json = static_cast<JsonData>(data);

// Create FhiclData from JsonData
FhiclData fromJson(json);
```

### 3.4 Choosing Between JsonData and FhiclData

| Feature | JsonData | FhiclData |
|---------|----------|-----------|
| Interoperability | Standard format | art/LArSoft ecosystem |
| Human Readability | Good | Excellent for nested configs |
| Tool Support | Universal | FHiCL-specific tools |
| Recommended For | General use, web APIs | art-based experiments |

**Best Practice:** Use `JsonData` unless you have a specific requirement for FHiCL format compatibility with existing art/LArSoft configurations.

---

## 4. Environment Setup

### 4.1 Setting the Database URI

The `ARTDAQ_DATABASE_URI` environment variable determines which storage backend to use:

```bash
# For FileSystemDB (recommended for development)
export ARTDAQ_DATABASE_URI="filesystemdb:///path/to/your/database"

# For MongoDB
export ARTDAQ_DATABASE_URI="mongodb://localhost:27017/my_database"

# For UconDB
export ARTDAQ_DATABASE_URI="ucondb://ucondb.fnal.gov:443/my_database"
```

### 4.2 Creating a FileSystemDB Directory

```bash
# Create database directory
mkdir -p /data/artdaq/configdb

# Set environment variable
export ARTDAQ_DATABASE_URI="filesystemdb:///data/artdaq/configdb"
```

### 4.3 Required Headers

```cpp
// Main interface header - this is the only required header
#include "artdaq-database/ConfigurationDB/configurationdbifc.h"

// Data types are included automatically, but you can also include explicitly:
#include "artdaq-database/BasicTypes/basictypes.h"

// For implementing MakeSerializable (in your implementation file):
#include "artdaq-database/ConfigurationDB/configurationdbifc_base.h"

// Namespace aliases (recommended)
namespace db = artdaq::database::configuration;
using artdaq::database::basictypes::JsonData;
using artdaq::database::basictypes::FhiclData;
```

### 4.4 Linking

Add to your CMakeLists.txt:

```cmake
target_link_libraries(your_target
    artdaq-database::artdaq-database_ConfigurationDB
)
```

---

## 5. Getting Started

### 5.1 Creating a ConfigurationInterface Instance

```cpp
#include "artdaq-database/ConfigurationDB/configurationdbifc.h"

namespace db = artdaq::database::configuration;

void example() {
    // Method 1: Default constructor (RECOMMENDED)
    db::ConfigurationInterface ifc;

    // Method 2: With connection string parameter
    // NOTE: The string parameter is currently IGNORED by the implementation.
    // It exists for API compatibility but has no effect.
    db::ConfigurationInterface ifc2("any_string_here");  // Parameter is ignored!

    // The interface is now ready to use
}
```

**Constructor Details:**

```cpp
// From configurationdbifc.h:
struct ConfigurationInterface final {
    ConfigurationInterface() = default;              // Default - use this
    ConfigurationInterface(std::string const&){};   // Parameter is ignored!
    ConfigurationInterface(ConfigurationInterface&&) = default;  // Move OK

    // DELETED - no copying allowed
    ConfigurationInterface(ConfigurationInterface const&) = delete;
    ConfigurationInterface& operator=(ConfigurationInterface const&) = delete;
    ConfigurationInterface& operator=(ConfigurationInterface&&) = delete;
};
```

**Key Points:**
- The string constructor parameter is **completely ignored** - it exists only for backward compatibility
- Instances are **move-constructible** but NOT copyable
- Create a new instance for each operation or keep one instance per thread
- The interface is **NOT thread-safe** for sharing across threads

### 5.2 Your First Store and Load

Here's a minimal example assuming you have a serializable configuration:

```cpp
#include "artdaq-database/ConfigurationDB/configurationdbifc.h"
#include "artdaq-database/BasicTypes/basictypes.h"

namespace db = artdaq::database::configuration;
using artdaq::database::basictypes::JsonData;

// Assume MyConfig is your configuration class with MakeSerializable implemented
void storeAndLoadExample(MyConfig* config) {
    db::ConfigurationInterface ifc;

    // Store the configuration
    // Note: For storing, use const pointer (MyConfig const*)
    auto storeResult = ifc.storeVersion<MyConfig const*, JsonData>(
        config,           // Your configuration object (pointer)
        "v1.0",          // Version string (REQUIRED - cannot be empty)
        "MyComponent1"   // Entity name (optional but recommended)
    );

    if (storeResult.first) {
        std::cout << "Configuration stored successfully!\n";
    } else {
        std::cerr << "Store failed: " << storeResult.second << "\n";
        return;
    }

    // Load the configuration back
    // Note: For loading, use non-const pointer (MyConfig*)
    MyConfig loadedConfig;
    auto loadResult = ifc.loadVersion<MyConfig*, JsonData>(
        &loadedConfig,    // Configuration to populate (non-const!)
        "v1.0",          // Version to load
        "MyComponent1"   // Entity name
    );

    if (loadResult.first) {
        std::cout << "Configuration loaded successfully!\n";
        // loadedConfig now contains the data
    } else {
        std::cerr << "Load failed: " << loadResult.second << "\n";
    }
}
```

**Critical Points:**
1. Use `const*` pointer type for **storing** (data won't be modified)
2. Use non-const `*` pointer type for **loading** (data will be written)
3. Version string **cannot be empty** - will throw `invalid_option_exception`

---

## 6. Working with Configuration Versions

### 6.1 Storing a New Version

Use `storeVersion` to save a configuration for the first time:

```cpp
template <typename CONF, typename TYPE>
result_t storeVersion(CONF configuration,
                      std::string const& version,
                      std::string const& entity) const noexcept;
```

**Template Parameters:**
- `CONF` - Your configuration type (usually a pointer like `MyConfig const*`)
- `TYPE` - Storage format: `JsonData` or `FhiclData`

**Parameters:**
- `configuration` - Pointer to your configuration object (use const pointer)
- `version` - Version string (**REQUIRED** - must not be empty)
- `entity` - Entity name (can be empty, but not recommended)

**Returns:**
- `result_t` - Pair of (success, message)

**Internal Implementation Flow:**
1. Validates version is not empty
2. Creates a `ConfigurationSerializer` to wrap your configuration
3. Gets the collection name from `configurationName()`
4. Calls `writeDocument<TYPE>()` to serialize your config to data
5. Calls the storage provider's `write_document()` operation

**Example:**

```cpp
db::ConfigurationInterface ifc;
MyConfig config;
config.param1 = 42;
config.param2 = "test";

auto result = ifc.storeVersion<MyConfig const*, JsonData>(
    &config,
    "v1.0.0",
    "DAQComponent1"
);

if (!result.first) {
    std::cerr << "Failed to store: " << result.second << "\n";
}
```

**Common Errors:**
- `"Version is empty"` - You provided an empty version string
- `"Document already exists"` - This version already exists (use `overwriteVersion` instead)
- Serialization errors if `writeDocumentImpl` fails

### 6.2 Overwriting an Existing Version

Use `overwriteVersion` to update an existing configuration:

```cpp
template <typename CONF, typename TYPE>
result_t overwriteVersion(CONF configuration,
                          std::string const& version,
                          std::string const& entity) const noexcept;
```

**Internal Implementation Flow:**
1. Validates version is not empty
2. **First reads** the existing document from the database
3. Serializes your new configuration data
4. **Replaces only the document portion** while preserving metadata
5. Writes the updated document back

**Example:**

```cpp
db::ConfigurationInterface ifc;
MyConfig updatedConfig;
updatedConfig.param1 = 100;  // Changed value

auto result = ifc.overwriteVersion<MyConfig const*, JsonData>(
    &updatedConfig,
    "v1.0.0",      // Existing version to overwrite
    "DAQComponent1"
);

if (!result.first) {
    std::cerr << "Failed to overwrite: " << result.second << "\n";
}
```

**Warning:** Overwriting is irreversible. Consider storing as a new version instead when possible.

**Common Errors:**
- `"Version is empty"` - Empty version string
- `"Document not found"` - The version you're trying to overwrite doesn't exist
- `"Document is read-only"` - Version was marked read-only

### 6.3 Loading a Configuration

Use `loadVersion` to retrieve a configuration from the database:

```cpp
template <typename CONF, typename TYPE>
result_t loadVersion(CONF configuration,
                     std::string const& version,
                     std::string const& entity) const noexcept;
```

**Important:** Use a **non-const pointer** for the `CONF` type parameter since the configuration will be modified (populated with data).

**Example:**

```cpp
db::ConfigurationInterface ifc;
MyConfig config;  // Will be populated

auto result = ifc.loadVersion<MyConfig*, JsonData>(
    &config,         // Non-const pointer!
    "v1.0.0",
    "DAQComponent1"
);

if (result.first) {
    // config is now populated with database data
    std::cout << "Loaded param1: " << config.param1 << "\n";
} else {
    std::cerr << "Load failed: " << result.second << "\n";
}
```

**Common Errors:**
- `"Document not found"` - No configuration exists with that version/entity
- `"Version is empty"` - Empty version string provided
- Deserialization errors if `readDocumentImpl` fails

### 6.4 Listing Available Versions

Use `getVersions` to see what versions exist:

```cpp
template <typename CONF>
std::list<std::string> getVersions(CONF configuration,
                                   std::string const& entity) const;
```

**Important:** This method **THROWS exceptions** on error. For exception-free operation, use `getVersions_safe()`.

**Example:**

```cpp
db::ConfigurationInterface ifc;
MyConfig config;  // Used only to determine collection name

try {
    auto versions = ifc.getVersions(&config, "DAQComponent1");

    std::cout << "Available versions:\n";
    for (const auto& ver : versions) {
        std::cout << "  - " << ver << "\n";
    }
} catch (const artdaq::database::runtime_exception& e) {
    std::cerr << "Database error: " << e.what() << "\n";
} catch (const std::exception& e) {
    std::cerr << "Error listing versions: " << e.what() << "\n";
}
```

**Note:** The configuration object is only used to determine the collection name via `configurationName()`. Its data content is not used.

### 6.5 Marking a Version as Read-Only

Use `markVersionReadonly` to protect a configuration from changes:

```cpp
template <typename CONF, typename TYPE>
result_t markVersionReadonly(CONF configuration,
                             std::string const& version,
                             std::string const& entity) const noexcept;
```

**Example:**

```cpp
db::ConfigurationInterface ifc;
MyConfig config;

// Mark production configuration as read-only
auto result = ifc.markVersionReadonly<MyConfig const*, JsonData>(
    &config,
    "v2.0_production",
    "DAQComponent1"
);

if (result.first) {
    std::cout << "Configuration is now protected\n";
}
```

After marking read-only, attempts to overwrite will fail with an error.

---

## 7. Working with Global Configurations

### 7.1 Understanding Global Configurations

A global configuration (also called a "composition") is a named group of configuration versions. It represents a complete system state for a specific run or experiment.

**Example Structure:**
```
"Run12345" (Global Configuration)
├── TriggerConfig v2.0 / Trigger1
├── BoardReaderConfig v1.5 / BoardReader_01
├── BoardReaderConfig v1.5 / BoardReader_02
└── EventBuilderConfig v3.1 / EventBuilder_01
```

### 7.2 Finding Global Configurations

Use `findGlobalConfigurations` to search for existing compositions:

```cpp
std::set<std::string> findGlobalConfigurations(std::string const& search) const;
```

**Parameters:**
- `search` - Search pattern. Empty string `""` returns all configurations. The search string is passed directly to the storage provider.

**Note:** This method **THROWS exceptions** on error.

**Example:**

```cpp
db::ConfigurationInterface ifc;

try {
    // Find all global configurations (empty string = all)
    auto allConfigs = ifc.findGlobalConfigurations("");

    std::cout << "All global configurations:\n";
    for (const auto& name : allConfigs) {
        std::cout << "  - " << name << "\n";
    }

    // Find configurations with a specific search term
    // Note: The exact matching behavior depends on the storage provider
    auto runConfigs = ifc.findGlobalConfigurations("Run");

    std::cout << "\nConfigurations matching 'Run':\n";
    for (const auto& name : runConfigs) {
        std::cout << "  - " << name << "\n";
    }
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
}
```

### 7.3 Loading a Global Configuration

Use `loadGlobalConfiguration` to get the composition members:

```cpp
VersionInfoList_t loadGlobalConfiguration(std::string const& configuration) const;
```

**Returns:** A `std::list<VersionInfo>` containing all members of the composition.

**Throws:**
- `invalid_option_exception` if configuration name is empty
- `runtime_exception` on database errors

**Example:**

```cpp
db::ConfigurationInterface ifc;

try {
    auto members = ifc.loadGlobalConfiguration("Run12345");

    std::cout << "Run12345 composition:\n";
    for (const auto& info : members) {
        std::cout << "  Collection: " << info.configuration << "\n";
        std::cout << "  Version:    " << info.version << "\n";
        std::cout << "  Entity:     " << info.entity << "\n";
        std::cout << "  ---\n";
    }
} catch (const artdaq::database::invalid_option_exception& e) {
    std::cerr << "Invalid parameter: " << e.what() << "\n";
} catch (const artdaq::database::runtime_exception& e) {
    std::cerr << "Database error: " << e.what() << "\n";
}
```

### 7.4 Storing a Global Configuration

Use `storeGlobalConfiguration` to create or update a composition:

```cpp
result_t storeGlobalConfiguration(
    VersionInfoList_t const& versionInfoList,
    std::string const& configuration,
    bool allowOverwrite = false) const;
```

**Parameters:**
- `versionInfoList` - List of `VersionInfo` structs (must not be empty)
- `configuration` - Name for the global configuration
- `allowOverwrite` - If `false` (default), fails if composition exists. If `true`, performs a merge update.

**Important - Merge Behavior When `allowOverwrite=true`:**

When updating an existing composition, the method performs an intelligent merge:

1. **Computes a diff** between old and new members using `(configuration, entity)` as the key
2. **Unassigns** members that are no longer in the new list OR have a different version
3. **Assigns only new/changed** members (skips unchanged ones for efficiency)
4. Members with the same `(configuration, entity)` but same version are **preserved** (no operation)

```
Old Composition:           New Composition:
- ConfigA v1.0 Entity1     - ConfigA v1.0 Entity1  <- UNCHANGED (skip)
- ConfigB v2.0 Entity2     - ConfigB v3.0 Entity2  <- VERSION CHANGED (unassign + assign)
- ConfigC v1.0 Entity3                             <- REMOVED (unassign)
                           - ConfigD v1.0 Entity4  <- NEW (assign)
```

**Example - Creating a new composition:**

```cpp
db::ConfigurationInterface ifc;

// Build the composition
db::ConfigurationInterface::VersionInfoList_t composition;

// Add trigger configuration
composition.push_back({
    "TriggerConfig",      // configuration type
    "v2.0",               // version
    "Trigger1"            // entity (REQUIRED for VersionInfo)
});

// Add board reader configurations
composition.push_back({"BoardReaderConfig", "v1.5", "BoardReader_01"});
composition.push_back({"BoardReaderConfig", "v1.5", "BoardReader_02"});

// Add event builder
composition.push_back({"EventBuilderConfig", "v3.1", "EventBuilder_01"});

// Store the composition (default: allowOverwrite=false)
auto result = ifc.storeGlobalConfiguration(composition, "Run12345");

if (result.first) {
    std::cout << "Global configuration 'Run12345' created!\n";
} else {
    std::cerr << "Failed: " << result.second << "\n";
}
```

**Example - Updating an existing composition:**

```cpp
db::ConfigurationInterface ifc;

// Load existing composition
auto existingMembers = ifc.loadGlobalConfiguration("Run12345");

// Convert to modifiable list and make changes
db::ConfigurationInterface::VersionInfoList_t updatedComposition;
for (const auto& member : existingMembers) {
    if (member.configuration == "TriggerConfig") {
        // Update trigger version
        updatedComposition.push_back({"TriggerConfig", "v2.1", member.entity});
    } else {
        // Keep other members unchanged
        updatedComposition.push_back(member);
    }
}

// Add a new board reader
updatedComposition.push_back({"BoardReaderConfig", "v1.5", "BoardReader_03"});

// Update with allowOverwrite=true
auto result = ifc.storeGlobalConfiguration(
    updatedComposition,
    "Run12345",
    true  // allowOverwrite - enables merge behavior
);

if (result.first) {
    std::cout << "Global configuration updated!\n";
}
```

**Common Errors:**
- `"Version info list is empty"` - Cannot create empty composition
- `"Global configuration name is empty"` - Name is required
- `"Global configuration 'X' already exists"` - Use `allowOverwrite=true`
- `"Configuration name is empty"` / `"Version is empty"` / `"Entity is empty"` - From `VersionInfo::validate()`

### 7.5 Multi-threaded Storage

For compositions with many members, use `storeGlobalConfiguration_mt`:

```cpp
result_t storeGlobalConfiguration_mt(
    VersionInfoList_t const& versionInfoList,
    std::string const& configuration,
    bool overwrite = false) const;
```

**How it works:**
1. Checks if composition exists and loads existing members if updating
2. Computes diff (same merge logic as single-threaded version)
3. Creates a `Multitasker` thread pool
4. Executes unassignments in parallel (if any)
5. Executes assignments in parallel
6. Collects and merges results

**Thread Pool Size:** `min(number_of_operations, hardware_concurrency / 2)`

**Example:**

```cpp
db::ConfigurationInterface ifc;

// Large composition with 100+ members
db::ConfigurationInterface::VersionInfoList_t largeComposition;
for (int i = 0; i < 100; ++i) {
    largeComposition.push_back({
        "BoardReaderConfig",
        "v1.0",
        "BoardReader_" + std::to_string(i)
    });
}

// Use multi-threaded version for better performance
auto result = ifc.storeGlobalConfiguration_mt(
    largeComposition,
    "LargeRun",
    false
);

if (result.first) {
    std::cout << "Large composition stored efficiently!\n";
}
```

**When to use `_mt` version:**
- Compositions with more than 10 members
- When performance is critical
- When database operations are slow (network latency to MongoDB/UconDB)
- NOT needed for FileSystemDB with local SSD (single-threaded may be faster)

### 7.6 Finding Compositions Containing a Specific Version

Use `findGlobalConfigurationsContaining` to find which compositions use a specific configuration:

```cpp
std::set<std::string> findGlobalConfigurationsContaining(
    std::string const& configurationType,
    std::string const& version,
    std::string const& entity = "") const;
```

**Parameters:**
- `configurationType` - Collection name (e.g., "TriggerConfig") - **REQUIRED**
- `version` - Version string - **REQUIRED**
- `entity` - Entity name (optional - if empty, matches any entity)

**Implementation Details:**

The method tries two approaches:
1. **Optimized path**: Uses storage provider's native search if available
2. **Fallback brute-force**: Loads all compositions and checks each one

**Example - Impact Analysis:**

```cpp
db::ConfigurationInterface ifc;

try {
    // Before modifying TriggerConfig v2.0, find all runs using it
    auto affectedRuns = ifc.findGlobalConfigurationsContaining(
        "TriggerConfig",  // configuration type
        "v2.0"            // version (entity is optional)
    );

    if (!affectedRuns.empty()) {
        std::cout << "WARNING: The following runs use TriggerConfig v2.0:\n";
        for (const auto& run : affectedRuns) {
            std::cout << "  - " << run << "\n";
        }
        std::cout << "Modifying this configuration may affect these runs.\n";
    } else {
        std::cout << "No runs use TriggerConfig v2.0\n";
    }

    // With specific entity
    auto specificRuns = ifc.findGlobalConfigurationsContaining(
        "BoardReaderConfig",
        "v1.5",
        "BoardReader_01"  // Only compositions with this specific entity
    );

} catch (const artdaq::database::invalid_option_exception& e) {
    std::cerr << "Invalid parameter: " << e.what() << "\n";
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
}
```

### 7.7 Listing Collections

Use `listCollections` to see all configuration types:

```cpp
std::set<std::string> listCollections(std::string const& name_prefix) const;
```

**Parameters:**
- `name_prefix` - Filter by prefix (empty string returns all)

**Example:**

```cpp
db::ConfigurationInterface ifc;

try {
    // List all collections
    auto allCollections = ifc.listCollections("");

    std::cout << "All configuration types:\n";
    for (const auto& col : allCollections) {
        std::cout << "  - " << col << "\n";
    }

    // List only Board-related collections
    auto boardCollections = ifc.listCollections("Board");

    std::cout << "\nBoard-related types:\n";
    for (const auto& col : boardCollections) {
        std::cout << "  - " << col << "\n";
    }
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
}
```

---

## 8. Making Your Configuration Serializable

To use `ConfigurationInterface`, your configuration class must be serializable. This section explains how.

### 8.1 The MakeSerializable Template

The `MakeSerializable` template is defined in `configurationdbifc_base.h`:

```cpp
template <typename CONF>
class MakeSerializable final {
    // CONF must be a pointer or reference type
    static_assert(std::is_pointer<CONF>::value || std::is_reference<CONF>::value,
                  "Template parameter must be either a pointer or reference type");

public:
    // Write configuration to data format (for storing)
    template <class TYPE>
    bool writeDocument(TYPE& data) const;

    // Read configuration from data format (for loading)
    template <class TYPE>
    bool readDocument(TYPE const& data);

    // Get the collection/configuration name
    std::string configurationName() const;

    // Constructor
    MakeSerializable(CONF conf) : _conf(conf) {}

private:
    // These must be specialized for your type:
    template <class TYPE>
    bool writeDocumentImpl(TYPE&) const;

    template <class TYPE>
    bool readDocumentImpl(TYPE const&);

    std::string configurationNameImpl() const;

    CONF _conf;  // Your configuration pointer/reference
};
```

### 8.2 Required Specializations

You need to specialize three methods for your configuration class:

```cpp
namespace artdaq {
namespace database {
namespace configuration {

// 1. For WRITING (storing to database) - use const pointer
template <>
template <>
bool MakeSerializable<YourConfig const*>::writeDocumentImpl<JsonData>(JsonData& data) const;

template <>
std::string MakeSerializable<YourConfig const*>::configurationNameImpl() const;

// 2. For READING (loading from database) - use non-const pointer
template <>
template <>
bool MakeSerializable<YourConfig*>::readDocumentImpl<JsonData>(JsonData const& data);

template <>
std::string MakeSerializable<YourConfig*>::configurationNameImpl() const;

}  // namespace configuration
}  // namespace database
}  // namespace artdaq
```

### 8.3 Complete Example (Realistic Pattern)

This example follows the actual patterns used in the artdaq-database codebase (specifically the OtsDaq integration pattern).

**Step 1: Define your configuration class with a View pattern:**

This pattern is commonly used in the codebase where the configuration has a "view" that handles serialization.

```cpp
// MyConfig.h
#ifndef MY_CONFIG_H
#define MY_CONFIG_H

#include <string>
#include <sstream>

// ConfigurationView handles the actual data and serialization
struct MyConfigView {
    // Configuration parameters
    int runNumber = 0;
    double threshold = 0.0;
    std::string description;
    int version = 1;

    // Serialize to JSON (write to stringstream)
    void printJSON(std::stringstream& ss) const {
        ss << "{\n";
        ss << "  \"runNumber\": " << runNumber << ",\n";
        ss << "  \"threshold\": " << threshold << ",\n";
        ss << "  \"description\": \"" << description << "\"\n";
        ss << "}";
    }

    // Deserialize from JSON string (returns 0 on success, negative on error)
    int fillFromJSON(std::string const& json) {
        // Simple parsing (in production, use a proper JSON parser)
        // This is simplified for illustration - actual implementation
        // would use boost::property_tree or similar
        if (json.find("runNumber") == std::string::npos) return -1;
        // ... parse fields from JSON string ...
        return 0;  // Success
    }
};

// ConfigurationBase - the configuration interface pattern
class MyConfig {
public:
    virtual ~MyConfig() = default;

    // Get the configuration type name (becomes the database collection name)
    virtual std::string getConfigurationName() const { return "MyConfig"; }

    // Access to the view (mutable and const)
    MyConfigView& getView() { return _view; }
    MyConfigView const& getView() const { return _view; }
    MyConfigView* getViewP() { return &_view; }

    // Get version from the view
    int getViewVersion() const { return _view.version; }

private:
    MyConfigView _view;
};

#endif
```

**Step 2: Implement MakeSerializable specializations:**

This is the **critical step** that connects your configuration to the database. This code follows the exact pattern used in the artdaq-database test suite.

```cpp
// MyConfigSerializable.cpp
#include "MyConfig.h"
#include "artdaq-database/ConfigurationDB/configurationdbifc.h"
#include "artdaq-database/BasicTypes/basictypes.h"
#include <sstream>

using artdaq::database::basictypes::JsonData;

namespace artdaq {
namespace database {
namespace configuration {

//==========================================================================
// For WRITING (saving to database) - const pointer
// This is called when you use storeVersion() or overwriteVersion()
//==========================================================================

template <>
template <>
bool MakeSerializable<MyConfig const*>::writeDocumentImpl<JsonData>(JsonData& data) const {
    // Use stringstream to capture the JSON output
    std::stringstream ss;

    // Call your configuration's serialization method
    _conf->getView().printJSON(ss);

    // Store the result in the JsonData buffer
    data.json_buffer = ss.str();

    return true;  // Return true on success
}

template <>
std::string MakeSerializable<MyConfig const*>::configurationNameImpl() const {
    // This determines the database collection name
    return _conf->getConfigurationName();
}

//==========================================================================
// For READING (loading from database) - non-const pointer
// This is called when you use loadVersion()
//==========================================================================

template <>
template <>
bool MakeSerializable<MyConfig*>::readDocumentImpl<JsonData>(JsonData const& data) {
    // data.json_buffer contains the JSON string retrieved from the database
    // Pass it to your configuration's deserialization method
    int retVal = _conf->getViewP()->fillFromJSON(data.json_buffer);

    // Return true if deserialization succeeded (retVal >= 0)
    return (retVal >= 0);
}

template <>
std::string MakeSerializable<MyConfig*>::configurationNameImpl() const {
    // This determines the database collection name
    return _conf->getConfigurationName();
}

}  // namespace configuration
}  // namespace database
}  // namespace artdaq
```

**Step 3: Use with ConfigurationInterface:**

```cpp
#include "MyConfig.h"
#include "artdaq-database/ConfigurationDB/configurationdbifc.h"
#include "artdaq-database/BasicTypes/basictypes.h"

namespace db = artdaq::database::configuration;
using artdaq::database::basictypes::JsonData;

void saveMyConfig() {
    db::ConfigurationInterface ifc;

    MyConfig config;
    config.getView().runNumber = 12345;
    config.getView().threshold = 3.14;
    config.getView().description = "Test run";
    config.getView().version = 1;

    // NOTE: Use const pointer for storing (MyConfig const*)
    auto result = ifc.storeVersion<MyConfig const*, JsonData>(
        &config,
        "1",           // Version string (often just the integer as string)
        "TestEntity"
    );

    if (result.first) {
        std::cout << "Saved!\n";
    } else {
        std::cerr << "Error: " << result.second << "\n";
    }
}

void loadMyConfig() {
    db::ConfigurationInterface ifc;

    MyConfig config;

    // NOTE: Use non-const pointer for loading (MyConfig*)
    auto result = ifc.loadVersion<MyConfig*, JsonData>(
        &config,
        "1",
        "TestEntity"
    );

    if (result.first) {
        std::cout << "Loaded run " << config.getView().runNumber << "\n";
    } else {
        std::cerr << "Error: " << result.second << "\n";
    }
}
```

### 8.4 Alternative: Simple Configuration Pattern

For simpler configurations that don't use the View pattern, you can implement a more straightforward approach:

```cpp
// SimpleConfig.h
class SimpleConfig {
public:
    int value = 0;
    std::string name;

    std::string getConfigurationName() const { return "SimpleConfig"; }

    // Direct serialization methods
    std::string toJson() const {
        return "{\"value\": " + std::to_string(value) +
               ", \"name\": \"" + name + "\"}";
    }

    bool fromJson(std::string const& json) {
        // Parse json and populate fields
        // (use boost::property_tree or similar in real code)
        return true;
    }
};

// SimpleConfigSerializable.cpp
namespace artdaq::database::configuration {

template <>
template <>
bool MakeSerializable<SimpleConfig const*>::writeDocumentImpl<JsonData>(JsonData& data) const {
    data.json_buffer = _conf->toJson();
    return true;
}

template <>
std::string MakeSerializable<SimpleConfig const*>::configurationNameImpl() const {
    return _conf->getConfigurationName();
}

template <>
template <>
bool MakeSerializable<SimpleConfig*>::readDocumentImpl<JsonData>(JsonData const& data) {
    return _conf->fromJson(data.json_buffer);
}

template <>
std::string MakeSerializable<SimpleConfig*>::configurationNameImpl() const {
    return _conf->getConfigurationName();
}

} // namespace
```

### 8.5 Key Points for Serialization

1. **Use const pointer for writing**: When storing, the configuration shouldn't be modified, so use `MyConfig const*`

2. **Use non-const pointer for reading**: When loading, the configuration will be populated, so use `MyConfig*`

3. **Return true/false**: The `readDocumentImpl` and `writeDocumentImpl` methods should return `true` on success, `false` on failure

4. **Configuration name determines collection**: The string returned by `configurationNameImpl()` becomes the database collection name

5. **Both pointer types need specializations**: You must specialize for BOTH `MyConfig*` AND `MyConfig const*`

6. **Fallback behavior**: If `configurationNameImpl()` throws, the system falls back to using the demangled type name

---

## 9. Error Handling and Exception Types

### 9.1 Understanding result_t

Most methods return `result_t`:

```cpp
using result_t = std::pair<bool, std::string>;
```

- `result.first` - `true` = success, `false` = failure
- `result.second` - Message describing the result or error

### 9.2 Exception Types

The library defines several exception types in `artdaq-database/SharedCommon/shared_exceptions.h`:

```cpp
namespace artdaq::database {

// Base exception class (inherits from cet::exception)
class exception : public cet::exception {
public:
    explicit exception(Category const& category);
    exception(Category const& category, std::string const& message);
};

// Invalid argument provided
class invalid_argument : public exception {
public:
    explicit invalid_argument(std::string const& category_);
};

// Runtime error during operation
class runtime_error : public exception {
public:
    explicit runtime_error(std::string const& category_);
};

// Invalid option/parameter provided
class invalid_option_exception : public exception {
public:
    explicit invalid_option_exception(Category const& category);
    invalid_option_exception(Category const& category, std::string const& message);
};

// General runtime exception
class runtime_exception : public exception {
public:
    explicit runtime_exception(Category const& category);
    runtime_exception(Category const& category, std::string const& message);
};

} // namespace
```

### 9.3 Exception vs result_t Methods

| Method Type | Behavior |
|------------|----------|
| `storeVersion`, `loadVersion`, `overwriteVersion`, `markVersionReadonly` | Returns `result_t`, catches exceptions internally |
| `getVersions`, `listCollections`, `findGlobalConfigurations`, `loadGlobalConfiguration`, `findGlobalConfigurationsContaining` | **THROWS exceptions** |
| `*_safe` variants | Returns `result_t`, never throws |

### 9.4 Checking Results

**Always check the result before proceeding:**

```cpp
auto result = ifc.storeVersion<MyConfig const*, JsonData>(&config, version, entity);

// Pattern 1: Simple check
if (!result.first) {
    std::cerr << "Error: " << result.second << "\n";
    return;
}

// Pattern 2: Structured binding (C++17) - RECOMMENDED
auto [success, message] = ifc.storeVersion<MyConfig const*, JsonData>(&config, version, entity);
if (!success) {
    std::cerr << "Error: " << message << "\n";
    return;
}

// Pattern 3: With logging
if (!result.first) {
    TLOG(TLVL_ERROR) << "Database operation failed: " << result.second;
    throw std::runtime_error(result.second);
}
```

**Important - Error Message Format Varies by Method:**

As documented in Section 2.7, different methods return different error formats:

```cpp
// storeVersion, loadVersion, overwriteVersion, markVersionReadonly
// Return RAW exception messages:
auto [success, message] = ifc.storeVersion<MyConfig const*, JsonData>(&config, "v1", "Entity");
// On error: message = "ConfigurationInterface::storeVersion: Version is empty"

// storeGlobalConfiguration and *_safe methods
// Return {error:"..."} format:
auto [success, message] = ifc.storeGlobalConfiguration(composition, "Run1");
// On error: message = {error:"Version info list is empty"}
```

**Best Practice - Don't Parse Error Messages:**

```cpp
// GOOD: Just log or display the message as-is
if (!result.first) {
    TLOG(TLVL_ERROR) << "Operation failed: " << result.second;
}

// BAD: Don't try to parse the error format - it varies!
if (!result.first) {
    // This assumes {error:"..."} format, but storeVersion returns raw messages!
    auto errorJson = parseJson(result.second);  // May fail!
}
```

### 9.5 Exception Handling for Throwing Methods

```cpp
try {
    auto versions = ifc.getVersions(&config, "Entity1");
    // Process versions...
} catch (const artdaq::database::invalid_option_exception& e) {
    // Invalid parameter provided (e.g., empty required string)
    std::cerr << "Invalid parameter: " << e.what() << "\n";
} catch (const artdaq::database::runtime_exception& e) {
    // Database operation failed
    std::cerr << "Database error: " << e.what() << "\n";
} catch (const artdaq::database::exception& e) {
    // Other artdaq-database exceptions
    std::cerr << "artdaq-database error: " << e.what() << "\n";
} catch (const cet::exception& e) {
    // cetlib exceptions (base class)
    std::cerr << "cetlib error: " << e.what() << "\n";
} catch (const std::exception& e) {
    // Standard exceptions
    std::cerr << "Unexpected error: " << e.what() << "\n";
}
```

### 9.6 Common Error Messages and Solutions

| Error Message | Cause | Solution |
|--------------|-------|----------|
| `"Version is empty"` | Empty version string | Provide a non-empty version |
| `"Configuration name is empty"` | `configurationNameImpl()` returned empty | Fix your MakeSerializable specialization |
| `"Entity is empty"` | Empty entity in `VersionInfo::validate()` | Provide entity name in VersionInfo |
| `"Document not found"` | Version/entity doesn't exist | Check version exists with `getVersions` first |
| `"Document already exists"` | Trying to store existing version | Use `overwriteVersion` or new version |
| `"Global configuration already exists"` | Composition name taken | Use `allowOverwrite=true` or different name |
| `"Version info list is empty"` | Empty composition | Add at least one member |
| `"Configuration type is empty"` | Empty config type in `findGlobalConfigurationsContaining` | Provide configuration type |

---

## 10. Safe vs Throwing Methods

### 10.1 Overview

The API provides two versions of data-returning methods:

| Throwing Method | Safe Method |
|-----------------|-------------|
| `getVersions()` | `getVersions_safe()` |
| `findGlobalConfigurations()` | `findGlobalConfigurations_safe()` |
| `loadGlobalConfiguration()` | `loadGlobalConfiguration_safe()` |
| `listCollections()` | `listCollections_safe()` |
| `findGlobalConfigurationsContaining()` | `findGlobalConfigurationsContaining_safe()` |

### 10.2 Method Signatures

**Safe methods take an output reference parameter:**

```cpp
// Throwing version (returns data, throws on error)
template <typename CONF>
std::list<std::string> getVersions(CONF configuration,
                                   std::string const& entity) const;

// Safe version (returns result_t, output via reference parameter)
template <typename CONF>
cf::result_t getVersions_safe(CONF configuration,
                              std::string const& entity,
                              std::list<std::string>& versions) const noexcept;
```

**All Safe Method Signatures:**

```cpp
// getVersions_safe
template <typename CONF>
cf::result_t getVersions_safe(CONF configuration,
                              std::string const& entity,
                              std::list<std::string>& versions) const noexcept;

// findGlobalConfigurations_safe
cf::result_t findGlobalConfigurations_safe(std::string const& search,
                                           std::set<std::string>& configurations) const noexcept;

// loadGlobalConfiguration_safe
cf::result_t loadGlobalConfiguration_safe(std::string const& configuration,
                                          VersionInfoList_t& members) const noexcept;

// listCollections_safe
cf::result_t listCollections_safe(std::string const& name_prefix,
                                  std::set<std::string>& collections) const noexcept;

// findGlobalConfigurationsContaining_safe
// NOTE: entity parameter is REQUIRED (but can be empty string to match any entity)
cf::result_t findGlobalConfigurationsContaining_safe(
    std::string const& configurationType,
    std::string const& version,
    std::string const& entity,           // Required! Use "" for "any entity"
    std::set<std::string>& compositions) const noexcept;
```

### 10.3 When to Use Each

**Use throwing methods when:**
- You want exceptions to propagate up the call stack
- You're in code that already uses try/catch
- Error handling is centralized
- The caller can handle exceptions

**Use safe methods when:**
- You need exception-free code (e.g., destructors, noexcept functions)
- You're writing C-compatible interfaces
- You prefer explicit error checking
- You're in a context where exceptions aren't allowed

### 10.4 Safe Method Examples

```cpp
db::ConfigurationInterface ifc;
MyConfig config;

// Using getVersions_safe
std::list<std::string> versions;
auto result = ifc.getVersions_safe(&config, "Entity1", versions);

if (result.first) {
    for (const auto& v : versions) {
        std::cout << "Version: " << v << "\n";
    }
} else {
    std::cerr << "Failed: " << result.second << "\n";
}

// Using findGlobalConfigurations_safe
std::set<std::string> configurations;
result = ifc.findGlobalConfigurations_safe("Run", configurations);

if (result.first) {
    for (const auto& c : configurations) {
        std::cout << "Config: " << c << "\n";
    }
}

// Using loadGlobalConfiguration_safe
db::ConfigurationInterface::VersionInfoList_t members;
result = ifc.loadGlobalConfiguration_safe("Run12345", members);

if (result.first) {
    for (const auto& m : members) {
        std::cout << m.configuration << " / " << m.version << "\n";
    }
}

// Using listCollections_safe
std::set<std::string> collections;
result = ifc.listCollections_safe("", collections);

if (result.first) {
    std::cout << "Found " << collections.size() << " collections\n";
}

// Using findGlobalConfigurationsContaining_safe
// NOTE: This method requires 4 parameters - entity is REQUIRED (but can be empty string)
std::set<std::string> compositions;
result = ifc.findGlobalConfigurationsContaining_safe(
    "TriggerConfig",   // configuration type (required, non-empty)
    "v2.0",            // version (required, non-empty)
    "",                // entity (required parameter, empty string = match any entity)
    compositions       // output parameter
);

if (result.first) {
    std::cout << "Found in " << compositions.size() << " compositions\n";
}

// With specific entity
result = ifc.findGlobalConfigurationsContaining_safe(
    "BoardReaderConfig",
    "v1.5",
    "BoardReader_01",  // Only find compositions with this specific entity
    compositions
);
```

---

## 11. Multi-threaded Operations

### 11.1 Thread Safety Rules

**Important:** `ConfigurationInterface` instances are NOT thread-safe.

**Rule 1:** Create one instance per thread

```cpp
// CORRECT: Each thread has its own instance
void workerThread() {
    db::ConfigurationInterface ifc;  // Local instance
    // Use ifc...
}

std::thread t1(workerThread);
std::thread t2(workerThread);
```

```cpp
// INCORRECT: Sharing across threads
db::ConfigurationInterface ifc;  // Shared instance

std::thread t1([&ifc]() {
    ifc.storeVersion(...);  // UNSAFE!
});
std::thread t2([&ifc]() {
    ifc.loadVersion(...);   // UNSAFE!
});
```

### 11.2 Using storeGlobalConfiguration_mt

The `_mt` version uses a thread pool internally:

```cpp
// This is safe - creates its own thread pool internally
auto result = ifc.storeGlobalConfiguration_mt(composition, "Run12345");
```

**How it works internally:**
1. Checks if composition exists (if updating)
2. Loads existing members and computes diff (if updating)
3. Creates a `Multitasker` thread pool
4. Processes unassignments in parallel (if any)
5. Processes assignments in parallel
6. Waits for all tasks to complete
7. Merges results and returns combined status

**Thread pool sizing:**
```cpp
auto threadCount = std::min<std::size_t>(
    members_to_assign.size(),
    std::thread::hardware_concurrency() / 2
);
```

### 11.3 Parallel Operations from Multiple Threads

When you need truly parallel database operations:

```cpp
#include <thread>
#include <vector>
#include <mutex>

void parallelVersionLoad() {
    std::vector<std::string> versionsToLoad = {"v1.0", "v2.0", "v3.0"};
    std::vector<MyConfig> configs(versionsToLoad.size());
    std::mutex resultMutex;
    std::vector<bool> results(versionsToLoad.size());

    std::vector<std::thread> threads;

    for (size_t i = 0; i < versionsToLoad.size(); ++i) {
        threads.emplace_back([&, i]() {
            // Each thread creates its own interface
            db::ConfigurationInterface ifc;

            auto result = ifc.loadVersion<MyConfig*, JsonData>(
                &configs[i],
                versionsToLoad[i],
                "Entity1"
            );

            std::lock_guard<std::mutex> lock(resultMutex);
            results[i] = result.first;
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // Check results
    for (size_t i = 0; i < results.size(); ++i) {
        if (results[i]) {
            std::cout << "Loaded " << versionsToLoad[i] << " successfully\n";
        }
    }
}
```

---

## 12. Complete Working Examples

### 12.1 Example: Configuration Version Lifecycle

```cpp
#include "artdaq-database/ConfigurationDB/configurationdbifc.h"
#include "artdaq-database/BasicTypes/basictypes.h"
#include <iostream>

namespace db = artdaq::database::configuration;
using artdaq::database::basictypes::JsonData;

// Assume MyConfig is your serializable configuration class

void configurationLifecycleExample() {
    db::ConfigurationInterface ifc;

    //=========================================
    // STEP 1: Create and store initial version
    //=========================================
    MyConfig config;
    config.runNumber = 1000;
    config.threshold = 1.5;

    auto result = ifc.storeVersion<MyConfig const*, JsonData>(
        &config, "v1.0", "TestComponent"
    );

    if (!result.first) {
        std::cerr << "Failed to store v1.0: " << result.second << "\n";
        return;
    }
    std::cout << "Stored v1.0\n";

    //=========================================
    // STEP 2: Store a second version
    //=========================================
    config.runNumber = 1001;
    config.threshold = 2.0;

    result = ifc.storeVersion<MyConfig const*, JsonData>(
        &config, "v1.1", "TestComponent"
    );

    if (!result.first) {
        std::cerr << "Failed to store v1.1: " << result.second << "\n";
        return;
    }
    std::cout << "Stored v1.1\n";

    //=========================================
    // STEP 3: List all versions
    //=========================================
    try {
        auto versions = ifc.getVersions(&config, "TestComponent");
        std::cout << "Available versions: ";
        for (const auto& v : versions) {
            std::cout << v << " ";
        }
        std::cout << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error listing versions: " << e.what() << "\n";
    }

    //=========================================
    // STEP 4: Load a specific version
    //=========================================
    MyConfig loadedConfig;
    result = ifc.loadVersion<MyConfig*, JsonData>(
        &loadedConfig, "v1.0", "TestComponent"
    );

    if (result.first) {
        std::cout << "Loaded v1.0: runNumber=" << loadedConfig.runNumber << "\n";
    }

    //=========================================
    // STEP 5: Update an existing version
    //=========================================
    loadedConfig.threshold = 1.75;  // Modify loaded config

    result = ifc.overwriteVersion<MyConfig const*, JsonData>(
        &loadedConfig, "v1.0", "TestComponent"
    );

    if (result.first) {
        std::cout << "Updated v1.0\n";
    }

    //=========================================
    // STEP 6: Mark production version read-only
    //=========================================
    result = ifc.markVersionReadonly<MyConfig const*, JsonData>(
        &config, "v1.1", "TestComponent"
    );

    if (result.first) {
        std::cout << "v1.1 is now read-only\n";
    }
}
```

### 12.2 Example: Global Configuration Management

```cpp
#include "artdaq-database/ConfigurationDB/configurationdbifc.h"
#include "artdaq-database/BasicTypes/basictypes.h"
#include <iostream>
#include <iomanip>

namespace db = artdaq::database::configuration;

void globalConfigurationExample() {
    db::ConfigurationInterface ifc;

    //=========================================
    // STEP 1: Create a global configuration
    //=========================================
    db::ConfigurationInterface::VersionInfoList_t composition;

    // Add system components
    composition.push_back({"TriggerConfig", "v2.0", "Trigger_Main"});
    composition.push_back({"BoardReaderConfig", "v1.5", "BoardReader_01"});
    composition.push_back({"BoardReaderConfig", "v1.5", "BoardReader_02"});
    composition.push_back({"EventBuilderConfig", "v3.0", "EventBuilder_01"});
    composition.push_back({"DataLoggerConfig", "v1.0", "DataLogger_Main"});

    auto result = ifc.storeGlobalConfiguration(composition, "Run_2025_001");

    if (!result.first) {
        std::cerr << "Failed to create composition: " << result.second << "\n";
        return;
    }
    std::cout << "Created global configuration 'Run_2025_001'\n\n";

    //=========================================
    // STEP 2: List all global configurations
    //=========================================
    try {
        auto allConfigs = ifc.findGlobalConfigurations("");

        std::cout << "All global configurations:\n";
        for (const auto& name : allConfigs) {
            std::cout << "  - " << name << "\n";
        }
        std::cout << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    //=========================================
    // STEP 3: Load and display composition
    //=========================================
    try {
        auto members = ifc.loadGlobalConfiguration("Run_2025_001");

        std::cout << "Composition of 'Run_2025_001':\n";
        std::cout << std::setw(25) << "Type"
                  << std::setw(15) << "Version"
                  << std::setw(20) << "Entity" << "\n";
        std::cout << std::string(60, '-') << "\n";

        for (const auto& m : members) {
            std::cout << std::setw(25) << m.configuration
                      << std::setw(15) << m.version
                      << std::setw(20) << m.entity << "\n";
        }
        std::cout << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    //=========================================
    // STEP 4: Update the composition (merge behavior)
    //=========================================
    composition.clear();
    composition.push_back({"TriggerConfig", "v2.1", "Trigger_Main"});      // Version changed!
    composition.push_back({"BoardReaderConfig", "v1.5", "BoardReader_01"}); // Unchanged
    composition.push_back({"BoardReaderConfig", "v1.5", "BoardReader_02"}); // Unchanged
    composition.push_back({"BoardReaderConfig", "v1.5", "BoardReader_03"}); // New!
    composition.push_back({"EventBuilderConfig", "v3.0", "EventBuilder_01"}); // Unchanged
    composition.push_back({"DataLoggerConfig", "v1.0", "DataLogger_Main"});   // Unchanged
    // Note: We didn't include DataLoggerConfig - it will be removed!

    // Wait, let's keep DataLoggerConfig and show proper update:
    result = ifc.storeGlobalConfiguration(composition, "Run_2025_001", true);  // allowOverwrite

    if (result.first) {
        std::cout << "Updated 'Run_2025_001':\n";
        std::cout << "  - TriggerConfig: v2.0 -> v2.1 (updated)\n";
        std::cout << "  - BoardReader_03: added\n";
        std::cout << "  - BoardReader_01, _02, EventBuilder, DataLogger: unchanged\n\n";
    }

    //=========================================
    // STEP 5: Find which compositions use a specific config
    //=========================================
    try {
        auto using_trigger = ifc.findGlobalConfigurationsContaining(
            "TriggerConfig", "v2.1"
        );

        std::cout << "Compositions using TriggerConfig v2.1:\n";
        for (const auto& name : using_trigger) {
            std::cout << "  - " << name << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}
```

### 12.3 Example: Wrapper Class Pattern

This pattern wraps `ConfigurationInterface` for your specific use case:

```cpp
#include "artdaq-database/ConfigurationDB/configurationdbifc.h"
#include "artdaq-database/BasicTypes/basictypes.h"

namespace db = artdaq::database::configuration;
using artdaq::database::basictypes::JsonData;

class MyDatabaseInterface {
public:
    using result_t = std::pair<bool, std::string>;
    using config_version_map_t = std::map<std::string, int>;

    MyDatabaseInterface() = default;

    // Load a configuration by version number
    int loadConfiguration(MyConfig* config, int version) const noexcept {
        db::ConfigurationInterface ifc;

        std::string versionStr = (version == -1)
            ? std::to_string(findLatestVersion(config))
            : std::to_string(version);

        auto result = ifc.loadVersion<MyConfig*, JsonData>(
            config, versionStr, m_defaultEntity
        );

        return result.first ? 0 : -1;
    }

    // Save a configuration
    int saveConfiguration(const MyConfig* config, bool overwrite = false) const noexcept {
        db::ConfigurationInterface ifc;

        std::string versionStr = std::to_string(config->getVersion());

        auto result = overwrite
            ? ifc.overwriteVersion<const MyConfig*, JsonData>(config, versionStr, m_defaultEntity)
            : ifc.storeVersion<const MyConfig*, JsonData>(config, versionStr, m_defaultEntity);

        return result.first ? 0 : -1;
    }

    // Get all versions as integers
    std::set<int> getVersions(const MyConfig* config) const noexcept {
        db::ConfigurationInterface ifc;
        std::set<int> result;

        try {
            auto versions = ifc.getVersions(config, m_defaultEntity);
            for (const auto& v : versions) {
                result.insert(std::stoi(v));
            }
        } catch (...) {
            // Return empty set on error
        }

        return result;
    }

    // Find latest version
    int findLatestVersion(const MyConfig* config) const noexcept {
        auto versions = getVersions(config);
        if (versions.empty()) return -1;
        return *versions.rbegin();
    }

    // Load specific run configuration
    config_version_map_t loadRunConfiguration(const std::string& runName) const {
        db::ConfigurationInterface ifc;
        config_version_map_t result;

        auto members = ifc.loadGlobalConfiguration(runName);

        for (const auto& m : members) {
            result[m.configuration] = std::stoi(m.version);
        }

        return result;
    }

    // Save run configuration
    result_t saveRunConfiguration(const config_version_map_t& configs,
                                  const std::string& runName,
                                  bool overwrite = false) const {
        db::ConfigurationInterface ifc;

        db::ConfigurationInterface::VersionInfoList_t composition;
        for (const auto& [configType, version] : configs) {
            composition.push_back({configType, std::to_string(version), m_defaultEntity});
        }

        return ifc.storeGlobalConfiguration(composition, runName, overwrite);
    }

private:
    std::string m_defaultEntity = "MySystem";
};
```

---

## 13. Real-World Integration Pattern (OtsDaq)

This section shows a production-quality integration pattern based on the OtsDaq test code.

### 13.1 The Interface Header

```cpp
// DatabaseConfigurationInterface.h
#ifndef DATABASE_CONFIGURATION_INTERFACE_H
#define DATABASE_CONFIGURATION_INTERFACE_H

#include <set>
#include <map>
#include <string>

// Forward declaration
class ConfigurationBase;

namespace ots {

class DatabaseConfigurationInterface {
public:
    using result_t = std::pair<bool, std::string>;
    using config_version_map_t = std::map<std::string, int>;

    //=======================================================================
    // Single configuration operations
    //=======================================================================

    // Load configuration (-1 = latest version)
    int fill(ConfigurationBase* configuration, int version) const noexcept;

    // Save configuration (overwrite=false for new, true for update)
    int saveActiveVersion(const ConfigurationBase* configuration,
                          bool overwrite = false) const noexcept;

    // Mark as read-only
    int markActiveVersionReadonly(const ConfigurationBase* configuration) const noexcept;

    // Find latest version
    int findLatestVersion(const ConfigurationBase* configuration) const noexcept;

    // Get all versions
    std::set<int> getVersions(const ConfigurationBase* configuration) const noexcept;

    //=======================================================================
    // Collection operations
    //=======================================================================

    std::set<std::string> listConfigurationsTypes() const;

    //=======================================================================
    // Global configuration operations
    //=======================================================================

    std::set<std::string> findAllGlobalConfigurations(std::string const& search) const;

    config_version_map_t loadGlobalConfiguration(std::string const& configuration) const;

    void storeGlobalConfiguration(config_version_map_t const& configurationMap,
                                  std::string const& configuration,
                                  bool allowOverwrite = false) const;

    void storeGlobalConfiguration_mt(config_version_map_t const& configurationMap,
                                     std::string const& configuration,
                                     bool allowOverwrite = false) const;

    std::set<std::string> findCompositionsContaining(std::string const& configurationType,
                                                     std::string const& version) const;

    //=======================================================================
    // Safe (non-throwing) versions
    //=======================================================================

    result_t getVersions_safe(const ConfigurationBase* configuration,
                              std::set<int>& versions) const noexcept;

    result_t listConfigurationsTypes_safe(std::set<std::string>& collections) const noexcept;

    result_t findAllGlobalConfigurations_safe(std::string const& search,
                                              std::set<std::string>& configurations) const noexcept;

    result_t loadGlobalConfiguration_safe(std::string const& configuration,
                                          config_version_map_t& configMap) const noexcept;

    result_t findCompositionsContaining_safe(std::string const& configurationType,
                                             std::string const& version,
                                             std::set<std::string>& compositions) const noexcept;
};

} // namespace ots

#endif
```

### 13.2 Implementation Highlights

```cpp
// DatabaseConfigurationInterface.cpp
#include "DatabaseConfigurationInterface.h"
#include "artdaq-database/ConfigurationDB/configurationdbifc.h"
#include "artdaq-database/BasicTypes/basictypes.h"

using artdaq::database::basictypes::JsonData;

namespace db = artdaq::database::configuration;
using VersionInfoList_t = db::ConfigurationInterface::VersionInfoList_t;

constexpr auto default_dbprovider = "filesystem";  // Ignored but kept for compatibility
constexpr auto default_entity = "OTSDAQROOT";

namespace ots {

int DatabaseConfigurationInterface::fill(ConfigurationBase* configuration, int version) const noexcept {
    auto ifc = db::ConfigurationInterface{default_dbprovider};

    auto versionstring = std::to_string(version);

    // -1 means "load latest version"
    if (-1 == version) {
        versionstring = std::to_string(findLatestVersion(configuration));
    }

    auto result = ifc.template loadVersion<decltype(configuration), JsonData>(
        configuration, versionstring, default_entity);

    if (result.first) {
        return 0;
    }

    std::cout << "DBI Error:" << result.second << "\n";
    return -1;
}

int DatabaseConfigurationInterface::saveActiveVersion(const ConfigurationBase* configuration,
                                                       bool overwrite) const noexcept {
    auto ifc = db::ConfigurationInterface{default_dbprovider};

    auto versionstring = std::to_string(configuration->getViewVersion());

    auto result = overwrite
        ? ifc.template overwriteVersion<decltype(configuration), JsonData>(
            configuration, versionstring, default_entity)
        : ifc.template storeVersion<decltype(configuration), JsonData>(
            configuration, versionstring, default_entity);

    if (result.first) {
        return 0;
    }

    std::cout << "DBI Error:" << result.second << "\n";
    return -1;
}

std::set<int> DatabaseConfigurationInterface::getVersions(
    const ConfigurationBase* configuration) const noexcept try {

    auto ifc = db::ConfigurationInterface{default_dbprovider};
    auto result = ifc.template getVersions<decltype(configuration)>(
        configuration, default_entity);

    // Convert string versions to integers
    auto to_set = [](auto const& inputList) {
        auto resultSet = std::set<int>{};
        std::for_each(inputList.begin(), inputList.end(),
            [&resultSet](std::string const& version) {
                resultSet.insert(std::stol(version, nullptr, 10));
            });
        return resultSet;
    };

    return to_set(result);
} catch (std::exception const& e) {
    std::cout << "DBI Exception:" << e.what() << "\n";
    return {};
}

// Helper to convert config map to VersionInfoList
namespace {
VersionInfoList_t to_version_info_list(
    std::map<std::string, int> const& inputMap) {

    auto resultList = VersionInfoList_t{};
    std::transform(inputMap.begin(), inputMap.end(),
        std::back_inserter(resultList),
        [](auto const& mapEntry) {
            return VersionInfoList_t::value_type{
                mapEntry.first,                    // configuration type
                std::to_string(mapEntry.second),   // version as string
                default_entity                     // entity
            };
        });
    return resultList;
}
} // anonymous namespace

void DatabaseConfigurationInterface::storeGlobalConfiguration(
    config_version_map_t const& configurationMap,
    std::string const& configuration,
    bool allowOverwrite) const try {

    auto ifc = db::ConfigurationInterface{default_dbprovider};

    auto result = ifc.storeGlobalConfiguration(
        to_version_info_list(configurationMap),
        configuration,
        allowOverwrite);

    if (result.first) {
        return;
    }

    throw std::runtime_error(result.second);
} catch (std::exception const& e) {
    std::cout << "DBI Exception:" << e.what() << "\n";
    throw std::runtime_error(e.what());
}

} // namespace ots
```

### 13.3 MakeSerializable Implementation

```cpp
// ConfigurationBaseImpl.cpp - MakeSerializable specializations
#include "artdaq-database/ConfigurationDB/configurationdbifc.h"
#include "artdaq-database/BasicTypes/basictypes.h"
#include "ConfigurationBase.h"

using artdaq::database::basictypes::JsonData;

namespace artdaq {
namespace database {
namespace configuration {

// For writing (storing to database)
template <>
template <>
bool MakeSerializable<ConfigurationBase const*>::writeDocumentImpl<JsonData>(
    JsonData& data) const {

    std::stringstream ss;
    _conf->getView().printJSON(ss);  // Your method to serialize to JSON
    data.json_buffer = ss.str();
    return true;
}

template <>
std::string MakeSerializable<ConfigurationBase const*>::configurationNameImpl() const {
    return _conf->getConfigurationName();
}

// For reading (loading from database)
template <>
template <>
bool MakeSerializable<ConfigurationBase*>::readDocumentImpl<JsonData>(
    JsonData const& data) {

    int retVal = _conf->getViewP()->fillFromJSON(data);  // Your method to deserialize
    return (retVal >= 0);
}

template <>
std::string MakeSerializable<ConfigurationBase*>::configurationNameImpl() const {
    return _conf->getConfigurationName();
}

}  // namespace configuration
}  // namespace database
}  // namespace artdaq
```

---

## 14. Troubleshooting Guide

### 14.1 Common Issues and Solutions

#### Issue: "Document not found" when loading

**Symptoms:** `loadVersion` returns false with "Document not found"

**Causes:**
1. Version doesn't exist
2. Entity name mismatch
3. Wrong database URI
4. Collection name mismatch (wrong `configurationNameImpl`)

**Debug Steps:**
```cpp
// List available versions first
try {
    auto versions = ifc.getVersions(&config, "MyEntity");
    std::cout << "Available versions for entity 'MyEntity':\n";
    for (const auto& v : versions) {
        std::cout << "  " << v << "\n";
    }
} catch (const std::exception& e) {
    std::cout << "No versions found or error: " << e.what() << "\n";
}

// Check what collection name is being used
auto serializer = db::ConfigurationSerializer<MyConfig*, db::MakeSerializable>::wrap(&config);
std::cout << "Collection name: " << serializer.configurationName() << "\n";

// Check database URI
std::cout << "ARTDAQ_DATABASE_URI: "
          << (getenv("ARTDAQ_DATABASE_URI") ? getenv("ARTDAQ_DATABASE_URI") : "NOT SET")
          << "\n";
```

#### Issue: "Document already exists" when storing

**Symptoms:** `storeVersion` fails for existing version

**Solution:** Use `overwriteVersion` or store with a new version:
```cpp
// Option 1: Overwrite existing
auto result = ifc.overwriteVersion<MyConfig const*, JsonData>(&config, "v1.0", "Entity1");

// Option 2: Store as new version
auto result = ifc.storeVersion<MyConfig const*, JsonData>(&config, "v1.1", "Entity1");
```

#### Issue: Empty results from searches

**Symptoms:** `findGlobalConfigurations` or `listCollections` returns empty

**Debug Steps:**
```cpp
// Check database connection
std::cout << "ARTDAQ_DATABASE_URI: "
          << (getenv("ARTDAQ_DATABASE_URI") ? getenv("ARTDAQ_DATABASE_URI") : "NOT SET")
          << "\n";

// For FileSystemDB, check directory exists and has data
// The path should be: $ARTDAQ_DATABASE_URI path + /SystemMetadata/
```

#### Issue: Linker errors for MakeSerializable

**Symptoms:** Undefined reference to `MakeSerializable<...>::readDocumentImpl`

**Solution:** Ensure template specializations are in a .cpp file that gets compiled and linked:
```cpp
// This must be in a compiled .cpp file, not just a header!
// MyConfigSerializable.cpp

template <>
template <>
bool MakeSerializable<MyConfig*>::readDocumentImpl<JsonData>(JsonData const& data) {
    // Implementation
}
```

Also ensure your CMakeLists.txt includes this .cpp file in your target's sources.

#### Issue: Configuration name is wrong

**Symptoms:** Data stored under unexpected collection name

**Debug:**
```cpp
// Check what name is being used
auto serializer = db::ConfigurationSerializer<MyConfig*, db::MakeSerializable>::wrap(&config);
std::cout << "Collection name: " << serializer.configurationName() << "\n";

// If configurationNameImpl throws, it falls back to demangled type name
// Make sure your implementation doesn't throw
```

#### Issue: "Entity is empty" in global configuration

**Symptoms:** `storeGlobalConfiguration` fails with validation error

**Solution:** All `VersionInfo` in a composition must have non-empty entity:
```cpp
// WRONG
composition.push_back({"MyConfig", "v1.0", ""});  // Empty entity!

// CORRECT
composition.push_back({"MyConfig", "v1.0", "MyEntity"});
```

### 14.2 Debugging Tips

#### Enable TRACE Logging

```bash
# Set environment variables before running
export TRACE_NAME="*"
export TRACE_LVLM=0xFFFFFFFFFFFFFFFF
export TRACE_FILE=/tmp/artdaq_trace.log
```

The ConfigurationInterface uses `TLOG(TLVL_DEBUG)` and `TLOG(TLVL_WARNING)` for internal logging.

#### Check FileSystemDB Structure

For FileSystemDB, the directory structure should be:
```
$ARTDAQ_DATABASE_URI_PATH/
├── SystemMetadata/
│   └── metadata.json
├── MyConfig/
│   ├── Entity1_v1.0.json
│   └── Entity1_v1.1.json
├── TriggerConfig/
│   └── Trigger1_v2.0.json
└── GlobalConfigurations/
    └── Run12345.json
```

#### Verify JSON Serialization

```cpp
// Test your serialization independently
MyConfig config;
config.param1 = 42;

// Test toJson
std::string json = config.toJson();
std::cout << "Serialized: " << json << "\n";

// Test fromJson
MyConfig loaded;
bool success = loaded.fromJson(json);
std::cout << "Deserialized: " << (success ? "OK" : "FAILED") << "\n";
std::cout << "param1 = " << loaded.param1 << "\n";
```

---

## 15. Best Practices

### 15.1 Version Naming Conventions

**Recommended Approaches:**

```cpp
// Semantic versioning (recommended for software configs)
"v1.0.0", "v1.0.1", "v1.1.0", "v2.0.0"

// Sequential integers (simple, easy to find latest)
"1", "2", "3", "100"

// Date-based (good for daily configurations)
"2025-01-15", "2025-01-16"

// Run-based (for experimental data)
"run_12345", "run_12346"
```

**Avoid:**
```cpp
// DON'T use ambiguous names
"latest"      // What is "latest"?
"new"         // Quickly becomes confusing
"test"        // Too generic
"temp"        // Will it be deleted?
""            // Empty string causes errors!
```

### 15.2 Entity Naming Conventions

```cpp
// Use hierarchical naming for complex systems
"System.Subsystem.Component"
"DAQ.Frontend.BoardReader01"
"DAQ.Backend.EventBuilder"

// Or flat naming for simple systems
"BoardReader_01"
"BoardReader_02"
"Trigger_Main"

// Use a consistent default for single-entity systems
constexpr auto DEFAULT_ENTITY = "PRODUCTION";
```

### 15.3 Error Handling Best Practices

```cpp
// ALWAYS check results
auto result = ifc.storeVersion<MyConfig const*, JsonData>(&config, version, entity);
if (!result.first) {
    // Log the error
    TLOG(TLVL_ERROR) << "Store failed: " << result.second;
    // Return error code or throw
    return -1;
}

// For throwing methods, use specific catch blocks (most specific first)
try {
    auto versions = ifc.getVersions(&config, entity);
} catch (const artdaq::database::invalid_option_exception& e) {
    // Handle invalid parameters
    TLOG(TLVL_WARNING) << "Invalid parameter: " << e.what();
} catch (const artdaq::database::runtime_exception& e) {
    // Handle database errors
    TLOG(TLVL_ERROR) << "Database error: " << e.what();
} catch (const std::exception& e) {
    // Handle unexpected errors
    TLOG(TLVL_ERROR) << "Unexpected: " << e.what();
}
```

### 15.4 Performance Tips

```cpp
// Use _mt version for large compositions (>10 members)
if (composition.size() > 10) {
    result = ifc.storeGlobalConfiguration_mt(composition, name);
} else {
    result = ifc.storeGlobalConfiguration(composition, name);
}

// Reuse interface instance within a function (but not across threads!)
void processMultipleConfigs() {
    db::ConfigurationInterface ifc;  // Create once

    for (const auto& config : configs) {
        ifc.storeVersion(...);  // Reuse for multiple operations
    }
}

// DON'T create interface in tight loops
// BAD:
for (int i = 0; i < 1000; ++i) {
    db::ConfigurationInterface ifc;  // Creating 1000 times!
    ifc.storeVersion(...);
}

// GOOD:
db::ConfigurationInterface ifc;  // Create once
for (int i = 0; i < 1000; ++i) {
    ifc.storeVersion(...);
}
```

### 15.5 Code Organization

```cpp
// Create a wrapper class for your project
class MyProjectDBInterface {
public:
    // Project-specific methods with your conventions
    int loadTriggerConfig(TriggerConfig* config, int version);
    int saveTriggerConfig(const TriggerConfig* config);
    std::set<int> getTriggerVersions();

private:
    static constexpr auto DEFAULT_ENTITY = "MyProject_Main";
};

// Use the wrapper throughout your code
MyProjectDBInterface dbInterface;
TriggerConfig trigConfig;

if (dbInterface.loadTriggerConfig(&trigConfig, 5) == 0) {
    // Success
}
```

### 15.6 Testing Recommendations

```cpp
// Test with FileSystemDB first (fastest, no external dependencies)
export ARTDAQ_DATABASE_URI="filesystemdb:///tmp/test_db"

// Clean up test database before/after tests
rm -rf /tmp/test_db && mkdir -p /tmp/test_db

// Test both store and load round-trip
void testRoundTrip() {
    MyConfig original;
    original.param1 = 42;

    db::ConfigurationInterface ifc;

    // Store
    auto storeResult = ifc.storeVersion<MyConfig const*, JsonData>(
        &original, "test_v1", "TestEntity");
    ASSERT_TRUE(storeResult.first);

    // Load
    MyConfig loaded;
    auto loadResult = ifc.loadVersion<MyConfig*, JsonData>(
        &loaded, "test_v1", "TestEntity");
    ASSERT_TRUE(loadResult.first);

    // Verify
    ASSERT_EQ(original.param1, loaded.param1);
}
```

---

## 16. Quick Reference

### 16.1 Method Summary

| Method | Returns | Throws | Error Format | Purpose |
|--------|---------|--------|--------------|---------|
| `storeVersion<CONF, TYPE>()` | `result_t` | No | Raw | Store new configuration |
| `overwriteVersion<CONF, TYPE>()` | `result_t` | No | Raw | Update existing configuration |
| `loadVersion<CONF, TYPE>()` | `result_t` | No | Raw | Load configuration |
| `markVersionReadonly<CONF, TYPE>()` | `result_t` | No | Raw | Protect configuration |
| `getVersions<CONF>()` | `list<string>` | **Yes** | N/A | List versions |
| `getVersions_safe<CONF>()` | `result_t` | No | `{error:...}` | List versions (safe) |
| `listCollections()` | `set<string>` | **Yes** | N/A | List configuration types |
| `listCollections_safe()` | `result_t` | No | `{error:...}` | List types (safe) |
| `findGlobalConfigurations()` | `set<string>` | **Yes** | N/A | Find compositions |
| `findGlobalConfigurations_safe()` | `result_t` | No | `{error:...}` | Find compositions (safe) |
| `loadGlobalConfiguration()` | `VersionInfoList_t` | **Yes** | N/A | Load composition |
| `loadGlobalConfiguration_safe()` | `result_t` | No | `{error:...}` | Load composition (safe) |
| `storeGlobalConfiguration()` | `result_t` | No | `{error:...}` | Store composition |
| `storeGlobalConfiguration_mt()` | `result_t` | No | `{error:...}` | Store composition (parallel) |
| `findGlobalConfigurationsContaining()` | `set<string>` | **Yes** | N/A | Reverse lookup |
| `findGlobalConfigurationsContaining_safe()` | `result_t` | No | `{error:...}` | Reverse lookup (safe) |

**Error Format Legend:**
- **Raw**: Returns `e.what()` directly (e.g., `"ConfigurationInterface::storeVersion: Version is empty"`)
- **`{error:...}`**: Returns `{error:"message"}` format
- **N/A**: Method throws exceptions instead of returning errors

### 16.2 Template Parameters

```cpp
// CONF = Configuration type (MUST be pointer or reference)
MyConfig*           // For mutable access (loading)
MyConfig const*     // For const access (storing)

// TYPE = Data format
JsonData            // JSON format (recommended)
FhiclData           // FHiCL format
```

### 16.3 Include Files

```cpp
// Main interface (includes everything you need)
#include "artdaq-database/ConfigurationDB/configurationdbifc.h"

// Data types (also included by configurationdbifc.h)
#include "artdaq-database/BasicTypes/basictypes.h"

// For implementing MakeSerializable (in your .cpp file)
#include "artdaq-database/ConfigurationDB/configurationdbifc_base.h"
```

### 16.4 Namespace Aliases

```cpp
namespace db = artdaq::database::configuration;
using artdaq::database::basictypes::JsonData;
using artdaq::database::basictypes::FhiclData;
using VersionInfo = db::VersionInfo;
using VersionInfoList_t = db::ConfigurationInterface::VersionInfoList_t;
using result_t = db::result_t;
```

### 16.5 Environment Variables

| Variable | Purpose | Example |
|----------|---------|---------|
| `ARTDAQ_DATABASE_URI` | Database connection | `filesystemdb:///data/db` |
| `TRACE_NAME` | Enable logging | `*` |
| `TRACE_LVLM` | Log level mask | `0xFFFFFFFFFFFFFFFF` |
| `TRACE_FILE` | Log file output | `/tmp/trace.log` |

### 16.6 Constructor Quick Reference

```cpp
// Default constructor (RECOMMENDED)
db::ConfigurationInterface ifc;

// String constructor (parameter is IGNORED)
db::ConfigurationInterface ifc("ignored_string");

// Move construction (OK)
db::ConfigurationInterface ifc2 = std::move(ifc);

// Copy construction (NOT ALLOWED - compile error)
// db::ConfigurationInterface ifc3 = ifc;  // ERROR!
```

---

## Appendix A: Internal Types and Helpers

### A.1 ConfigurationSerializer

The `ConfigurationSerializer` template wraps your configuration and handles the connection to `MakeSerializable`:

```cpp
// From configurationdbifc_base.h
template <typename CONF, template <typename ARG = CONF> class SERIALIZABLE>
class ConfigurationSerializer final {
public:
    using Serializable_t = SERIALIZABLE<CONF>;

    // Factory method to create a serializer
    static ConfigurationSerializer<CONF, SERIALIZABLE> wrap(CONF conf);

    // Write configuration to data format (for storing)
    // Returns result_t (pair<bool, string>)
    template <class TYPE>
    cf::result_t writeDocument(TYPE& data) const noexcept;

    // Read configuration from data format (for loading)
    // Returns result_t (pair<bool, string>)
    template <class TYPE>
    cf::result_t readDocument(TYPE const& data) noexcept;

    // Get configuration name (with fallback to demangled type name)
    std::string configurationName() const noexcept;
};
```

**Key Feature - Fallback Configuration Name:**

If your `configurationNameImpl()` throws an exception, `ConfigurationSerializer::configurationName()` falls back to using the demangled C++ type name:

```cpp
std::string configurationName() const noexcept try {
    return Serializable_t{_conf}.configurationName();
} catch (std::exception const&) {
    // Fallback: use demangled type name
    using configurationClazz = typename std::remove_pointer<
        typename std::remove_reference<CONF>::type>::type;

    auto name = demangle(typeid(configurationClazz).name());

    std::size_t found = name.find_last_of(":");
    if (found != std::string::npos) return name.substr(found + 1);

    return name;
}
```

### A.2 CompositionDiff (Internal)

The `storeGlobalConfiguration` method uses an internal `CompositionDiff` structure to compute changes when updating existing compositions:

```cpp
// Internal to ConfigurationInterface
struct CompositionDiff {
    VersionInfoList_t to_unassign;  // Members to remove
    VersionInfoList_t to_assign;    // Members to add
    std::set<MembershipKey_t> unchanged;  // Members that stay the same
};

// MembershipKey_t = pair<configuration_type, entity>
```

This enables efficient updates by only modifying what changed rather than replacing the entire composition.

### A.3 Data Format Selection

The internal `getDataFormat<TYPE>()` helper determines the storage format:

```cpp
template <typename TYPE>
static constexpr data_format_t getDataFormat() {
    if constexpr (std::is_same_v<TYPE, JsonData>) {
        return data_format_t::json;
    } else if constexpr (std::is_same_v<TYPE, FhiclData>) {
        return data_format_t::fhicl;
    } else {
        static_assert(sizeof(TYPE) == 0,
            "Unsupported format type: use JsonData or FhiclData");
    }
}
```

This means you'll get a **compile-time error** if you try to use an unsupported data format.

---

## Appendix B: Complete Code Template

```cpp
//==========================================================================
// MyConfigDatabase.h - Header file
//==========================================================================
#ifndef MY_CONFIG_DATABASE_H
#define MY_CONFIG_DATABASE_H

#include "artdaq-database/ConfigurationDB/configurationdbifc.h"
#include "artdaq-database/BasicTypes/basictypes.h"
#include "MyConfig.h"

namespace db = artdaq::database::configuration;
using artdaq::database::basictypes::JsonData;

class MyConfigDatabase {
public:
    using result_t = db::result_t;

    // Store new version
    result_t store(const MyConfig& config, const std::string& version,
                   const std::string& entity) const {
        db::ConfigurationInterface ifc;
        return ifc.storeVersion<const MyConfig*, JsonData>(&config, version, entity);
    }

    // Load version
    result_t load(MyConfig& config, const std::string& version,
                  const std::string& entity) const {
        db::ConfigurationInterface ifc;
        return ifc.loadVersion<MyConfig*, JsonData>(&config, version, entity);
    }

    // List versions (safe)
    result_t listVersions(const MyConfig& config, const std::string& entity,
                          std::list<std::string>& versions) const {
        db::ConfigurationInterface ifc;
        return ifc.getVersions_safe(&config, entity, versions);
    }
};

#endif

//==========================================================================
// MyConfigSerializable.cpp - MakeSerializable implementation
//==========================================================================
#include "MyConfig.h"
#include "artdaq-database/ConfigurationDB/configurationdbifc.h"

using artdaq::database::basictypes::JsonData;

namespace artdaq {
namespace database {
namespace configuration {

// Reading (loading)
template <>
template <>
bool MakeSerializable<MyConfig*>::readDocumentImpl<JsonData>(JsonData const& data) {
    return _conf->fromJson(data.json_buffer);
}

template <>
std::string MakeSerializable<MyConfig*>::configurationNameImpl() const {
    return _conf->getConfigurationName();
}

// Writing (storing)
template <>
template <>
bool MakeSerializable<MyConfig const*>::writeDocumentImpl<JsonData>(JsonData& data) const {
    data.json_buffer = _conf->toJson();
    return true;
}

template <>
std::string MakeSerializable<MyConfig const*>::configurationNameImpl() const {
    return _conf->getConfigurationName();
}

}  // namespace configuration
}  // namespace database
}  // namespace artdaq
```

---

## Appendix C: Exception Hierarchy

```
std::exception
└── cet::exception
    └── artdaq::database::exception
        ├── artdaq::database::invalid_argument
        ├── artdaq::database::runtime_error
        ├── artdaq::database::invalid_option_exception
        └── artdaq::database::runtime_exception
```

**Exception Definitions (from shared_exceptions.h):**

```cpp
namespace artdaq::database {

// Base exception (inherits from cet::exception)
class exception : public cet::exception {
public:
    explicit exception(Category const& category);
    exception(Category const& category, std::string const& message);
};

// For invalid arguments passed to functions
class invalid_argument : public exception {
public:
    explicit invalid_argument(std::string const& category_);
};

// For general runtime errors
class runtime_error : public exception {
public:
    explicit runtime_error(std::string const& category_);
};

// For invalid options/parameters (most common in ConfigurationInterface)
class invalid_option_exception : public exception {
public:
    explicit invalid_option_exception(Category const& category);
    invalid_option_exception(Category const& category, std::string const& message);
};

// For runtime exceptions during database operations (most common)
class runtime_exception : public exception {
public:
    explicit runtime_exception(Category const& category);
    runtime_exception(Category const& category, std::string const& message);
};

} // namespace
```

**Common Exception Scenarios:**

| Exception Type | When It's Thrown |
|----------------|------------------|
| `invalid_option_exception` | Empty version string, empty configuration name, invalid `VersionInfo` |
| `runtime_exception` | Database connection failed, document not found, JSON parsing error |
| `invalid_argument` | Invalid data passed to internal functions |

---

## Appendix D: Complete API Reference

### All Public Methods

```cpp
struct ConfigurationInterface final {
    // Type aliases
    using VersionInfoList_t = std::list<VersionInfo>;

    // Constructors
    ConfigurationInterface() = default;
    ConfigurationInterface(std::string const&);  // Parameter IGNORED
    ConfigurationInterface(ConfigurationInterface&&) = default;

    // Deleted operations
    ConfigurationInterface(ConfigurationInterface const&) = delete;
    ConfigurationInterface& operator=(ConfigurationInterface const&) = delete;
    ConfigurationInterface& operator=(ConfigurationInterface&&) = delete;

    // Version operations (return result_t, noexcept)
    template <typename CONF, typename TYPE>
    cf::result_t storeVersion(CONF configuration, std::string const& version,
                              std::string const& entity) const noexcept;

    template <typename CONF, typename TYPE>
    cf::result_t overwriteVersion(CONF configuration, std::string const& version,
                                  std::string const& entity) const noexcept;

    template <typename CONF, typename TYPE>
    cf::result_t loadVersion(CONF configuration, std::string const& version,
                             std::string const& entity) const noexcept;

    template <typename CONF, typename TYPE>
    cf::result_t markVersionReadonly(CONF configuration, std::string const& version,
                                     std::string const& entity) const noexcept;

    // Query operations (THROW on error)
    template <typename CONF>
    std::list<std::string> getVersions(CONF configuration, std::string const& entity) const;

    std::set<std::string> findGlobalConfigurations(std::string const& search) const;

    VersionInfoList_t loadGlobalConfiguration(std::string const& configuration) const;

    std::set<std::string> listCollections(std::string const& name_prefix) const;

    std::set<std::string> findGlobalConfigurationsContaining(
        std::string const& configurationType, std::string const& version,
        std::string const& entity = "") const;

    // Global configuration storage (return result_t)
    cf::result_t storeGlobalConfiguration(VersionInfoList_t const& versionInfoList,
                                          std::string const& configuration,
                                          bool allowOverwrite = false) const;

    cf::result_t storeGlobalConfiguration_mt(VersionInfoList_t const& versionInfoList,
                                             std::string const& configuration,
                                             bool overwrite = false) const;

    // Safe versions (return result_t, noexcept)
    template <typename CONF>
    cf::result_t getVersions_safe(CONF configuration, std::string const& entity,
                                  std::list<std::string>& versions) const noexcept;

    cf::result_t findGlobalConfigurations_safe(std::string const& search,
                                               std::set<std::string>& configurations) const noexcept;

    cf::result_t loadGlobalConfiguration_safe(std::string const& configuration,
                                              VersionInfoList_t& members) const noexcept;

    cf::result_t listCollections_safe(std::string const& name_prefix,
                                      std::set<std::string>& collections) const noexcept;

    cf::result_t findGlobalConfigurationsContaining_safe(
        std::string const& configurationType, std::string const& version,
        std::string const& entity, std::set<std::string>& compositions) const noexcept;
};
```

---

**End of Tutorial**

*For additional help, see the API reference documentation or contact the artdaq-database maintainers.*

---

**Document History:**
- Version 2.1: Critical accuracy fixes
  - Fixed: Documented inconsistent error message formats (raw vs `{error:...}`) between methods
  - Fixed: Documented dual `result_t` namespace definitions
  - Fixed: Clarified entity behavior when empty (option omitted vs set to empty)
  - Added: Error Format column to Method Summary table
  - Added: Best practices for handling variable error formats
- Version 2.0: Comprehensive rewrite with accurate API documentation
- Reviewed against source code: `configurationdbifc.h`, `configurationdbifc_base.h`
- Examples based on OtsDaq integration test patterns
