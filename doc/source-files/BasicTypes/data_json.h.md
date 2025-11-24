# data_json.h

## File Overview

**Location**: `/home/user/artdaq-database/artdaq-database/BasicTypes/data_json.h`

This header defines the `JsonData` class, which is the central data type for representing JSON-formatted configuration data in the artdaq-database system. It serves as both a standalone JSON container and an intermediary format for converting between other data types (FHICL and XML).

**Purpose**: Provides a wrapper class for JSON data with conversion capabilities to/from other configuration formats.

## Dependencies

- `artdaq-database/BasicTypes/common.h` - Common utilities and TRACE logging support
- `trace.h` (via common.h) - TRACE logging framework

### TRACE Configuration

```cpp
#define TRACE_NAME "data_json.h"
```

## Namespace Structure

```cpp
namespace artdaq {
namespace database {
namespace basictypes {
    // JsonData is defined here
}}}
```

## Key Types/Classes

### JsonData

```cpp
struct JsonData final {
    JsonData(std::string);

    template <typename TYPE>
    bool convert_to(TYPE&) const;

    template <typename TYPE>
    bool convert_from(TYPE const&);

    operator std::string const&() const;
    operator std::string&();

    bool empty() const;

    static constexpr auto type_version() { return "V1.0.0"; }

    std::string json_buffer;
};
```

**Purpose**: A wrapper class for JSON data that provides:
1. Storage for JSON-formatted strings
2. Conversion capabilities to/from other formats (FHICL, XML)
3. Stream I/O operators
4. Version tracking

**Design**: Marked `final` - cannot be inherited from

#### Member Variables

##### json_buffer

```cpp
std::string json_buffer;
```

**Purpose**: Holds the actual JSON-formatted string data.

**Access**: Public member, can be accessed directly or through conversion operators.

## Constructors

### JsonData(std::string)

```cpp
JsonData(std::string buffer);
```

**Purpose**: Constructs a JsonData object from a JSON string.

**Parameters**:
- `buffer` - A string containing JSON-formatted data

**Usage Example**:
```cpp
std::string json_str = R"({"config": "value"})";
JsonData json(json_str);
```

**Note**: The implementation (in data_json.cpp) uses move semantics for efficiency.

## Methods/Functions

### convert_to

```cpp
template <typename TYPE>
bool convert_to(TYPE& target) const;
```

**Purpose**: Converts the JSON data to another format (e.g., FHICL, XML).

**Template Parameter**:
- `TYPE` - The target type to convert to (FhiclData or XmlData)

**Parameters**:
- `target` - Reference to object that will receive the converted data

**Return Value**: `true` if conversion succeeded, `false` otherwise

**Usage Example**:
```cpp
JsonData json(R"({"parameter": "value"})");
FhiclData fhicl;
if (json.convert_to(fhicl)) {
    // Conversion successful
}
```

**Specializations**:
- `bool convert_to(FhiclData&)` - Implemented in data_fhicl.cpp
- `bool convert_to(XmlData&)` - Implemented in data_xml.cpp

---

### convert_from

```cpp
template <typename TYPE>
bool convert_from(TYPE const& source);
```

**Purpose**: Converts data from another format (FHICL, XML) into JSON.

**Template Parameter**:
- `TYPE` - The source type to convert from

**Parameters**:
- `source` - Const reference to the source data

**Return Value**: `true` if conversion succeeded, `false` otherwise

**Usage Example**:
```cpp
FhiclData fhicl("parameter: value");
JsonData json("");
if (json.convert_from(fhicl)) {
    // Conversion successful, json.json_buffer now contains JSON
}
```

**Specializations**:
- `bool convert_from(FhiclData const&)` - Implemented in data_fhicl.cpp
- `bool convert_from(XmlData const&)` - Implemented in data_xml.cpp

---

### Conversion Operators

#### operator std::string const&

```cpp
operator std::string const&() const;
```

**Purpose**: Implicitly converts JsonData to a const string reference.

**Return Value**: Const reference to the internal `json_buffer`

**Usage Example**:
```cpp
JsonData json(R"({"key": "value"})");
std::string str = json;  // Implicit conversion
std::cout << json;       // Works because of this operator
```

---

#### operator std::string&

```cpp
operator std::string&();
```

**Purpose**: Implicitly converts JsonData to a mutable string reference.

**Return Value**: Reference to the internal `json_buffer` (allows modification)

**Usage Example**:
```cpp
JsonData json("{}");
std::string& ref = json;
ref = R"({"new": "data"})";  // Modifies json.json_buffer
```

**Warning**: Use carefully - modifying through this reference bypasses any validation.

---

### empty

```cpp
bool empty() const;
```

**Purpose**: Checks if the JSON buffer is empty.

**Return Value**: `true` if `json_buffer` is empty, `false` otherwise

**Usage Example**:
```cpp
JsonData json("");
if (json.empty()) {
    std::cout << "No JSON data\n";
}
```

