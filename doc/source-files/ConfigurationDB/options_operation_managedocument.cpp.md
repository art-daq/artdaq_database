# options_operation_managedocument.cpp

**Path:** `artdaq-database/ConfigurationDB/options_operation_managedocument.cpp`

**Implements:** [options_operation_managedocument.h](./options_operation_managedocument.h.md)

**Purpose:** Implements the `ManageDocumentOperation` class which provides options handling for document-level operations such as reading, writing, and managing individual configuration documents. This file handles parsing of command-line arguments, JSON configuration data, and provides serialization methods for document operation parameters.

## Implementation Overview

This file implements the `ManageDocumentOperation` class, which extends `OperationBase` to add document-specific operation parameters including version, run number, entity name, configuration name, and source file name. The implementation provides:

1. Getter/setter methods for all document-related parameters with validation
2. JSON serialization and deserialization of operation options
3. Command-line argument parsing via Boost.Program_Options
4. Query filter generation for database searches

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | TRACE logging macros |
| `artdaq-database/ConfigurationDB/options_operation_managedocument.h` | Class declaration |
| `artdaq-database/ConfigurationDB/shared_helper_functions.h` | Helper utilities |
| `artdaq-database/DataFormats/shared_literals.h` | Literal strings |
| `artdaq-database/BasicTypes/basictypes.h` | JsonData type |
| `artdaq-database/DataFormats/Json/json_reader.h` | JSON parsing |
| `artdaq-database/DataFormats/Json/json_writer.h` | JSON generation |

## Key Algorithms

### JSON Data Parsing

The `readJsonData()` method parses incoming JSON configuration data in two steps:

**Steps:**
1. Parse the JSON buffer into an AST (Abstract Syntax Tree) using `JsonReader`
2. Extract top-level options (configuration, version, entity, run, source)
3. If a search filter object is present, extract nested filter parameters
4. Silently ignore missing optional fields (using try/catch blocks)

**Why this approach:** The dual-level parsing allows the same JSON structure to be used for both direct options and nested search filters, providing flexibility for different API use cases.

### Query Filter Generation

The `query_filter_to_JsonData()` method generates a database query filter:

**Steps:**
1. If a raw query filter was provided, return it directly
2. Start with base class filter from `OperationBase`
3. Add version, entity, and configuration filters if provided
4. Skip configuration filter for "assignconfig" operations to avoid self-referential queries
5. Return empty JSON if no filters were specified

## Internal Functions

### `ManageDocumentOperation::ManageDocumentOperation(std::string const& process_name)`

**Brief:** Constructor that initializes the operation with a process name by delegating to the base class constructor.

**Called by:** Client code creating document management operations

**Purpose:** Establishes the base operation state with the invoking process name for logging and identification.

### `version() const -> std::string const&`

**Brief:** Returns the current version string for this operation.

**Called by:** `writeJsonData()`, `query_filter_to_JsonData()`, `version_to_JsonData()`

**Purpose:** Provides read access to the version parameter with assertion that it has been set.

### `version(std::string const&) -> std::string const&`

**Brief:** Sets the version string for this operation, validating that it is not empty.

**Called by:** `readJsonData()`, `readProgramOptions()`

**Purpose:** Updates the version with validation and TRACE logging.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When version string is empty |

### `run() const -> std::string const&`

**Brief:** Returns the current run identifier for this operation.

**Called by:** `writeJsonData()`, `run_to_JsonData()`

**Purpose:** Provides read access to the run parameter with assertion that it has been set.

### `run(std::string const&) -> std::string const&`

**Brief:** Sets the run identifier for this operation, validating that it is not empty.

**Called by:** `readJsonData()`, `readProgramOptions()`

**Purpose:** Updates the run number with validation and TRACE logging.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When run string is empty |

### `entity() const -> std::string const&`

**Brief:** Returns the current configurable entity name for this operation.

**Called by:** `writeJsonData()`, `query_filter_to_JsonData()`, `entity_to_JsonData()`

**Purpose:** Provides read access to the entity name parameter.

### `entity(std::string const&) -> std::string const&`

**Brief:** Sets the configurable entity name, validating that it is not empty.

**Called by:** `readJsonData()`, `readProgramOptions()`

**Purpose:** Updates the entity name with validation and TRACE logging.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When entity string is empty |

### `configuration() const -> std::string const&`

**Brief:** Returns the current global configuration name for this operation.

**Called by:** `writeJsonData()`, `query_filter_to_JsonData()`, `configuration_to_JsonData()`, `configurationsname_to_JsonData()`

**Purpose:** Provides read access to the configuration name parameter.

### `configuration(std::string const&) -> std::string const&`

**Brief:** Sets the global configuration name, validating that it is not empty.

**Called by:** `readJsonData()`, `readProgramOptions()`

**Purpose:** Updates the global configuration name with validation and TRACE logging.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When configuration string is empty |

### `sourceFileName() const -> std::string const&`

**Brief:** Returns the source file name for configuration import/export operations.

**Called by:** `writeJsonData()`

**Purpose:** Provides read access to the source file name parameter.

