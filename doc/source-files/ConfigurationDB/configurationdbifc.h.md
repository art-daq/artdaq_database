# configurationdbifc.h

## File Overview

This header file defines the primary `ConfigurationInterface` class that provides a high-level C++ API for storing, loading, and managing artdaq configurations in the database. It serves as the main entry point for user code to interact with the configuration database.

**Location**: `/home/user/artdaq-database/artdaq-database/ConfigurationDB/configurationdbifc.h`

**Lines of Code**: 544

**Purpose**: Main configuration database interface providing high-level API for configuration management

## Dependencies

### Standard Library
- None directly included (inherited from base classes)

### Project Headers
- `"artdaq-database/ConfigurationDB/Multitasker.h"` - Multi-threaded task execution
- `"artdaq-database/ConfigurationDB/configurationdbifc_base.h"` - Base classes and serialization support
- `"artdaq-database/JsonDocument/JSONDocumentBuilder.h"` - JSON document construction
- `"dboperation_findcompositions.h"` - Find compositions containing specific versions
- `"options_operation_manageconfigs.h"` - Configuration management operations

## Namespace: artdaq::database::configuration

All types and functions in this file are declared within the `artdaq::database::configuration` namespace.

## Constants

### apiname
```cpp
constexpr auto apiname = "ConfigurationInterface";
```

**Purpose**: String literal identifying this API interface in logging and error messages.

---

## Class: ConfigurationInterface

### Purpose

`ConfigurationInterface` is the primary facade class for interacting with the artdaq configuration database. It provides template-based methods for storing and retrieving configurations in multiple formats (JSON, FHiCL), managing versions, and handling global configurations.

### Type Aliases

```cpp
using VersionInfoList_t = std::list<VersionInfo>;
```

**Purpose**: Container type for lists of version information tuples (configuration, version, entity).

---

## Constructors and Lifecycle

### Constructor
```cpp
ConfigurationInterface(std::string const&)
```

**Purpose**: Construct a configuration interface instance.

**Parameters**:
- Connection string or configuration (currently unused)

**Note**: The constructor body is empty but maintains signature compatibility.

### Default Constructor
```cpp
ConfigurationInterface() = default;
```

**Purpose**: Default constructor for creating instances without parameters.

### Move Constructor
```cpp
ConfigurationInterface(ConfigurationInterface&&) = default;
```

**Purpose**: Enables moving of ConfigurationInterface instances.

### Deleted Operations
```cpp
ConfigurationInterface(ConfigurationInterface const&) = delete;
ConfigurationInterface& operator=(ConfigurationInterface const&) = delete;
ConfigurationInterface& operator=(ConfigurationInterface&&) = delete;
```

**Design**: Copy operations are deleted - ConfigurationInterface is move-only.

---

## Template Methods: Version Management

### storeVersion
```cpp
template <typename CONF, typename TYPE>
cf::result_t storeVersion(CONF configuration,
                          std::string const& version,
                          std::string const& entity) const noexcept
```

**Purpose**: Store a new configuration version to the database.

**Template Parameters**:
- `CONF` - Configuration object type (pointer or reference)
- `TYPE` - Storage format type (`JsonData` or `FhiclData`)

**Parameters**:
- `configuration` - Configuration object to store
- `version` - Version identifier (must be non-empty)
- `entity` - Entity name (component/system name), optional

**Returns**: `result_t` - Pair of (success boolean, message string)

**Exception Safety**: noexcept - catches all exceptions and returns error result

**Usage Example**:
```cpp
ConfigurationInterface ifc;
MyConfiguration config;
// ... populate config ...

auto result = ifc.storeVersion<MyConfiguration*, JsonData>(
    &config,
    "v1.0",
    "DAQComponent1"
);

if (result.first) {
    std::cout << "Successfully stored configuration\n";
} else {
    std::cerr << "Error: " << result.second << "\n";
}
```

**Implementation Details**:
1. Validates version is non-empty
2. Creates serializer wrapper for the configuration
3. Sets up document operation with collection name from configuration
4. Serializes configuration to JSON or FHiCL format
5. Writes document to database via implementation layer
6. Returns success/failure with message

**Validation**:
- Throws `invalid_option_exception` if version is empty
- Throws `invalid_option_exception` if TYPE is not JsonData or FhiclData

---

