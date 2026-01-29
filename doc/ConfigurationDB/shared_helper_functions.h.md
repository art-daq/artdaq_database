# shared_helper_functions.h

**Path:** `artdaq-database/ConfigurationDB/shared_helper_functions.h`

**Purpose:** Defines the `data_format_t` enumeration and provides conversion functions between format enums and string representations. This header is essential for handling different data formats (JSON, FHiCL, XML, etc.) throughout the configuration database system, enabling format selection and validation.


## Key Concepts

### Data Format Enumeration

The `data_format_t` enum provides a strongly-typed way to represent supported data formats. Using an enum instead of strings prevents typos, enables compile-time checking, and allows efficient switch statements.

### Format Conversion Pattern

The bidirectional conversion functions `to_string()` and `to_data_format()` enable:
- User-friendly string input/output (command line, configuration files)
- Type-safe internal processing
- Easy validation of format specifications

## Thread Safety

- **Thread-safe:** Yes
- **Concurrent access:** Safe for all functions (stateless conversions)
- **Locking:** None required

All functions are pure conversions with no shared state, making them inherently thread-safe.

## Dependencies

| Include | Purpose |
|---------|---------|
| `<string>` | std::string for format name representations |
| `<vector>` | std::vector for potential collections (included for future use) |

## Enumerations

### `data_format_t`

```cpp
namespace artdaq::database::configuration::options {

enum struct data_format_t {
  unknown,  // Unknown or uninitialized format
  origin,   // Original/native format
  json,     // JSON format
  fhicl,    // FHiCL (Fermilab Hierarchical Configuration Language)
  gui,      // GUI-friendly format
  db,       // Database internal format
  xml,      // XML format
  csv       // CSV format
};

}  // namespace options
```

**Brief:** Strongly-typed enumeration of all supported data formats in the configuration database system.

**Format Descriptions:**

| Value | String | Description |
|-------|--------|-------------|
| `unknown` | `"unknown"` | Uninitialized or invalid format - indicates an error condition |
| `origin` | `"origin"` | Original format as stored - no conversion applied |
| `json` | `"json"` | JavaScript Object Notation - human-readable structured data |
| `fhicl` | `"fhicl"` | Fermilab Hierarchical Configuration Language - specialized for physics applications |
| `gui` | `"gui"` | Format optimized for GUI display and manipulation |
| `db` | `"db"` | Internal database format with full metadata |
| `xml` | `"xml"` | Extensible Markup Language |
| `csv` | `"csv"` | Comma-Separated Values - tabular data |

**Thread Safety:** Enum values are immutable and thread-safe.

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"

using namespace artdaq::database::configuration::options;

void selectFormat(data_format_t format) {
  switch (format) {
    case data_format_t::json:
      processAsJson();
      break;
    case data_format_t::fhicl:
      processAsFhicl();
      break;
    case data_format_t::unknown:
      handleInvalidFormat();
      break;
    default:
      processGeneric(format);
      break;
  }
}
```

## Functions

### `to_string(format) -> std::string`

```cpp
std::string to_string(options::data_format_t const& format);
```

**Brief:** Converts a data format enumeration value to its string representation for display, logging, or serialization.

**Parameters:**
- `format` - The data format enumeration value to convert

**Returns:** String name of the format (e.g., `"json"`, `"fhicl"`, `"unknown"`)

**Postconditions:**
- Always returns a valid, non-empty string
- Returns `"unknown"` for unrecognized enum values (including `data_format_t::unknown`)

**Thread Safety:** Safe (pure function, no state)

**Complexity:** O(1) - simple switch statement

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"

using namespace artdaq::database::configuration;
using namespace artdaq::database::configuration::options;

void logFormat(data_format_t format) {
  std::cout << "Using format: " << to_string(format) << std::endl;
  // Possible outputs: "json", "fhicl", "xml", "csv", "gui", "db", "origin", "unknown"
}

void example() {
  auto format = data_format_t::json;
  std::string name = to_string(format);  // Returns "json"

  // Use in logging
  TLOG(10) << "Processing configuration in " << to_string(format) << " format";
}
```

---

### `to_data_format(format_string) -> data_format_t`

```cpp
options::data_format_t to_data_format(std::string const& format_string);
```

**Brief:** Converts a string representation of a data format to its corresponding enumeration value. Used for parsing user input and configuration files.

**Parameters:**
- `format_string` - String name of the format (e.g., `"json"`, `"fhicl"`)

**Preconditions:**
- `format_string` should be non-empty (asserted with `confirm()`)

**Returns:** Corresponding `data_format_t` enumeration value, or `data_format_t::unknown` for unrecognized strings

**Postconditions:**
- Returns a valid `data_format_t` value
- Returns `data_format_t::unknown` for empty or unrecognized input

**Thread Safety:** Safe (pure function, no state)

**Complexity:** O(n) where n is the number of format types (currently 7 comparisons)

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"

using namespace artdaq::database::configuration;
using namespace artdaq::database::configuration::options;

