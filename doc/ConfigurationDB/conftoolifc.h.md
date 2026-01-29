# conftoolifc.h

**Path:** `artdaq-database/ConfigurationDB/conftoolifc.h`

**Purpose:** Provides the template-based command-line tool implementation infrastructure for the ConfigurationDB module. This header defines the `conftool_impl` function template that enables building command-line database tools with Boost.Program_options integration. It serves as the bridge between command-line argument parsing and database operations.


## Key Concepts

### Command-Line Tool Framework

This header implements a generic framework for building configuration database command-line tools. The framework follows these steps:

1. **Parse Arguments**: Uses Boost.Program_options to parse command-line arguments
2. **Register Operations**: Registers all supported database operations with their implementations
3. **Dispatch**: Invokes the requested operation based on user input
4. **Return Results**: Returns success/failure status with appropriate messages

### Operation Registration Pattern

The framework uses a registration pattern where each database operation is registered with:
- An operation name (string literal from `apiliteral::operation`)
- A function pointer to the operation implementation
- The operation's bound input parameters

This pattern enables extensibility - new operations can be added by registering them in `conftool_impl`.

### Options Class Contract

The `OPTS` template parameter must satisfy a specific interface contract, providing methods for:
- Creating program options descriptions
- Reading and validating parsed options
- Providing operation names and file paths
- Serializing options to strings

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not applicable - designed for single-threaded CLI tools
- **Locking:** None

This function is designed for command-line tools that run as single-threaded processes.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | Common utilities, macros, and trace logging |
| `artdaq-database/ConfigurationDB/configuration_common.h` | Configuration types, literals, and operation signatures |

## Namespace Aliases

```cpp
namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfo = cf::options;
namespace bpo = boost::program_options;
namespace jsonliteral = db::dataformats::literal;
namespace apiliteral = db::configapi::literal;
```

These aliases simplify code and make it more readable throughout the implementation.

## Functions

### `expand_environment_variables(var) -> std::string`

```cpp
std::string expand_environment_variables(const std::string& var);
```

**Brief:** Expands environment variables in file paths and configuration strings, replacing `$VAR` or `${VAR}` patterns with their values.

**Parameters:**
- `var` - String potentially containing environment variables (e.g., `$HOME/config.json`)

**Returns:** String with environment variables replaced by their values

**Thread Safety:** Safe (uses thread-local or const data)

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/conftoolifc.h"

void example() {
  std::string path = "$HOME/configs/detector.json";
  std::string expanded = artdaq::database::expand_environment_variables(path);
  // expanded might be "/home/user/configs/detector.json"
}
```

---

### `conftool_impl<OPTS>(options, argc, argv) -> result_t`

```cpp
template <typename OPTS>
result_t conftool_impl(std::unique_ptr<OPTS>& options, int argc, char* argv[]);
```

**Brief:** Main implementation template for configuration database command-line tools. Parses arguments, registers operations, and dispatches to the requested operation.

**Template Parameters:**
- `OPTS` - Options class type that provides program options and operation handling

**Parameters:**
- `options` - Unique pointer to the options object (modified during execution)
- `argc` - Argument count from `main()`
- `argv` - Argument vector from `main()`

**Preconditions:**
- `options` must be a valid, non-null unique_ptr
- `OPTS` must implement the required interface (see below)

**Returns:** `result_t` pair containing:
- `first`: `true` on success, `false` on failure
- `second`: Message string (success message or error description)

**Postconditions:**
- On success with read operations, result file contains retrieved data
- On failure, error message describes the problem

**Side Effects:**
- Reads from and writes to files based on operation
- Connects to database (FileSystemDB or MongoDB)
- Modifies the `options` object during parsing

**Thread Safety:** Not thread-safe - designed for single-threaded CLI use

#### Required OPTS Interface

The `OPTS` template parameter must implement these methods:

| Method | Return Type | Purpose |
|--------|-------------|---------|
| `makeProgramOptions()` | `bpo::options_description` | Creates Boost.Program_options description |
| `readProgramOptions(vm)` | `process_exit_code` | Processes parsed options, returns exit code |
| `processName()` | `std::string` | Returns tool name for error messages |
| `operation()` | `std::string` | Returns the requested operation name |
| `resultFileName()` | `std::string` | Returns the output file name |
| `to_string()` | `std::string` | Serializes options to JSON string |

#### Registered Operations

| Operation Name | Function | Description |
|----------------|----------|-------------|
| `readdocument` | `read_document` | Read a single document from the database |
| `readconfiguration` | `read_configuration` | Read a configuration with format conversion |
| `writedocument` | `write_document` | Write a document to the database |
| `writeconfiguration` | `write_configuration` | Write a configuration with format conversion |
| `findconfigs` | `find_configurations` | Find global configurations matching criteria |
| `confcomposition` | `configuration_composition` | Get composition of a global configuration |
| `assignconfig` | `assign_configuration` | Assign a version to a global configuration |
| `removeconfig` | `remove_configuration` | Remove a version from a global configuration |
| `searchcollection` | `search_collection` | Search within a collection |
| `findversions` | `find_versions` | Find document versions |
| `findentities` | `find_entities` | Find entities in a collection |
| `addentity` | `add_entity` | Add an entity to a document |
| `rmentity` | `remove_entity` | Remove an entity from a document |
| `markdeleted` | `mark_document_deleted` | Mark a document as deleted |
| `markreadonly` | `mark_document_readonly` | Mark a document as read-only |
| `findversionalias` | `find_version_aliases` | Find version aliases |
| `addversionalias` | `add_version_alias` | Add a version alias |
| `rmversionalias` | `remove_version_alias` | Remove a version alias |
| `listdatabases` | `list_databases` | List available databases |
| `listcollections` | `list_collections` | List collections in a database |
| `readdbinfo` | `read_dbinfo` | Read database information |
| `exportcollection` | `export_collection` | Export a collection to file |
| `importcollection` | `import_collection` | Import a collection from file |
| `exportdatabase` | `export_database` | Export entire database |
| `importdatabase` | `import_database` | Import entire database |

#### Error Handling

Returns `Failure()` result in these cases:

| Condition | Error Message Pattern |
|-----------|----------------------|
| Invalid command-line arguments | `process_exit_code::INVALID_ARGUMENT` |
| Missing required result file for read ops | `process_exit_code::INVALID_ARGUMENT \| 1` |
| Invalid options values | `process_exit_code::<code>` |
| Unsupported operation | `Unsupported operation <name>` |
| Operation execution failure | Operation-specific error |

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/conftoolifc.h"
#include "artdaq-database/ConfigurationDB/options_operation_managedocument.h"

int main(int argc, char* argv[]) {
  try {
    // Create options object for document management
    auto options = std::make_unique<cf::ManageDocumentOperation>("conftool");

    // Execute the tool
    auto result = artdaq::database::conftool_impl(options, argc, argv);

    if (result.first) {
      std::cout << "Success: " << result.second << std::endl;
      return 0;
    } else {
      std::cerr << "Error: " << result.second << std::endl;
      return 1;
    }
  } catch (const std::exception& e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
    return 2;
  }
}
```

