# xml_types.h

## File Overview

This minimal header provides the type infrastructure for XML data formats. Unlike JSON and FHiCL which have their own type systems, XML parsing/writing uses the JSON AST types directly.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Xml/xml_types.h`

## Dependencies

### Project Headers
- `"artdaq-database/DataFormats/common.h"` - Common includes and configuration

## Design

The XML module does not define its own type system. Instead, it:
- Parses XML to `json::object_t` AST
- Writes `json::object_t` AST to XML format
- Reuses all JSON type infrastructure

This design allows XML documents to be converted to/from JSON easily and share the same internal representation.

## Related Files

- **xml_reader.h** - Parses XML to json::object_t
- **xml_writer.h** - Writes json::object_t to XML
- **json_types.h** - Actual type definitions used
- **convertxml2json.h** - XML to JSON conversion utilities
