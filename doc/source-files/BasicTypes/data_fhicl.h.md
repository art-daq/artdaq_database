# data_fhicl.h

## File Overview

**Location**: `/home/user/artdaq-database/artdaq-database/BasicTypes/data_fhicl.h`

This header defines the `FhiclData` class, which represents FHICL (Fermilab Hierarchical Configuration Language) formatted configuration data. FHICL is the primary configuration language used throughout the artdaq ecosystem for configuring DAQ systems.

**Purpose**: Provides a wrapper class for FHICL configuration data with conversion capabilities to/from JSON format.

## Dependencies

- `artdaq-database/BasicTypes/common.h` - Common utilities and TRACE logging support

### TRACE Configuration

```cpp
#define TRACE_NAME "data_fhicl.h"
```

## Namespace Structure

```cpp
namespace artdaq {
namespace database {
namespace basictypes {
    // FhiclData is defined here
}}}
```

## Key Types/Classes

### Forward Declarations

```cpp
struct JsonData;
```

**Purpose**: Forward declaration of `JsonData` to enable conversion operators without circular dependencies.

---

### FhiclData

```cpp
struct FhiclData final {
    FhiclData(std::string);
    FhiclData() = default;

    FhiclData(JsonData const&);
    operator JsonData() const;
    operator std::string const&() const;

    static constexpr auto type_version() { return "V100"; }

    std::string fhicl_buffer = "";
    std::string fhicl_file_name = "notprovided";
};
```

**Purpose**: A wrapper class for FHICL configuration data that provides:
1. Storage for FHICL-formatted strings
2. Bidirectional conversion with JSON format
3. File name tracking for debugging/logging
4. Stream I/O operators

**Design**: Marked `final` - cannot be inherited from

#### Member Variables

##### fhicl_buffer

```cpp
std::string fhicl_buffer = "";
```

**Purpose**: Holds the actual FHICL-formatted configuration string.

**Default Value**: Empty string

**Example Content**:
```fhicl
parameter1: value1
parameter2: {
    nested_param: value2
}
array_param: [1, 2, 3]
```

---

##### fhicl_file_name

```cpp
std::string fhicl_file_name = "notprovided";
```

**Purpose**: Stores the original filename from which the FHICL data was loaded (if applicable).

**Default Value**: "notprovided"

**Use Cases**:
- Debugging: Identify source of configuration
- Logging: Track which file caused errors
- Error messages: Provide context in exceptions
- Auditing: Record configuration sources

**Note**: This is metadata and doesn't affect the actual configuration content.

## Constructors

### FhiclData(std::string)

```cpp
FhiclData(std::string buffer);
```

**Purpose**: Constructs a FhiclData object from a FHICL string.

**Parameters**:
- `buffer` - A string containing FHICL-formatted configuration data

**Implementation**: Uses move semantics (see data_fhicl.cpp.md)

**Usage Example**:
```cpp
std::string config = R"(
    daq_parameter: 1000
    buffer_size: 4096
)";
FhiclData fhicl(config);
```

---

### FhiclData() (default)

```cpp
FhiclData() = default;
```

**Purpose**: Default constructor creates an empty FhiclData object.

**Result**:
- `fhicl_buffer = ""`
- `fhicl_file_name = "notprovided"`

**Usage Example**:
```cpp
FhiclData fhicl;  // Empty FHICL object
// Later populate via stream or assignment
```

---

### FhiclData(JsonData const&)

```cpp
FhiclData(JsonData const& document);
```

**Purpose**: Constructs a FhiclData object by converting from JSON format.

**Parameters**:
- `document` - JsonData object containing FHICL data encoded in JSON

**Process**:
1. Extracts Base64-encoded FHICL from JSON
2. Decodes Base64 to get intermediate JSON
3. Converts JSON to FHICL format
4. Stores result in `fhicl_buffer`

**Throws**: `std::runtime_error` if conversion fails

**Usage Example**:
```cpp
JsonData json(R"({"base64": "cGFyYW1ldGVyOiB2YWx1ZQ=="})");
FhiclData fhicl(json);  // Converts JSON → FHICL
```

