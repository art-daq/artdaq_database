# options_operation_managedocument.h

**Path:** `artdaq-database/ConfigurationDB/options_operation_managedocument.h`

**Purpose:** Declares the `ManageDocumentOperation` class which extends `OperationBase` to provide additional options handling for document-level database operations. This class adds support for version, run, entity, configuration, and source file parameters needed for reading, writing, and managing individual configuration documents.


## Key Concepts

### Document Operations

Document operations are the most common type of database operations, working with individual configuration documents:

| Operation | Description |
|-----------|-------------|
| `readdocument` | Read a document from the database |
| `writedocument` | Write a new document to the database |
| `overwritedocument` | Overwrite an existing document |
| `findversions` | Find available versions of a document |
| `markreadonly` | Mark a document as read-only |
| `markdeleted` | Mark a document as deleted |

### Class Hierarchy

```
OperationBase (base class)
    |
    +-- ManageDocumentOperation (this class)
            - version
            - entity
            - run
            - configuration
            - sourceFileName
```

### Key Parameters

| Parameter | Description | Example |
|-----------|-------------|---------|
| `version` | Document version string | "v1.0", "v2.3.1" |
| `entity` | Configurable entity name | "BoardReader01", "EventBuilder01" |
| `run` | Run number identifier | "123", "run_001" |
| `configuration` | Global configuration name | "production_config", "test_v2" |
| `sourceFileName` | Source file for import/export | "/path/to/config.json" |

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not safe for concurrent modification; use one instance per thread
- **Locking:** No internal locking; callers must synchronize access if sharing instances

**Rationale:** Document operations are typically short-lived request handlers. Creating separate instances avoids synchronization overhead.

## Dependencies

| Include | Purpose |
|---------|---------|
| `options_operation_base.h` | Base class `OperationBase` providing common operation parameters |
| `<boost/program_options.hpp>` | Command-line argument parsing infrastructure |

## Forward Declarations

| Type | Purpose |
|------|---------|
| `artdaq::database::basictypes::JsonData` | JSON data wrapper type |
| `TraceStreamer` | TRACE logging support for stream operators |

## Classes/Structures

### `ManageDocumentOperation`

**Brief:** Final class extending `OperationBase` with document-specific parameters for version, entity, run, configuration, and source file name. Used for all document-level database operations.

**Thread Safety:** Not thread-safe; use separate instances per thread.

#### Constructor

##### `ManageDocumentOperation(std::string const& process_name)`

**Brief:** Constructs a ManageDocumentOperation with the specified process name.

**Parameters:**
- `process_name` - Name of the invoking process, used for logging and help messages

**Preconditions:**
- None

**Postconditions:**
- All document parameters initialized to `apiliteral::notprovided`
- Base class initialized with process name
- Provider auto-detected from environment

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Constructor does not throw |

**Thread Safety:** Safe - construction creates independent instance

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/options_operation_managedocument.h"

namespace cf = artdaq::database::configuration;

void example() {
  // Create document operation options
  cf::ManageDocumentOperation opts("my_application");

  // Configure for document read
  opts.operation("readdocument");
  opts.collection("detector_config");
  opts.entity("BoardReader01");
  opts.version("v1.0");
  opts.format("json");
}
```

#### Methods

##### `version() const -> std::string const&`

**Brief:** Returns the current document version string.

**Parameters:** None

**Preconditions:**
- Version must have been set (assertion fires if empty in debug builds)

**Returns:** Const reference to the version string

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::logic_error` | If version is empty (debug builds via `confirm()`) |

**Thread Safety:** Safe for read access if no concurrent writes

---

##### `version(std::string const& version) -> std::string const&`

**Brief:** Sets the document version and returns the new value.

**Parameters:**
- `version` - Version string to set (e.g., "v1.0", "20231215_1430")

**Preconditions:**
- `version` must not be empty

**Returns:** Const reference to the updated version

**Postconditions:**
- Internal version is updated
- TRACE log at level 20 is generated

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` | When `version` is empty |

**Thread Safety:** Not safe for concurrent access

**Example:**
```cpp
cf::ManageDocumentOperation opts("myapp");
opts.version("v1.0");
std::cout << "Version: " << opts.version() << std::endl;
```

---

##### `sourceFileName() const -> std::string const&`

**Brief:** Returns the source file name for input data.

**Parameters:** None

**Preconditions:**
- Source file name must have been set

**Returns:** Const reference to the source file path

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::logic_error` | If source file name is empty (debug builds via `confirm()`) |

