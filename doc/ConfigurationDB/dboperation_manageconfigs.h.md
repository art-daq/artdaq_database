# dboperation_manageconfigs.h

**Path:** `artdaq-database/ConfigurationDB/dboperation_manageconfigs.h`

**Purpose:** Declares functions for managing global configurations (compositions) in the configuration database. Global configurations are named groupings of specific configuration versions that together define a complete system configuration for a data acquisition run.


## Key Concepts

### Global Configurations (Compositions)

A **global configuration** is a named composition that links together multiple configuration versions to define a complete system state. This is essential for managing complex data acquisition systems where multiple components need coordinated configurations.

Each composition:
- Has a unique name (e.g., "Run12345", "Production2024", "TestSetup01")
- Contains references to specific versions of different configuration types
- Each reference includes: collection name, version identifier, and entity name

### Use Case Example

For a particle physics experiment, a global configuration might include:
- Detector component 1: v2.1 of "TriggerConfig"
- Detector component 2: v3.0 of "ReadoutConfig"
- DAQ controller: v1.5 of "ControllerConfig"

All these are bundled together under a name like "Run2024_Physics" for reproducibility.

### Configuration Operations

- **Find**: Search for existing global configurations by name pattern
- **Create**: Create a new global configuration with multiple members
- **Assign**: Add a configuration version to an existing composition
- **Remove**: Remove a configuration version from a composition
- **Composition**: Get the complete list of members in a global configuration

### Two-Namespace Pattern

Operations are declared in two namespaces:
- **`opts`**: Takes `ManageDocumentOperation` objects directly - for programmatic access
- **`json`**: Takes JSON string payloads - for REST API and CLI compatibility

## Thread Safety

- **Thread-safe:** Conditional
- **Concurrent access:** Concurrent modifications to the same global configuration may result in race conditions
- **Locking:** No internal locking; callers must provide external synchronization if needed

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/SharedCommon/returned_result.h` | Provides `result_t` type definition for return values |

## Functions

### Namespace: opts

Functions in this namespace accept `ManageDocumentOperation` objects containing all operation parameters.

---

#### `find_configurations(options) -> result_t`

**Brief:** Searches for global configurations matching the specified criteria. Returns a list of configuration names that match the search pattern.

**Parameters:**
- `options` - A `ManageDocumentOperation` object containing search criteria:
  - `configuration`: Search pattern (use "*" for all, or partial name with wildcards)
  - `format`: Output format (typically `gui`)

**Preconditions:**
- The `options` object must be properly initialized with the operation name

**Returns:** `result_t` where:
- On success: `{true, json_array}` containing matching configuration names
- On failure: `{false, error_message}`

**Postconditions:**
- The returned JSON array contains all matching global configuration names

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Safe - read-only operation

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_manageconfigs.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include <iostream>

namespace cf = artdaq::database::configuration;

void listConfigurations() {
  cf::ManageDocumentOperation opts{"findconfigs"};
  opts.configuration("Run*");  // Find all configs starting with "Run"

  auto result = cf::opts::find_configurations(opts);
  if (result.first) {
    std::cout << "Found configurations: " << result.second << std::endl;
    // Output: ["Run12345", "Run12346", "Run99999"]
  } else {
    std::cerr << "Error: " << result.second << std::endl;
  }
}
```

---

#### `assign_configuration(options) -> result_t`

**Brief:** Assigns a specific configuration version to an existing global configuration. This adds or updates a member in the composition.

**Parameters:**
- `options` - A `ManageDocumentOperation` object containing:
  - `configuration`: Global configuration name
  - `collection`: Configuration type/collection to assign
  - `version`: Version identifier to assign
  - `entity`: Entity name

**Preconditions:**
- The global configuration must exist (or will be created)
- The referenced configuration version must exist in the database

**Returns:** `result_t` where:
- On success: `{true, confirmation_message}`
- On failure: `{false, error_message}`

**Postconditions:**
- On success, the specified version is now part of the global configuration
- If the collection was already assigned, it is updated to the new version

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional - not safe for concurrent modifications to the same configuration

**Side Effects:**
- Modifies the global configuration record in the database

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_manageconfigs.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include <iostream>

namespace cf = artdaq::database::configuration;

