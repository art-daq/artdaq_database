# options_operation_manageconfigs.h

**Path:** `artdaq-database/ConfigurationDB/options_operation_manageconfigs.h`

**Purpose:** Declares the `ManageConfigsOperation` class which encapsulates options and parameters for global configuration management operations. This class extends `OperationBase` to provide handling for operations that work with named configurations (groups of related documents), including listing, creating, and assigning configurations.


## Key Concepts

### Global Configurations
A global configuration represents a named collection of document versions that together define a complete system configuration. Unlike document operations that target individual documents, configuration operations manage these higher-level groupings.

### Configuration vs Document Operations
- **Document operations** (ManageDocumentOperation): Work with individual configuration documents
- **Configuration operations** (ManageConfigsOperation): Work with named groupings of documents

### Options Hierarchy
This class inherits from `OperationBase` and adds configuration-specific parameters:
- `version` - Version identifier for filtering
- `entity` - Entity name for filtering
- `configuration` - Global configuration name

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not safe for concurrent modification; each thread should have its own instance
- **Locking:** No internal locking; callers must synchronize if sharing instances

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

### `ManageConfigsOperation`

A final class extending `OperationBase` that provides options handling for global configuration management operations. This is a lighter-weight version of `ManageDocumentOperation` focused on configuration-level operations.

**Thread Safety:** Not thread-safe; create separate instances per thread.

#### Constructor

##### `ManageConfigsOperation(std::string const& process_name)`

**Brief:** Constructs a ManageConfigsOperation with the specified process name for logging and identification.

**Parameters:**
- `process_name` - Name of the invoking process

**Preconditions:**
- None

**Postconditions:**
- All parameters initialized to `apiliteral::notprovided`
- Process name stored for logging

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/options_operation_manageconfigs.h"

using namespace artdaq::database::configuration;

