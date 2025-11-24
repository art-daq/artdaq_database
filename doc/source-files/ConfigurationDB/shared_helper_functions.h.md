# shared_helper_functions.h / shared_helper_functions.cpp

## File Overview

Defines the `data_format_t` enumeration and provides conversion functions between format enums and string representations. Essential for handling different data formats throughout the configuration database.

**Location**:
- `/home/user/artdaq-database/artdaq-database/ConfigurationDB/shared_helper_functions.h`
- `/home/user/artdaq-database/artdaq-database/ConfigurationDB/shared_helper_functions.cpp`

**Lines of Code**: 21 (header) + implementation

**Purpose**: Data format enumeration and conversion utilities

## Dependencies

### Standard Library
- `<string>` - String operations
- `<vector>` - Collections (in implementation)

## Namespace: artdaq::database::configuration

## Enums

### data_format_t

```cpp
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
```

**Purpose**: Strongly-typed enumeration of supported data formats.

**Usage**:
```cpp
using artdaq::database::configuration::options::data_format_t;

data_format_t format = data_format_t::json;
if (format == data_format_t::fhicl) {
    // Handle FHiCL format
}
```

**Format Descriptions**:
- **unknown**: Uninitialized or invalid format
- **origin**: Original format (format-agnostic)
- **json**: JavaScript Object Notation - human-readable structured data
- **fhicl**: Fermilab's configuration language - specialized for physics applications
- **gui**: Format optimized for GUI display and manipulation
- **db**: Internal database format with full metadata
- **xml**: Extensible Markup Language
- **csv**: Comma-Separated Values - tabular data

## Functions

### to_string

```cpp
std::string to_string(options::data_format_t const& format)
```

**Purpose**: Convert data format enum to string representation.

**Parameters**:
- `format` - Data format enum value

**Returns**: String name of the format ("json", "fhicl", etc.)

**Usage**:
```cpp
auto format = data_format_t::json;
std::string name = to_string(format);  // Returns "json"

std::cout << "Using format: " << to_string(format) << "\n";
```

**String Values**:
- `data_format_t::unknown` → "unknown"
- `data_format_t::origin` → "origin"
- `data_format_t::json` → "json"
- `data_format_t::fhicl` → "fhicl"
- `data_format_t::gui` → "gui"
- `data_format_t::db` → "db"
- `data_format_t::xml` → "xml"
- `data_format_t::csv` → "csv"

---

### to_data_format

```cpp
options::data_format_t to_data_format(std::string const& format_string)
```

**Purpose**: Convert string representation to data format enum.

**Parameters**:
- `format_string` - String name of format ("json", "fhicl", etc.)

**Returns**: Corresponding `data_format_t` enum value

**Throws**: May throw or return `unknown` for invalid strings (implementation-dependent)

**Usage**:
```cpp
std::string user_input = "json";
auto format = to_data_format(user_input);

if (format == data_format_t::unknown) {
    std::cerr << "Invalid format: " << user_input << "\n";
}
```

**Case Sensitivity**: Typically case-insensitive (implementation detail)

**Error Handling**: Returns `data_format_t::unknown` for unrecognized strings

## Usage Examples

### Format Selection

```cpp
#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"

using artdaq::database::configuration::options::data_format_t;
using artdaq::database::configuration::to_string;
using artdaq::database::configuration::to_data_format;

void process_configuration(std::string format_name) {
    auto format = to_data_format(format_name);

    switch (format) {
        case data_format_t::json:
            std::cout << "Processing as JSON\n";
            break;
        case data_format_t::fhicl:
            std::cout << "Processing as FHiCL\n";
            break;
        case data_format_t::unknown:
            std::cerr << "Unknown format: " << format_name << "\n";
            break;
        default:
            std::cout << "Processing as " << to_string(format) << "\n";
    }
}
```

### Command-Line Options

```cpp
// Parse command line
std::string format_arg = vm["format"].as<std::string>();
auto format = to_data_format(format_arg);

if (format == data_format_t::unknown) {
    std::cerr << "Invalid format. Supported: json, fhicl, xml, csv\n";
    return 1;
}

// Use format
operation.format(format);
```

### Format Conversion

```cpp
void convert_format(data_format_t from, data_format_t to) {
    std::cout << "Converting from " << to_string(from)
              << " to " << to_string(to) << "\n";

    // Perform conversion logic...
}
```

## Best Practices

### Always Validate Unknown Format

```cpp
// GOOD: Check for unknown
auto format = to_data_format(user_input);
if (format == data_format_t::unknown) {
    // Handle error
}

// BAD: Assume valid
auto format = to_data_format(user_input);
// format might be unknown!
```

### Use Switch for Format Handling

```cpp
// GOOD: Exhaustive switch
switch (format) {
    case data_format_t::json:
        handle_json();
        break;
    case data_format_t::fhicl:
        handle_fhicl();
        break;
    case data_format_t::unknown:
        handle_error();
        break;
    // ... handle all cases
}

// BAD: Incomplete if-else chain
if (format == data_format_t::json) {
    handle_json();
}
// What about other formats?
```

### Never Use Unknown for Valid Data

```cpp
// GOOD: Use appropriate format
operation.format(data_format_t::json);

// BAD: Using unknown
operation.format(data_format_t::unknown);  // Will cause assertions!
```

## Related Files

- **options_operation_base.h** - Uses `data_format_t` for format specification
- **configurationdbifc.h** - Uses format enum in template methods
- All operation classes use this enum

**Documentation generated for artdaq-database ConfigurationDB module**
