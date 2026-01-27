# options_operation_manageconfigs.cpp

**Path:** `artdaq-database/ConfigurationDB/options_operation_manageconfigs.cpp`

**Implements:** [options_operation_manageconfigs.h](./options_operation_manageconfigs.h.md)

**Purpose:** Implements the `ManageConfigsOperation` class which handles options for global configuration management operations. This implementation provides getter/setter methods with validation, JSON serialization/deserialization, and command-line argument parsing for configuration-level operations.

## Implementation Overview

This file implements a streamlined version of operation options focused on global configuration management. Unlike `ManageDocumentOperation`, this class handles only the essential parameters needed for configuration-level operations: version, entity, and configuration name.

The implementation follows the same patterns as other option classes:
1. Validate inputs in setters (throw on empty values)
2. Use `confirm()` macro for debug assertions in getters
3. Serialize to/from JSON using the json_reader and json_writer utilities
4. Support both JSON input and command-line argument parsing

## Key Algorithms

### JSON Data Parsing

The `readJsonData()` method extracts configuration options from JSON:

**Steps:**
1. Call base class `readJsonData()` first to handle common options
2. Parse JSON buffer into AST using `JsonReader`
3. Extract configuration, version, entity from top-level
4. If search filter object exists, extract nested filter values
5. Use try/catch to handle missing optional fields gracefully

**Why this approach:** Allows the same JSON structure to work whether options are at the top level or nested in a search filter object.

### Query Filter Generation

The `query_filter_to_JsonData()` method builds database search criteria:

**Steps:**
1. Return raw query filter if one was explicitly set
2. Start with base class filter
3. Add version filter if provided
4. Add entity filter if provided
5. Add configuration filter unless operation is "assignconfig"
6. Return empty JSON `{}` if no filters specified

**Why this approach:** The "assignconfig" exclusion prevents circular references when assigning documents to a configuration.

## Internal Functions

### `ManageConfigsOperation::ManageConfigsOperation(std::string const& process_name)`

**Brief:** Constructor that initializes the operation by delegating to the base class with the process name.

**Called by:** Client code creating configuration management operations

**Purpose:** Establishes base operation state including process name for logging.

### `version() const -> std::string const&`

**Brief:** Returns the version filter string for this configuration operation.

**Called by:** `writeJsonData()`, `query_filter_to_JsonData()`, `version_to_JsonData()`

**Purpose:** Provides read access with debug assertion that value is set.

### `version(std::string const& version) -> std::string const&`

**Brief:** Sets the version filter with validation and logging.

**Called by:** `readJsonData()`, `readProgramOptions()`

**Purpose:** Updates version with empty-check validation.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When version string is empty |

### `entity() const -> std::string const&`

**Brief:** Returns the entity filter name for this configuration operation.

**Called by:** `writeJsonData()`, `query_filter_to_JsonData()`, `entity_to_JsonData()`

**Purpose:** Provides read access with debug assertion.

### `entity(std::string const& entity) -> std::string const&`

**Brief:** Sets the entity filter with validation and logging.

**Called by:** `readJsonData()`, `readProgramOptions()`

**Purpose:** Updates entity name with empty-check validation.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When entity string is empty |

### `configuration() const -> std::string const&`

**Brief:** Returns the global configuration name for this operation.

**Called by:** `writeJsonData()`, `query_filter_to_JsonData()`, `configuration_to_JsonData()`

**Purpose:** Provides read access with debug assertion.

### `configuration(std::string const& global_configuration) -> std::string const&`

**Brief:** Sets the global configuration name with validation and logging.

**Called by:** `readJsonData()`, `readProgramOptions()`

**Purpose:** Updates configuration name with empty-check validation.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When configuration string is empty |

### `readJsonData(JsonData const& data) -> void`

**Brief:** Parses JSON input to populate operation parameters, handling both top-level options and nested search filters.

**Called by:** API entry points receiving JSON operation requests

**Purpose:** Deserializes JSON to internal state.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON parsing fails |

### `readProgramOptions(bpo::variables_map const& vm) -> int`

