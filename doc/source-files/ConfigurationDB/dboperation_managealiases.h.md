# dboperation_managealiases.h

**Path:** `artdaq-database/ConfigurationDB/dboperation_managealiases.h`

**Purpose:** Declares functions for managing version aliases and configuration aliases in the configuration database. Aliases provide human-readable names for specific configuration versions or global configurations, making it easier to reference commonly used configurations by purpose rather than by version number.


## Key Concepts

### Version Aliases

Aliases are symbolic names that point to specific version identifiers. They provide a level of indirection that allows users to reference configurations by purpose rather than by version number.

Examples:
- `"production"` -> `"v2.3.1"` (the current production version)
- `"testing"` -> `"v2.4.0-rc1"` (release candidate being tested)
- `"stable"` -> `"v2.3.0"` (last known stable version)
- `"default"` -> `"v1.0.0"` (default configuration for new setups)

### Configuration Aliases

Similar to version aliases, but applied to global configurations (compositions). This allows referencing a complete system configuration by a meaningful name.

Examples:
- `"current_run"` -> `"Run12345"` (the currently active run configuration)
- `"last_good_run"` -> `"Run12340"` (last successful run)

### Operation Classes

This module uses two operation classes:
- **`ManageDocumentOperation`**: General-purpose options class for most operations
- **`ManageAliasesOperation`**: Specialized options class for alias-specific operations (used internally by JSON functions)

### Two-Namespace Pattern

Operations are declared in two namespaces:
- **`opts`**: Takes operation objects directly (`ManageDocumentOperation` or `ManageAliasesOperation`)
- **`json`**: Takes JSON string payloads - for REST API and CLI compatibility

## Thread Safety

- **Thread-safe:** Conditional
- **Concurrent access:** Concurrent modifications to the same alias may result in race conditions
- **Locking:** No internal locking; callers must provide external synchronization if needed

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/SharedCommon/returned_result.h` | Provides `result_t` type definition for return values |

## Functions

### Namespace: opts

Functions in this namespace accept operation objects containing all parameters.

---

#### `add_version_alias(options) -> result_t`

**Brief:** Adds a new version alias to a configuration version. Creates a symbolic name that points to a specific version, allowing users to reference the version by its alias.

**Parameters:**
- `options` - A `ManageDocumentOperation` object containing:
  - `collection`: Configuration collection name
  - `version`: Target version identifier to alias
  - `alias`: New alias name to create

**Preconditions:**
- The target version must exist in the collection
- The alias name must not already exist in the collection (or will be updated)

**Returns:** `result_t` where:
- On success: `{true, confirmation_message}`
- On failure: `{false, error_message}`

**Postconditions:**
- On success, the alias is created and points to the specified version
- The alias can be used in place of the version in subsequent operations

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional - not safe for concurrent modifications to the same alias

**Side Effects:**
- Creates or updates an alias record in the database

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_managealiases.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include <iostream>

namespace cf = artdaq::database::configuration;

void createProductionAlias() {
  cf::ManageDocumentOperation opts{"addversionalias"};
  opts.collection("DetectorConfigs");
  opts.version("v2.3.1");
  opts.alias("production");

  auto result = cf::opts::add_version_alias(opts);
  if (result.first) {
    std::cout << "Alias 'production' now points to v2.3.1" << std::endl;
  } else {
    std::cerr << "Error: " << result.second << std::endl;
  }
}
```

---

#### `remove_version_alias(options) -> result_t`

**Brief:** Removes an existing version alias from a collection. The underlying configuration version is not affected, only the alias is removed.

**Parameters:**
- `options` - A `ManageDocumentOperation` object containing:
  - `collection`: Configuration collection name
  - `alias`: Alias name to remove

**Preconditions:**
- The alias must exist in the collection

**Returns:** `result_t` where:
- On success: `{true, confirmation_message}`
- On failure: `{false, error_message}`

**Postconditions:**
- On success, the alias no longer exists
- Attempts to use the alias will fail

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional - not safe for concurrent modifications

**Side Effects:**
- Removes an alias record from the database

---

#### `find_version_aliases(options) -> result_t`

**Brief:** Finds all aliases defined in a collection. Optionally filters by version to find aliases pointing to a specific version.

**Parameters:**
- `options` - A `ManageDocumentOperation` object containing:
  - `collection`: Configuration collection name
  - `version`: (Optional) Filter to find aliases for a specific version