**Thread Safety:** Safe for read access if no concurrent writes

---

##### `sourceFileName(std::string const& source_file_name) -> std::string const&`

**Brief:** Sets the source file name for input data. Environment variables are automatically expanded.

**Parameters:**
- `source_file_name` - Path to input file, may contain environment variables like `${HOME}`

**Preconditions:**
- `source_file_name` must not be empty

**Returns:** Const reference to the expanded file path

**Postconditions:**
- Environment variables in path are expanded
- TRACE log at level 24 is generated

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` | When `source_file_name` is empty |

**Thread Safety:** Not safe for concurrent access

**Example:**
```cpp
cf::ManageDocumentOperation opts("myapp");
opts.sourceFileName("${HOME}/configs/detector.json");
// Expands to "/home/user/configs/detector.json"
```

---

##### `entity() const -> std::string const&`

**Brief:** Returns the entity/component name.

**Parameters:** None

**Preconditions:**
- Entity must have been set

**Returns:** Const reference to the entity name

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::logic_error` | If entity is empty (debug builds via `confirm()`) |

**Thread Safety:** Safe for read access if no concurrent writes

---

##### `entity(std::string const& entity) -> std::string const&`

**Brief:** Sets the entity/component name for the operation.

**Parameters:**
- `entity` - Entity name (e.g., "BoardReader01", "EventBuilder01")

**Preconditions:**
- `entity` must not be empty

**Returns:** Const reference to the updated entity name

**Postconditions:**
- Internal entity is updated
- TRACE log at level 22 is generated

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` | When `entity` is empty |

**Thread Safety:** Not safe for concurrent access

---

##### `run() const -> std::string const&`

**Brief:** Returns the run number string.

**Parameters:** None

**Preconditions:**
- Run must have been set

**Returns:** Const reference to the run number string

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::logic_error` | If run is empty (debug builds via `confirm()`) |

**Thread Safety:** Safe for read access if no concurrent writes

---

##### `run(std::string const& run) -> std::string const&`

**Brief:** Sets the run number for run-specific queries.

**Parameters:**
- `run` - Run number as string (e.g., "12345")

**Preconditions:**
- `run` must not be empty

**Returns:** Const reference to the updated run number

**Postconditions:**
- Internal run is updated
- TRACE log at level 21 is generated

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` | When `run` is empty |

**Thread Safety:** Not safe for concurrent access

---

##### `configuration() const -> std::string const&`

**Brief:** Returns the global configuration name.

**Parameters:** None

**Preconditions:**
- Configuration must have been set

**Returns:** Const reference to the configuration name

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::logic_error` | If configuration is empty (debug builds via `confirm()`) |

**Thread Safety:** Safe for read access if no concurrent writes

---

##### `configuration(std::string const& global_configuration) -> std::string const&`

**Brief:** Sets the global configuration name.

**Parameters:**
- `global_configuration` - Configuration name (e.g., "production_v2", "test_config")

**Preconditions:**
- `global_configuration` must not be empty

**Returns:** Const reference to the updated configuration name

