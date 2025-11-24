# data_xml.h

## File Overview

**Location**: `/home/user/artdaq-database/artdaq-database/BasicTypes/data_xml.h`

This header defines the `XmlData` class, which represents XML (Extensible Markup Language) formatted configuration data. XML is used as an alternative configuration format in the artdaq-database system, providing a structured, hierarchical representation of configuration data.

**Purpose**: Provides a wrapper class for XML configuration data with conversion capabilities to/from JSON format.

## Dependencies

- `artdaq-database/BasicTypes/common.h` - Common utilities and TRACE logging support

### TRACE Configuration

```cpp
#define TRACE_NAME "BTPS:XmlData_H"
```

**Note**: Uses abbreviated TRACE name "BTPS:XmlData_H" (BTPS = BasicTypes)

## Namespace Structure

```cpp
namespace artdaq {
namespace database {
namespace basictypes {
    // XmlData is defined here
}}}
```

## Key Types/Classes

### Forward Declarations

```cpp
struct JsonData;
```

**Purpose**: Forward declaration of `JsonData` to enable conversion operators without circular dependencies.

---

### XmlData

```cpp
struct XmlData final {
    XmlData(std::string);
    XmlData() = default;

    XmlData(JsonData const&);
    operator JsonData() const;
    operator std::string const&() const;

    static constexpr auto type_version() { return "V100"; }

    std::string xml_buffer;
};
```

**Purpose**: A wrapper class for XML configuration data that provides:
1. Storage for XML-formatted strings
2. Bidirectional conversion with JSON format
3. Stream I/O operators
4. Version tracking

**Design**: Marked `final` - cannot be inherited from

**Comparison with FhiclData**:
- XmlData has only `xml_buffer` (no filename metadata)
- Simpler structure than FhiclData
- Same conversion pattern as FhiclData

#### Member Variables

##### xml_buffer

```cpp
std::string xml_buffer;
```

**Purpose**: Holds the actual XML-formatted configuration string.

**Default Value**: Empty (implicitly initialized)

**Example Content**:
```xml
<?xml version="1.0"?>
<configuration>
    <parameter name="buffer_size">8192</parameter>
    <parameter name="timeout">1000</parameter>
    <settings>
        <debug>true</debug>
    </settings>
</configuration>
```

## Constructors

### XmlData(std::string)

```cpp
XmlData(std::string buffer);
```

**Purpose**: Constructs an XmlData object from an XML string.

**Parameters**:
- `buffer` - A string containing XML-formatted data

**Implementation**: Uses move semantics (see data_xml.cpp.md)

**Usage Example**:
```cpp
std::string xml_str = R"(
    <config>
        <param>value</param>
    </config>
)";
XmlData xml(xml_str);
```

---

### XmlData() (default)

```cpp
XmlData() = default;
```

**Purpose**: Default constructor creates an empty XmlData object.

**Result**: `xml_buffer` is empty

**Usage Example**:
```cpp
XmlData xml;  // Empty XML object
// Later populate via stream or assignment
```

---

### XmlData(JsonData const&)

```cpp
XmlData(JsonData const& document);
```

**Purpose**: Constructs an XmlData object by converting from JSON format.

**Parameters**:
- `document` - JsonData object containing XML data encoded in JSON

**Process**:
1. Validates JSON is not empty
2. Extracts Base64-encoded XML from JSON
3. Decodes Base64 to get intermediate JSON
4. Converts JSON to XML format
5. Stores result in `xml_buffer`

**Throws**: `std::runtime_error` if conversion fails

**Usage Example**:
```cpp
JsonData json(R"({"base64": "PGNvbmZpZz48L2NvbmZpZz4="})");
XmlData xml(json);  // Converts JSON → XML
```

**See**: data_xml.cpp.md for implementation details

## Methods/Functions

### Conversion to JsonData

```cpp
operator JsonData() const;
```

**Purpose**: Converts XML data to JSON format.

**Return Value**: JsonData object containing the XML data encoded in JSON with Base64

**Process**:
1. Converts XML to JSON structure
2. Encodes XML buffer in Base64
3. Creates JSON document with encoded data
4. Returns JsonData object

**Throws**: `std::runtime_error` if conversion fails

**Usage Example**:
```cpp
XmlData xml("<config><param>value</param></config>");
JsonData json = xml;  // Implicit conversion
// json now contains Base64-encoded XML in JSON format
```

