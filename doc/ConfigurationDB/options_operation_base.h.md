# options_operation_base.h

**Path:** `artdaq-database/ConfigurationDB/options_operation_base.h`

**Purpose:** Defines the `OperationBase` class, which serves as the base class for all database operation parameter containers. This class encapsulates common parameters shared across all database operations, including provider selection, operation type, data format, collection name, and query filters.


## Key Concepts

### Operation Options Pattern

The ConfigurationDB module uses an **Options Pattern** where operation parameters are encapsulated in dedicated classes. `OperationBase` is the foundation of this hierarchy, providing common parameters that all operations need.

### Class Hierarchy

```
OperationBase (this class)
    |
    +-- ManageDocumentOperation (document read/write)
    +-- ManageConfigsOperation (configuration management)
    +-- ManageAliasesOperation (alias management)
```

Note: `BulkOperations` is a separate class (not derived from `OperationBase`) that contains a list of operations.

### Data Format Enumeration

The class uses `data_format_t` to specify input/output data formats:

| Value | Description |
|-------|-------------|
| `unknown` | Format not specified |
| `json` | JSON format |
| `fhicl` | FHiCL format |
| `xml` | XML format |
| `gui` | GUI-specific format |
| `db` | Database internal format |
| `csv` | Comma-separated values |

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Each thread should use its own `OperationBase` instance
- **Locking:** No internal locking; instances are not designed for shared access

**Rationale:** Operation option objects are typically short-lived and used within a single request context. Creating separate instances per thread avoids synchronization overhead.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/shared_helper_functions.h` | Helper functions including `data_format_t` enumeration |
| `artdaq-database/DataFormats/shared_literals.h` | Data format string literals |
| `artdaq-database/SharedCommon/configuraion_api_literals.h` | API string literals for option names |
| `<boost/program_options.hpp>` | Command-line option parsing infrastructure |

## Forward Declarations

| Type | Purpose |
|------|---------|
| `artdaq::database::basictypes::JsonData` | JSON data wrapper type used for serialization |
| `artdaq::database::docrecord::JSONDocument` | Document record type for conversion operators |

## Classes/Structures

### `OperationBase`

**Brief:** Base class for database operation parameter containers. Encapsulates common parameters shared by all database operations including provider, operation type, format, collection, and query filters.

**Thread Safety:** Not thread-safe - use one instance per thread

#### Constructor

##### `OperationBase(std::string process_name)`

**Brief:** Constructs an OperationBase instance with the given process name. The storage provider is automatically determined from the `ARTDAQ_DATABASE_URI` environment variable.

**Parameters:**
- `process_name` - Name of the calling process, used in help messages and logging

**Preconditions:**
- None (process_name can be empty but is not recommended)

**Postconditions:**
- Object is initialized with default values
- Provider is set based on `ARTDAQ_DATABASE_URI` environment variable (defaults to "filesystem")
- Operation defaults to "readdocument"
- Format defaults to `data_format_t::unknown`

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Constructor does not throw |

**Thread Safety:** Safe - construction creates independent instance

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/options_operation_base.h"

namespace cf = artdaq::database::configuration;

void example() {
  // Create options with process name for logging
  cf::OperationBase opts("my_application");

  // opts is now initialized with defaults:
  // - provider: determined from ARTDAQ_DATABASE_URI or "filesystem"
  // - operation: "readdocument"
  // - format: unknown (must be set before use)
}
```

#### Destructor

##### `~OperationBase()`

**Brief:** Virtual destructor for proper polymorphic destruction of derived classes.

**Thread Safety:** Safe if no other threads access the object

#### Methods

##### `operation() const -> std::string const&`

**Brief:** Returns the current operation type (e.g., "readdocument", "writedocument").

**Parameters:** None

**Preconditions:**
- Operation must have been set (assertion fires via `confirm()` if empty)

**Returns:** Const reference to the operation name string

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::logic_error` | If operation is empty (debug builds only, via `confirm()`) |

**Thread Safety:** Safe for read access if no concurrent writes

---

##### `operation(std::string const& operation) -> std::string const&`

**Brief:** Sets the operation type and returns the new value.

**Parameters:**
- `operation` - The operation name to set (e.g., "readdocument", "writedocument", "findversions")

**Preconditions:**
- `operation` must not be empty

**Returns:** Const reference to the updated operation name

**Postconditions:**
- Internal operation is updated to the new value
- TRACE log at level 20 is generated

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::logic_error` | If `operation` is empty (debug builds via `confirm()`) |

