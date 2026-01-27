# options_operation_bulkoperations.cpp

**Path:** `artdaq-database/ConfigurationDB/options_operation_bulkoperations.cpp`

**Implements:** [options_operation_bulkoperations.h](./options_operation_bulkoperations.h.md)

**Purpose:** Implements the `BulkOperations` class which manages parsing and iteration of multiple document operations specified in a JSON array. This enables batch processing of configuration database operations for efficient bulk imports, exports, and modifications.

## Implementation Overview

This file implements bulk operation handling with the following key features:
1. JSON parsing of an operations array into individual `ManageDocumentOperation` objects
2. Automatic file loading when the bulkoperations argument is a file path
3. Iterator support for processing operations sequentially
4. String dequoting for handling various input formats

The implementation stores operations in a `std::list` of tuples, where each tuple contains the operation name and a unique pointer to the parsed operation object.

## Key Algorithms

### JSON Operations Array Parsing

The `readJsonData()` method parses the bulk operations JSON:

**Steps:**
1. Parse input JSON into AST using `JsonReader`
2. Store raw JSON in `_bulk_operations` for later reference
3. Extract the "operations" array from the AST
4. For each operation object in the array:
   - Extract the operation name from the "operation" field
   - Serialize the operation object back to JSON string
   - Create a new `ManageDocumentOperation` instance
   - Call `readJsonData()` on the new operation to populate its parameters
   - Add tuple of (name, operation) to the operations list

**Why this approach:** Re-serializing each operation to JSON before parsing into `ManageDocumentOperation` reuses the existing JSON parsing logic rather than duplicating field extraction code.

### File vs String Detection

The `readProgramOptions()` method auto-detects input type:

**Steps:**
1. Get the bulkoperations argument value
2. Attempt to open it as a file using `std::ifstream`
3. If file opens successfully, read entire contents as JSON
4. Otherwise, use the argument value directly as JSON
5. Pass JSON to `readJsonData()` for parsing

**Why this approach:** Allows users to provide JSON inline or as a file path without separate command-line flags.

## Internal Functions

### `BulkOperations::BulkOperations(std::string process_name)`

**Brief:** Constructor that stores the process name for creating child operations.

**Called by:** CLI tools and API entry points creating bulk operation handlers

**Purpose:** Initializes the container with process identification for logging.

### `bulkOperations() const -> std::string const&`

**Brief:** Returns the raw JSON specification string.

**Called by:** `to_JsonData()`, `to_string()`

**Purpose:** Provides access to the original JSON for serialization.

### `bulkOperations(std::string const& query_payload) -> std::string const&`

**Brief:** Sets the bulk operations JSON after applying dequoting.

**Called by:** `readJsonData()`

**Purpose:** Stores the JSON specification with normalization.

### `format() const -> data_format_t const&`

**Brief:** Returns the data format setting (defaults to `gui`).

**Called by:** Output formatting code

**Purpose:** Provides format hint for result serialization.

### `makeProgramOptions() const -> bpo::options_description`

**Brief:** Creates command-line options for bulk operations.

**Called by:** CLI tools building argument parsers

**Purpose:** Defines:
- `--help/-h` - Display help
- `--bulkoperations` - JSON string or file path
- `--result/-x` - Expected result file for testing

### `readProgramOptions(bpo::variables_map const& vm) -> int`

**Brief:** Reads command-line arguments, loading JSON from file if needed.

**Called by:** CLI main functions after argument parsing

**Purpose:** Processes bulkoperations argument with file/string auto-detection.

**Returns:**
- `process_exit_code::HELP` if help requested
- `process_exit_code::SUCCESS` otherwise

### `readJsonData(JsonData const& data) -> void`

**Brief:** Parses JSON operations array and populates the operations list.

**Called by:** `readProgramOptions()`, API entry points

**Purpose:** Main parsing logic that creates `ManageDocumentOperation` objects.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `invalid_option_exception` | When JSON parsing fails |
| `invalid_option_exception` | When operations array serialization fails |

### `to_JsonData() const -> JsonData`

**Brief:** Returns the bulk operations specification as JsonData.

**Called by:** Serialization code

**Purpose:** Provides JSON representation of bulk operations.

### `to_string() const -> std::string`

**Brief:** Returns string representation by delegating to `to_JsonData()`.

**Called by:** Logging, TraceStreamer operator

**Purpose:** Provides string serialization.

### `cf::debug::options::BulkOperations() -> void`

**Brief:** Enables TRACE debugging for bulk operations handling.

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
| `artdaq-database/ConfigurationDB/options_operation_bulkoperations.h` | Class declaration |
| `artdaq-database/ConfigurationDB/options_operation_managedocument.h` | ManageDocumentOperation for each bulk item |
| `artdaq-database/ConfigurationDB/shared_helper_functions.h` | Helper utilities |
| `artdaq-database/DataFormats/shared_literals.h` | Literal strings |
| `artdaq-database/BasicTypes/basictypes.h` | JsonData type |
| `artdaq-database/DataFormats/Json/json_reader.h` | JSON parsing |
| `artdaq-database/DataFormats/Json/json_writer.h` | JSON generation |
| `<fstream>` | File I/O for loading JSON from files |

## TRACE Logging Levels

| Level | Purpose |
|-------|---------|
| 10 | Debug function enable confirmation |
| 25 | Bulk operations JSON changes |

## Performance Considerations

- Re-serializing each operation to JSON and re-parsing adds overhead; for very large operation counts, a direct AST-based extraction would be more efficient
- Operations are stored in `std::list` for stable iterators during iteration; `std::vector` would have better cache locality but iterators may be invalidated
- File reading loads entire contents into memory; for very large files, streaming would be more memory-efficient

## Error Handling Strategy

- JSON parsing failures throw `invalid_option_exception` with descriptive messages
- File open failures are silent (falls back to treating input as JSON string)
- Individual operation parsing failures propagate from `ManageDocumentOperation::readJsonData()`
- The `confirm()` macro provides debug assertions

## Testing Notes

- **Unit tests:** `test/ConfigurationDB/bulkoperations_t.cc`
- **Key test cases:**
  - JSON string parsing
  - File-based JSON loading
  - Multiple operations iteration
  - Error handling for malformed JSON

## Maintenance Notes

- The file detection logic uses `is.good()` after attempting to open - this may have false positives if the JSON string happens to match a readable file path
- All operations are created as `ManageDocumentOperation` regardless of operation type; this works because `ManageDocumentOperation` is the most general operation class
- The `_data_format` member is initialized but never modified by any method

## Example JSON Input

```json
{
  "operations": [
    {
      "operation": "writedocument",
      "provider": "filesystem",
      "collection": "artdaq_configurations",
      "format": "gui",
      "entity": "BoardReader01",
      "version": "v1.0",
      "configuration": "demo_config"
    },
    {
      "operation": "writedocument",
      "provider": "filesystem",
      "collection": "artdaq_configurations",
      "format": "gui",
      "entity": "EventBuilder01",
      "version": "v1.0",
      "configuration": "demo_config"
    }
  ]
}
```

## See Also

- [options_operation_bulkoperations.h](./options_operation_bulkoperations.h.md) - Header file
- [options_operation_managedocument.h](./options_operation_managedocument.h.md) - Operation type created for each item
- [options_operations.h](./options_operations.h.md) - Header aggregator including this file

---

**Documentation generated for artdaq-database ConfigurationDB module**