---

### Conversion to String

```cpp
operator std::string const&() const;
```

**Purpose**: Provides access to the raw XML buffer string.

**Return Value**: Const reference to `xml_buffer`

**Usage Example**:
```cpp
XmlData xml("<config></config>");
std::string config = xml;  // Implicit conversion
std::cout << xml;  // Works via this operator
```

---

### type_version (static)

```cpp
static constexpr auto type_version() { return "V100"; }
```

**Purpose**: Returns the version identifier for the XmlData type.

**Return Value**: String literal "V100"

**Usage**: Version tracking for database schema compatibility.

**Note**: Used when creating collection names (e.g., "XmlData_V100")

## Stream Operators

### operator>>

```cpp
std::istream& operator>>(std::istream& is, artdaq::database::basictypes::XmlData& data);
```

**Purpose**: Reads XML data from an input stream (in JSON format).

**Parameters**:
- `is` - Input stream
- `data` - XmlData object to populate

**Return Value**: Reference to the stream (for chaining)

**Process**:
1. Reads entire stream into a string
2. Creates JsonData from the string
3. Converts JsonData to XmlData
4. Stores result in `data`

**Note**: Input is expected to be JSON-encoded XML, not raw XML

**Usage Example**:
```cpp
std::ifstream file("config.json");
XmlData xml;
file >> xml;  // Reads JSON, converts to XML
```

---

### operator<<

```cpp
std::ostream& operator<<(std::ostream& os, artdaq::database::basictypes::XmlData const& data);
```

**Purpose**: Writes raw XML data to an output stream.

**Parameters**:
- `os` - Output stream
- `data` - XmlData object to write

**Return Value**: Reference to the stream (for chaining)

**Output Format**: Raw XML text (not JSON-encoded)

**Usage Example**:
```cpp
XmlData xml("<config><param>value</param></config>");
std::cout << xml;  // Outputs: <config><param>value</param></config>

std::ofstream file("config.xml");
file << xml;  // Writes raw XML to file
```

## TRACE Integration

### TraceStreamer Specialization

```cpp
namespace {
template <>
inline TraceStreamer& TraceStreamer::operator<<(const artdaq::database::basictypes::XmlData& r) {
    std::ostringstream s;
    s << r;
    msg_append(s.str().c_str());
    return *this;
}
}
```

**Purpose**: Allows XmlData objects to be used in TRACE logging statements.

**Usage Example**:
```cpp
XmlData xml("<debug>info</debug>");
TLOG(5) << "Configuration: " << xml;  // Works via this specialization
```

## Usage Context

### Role in artdaq-database

XML is an **alternative configuration format** in artdaq-database:

```
XML Config File → XmlData → JsonData → Database
Database → JsonData → XmlData → XML Config File
```

Similar to FHICL, XML conversions use JSON as an intermediary format.

### Typical Usage Patterns

#### 1. Load XML Configuration

```cpp
// From string
XmlData config(R"(
    <daq>
        <buffer_size>8192</buffer_size>
        <timeout_ms>1000</timeout_ms>
    </daq>
)");

// From file (as JSON)
std::ifstream file("config.json");
XmlData config2;
file >> config2;
```

#### 2. Store in Database (via JSON)

```cpp
XmlData xml("<config><param>value</param></config>");
JsonData json = xml;  // Convert to JSON
// Store json in database
```

#### 3. Retrieve from Database

```cpp
// Retrieve json from database
JsonData json = get_from_database();
XmlData xml(json);  // Convert to XML
std::cout << xml;   // Use the configuration
```

#### 4. Convert Formats

```cpp
// XML ↔ JSON conversions
XmlData xml("<param>value</param>");
JsonData json = xml;         // XML → JSON
XmlData back = json;         // JSON → XML
```

## What is XML?

**XML (Extensible Markup Language)** is a markup language for encoding data in a format that is both human-readable and machine-readable.

### XML Syntax Examples

```xml
<!-- Simple element -->
<parameter>value</parameter>

<!-- Element with attributes -->
<module name="MyModule" enabled="true"/>

<!-- Nested structure -->
<configuration>
    <settings>
        <threshold>100</threshold>
        <enabled>true</enabled>
    </settings>
    <channels>
        <channel id="0"/>
        <channel id="1"/>
        <channel id="2"/>
    </channels>
</configuration>

<!-- With XML declaration -->
<?xml version="1.0" encoding="UTF-8"?>
<config>
    <data>content</data>
</config>
```