**Thread Safety:** Not safe for concurrent access

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/options_operation_base.h"

namespace cf = artdaq::database::configuration;

void setOperation() {
  cf::OperationBase opts("myapp");
  opts.operation("writedocument");
  std::cout << "Operation: " << opts.operation() << std::endl;
  // Output: Operation: writedocument
}
```

---

##### `collection() const -> std::string const&`

**Brief:** Returns the current collection name.

**Parameters:** None

**Preconditions:**
- Collection must have been set

**Returns:** Const reference to the collection name string

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::logic_error` | If collection is empty (debug builds via `confirm()`) |

**Thread Safety:** Safe for read access if no concurrent writes

---

##### `collection(std::string const& collection_name) -> std::string const&`

**Brief:** Sets the collection name and returns the new value.

**Parameters:**
- `collection_name` - The collection name to set (e.g., "artdaq_configurations", "detector_config")

**Preconditions:**
- `collection_name` must not be empty

**Returns:** Const reference to the updated collection name

**Postconditions:**
- Internal collection name is updated
- TRACE log at level 21 is generated

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::logic_error` | If `collection_name` is empty (debug builds via `confirm()`) |

**Thread Safety:** Not safe for concurrent access

---

##### `provider() const -> std::string const&`

**Brief:** Returns the current storage provider name.

**Parameters:** None

**Preconditions:**
- Provider must have been set (typically auto-detected from URI at construction)

**Returns:** Const reference to the provider name ("filesystem", "mongo", or "ucon")

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::logic_error` | If provider is empty (debug builds via `confirm()`) |

**Thread Safety:** Safe for read access if no concurrent writes

---

##### `provider(std::string const& provider) -> std::string const&`

**Brief:** Sets the storage provider and returns the new value. Validates that the provider is one of the supported values.

**Parameters:**
- `provider` - The provider name to set ("filesystem", "mongo", or "ucon")

**Preconditions:**
- `provider` must not be empty
- `provider` must be one of: "filesystem", "mongo", "ucon"

**Returns:** Const reference to the updated provider name

**Postconditions:**
- Internal provider is updated
- TRACE log at level 22 is generated

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | If `provider` is not a valid provider name |
| `std::logic_error` | If `provider` is empty (debug builds via `confirm()`) |

**Thread Safety:** Not safe for concurrent access

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/options_operation_base.h"

namespace cf = artdaq::database::configuration;
namespace db = artdaq::database;

void setProvider() {
  cf::OperationBase opts("myapp");

  try {
    opts.provider("mongo");
    std::cout << "Provider set to: " << opts.provider() << std::endl;
  } catch (db::invalid_option_exception const& e) {
    std::cerr << "Invalid provider: " << e.what() << std::endl;
  }
}
```

---

##### `format() const -> data_format_t const&`

**Brief:** Returns the current data format setting.

**Parameters:** None

**Preconditions:**
- Format must have been set to a known value (not `unknown`)

**Returns:** Const reference to the data format enumeration value

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::logic_error` | If format is `unknown` (debug builds via `confirm()`) |

**Thread Safety:** Safe for read access if no concurrent writes

---

##### `format(data_format_t const& data_format) -> data_format_t const&`

**Brief:** Sets the data format using the enumeration value.

**Parameters:**
- `data_format` - The data format enumeration value to set

**Preconditions:**
- `data_format` must not be `data_format_t::unknown`

**Returns:** Const reference to the updated format

**Postconditions:**
- Internal format is updated
- TRACE log at level 23 is generated

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::logic_error` | If `data_format` is `unknown` (debug builds via `confirm()`) |

**Thread Safety:** Not safe for concurrent access

---

##### `format(std::string const& format) -> data_format_t const&`

**Brief:** Sets the data format by parsing a string (e.g., "json", "fhicl", "xml", "gui", "db", "csv").

**Parameters:**
- `format` - String representation of the format

**Preconditions:**
- `format` must not be empty

**Returns:** Const reference to the updated format enumeration

**Postconditions:**
- Format is converted from string and stored
- TRACE log at level 24 is generated

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::logic_error` | If `format` is empty (debug builds via `confirm()`) |

