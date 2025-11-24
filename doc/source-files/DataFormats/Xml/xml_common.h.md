# xml_common.h

## File Overview

Convenience aggregator header providing all XML DataFormat functionality in one include.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Xml/xml_common.h`

## Dependencies

### Project Headers
- `"artdaq-database/DataFormats/Xml/xml_reader.h"` - XML parser
- `"artdaq-database/DataFormats/Xml/xml_writer.h"` - XML generator
- `"artdaq-database/DataFormats/Xml/xml_types.h"` - Type definitions (minimal)
- `"artdaq-database/DataFormats/common.h"` - Common infrastructure

## Purpose

Single include point for:
- XML parsing (to JSON AST)
- XML generation (from JSON AST)
- Common DataFormats infrastructure

## Usage

```cpp
#include "artdaq-database/DataFormats/Xml/xml_common.h"

std::string xml = "<root><item>value</item></root>";
json::object_t ast;

xml::XmlReader{}.read(xml, ast);
```

## Related Files

- **xml_reader.h** - Parser
- **xml_writer.h** - Generator
- **json_types.h** - AST types