### `sourceFileName(std::string const&) -> std::string const&`

**Brief:** Sets the source file name for import/export operations, validating that it is not empty.

**Called by:** `readJsonData()`, `readProgramOptions()`

**Purpose:** Updates the source file name with validation and TRACE logging.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When source file name string is empty |

### `readJsonData(JsonData const&) -> void`

**Brief:** Parses JSON input data to populate operation parameters including nested search filter extraction.

**Called by:** API entry points that receive JSON-formatted operation requests

**Purpose:** Deserializes JSON operation specifications into internal state.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON buffer cannot be parsed |

### `readProgramOptions(bpo::variables_map const&) -> int`

**Brief:** Reads command-line program options from a Boost.Program_Options variables map and sets corresponding operation parameters.

**Called by:** CLI tools parsing command-line arguments

**Purpose:** Maps command-line arguments to internal operation state.

**Returns:** `process_exit_code::SUCCESS` on successful parsing, or error code from base class.

### `makeProgramOptions() const -> bpo::options_description`

**Brief:** Creates a Boost.Program_Options description containing all document operation command-line options.

**Called by:** CLI tools building help messages and argument parsers

**Purpose:** Defines the command-line interface for document operations including version (-v), run (-r), entity (-e), configuration (-g), and source (-s) options.

### `writeJsonData() const -> JsonData`

**Brief:** Serializes all operation parameters to a JSON structure for transmission or storage.

**Called by:** API methods that need to serialize operation state

**Purpose:** Creates a complete JSON representation of the current operation options.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

### `query_filter_to_JsonData() const -> JsonData`

**Brief:** Generates a JSON query filter containing version, entity, and configuration criteria for database searches.

**Called by:** Database query methods that need to filter results

**Purpose:** Constructs search criteria for retrieving documents from storage providers.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

### `configuration_to_JsonData() const -> JsonData`

**Brief:** Creates a simple JSON object containing only the configuration name as a "name" field.

**Called by:** Operations that need a minimal configuration identifier

**Purpose:** Provides a JSON representation of just the configuration name.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

### `configurationsname_to_JsonData() const -> JsonData`

**Brief:** Creates a JSON object with the configuration name keyed by the filter configurations literal.

**Called by:** Operations filtering by configuration name

**Purpose:** Provides a filter-compatible JSON representation of the configuration name.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

### `collection_to_JsonData() const -> JsonData`

**Brief:** Creates a JSON object containing the collection name for database operations.

**Called by:** Operations targeting specific database collections

**Purpose:** Provides a JSON representation of the target collection.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

### `version_to_JsonData() const -> JsonData`

**Brief:** Creates a simple JSON object containing only the version as a "name" field.

**Called by:** Operations that need a minimal version identifier

**Purpose:** Provides a JSON representation of just the version.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

### `entity_to_JsonData() const -> JsonData`

**Brief:** Creates a simple JSON object containing only the entity name as a "name" field.

**Called by:** Operations that need a minimal entity identifier

**Purpose:** Provides a JSON representation of just the entity name.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

### `run_to_JsonData() const -> JsonData`

**Brief:** Creates a simple JSON object containing only the run identifier as a "name" field.

**Called by:** Operations that need a minimal run identifier

**Purpose:** Provides a JSON representation of just the run number.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

### `cf::debug::options::ManageDocuments() -> void`

**Brief:** Enables TRACE debugging for the ManageDocumentOperation class.

**Called by:** Debug/diagnostic code

**Purpose:** Configures TRACE logging for troubleshooting option parsing and serialization.

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not safe for concurrent modification
- **Locking:** No internal synchronization

## Performance Considerations

- JSON parsing and serialization is performed on every read/write operation; consider caching if performance is critical
- The `confirm()` macro performs runtime assertions that may impact debug build performance
- String comparisons against `apiliteral::notprovided` are used extensively; these are efficient due to string interning

## Error Handling Strategy

- Empty input validation throws `runtime_error` with descriptive messages
- JSON parsing failures throw `invalid_option_exception`
- Missing optional fields are silently ignored using try/catch blocks
- The `confirm()` macro provides debug-time assertions for precondition checking

## Testing Notes

- **Unit tests:** `test/ConfigurationDB/options_t.cc`
- **Key test cases:**
  - JSON round-trip serialization
  - Command-line argument parsing
  - Query filter generation with various option combinations

## Maintenance Notes

- The empty catch blocks for optional field parsing are intentional - missing fields should not cause failures
- The commented-out run filter in `query_filter_to_JsonData()` suggests historical changes - verify intended behavior before modifying
- The special case for "assignconfig" operation preventing configuration filter inclusion prevents self-referential queries

## See Also

- [options_operation_managedocument.h](./options_operation_managedocument.h.md) - Header file with class declaration
- [options_operation_base.h](./options_operation_base.h.md) - Base class for all operation options
- [json_reader.h](../DataFormats/Json/json_reader.h.md) - JSON parsing utilities
- [json_writer.h](../DataFormats/Json/json_writer.h.md) - JSON serialization utilities

---

**Documentation generated for artdaq-database ConfigurationDB module**
