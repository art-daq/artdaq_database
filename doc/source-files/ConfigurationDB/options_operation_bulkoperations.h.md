# options_operation_bulkoperations.h

**Path:** `artdaq-database/ConfigurationDB/options_operation_bulkoperations.h`

**Purpose:** Declares the `BulkOperations` class which provides a container for executing multiple document operations as a batch. This class manages a list of individual `ManageDocumentOperation` objects parsed from a JSON specification, enabling bulk import, export, and modification of configurations.


## Key Concepts

### Bulk Operations
Bulk operations allow executing multiple configuration database operations in a single batch. Instead of making individual API calls, users can specify a JSON array of operations that will be processed together, improving efficiency for large-scale configuration management tasks.

### Operation List
The class maintains an ordered list of operations, where each entry is a tuple of:
- Operation name (string identifying the operation type)
- Unique pointer to a `ManageDocumentOperation` containing the operation parameters

### JSON-Driven Configuration
Bulk operations are specified entirely in JSON format, either as a string or loaded from a file. The JSON contains an array of operations, each with its type and parameters.

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not safe for concurrent modification
- **Locking:** No internal locking; iteration is not thread-safe during modification

## Dependencies

| Include | Purpose |
|---------|---------|
| `<list>` | STL list container for operation storage |
| `<tuple>` | `std::tuple` for operation name + pointer pairs |
| `<memory>` | `std::unique_ptr` for operation ownership |
| `options_operation_base.h` | Base types and `data_format_t` enumeration |
| `<boost/program_options.hpp>` | Command-line argument parsing |

## Forward Declarations

| Type | Purpose |
|------|---------|
| `artdaq::database::basictypes::JsonData` | JSON data wrapper type |
| `TraceStreamer` | TRACE logging support for stream operators |

## Classes/Structures

### `BulkOperations`

A class that manages a list of document operations for batch execution. Unlike the operation base classes, this is not derived from `OperationBase` as it represents a collection of operations rather than a single operation.

**Thread Safety:** Not thread-safe; do not modify while iterating.

#### Type Definitions

##### `OperationBaseUPtr`

**Brief:** Unique pointer type alias for OperationBase objects.

```cpp
using OperationBaseUPtr = std::unique_ptr<OperationBase>;
```

##### `OperationsList`

**Brief:** Type alias for the list of operation tuples (name, operation pointer).

```cpp
using OperationsList = std::list<std::tuple<std::string, OperationBaseUPtr>>;
```

##### `iterator` / `const_iterator`

**Brief:** Iterator types for traversing the operations list.

```cpp
using iterator = std::list<std::tuple<std::string, OperationBaseUPtr>>::iterator;
using const_iterator = std::list<std::tuple<std::string, OperationBaseUPtr>>::const_iterator;
```

#### Constructor

##### `BulkOperations(std::string process_name)`

**Brief:** Constructs a BulkOperations container with the specified process name.

**Parameters:**
- `process_name` - Name of the invoking process for logging and child operation creation

**Preconditions:**
- None

**Postconditions:**
- Empty operations list initialized
- Data format set to `gui` by default

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/options_operation_bulkoperations.h"

using namespace artdaq::database::configuration;

void processBulkOps() {
  auto bulk = BulkOperations{"bulkloader"};

  // Load operations from JSON file
  bpo::variables_map vm;
  // ... parse command line with bulkoperations argument ...
  bulk.readProgramOptions(vm);

  // Iterate and execute
  for (auto& [opName, opPtr] : bulk) {
    std::cout << "Executing: " << opName << "\n";
    // Execute operation...
  }
}
```

#### Methods

##### `bulkOperations() const -> std::string const&`

**Brief:** Returns the raw JSON string containing the bulk operations specification.

**Preconditions:**
- Bulk operations should have been set (assertion fires if empty)

**Returns:** Reference to the JSON bulk operations string

**Thread Safety:** Safe for concurrent reads if no concurrent writes

##### `bulkOperations(std::string const& query_payload) -> std::string const&`

**Brief:** Sets the bulk operations JSON string after dequoting.

**Parameters:**
- `query_payload` - JSON string (possibly quoted) containing operations array

**Preconditions:**
- `query_payload` should not be empty (assertion fires)

**Returns:** Reference to the stored bulk operations string

**Postconditions:**
- String is dequoted and stored
- TRACE log entry at level 25

**Thread Safety:** Not safe for concurrent access

##### `format() const -> data_format_t const&`

**Brief:** Returns the data format for bulk operations output.

**Returns:** Reference to the data format (defaults to `gui`)

**Thread Safety:** Safe for concurrent reads

##### `to_JsonData() const -> JsonData`

**Brief:** Returns the bulk operations specification as JsonData.

**Returns:** JsonData containing the bulk operations JSON

**Thread Safety:** Safe for concurrent reads if no concurrent writes

##### `to_string() const -> std::string`

**Brief:** Returns the string representation of the bulk operations.

**Returns:** The bulk operations JSON as a string

**Thread Safety:** Safe for concurrent reads if no concurrent writes

##### `makeProgramOptions() const -> bpo::options_description`

**Brief:** Creates Boost.Program_Options description for bulk operation command-line options.

**Returns:** Options description including:
- `-h/--help` - Help message
- `--bulkoperations` - JSON string or file path containing operations
- `-x/--result` - Expected result file name

**Thread Safety:** Safe (creates new object)

##### `readProgramOptions(bpo::variables_map const& vm) -> int`

**Brief:** Reads command-line options and loads bulk operations from JSON string or file.

**Parameters:**
- `vm` - Boost.Program_Options variables map

**Returns:**
- `process_exit_code::HELP` if help was requested
- `process_exit_code::SUCCESS` on successful parsing

**Postconditions:**
- If bulkoperations argument is a file path, file contents are loaded
- Operations list is populated from JSON

**Thread Safety:** Not safe for concurrent access

**Example:**
```cpp
BulkOperations bulk{"myapp"};
bpo::variables_map vm;
bpo::store(bpo::parse_command_line(argc, argv, bulk.makeProgramOptions()), vm);
bpo::notify(vm);