### overwriteVersion
```cpp
template <typename CONF, typename TYPE>
cf::result_t overwriteVersion(CONF configuration,
                              std::string const& version,
                              std::string const& entity) const noexcept
```

**Purpose**: Overwrite an existing configuration version in the database.

**Template Parameters**:
- `CONF` - Configuration object type
- `TYPE` - Storage format type (`JsonData` or `FhiclData`)

**Parameters**:
- `configuration` - Configuration object with new data
- `version` - Version identifier to overwrite
- `entity` - Entity name, optional

**Returns**: `result_t` - Success status and message

**Exception Safety**: noexcept

**Implementation Details**:
1. Reads existing document from database
2. Serializes new configuration data
3. Replaces document data while preserving metadata
4. Writes updated document back to database

**Usage**:
```cpp
auto result = ifc.overwriteVersion<Config*, JsonData>(
    &newConfig,
    "v1.0",  // existing version to overwrite
    "Component1"
);
```

**Warning**: Overwrites existing data - ensure version exists before calling.

---

### markVersionReadonly
```cpp
template <typename CONF, typename TYPE>
cf::result_t markVersionReadonly(CONF configuration,
                                 std::string const& version,
                                 std::string const& entity) const noexcept
```

**Purpose**: Mark a configuration version as read-only to prevent future modifications.

**Template Parameters**:
- `CONF` - Configuration object type
- `TYPE` - Storage format type

**Parameters**:
- `configuration` - Configuration object (used for collection name)
- `version` - Version to mark read-only
- `entity` - Entity name, optional

**Returns**: `result_t` - Success status and message

**Usage**:
```cpp
// Protect a production configuration from changes
auto result = ifc.markVersionReadonly<Config*, JsonData>(
    &config,
    "v2.0_production",
    "DAQSystem"
);
```

**Effect**: After marking read-only, attempts to overwrite the version will fail.

---

### loadVersion
```cpp
template <typename CONF, typename TYPE>
cf::result_t loadVersion(CONF configuration,
                         std::string const& version,
                         std::string const& entity) const noexcept
```

**Purpose**: Load a configuration version from the database.

**Template Parameters**:
- `CONF` - Configuration object type (will be populated)
- `TYPE` - Storage format type (`JsonData` or `FhiclData`)

**Parameters**:
- `configuration` - Configuration object to populate (in/out parameter)
- `version` - Version identifier to load
- `entity` - Entity name, optional

**Returns**: `result_t` - Success status and message

**Usage Example**:
```cpp
ConfigurationInterface ifc;
MyConfiguration config;

auto result = ifc.loadVersion<MyConfiguration*, JsonData>(
    &config,
    "v1.5",
    "Component1"
);

if (result.first) {
    // config is now populated with data from database
    std::cout << "Loaded configuration v1.5\n";
} else {
    std::cerr << "Failed to load: " << result.second << "\n";
}
```

**Implementation**:
1. Reads document from database in specified format
2. Deserializes data using configuration's readDocument method
3. Populates the configuration object with loaded data

---

### getVersions
```cpp
template <typename CONF>
std::list<std::string> getVersions(CONF configuration,
                                   std::string const& entity) const
```

**Purpose**: Retrieve list of all available versions for a configuration type.

**Template Parameters**:
- `CONF` - Configuration object type

**Parameters**:
- `configuration` - Configuration object (used to determine collection name)
- `entity` - Entity name to filter by (optional, empty for all)

**Returns**: `std::list<std::string>` - List of version identifiers

**Throws**:
- `runtime_exception` on database errors
- `runtime_exception` on JSON parsing errors

**Usage Example**:
```cpp
ConfigurationInterface ifc;
MyConfiguration config;

try {
    auto versions = ifc.getVersions(&config, "DAQComponent1");

    std::cout << "Available versions:\n";
    for (auto const& ver : versions) {
        std::cout << "  - " << ver << "\n";
    }
} catch (std::exception const& e) {
    std::cerr << "Error: " << e.what() << "\n";
}
```

**Return Value**: Returns empty list on error (after throwing).

---

## Methods: Global Configuration Management

### findGlobalConfigurations
```cpp
std::set<std::string> findGlobalConfigurations(std::string const& search) const
```

**Purpose**: Find global configuration names matching search criteria.

**Parameters**:
- `search` - Search pattern (empty string for all configurations, or specific name/pattern)

**Returns**: `std::set<std::string>` - Set of matching configuration names