void listConfigurations() {
  auto opts = ManageConfigsOperation{"conftool"};
  opts.operation("listconfigs");
  // Use opts for configuration listing
}
```

#### Methods

##### `version() const -> std::string const&`

**Brief:** Returns the current version filter string for this configuration operation.

**Preconditions:**
- Version should have been set (assertion fires if empty)

**Returns:** Reference to the version string

**Thread Safety:** Safe for concurrent reads if no concurrent writes

##### `version(std::string const& version) -> std::string const&`

**Brief:** Sets the version filter for configuration operations.

**Parameters:**
- `version` - Version identifier to filter by

**Preconditions:**
- `version` must not be empty

**Returns:** Reference to the newly set version

**Postconditions:**
- Internal version is updated
- TRACE log entry at level 20

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When `version` is empty |

**Thread Safety:** Not safe for concurrent access

##### `entity() const -> std::string const&`

**Brief:** Returns the current entity filter name for this configuration operation.

**Preconditions:**
- Entity should have been set (assertion fires if empty)

**Returns:** Reference to the entity name string

**Thread Safety:** Safe for concurrent reads if no concurrent writes

##### `entity(std::string const& entity) -> std::string const&`

**Brief:** Sets the entity filter for configuration operations.

**Parameters:**
- `entity` - Entity name to filter by

**Preconditions:**
- `entity` must not be empty

**Returns:** Reference to the newly set entity name

**Postconditions:**
- Internal entity is updated
- TRACE log entry at level 21

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When `entity` is empty |

**Thread Safety:** Not safe for concurrent access

##### `configuration() const -> std::string const&`

**Brief:** Returns the current global configuration name for this operation.

**Preconditions:**
- Configuration should have been set (assertion fires if empty)

**Returns:** Reference to the configuration name string

**Thread Safety:** Safe for concurrent reads if no concurrent writes

##### `configuration(std::string const& global_configuration) -> std::string const&`

**Brief:** Sets the global configuration name for this operation.

**Parameters:**
- `global_configuration` - Name of the global configuration

**Preconditions:**
- `global_configuration` must not be empty

**Returns:** Reference to the newly set configuration name

**Postconditions:**
- Internal configuration is updated
- TRACE log entry at level 22

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When `global_configuration` is empty |

**Thread Safety:** Not safe for concurrent access

**Example:**
```cpp
auto opts = ManageConfigsOperation{"myapp"};
opts.configuration("production_config");
opts.version("v1.0");
// Now ready for configuration operations
```

##### `query_filter_to_JsonData() const -> JsonData` (override)

**Brief:** Generates a JSON query filter for configuration searches based on current options.

**Preconditions:**
- None (returns appropriate filter based on set options)

**Returns:** JsonData containing the query filter

**Postconditions:**
- Returns only non-default parameters
- Configuration filter excluded for "assignconfig" operations

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

**Thread Safety:** Safe for concurrent reads if no concurrent writes

##### `configuration_to_JsonData() const -> JsonData`

**Brief:** Creates a JSON object with the configuration name using the filter configurations key.

**Returns:** JsonData with format `{"configurations": "<configuration>"}`

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

##### `version_to_JsonData() const -> JsonData`

**Brief:** Creates a JSON object containing just the version with a "name" key.

**Returns:** JsonData with format `{"name": "<version>"}`

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

##### `entity_to_JsonData() const -> JsonData`

**Brief:** Creates a JSON object containing just the entity name with a "name" key.

**Returns:** JsonData with format `{"name": "<entity>"}`

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

##### `makeProgramOptions() const -> bpo::options_description` (override)

**Brief:** Creates Boost.Program_Options description for configuration operation command-line options.

**Returns:** Options description including -v/--version, -e/--entity, -g/--configuration, -s/--source

**Thread Safety:** Safe (creates new object)

##### `readProgramOptions(bpo::variables_map const& vm) -> int` (override)

**Brief:** Reads parsed command-line options and sets corresponding operation parameters.

**Parameters:**
- `vm` - Boost.Program_Options variables map

**Returns:** `process_exit_code::SUCCESS` on success, or error code

**Thread Safety:** Not safe for concurrent access

##### `readJsonData(JsonData const& data) -> void` (override)

**Brief:** Parses JSON input data to populate operation parameters.

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

**Brief:** Serializes current operation parameters to JSON.

**Returns:** JsonData containing all non-default parameters

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

**Thread Safety:** Safe for concurrent reads if no concurrent writes

## Functions

### `debug::options::ManageConfigs() -> void`

**Brief:** Enables TRACE debugging for ManageConfigsOperation parsing and serialization.

**Side Effects:**
- Configures TRACE logging for this module

## TraceStreamer Operator

### `operator<<(TraceStreamer&, ManageConfigsOperation const&) -> TraceStreamer&`

**Brief:** Stream insertion operator for logging ManageConfigsOperation to TraceStreamer.

**Parameters:**
- First parameter: TraceStreamer reference
- Second parameter: ManageConfigsOperation to serialize

**Returns:** Reference to TraceStreamer for chaining

## Relationship to Other Components

`ManageConfigsOperation` is simpler than `ManageDocumentOperation`, lacking:
- `run` parameter
- `sourceFileName` parameter
- Various JSON serialization helpers

Use this class when:
- Listing available configurations
- Assigning documents to configurations
- Querying configuration metadata

Use `ManageDocumentOperation` when:
- Reading/writing individual documents
- Specifying source files for import/export
- Working with run numbers

## See Also

- [options_operation_manageconfigs.cpp](./options_operation_manageconfigs.cpp.md) - Implementation
- [options_operation_base.h](./options_operation_base.h.md) - Base class
- [options_operation_managedocument.h](./options_operation_managedocument.h.md) - More comprehensive document operations
- [options_operation_managealiases.h](./options_operation_managealiases.h.md) - Alias management operations

## Notes for Developers

### Common Pitfalls

- **Wrong class choice:** Use `ManageDocumentOperation` if you need run numbers or source files
- **Assertion failures:** Getters assert on unset values; always set before getting

### Usage Pattern

```cpp
// Typical configuration listing flow
auto opts = ManageConfigsOperation{"conftool"};
opts.operation("listconfigs");
opts.provider("filesystem");
opts.collection("artdaq_configurations");

// Optional filters
if (needVersionFilter) {
  opts.version("v2.0");
}

auto filter = opts.query_filter_to_JsonData();
// Use filter to query database
```

---

**Documentation generated for artdaq-database ConfigurationDB module**