void assignToConfiguration() {
  cf::ManageDocumentOperation opts{"assignconfig"};
  opts.configuration("Run12345");
  opts.collection("TriggerConfig");
  opts.version("v2.1");
  opts.entity("Detector1");

  auto result = cf::opts::assign_configuration(opts);
  if (result.first) {
    std::cout << "Configuration assigned successfully" << std::endl;
  } else {
    std::cerr << "Error: " << result.second << std::endl;
  }
}
```

---

#### `remove_configuration(options) -> result_t`

**Brief:** Removes a configuration type from a global configuration. The underlying configuration document is not deleted, only its association with the global configuration.

**Parameters:**
- `options` - A `ManageDocumentOperation` object containing:
  - `configuration`: Global configuration name
  - `collection`: Configuration type/collection to remove

**Preconditions:**
- The global configuration must exist
- The collection must be assigned to the global configuration

**Returns:** `result_t` where:
- On success: `{true, confirmation_message}`
- On failure: `{false, error_message}`

**Postconditions:**
- On success, the collection is no longer part of the global configuration

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional - not safe for concurrent modifications to the same configuration

**Side Effects:**
- Modifies the global configuration record in the database

---

#### `create_configuration(options) -> result_t`

**Brief:** Creates a new global configuration. This operation is typically used via the JSON interface with an array of member assignments.

**Parameters:**
- `options` - A `ManageDocumentOperation` object (currently not fully implemented)

**Returns:** `result_t` where:
- Currently returns: `{false, "Not Implemented"}`

**Note:** The opts version is not implemented. Use `json::create_configuration()` instead, which accepts a JSON array of operations.

**Thread Safety:** N/A - not implemented

---

#### `configuration_composition(options) -> result_t`

**Brief:** Retrieves the complete composition (member list) of a global configuration. Returns all configuration versions that make up the specified global configuration.

**Parameters:**
- `options` - A `ManageDocumentOperation` object containing:
  - `configuration`: Global configuration name

**Preconditions:**
- The global configuration must exist

**Returns:** `result_t` where:
- On success: `{true, json_object}` containing all member versions with their collection, version, and entity information
- On failure: `{false, error_message}`

**Postconditions:**
- The returned JSON contains complete information about all members

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Safe - read-only operation

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_manageconfigs.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include <iostream>

namespace cf = artdaq::database::configuration;

void getComposition() {
  cf::ManageDocumentOperation opts{"confcomposition"};
  opts.configuration("Run12345");

  auto result = cf::opts::configuration_composition(opts);
  if (result.first) {
    std::cout << "Composition: " << result.second << std::endl;
    // Output: { "members": [
    //   {"collection": "TriggerConfig", "version": "v2.1", "entity": "Detector1"},
    //   {"collection": "ReadoutConfig", "version": "v3.0", "entity": "DAQ1"}
    // ]}
  } else {
    std::cerr << "Error: " << result.second << std::endl;
  }
}
```

---

### Namespace: json

Functions in this namespace accept JSON string payloads for REST API compatibility.

---

#### `find_configurations(task_payload) -> result_t`

**Brief:** Searches for global configurations using a JSON payload to specify criteria.

**Parameters:**
- `task_payload` - JSON string containing search parameters:
  - `configuration`: (Optional) Search pattern for configuration names

**Returns:** `result_t` with JSON array of configuration names on success

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Safe - read-only operation

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_manageconfigs.h"
#include <iostream>

namespace cf = artdaq::database::configuration;

void findConfigsViaJson() {
  std::string payload = R"({
    "configuration": "Run*"
  })";

  auto result = cf::json::find_configurations(payload);
  if (result.first) {
    std::cout << "Found: " << result.second << std::endl;
  } else {
    std::cerr << "Error: " << result.second << std::endl;
  }
}
```

---

#### `assign_configuration(task_payload) -> result_t`

**Brief:** Assigns a configuration version to a global configuration using a JSON payload.

**Parameters:**
- `task_payload` - JSON string containing:
  - `configuration`: Global configuration name
  - `collection`: Configuration type
  - `version`: Version to assign
  - `entity`: Entity name

**Preconditions:**
- `task_payload` must not be empty

**Returns:** `result_t` indicating success or failure

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_manageconfigs.h"
#include <iostream>

namespace cf = artdaq::database::configuration;

void assignViaJson() {
  std::string payload = R"({
    "configuration": "Run12345",
    "collection": "TriggerConfig",
    "version": "v2.1",
    "entity": "Detector1"
  })";

  auto result = cf::json::assign_configuration(payload);
  if (result.first) {
    std::cout << "Assigned successfully" << std::endl;
  } else {
    std::cerr << "Error: " << result.second << std::endl;
  }
}
```

---

#### `remove_configuration(task_payload) -> result_t`

**Brief:** Removes a configuration type from a global configuration using a JSON payload.

**Parameters:**
- `task_payload` - JSON string specifying the configuration and collection to remove

**Preconditions:**
- `task_payload` must not be empty

**Returns:** `result_t` indicating success or failure

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional

---

#### `create_configuration(task_payload) -> result_t`