**Throws**: `runtime_exception` on errors

**Usage Example**:
```cpp
ConfigurationInterface ifc;

// Find all global configurations
auto allConfigs = ifc.findGlobalConfigurations("");

// Find specific configuration
auto testConfigs = ifc.findGlobalConfigurations("TestRun*");

for (auto const& name : allConfigs) {
    std::cout << "Configuration: " << name << "\n";
}
```

**Note**: Global configurations are compositions of multiple configuration versions.

---

### loadGlobalConfiguration
```cpp
VersionInfoList_t loadGlobalConfiguration(std::string const& configuration) const
```

**Purpose**: Load composition of a global configuration.

**Parameters**:
- `configuration` - Global configuration name to load

**Returns**: `VersionInfoList_t` - List of VersionInfo structs describing the configuration composition

**Throws**: `runtime_exception` on errors

**Usage Example**:
```cpp
ConfigurationInterface ifc;

try {
    auto composition = ifc.loadGlobalConfiguration("Run12345Config");

    std::cout << "Configuration composition:\n";
    for (auto const& info : composition) {
        std::cout << "  Collection: " << info.configuration << "\n";
        std::cout << "  Version: " << info.version << "\n";
        std::cout << "  Entity: " << info.entity << "\n";
        std::cout << "  ---\n";
    }
} catch (std::exception const& e) {
    std::cerr << "Error loading: " << e.what() << "\n";
}
```

**Return Value**: Each VersionInfo contains:
- `configuration` - Collection/configuration type name
- `version` - Specific version identifier
- `entity` - Entity/component name

---

### storeGlobalConfiguration
```cpp
cf::result_t storeGlobalConfiguration(
    VersionInfoList_t const& versionInfoList,
    std::string const& configuration,
    bool allowOverwrite = false) const
```

**Purpose**: Store a global configuration as a composition of multiple configuration versions. By default, rejects duplicate composition names. Use `allowOverwrite=true` to update existing compositions.

**Parameters**:
- `versionInfoList` - List of configuration versions to include (must not be empty)
- `configuration` - Name for the global configuration (must not be empty)
- `allowOverwrite` - If true, allows updating existing compositions via merge-based overwrite

**Returns**: `result_t` - Success status and message

**Exception Safety**: Catches exceptions and returns error result

**Validation**:
- Configuration name must not be empty
- Version info list must not be empty
- Each VersionInfo must be valid (configuration, version, entity all non-empty)

**Overwrite Behavior** (when `allowOverwrite=true`):
- Performs merge-based overwrite rather than full replacement
- Unassigns old members that are no longer in the new composition
- Assigns only new members (skips unchanged ones)
- Efficient for incremental updates to large compositions

**Usage Example**:
```cpp
ConfigurationInterface ifc;

// Build composition
ConfigurationInterface::VersionInfoList_t composition;
composition.push_back({"ComponentConfigs", "v1.0", "DAQ1"});
composition.push_back({"ComponentConfigs", "v1.0", "DAQ2"});
composition.push_back({"BoardConfigs", "v2.3", "BoardReader1"});

// Store new composition
auto result = ifc.storeGlobalConfiguration(composition, "Run12345");

if (result.first) {
    std::cout << "Global configuration stored successfully\n";
}

// Update existing composition
composition.push_back({"TriggerConfigs", "v1.1", "Trigger1"});
result = ifc.storeGlobalConfiguration(composition, "Run12345", true);  // allowOverwrite
```

**Implementation**:
- Without overwrite: Creates JSON payload with multiple assign operations
- With overwrite: Computes diff between old and new members, then unassigns removed and assigns new

---

### storeGlobalConfiguration_mt
```cpp
cf::result_t storeGlobalConfiguration_mt(
    VersionInfoList_t const& versionInfoList,
    std::string const& configuration,
    bool overwrite = false) const
```

**Purpose**: Multi-threaded version of storeGlobalConfiguration for faster performance with large compositions.

**Parameters**:
- `versionInfoList` - List of configuration versions to include
- `configuration` - Name for the global configuration
- `overwrite` - If true, allows updating existing compositions

**Returns**: `result_t` - Merged results from all threads

**Performance**:
- Uses thread pool with size = min(versionInfoList.size(), hardware_concurrency/2)
- Each version assignment runs in parallel
- Unassignments (when overwriting) also run in parallel
- Results are merged and returned together