**Thread Safety:** Not safe for concurrent access

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/options_operation_base.h"

namespace cf = artdaq::database::configuration;

void setFormat() {
  cf::OperationBase opts("myapp");

  // Set format using string
  opts.format("json");

  // Or using enumeration
  opts.format(cf::options::data_format_t::fhicl);
}
```

---

##### `queryFilter() const -> std::string const&`

**Brief:** Returns the current query filter JSON string.

**Parameters:** None

**Preconditions:**
- Query filter must have been set

**Returns:** Const reference to the query filter string

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::logic_error` | If query filter is empty (debug builds via `confirm()`) |

**Thread Safety:** Safe for read access if no concurrent writes

---

##### `queryFilter(std::string const& query_payload) -> std::string const&`

**Brief:** Sets the query filter from a JSON string. The string is automatically dequoted if necessary.

**Parameters:**
- `query_payload` - JSON string containing the query filter

**Preconditions:**
- `query_payload` must not be empty

**Returns:** Const reference to the updated query filter

**Postconditions:**
- Query filter is set after dequoting
- TRACE log at level 25 is generated

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::logic_error` | If `query_payload` is empty (debug builds via `confirm()`) |

**Thread Safety:** Not safe for concurrent access

---

##### `processName() const -> std::string const&`

**Brief:** Returns the process name set during construction.

**Parameters:** None

**Returns:** Const reference to the process name string

**Thread Safety:** Safe for read access (immutable after construction)

---

##### `resultFileName() const -> std::string const&`

**Brief:** Returns the result file name for output.

**Parameters:** None

**Preconditions:**
- Result file name must have been set

**Returns:** Const reference to the result file name

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::logic_error` | If result file name is empty (debug builds via `confirm()`) |

**Thread Safety:** Safe for read access if no concurrent writes

---

##### `resultFileName(std::string const& result_file_name) -> std::string const&`

**Brief:** Sets the result file name. Environment variables in the path are automatically expanded.

**Parameters:**
- `result_file_name` - File path for results, may contain environment variables like `${HOME}`

**Preconditions:**
- `result_file_name` must not be empty

**Returns:** Const reference to the expanded file name

**Postconditions:**
- Environment variables are expanded
- TRACE log at level 24 is generated

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::logic_error` | If `result_file_name` is empty (debug builds via `confirm()`) |

**Thread Safety:** Not safe for concurrent access

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/options_operation_base.h"

namespace cf = artdaq::database::configuration;

void setResultFile() {
  cf::OperationBase opts("myapp");
  opts.resultFileName("${HOME}/output/result.json");
  // Expands to "/home/user/output/result.json"
  std::cout << "Result file: " << opts.resultFileName() << std::endl;
}
```

---

##### `query_filter_to_JsonData() const -> JsonData` (virtual)

**Brief:** Converts the query filter string to a JsonData object. Returns empty JSON if no filter is set.

**Parameters:** None

**Returns:** JsonData object containing the query filter, or empty JSON object `{}`

**Thread Safety:** Safe for read access if no concurrent writes

---

##### `to_JsonData() const -> JsonData`

**Brief:** Serializes all operation parameters to a JsonData object.

**Parameters:** None

**Returns:** JsonData object containing all operation parameters as JSON

**Thread Safety:** Safe for read access if no concurrent writes

---

##### `to_string() const -> std::string`

**Brief:** Converts operation parameters to a JSON string representation.

**Parameters:** None

**Returns:** JSON string representation of all parameters

**Thread Safety:** Safe for read access if no concurrent writes

---

##### `operator std::string() const`

**Brief:** Implicit conversion to string, equivalent to `to_string()`.

**Returns:** JSON string representation

---

##### `operator JSONDocument() const`

**Brief:** Implicit conversion to JSONDocument.

**Returns:** JSONDocument containing operation parameters

---

##### `makeProgramOptions() const -> bpo::options_description` (virtual)

**Brief:** Creates Boost.ProgramOptions description for command-line parsing.

