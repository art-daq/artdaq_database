# xmljsondb.h

**Path:** `artdaq-database/DataFormats/Xml/xmljsondb.h`

**Purpose:** Provides high-level conversion functions between XML configuration format and the database's JSON storage format. These functions handle the complete transformation including metadata wrapping, making them suitable for import/export operations with the configuration database.

## Key Concepts

### Database JSON Format

The database stores configurations in a JSON format with metadata:

```json
{
  "document": {
    "data": { ... }  // Actual configuration content
  },
  "origin": {
    "format": "xml",
    "source": "xml_to_json",
    "timestamp": "2024-01-15T10:30:00Z"
  }
}
```

The functions in this header handle the conversion between raw XML and this enriched database format.

### Conversion Pipeline

**XML to JSON (import):**
```
XML string -> XmlReader -> JSON AST -> JsonWriter -> Database JSON string
                              |
                              +-> origin metadata added (format, source, timestamp)
```

**JSON to XML (export):**
```
Database JSON string -> JsonReader -> JSON AST -> XmlWriter -> XML string
                                         |
                                         +-> document.data extracted
```

## Thread Safety

- **Thread-safe:** No (functions use local state but share underlying readers/writers)
- **Concurrent access:** Safe to call from multiple threads with different input/output parameters
- **Locking:** None; thread safety depends on caller not sharing string references

## Dependencies

| Include | Purpose |
|---------|---------|
| `<string>` | Standard string type for input/output parameters |

## Functions

### `xmljson::xml_to_json(std::string const& xml, std::string& json) -> bool`

**Brief:** Converts an XML configuration document to the database JSON storage format, adding origin metadata to track the source format and conversion timestamp.

**Parameters:**
- `xml` - Input XML configuration string. Must not be empty.
- `json` - Output JSON string in database format. Must be empty before calling.

**Preconditions:**
- `xml` must contain valid, non-empty XML content
- `json` must be an empty string

**Returns:** `true` if conversion succeeds; `false` if XML parsing fails or produces empty content.

**Postconditions:**
- On success, `json` contains a complete database document with `document` and `origin` sections
- The `origin.format` field is set to `"xml"`
- The `origin.timestamp` field contains the conversion time

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::exception` | When XML parsing fails (malformed XML) |
| `boost::property_tree::xml_parser_error` | When XML syntax is invalid |

**Thread Safety:** Safe when called with non-shared parameters.

**Side Effects:**
- None beyond output parameter modification

**Example:**

```cpp
#include "artdaq-database/DataFormats/Xml/xmljsondb.h"
#include <iostream>

void importXmlConfiguration() {
  using namespace artdaq::database::xmljson;

  std::string xml_config = R"(<?xml version="1.0"?>
    <detector>
      <name>tracker</name>
      <enabled>true</enabled>
    </detector>)";

  std::string json_db;

  try {
    if (xml_to_json(xml_config, json_db)) {
      std::cout << "Conversion successful\n";
      std::cout << json_db << "\n";
      // Output includes document.data with parsed XML
      // and origin section with format="xml"
    } else {
      std::cerr << "Conversion failed: empty result\n";
    }
  } catch (const std::exception& e) {
    std::cerr << "XML parsing error: " << e.what() << "\n";
  }
}
```

### `xmljson::json_to_xml(std::string const& json, std::string& xml) -> bool`

**Brief:** Converts a database JSON document back to XML format, extracting the configuration data from the document structure and generating properly formatted XML.

**Parameters:**
- `json` - Input JSON string in database format. Must not be empty and must contain a `document` section.
- `xml` - Output XML string. Must be empty before calling.

**Preconditions:**
- `json` must be a valid database JSON document with `document.data` structure
- `xml` must be an empty string

**Returns:** `true` if conversion succeeds; `false` if JSON parsing fails or produces empty content.

**Postconditions:**
- On success, `xml` contains a valid XML document with proper declaration
- Metadata (origin, bookkeeping, etc.) is not included in the XML output

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::exception` | When JSON parsing fails or structure is invalid |
| `boost::bad_get` | When expected JSON structure is missing or has wrong type |

