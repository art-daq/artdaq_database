# options_operation_managealiases.cpp

**Path:** `artdaq-database/ConfigurationDB/options_operation_managealiases.cpp`

**Implements:** [options_operation_managealiases.h](./options_operation_managealiases.h.md)

**Purpose:** Implements the `ManageAliasesOperation` class which handles options for alias management operations including version aliases, configuration aliases, and run associations. This file provides validation, JSON serialization/deserialization, and command-line parsing for alias-related operations.

## Implementation Overview

This file implements the most feature-rich operation options class, supporting:
- Version aliases (human-readable names for document versions)
- Configuration aliases (human-readable names for global configurations)
- Run associations (linking run numbers to configuration states)
- Standard filtering by entity, version, and configuration

The implementation handles the complexity of alias operations where the same JSON field names may appear at different nesting levels (top-level options vs. search filter).

## Key Algorithms

### JSON Data Parsing with Fallback

The `readJsonData()` method uses a two-level extraction strategy:

**Steps:**
1. Call base class `readJsonData()` for common options
2. Parse JSON into AST
3. For each parameter (entity, versionAlias, configurationAlias, version, configuration, run):
   - First try to extract from nested search filter
   - If not found, fall back to top-level extraction
   - Silently ignore if not present at either level
4. Handle empty filter specially by setting `queryFilter` to `notprovided`

**Why this approach:** API calls may specify parameters either at top level or nested in a search filter object. The fallback pattern ensures both styles work.

### Query Filter with Operation-Specific Exclusions

The `query_filter_to_JsonData()` method generates filters with special handling:

**Steps:**
1. Return raw query filter if explicitly provided
2. Start with base class filter
3. Add entity filter if provided
4. Add version filter if provided
5. Add configuration filter unless operation is "assignconfig"
6. Add version alias filter unless operation is "addversionalias"
7. Add run filter unless operation is "addrun"
8. Return empty JSON if no filters specified

