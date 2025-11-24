# xml_writer.h

## File Overview

Defines the XmlWriter class for generating XML documents from JSON AST representation.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Xml/xml_writer.h`

## Dependencies

### Project Headers
- `"artdaq-database/DataFormats/common.h"` - Common includes
- `"artdaq-database/DataFormats/Json/json_types.h"` - Source AST types
- `"artdaq-database/DataFormats/Xml/xml_types.h"` - XML type definitions

## XmlWriter Class

```cpp
struct XmlWriter final {
    bool write(jsn::object_t const&, std::string&);
};
```

**Method**: `write(ast, xml_output)`

**Parameters**:
- `ast` - Input JSON object AST (must not be empty)
- `xml_output` - Output XML string (must be empty)

**Returns**: `true` on success, `false` on failure

**Behavior**: Converts JSON object AST to XML format where JSON keys become XML elements and JSON values become element text content.

## Debug Utilities

```cpp
namespace debug {
    void XmlWriter();
}
```

## Usage Example

```cpp
using namespace artdaq::database;

json::object_t ast;
ast["config"] = json::object_t{};
// ... populate ast ...

std::string xml;
if (xml::XmlWriter{}.write(ast, xml)) {
    std::cout << xml;  // XML document
}
```

## Related Files

- **xml_writer.cpp** - Implementation
- **xml_reader.h** - Complementary reader
- **json_types.h** - AST type definitions