**Parameters:** None

**Returns:** Options description with all supported command-line options

**Thread Safety:** Safe (creates new object)

**Options Created:**

| Option | Short | Description |
|--------|-------|-------------|
| `--help` | `-h` | Show help message |
| `--operation` | `-o` | Operation type |
| `--format` | `-f` | Data format (fhicl, xml, gui, db, or csv) |
| `--collection` | `-c` | Collection name |
| `--provider` | `-p` | Database provider (deprecated) |
| `--searchfilter` | | Search filter JSON |
| `--searchquery` | | Search query JSON or file path |
| `--result` | `-x` | Expected result file name |

---

##### `readProgramOptions(bpo::variables_map const& vm) -> int` (virtual)

**Brief:** Reads and processes parsed command-line options.

**Parameters:**
- `vm` - Boost.ProgramOptions variables map containing parsed options

**Returns:** Process exit code (0 for success, non-zero for errors)

**Return Values:**
| Value | Meaning |
|-------|---------|
| `process_exit_code::SUCCESS` (0) | Options processed successfully |
| `process_exit_code::HELP` | Help was requested |
| `process_exit_code::INVALID_ARGUMENT \| 2` | Missing required operation |

**Postconditions:**
- Object is populated with values from command line
- If `--searchquery` contains a file path, file contents are read and parsed

**Thread Safety:** Not safe for concurrent access

---

##### `readJsonData(JsonData const& data)` (virtual)

**Brief:** Populates operation parameters from a JSON data object.

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
  "format": "json",
  "collection": "detector_config",
  "searchfilter": {
    "version": "v1.0",
    "entity": "Component1"
  }
}
```

---

##### `writeJsonData() const -> JsonData` (virtual)

**Brief:** Serializes current operation parameters to JsonData.

**Parameters:** None

**Returns:** JsonData object with serialized parameters

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | If serialization fails |

**Thread Safety:** Safe for read access if no concurrent writes

## Free Functions

### `operator<<(std::ostream& os, OperationBase const& o) -> std::ostream&`

**Brief:** Stream insertion operator for outputting OperationBase to streams.

**Parameters:**
- `os` - Output stream
- `o` - OperationBase instance to output

**Returns:** Reference to the output stream

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/options_operation_base.h"
#include <iostream>

namespace cf = artdaq::database::configuration;

void printOptions() {
  cf::OperationBase opts("myapp");
  opts.operation("readdocument");
  opts.collection("detector_config");
  opts.format("json");

  std::cout << opts << std::endl;
  // Outputs JSON representation of all parameters
}
```

## Debug Functions

### Namespace: `artdaq::database::configuration::debug::options`

#### `OperationBase() -> void`

**Brief:** Enables TRACE debugging for the OperationBase class. Call this to enable verbose logging of option operations.

**Side Effects:**
- Sets TRACE control for "options_operation_base.cpp"
- Enables all log levels for this module

## Private Members

| Member | Type | Default | Description |
|--------|------|---------|-------------|
| `_process_name` | `std::string` | "notprovided" | Process/application name |
| `_provider` | `std::string` | from URI or "filesystem" | Database provider |
| `_operation` | `std::string` | "readdocument" | Operation type |
| `_data_format` | `data_format_t` | `unknown` | Data format |
| `_collection_name` | `std::string` | "notprovided" | Collection name |
| `_query_payload` | `std::string` | "notprovided" | Query filter JSON |
| `_result_file_name` | `std::string` | "notprovided" | Result file path |

## Private Methods

### `_getProviderFromURI() -> std::string`

**Brief:** Determines the database provider from the `ARTDAQ_DATABASE_URI` environment variable.

**Returns:** "mongo" if URI starts with "mongodb", otherwise "filesystem"

## Relationship to Other Components

- **ManageDocumentOperation**: Extends this class for document-specific operations
- **ManageConfigsOperation**: Extends this class for configuration operations
- **ManageAliasesOperation**: Extends this class for alias operations
- **BulkOperations**: Uses ManageDocumentOperation internally (does not extend OperationBase)
- **options_operations.h**: Aggregates all option classes
- **conftool**: CLI tool uses option classes for command-line implementation

