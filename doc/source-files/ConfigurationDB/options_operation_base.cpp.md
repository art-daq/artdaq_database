# options_operation_base.cpp

**Path:** `artdaq-database/ConfigurationDB/options_operation_base.cpp`

**Implements:** [options_operation_base.h](./options_operation_base.h.md)

**Purpose:** Implements the `OperationBase` class methods including parameter getters/setters, command-line option parsing, and JSON serialization/deserialization. This file provides the core functionality for configuring database operations.

## Implementation Overview

This implementation file provides the complete functionality for the `OperationBase` class, including:
- Constructor and destructor implementations
- Getter/setter methods for all operation parameters
- Boost.ProgramOptions integration for CLI parsing
- JSON serialization and deserialization
- Provider detection from environment variables

## Key Algorithms

### Provider Detection from URI

The `_getProviderFromURI()` method determines the storage provider from the `ARTDAQ_DATABASE_URI` environment variable:

**Steps:**
1. Read `ARTDAQ_DATABASE_URI` from environment
2. Expand any environment variables in the URI string
3. Check if URI starts with "mongo" prefix
4. Return "mongo" if MongoDB URI, otherwise "filesystem"

**Why this approach:** The URI scheme provides a standard way to specify database connections, making it easy to switch providers via configuration without code changes.

### Command-Line Option Processing

The `readProgramOptions()` method processes CLI arguments in a specific order:

**Steps:**
1. Check for `--help` flag and return early if set
2. Set result file name (with default fallback to `${HOME}/${0}.result.out`)
3. If `--searchquery` is provided and is a file path, read and parse file contents
4. Determine provider from environment URI
5. Validate and set operation (required)
6. Set format, collection, and search filter if provided

**Why this approach:** The ordering ensures that JSON query files can override individual options, providing flexibility for complex configurations.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | TRACE logging macros and shared utilities |
| `artdaq-database/ConfigurationDB/options_operation_base.h` | Class declaration |
| `<utility>` | `std::move` for efficient string handling |
| `artdaq-database/ConfigurationDB/shared_helper_functions.h` | Helper utilities including `dequote`, `expand_environment_variables` |
| `artdaq-database/DataFormats/shared_literals.h` | Format string literals |
| `artdaq-database/BasicTypes/basictypes.h` | JsonData type |
| `artdaq-database/DataFormats/Json/json_reader.h` | JSON parsing via JsonReader |
| `artdaq-database/DataFormats/Json/json_writer.h` | JSON generation via JsonWriter |
| `artdaq-database/JsonDocument/JSONDocument.h` | Document type for conversion |

## Namespace Aliases

| Alias | Full Namespace | Purpose |
|-------|----------------|---------|
| `db` | `artdaq::database` | Root database namespace |
| `cf` | `artdaq::database::configuration` | Configuration namespace |
| `dbbt` | `artdaq::database::basictypes` | Basic types (JsonData, etc.) |

## Internal Functions

### Constructor Implementation

```cpp
OperationBase::OperationBase(std::string process_name)
    : _process_name{std::move(process_name)}, _provider{_getProviderFromURI()} {}
```

**Brief:** Initializes the object with a process name and auto-detects the provider from the environment.

**Key Details:**
- Uses `std::move` for efficient string initialization
- Calls `_getProviderFromURI()` during construction to set the default provider

### Destructor Implementation

```cpp
OperationBase::~OperationBase() = default;
```

**Brief:** Default destructor - no special cleanup needed.

### Getter/Setter Pattern

All getter/setter pairs follow this pattern:

```cpp
std::string const& OperationBase::operation() const {
  confirm(!_operation.empty());
  return _operation;
}

std::string const& OperationBase::operation(std::string const& operation) {
  confirm(!operation.empty());
  TLOG(20) << "Options: Updating operation from " << _operation << " to " << operation << ".";
  _operation = operation;
  return _operation;
}
```

**Key Details:**
- Getters use `confirm()` to assert non-empty state in debug builds
- Setters log changes at specific TRACE levels for debugging
- Both return const references to avoid unnecessary copying

### Provider Validation

```cpp
std::string const& OperationBase::provider(std::string const& provider) {
  confirm(!provider.empty());

  if (db::not_equal(provider, apiliteral::provider::filesystem) &&
      db::not_equal(provider, apiliteral::provider::mongo) &&
      db::not_equal(provider, apiliteral::provider::ucon)) {
    throw db::invalid_option_exception("OperationBase")
        << "Invalid database provider; database provider=" << provider << ".";
  }

  TLOG(22) << "Options: Updating provider from " << _provider << " to " << provider << ".";
  _provider = provider;
  return _provider;
}
```

**Brief:** Validates provider name before setting, throwing exception for invalid values.

**Called by:** `readJsonData()`, `readProgramOptions()`, and direct API calls

### JSON Serialization

```cpp
JsonData OperationBase::writeJsonData() const {
  using namespace artdaq::database::json;

  auto queryFilterAST = object_t{};
  if (!JsonReader{}.read(query_filter_to_JsonData(), queryFilterAST)) {
    throw db::invalid_option_exception("OperationBase") << "Unable to read query_filter_to_JsonData().";
  }

  auto docAST = object_t{};
  docAST[apiliteral::option::searchfilter] = queryFilterAST;
  docAST[apiliteral::option::operation] = operation();
  docAST[apiliteral::option::collection] = collection();
  docAST[apiliteral::option::format] = cf::to_string(format());

  auto json_buffer = std::string{};
  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("OperationBase") << "Unable to write JSON buffer.";
  }
  return {json_buffer};
}
```