### Why XML?

- **Structured**: Hierarchical data representation
- **Standard**: Widely used, many parsing libraries available
- **Extensible**: Can define custom tags and attributes
- **Interoperable**: Works across different systems and platforms

## JSON Encoding Format

When XML is stored in JSON (for database), it uses Base64 encoding:

```json
{
    "base64": "PGNvbmZpZz48cGFyYW0+dmFsdWU8L3BhcmFtPjwvY29uZmlnPg=="
}
```

This ensures:
- XML special characters are preserved
- Nested structures are maintained
- JSON remains valid

## Header Guards

```cpp
#ifndef _ARTDAQ_DATABASE_BASICTYPES_XML_H_
#define _ARTDAQ_DATABASE_BASICTYPES_XML_H_
```

**Note**: Comment at end says `_ARTDAQ_DATABASE_BASICTYPES_FHICL_H_` - this appears to be a copy-paste error from data_fhicl.h.

## Notes for Developers

### Differences from FhiclData

1. **No File Name**: XmlData doesn't have a `file_name` member like FhiclData
2. **Simpler**: Only one data member (`xml_buffer`)
3. **Same Pattern**: Conversion logic follows the same pattern as FHICL

### Why No File Name?

The design choice to omit a filename member could be:
- XML files are often generated programmatically
- Less emphasis on tracking source files for XML
- Simpler structure for serialization

If you need to track the source file, you can wrap XmlData in another struct.

### Conversion Path

All XML↔JSON conversions go through:
1. `XmlData` ↔ intermediate JSON ↔ Base64 ↔ `JsonData`

### Error Handling

Conversions can throw `std::runtime_error`:
- Invalid JSON format
- Base64 decoding failures
- XML parsing errors

Always wrap conversions in try-catch when dealing with untrusted input.

### Thread Safety

The class is **not thread-safe** for concurrent writes.

## Best Practices

1. **Validate XML**: Check XML syntax before creating XmlData objects
2. **Handle Errors**: Wrap conversions in try-catch blocks
3. **Use Well-Formed XML**: Ensure XML has proper structure and closing tags
4. **Consider Alternatives**: FHICL might be more appropriate for artdaq configurations

## Example: Complete Workflow

```cpp
#include "artdaq-database/BasicTypes/data_xml.h"
#include "artdaq-database/BasicTypes/data_json.h"
#include <fstream>

try {
    // 1. Create XML configuration
    XmlData config(R"(
        <daq_settings>
            <buffer_size>8192</buffer_size>
            <timeout_ms>1000</timeout_ms>
        </daq_settings>
    )");

    // 2. Convert to JSON for database storage
    JsonData json = config;

    // 3. Store to database (pseudo-code)
    database.store("xml_configurations", json);

    // 4. Retrieve from database
    JsonData retrieved = database.get("xml_configurations");

    // 5. Convert back to XML
    XmlData restored(retrieved);

    // 6. Use the configuration
    std::cout << "Restored config:\n" << restored << "\n";

} catch (const std::runtime_error& e) {
    std::cerr << "Configuration error: " << e.what() << "\n";
}
```

## XML vs FHICL

When to use XML vs FHICL:

### Use XML When:
- Interoperating with external systems that use XML
- Need standard XML tooling (XPath, XSLT, etc.)
- Configuration comes from XML-based systems
- Strict schema validation is required (XML Schema)

### Use FHICL When:
- Working within artdaq ecosystem
- Need artdaq-specific features (references, includes)
- Human-readable configuration files are priority
- Simpler syntax is preferred

## Related Documentation

- `data_xml.cpp.md` - Implementation details and conversion logic
- `data_xml_fusion.h.md` - Boost.Fusion adaptation
- `data_json.h.md` - JSON data type for conversions
- `data_fhicl.h.md` - FHICL data type (similar pattern)

## Known Issues

### Header Guard Comment

The final endif comment references the wrong header:
```cpp
#endif /* _ARTDAQ_DATABASE_BASICTYPES_FHICL_H_ */
```

Should be:
```cpp
#endif /* _ARTDAQ_DATABASE_BASICTYPES_XML_H_ */
```

This is a cosmetic issue and doesn't affect functionality.