int result = bulk.readProgramOptions(vm);
if (result == process_exit_code::HELP) {
  return 0;
}
```

##### `readJsonData(JsonData const& data) -> void`

**Brief:** Parses JSON data containing an operations array and populates the operations list.

**Parameters:**
- `data` - JSON data containing "operations" array

**Preconditions:**
- `data` must not be empty
- JSON must contain an "operations" array

**Postconditions:**
- Operations list populated with `ManageDocumentOperation` objects
- Each operation has its parameters set from the JSON

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON parsing fails or operations array missing |

**Thread Safety:** Not safe for concurrent access

##### `begin() -> iterator`

**Brief:** Returns an iterator to the first operation in the list.

**Returns:** Iterator to the beginning of the operations list

**Thread Safety:** Safe if no concurrent modifications

##### `end() -> iterator`

**Brief:** Returns an iterator past the last operation in the list.

**Returns:** Iterator to the end of the operations list

**Thread Safety:** Safe if no concurrent modifications

##### `begin() const -> const_iterator`

**Brief:** Returns a const iterator to the first operation in the list.

**Returns:** Const iterator to the beginning

**Thread Safety:** Safe if no concurrent modifications

##### `end() const -> const_iterator`

**Brief:** Returns a const iterator past the last operation in the list.

**Returns:** Const iterator to the end

**Thread Safety:** Safe if no concurrent modifications

## Functions

### `debug::options::BulkOperations() -> void`

**Brief:** Enables TRACE debugging for BulkOperations parsing and iteration.

**Side Effects:**
- Configures TRACE logging for this module

## TraceStreamer Operator

### `operator<<(TraceStreamer&, BulkOperations const&) -> TraceStreamer&`

**Brief:** Stream insertion operator for logging BulkOperations to TraceStreamer.

**Parameters:**
- First parameter: TraceStreamer reference
- Second parameter: BulkOperations to serialize

**Returns:** Reference to TraceStreamer for chaining

## JSON Format

The bulk operations JSON should follow this structure:

```json
{
  "operations": [
    {
      "operation": "writedocument",
      "collection": "artdaq_configurations",
      "entity": "BoardReader01",
      "version": "v1.0",
      "configuration": "demo_config"
    },
    {
      "operation": "writedocument",
      "collection": "artdaq_configurations",
      "entity": "EventBuilder01",
      "version": "v1.0",
      "configuration": "demo_config"
    }
  ]
}
```

## Relationship to Other Components

`BulkOperations` is used by:
- **bulkloader** CLI tool - For batch configuration imports
- **bulkdownloader** CLI tool - For batch configuration exports
- **API bulk endpoints** - For programmatic batch operations

The class creates `ManageDocumentOperation` objects internally for each operation in the array.

## See Also

- [options_operation_bulkoperations.cpp](./options_operation_bulkoperations.cpp.md) - Implementation
- [options_operation_managedocument.h](./options_operation_managedocument.h.md) - Operation type created for each bulk item
- [bulkloader.cc](../Utilities/bulkloader.cc.md) - Primary user of this class

## Notes for Developers

### Common Pitfalls

- **File vs JSON string:** The `readProgramOptions()` method auto-detects whether the argument is a file path or JSON string by attempting to open it as a file first
- **Operation ownership:** Operations are stored as unique pointers; do not keep raw pointers after iteration

### Usage Pattern

```cpp
// Typical bulk operations workflow
BulkOperations bulk{"bulkloader"};

// Load from file or command line
bulk.readProgramOptions(vm);  // or bulk.readJsonData(jsonData);

// Process each operation
for (auto it = bulk.begin(); it != bulk.end(); ++it) {
  auto& [operationName, operationPtr] = *it;

  // Execute based on operation type
  if (operationName == "writedocument") {
    // Perform write operation using operationPtr
  }
}
```

---

**Documentation generated for artdaq-database ConfigurationDB module**
