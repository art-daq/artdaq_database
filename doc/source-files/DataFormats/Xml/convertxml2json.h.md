# convertxml2json.h

## File Overview

Provides high-level string-to-string conversion functions between XML and JSON formats.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Xml/convertxml2json.h`

## Dependencies

### Project Headers
- `"artdaq-database/DataFormats/common.h"` - Common includes

## Conversion Functions

```cpp
namespace artdaq {
namespace database {

bool xml_to_json(std::string const& xml, std::string& json);
bool json_to_xml(std::string const& json, std::string& xml);

}  // namespace database
}  // namespace artdaq
```

### xml_to_json

**Purpose**: Converts XML document to JSON format.

**Parameters**:
- `xml` - Input XML string
- `json` - Output JSON string (must be empty)

**Returns**: `true` on success

### json_to_xml

**Purpose**: Converts JSON document to XML format.

**Parameters**:
- `json` - Input JSON string
- `xml` - Output XML string (must be empty)

**Returns**: `true` on success

## Usage Example

```cpp
std::string xml = "<config><host>localhost</host></config>";
std::string json;

if (xml_to_json(xml, json)) {
    std::cout << json;  // {"config":{"host":"localhost"}}
}

std::string xml_back;
if (json_to_xml(json, xml_back)) {
    // xml_back contains XML representation
}
```

## Related Files

- **convertxml2json.cpp** - Implementation
- **xml_reader.h** - XML parser
- **xml_writer.h** - XML generator
- **json_reader/writer.h** - JSON I/O
