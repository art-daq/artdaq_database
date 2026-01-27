# xml_reader.h

**Path:** `artdaq-database/DataFormats/Xml/xml_reader.h`

**Purpose:** Defines the `XmlReader` struct that parses XML documents and converts them into the internal JSON AST (Abstract Syntax Tree) representation used throughout artdaq-database. This enables XML configuration files to be stored and manipulated using the database's JSON-based document model.

## Key Concepts

### XML to JSON AST Conversion

The `XmlReader` transforms XML documents into a JSON object tree (`jsn::object_t`). This conversion follows these rules:

- **XML elements** become JSON object keys
- **XML text content** becomes JSON string values
- **Nested XML elements** become nested JSON objects
- The parsed content is wrapped in a `"data"` key to conform to the database document structure

### JSON AST Types

The output `jsn::object_t` is a recursive structure defined in `json_types.h`:
- `object_t` - A table of key-value pairs (like a JSON object)
- `value_t` - A variant that can hold strings, numbers, booleans, nested objects, or arrays

## Thread Safety

- **Thread-safe:** No (stateless struct, but not designed for concurrent use on same data)
- **Concurrent access:** Each `XmlReader` instance should be used by a single thread
- **Locking:** None; callers must synchronize if sharing parsed output

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/common.h` | Common definitions, macros, and TRACE logging |
| `artdaq-database/DataFormats/Json/json_types.h` | Defines `jsn::object_t` and related AST types |
| `artdaq-database/DataFormats/Xml/xml_types.h` | XML-specific type definitions (currently minimal) |

## Classes/Structures

### `XmlReader`

A stateless struct that provides XML parsing functionality. Marked `final` to prevent inheritance.

**Thread Safety:** Not thread-safe; use separate instances per thread.

#### Methods

##### `read(std::string const& xml_input, jsn::object_t& json_output) -> bool`

**Brief:** Parses an XML document string and converts it into a JSON AST representation, storing the result in the provided output object.

**Parameters:**
- `xml_input` - The XML document as a string. Must not be empty.
- `json_output` - Reference to receive the parsed JSON AST. Must be empty before calling.

**Preconditions:**
- `xml_input` must contain valid, non-empty XML content
- `json_output` must be an empty `jsn::object_t`

**Returns:** `true` if parsing succeeds and produces non-empty output; `false` if the parsed tree is empty.

**Postconditions:**
- On success, `json_output` contains the parsed XML as a JSON object with a `"data"` key
- On failure, `json_output` may be in an indeterminate state

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::exception` | When XML parsing fails (malformed XML) |
| `boost::property_tree::xml_parser_error` | When the XML parser encounters invalid syntax |

**Thread Safety:** Unsafe; do not call concurrently on shared output objects.

**Example:**

```cpp
#include "artdaq-database/DataFormats/Xml/xml_reader.h"
#include <iostream>

void parseXmlConfiguration() {
  using namespace artdaq::database;

  std::string xml_config = R"(<?xml version="1.0"?>
    <config>
      <host>localhost</host>
      <port>8080</port>
    </config>)";

  json::object_t parsed_ast;

  try {
    xml::XmlReader reader;
    bool success = reader.read(xml_config, parsed_ast);

    if (success) {
      std::cout << "XML parsed successfully\n";
      // parsed_ast now contains: {"data": {"config": {"host": "localhost", "port": "8080"}}}
    } else {
      std::cerr << "Parsing produced empty result\n";
    }
  } catch (const std::exception& e) {
    std::cerr << "XML parsing failed: " << e.what() << "\n";
  }
}
```

## Functions

### `debug::XmlReader() -> void`

**Brief:** Enables TRACE debugging output for the XML reader module by configuring trace levels and modes.

**Parameters:** None

**Preconditions:** None

**Returns:** Nothing

**Postconditions:**
- TRACE logging is enabled for `"xml_reader.cpp"` at all debug levels

**Throws:** None

**Thread Safety:** Safe to call from any thread; affects global TRACE state.

**Side Effects:**
- Modifies global TRACE configuration
- Subsequent XML reader operations will produce debug output

**Example:**

```cpp
#include "artdaq-database/DataFormats/Xml/xml_reader.h"

void enableXmlDebugging() {
  // Enable detailed trace output for XML parsing
  artdaq::database::xml::debug::XmlReader();

  // Now XML operations will produce TRACE output
  xml::XmlReader reader;
  // ... parsing operations will be logged
}
```

## Relationship to Other Components

`XmlReader` is part of the DataFormats/Xml module and serves as the entry point for importing XML data into the database:

1. **Input pipeline:** XML string -> `XmlReader` -> JSON AST -> `JsonWriter` -> database storage
2. **Paired with:** `XmlWriter` for the reverse conversion (JSON AST -> XML)
3. **Used by:** `xmljsondb.cpp` functions (`xml_to_json`) for high-level format conversion
4. **Depends on:** Boost.PropertyTree for XML parsing, JSON types for output

## See Also

- [xml_reader.cpp](./xml_reader.cpp.md) - Implementation details
- [xml_writer.h](./xml_writer.h.md) - Complementary XML generation
- [xmljsondb.h](./xmljsondb.h.md) - High-level XML/JSON conversion with metadata
- [json_types.h](../Json/json_types.h.md) - JSON AST type definitions
- [External: Boost.PropertyTree](https://www.boost.org/doc/libs/release/libs/property_tree/) - Underlying XML parser

## Notes for Developers

### Common Pitfalls

- **Empty output parameter:** The `json_output` parameter must be empty before calling `read()`. The function uses `confirm()` assertions that will fail in debug builds if this precondition is violated.

- **XML attributes:** XML attributes are handled by Boost.PropertyTree using the `<xmlattr>` convention. This may produce unexpected JSON structures for attribute-heavy XML.

- **Character encoding:** Input XML must be valid UTF-8. Other encodings may cause parsing failures or data corruption.

### Anti-patterns

```cpp
// DON'T do this - output must be empty:
jsn::object_t ast;
ast["existing"] = "data";
reader.read(xml, ast);  // Will fail confirm() assertion

// DO this instead:
jsn::object_t ast;  // Empty
reader.read(xml, ast);  // Correct
```