## Implementation Details

### Argument Parsing Flow

```
1. options->makeProgramOptions() creates description
2. bpo::store() parses argc/argv
3. bpo::notify() triggers validation
4. options->readProgramOptions(vm) processes values
5. Validate read operations have result file
6. Register all operations
7. Load source file if specified
8. Dispatch to requested operation
```

### Source File Handling

For write operations, the source file is loaded before operation dispatch:

```cpp
try {
  db::read_buffer_from_file(test_document, file_src_name);
  // Register write operations with loaded content
  cf::registerOperation<...>(apiliteral::operation::writedocument, ...);
} catch (...) {
  // Source file not required for non-write operations
}
```

### Operation Dispatch

Operations are stored in a map and invoked by name:

```cpp
if (cf::getOperations().count(options->operation()) == 0) {
  return Failure("Unsupported operation "s + options->operation());
}
return cf::getOperations().at(options->operation())->invoke();
```

## Relationship to Other Components

```
conftool.cc (executable)
     |
     v
conftoolifc.h (this file - template implementation)
     |
     +---> options_operations.h (option classes)
     |
     +---> configuration_common.h (operation functions)
     |
     +---> dispatch_signatures.h (operation registration)
```

- **Utilities/conftool.cc**: The main executable that uses this template
- **options_operations.h**: Aggregates all option class headers
- **configuration_common.h**: Provides operation function declarations
- **dispatch_signatures.h**: Provides the `registerOperation` infrastructure

## Common Pitfalls

### Missing Result File for Read Operations

```cpp
// DON'T: Forget --result for read operations
// conftool --operation=readdocument --collection=Configs
// ERROR: "no result file name given"

// DO: Always specify result file for read operations
// conftool --operation=readdocument --collection=Configs --result=output.json
```

### Invalid Operation Names

```cpp
// DON'T: Use incorrect operation names
// conftool --operation=read  // ERROR: "Unsupported operation read"

// DO: Use exact operation names from the table above
// conftool --operation=readdocument
```

### Implementing Custom Options Classes

```cpp
// DON'T: Miss required interface methods
class MyOptions {
  // Missing makeProgramOptions(), etc.
};
// COMPILE ERROR or runtime failure

// DO: Implement all required methods
class MyOptions {
public:
  bpo::options_description makeProgramOptions() { ... }
  process_exit_code readProgramOptions(bpo::variables_map& vm) { ... }
  std::string processName() const { return "mytool"; }
  std::string operation() const { return _operation; }
  std::string resultFileName() const { return _result; }
  std::string to_string() const { ... }
private:
  std::string _operation;
  std::string _result;
};
```

## See Also

- [configuration_common.h](./configuration_common.h.md) - Operation function declarations and types
- [options_operations.h](./options_operations.h.md) - Aggregated options class headers
- [dispatch_signatures.h](./dispatch_signatures.h.md) - Operation registration infrastructure
- [options_operation_managedocument.h](./options_operation_managedocument.h.md) - Example options class
- [Utilities/conftool.cc](../Utilities/conftool.cc.md) - Main executable using this template

---

**Documentation generated for artdaq-database ConfigurationDB module**
