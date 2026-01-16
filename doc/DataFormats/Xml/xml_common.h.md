# xml_common.h

**Path:** `artdaq-database/DataFormats/Xml/xml_common.h`

**Purpose:** Convenience aggregator header that provides all XML DataFormat functionality through a single include. This header collects the XML reader, writer, and common infrastructure, allowing users to access complete XML support with one include statement.


## Key Concepts

### Aggregator Header Pattern

This file follows the aggregator header pattern, commonly used in C++ libraries to:
- Provide a convenient single entry point for module functionality
- Reduce the number of includes needed in client code
- Ensure consistent include ordering to avoid dependency issues

### XML and JSON Interoperability

The XML module in artdaq-database does not define its own AST types. Instead, it:
- Parses XML documents into `json::object_t` (the JSON AST)
- Writes `json::object_t` to XML format
- Enables seamless conversion between XML and JSON representations

This design choice allows configurations to be stored in either format while using a single internal representation.

## Thread Safety

- **Thread-safe:** Depends on included components
- **Concurrent access:** See individual component documentation
- **Locking:** No locking introduced by this header

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/Xml/xml_reader.h` | XML parsing to JSON AST |
| `artdaq-database/DataFormats/Xml/xml_writer.h` | JSON AST to XML generation |
| `artdaq-database/DataFormats/common.h` | Common DataFormats infrastructure (TRACE, confirm, etc.) |

## Provided Functionality

By including `xml_common.h`, you gain access to:

### From xml_reader.h
- `xml::XmlReader` - Parses XML strings to `json::object_t`
- XML parsing with attribute and element support

### From xml_writer.h
- `xml::XmlWriter` - Generates XML from `json::object_t`
- Formatted XML output with proper escaping

### From common.h
- TRACE logging macros
- `confirm()` assertion macro
- Boost exception support
- SharedCommon utilities

## Functions

This is an aggregator header and does not define any functions directly. See the included headers for function documentation.

## Relationship to Other Components

```
xml_common.h (this file)
    |
    +-- xml_reader.h --> Parses XML to json::object_t
    +-- xml_writer.h --> Generates XML from json::object_t
    +-- common.h --> Common infrastructure
    |
    +-- Uses: json::object_t, json::array_t, json::value_t
```

The XML module serves as a format bridge, allowing configurations authored in XML to be stored and manipulated using the JSON-based internal representation.

## Example Usage

```cpp
#include "artdaq-database/DataFormats/Xml/xml_common.h"
#include <iostream>

using namespace artdaq::database::xml;
using namespace artdaq::database::json;

void convertXmlToJson() {
  std::string xml = R"(
    <configuration>
      <detector name="TPC">
        <channels>1024</channels>
        <enabled>true</enabled>
      </detector>
    </configuration>
  )";

  object_t ast;
  XmlReader reader;

  if (reader.read(xml, ast)) {
    // ast now contains JSON representation of the XML
    std::cout << "Parsed XML successfully\n";

    // Can be written back to XML
    std::string xml_output;
    XmlWriter writer;
    if (writer.write(ast, xml_output)) {
      std::cout << "Generated XML:\n" << xml_output << "\n";
    }
  } else {
    std::cerr << "Failed to parse XML\n";
  }
}
```

## See Also

- [xml_reader.h](./xml_reader.h.md) - XML parsing functionality
- [xml_writer.h](./xml_writer.h.md) - XML generation functionality
- [xml_types.h](./xml_types.h.md) - XML type definitions (minimal, uses JSON types)
- [convertxml2json.h](./convertxml2json.h.md) - XML to JSON conversion utilities
- [../Json/json_types.h](../Json/json_types.h.md) - AST types used by XML module

## Notes for Developers

### Common Pitfalls

- **Pitfall 1:** Assuming XML-specific types exist. The XML module reuses JSON types; there is no `xml::object_t` or similar.

- **Pitfall 2:** Including individual headers inconsistently. Use `xml_common.h` for complete functionality or ensure proper include ordering when using individual headers.

### Best Practices

```cpp
// PREFERRED - Single include for full XML support:
#include "artdaq-database/DataFormats/Xml/xml_common.h"

// ALSO VALID - Include only what you need:
#include "artdaq-database/DataFormats/Xml/xml_reader.h"  // Just parsing
// or
#include "artdaq-database/DataFormats/Xml/xml_writer.h"  // Just generation
```

### Design Rationale

The decision to use JSON AST types for XML data has several benefits:
1. **Single representation:** One internal format simplifies code
2. **Format agility:** Configurations can be stored as JSON or XML
3. **Easier conversion:** XML-to-JSON conversion is implicit
4. **Code reuse:** JSON manipulation utilities work on XML-sourced data