**Usage**:
```cpp
// Same as storeGlobalConfiguration but faster for large lists
auto result = ifc.storeGlobalConfiguration_mt(largeComposition, "Run12345");

// Overwrite with parallel execution
result = ifc.storeGlobalConfiguration_mt(updatedComposition, "Run12345", true);
```

**Thread Safety**: Creates new Multitasker instance per call - safe to call from multiple threads.

**When to Use**: Prefer this method when storing global configurations with many components (>10).

---

## Methods: Collection Management

### listCollections
```cpp
std::set<std::string> listCollections(std::string const& name_prefix) const
```

**Purpose**: List all configuration collections (types) in the database.

**Parameters**:
- `name_prefix` - Optional prefix filter (empty for all collections)

**Returns**: `std::set<std::string>` - Set of collection names

**Throws**: `runtime_exception` on errors

**Usage Example**:
```cpp
ConfigurationInterface ifc;

// List all collections
auto all = ifc.listCollections("");

// List collections starting with "Component"
auto filtered = ifc.listCollections("Component");

std::cout << "Collections:\n";
for (auto const& col : all) {
    std::cout << "  - " << col << "\n";
}
```

**Collections**: Typically include names like:
- `ComponentConfigs`
- `BoardConfigs`
- `DAQConfigs`
- etc.

---

### findGlobalConfigurationsContaining
```cpp
std::set<std::string> findGlobalConfigurationsContaining(
    std::string const& configurationType,
    std::string const& version,
    std::string const& entity = "") const
```

**Purpose**: Find all global configurations (compositions) that contain a specific configuration version. This provides reverse lookup capability for impact analysis.

**Parameters**:
- `configurationType` - Configuration type/collection name to search for (required, non-empty)
- `version` - Version identifier to match (required, non-empty)
- `entity` - Entity name to filter by (optional, empty string matches all)

**Returns**: `std::set<std::string>` - Set of global configuration names containing the specified member

**Throws**:
- `invalid_option_exception` - If configurationType or version is empty
- `runtime_exception` - On database errors

**Usage Example**:
```cpp
ConfigurationInterface ifc;

try {
    // Find which runs use TriggerConfig v2.1 for DAQ1
    auto compositions = ifc.findGlobalConfigurationsContaining(
        "TriggerConfig", "v2.1", "DAQ1"
    );

    std::cout << "Global configurations using TriggerConfig v2.1:\n";
    for (auto const& name : compositions) {
        std::cout << "  - " << name << "\n";
    }
} catch (std::exception const& e) {
    std::cerr << "Search failed: " << e.what() << "\n";
}
```

**Implementation Notes**:
- First attempts an optimized path using direct storage provider queries
- Falls back to brute-force search if optimized path fails
- Brute-force: loads all compositions and checks membership individually

**Use Cases**:
- Impact analysis before modifying a configuration version
- Finding all runs that used a specific configuration
- Auditing configuration usage across the system

---

## Safe Wrapper Methods

These methods provide exception-free alternatives to the throwing methods above. They return `result_t` instead of throwing exceptions, making them suitable for C-compatible APIs and contexts where exceptions must be avoided.

### getVersions_safe
```cpp
template <typename CONF>
cf::result_t getVersions_safe(CONF configuration,
                               std::string const& entity,
                               std::list<std::string>& versions) const noexcept
```

**Purpose**: Exception-safe version of `getVersions()`. Returns versions via output parameter instead of return value.

**Parameters**:
- `configuration` - Configuration object (used for collection name)
- `entity` - Entity name filter (empty for all)
- `versions` - Output parameter receiving the list of versions

**Returns**: `result_t` - Pair of (success flag, message)

**Postconditions**:
- On success: `versions` contains the version list
- On failure: `versions` is cleared

**Usage**:
```cpp
ConfigurationInterface ifc;
MyConfiguration config;
std::list<std::string> versions;

auto result = ifc.getVersions_safe(&config, "Entity1", versions);
if (result.first) {
    for (auto const& v : versions) {
        std::cout << "Version: " << v << "\n";
    }
} else {
    std::cerr << "Error: " << result.second << "\n";
}
```

---

### findGlobalConfigurations_safe
```cpp
cf::result_t findGlobalConfigurations_safe(
    std::string const& search,
    std::set<std::string>& configurations) const noexcept
```

