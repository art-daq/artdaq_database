# convertxml2json.cpp

## File Overview

Implementation of bidirectional XML ↔ JSON string conversion.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Xml/convertxml2json.cpp`

## Implementation

### xml_to_json

Converts XML string to JSON string via XmlReader and JsonWriter.

### json_to_xml

Converts JSON string to XML string via JsonReader and XmlWriter.

## TRACE Configuration

```cpp
#define TRACE_NAME "convertxml2json.cpp"
```

## Related Files

- **convertxml2json.h** - Function declarations
- **xml_reader/writer.h** - XML I/O
- **json_reader/writer.h** - JSON I/O
