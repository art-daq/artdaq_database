# xml_writer.cpp

## File Overview

Implementation of JSON AST to XML generation using Boost.PropertyTree.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Xml/xml_writer.cpp`

## Dependencies

- Boost.PropertyTree for XML generation
- JSON reader for intermediate conversion

## Implementation

Converts JSON AST to JSON string, parses to Boost.PropertyTree, writes as XML.

**Algorithm**:
1. Convert json::object_t to JSON string
2. Parse JSON string to boost::property_tree::ptree
3. Write ptree as XML

## TRACE Configuration

```cpp
#define TRACE_NAME "xml_writer.cpp"
```

## Related Files

- **xml_writer.h** - XmlWriter declaration
- **json_writer.h** - Used for JSON generation step
