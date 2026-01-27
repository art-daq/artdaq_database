# options_operation_managealiases.h

**Path:** `artdaq-database/ConfigurationDB/options_operation_managealiases.h`

**Purpose:** Declares the `ManageAliasesOperation` class which encapsulates options and parameters for alias management operations in the configuration database. This class extends `OperationBase` to handle operations involving version aliases, configuration aliases, and run associations.


## Key Concepts

### Aliases in Configuration Management
Aliases provide human-readable names that point to specific versions or configurations:
- **Version alias:** A named pointer to a specific document version (e.g., "production" -> "v2.3.1")
- **Configuration alias:** A named pointer to a specific global configuration (e.g., "current" -> "config_20231215")
- **Run association:** Links a run number to a specific configuration state

### Use Cases
- Tagging a version as "production" or "testing"
- Creating meaningful names for configuration snapshots
- Associating run numbers with the configurations used during data taking

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not safe for concurrent modification
- **Locking:** No internal locking; callers must synchronize access

## Dependencies

| Include | Purpose |
|---------|---------|
| `options_operation_base.h` | Base class `OperationBase` providing common operation functionality |
| `<boost/program_options.hpp>` | Command-line argument parsing infrastructure |

## Forward Declarations

| Type | Purpose |
|------|---------|
| `artdaq::database::basictypes::JsonData` | JSON data wrapper type |
| `TraceStreamer` | TRACE logging support for stream operators |

## Classes/Structures

### `ManageAliasesOperation`

A final class extending `OperationBase` that provides options handling for alias management operations. This class supports both version-level aliases and configuration-level aliases, plus run associations.

**Thread Safety:** Not thread-safe; create separate instances per thread.

#### Constructor

##### `ManageAliasesOperation(std::string const& process_name)`

**Brief:** Constructs a ManageAliasesOperation with the specified process name for logging and identification.

**Parameters:**
- `process_name` - Name of the invoking process

**Preconditions:**
- None

**Postconditions:**
- All parameters initialized to `apiliteral::notprovided`

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/options_operation_managealiases.h"

using namespace artdaq::database::configuration;