## See Also

- [options_operation_base.cpp](./options_operation_base.cpp.md) - Implementation file
- [options_operation_managedocument.h](./options_operation_managedocument.h.md) - Document operations extension
- [options_operation_manageconfigs.h](./options_operation_manageconfigs.h.md) - Configuration operations extension
- [options_operation_managealiases.h](./options_operation_managealiases.h.md) - Alias operations extension
- [shared_helper_functions.h](./shared_helper_functions.h.md) - Helper functions and data_format_t
- [External: Boost.ProgramOptions](https://www.boost.org/doc/libs/release/doc/html/program_options.html)

## Notes for Developers

### Default Values

| Parameter | Default Value |
|-----------|---------------|
| `_provider` | Determined from `ARTDAQ_DATABASE_URI`, defaults to "filesystem" |
| `_operation` | "readdocument" |
| `_data_format` | `data_format_t::unknown` |
| `_collection_name` | "notprovided" |
| `_query_payload` | "notprovided" |
| `_result_file_name` | "notprovided" |

### Common Pitfalls

- **Pitfall 1:** Forgetting to set required parameters before use. Always set `collection` and `format` before performing operations.
- **Pitfall 2:** Using `confirm()` getters without setting values first will assert in debug builds.
- **Pitfall 3:** The provider setter validates the value - use the exact provider names ("filesystem", "mongo", "ucon").

### Anti-patterns

```cpp
// DON'T do this: Accessing without setting
cf::OperationBase opts("myapp");
auto coll = opts.collection();  // ASSERTS in debug!

// DO this instead: Set before get
cf::OperationBase opts("myapp");
opts.collection("my_collection");
auto coll = opts.collection();  // OK
```

## Usage Examples

### Basic Configuration

```cpp
#include "artdaq-database/ConfigurationDB/options_operation_base.h"
#include <iostream>

namespace cf = artdaq::database::configuration;

int main() {
  cf::OperationBase opts("my_application");

  // Configure operation
  opts.operation("readdocument");
  opts.collection("detector_config");
  opts.format(cf::options::data_format_t::json);
  opts.queryFilter(R"({"version": "v1.0"})");

  // Serialize to JSON for debugging
  std::cout << "Operation config: " << opts.to_string() << std::endl;

  return 0;
}
```

### Command-Line Parsing

```cpp
#include "artdaq-database/ConfigurationDB/options_operation_base.h"
#include <boost/program_options.hpp>
#include <iostream>

namespace bpo = boost::program_options;
namespace cf = artdaq::database::configuration;

int main(int argc, char* argv[]) {
  cf::OperationBase opts("conftool");

  // Create and parse command-line options
  auto opts_desc = opts.makeProgramOptions();
  bpo::variables_map vm;

  try {
    bpo::store(bpo::parse_command_line(argc, argv, opts_desc), vm);
    bpo::notify(vm);

    int result = opts.readProgramOptions(vm);
    if (result != 0) {
      return result;  // Help requested or error
    }

    // Use configured options
    std::cout << "Provider: " << opts.provider() << std::endl;
    std::cout << "Operation: " << opts.operation() << std::endl;

  } catch (std::exception const& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
```

### JSON-Based Configuration

```cpp
#include "artdaq-database/ConfigurationDB/options_operation_base.h"
#include "artdaq-database/BasicTypes/basictypes.h"
#include <iostream>

namespace cf = artdaq::database::configuration;
namespace db = artdaq::database;
using artdaq::database::basictypes::JsonData;

void configureFromJson() {
  std::string json_config = R"({
    "operation": "writedocument",
    "collection": "detector_config",
    "format": "json",
    "searchfilter": {
      "version": "v2.0",
      "entity": "Component1"
    }
  })";

  cf::OperationBase opts("my_application");

  try {
    opts.readJsonData(JsonData{json_config});

    // Options are now configured from JSON
    std::cout << "Operation: " << opts.operation() << std::endl;
    std::cout << "Collection: " << opts.collection() << std::endl;
  } catch (db::invalid_option_exception const& e) {
    std::cerr << "Failed to parse options: " << e.what() << std::endl;
  }
}
```

---

**Documentation generated for artdaq-database ConfigurationDB module**