**Brief:** Creates a new global configuration from a JSON array of operations. This is the preferred method for creating global configurations with multiple members.

**Parameters:**
- `task_payload` - JSON string containing an array of assign operations

**Preconditions:**
- `task_payload` must not be empty
- The JSON must contain valid operation specifications

**Returns:** `result_t` where:
- On success: `{true, result_data}`
- On failure: `{false, error_message}`

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_manageconfigs.h"
#include <iostream>

namespace cf = artdaq::database::configuration;

void createGlobalConfig() {
  std::string payload = R"([
    {
      "operation": "assignconfig",
      "configuration": "Run12345",
      "collection": "TriggerConfig",
      "version": "v2.1",
      "entity": "Detector1"
    },
    {
      "operation": "assignconfig",
      "configuration": "Run12345",
      "collection": "ReadoutConfig",
      "version": "v3.0",
      "entity": "DAQ1"
    }
  ])";

  auto result = cf::json::create_configuration(payload);
  if (result.first) {
    std::cout << "Global configuration created successfully" << std::endl;
  } else {
    std::cerr << "Error: " << result.second << std::endl;
  }
}
```

---

#### `configuration_composition(task_payload) -> result_t`

**Brief:** Gets the composition of a global configuration using a JSON payload.

**Parameters:**
- `task_payload` - JSON string specifying the configuration name

**Returns:** `result_t` with composition details on success

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Safe - read-only operation

---

### Namespace: debug

#### `ManageConfigs() -> void`

**Brief:** Enables TRACE logging for all global configuration management operations. Activates verbose debug output for troubleshooting configuration composition issues.

**Thread Safety:** Not thread-safe - should be called during initialization only

**Side Effects:**
- Configures TRACE logging system
- Sets all trace levels to maximum verbosity

---

#### `detail::ManageConfigs() -> void`

**Brief:** Enables TRACE logging for internal detail functions. Provides more granular debug output.

**Thread Safety:** Not thread-safe - should be called during initialization only

## Relationship to Other Components

```
+-------------------------+
|  ConfigurationInterface |
+-----------+-------------+
            |
            v
+-------------------------+
| dboperation_manageconfigs |
| (this file)              |
+-----------+-------------+
            |
            v
+-------------------------+
| detail_manageconfigs.cpp |
+-----------+-------------+
            |
            v
+-------------------------+
|   Storage Providers     |
| (FileSystemDB, MongoDB) |
+-------------------------+
```

- **ManageDocumentOperation**: Options class that holds all parameters for configuration operations
- **ConfigurationInterface**: High-level API that uses these functions for `storeGlobalConfiguration` and `loadGlobalConfiguration`
- **detail_manageconfigs.cpp**: Contains the actual implementation logic
- **dispatch_*.h**: Provider-specific implementations (FileSystemDB, MongoDB)

## Notes for Developers

### Key Behaviors

1. **Atomic Operations**: `assign_configuration` and `remove_configuration` are individual operations; use `create_configuration` for batch creation
2. **Overwrite Handling**: By default, assigning to an existing collection in a composition updates it
3. **Composition Structure**: Each member has a `{collection, version, entity}` tuple
4. **Result Format**: Returns JSON arrays or objects depending on the operation

### Common Pitfalls

- **Empty configuration name**: Operations will fail if the configuration name is not specified
- **Missing version**: Assigning a non-existent version will fail
- **opts::create_configuration**: This function is not implemented; use the JSON version instead

### Anti-patterns

```cpp
// DON'T do this - create_configuration is not implemented in opts namespace
auto result = cf::opts::create_configuration(opts);  // Returns "Not Implemented"

// DO this instead - use the json namespace version
auto result = cf::json::create_configuration(jsonPayload);
```

```cpp
// DON'T do this - no error handling
auto result = cf::opts::assign_configuration(opts);
// Immediately proceed without checking

// DO this instead - always check result
auto result = cf::opts::assign_configuration(opts);
if (!result.first) {
  std::cerr << "Assignment failed: " << result.second << std::endl;
  return;
}
```

## See Also

- [dboperation_manageconfigs.cpp.md](./dboperation_manageconfigs.cpp.md) - Implementation details
- [detail_manageconfigs.cpp.md](./detail_manageconfigs.cpp.md) - Internal implementation
- [configurationdbifc.h.md](./configurationdbifc.h.md) - High-level API that uses these functions
- [dboperation_managedocument.h.md](./dboperation_managedocument.h.md) - Individual document operations
- [returned_result.h.md](../SharedCommon/returned_result.h.md) - result_t type definition

---

**Documentation generated for artdaq-database ConfigurationDB module**