**Purpose**: Exception-safe version of `findGlobalConfigurations()`.

**Parameters**:
- `search` - Search pattern (empty for all)
- `configurations` - Output parameter receiving matching configuration names

**Returns**: `result_t` - Success status and message

**Postconditions**:
- On success: `configurations` contains matching names
- On failure: `configurations` is cleared

---

### loadGlobalConfiguration_safe
```cpp
cf::result_t loadGlobalConfiguration_safe(
    std::string const& configuration,
    VersionInfoList_t& members) const noexcept
```

**Purpose**: Exception-safe version of `loadGlobalConfiguration()`.

**Parameters**:
- `configuration` - Global configuration name to load
- `members` - Output parameter receiving the composition members

**Returns**: `result_t` - Success status and message

**Postconditions**:
- On success: `members` contains the composition's VersionInfo list
- On failure: `members` is cleared

---

### listCollections_safe
```cpp
cf::result_t listCollections_safe(
    std::string const& name_prefix,
    std::set<std::string>& collections) const noexcept
```

**Purpose**: Exception-safe version of `listCollections()`.

**Parameters**:
- `name_prefix` - Optional prefix filter
- `collections` - Output parameter receiving collection names

**Returns**: `result_t` - Success status and message

---

### findGlobalConfigurationsContaining_safe
```cpp
cf::result_t findGlobalConfigurationsContaining_safe(
    std::string const& configurationType,
    std::string const& version,
    std::string const& entity,
    std::set<std::string>& compositions) const noexcept
```

**Purpose**: Exception-safe version of `findGlobalConfigurationsContaining()`.

**Parameters**:
- `configurationType` - Configuration type to search for
- `version` - Version to match
- `entity` - Entity filter (empty matches all)
- `compositions` - Output parameter receiving matching composition names

**Returns**: `result_t` - Success status and message

**Usage**:
```cpp
std::set<std::string> compositions;
auto result = ifc.findGlobalConfigurationsContaining_safe(
    "TriggerConfig", "v2.1", "", compositions
);

if (result.first) {
    std::cout << "Found " << compositions.size() << " compositions\n";
}
```

---

## Design Patterns

### Template-Based Serialization

The interface uses template methods to support different configuration types and formats:

```cpp
// Supports any serializable configuration type
template <typename CONF, typename TYPE>
cf::result_t storeVersion(CONF configuration, ...);
```

**Benefits**:
1. Type safety at compile time
2. No runtime overhead for type checking
3. Extensible to new configuration types
4. Supports both JSON and FHiCL formats

### Serializer Wrapper Pattern

```cpp
auto serializer = ConfigurationSerializer<CONF, MakeSerializable>::wrap(configuration);
auto collectionName = serializer.configurationName();
serializer.writeDocument<TYPE>(data);
```

**Purpose**: Abstracts serialization logic from the interface layer.

### noexcept Result Pattern

Template methods return `result_t` and are marked `noexcept`:

```cpp
template <typename CONF, typename TYPE>
cf::result_t storeVersion(...) const noexcept try {
    // ... implementation ...
} catch (std::exception const& e) {
    return {false, {e.what()}};
}
```

**Benefits**:
1. Never throws - safe for C-compatible APIs
2. Always returns result indicating success/failure
3. Error messages in return value
4. Exception-safe resource management

---

## Configuration Serialization Requirements

To use ConfigurationInterface with a custom configuration type, implement `MakeSerializable` specialization:

```cpp
template <>
class MakeSerializable<MyConfig*> {
 public:
  template <class TYPE>
  bool writeDocumentImpl(TYPE& data) const {
    // Serialize _conf to data (JsonData or FhiclData)
    return true;
  }

  template <class TYPE>
  bool readDocumentImpl(TYPE const& data) {
    // Deserialize data into _conf
    return true;
  }

  std::string configurationNameImpl() const {
    return "MyConfigs";  // Collection name
  }

 private:
  MyConfig* _conf;
};
```

---

## Usage Example: Complete Workflow