**Postconditions:**
- Internal configuration is updated
- TRACE log at level 23 is generated

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` | When `global_configuration` is empty |

**Thread Safety:** Not safe for concurrent access

---

##### `query_filter_to_JsonData() const -> JsonData` (override)

**Brief:** Generates a JSON query filter containing version, entity, and configuration criteria.

**Parameters:** None

**Returns:** JsonData containing query filter, or empty JSON `{}` if no filters set

**Postconditions:**
- Configuration filter excluded for "assignconfig" operations
- Only non-default parameters are included

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

**Thread Safety:** Safe for read access if no concurrent writes

---

##### `configuration_to_JsonData() const -> JsonData`

**Brief:** Creates a JsonData object with the configuration name using the "name" key.

**Parameters:** None

**Returns:** JsonData with format `{"name": "<configuration>"}`

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

**Thread Safety:** Safe for read access if no concurrent writes

---

##### `configurationsname_to_JsonData() const -> JsonData`

**Brief:** Creates a JsonData object with the configuration name using the filter configurations key.

**Parameters:** None

**Returns:** JsonData with format `{"configurations": "<configuration>"}`

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

**Thread Safety:** Safe for read access if no concurrent writes

---

##### `version_to_JsonData() const -> JsonData`

**Brief:** Creates a JsonData object with the version using the "name" key.

**Parameters:** None

**Returns:** JsonData with format `{"name": "<version>"}`

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

**Thread Safety:** Safe for read access if no concurrent writes

---

##### `entity_to_JsonData() const -> JsonData`

**Brief:** Creates a JsonData object with the entity name using the "name" key.

**Parameters:** None

**Returns:** JsonData with format `{"name": "<entity>"}`

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

**Thread Safety:** Safe for read access if no concurrent writes

---

##### `collection_to_JsonData() const -> JsonData`

**Brief:** Creates a JsonData object containing the collection name.

**Parameters:** None

**Returns:** JsonData with collection field

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

**Thread Safety:** Safe for read access if no concurrent writes

---

##### `run_to_JsonData() const -> JsonData`

**Brief:** Creates a JsonData object with the run identifier using the "name" key.

**Parameters:** None

**Returns:** JsonData with format `{"name": "<run>"}`

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

**Thread Safety:** Safe for read access if no concurrent writes

---

##### `makeProgramOptions() const -> bpo::options_description` (override)

**Brief:** Creates Boost.ProgramOptions description including base class options plus document-specific options.

**Parameters:** None

**Returns:** Options description with all command-line options

**Thread Safety:** Safe (creates new object)

**Additional Options (beyond base class):**

| Option | Short | Description |
|--------|-------|-------------|
| `--version` | `-v` | Document version |
| `--entity` | `-e` | Entity/component name |
| `--run` | `-r` | Run number |
| `--configuration` | `-g` | Global configuration name |
| `--source` | `-s` | Source input file |

---

##### `readProgramOptions(bpo::variables_map const& vm) -> int` (override)

**Brief:** Processes parsed command-line options including document-specific parameters.

**Parameters:**
- `vm` - Boost.ProgramOptions variables map

**Returns:** Exit code (`process_exit_code::SUCCESS` for success)

**Thread Safety:** Not safe for concurrent access

---

##### `readJsonData(JsonData const& data)` (override)

**Brief:** Populates document parameters from JSON data, including nested search filter extraction.

**Parameters:**
- `data` - JsonData containing operation parameters

**Preconditions:**
- `data` must not be empty
- `data` must contain valid JSON

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | If JSON cannot be parsed |

**Thread Safety:** Not safe for concurrent access

**Expected JSON Format:**
```json
{
  "operation": "readdocument",
  "collection": "detector_config",
  "format": "json",
  "version": "v1.0",
  "entity": "detector01",
  "run": "12345",
  "configuration": "production_v2",
  "searchfilter": {
    "version": "v1.0",
    "entity": "detector01"
  }
}
```

---

##### `writeJsonData() const -> JsonData` (override)

**Brief:** Serializes all parameters including document-specific fields to JsonData.

**Parameters:** None

**Returns:** JsonData with all operation parameters

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON serialization fails |

**Thread Safety:** Safe for read access if no concurrent writes

## Private Members

| Member | Type | Default | Description |
|--------|------|---------|-------------|
| `_version` | `std::string` | "notprovided" | Document version |
| `_entity_name` | `std::string` | "notprovided" | Entity/component name |
| `_run` | `std::string` | "notprovided" | Run number |
| `_global_configuration` | `std::string` | "notprovided" | Global configuration name |
| `_source_file_name` | `std::string` | "notprovided" | Source input file path |

## Free Functions

### `operator<<(TraceStreamer& ts, ManageDocumentOperation const& o) -> TraceStreamer&`

**Brief:** Stream insertion operator for TRACE logging.

**Parameters:**
- `ts` - TraceStreamer reference
- `o` - ManageDocumentOperation to serialize

**Returns:** Reference to TraceStreamer for chaining

**Example:**
```cpp
cf::ManageDocumentOperation opts("myapp");
opts.version("v1.0");
TLOG(10) << "Options: " << opts;  // Uses this specialization
```

## Debug Functions

### Namespace: `artdaq::database::configuration::debug::options`

#### `ManageDocuments() -> void`

**Brief:** Enables TRACE debugging for ManageDocumentOperation parsing and serialization.

**Side Effects:**
- Configures TRACE logging for "options_operation_managedocument.cpp"
- Enables all log levels for this module

## Relationship to Other Components

- **OperationBase**: Parent class providing common parameters
- **dboperation_managedocument.h**: Uses this class for document operations
- **conftool**: CLI tool uses this for document commands
- **BulkOperations**: Contains list of ManageDocumentOperation instances
- **options_operations.h**: Aggregates this with other option classes

## See Also

- [options_operation_managedocument.cpp](./options_operation_managedocument.cpp.md) - Implementation
- [options_operation_base.h](./options_operation_base.h.md) - Base class
- [options_operation_manageconfigs.h](./options_operation_manageconfigs.h.md) - Configuration operations
- [options_operation_managealiases.h](./options_operation_managealiases.h.md) - Alias operations
- [dboperation_managedocument.h](./dboperation_managedocument.h.md) - Document database operations

## Notes for Developers

### Command-Line Usage

```bash
# Read a document
conftool -o readdocument -c detector_config -v v1.0 -e detector01