**Preconditions:**
- The collection must exist

**Returns:** `result_t` where:
- On success: `{true, json_array}` containing alias names and their target versions
- On failure: `{false, error_message}`

**Postconditions:**
- The returned JSON contains all matching aliases

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Safe - read-only operation

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_managealiases.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include <iostream>

namespace cf = artdaq::database::configuration;

void listAliases() {
  cf::ManageDocumentOperation opts{"findversionalias"};
  opts.collection("DetectorConfigs");

  auto result = cf::opts::find_version_aliases(opts);
  if (result.first) {
    std::cout << "Aliases: " << result.second << std::endl;
    // Output: [{"alias": "production", "version": "v2.3.1"},
    //          {"alias": "testing", "version": "v2.4.0-rc1"}]
  } else {
    std::cerr << "Error: " << result.second << std::endl;
  }
}
```

---

#### `add_configuration_alias(options) -> result_t`

**Brief:** Adds an alias to a global configuration. Creates a symbolic name that points to a specific global configuration name.

**Parameters:**
- `options` - A `ManageDocumentOperation` object containing:
  - `configuration`: Target global configuration name
  - `alias`: New alias name to create

**Preconditions:**
- The target global configuration must exist

**Returns:** `result_t` where:
- On success: `{true, confirmation_message}`
- On failure: `{false, error_message}`

**Postconditions:**
- On success, the alias points to the specified global configuration

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional

**Side Effects:**
- Creates or updates an alias record in the database

---

#### `remove_configuration_alias(options) -> result_t`

**Brief:** Removes an alias from a global configuration. The underlying global configuration is not affected.

**Parameters:**
- `options` - A `ManageDocumentOperation` object containing:
  - `alias`: Alias name to remove

**Preconditions:**
- The alias must exist

**Returns:** `result_t` where:
- On success: `{true, confirmation_message}`
- On failure: `{false, error_message}`

**Postconditions:**
- On success, the alias no longer exists

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional

**Side Effects:**
- Removes an alias record from the database

---

### Namespace: json

Functions in this namespace accept JSON string payloads for REST API compatibility.

---

#### `add_version_alias(task_payload) -> result_t`

**Brief:** Adds a version alias using a JSON payload to specify the operation parameters.

**Parameters:**
- `task_payload` - JSON string containing:
  - `collection`: Collection name
  - `version`: Target version
  - `alias`: Alias to create

**Preconditions:**
- `task_payload` must not be empty
- The JSON must contain required fields

**Returns:** `result_t` indicating success or failure

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dboperation_managealiases.h"
#include <iostream>

namespace cf = artdaq::database::configuration;

void addAliasViaJson() {
  std::string payload = R"({
    "collection": "DetectorConfigs",
    "version": "v2.3.1",
    "alias": "production"
  })";

  auto result = cf::json::add_version_alias(payload);
  if (result.first) {
    std::cout << "Alias created successfully" << std::endl;
  } else {
    std::cerr << "Error: " << result.second << std::endl;
  }
}
```

---

#### `remove_version_alias(task_payload) -> result_t`

**Brief:** Removes a version alias using a JSON payload.

**Parameters:**
- `task_payload` - JSON string specifying the collection and alias to remove

**Preconditions:**
- `task_payload` must not be empty

**Returns:** `result_t` indicating success or failure

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional

---

#### `find_version_aliases(task_payload) -> result_t`

**Brief:** Finds version aliases using a JSON payload to specify search criteria.

**Parameters:**
- `task_payload` - JSON string containing search parameters

**Preconditions:**
- `task_payload` must not be empty

**Returns:** `result_t` with JSON array of aliases on success

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Safe - read-only operation

---

#### `add_configuration_alias(task_payload) -> result_t`

**Brief:** Adds a configuration alias using a JSON payload.

**Parameters:**
- `task_payload` - JSON string containing configuration name and alias

**Preconditions:**
- `task_payload` must not be empty

**Returns:** `result_t` indicating success or failure

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional

---

#### `remove_configuration_alias(task_payload) -> result_t`

**Brief:** Removes a configuration alias using a JSON payload.

**Parameters:**
- `task_payload` - JSON string specifying the alias to remove

**Preconditions:**
- `task_payload` must not be empty

**Returns:** `result_t` indicating success or failure

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | All exceptions are caught internally; errors returned via `result_t` |

