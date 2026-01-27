# xml_writer.h

**Path:** `artdaq-database/DataFormats/Xml/xml_writer.h`

**Purpose:** Defines the `XmlWriter` struct that generates XML documents from the internal JSON AST (Abstract Syntax Tree) representation. This enables configurations stored in the database's JSON format to be exported as XML files for use by external systems or human review.

## Key Concepts

### JSON AST to XML Conversion

The `XmlWriter` transforms a JSON object tree (`jsn::object_t`) into an XML document string. This conversion follows these rules:

- **JSON object keys** become XML element names
- **JSON string/number/boolean values** become XML element text content
- **Nested JSON objects** become nested XML elements
- The input must contain a `"data"` key; its contents are serialized as XML

### Document Structure Requirement

The input JSON AST is expected to follow the database document format with a `"data"` key containing the actual configuration data. The writer extracts and serializes only the contents of this `"data"` section.

## Thread Safety

- **Thread-safe:** No (stateless struct, but not designed for concurrent use on same data)
- **Concurrent access:** Each `XmlWriter` instance should be used by a single thread
- **Locking:** None; callers must synchronize if sharing input/output objects

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/common.h` | Common definitions, macros, and TRACE logging |
| `artdaq-database/DataFormats/Json/json_types.h` | Defines `jsn::object_t` and related AST types |
| `artdaq-database/DataFormats/Xml/xml_types.h` | XML-specific type definitions (currently minimal) |

## Classes/Structures

### `XmlWriter`

A stateless struct that provides XML generation functionality. Marked `final` to prevent inheritance.

**Thread Safety:** Not thread-safe; use separate instances per thread.

#### Methods

##### `write(jsn::object_t const& json_input, std::string& xml_output) -> bool`

**Brief:** Converts a JSON AST representation to an XML document string, extracting the content from the `"data"` key of the input object.

**Parameters:**
- `json_input` - The JSON AST containing data to serialize. Must not be empty and must contain a `"data"` key.
- `xml_output` - Reference to receive the generated XML string. Must be empty before calling.

**Preconditions:**
- `json_input` must be a non-empty JSON object containing a `"data"` key
- `xml_output` must be an empty string

**Returns:** `true` if generation succeeds and produces non-empty output; `false` if the generated XML is empty.

**Postconditions:**
- On success, `xml_output` contains a valid XML document with proper declaration and formatting
- On failure, `xml_output` may be in an indeterminate state

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::exception` | When JSON structure is invalid or missing required keys |
| `boost::bad_get` | When type extraction fails (e.g., `"data"` is not an object) |

**Thread Safety:** Unsafe; do not call concurrently on shared input/output objects.

**Example:**

```cpp
#include "artdaq-database/DataFormats/Xml/xml_writer.h"
#include "artdaq-database/DataFormats/Json/json_types.h"
#include <iostream>

void generateXmlConfiguration() {
  using namespace artdaq::database;

  // Build JSON AST with required "data" structure
  json::object_t ast;
  ast["data"] = json::object_t{};
  auto& data = boost::get<json::object_t>(ast["data"]);

  // Add configuration content
  json::object_t config;
  config.push_back({"host", std::string("localhost")});
  config.push_back({"port", std::string("8080")});
  data.push_back({"config", config});

  std::string xml_result;

  try {
    xml::XmlWriter writer;
    bool success = writer.write(ast, xml_result);

    if (success) {
      std::cout << "Generated XML:\n" << xml_result << "\n";
      // Output:
      // <?xml version="1.0" encoding="utf-8"?>
      // <config>
      //     <host>localhost</host>
      //     <port>8080</port>
      // </config>
    } else {
      std::cerr << "XML generation produced empty result\n";
    }
  } catch (const std::exception& e) {
    std::cerr << "XML generation failed: " << e.what() << "\n";
  }
}
```

## Functions

### `debug::XmlWriter() -> void`

**Brief:** Enables TRACE debugging output for the XML writer module by configuring trace levels and modes.

**Parameters:** None

**Preconditions:** None

**Returns:** Nothing

**Postconditions:**
- TRACE logging is enabled for `"xml_writer.cpp"` at all debug levels

**Throws:** None

**Thread Safety:** Safe to call from any thread; affects global TRACE state.

**Side Effects:**
- Modifies global TRACE configuration
- Subsequent XML writer operations will produce debug output

**Example:**

```cpp
#include "artdaq-database/DataFormats/Xml/xml_writer.h"

void enableXmlWriterDebugging() {
  // Enable detailed trace output for XML generation
  artdaq::database::xml::debug::XmlWriter();

  // Now XML write operations will produce TRACE output
  xml::XmlWriter writer;
  // ... generation operations will be logged
}
```

## Relationship to Other Components

`XmlWriter` is part of the DataFormats/Xml module and serves as the export path for converting database documents to XML:

1. **Output pipeline:** Database storage -> `JsonReader` -> JSON AST -> `XmlWriter` -> XML string
2. **Paired with:** `XmlReader` for the reverse conversion (XML -> JSON AST)
3. **Used by:** `xmljsondb.cpp` functions (`json_to_xml`) for high-level format conversion
4. **Depends on:** Boost.PropertyTree for XML generation, JSON types for input

## See Also

- [xml_writer.cpp](./xml_writer.cpp.md) - Implementation details
- [xml_reader.h](./xml_reader.h.md) - Complementary XML parsing
- [xmljsondb.h](./xmljsondb.h.md) - High-level XML/JSON conversion with metadata
- [json_types.h](../Json/json_types.h.md) - JSON AST type definitions
- [External: Boost.PropertyTree](https://www.boost.org/doc/libs/release/libs/property_tree/) - Underlying XML generator

## Notes for Developers

### Common Pitfalls

- **Missing "data" key:** The input JSON AST must contain a `"data"` key. Attempting to write an AST without this structure will cause a `std::out_of_range` or `boost::bad_get` exception.

- **Empty output parameter:** The `xml_output` parameter must be empty before calling `write()`. The function uses `confirm()` assertions that will fail in debug builds if this precondition is violated.

- **Value type preservation:** All values are converted to strings in the XML output. Numeric and boolean type information is lost during conversion.

### Anti-patterns

```cpp
// DON'T do this - missing "data" key:
jsn::object_t ast;
ast["config"] = json::object_t{};  // Wrong: no "data" wrapper
writer.write(ast, xml);  // Will throw exception

// DO this instead - proper structure:
jsn::object_t ast;
ast["data"] = json::object_t{};
auto& data = boost::get<json::object_t>(ast["data"]);
data.push_back({"config", json::object_t{}});
writer.write(ast, xml);  // Correct

// DON'T do this - output must be empty:
std::string xml = "existing content";
writer.write(ast, xml);  // Will fail confirm() assertion

// DO this instead:
std::string xml;  // Empty
writer.write(ast, xml);  // Correct
```

### XML Formatting

The generated XML uses the following formatting settings:
- 4-space indentation
- UTF-8 encoding declaration
- XML version 1.0 declaration
