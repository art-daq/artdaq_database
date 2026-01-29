# xml_types.h

**Path:** `artdaq-database/DataFormats/Xml/xml_types.h`

**Purpose:** Provides the type infrastructure for XML data formats. This is a minimal header that primarily includes common infrastructure, as the XML module intentionally reuses the JSON AST types rather than defining its own type system.


## Key Concepts

### Unified Type System

Unlike many serialization libraries that define separate type systems for each format, artdaq-database uses a unified approach:

- **JSON types are the internal representation:** All data, regardless of source format (JSON, XML, FHiCL), is converted to the JSON AST types.
- **XML has no distinct types:** The XML module parses to and generates from `json::object_t`, `json::array_t`, and `json::value_t`.
- **Format is a serialization concern:** The internal representation is format-agnostic.

### Benefits of This Design

| Benefit | Description |
|---------|-------------|
| Simplicity | One set of types to understand and manipulate |
| Interoperability | Easy conversion between XML and JSON |
| Code Reuse | JSON manipulation utilities work on XML data |
| Consistency | Same API regardless of source format |

## Thread Safety

- **Thread-safe:** N/A (header contains no runtime code)
- **Concurrent access:** N/A
- **Locking:** N/A

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/common.h` | Common includes and configuration |

## Classes/Structures

This header does not define any classes or structures. The XML module uses types from `json_types.h`:

| JSON Type | XML Usage |
|-----------|-----------|
| `json::object_t` | Represents XML elements with child elements |
| `json::array_t` | Represents repeated XML elements |
| `json::value_t` | Represents XML text content and attribute values |
| `json::data_t` | Represents XML element name and content pairs |
| `json::key_t` | Represents XML element or attribute names |

## Functions

This header does not define any functions.

## Relationship to Other Components

```
xml_types.h (this file)
    |
    +-- common.h --> Shared infrastructure
    |
    +-- Used by: xml_reader.h, xml_writer.h
    |
    +-- Actual types from: json_types.h
```

The XML type system is essentially a passthrough to the JSON type system, establishing that XML data will use JSON's internal representation.

## Example Usage

Since the XML module uses JSON types, XML data manipulation looks identical to JSON manipulation:

```cpp
#include "artdaq-database/DataFormats/Xml/xml_common.h"
#include "artdaq-database/DataFormats/Json/json_types.h"
#include <iostream>

using namespace artdaq::database::xml;
using namespace artdaq::database::json;

void manipulateXmlAsJson() {
  // Parse XML
  std::string xml = R"(<config><threshold>100</threshold></config>)";
  object_t ast;  // This is json::object_t, used for XML too
  XmlReader{}.read(xml, ast);

  // Manipulate using JSON AST operations
  auto& config = boost::get<object_t>(ast.at("config"));
  config["new_field"] = std::string("added_value");

  // Write back to XML
  std::string output;
  XmlWriter{}.write(ast, output);
  std::cout << output << "\n";
}
```

## See Also

- [xml_reader.h](./xml_reader.h.md) - Parses XML to JSON types
- [xml_writer.h](./xml_writer.h.md) - Generates XML from JSON types
- [xml_common.h](./xml_common.h.md) - Aggregator header for XML module
- [../Json/json_types.h](../Json/json_types.h.md) - Actual type definitions used
- [convertxml2json.h](./convertxml2json.h.md) - XML/JSON conversion utilities

## Notes for Developers

### Common Pitfalls

- **Pitfall 1:** Looking for `xml::object_t` or similar types. These do not exist; use `json::object_t` for all XML data.

- **Pitfall 2:** Expecting XML-specific type semantics. XML attributes, namespaces, and other XML-specific constructs are mapped to the JSON model during parsing.

### Design Rationale

The minimal nature of this header is intentional:

1. **No XML-specific types:** By design, XML uses JSON's type system
2. **Common.h dependency:** Brings in shared infrastructure for all DataFormat modules
3. **Placeholder for future:** Could be extended if XML-specific types are ever needed

### XML to JSON Mapping

When XML is parsed, it is mapped to JSON types according to these conventions:

| XML Construct | JSON Representation |
|---------------|---------------------|
| Element | `object_t` with element name as key |
| Attributes | Key-value pairs in the element's object |
| Text content | String value |
| Repeated elements | `array_t` of element objects |
| Mixed content | Combination of above |

Example:
```xml
<root attr="value">
  <child>text</child>
  <items><item>1</item><item>2</item></items>
</root>
```

Maps to JSON structure:
```json
{
  "root": {
    "@attr": "value",
    "child": "text",
    "items": {
      "item": ["1", "2"]
    }
  }
}
```

Note: The exact mapping conventions depend on the `XmlReader` implementation. Consult `xml_reader.h` documentation for details.
