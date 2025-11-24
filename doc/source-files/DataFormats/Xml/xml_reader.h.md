# xml_reader.h

## File Overview

Defines the XmlReader class for parsing XML documents into JSON AST representation.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Xml/xml_reader.h`

## Dependencies

### Project Headers
- `"artdaq-database/DataFormats/common.h"` - Common includes
- `"artdaq-database/DataFormats/Json/json_types.h"` - Target AST types
- `"artdaq-database/DataFormats/Xml/xml_types.h"` - XML type definitions

## XmlReader Class

```cpp
struct XmlReader final {
    bool read(std::string const&, jsn::object_t&);
};
```

**Method**: `read(xml_string, ast)`

**Parameters**:
- `xml_string` - XML document text
- `ast` - Output JSON object AST (must be empty)

**Returns**: `true` on success, `false` on parse failure

**Behavior**: Parses XML into JSON object representation where XML elements become JSON keys and XML text content becomes JSON values.

## Debug Utilities

```cpp
namespace debug {
    void XmlReader();
}
```

**Purpose**: Debug/test functionality for XmlReader.

## Usage Example

```cpp
using namespace artdaq::database;

std::string xml = "<config><host>localhost</host></config>";
json::object_t ast;

if (xml::XmlReader{}.read(xml, ast)) {
    // ast contains parsed structure
}
```

## Related Files

- **xml_reader.cpp** - Implementation
- **xml_writer.h** - Complementary writer
- **json_types.h** - AST type definitions
