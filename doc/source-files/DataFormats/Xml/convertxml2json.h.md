# convertxml2json.h

**Path:** `artdaq-database/DataFormats/Xml/convertxml2json.h`

**Purpose:** Defines tag types for XML/JSON conversion operations and provides namespace structure for the conversion functionality. This header establishes the `xmljson` namespace and declares empty struct tags (`xml2json`, `json2xml`) that can be used for template specialization or type-based dispatch in conversion pipelines.

**Note:** This file currently provides minimal functionality. The actual conversion logic is implemented in `xmljsondb.h/.cpp`. The tag types are placeholders for potential future template-based conversion APIs.

## Key Concepts

### Tag Types for Type Dispatch

The empty structs `xml2json` and `json2xml` serve as tag types - they carry no data but can be used to:

- Select template specializations at compile time
- Provide type-safe function overloading
- Document conversion direction in type signatures

This pattern is common in C++ for creating type-safe APIs without runtime overhead.

### Current vs. Planned Usage

**Current state:** These types are defined but not actively used in the codebase. The conversion functions in `xmljsondb.h` use explicit function names (`xml_to_json`, `json_to_xml`) rather than template dispatch.

**Potential future use:** A unified conversion API might use these tags:

```cpp
template<typename Direction>
bool convert(std::string const& input, std::string& output);

// Specializations would be selected by tag type:
convert<xml2json>(xml_str, json_str);
convert<json2xml>(json_str, xml_str);
```

## Thread Safety

- **Thread-safe:** Yes (contains only type definitions, no state)
- **Concurrent access:** Safe; types have no mutable state
- **Locking:** Not applicable

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/common.h` | Common definitions and macros |
| `artdaq-database/DataFormats/Json/json_types.h` | JSON AST types (for `jsn` namespace alias) |
| `artdaq-database/DataFormats/Xml/xml_types.h` | XML type definitions |

## Classes/Structures

### `xml2json`

**Brief:** Empty tag type representing the XML-to-JSON conversion direction.

```cpp
struct xml2json final {};
```

**Thread Safety:** Inherently thread-safe (stateless).

**Usage:** Type tag for template dispatch or compile-time conversion direction selection.

### `json2xml`

**Brief:** Empty tag type representing the JSON-to-XML conversion direction.

```cpp
struct json2xml final {};
```

**Thread Safety:** Inherently thread-safe (stateless).

**Usage:** Type tag for template dispatch or compile-time conversion direction selection.

## Functions

### `debug::Xml2Json() -> void`

**Brief:** Enables TRACE debugging output for the XML/JSON conversion module by configuring trace levels and modes.

**Parameters:** None

**Preconditions:** None

**Returns:** Nothing

**Postconditions:**
- TRACE logging is enabled for `"convertxml2json.cpp"` at all debug levels

**Throws:** None

**Thread Safety:** Safe to call from any thread; affects global TRACE state.

**Side Effects:**
- Modifies global TRACE configuration

**Example:**

```cpp
#include "artdaq-database/DataFormats/Xml/convertxml2json.h"

void enableConversionDebugging() {
  // Enable trace output for conversion operations
  artdaq::database::xmljson::debug::Xml2Json();

  // Subsequent conversion operations will produce TRACE output
}
```

## Namespace Alias

The header establishes a namespace alias for convenience:

```cpp
namespace jsn = artdaq::database::json;
```

This alias is used within the `xmljson` namespace to reference JSON types without full qualification.

## Relationship to Other Components

This header is part of the XML conversion infrastructure but plays a minimal role:

1. **Type definitions:** Provides tag types that could be used for template dispatch
2. **Namespace organization:** Establishes the `xmljson` namespace
3. **Debug support:** Declares debug function for trace enabling

**Actual conversion logic is in:**
- `xmljsondb.h` - High-level `xml_to_json()` and `json_to_xml()` functions
- `xml_reader.h` / `xml_writer.h` - Low-level AST conversion

## See Also

- [convertxml2json.cpp](./convertxml2json.cpp.md) - Implementation (debug function only)
- [xmljsondb.h](./xmljsondb.h.md) - Actual conversion function declarations
- [xml_reader.h](./xml_reader.h.md) - XML to JSON AST conversion
- [xml_writer.h](./xml_writer.h.md) - JSON AST to XML conversion

## Notes for Developers

### Design Intent

The tag types appear to be placeholders for a more generic conversion framework that was either:
- Planned but not implemented
- Partially implemented and later simplified
- Reserved for future template-based APIs

### Current Best Practice

For XML/JSON conversion, use the explicit functions in `xmljsondb.h`:

```cpp
// Recommended approach - use explicit functions
#include "artdaq-database/DataFormats/Xml/xmljsondb.h"

artdaq::database::xmljson::xml_to_json(xml_str, json_str);
artdaq::database::xmljson::json_to_xml(json_str, xml_str);
```

### Potential Future API

If the tag types are utilized in the future:

```cpp
// Hypothetical template-based API
template<typename Conversion>
struct Converter;

template<>
struct Converter<xml2json> {
  static bool convert(std::string const& in, std::string& out) {
    return xml_to_json(in, out);
  }
};

template<>
struct Converter<json2xml> {
  static bool convert(std::string const& in, std::string& out) {
    return json_to_xml(in, out);
  }
};

// Usage:
Converter<xml2json>::convert(xml, json);
```

### Header Guard Note

The header guard `_ARTDAQ_DATABASE_DATAFORMATS_XML_CONVERTXML2JSON_H_` follows the project's naming convention using the full path to ensure uniqueness.