**See**: data_fhicl.cpp.md for implementation details

## Methods/Functions

### Conversion to JsonData

```cpp
operator JsonData() const;
```

**Purpose**: Converts FHICL data to JSON format.

**Return Value**: JsonData object containing the FHICL data encoded in JSON with Base64

**Process**:
1. Converts FHICL to JSON structure
2. Encodes FHICL buffer in Base64
3. Creates JSON document with encoded data
4. Returns JsonData object

**Throws**: `std::runtime_error` if conversion fails

**Usage Example**:
```cpp
FhiclData fhicl("parameter: value");
JsonData json = fhicl;  // Implicit conversion
// json now contains Base64-encoded FHICL in JSON format
```

---

### Conversion to String

```cpp
operator std::string const&() const;
```

**Purpose**: Provides access to the raw FHICL buffer string.

**Return Value**: Const reference to `fhicl_buffer`

**Usage Example**:
```cpp
FhiclData fhicl("parameter: value");
std::string config = fhicl;  // Implicit conversion
std::cout << fhicl;  // Works via this operator
```

---

### type_version (static)

```cpp
static constexpr auto type_version() { return "V100"; }
```

**Purpose**: Returns the version identifier for the FhiclData type.

**Return Value**: String literal "V100"

**Usage**: Version tracking for database schema compatibility.

**Note**: Used when creating collection names (e.g., "FhiclData_V100")

## Stream Operators

### operator>>

```cpp
std::istream& operator>>(std::istream& is, artdaq::database::basictypes::FhiclData& data);
```

**Purpose**: Reads FHICL data from an input stream (in JSON format).

**Parameters**:
- `is` - Input stream
- `data` - FhiclData object to populate

**Return Value**: Reference to the stream (for chaining)

**Process**:
1. Reads entire stream into a string
2. Creates JsonData from the string
3. Converts JsonData to FhiclData
4. Stores result in `data`

**Note**: Input is expected to be JSON-encoded FHICL, not raw FHICL

**Usage Example**:
```cpp
std::ifstream file("config.json");
FhiclData fhicl;
file >> fhicl;  // Reads JSON, converts to FHICL
```

---

### operator<<

```cpp
std::ostream& operator<<(std::ostream& os, artdaq::database::basictypes::FhiclData const& data);
```

**Purpose**: Writes raw FHICL data to an output stream.

**Parameters**:
- `os` - Output stream
- `data` - FhiclData object to write

**Return Value**: Reference to the stream (for chaining)

**Output Format**: Raw FHICL text (not JSON-encoded)

**Usage Example**:
```cpp
FhiclData fhicl("parameter: value");
std::cout << fhicl;  // Outputs: parameter: value

std::ofstream file("config.fcl");
file << fhicl;  // Writes raw FHICL to file
```

## TRACE Integration

### TraceStreamer Specialization

```cpp
namespace {
template <>
inline TraceStreamer& TraceStreamer::operator<<(const artdaq::database::basictypes::FhiclData& r) {
    std::ostringstream s;
    s << r;
    msg_append(s.str().c_str());
    return *this;
}
}
```

**Purpose**: Allows FhiclData objects to be used in TRACE logging statements.

**Usage Example**:
```cpp
FhiclData fhicl("debug_level: 10");
TLOG(5) << "Configuration: " << fhicl;  // Works via this specialization
```

## Usage Context

### Role in artdaq-database

FHICL is the **primary configuration language** for artdaq systems:

```
FHICL Config File → FhiclData → JsonData → Database
Database → JsonData → FhiclData → FHICL Config File
```

### Typical Usage Patterns

#### 1. Load FHICL Configuration

```cpp
// From string
FhiclData config(R"(
    buffer_size: 8192
    timeout_ms: 1000
)");

// From file (as JSON)
std::ifstream file("config.json");
FhiclData config2;
file >> config2;
```

#### 2. Store in Database (via JSON)

