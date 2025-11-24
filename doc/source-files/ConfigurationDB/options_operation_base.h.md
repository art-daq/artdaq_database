# options_operation_base.h / options_operation_base.cpp

## File Overview

These files define the `OperationBase` class, which serves as the base class for all database operation option classes in the ConfigurationDB module. It provides common functionality for configuring operations, including provider selection, operation type, data format, collection name, and query filters.

**Location**:
- `/home/user/artdaq-database/artdaq-database/ConfigurationDB/options_operation_base.h`
- `/home/user/artdaq-database/artdaq-database/ConfigurationDB/options_operation_base.cpp`

**Lines of Code**: 99 (header) + 360 (implementation)

**Purpose**: Base class providing common configuration options for all database operations

## Dependencies

### Standard Library
- `<utility>` - std::move
- `<fstream>` - File I/O for reading query files

### External Libraries
- `<boost/program_options.hpp>` - Command-line option parsing

### Project Headers
- `"artdaq-database/ConfigurationDB/shared_helper_functions.h"` - Helper utilities
- `"artdaq-database/DataFormats/shared_literals.h"` - String literals
- `"artdaq-database/SharedCommon/configuraion_api_literals.h"` - API literals
- `"artdaq-database/BasicTypes/basictypes.h"` - JsonData, FhiclData
- `"artdaq-database/JsonDocument/JSONDocument.h"` - JSON document class
- `"artdaq-database/DataFormats/Json/json_reader.h"` - JSON parsing
- `"artdaq-database/DataFormats/Json/json_writer.h"` - JSON generation

## Namespace: artdaq::database::configuration

All types and functions are declared within the `artdaq::database::configuration` namespace.

---

## Class: OperationBase

### Purpose

`OperationBase` is an abstract base class that encapsulates common configuration options needed for database operations. All specific operation classes (ManageDocumentOperation, ManageConfigsOperation, etc.) derive from this class.

---

## Public Methods

### Constructor

```cpp
OperationBase(std::string process_name)
```

**Purpose**: Construct an operation base with process name.

**Parameters**:
- `process_name` - Name of the process/application (for logging and help text)

**Initialization**:
- Sets process name
- Determines provider from ARTDAQ_DATABASE_URI environment variable
- Defaults to filesystem provider if not set

---

### Destructor

```cpp
virtual ~OperationBase()
```

**Purpose**: Virtual destructor for proper cleanup in derived classes.

---

### operation (getter)

```cpp
std::string const& operation() const
```

**Purpose**: Get the operation name (e.g., "readdocument", "writedocument").

**Returns**: Operation name string

**Validation**: Asserts operation is not empty

---

### operation (setter)

```cpp
std::string const& operation(std::string const& operation)
```

**Purpose**: Set the operation name.

**Parameters**:
- `operation` - Operation to perform (from apiliteral::operation::*)

**Returns**: Reference to the set operation

**Logging**: Logs operation change at TLOG level 20

**Example Operations**:
- `readdocument` - Read a document
- `writedocument` - Write a new document
- `overwritedocument` - Overwrite existing document
- `findversions` - Find available versions
- `listcollections` - List collections

---

### collection (getter/setter)

```cpp
std::string const& collection() const
std::string const& collection(std::string const& collection_name)
```

**Purpose**: Get/set the collection (table) name.

**Parameters**:
- `collection_name` - Name of the collection to operate on

**Returns**: Collection name

**Logging**: Changes logged at TLOG level 21

**Examples**: "ComponentConfigs", "BoardConfigs", "SystemMetadata"

---

### provider (getter/setter)

```cpp
std::string const& provider() const
std::string const& provider(std::string const& provider)
```

**Purpose**: Get/set the database provider.

**Parameters**:
- `provider` - Provider name (must be "filesystem", "mongodb", or "ucondb")

**Returns**: Provider name

**Throws**: `invalid_option_exception` if provider is invalid

**Logging**: Changes logged at TLOG level 22

**Valid Providers**:
- `filesystem` - File-based database
- `mongodb` - MongoDB database
- `ucondb` - UConDB (Microservice Configuration Database)

---

### format (getter/setters)

```cpp
data_format_t const& format() const
data_format_t const& format(data_format_t const& data_format)
data_format_t const& format(std::string const& format)
```

**Purpose**: Get/set the data format for input/output.

**Parameters**:
- `data_format` - Format enum value
- `format` - Format string ("json", "fhicl", "xml", "gui", "db", "csv")

**Returns**: Data format

**Throws**: Assertion if format is `unknown`

**Logging**: Changes logged at TLOG levels 23-24

**Available Formats**:
- `json` - JSON format
- `fhicl` - FHiCL (Fermilab Hierarchical Configuration Language)
- `xml` - XML format
- `gui` - GUI-friendly format
- `db` - Database internal format
- `csv` - CSV format