void addVersionAlias() {
  auto opts = ManageAliasesOperation{"conftool"};
  opts.operation("addversionalias");
  opts.entity("BoardReader01");
  opts.version("v1.2.3");
  opts.versionAlias("production");
  // Execute alias creation
}
```

#### Methods

##### `version() const -> std::string const&`

**Brief:** Returns the target version for alias operations.

**Preconditions:**
- Version should have been set (assertion fires if empty)

**Returns:** Reference to the version string

**Thread Safety:** Safe for concurrent reads if no concurrent writes

##### `version(std::string const& version) -> std::string const&`

**Brief:** Sets the target version that an alias will point to.

**Parameters:**
- `version` - Version identifier to target

**Preconditions:**
- `version` must not be empty

**Returns:** Reference to the newly set version

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When `version` is empty |

**Thread Safety:** Not safe for concurrent access

##### `versionAlias() const -> std::string const&`

**Brief:** Returns the version alias name for this operation.

**Preconditions:**
- Version alias should have been set (assertion fires if empty)

**Returns:** Reference to the version alias string

**Thread Safety:** Safe for concurrent reads if no concurrent writes

##### `versionAlias(std::string const& alias) -> std::string const&`

**Brief:** Sets the version alias name to create or query.

**Parameters:**
- `alias` - Human-readable alias name (e.g., "production", "testing")

**Preconditions:**
- `alias` must not be empty

**Returns:** Reference to the newly set alias

**Postconditions:**
- Internal version alias is updated
- TRACE log entry at level 20

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When `alias` is empty |

**Thread Safety:** Not safe for concurrent access

**Example:**
```cpp
auto opts = ManageAliasesOperation{"myapp"};
opts.versionAlias("stable");
opts.version("v2.0.1");
// "stable" will now point to "v2.0.1"
```

##### `entity() const -> std::string const&`

**Brief:** Returns the entity name for entity-scoped alias operations.

**Preconditions:**
- Entity should have been set (assertion fires if empty)

**Returns:** Reference to the entity name

**Thread Safety:** Safe for concurrent reads if no concurrent writes

##### `entity(std::string const& entity) -> std::string const&`

**Brief:** Sets the entity name for scoping alias operations.

**Parameters:**
- `entity` - Name of the configurable entity

**Preconditions:**
- `entity` must not be empty

**Returns:** Reference to the newly set entity

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When `entity` is empty |

**Thread Safety:** Not safe for concurrent access

##### `run() const -> std::string const&`

**Brief:** Returns the run identifier for run-based operations.

**Preconditions:**
- Run should have been set (assertion fires if empty)

**Returns:** Reference to the run identifier

**Thread Safety:** Safe for concurrent reads if no concurrent writes

##### `run(std::string const& run) -> std::string const&`

**Brief:** Sets the run identifier for run association operations.

**Parameters:**
- `run` - Run number or identifier

**Preconditions:**
- `run` must not be empty

**Returns:** Reference to the newly set run

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When `run` is empty |

**Thread Safety:** Not safe for concurrent access

##### `configuration() const -> std::string const&`

**Brief:** Returns the global configuration name for configuration-level alias operations.

**Preconditions:**
- Configuration should have been set (assertion fires if empty)

**Returns:** Reference to the configuration name

**Thread Safety:** Safe for concurrent reads if no concurrent writes

##### `configuration(std::string const& global_configuration) -> std::string const&`

**Brief:** Sets the global configuration name that an alias will point to.

**Parameters:**
- `global_configuration` - Name of the global configuration

**Preconditions:**
- `global_configuration` must not be empty

**Returns:** Reference to the newly set configuration

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When `global_configuration` is empty |

**Thread Safety:** Not safe for concurrent access

##### `configurationAlias() const -> std::string const&`

**Brief:** Returns the configuration alias name for this operation.

**Preconditions:**
- Configuration alias should have been set (assertion fires if empty)

**Returns:** Reference to the configuration alias string

**Thread Safety:** Safe for concurrent reads if no concurrent writes

##### `configurationAlias(std::string const& global_configuration_alias) -> std::string const&`

**Brief:** Sets the configuration alias name to create or query.

**Parameters:**
- `global_configuration_alias` - Human-readable alias for a configuration (e.g., "current", "backup")

**Preconditions:**
- `global_configuration_alias` must not be empty

**Returns:** Reference to the newly set configuration alias

**Postconditions:**
- Internal configuration alias is updated
- TRACE log entry at level 20

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When `global_configuration_alias` is empty |

**Thread Safety:** Not safe for concurrent access

##### `query_filter_to_JsonData() const -> JsonData` (override)

**Brief:** Generates a JSON query filter for alias searches, with special handling for certain operation types.

**Preconditions:**
- None

**Returns:** JsonData containing the query filter

**Postconditions:**
- Version alias filter excluded for "addversionalias" operations
- Run filter excluded for "addrun" operations
- Configuration filter excluded for "assignconfig" operations

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

**Thread Safety:** Safe for concurrent reads if no concurrent writes

##### `versionAlias_to_JsonData() const -> JsonData`

**Brief:** Creates a JSON object containing the version alias with a "name" key.

**Returns:** JsonData with format `{"name": "<version_alias>"}`

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

##### `run_to_JsonData() const -> JsonData`

**Brief:** Creates a JSON object containing the run identifier with a "name" key.

**Returns:** JsonData with format `{"name": "<run>"}`

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

##### `makeProgramOptions() const -> bpo::options_description` (override)

**Brief:** Creates Boost.Program_Options description for alias operation command-line options.

**Returns:** Options description including:
- `-v/--version` - Target version
- `-a/--version_alias` - Version alias name
- `-e/--entity` - Entity name
- `-g/--configuration` - Configuration name
- `-q/--configuration_alias` - Configuration alias name
- `-r/--run` - Run identifier

**Thread Safety:** Safe (creates new object)

##### `readProgramOptions(bpo::variables_map const& vm) -> int` (override)

**Brief:** Reads parsed command-line options and sets corresponding operation parameters.

**Parameters:**
- `vm` - Boost.Program_Options variables map

**Returns:** `process_exit_code::SUCCESS` on success, or error code

**Thread Safety:** Not safe for concurrent access

##### `readJsonData(JsonData const& data) -> void` (override)

**Brief:** Parses JSON input data to populate all alias operation parameters.

**Parameters:**
- `data` - JSON data containing operation parameters

**Preconditions:**
- `data` must not be empty

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON cannot be parsed |

**Thread Safety:** Not safe for concurrent access

##### `writeJsonData() const -> JsonData` (override)

**Brief:** Serializes all current alias operation parameters to JSON.

**Returns:** JsonData containing all non-default parameters

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

**Thread Safety:** Safe for concurrent reads if no concurrent writes

## Functions

### `debug::options::ManageAliases() -> void`

**Brief:** Enables TRACE debugging for ManageAliasesOperation parsing and serialization.

**Side Effects:**
- Configures TRACE logging for this module

## TraceStreamer Operator

### `operator<<(TraceStreamer&, ManageAliasesOperation const&) -> TraceStreamer&`

**Brief:** Stream insertion operator for logging ManageAliasesOperation to TraceStreamer.

**Parameters:**
- First parameter: TraceStreamer reference
- Second parameter: ManageAliasesOperation to serialize

**Returns:** Reference to TraceStreamer for chaining

## Relationship to Other Components

`ManageAliasesOperation` provides unique capabilities not found in sibling classes:
- **Version aliases** - Tagging specific versions with names
- **Configuration aliases** - Tagging specific configurations with names
- **Run associations** - Linking runs to configurations

The class is used by:
- `conftool` CLI for alias management commands
- Configuration API for programmatic alias operations
- Run management subsystems

## See Also

- [options_operation_managealiases.cpp](./options_operation_managealiases.cpp.md) - Implementation
- [options_operation_base.h](./options_operation_base.h.md) - Base class
- [options_operation_managedocument.h](./options_operation_managedocument.h.md) - Document operations
- [dboperation_managealiases.h](./dboperation_managealiases.h.md) - Alias database operations

## Notes for Developers

### Common Pitfalls

- **Filter exclusions:** The query filter generation has special cases for certain operations - be aware when adding new operations
- **Alias vs target confusion:** Don't confuse `version` (target) with `versionAlias` (the alias name)

### Typical Usage Patterns

```cpp
// Adding a version alias
ManageAliasesOperation opts{"conftool"};
opts.operation("addversionalias");
opts.entity("MyComponent");
opts.version("v1.2.3");        // The target version
opts.versionAlias("stable");   // The alias name

// Querying by alias
ManageAliasesOperation query{"conftool"};
query.operation("findversionalias");
query.entity("MyComponent");
query.versionAlias("stable");
// Returns the version that "stable" points to
```

---

**Documentation generated for artdaq-database ConfigurationDB module**