**Thread Safety:** Conditional

---

### Namespace: debug

#### `ManageAliases() -> void`

**Brief:** Enables TRACE logging for all alias management operations. Activates verbose debug output for troubleshooting alias creation and lookup issues.

**Thread Safety:** Not thread-safe - should be called during initialization only

**Side Effects:**
- Configures TRACE logging system
- Sets all trace levels to maximum verbosity

---

#### `detail::ManageAliases() -> void`

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
| dboperation_managealiases |
| (this file)              |
+-----------+-------------+
            |
            v
+-------------------------+
| detail_managealiases.cpp |
+-----------+-------------+
            |
            v
+-------------------------+
|   Storage Providers     |
| (FileSystemDB, MongoDB) |
+-------------------------+
```

- **ManageDocumentOperation**: Options class that holds parameters for alias operations
- **ManageAliasesOperation**: Specialized options class for alias-specific operations (used internally)
- **detail_managealiases.cpp**: Contains the actual implementation logic
- **dispatch_*.h**: Provider-specific implementations (FileSystemDB, MongoDB)
- **ConfigurationInterface**: High-level API that resolves aliases when loading configurations

## Notes for Developers

### Key Behaviors

1. **Alias Uniqueness**: Aliases must be unique within a collection (for version aliases) or globally (for configuration aliases)
2. **Alias Lifecycle**: Aliases can be updated by removing and re-adding with the same name
3. **Version Requirement**: The target version or configuration must exist before an alias can be created
4. **Alias Resolution**: When loading a configuration, aliases are automatically resolved to the actual version

### Common Pitfalls

- **Non-existent target**: Adding an alias to a non-existent version will fail
- **Duplicate aliases**: Behavior for duplicate aliases may vary by storage provider
- **Circular references**: Aliases cannot point to other aliases (they always resolve to actual versions)
- **Case sensitivity**: Alias names may be case-sensitive depending on the storage provider

### Anti-patterns

```cpp
// DON'T do this - alias points to non-existent version
opts.version("v99.99.99");  // Version doesn't exist
opts.alias("broken");
auto result = cf::opts::add_version_alias(opts);  // Will fail

// DO this instead - verify version exists first
opts.version("v2.3.1");  // Known to exist
opts.alias("production");
auto result = cf::opts::add_version_alias(opts);  // Success
```

### Update Pattern

To update an alias to point to a new version:

```cpp
#include "artdaq-database/ConfigurationDB/dboperation_managealiases.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"

namespace cf = artdaq::database::configuration;

void updateProductionAlias() {
  cf::ManageDocumentOperation opts;
  opts.collection("DetectorConfigs");

  // Remove the old alias
  opts.alias("production");
  auto removeResult = cf::opts::remove_version_alias(opts);
  if (!removeResult.first) {
    std::cerr << "Failed to remove old alias: " << removeResult.second << std::endl;
    return;
  }

  // Add it back pointing to new version
  opts.version("v2.4.0");
  opts.alias("production");
  auto addResult = cf::opts::add_version_alias(opts);
  if (!addResult.first) {
    std::cerr << "Failed to create new alias: " << addResult.second << std::endl;
    return;
  }

  std::cout << "Alias 'production' now points to v2.4.0" << std::endl;
}
```

### Implementation Notes

**Note:** Some functions declared in this header may not be implemented in the corresponding `.cpp` file:
- `opts::add_version_alias()` - May be missing
- `opts::remove_version_alias()` - May be missing
- `opts::find_version_aliases()` - May be missing
- `opts::add_configuration_alias()` - May be missing
- `opts::remove_configuration_alias()` - May be missing
- `json::add_configuration_alias()` - May be missing
- `json::remove_configuration_alias()` - May be missing

Check the implementation file for the current state of these functions.

## See Also

- [dboperation_managealiases.cpp.md](./dboperation_managealiases.cpp.md) - Implementation details
- [detail_managealiases.cpp.md](./detail_managealiases.cpp.md) - Internal implementation
- [dboperation_managedocument.h.md](./dboperation_managedocument.h.md) - Document operations that can use aliases
- [configurationdbifc.h.md](./configurationdbifc.h.md) - High-level API with alias resolution
- [returned_result.h.md](../SharedCommon/returned_result.h.md) - result_t type definition

---

**Documentation generated for artdaq-database ConfigurationDB module**
