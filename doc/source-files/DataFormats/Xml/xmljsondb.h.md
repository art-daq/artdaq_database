# xmljsondb.h

## File Overview

Provides conversion functions between XML configuration format and database JSON format.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Xml/xmljsondb.h`

## Conversion Functions

```cpp
bool xml_to_jsondb(std::string const& xml, std::string& json_db);
bool jsondb_to_xml(std::string const& json_db, std::string& xml);
```

### xml_to_jsondb

Converts XML configuration to database JSON format (with metadata separation).

### jsondb_to_xml

Converts database JSON format to XML configuration.

## Purpose

Bridges between:
- XML configuration files (user-facing)
- Database JSON format (storage format with metadata)

## Related Files

- **xmljsondb.cpp** - Implementation
- **convertxml2json.h** - Basic XML/JSON conversion