**Why the exclusions:** When adding a new alias or run, including it in the filter would prevent finding the target document (since the alias doesn't exist yet).

## Internal Functions

### `ManageAliasesOperation::ManageAliasesOperation(std::string const& process_name)`

**Brief:** Constructor that initializes the alias operation by delegating to the base class.

**Called by:** Client code creating alias management operations

**Purpose:** Establishes base operation state with process name for logging.

### `version() const -> std::string const&`

**Brief:** Returns the target version for alias operations.

**Called by:** `writeJsonData()`, `query_filter_to_JsonData()`

**Purpose:** Provides read access with debug assertion.

### `version(std::string const& version) -> std::string const&`

**Brief:** Sets the target version with validation.

**Called by:** `readJsonData()`, `readProgramOptions()`

**Purpose:** Updates target version with empty-check validation.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When version is empty |

### `entity() const -> std::string const&`

**Brief:** Returns the entity name for scoped alias operations.

**Called by:** `writeJsonData()`, `query_filter_to_JsonData()`

**Purpose:** Provides read access with debug assertion.

### `entity(std::string const& entity) -> std::string const&`

**Brief:** Sets the entity name with validation.

**Called by:** `readJsonData()`, `readProgramOptions()`

**Purpose:** Updates entity name with empty-check validation.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When entity is empty |

### `run() const -> std::string const&`

**Brief:** Returns the run identifier for run association operations.

**Called by:** `writeJsonData()`, `query_filter_to_JsonData()`, `run_to_JsonData()`

**Purpose:** Provides read access with debug assertion.

### `run(std::string const& run) -> std::string const&`

**Brief:** Sets the run identifier with validation.

**Called by:** `readJsonData()`, `readProgramOptions()`

**Purpose:** Updates run identifier with empty-check validation.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When run is empty |

### `configuration() const -> std::string const&`

**Brief:** Returns the target configuration name for configuration alias operations.

**Called by:** `writeJsonData()`, `query_filter_to_JsonData()`

**Purpose:** Provides read access with debug assertion.

### `configuration(std::string const& global_configuration) -> std::string const&`

**Brief:** Sets the target configuration name with validation.

**Called by:** `readJsonData()`, `readProgramOptions()`

**Purpose:** Updates configuration name with empty-check validation.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When configuration is empty |

### `versionAlias() const -> std::string const&`

**Brief:** Returns the version alias name.

**Called by:** `writeJsonData()`, `query_filter_to_JsonData()`, `versionAlias_to_JsonData()`

**Purpose:** Provides read access with debug assertion.

### `versionAlias(std::string const& alias) -> std::string const&`

**Brief:** Sets the version alias name with validation.

**Called by:** `readJsonData()`, `readProgramOptions()`

**Purpose:** Updates version alias with empty-check validation and TRACE logging.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When alias is empty |

### `configurationAlias() const -> std::string const&`

**Brief:** Returns the configuration alias name.

**Called by:** `writeJsonData()`

**Purpose:** Provides read access with debug assertion.

### `configurationAlias(std::string const& global_configuration_alias) -> std::string const&`

**Brief:** Sets the configuration alias name with validation.

**Called by:** `readJsonData()`, `readProgramOptions()`

**Purpose:** Updates configuration alias with empty-check validation and TRACE logging.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When configuration alias is empty |

### `readJsonData(JsonData const& data) -> void`

**Brief:** Parses JSON input with two-level extraction for all alias parameters.

**Called by:** API entry points receiving JSON operation requests

**Purpose:** Deserializes JSON to internal state, handling both top-level and nested filter formats.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON parsing fails |

### `readProgramOptions(bpo::variables_map const& vm) -> int`

**Brief:** Reads command-line arguments and sets all alias operation parameters.

**Called by:** CLI tools processing command-line arguments

**Purpose:** Maps CLI arguments to internal state including entity, configuration, version, run, version_alias, and configuration_alias.

**Returns:** `process_exit_code::SUCCESS` or error code from base class

### `makeProgramOptions() const -> bpo::options_description`

**Brief:** Creates command-line option descriptions for alias operations.

**Called by:** CLI tools building help and argument parsers

**Purpose:** Defines CLI interface:
- `-v/--version` - Target version
- `-a/--version_alias` - Version alias name
- `-e/--entity` - Entity name
- `-g/--configuration` - Configuration name
- `-q/--configuration_alias` - Configuration alias name
- `-r/--run` - Run identifier

### `writeJsonData() const -> JsonData`

**Brief:** Serializes all alias operation parameters to JSON.

**Called by:** Methods needing to transmit or store operation state

**Purpose:** Creates JSON with configuration, versionAlias, configurationAlias, version, and run fields.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

### `query_filter_to_JsonData() const -> JsonData`

**Brief:** Generates a JSON query filter with operation-specific exclusions for adding new aliases or runs.

**Called by:** Database query methods

**Purpose:** Constructs search criteria with special handling to avoid self-referential queries.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

### `versionAlias_to_JsonData() const -> JsonData`

**Brief:** Creates a simple JSON object with version alias as the "name" field.

**Called by:** Operations needing minimal alias identification

**Purpose:** Provides simple JSON wrapper for version alias.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

### `run_to_JsonData() const -> JsonData`

**Brief:** Creates a simple JSON object with run as the "name" field.

**Called by:** Operations needing minimal run identification

**Purpose:** Provides simple JSON wrapper for run identifier.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

### `cf::debug::options::ManageAliases() -> void`

**Brief:** Enables TRACE debugging for alias option handling.

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
| `artdaq-database/ConfigurationDB/options_operation_managealiases.h` | Class declaration |
| `artdaq-database/ConfigurationDB/shared_helper_functions.h` | Helper utilities |
| `artdaq-database/DataFormats/shared_literals.h` | Literal strings |
| `artdaq-database/BasicTypes/basictypes.h` | JsonData type |
| `artdaq-database/DataFormats/Json/json_reader.h` | JSON parsing |
| `artdaq-database/DataFormats/Json/json_writer.h` | JSON generation |

## TRACE Logging Levels

| Level | Purpose |
|-------|---------|
| 10 | Debug function enable confirmation |
| 20 | Version alias changes |
| 21 | Run changes |
| 22 | Entity changes |
| 23 | Configuration alias changes |

## Performance Considerations

- The two-level JSON extraction (filter then top-level) doubles parse attempts for some fields
- Exception-based flow control for missing fields has overhead but simplifies code
- Consider caching if `readJsonData()` is called repeatedly with same data

## Error Handling Strategy

- Empty input validation throws `runtime_error` with descriptive messages
- JSON parsing failures throw `invalid_option_exception`
- Missing optional fields handled via try/catch (silent ignore)
- Note: `versionAlias_to_JsonData()` exception message incorrectly references "ManageConfigsOperation" (copy-paste error in source)

## Testing Notes

- **Unit tests:** `test/ConfigurationDB/options_t.cc`
- **Key test cases:**
  - JSON round-trip with all alias parameters
  - Query filter exclusion logic for each operation type
  - Two-level JSON extraction (filter vs top-level)

## Maintenance Notes

- The fallback extraction pattern (try filter, catch and try top-level) is verbose but necessary for API compatibility
- The operation-specific exclusions in `query_filter_to_JsonData()` are critical for add operations to work correctly
- There's a minor issue in `versionAlias_to_JsonData()` - the exception message says "ManageConfigsOperation" instead of "ManageAliasesOperation"

## See Also

- [options_operation_managealiases.h](./options_operation_managealiases.h.md) - Header file
- [options_operation_base.h](./options_operation_base.h.md) - Base class
- [dboperation_managealiases.h](./dboperation_managealiases.h.md) - Database operations using these options

---

**Documentation generated for artdaq-database ConfigurationDB module**
