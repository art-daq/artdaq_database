# xml_reader.cpp

## File Overview

Implementation of XML to JSON AST parsing using Boost.PropertyTree.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Xml/xml_reader.cpp`

## Dependencies

- Boost.PropertyTree for XML parsing
- JSON writer for intermediate conversion

## Implementation

Parses XML to Boost.PropertyTree, then converts to JSON AST.

**Algorithm**:
1. Parse XML string to boost::property_tree::ptree
2. Convert ptree to JSON string
3. Parse JSON string to json::object_t

## TRACE Configuration

```cpp
#define TRACE_NAME "xml_reader.cpp"
```

## Related Files

- **xml_reader.h** - XmlReader declaration
- **json_reader.h** - Used for JSON parsing step