data_format_t parseUserInput(const std::string& input) {
  auto format = to_data_format(input);

  if (format == data_format_t::unknown) {
    std::cerr << "Invalid format: " << input << std::endl;
    std::cerr << "Supported formats: json, fhicl, xml, csv, gui, db, origin" << std::endl;
    return data_format_t::unknown;
  }

  return format;
}

void processCommandLine(int argc, char* argv[]) {
  // Parse --format=json argument
  std::string format_arg = getArgument(argc, argv, "--format");
  auto format = to_data_format(format_arg);

  if (format == data_format_t::unknown) {
    throw std::invalid_argument("Unsupported format: " + format_arg);
  }

  // Use the format
  processConfiguration(format);
}
```

## Usage Examples

### Format Selection in Operations

```cpp
#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"

using namespace artdaq::database::configuration;
using namespace artdaq::database::configuration::options;

void convertConfiguration(const std::string& source_format,
                          const std::string& target_format) {
  auto from = to_data_format(source_format);
  auto to = to_data_format(target_format);

  if (from == data_format_t::unknown || to == data_format_t::unknown) {
    throw std::invalid_argument("Invalid format specification");
  }

  std::cout << "Converting from " << to_string(from)
            << " to " << to_string(to) << std::endl;

  // Perform conversion...
}
```

### Validation in Options Classes

```cpp
#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/ConfigurationDB/options_operation_base.h"

using namespace artdaq::database::configuration;
using namespace artdaq::database::configuration::options;

class MyOperation : public OperationBase {
public:
  void setFormat(const std::string& format_name) {
    auto format = to_data_format(format_name);

    if (format == data_format_t::unknown) {
      throw artdaq::database::invalid_option_exception("MyOperation")
          << "Unknown format: " << format_name;
    }

    _format = format;
  }

  std::string formatAsString() const {
    return to_string(_format);
  }

private:
  data_format_t _format = data_format_t::json;  // Default to JSON
};
```

### Switch Statement Pattern

```cpp
#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"

using namespace artdaq::database::configuration::options;

std::string getFileExtension(data_format_t format) {
  switch (format) {
    case data_format_t::json:
      return ".json";
    case data_format_t::fhicl:
      return ".fcl";
    case data_format_t::xml:
      return ".xml";
    case data_format_t::csv:
      return ".csv";
    case data_format_t::gui:
      return ".gui.json";
    case data_format_t::db:
      return ".db.json";
    case data_format_t::origin:
    case data_format_t::unknown:
    default:
      return "";
  }
}
```

## Relationship to Other Components

The `data_format_t` enum is used throughout the ConfigurationDB module:

```
shared_helper_functions.h (this file)
        |
        +---> options_operation_base.h (format specification)
        |
        +---> configurationdbifc.h (format selection for I/O)
        |
        +---> DataFormats/* (format-specific converters)
```

- **options_operation_base.h**: Uses `data_format_t` for the `format()` option
- **configurationdbifc.h**: Uses format enum for template methods
- **DataFormats modules**: Implement converters for each format type

## Common Pitfalls

### Not Checking for Unknown Format

```cpp
// DON'T: Assume conversion always succeeds
auto format = to_data_format(user_input);
processAsFormat(format);  // May process as "unknown"!

// DO: Always validate the result
auto format = to_data_format(user_input);
if (format == data_format_t::unknown) {
  throw std::invalid_argument("Invalid format: " + user_input);
}
processAsFormat(format);
```

### Using data_format_t::unknown as Valid Format

```cpp
// DON'T: Use unknown as a default that might be processed
data_format_t format = data_format_t::unknown;
// ... forget to set it ...
writeDocument(format);  // Will cause assertion or error

// DO: Use a valid default or require explicit setting
data_format_t format = data_format_t::json;  // Safe default
// OR
std::optional<data_format_t> format;  // Explicit "not set"
```

### Case Sensitivity Assumptions

```cpp
// DON'T: Assume case-insensitive matching
auto format = to_data_format("JSON");  // Returns unknown!

// DO: Ensure lowercase input
std::string input = "JSON";
std::transform(input.begin(), input.end(), input.begin(), ::tolower);
auto format = to_data_format(input);  // Returns json
```

## Notes for Developers

### Adding New Formats

To add a new data format:

1. Add the enum value to `data_format_t` in this header
2. Add the case to `to_string()` in `shared_helper_functions.cpp`
3. Add the case to `to_data_format()` in `shared_helper_functions.cpp`
4. Update any switch statements that handle all formats
5. Implement format-specific converters if needed

### String Format Convention

All format strings are lowercase, single-word identifiers:
- `"json"` (not `"JSON"` or `"Json"`)
- `"fhicl"` (not `"FHiCL"` or `"FHICL"`)

This convention simplifies comparison and user input handling.

## See Also

- [shared_helper_functions.cpp](./shared_helper_functions.cpp.md) - Implementation of conversion functions
- [options_operation_base.h](./options_operation_base.h.md) - Uses data_format_t for format options
- [configurationdbifc.h](./configurationdbifc.h.md) - Uses format enum in template methods
- [DataFormats/README.md](../DataFormats/README.md) - Format-specific converter implementations

---

**Documentation generated for artdaq-database ConfigurationDB module**