---

### queryFilter (getter/setter)

```cpp
std::string const& queryFilter() const
std::string const& queryFilter(std::string const& query_payload)
```

**Purpose**: Get/set the query filter JSON.

**Parameters**:
- `query_payload` - JSON string specifying filter criteria

**Returns**: Query filter string

**Processing**: Removes quotes if present

**Logging**: Logged at TLOG level 25

**Example**:
```cpp
opts.queryFilter("{\"version\":\"v1.0\",\"entity\":\"Component1\"}");
```

---

### processName

```cpp
std::string const& processName() const
```

**Purpose**: Get the process name.

**Returns**: Process/application name

---

### resultFileName (getter/setter)

```cpp
std::string const& resultFileName() const
std::string const& resultFileName(std::string const& result_file_name)
```

**Purpose**: Get/set the result output file name.

**Parameters**:
- `result_file_name` - Path to result file (environment variables expanded)

**Returns**: Result file name

**Processing**: Expands environment variables (e.g., `${HOME}`)

**Default**: `${HOME}/${process_name}.result.out`

---

### query_filter_to_JsonData

```cpp
virtual JsonData query_filter_to_JsonData() const
```

**Purpose**: Convert query filter to JsonData object.

**Returns**: JsonData containing filter, or empty JSON if no filter set

**Virtual**: Can be overridden in derived classes

---

### to_JsonData

```cpp
JsonData to_JsonData() const
```

**Purpose**: Convert entire operation configuration to JSON.

**Returns**: JsonData representing complete operation options

**Usage**: Used to serialize operation for transmission to provider dispatch

---

### to_string

```cpp
std::string to_string() const
```

**Purpose**: Convert operation to JSON string.

**Returns**: JSON string representation

**Usage**: For logging, debugging, or serialization

---

### Conversion Operators

```cpp
operator std::string() const
operator JSONDocument() const
```

**Purpose**: Allow implicit conversion to string or JSONDocument.

**Usage**:
```cpp
OperationBase opts("MyApp");
std::string str = opts;  // Calls to_string()
JSONDocument doc = opts;  // Calls to JSONDocument conversion
```

---

### makeProgramOptions

```cpp
virtual bpo::options_description makeProgramOptions() const
```

**Purpose**: Create Boost.Program_Options description for command-line parsing.

**Returns**: options_description with all supported options

**Options Defined**:
- `--help, -h` - Show help message
- `--operation, -o` - Operation name
- `--format, -f` - Data format
- `--collection, -c` - Collection name
- `--provider, -p` - Database provider (deprecated)
- `--searchfilter` - Query filter
- `--searchquery` - Query from file
- `--result, -x` - Result file name

**Usage**:
```cpp
auto opts_desc = opts.makeProgramOptions();
boost::program_options::variables_map vm;
// Parse command line using opts_desc...
```

---

### readProgramOptions

```cpp
virtual int readProgramOptions(bpo::variables_map const& vm)
```

**Purpose**: Process parsed command-line options and populate operation fields.

**Parameters**:
- `vm` - Boost variables_map from program_options parser

**Returns**: Exit code (0 = success, non-zero = error/help)

**Return Values**:
- `process_exit_code::SUCCESS` (0) - Options processed successfully
- `process_exit_code::HELP` (1) - Help requested
- `process_exit_code::INVALID_ARGUMENT | N` - Invalid arguments

**Processing**:
1. Checks for help flag
2. Sets result file name
3. Processes JSON query file if provided
4. Reads operation, format, collection from options
5. Reads search filter if provided

**Special Handling**:
- If `--searchquery` is a file path, reads JSON from file
- Provider determined from ARTDAQ_DATABASE_URI environment variable

---

### readJsonData

```cpp
virtual void readJsonData(JsonData const& data)
```

**Purpose**: Populate operation fields from JSON data.

**Parameters**:
- `data` - JSON string containing operation options

**Throws**: `invalid_option_exception` if JSON is invalid

**JSON Format**:
```json
{
  "operation": "readdocument",
  "format": "json",
  "collection": "ComponentConfigs",
  "searchfilter": {
    "version": "v1.0",
    "entity": "Component1"
  }
}
```

**Processing**: Safely extracts each field, ignoring missing fields

---

### writeJsonData

```cpp
virtual JsonData writeJsonData() const
```

**Purpose**: Serialize operation options to JSON format.

**Returns**: JsonData with all operation fields

**Throws**: `invalid_option_exception` if unable to generate JSON

**Usage**: Called by `to_JsonData()` and conversion operators

---

## Private Methods

### _getProviderFromURI

```cpp
std::string _getProviderFromURI()
```

**Purpose**: Determine database provider from ARTDAQ_DATABASE_URI environment variable.

**Returns**: Provider name ("mongodb" or "filesystem")