**Thread Safety:** Safe when called with non-shared parameters.

**Side Effects:**
- None beyond output parameter modification

**Example:**

```cpp
#include "artdaq-database/DataFormats/Xml/xmljsondb.h"
#include <iostream>

void exportXmlConfiguration() {
  using namespace artdaq::database::xmljson;

  // Database JSON format with document wrapper
  std::string json_db = R"({
    "document": {
      "data": {
        "detector": {
          "name": "tracker",
          "enabled": "true"
        }
      }
    },
    "origin": {
      "format": "xml"
    }
  })";

  std::string xml_output;

  try {
    if (json_to_xml(json_db, xml_output)) {
      std::cout << "Export successful:\n" << xml_output << "\n";
      // Output:
      // <?xml version="1.0" encoding="utf-8"?>
      // <detector>
      //     <name>tracker</name>
      //     <enabled>true</enabled>
      // </detector>
    } else {
      std::cerr << "Export failed: empty result\n";
    }
  } catch (const std::exception& e) {
    std::cerr << "JSON parsing error: " << e.what() << "\n";
  }
}
```

### `xmljson::debug::XmlJson() -> void`

**Brief:** Enables TRACE debugging output for the XML/JSON conversion module by configuring trace levels and modes.

**Parameters:** None

**Preconditions:** None

**Returns:** Nothing

**Postconditions:**
- TRACE logging is enabled for `"xmljsondb.cpp"` at all debug levels

**Throws:** None

**Thread Safety:** Safe to call from any thread; affects global TRACE state.

**Side Effects:**
- Modifies global TRACE configuration

### `xml::debug::XmlReader() -> void`

**Brief:** Enables TRACE debugging for the XML reader component.

**Note:** This is a forward declaration; implementation is in `xml_reader.cpp`.

### `xml::debug::XmlWriter() -> void`

**Brief:** Enables TRACE debugging for the XML writer component.

**Note:** This is a forward declaration; implementation is in `xml_writer.cpp`.

## Relationship to Other Components

This header provides the primary interface for XML format support in the database:

1. **Import path:** User XML files -> `xml_to_json()` -> Database storage
2. **Export path:** Database storage -> `json_to_xml()` -> User XML files
3. **Uses internally:** `XmlReader`, `XmlWriter`, `JsonReader`, `JsonWriter`
4. **Called by:** Configuration import/export utilities, `conftool`

## See Also

- [xmljsondb.cpp](./xmljsondb.cpp.md) - Implementation details
- [xml_reader.h](./xml_reader.h.md) - Low-level XML parsing
- [xml_writer.h](./xml_writer.h.md) - Low-level XML generation
- [convertxml2json.h](./convertxml2json.h.md) - Alternative conversion types (currently minimal)
- [../Json/json_common.h](../Json/json_common.h.md) - JSON reader/writer includes

## Notes for Developers

### Common Pitfalls

- **JSON structure requirements:** The `json_to_xml()` function expects the database document format with `document.data`. Raw JSON without this wrapper will fail.

- **Metadata preservation:** When converting JSON to XML, metadata (origin, bookkeeping) is discarded. To preserve provenance, store the original JSON alongside exports.

- **Empty parameters:** Both input and output parameters have strict requirements (input non-empty, output empty). Violations trigger assertion failures in debug builds.

### Round-Trip Considerations

Converting XML -> JSON -> XML may not produce identical output due to:
- Whitespace normalization
- Attribute handling differences
- Comment stripping
- Declaration differences

For exact preservation, store the original format alongside the database document.

### Anti-patterns

```cpp
// DON'T do this - output must be empty:
std::string json = "existing";
xml_to_json(xml, json);  // Assertion failure

// DO this instead:
std::string json;  // Empty
xml_to_json(xml, json);  // Correct

// DON'T do this - wrong JSON structure for export:
std::string raw_json = R"({"key": "value"})";  // No document wrapper
json_to_xml(raw_json, xml);  // Will throw or fail

// DO this instead - proper database format:
std::string db_json = R"({"document": {"data": {"key": "value"}}})";
json_to_xml(db_json, xml);  // Correct
```