```cpp
#include "artdaq-database/ConfigurationDB/configurationdbifc.h"

// Custom configuration type
struct DAQConfiguration {
    std::string name;
    int run_number;
    std::vector<std::string> components;

    // Serialization methods...
};

// Use the interface
void example() {
    ConfigurationInterface ifc;
    DAQConfiguration config;
    config.name = "TestRun";
    config.run_number = 12345;
    config.components = {"DAQ1", "DAQ2"};

    // Store a version
    auto result = ifc.storeVersion<DAQConfiguration*, JsonData>(
        &config, "v1.0", "Production"
    );

    if (!result.first) {
        std::cerr << "Store failed: " << result.second << "\n";
        return;
    }

    // Load it back
    DAQConfiguration loaded;
    result = ifc.loadVersion<DAQConfiguration*, JsonData>(
        &loaded, "v1.0", "Production"
    );

    // List all versions
    auto versions = ifc.getVersions(&config, "Production");

    // Create global configuration
    ConfigurationInterface::VersionInfoList_t composition;
    composition.push_back({"DAQConfiguration", "v1.0", "Production"});

    result = ifc.storeGlobalConfiguration(composition, "Run12345");
}
```

---

## Thread Safety

- **ConfigurationInterface instances**: Not thread-safe - use one instance per thread
- **storeGlobalConfiguration_mt**: Uses internal thread pool - safe to call concurrently
- **Multitasker**: Should not be shared across threads (enforced at runtime)

---

## Error Handling

### Result Type
```cpp
using result_t = std::pair<bool, std::string>;
```

**Fields**:
- `first` - Success flag (true = success, false = failure)
- `second` - Message (success message or error description)

### Exception vs Result

**Methods returning `result_t`**: Never throw (noexcept), always return error in result

**Methods returning data**: May throw `runtime_exception` on errors

**Usage Pattern**:
```cpp
// Check result for methods returning result_t
auto result = ifc.storeVersion(...);
if (!result.first) {
    // Handle error: result.second contains message
}

// Use try/catch for methods returning data
try {
    auto versions = ifc.getVersions(...);
} catch (std::exception const& e) {
    // Handle error
}
```

---

## Related Files

- **configurationdbifc_base.h** - Base classes, serialization support
- **Multitasker.h** - Multi-threaded execution for parallel operations
- **options_operation_managedocument.h** - Document operation classes
- **options_operation_manageconfigs.h** - Configuration operation classes
- **JSONDocumentBuilder.h** - JSON document construction utilities

---

## Best Practices

### Version Naming
```cpp
// GOOD: Semantic versioning
ifc.storeVersion(..., "v1.0.0", "Component");
ifc.storeVersion(..., "v2.1.0", "Component");

// GOOD: Date-based versioning
ifc.storeVersion(..., "2025-01-15", "Component");

// AVOID: Generic names
ifc.storeVersion(..., "latest", "Component");  // Ambiguous
ifc.storeVersion(..., "new", "Component");     // Non-descriptive
```

### Entity Organization
```cpp
// Use meaningful entity names for component identification
ifc.storeVersion(..., "v1.0", "DAQComponent1");
ifc.storeVersion(..., "v1.0", "DAQComponent2");

// Or use hierarchical naming
ifc.storeVersion(..., "v1.0", "System1.DAQ.Component1");
```

### Global Configuration Management
```cpp
// Build compositions programmatically
ConfigurationInterface::VersionInfoList_t buildRunConfig(int run_number) {
    ConfigurationInterface::VersionInfoList_t list;

    // Add all components for this run
    for (auto const& component : getComponents(run_number)) {
        list.push_back({
            component.config_type,
            component.version,
            component.entity_name
        });
    }

    return list;
}

// Use multi-threaded version for large compositions
auto composition = buildRunConfig(12345);
if (composition.size() > 10) {
    result = ifc.storeGlobalConfiguration_mt(composition, "Run12345");
} else {
    result = ifc.storeGlobalConfiguration(composition, "Run12345");
}
```

### Error Handling
```cpp
// Always check results
auto result = ifc.storeVersion<Config*, JsonData>(&config, version, entity);
if (!result.first) {
    TLOG(TLVL_ERROR) << "Failed to store version: " << result.second;
    // Take corrective action
    return false;
}

// Use try/catch for data retrieval methods
try {
    auto versions = ifc.getVersions(&config, entity);
    // Process versions...
} catch (artdaq::database::runtime_exception const& e) {
    TLOG(TLVL_ERROR) << "Database error: " << e.what();
    return false;
} catch (std::exception const& e) {
    TLOG(TLVL_ERROR) << "Unexpected error: " << e.what();
    return false;
}
```

---

**Documentation generated for artdaq-database ConfigurationDB module**