**Logic**:
- If URI starts with "mongodb", returns "mongodb"
- Otherwise, returns "filesystem"
- If ARTDAQ_DATABASE_URI not set, returns "filesystem"

**Example URIs**:
- `mongodb://localhost:27017/artdaq_db` → "mongodb"
- `filesystemdb:///data/configs` → "filesystem"

---

## Private Members

```cpp
std::string _process_name = {apiliteral::notprovided};
std::string _provider = {apiliteral::provider::filesystem};
std::string _operation = {apiliteral::operation::readdocument};
data_format_t _data_format = {data_format_t::unknown};
std::string _collection_name = {apiliteral::notprovided};
std::string _query_payload = {apiliteral::notprovided};
std::string _result_file_name = {apiliteral::notprovided};
```

**Purpose**: Internal storage for all operation configuration options.

**Defaults**:
- Provider: filesystem
- Operation: readdocument
- Format: unknown
- Others: "notprovided"

---

## Free Functions

### operator<<

```cpp
std::ostream& operator<<(std::ostream& os, OperationBase const& o)
```

**Purpose**: Stream insertion operator for OperationBase.

**Usage**:
```cpp
OperationBase opts("MyApp");
std::cout << opts << std::endl;  // Prints JSON representation
```

---

## Debug Functions

### artdaq::database::configuration::debug::options::OperationBase()

```cpp
void debug::options::OperationBase()
```

**Purpose**: Enable full trace logging for OperationBase.

**Usage**:
```cpp
artdaq::database::configuration::debug::options::OperationBase();
// Now all TLOG messages will be printed
```

---

## Usage Examples

### Basic Usage

```cpp
#include "artdaq-database/ConfigurationDB/options_operation_base.h"

using artdaq::database::configuration::OperationBase;
using artdaq::database::configuration::options::data_format_t;

// Derived class would typically use this
class MyOperation : public OperationBase {
public:
    MyOperation(std::string name) : OperationBase(name) {}
};

MyOperation opts("MyApp");
opts.operation("readdocument");
opts.collection("ComponentConfigs");
opts.format(data_format_t::json);
opts.queryFilter("{\"version\":\"v1.0\"}");

// Serialize to JSON
std::string json = opts.to_string();
```

### Command-Line Parsing

```cpp
int main(int argc, char** argv) {
    MyOperation opts("myapp");

    auto opts_desc = opts.makeProgramOptions();
    bpo::variables_map vm;

    try {
        bpo::store(bpo::parse_command_line(argc, argv, opts_desc), vm);
        bpo::notify(vm);

        int exit_code = opts.readProgramOptions(vm);
        if (exit_code != 0) {
            return exit_code;
        }

        // Use opts for database operation...

    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
```

### JSON-Based Configuration

```cpp
std::string json_config = R"({
    "operation": "writedocument",
    "collection": "ComponentConfigs",
    "format": "json",
    "searchfilter": {
        "version": "v2.0",
        "entity": "DAQ1"
    }
})";

MyOperation opts("MyApp");
opts.readJsonData(JsonData{json_config});

// opts is now configured from JSON
std::cout << "Operation: " << opts.operation() << "\n";
std::cout << "Collection: " << opts.collection() << "\n";
```

---

## Thread Safety

**Not Thread-Safe**: OperationBase instances should not be shared across threads.

**Usage Pattern**: Create separate instances per thread:
```cpp
void thread_function() {
    MyOperation opts("ThreadWorker");
    // Configure and use opts...
}
```

---

## Best Practices

### Always Set Required Fields

```cpp
// GOOD: All required fields set
opts.operation("readdocument");
opts.collection("ComponentConfigs");
opts.format(data_format_t::json);

// BAD: Missing required fields will cause assertions
// opts.operation("readdocument");
// auto col = opts.collection();  // ASSERTION FAILURE
```

### Use Literals for Operation Names

```cpp
namespace apiliteral = artdaq::database::configapi::literal;

// GOOD: Use constants
opts.operation(apiliteral::operation::readdocument);

// BAD: Magic strings
opts.operation("readdocument");  // Typo-prone
```

### Validate Provider Names

```cpp
// GOOD: Use valid providers
opts.provider("filesystem");
opts.provider("mongodb");
opts.provider("ucondb");

// BAD: Invalid provider throws exception
try {
    opts.provider("invalid");  // Throws invalid_option_exception
} catch (...) {
    // Handle error
}
```

---

## Related Files

- **options_operation_managedocument.h** - Document management operations
- **options_operation_manageconfigs.h** - Configuration management operations
- **options_operation_managealiases.h** - Alias management operations
- **options_operation_bulkoperations.h** - Bulk operations
- **shared_helper_functions.h** - Utility functions
- **configuraion_api_literals.h** - String literal constants

---

**Documentation generated for artdaq-database ConfigurationDB module**