# Write a document from file
conftool -o writedocument -c detector_config -v v1.0 -e detector01 -s input.json

# Find versions
conftool -o findversions -c detector_config -e detector01
```

### Common Pitfalls

- **Pitfall 1:** Forgetting to set version and entity for read operations - most document operations require both
- **Pitfall 2:** Not specifying sourceFileName for write operations - the source file contains the document data
- **Pitfall 3:** Confusing `configuration` (global config name) with `collection` (database collection)
- **Pitfall 4:** Configuration filter is excluded for "assignconfig" operations to prevent circular queries

### Anti-patterns

```cpp
// DON'T do this: Write without source file
cf::ManageDocumentOperation opts("myapp");
opts.operation("writedocument");
opts.collection("detector_config");
opts.version("v1.0");
// Missing: opts.sourceFileName(...) - where is the data?

// DO this instead: Provide complete parameters
cf::ManageDocumentOperation opts("myapp");
opts.operation("writedocument");
opts.collection("detector_config");
opts.version("v1.0");
opts.entity("detector01");
opts.sourceFileName("/path/to/config.json");
```

## Usage Examples

### Reading a Document

```cpp
#include "artdaq-database/ConfigurationDB/options_operation_managedocument.h"
#include <iostream>

namespace cf = artdaq::database::configuration;

void readDocument() {
  cf::ManageDocumentOperation opts("my_application");

  // Configure for read
  opts.operation("readdocument");
  opts.collection("detector_config");
  opts.format(cf::options::data_format_t::json);
  opts.version("v1.0");
  opts.entity("detector01");

  // Convert to JSON for the operation
  auto queryJson = opts.query_filter_to_JsonData();
  std::cout << "Query: " << queryJson.json_buffer << std::endl;

  // Would then call: cf::json::read_document(opts, result);
}
```

### Writing a Document

```cpp
#include "artdaq-database/ConfigurationDB/options_operation_managedocument.h"

namespace cf = artdaq::database::configuration;

void writeDocument() {
  cf::ManageDocumentOperation opts("my_application");

  // Configure for write
  opts.operation("writedocument");
  opts.collection("detector_config");
  opts.format(cf::options::data_format_t::json);
  opts.version("v2.0");
  opts.entity("detector01");
  opts.sourceFileName("${HOME}/configs/new_config.json");

  // Would then call: cf::json::write_document(opts, result);
}
```

### Finding Versions

```cpp
#include "artdaq-database/ConfigurationDB/options_operation_managedocument.h"

namespace cf = artdaq::database::configuration;

void findVersions() {
  cf::ManageDocumentOperation opts("my_application");

  opts.operation("findversions");
  opts.collection("detector_config");
  opts.format(cf::options::data_format_t::json);
  opts.entity("detector01");  // Find all versions for this entity

  // Would then call: cf::json::find_versions(opts, result);
}
```

---

**Documentation generated for artdaq-database ConfigurationDB module**