```cpp
FhiclData fhicl("parameter: value");
JsonData json = fhicl;  // Convert to JSON
// Store json in database
```

#### 3. Retrieve from Database

```cpp
// Retrieve json from database
JsonData json = get_from_database();
FhiclData fhicl(json);  // Convert to FHICL
std::cout << fhicl;     // Use the configuration
```

#### 4. Convert Formats

```cpp
// FHICL ↔ JSON conversions
FhiclData fhicl("param: value");
JsonData json = fhicl;         // FHICL → JSON
FhiclData back = json;         // JSON → FHICL
```

## What is FHICL?

**FHICL (Fermilab Hierarchical Configuration Language)** is a configuration language developed at Fermilab for configuring high-energy physics experiments.

### FHICL Syntax Examples

```fhicl
# Simple parameter
parameter: value

# Nested structure
module: {
    name: "MyModule"
    settings: {
        threshold: 100
        enabled: true
    }
}

# Arrays
channels: [0, 1, 2, 3, 4]

# References
base_config: {
    timeout: 1000
}
my_config: @local::base_config
```

### Why FHICL?

- **Hierarchical**: Supports nested configurations
- **Readable**: Human-friendly syntax
- **Powerful**: Supports references, includes, and substitutions
- **Standard**: Used throughout artdaq ecosystem

## JSON Encoding Format

When FHICL is stored in JSON (for database), it uses Base64 encoding:

```json
{
    "base64": "cGFyYW1ldGVyOiB2YWx1ZQ=="
}
```

This ensures:
- FHICL special characters are preserved
- Binary data (if any) is safely encoded
- JSON remains valid

## Header Guards

```cpp
#ifndef _ARTDAQ_DATABASE_BASICTYPES_FHICL_H_
#define _ARTDAQ_DATABASE_BASICTYPES_FHICL_H_
```

## Notes for Developers

### File Name Metadata

The `fhicl_file_name` member is informational:
- Not used in comparisons
- Not used in conversions
- Helpful for debugging
- Consider setting it when loading from files

### Conversion Path

All FHICL↔JSON conversions go through:
1. `FhiclData` ↔ intermediate JSON ↔ Base64 ↔ `JsonData`

### Error Handling

Conversions can throw `std::runtime_error`:
- Invalid JSON format
- Base64 decoding failures
- FHICL parsing errors

Always wrap conversions in try-catch when dealing with untrusted input.

### Thread Safety

The class is **not thread-safe** for concurrent writes.

## Best Practices

1. **Set File Names**: When loading from files, set `fhicl_file_name` for debugging
2. **Validate Input**: Check FHICL syntax before creating FhiclData objects
3. **Handle Errors**: Wrap conversions in try-catch blocks
4. **Use Constants**: Define configuration constants rather than string literals

## Example: Complete Workflow

```cpp
#include "artdaq-database/BasicTypes/data_fhicl.h"
#include "artdaq-database/BasicTypes/data_json.h"
#include <fstream>

try {
    // 1. Create FHICL configuration
    FhiclData config(R"(
        daq_settings: {
            buffer_size: 8192
            timeout_ms: 1000
        }
    )");
    config.fhicl_file_name = "myconfig.fcl";

    // 2. Convert to JSON for database storage
    JsonData json = config;

    // 3. Store to database (pseudo-code)
    database.store("configurations", json);

    // 4. Retrieve from database
    JsonData retrieved = database.get("configurations");

    // 5. Convert back to FHICL
    FhiclData restored(retrieved);

    // 6. Use the configuration
    std::cout << "Config from " << restored.fhicl_file_name
              << ":\n" << restored << "\n";

} catch (const std::runtime_error& e) {
    std::cerr << "Configuration error: " << e.what() << "\n";
}
```

## Related Documentation

- `data_fhicl.cpp.md` - Implementation details and conversion logic
- `data_fhicl_fusion.h.md` - Boost.Fusion adaptation
- `data_json.h.md` - JSON data type for conversions
- FHICL documentation: See artdaq documentation