**Use Cases**:
- Validation before processing
- Checking if data was successfully loaded
- Guard conditions in conversion functions

---

### type_version (static)

```cpp
static constexpr auto type_version() { return "V1.0.0"; }
```

**Purpose**: Returns the version identifier for the JsonData type.

**Return Value**: String literal "V1.0.0"

**Usage**: Version tracking for serialization/deserialization, ensuring compatibility across different versions of the database schema.

## Stream Operators

### operator<<

```cpp
std::ostream& operator<<(std::ostream& os, artdaq::database::basictypes::JsonData const& data);
```

**Purpose**: Writes JsonData to an output stream.

**Parameters**:
- `os` - Output stream
- `data` - JsonData to write

**Return Value**: Reference to the stream (for chaining)

**Usage Example**:
```cpp
JsonData json(R"({"config": "value"})");
std::cout << json << "\n";  // Outputs the JSON string
```

---

### operator>>

```cpp
std::istream& operator>>(std::istream& is, artdaq::database::basictypes::JsonData& data);
```

**Purpose**: Reads JSON data from an input stream.

**Parameters**:
- `is` - Input stream
- `data` - JsonData to populate

**Return Value**: Reference to the stream (for chaining)

**Implementation**: Reads entire stream content into `json_buffer` using `istreambuf_iterator`

**Usage Example**:
```cpp
std::ifstream file("config.json");
JsonData json("");
file >> json;  // Reads entire file into json.json_buffer
```

## TRACE Integration

### TraceStreamer Specialization

```cpp
namespace {
template <>
inline TraceStreamer& TraceStreamer::operator<<(const artdaq::database::basictypes::JsonData& r) {
    std::ostringstream s;
    s << r;
    msg_append(s.str().c_str());
    return *this;
}
}
```

**Purpose**: Allows JsonData objects to be used directly in TRACE logging statements.

**Usage Example**:
```cpp
JsonData json(R"({"debug": "info"})");
TLOG(10) << "JSON data: " << json;  // Works because of this specialization
```

## Usage Context

### Role in the System

`JsonData` is the **pivot format** in the artdaq-database system:

```
FHICL ←→ JSON ←→ XML
         ↓
     Database
```

All conversions go through JSON as an intermediary format.

### Typical Usage Patterns

#### 1. Direct JSON Storage

```cpp
JsonData json(R"({"parameter": "value"})");
// Store json in database
```

#### 2. FHICL → JSON Conversion

```cpp
FhiclData fhicl("parameter: value");
JsonData json = fhicl;  // Uses FhiclData::operator JsonData()
```

#### 3. JSON → FHICL Conversion

```cpp
JsonData json(R"({"parameter": "value"})");
FhiclData fhicl(json);  // Uses FhiclData constructor
```

#### 4. Reading from File

```cpp
std::ifstream file("config.json");
JsonData json("");
file >> json;
```

### Files That Use JsonData

- `data_fhicl.cpp` - Implements FHICL ↔ JSON conversions
- `data_xml.cpp` - Implements XML ↔ JSON conversions
- Database interface modules (throughout the project)
- Configuration management systems

## Header Guards

```cpp
#ifndef _ARTDAQ_DATABASE_BASICTYPES_JSON_H_
#define _ARTDAQ_DATABASE_BASICTYPES_JSON_H_
```

## Notes for Developers

### Template Method Pattern

The `convert_to` and `convert_from` methods use template specialization:
- **Declaration**: In this header (generic template)
- **Specializations**: In data_fhicl.cpp and data_xml.cpp
- **Benefit**: Keeps conversion logic with the respective types

### Why `final`?

The class is marked `final` to:
- Prevent inheritance
- Enable compiler optimizations
- Clearly indicate design intent (this is a value type, not a base class)

### Design Philosophy

- **Simplicity**: Just a string wrapper with conversion capabilities
- **Flexibility**: Public member allows direct access when needed
- **Safety**: Conversion methods provide validation
- **Convenience**: Implicit conversions and stream operators

### Potential Issues

1. **No JSON Validation**: The class doesn't validate JSON syntax
2. **Direct Access**: Public `json_buffer` can be modified without validation
3. **Memory**: Large JSON documents are stored as strings (not parsed structures)

### Best Practices

1. **Validation**: Validate JSON syntax before creating JsonData objects
2. **Use Conversion Methods**: Prefer `convert_to/from` over direct buffer manipulation
3. **Check `empty()`**: Always check if data is empty before processing
4. **Error Handling**: Check return values of conversion methods

## Related Documentation

- `data_json.cpp.md` - Implementation details
- `data_json_fusion.h.md` - Boost.Fusion adaptation for serialization
- `data_fhicl.h.md` - FHICL data type and conversions
- `data_xml.h.md` - XML data type and conversions

## Version History

- **V1.0.0** - Current version (returned by `type_version()`)