**Brief:** Reads command-line arguments from a parsed variables map and sets operation parameters.

**Called by:** CLI tools processing command-line arguments

**Purpose:** Maps CLI arguments to internal state.

**Returns:** `process_exit_code::SUCCESS` or error code from base class

### `makeProgramOptions() const -> bpo::options_description`

**Brief:** Creates command-line option descriptions for configuration operations.

**Called by:** CLI tools building help and argument parsers

**Purpose:** Defines the CLI interface: -v/--version, -e/--entity, -g/--configuration, -s/--source

### `writeJsonData() const -> JsonData`

**Brief:** Serializes all operation parameters to JSON format.

**Called by:** Methods needing to transmit or store operation state

**Purpose:** Creates JSON with configuration, entity, and version fields.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

### `query_filter_to_JsonData() const -> JsonData`

**Brief:** Generates a JSON query filter for database searches based on current filter options.

**Called by:** Database query methods

**Purpose:** Constructs search criteria for storage provider queries.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

### `configuration_to_JsonData() const -> JsonData`

**Brief:** Creates a JSON object with configuration name keyed by the filter configurations literal.

**Called by:** Operations filtering by configuration

**Purpose:** Provides filter-compatible configuration name JSON.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

### `version_to_JsonData() const -> JsonData`

**Brief:** Creates a simple JSON object with version as the "name" field.

**Called by:** Operations needing minimal version identification

**Purpose:** Provides simple JSON version wrapper.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

### `entity_to_JsonData() const -> JsonData`

**Brief:** Creates a simple JSON object with entity as the "name" field.

**Called by:** Operations needing minimal entity identification

**Purpose:** Provides simple JSON entity wrapper.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

### `cf::debug::options::ManageConfigs() -> void`

**Brief:** Enables TRACE debugging for configuration option handling.

**Called by:** Debug/diagnostic code

**Purpose:** Configures TRACE logging for troubleshooting.

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not safe for concurrent modification
- **Locking:** No internal synchronization

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | TRACE logging macros |
| `artdaq-database/ConfigurationDB/options_operation_manageconfigs.h` | Class declaration |
| `artdaq-database/ConfigurationDB/shared_helper_functions.h` | Helper utilities |
| `artdaq-database/DataFormats/shared_literals.h` | Literal strings |
| `artdaq-database/BasicTypes/basictypes.h` | JsonData type |
| `artdaq-database/DataFormats/Json/json_reader.h` | JSON parsing |
| `artdaq-database/DataFormats/Json/json_writer.h` | JSON generation |

## TRACE Logging Levels

| Level | Purpose |
|-------|---------|
| 10 | Debug function enable confirmation |
| 20 | Version changes |
| 21 | Entity changes |
| 22 | Configuration changes |

## Performance Considerations

- JSON parsing occurs on every `readJsonData()` call; cache results if called repeatedly
- String comparisons against `apiliteral::notprovided` are efficient
- No heap allocations beyond JSON parsing

## Error Handling Strategy

- Empty input validation throws `runtime_error` with descriptive messages
- JSON parsing failures throw `invalid_option_exception`
- Missing optional fields are silently ignored (try/catch blocks)
- Debug assertions via `confirm()` catch programming errors

## Testing Notes

- **Unit tests:** `test/ConfigurationDB/options_t.cc`
- **Key test cases:**
  - JSON round-trip serialization
  - CLI argument parsing
  - Query filter generation

## Maintenance Notes

- The source option (-s) is defined in `makeProgramOptions()` but not read in `readProgramOptions()` - this may be intentional for compatibility
- The "assignconfig" exclusion in query filter generation is critical for preventing circular references

## See Also

- [options_operation_manageconfigs.h](./options_operation_manageconfigs.h.md) - Header file
- [options_operation_base.h](./options_operation_base.h.md) - Base class
- [options_operation_managedocument.cpp](./options_operation_managedocument.cpp.md) - Similar implementation with more options

---

**Documentation generated for artdaq-database ConfigurationDB module**