**Brief:** Converts operation parameters to a JSON object for transmission or storage.

**Called by:** `to_JsonData()`, `to_string()`, derived class `writeJsonData()` methods

### JSON Deserialization

```cpp
void OperationBase::readJsonData(JsonData const& data) {
  confirm(!data.empty());
  confirm(data.json_buffer != apiliteral::notprovided);

  using namespace artdaq::database::json;
  auto filterAST = object_t{};

  if (!JsonReader{}.read(data, filterAST)) {
    throw db::invalid_option_exception("OperationBase") << "Unable to read JSON buffer.";
  }

  try { provider(_getProviderFromURI()); } catch (...) {}
  try { operation(boost::get<std::string>(filterAST.at(apiliteral::option::operation))); } catch (...) {}
  try { format(boost::get<std::string>(filterAST.at(apiliteral::option::format))); } catch (...) {}
  try { collection(boost::get<std::string>(filterAST.at(apiliteral::option::collection))); } catch (...) {}
  // ... handle searchfilter
}
```

**Brief:** Parses JSON data and populates operation parameters, silently ignoring missing fields.

**Key Details:**
- Uses try/catch blocks to handle missing fields gracefully
- Allows partial JSON configurations
- Provider is always determined from environment, not JSON

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not safe for concurrent modification
- **Locking:** No internal synchronization

## Performance Considerations

- **String handling**: Uses `std::move` and const references to minimize copying
- **Lazy evaluation**: Provider is determined once at construction
- **JSON parsing**: Parses filter to AST only when needed for serialization

## Error Handling Strategy

The implementation uses three levels of error handling:

1. **Debug assertions** (`confirm()`): Catch programming errors in debug builds
2. **Validation exceptions**: Throw `invalid_option_exception` for invalid input
3. **Silent catch**: Ignore missing optional fields during JSON deserialization

## TRACE Logging Levels

| Level | Purpose |
|-------|---------|
| 10 | Debug function enable confirmation |
| 20 | Operation changes |
| 21 | Collection changes |
| 22 | Provider changes |
| 23 | Format changes (enum) |
| 24 | Format/file name changes (string), JSON query logging |
| 25 | Query filter changes |

## Testing Notes

- **Unit tests:** Located in `test/ConfigurationDB/` directory
- **Key test cases:**
  - Constructor with various process names
  - Valid and invalid provider names
  - JSON round-trip (serialize then deserialize)
  - Command-line parsing with various option combinations
  - Environment variable expansion in file paths

## Maintenance Notes

### Adding a New Option

To add a new option to OperationBase:

1. Add private member variable with default value
2. Add getter/setter methods following existing pattern
3. Update `makeProgramOptions()` to include CLI option
4. Update `readProgramOptions()` to process the option
5. Update `readJsonData()` to parse from JSON
6. Update `writeJsonData()` to serialize to JSON

### Debug Function

```cpp
void cf::debug::options::OperationBase() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);
  TLOG(10) << "artdaq::database::configuration::options::OperationBase trace_enable";
}
```

**Brief:** Enables full TRACE logging for this module. Call for debugging option processing.

## See Also

- [options_operation_base.h](./options_operation_base.h.md) - Header file with class declaration
- [options_operation_managedocument.cpp](./options_operation_managedocument.cpp.md) - Derived class implementation
- [json_reader.h](../DataFormats/Json/json_reader.h.md) - JSON parsing utilities
- [json_writer.h](../DataFormats/Json/json_writer.h.md) - JSON generation utilities

## Usage Examples

### Basic Option Configuration

```cpp
#include "artdaq-database/ConfigurationDB/options_operation_base.h"

namespace cf = artdaq::database::configuration;

void configureOperation() {
  cf::OperationBase opts("my_application");

  // Configure basic parameters
  opts.operation("readdocument");
  opts.collection("detector_config");
  opts.format(cf::options::data_format_t::json);
  opts.queryFilter(R"({"version": "v1.0"})");

  // Convert to JSON for transmission
  auto json = opts.to_JsonData();
  std::cout << "Configuration: " << json.json_buffer << std::endl;
}
```

### Processing Command-Line Options

```cpp
#include "artdaq-database/ConfigurationDB/options_operation_base.h"
#include <boost/program_options.hpp>

namespace bpo = boost::program_options;
namespace cf = artdaq::database::configuration;

int processCommandLine(int argc, char* argv[]) {
  cf::OperationBase opts("conftool");

  // Create options description
  auto desc = opts.makeProgramOptions();

  // Parse command line
  bpo::variables_map vm;
  try {
    bpo::store(bpo::parse_command_line(argc, argv, desc), vm);
    bpo::notify(vm);
  } catch (std::exception const& e) {
    std::cerr << "Parse error: " << e.what() << std::endl;
    return 1;
  }

  // Process parsed options
  int result = opts.readProgramOptions(vm);
  if (result != 0) {
    return result;
  }

  // Options are now populated
  std::cout << "Operation: " << opts.operation() << std::endl;
  std::cout << "Collection: " << opts.collection() << std::endl;

  return 0;
}
```

---

**Documentation generated for artdaq-database ConfigurationDB module**
