# Xml Submodule Documentation

## Overview

The Xml submodule provides XML parsing and generation using Boost.PropertyTree, with conversion to/from JSON AST for unified processing.

## Files

### Core Types
- **[xml_types.h](xml_types.h.md)** - Minimal type definitions (reuses JSON AST)

### Parsing and Generation
- **[xml_reader.h](xml_reader.h.md)** - XmlReader class declaration
- **[xml_reader.cpp](xml_reader.cpp.md)** - XML to JSON AST parser
- **[xml_writer.h](xml_writer.h.md)** - XmlWriter class declaration
- **[xml_writer.cpp](xml_writer.cpp.md)** - JSON AST to XML generator

### Conversion Utilities
- **[convertxml2json.h](convertxml2json.h.md)** - XML ↔ JSON string conversion functions
- **[convertxml2json.cpp](convertxml2json.cpp.md)** - Conversion implementation
- **[xmljsondb.h](xmljsondb.h.md)** - XML ↔ database JSON conversion
- **[xmljsondb.cpp](xmljsondb.cpp.md)** - Database conversion implementation

### Convenience
- **[xml_common.h](xml_common.h.md)** - Aggregator header for all XML functionality

## Design

XML module reuses JSON AST types (`json::object_t`) as intermediate representation, enabling easy conversion between XML and JSON formats.

## Usage Example

```cpp
std::string xml = "<config><host>localhost</host></config>";
std::string json;
xml_to_json(xml, json);  // Converts to JSON format
```

## Related Files

- [../Json/json_types.h.md](../Json/json_types.h.md) - AST types used by XML module
