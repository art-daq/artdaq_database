# data_xml.h

**Path:** `artdaq-database/BasicTypes/data_xml.h`

**Purpose:** Defines the XmlData structure, which wraps XML (Extensible Markup Language) formatted configuration data. XML provides an alternative configuration format in artdaq-database, enabling interoperability with external systems and standard XML tooling.


## Key Concepts

### XML Configuration Format

XML (Extensible Markup Language) is a widely-used markup language for structured data:

```xml
<?xml version="1.0"?>
<configuration>
    <module name="detector">
        <parameter name="buffer_size">8192</parameter>
        <parameter name="enabled">true</parameter>
        <channels>
            <channel id="0"/>
            <channel id="1"/>
            <channel id="2"/>
        </channels>
    </module>
</configuration>
```

### JSON as Pivot Format

Like FhiclData, XmlData converts to/from JsonData for database storage. The conversion process:
1. Parses XML to an intermediate JSON structure
2. Base64-encodes the original XML for perfect round-trip fidelity
3. Embeds both in the final JSON document

This Base64 encoding is critical: it preserves the exact original XML format during round-trips through the database, including whitespace, comments, and formatting that would otherwise be lost in the JSON intermediate representation.

### Simpler Than FhiclData

XmlData has only `xml_buffer` (no filename metadata), making it simpler than FhiclData. This reflects the typical use case where XML configurations are programmatically generated or retrieved from external systems rather than local files.

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Multiple readers are safe; concurrent read/write requires external synchronization
- **Locking:** No internal locking; callers must synchronize access when sharing XmlData objects between threads

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/BasicTypes/common.h` | TRACE logging and Boost.Core utilities |

## Forward Declarations

```cpp
struct JsonData;
```

Forward declaration of JsonData enables conversion operators without requiring the full JsonData definition in this header.

## Classes/Structures

### `XmlData`

**Brief:** A value type that wraps XML-formatted configuration data with bidirectional JSON conversion support for database storage.

```cpp
struct XmlData final {
    XmlData(std::string);
    XmlData() = default;

    XmlData(JsonData const&);
    operator JsonData() const;
    operator std::string const&() const;

    static constexpr auto type_version() { return "V100"; }

    std::string xml_buffer;
};
```

**Thread Safety:** Not thread-safe for concurrent modification

#### Member Variables

##### `xml_buffer`

**Brief:** Holds the XML-formatted configuration string containing the complete XML document.

**Type:** `std::string`

**Default:** Empty string (when using default constructor)

#### Constructors

##### `XmlData(std::string buffer)`

**Brief:** Constructs an XmlData object from an XML-formatted string, using move semantics for efficiency.

**Parameters:**
- `buffer` - A string containing valid XML data. The string is moved into `xml_buffer`.

**Preconditions:**
- None (string may be empty, but empty strings will cause conversion failures)

**Postconditions:**
- `xml_buffer` contains the provided string

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Constructor does not throw |

**Thread Safety:** safe (construction)

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_xml.h"

using namespace artdaq::database::basictypes;

void createXmlData() {
  // Create from XML string
  XmlData config(R"(
    <config>
      <param name="threshold">100</param>
      <param name="enabled">true</param>
    </config>
  )");

  std::cout << "Config: " << config << "\n";
}
```

---

##### `XmlData()` (default)

**Brief:** Creates an empty XmlData with an empty `xml_buffer`, suitable for later assignment or stream input.

**Postconditions:**
- `xml_buffer` is empty string

**Thread Safety:** safe (construction)

---

##### `XmlData(JsonData const& document)`

**Brief:** Constructs XmlData by extracting and decoding XML from a JSON document that contains Base64-encoded XML data. This constructor is used when retrieving XML configurations from the database.

**Parameters:**
- `document` - A JsonData object containing Base64-encoded XML. Must not be empty.

**Preconditions:**
- `document` must not be empty
- `document` must contain a "base64" field with valid Base64-encoded data

**Postconditions:**
- `xml_buffer` contains the decoded XML configuration

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` | When `document` is empty |
| `std::runtime_error` | When regex fails to find "base64" field in JSON |
| `std::runtime_error` | When regex match returns unexpected size (not exactly 1 match) |
| `std::runtime_error` | When Base64 decoding or XML conversion fails |

**Thread Safety:** safe (construction)

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_xml.h"
#include "artdaq-database/BasicTypes/data_json.h"
#include <iostream>

using namespace artdaq::database::basictypes;

void convertFromJson() {
  // Assume we retrieved JSON from database
  JsonData json_from_db = retrieveFromDatabase("xml_config_id");

  try {
    XmlData xml(json_from_db);
    std::cout << "Retrieved XML configuration:\n" << xml << "\n";
  } catch (const std::runtime_error& e) {
    std::cerr << "Failed to convert JSON to XML: " << e.what() << "\n";
  }
}
```

#### Conversion Operators

##### `operator JsonData() const`

**Brief:** Converts the XML data to JSON format for database storage. The conversion includes Base64 encoding of the original XML content to preserve exact formatting during round-trips.

**Returns:** JsonData containing the XML configuration in JSON format with embedded Base64.

**Postconditions:**
- Returned JsonData can be converted back to identical XmlData

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` | When XML-to-JSON conversion fails (invalid XML syntax) |

**Thread Safety:** safe (reads only)

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_xml.h"
#include "artdaq-database/BasicTypes/data_json.h"

using namespace artdaq::database::basictypes;

void storeToDatabase() {
  XmlData xml("<config><buffer_size>8192</buffer_size></config>");

  try {
    // Implicit conversion to JsonData
    JsonData json = xml;

    // Store JSON in database
    storeInDatabase("xml_config", json);
  } catch (const std::runtime_error& e) {
    std::cerr << "Conversion failed: " << e.what() << "\n";
  }
}
```

---

##### `operator std::string const&() const`

**Brief:** Provides implicit conversion to const string reference, returning the raw XML content for direct access to the XML buffer.

**Returns:** Const reference to `xml_buffer`.

**Thread Safety:** safe (returns const reference)

**Example:**
```cpp
XmlData xml("<config/>");
std::string content = xml;  // Implicit conversion
const std::string& ref = xml;  // Reference to buffer
```

#### Static Methods

##### `type_version() -> const char*`

**Brief:** Returns a version identifier string used for database collection naming and schema compatibility tracking.

**Returns:** `"V100"` - indicates version 1.0.0 of the XmlData schema.

**Thread Safety:** safe (constexpr)

**Example:**
```cpp
// Used for collection naming
std::string collection = std::string("XmlData_") + XmlData::type_version();
// Result: "XmlData_V100"
```

## Functions

### `operator>>(std::istream& is, XmlData& data) -> std::istream&`

**Brief:** Reads XML data from an input stream. The input is expected to be JSON-encoded XML (as stored in the database), not raw XML.

**Parameters:**
- `is` - Input stream containing JSON-encoded XML data
- `data` - XmlData object to populate

**Returns:** Reference to the input stream.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` | When the stream content cannot be converted to XML |

**Thread Safety:** unsafe (modifies data)

**Side Effects:**
- Reads entire stream content
- Stream position will be at EOF after read

**Example:**
```cpp
#include <fstream>

std::ifstream file("config.json");  // JSON-encoded XML
XmlData xml;
file >> xml;  // Reads and converts
```

---

### `operator<<(std::ostream& os, XmlData const& data) -> std::ostream&`

**Brief:** Writes raw XML text to the output stream (not JSON-encoded). Useful for displaying or saving the XML configuration.

**Parameters:**
- `os` - Output stream
- `data` - XmlData object to write

**Returns:** Reference to the output stream.

**Thread Safety:** safe (reads only)

**Side Effects:**
- Writes `xml_buffer` content to stream

**Example:**
```cpp
XmlData xml("<config><param>value</param></config>");

// Write to console
std::cout << xml << "\n";

// Write to file
std::ofstream out("config.xml");
out << xml;
```

---

### `TraceStreamer::operator<<(const XmlData& r)` (template specialization)

**Brief:** Enables XmlData objects to be used directly in TRACE logging statements for debugging and diagnostics.

**Thread Safety:** safe (reads only)

**Example:**
```cpp
XmlData xml("<config/>");
TLOG(10) << "Configuration: " << xml;
```

## Relationship to Other Components

### Data Flow

```
XML Config --> XmlData --> JsonData --> Database (MongoDB/FileSystemDB)
Database --> JsonData --> XmlData --> XML Config
```

### Comparison with FhiclData

| Feature | XmlData | FhiclData |
|---------|---------|-----------|
| Buffer member | `xml_buffer` | `fhicl_buffer` |
| File name member | None | `fhicl_file_name` |
| Complexity | Simpler | Slightly more complex |
| Primary use | External systems integration | artdaq ecosystem configurations |

### Module Dependencies

- **data_xml.cpp** - Implements constructors, conversion operators, and template specializations
- **data_xml_fusion.h** - Provides Boost.Fusion adaptation for generic programming
- **data_json.h** - JsonData is the pivot format for all conversions
- **base64.h** - Used internally for encoding XML in JSON documents to preserve original format
- **DataFormats/Xml/** - Provides underlying XML parsing and conversion

## See Also

- [data_xml.cpp](./data_xml.cpp.md) - Implementation details
- [data_xml_fusion.h](./data_xml_fusion.h.md) - Boost.Fusion adaptation
- [data_json.h](./data_json.h.md) - JsonData pivot format
- [data_fhicl.h](./data_fhicl.h.md) - FHiCL format wrapper (similar structure)
- [base64.h](./base64.h.md) - Base64 encoding for round-trip preservation
- [basictypes.h](./basictypes.h.md) - Umbrella header

## Notes for Developers

### When to Use XML vs FHiCL

**Use XML when:**
- Interoperating with external XML-based systems
- Need standard XML tooling (XPath, XSLT, schema validation)
- Configuration comes from XML sources (web services, legacy systems)
- Strict schema validation is required (XML Schema, DTD)

**Use FHiCL when:**
- Working within the artdaq ecosystem
- Need artdaq-specific features (references, includes)
- Human-readable configuration files are priority
- Familiar with existing FHiCL configurations

### Common Pitfalls

- **Pitfall 1:** Expecting `operator>>` to read raw XML. It reads JSON-encoded XML. For raw XML, construct directly from a string.
- **Pitfall 2:** Not handling exceptions during conversion. Always wrap conversions in try-catch when dealing with untrusted input.
- **Pitfall 3:** Assuming XML schema validation occurs. The class stores raw XML without validation.

### Anti-patterns

```cpp
// DON'T do this - ignoring conversion errors:
JsonData json = database.get("config");
XmlData xml(json);  // May throw if JSON is invalid!

// DO this instead - handle conversion errors:
try {
  JsonData json = database.get("config");
  if (json.empty()) {
    std::cerr << "Configuration not found\n";
    return;
  }
  XmlData xml(json);
  processConfig(xml);
} catch (const std::runtime_error& e) {
  std::cerr << "Configuration error: " << e.what() << "\n";
}

// DON'T do this - reading raw XML with operator>>:
std::ifstream file("config.xml");  // Raw XML file
XmlData xml;
file >> xml;  // WRONG - expects JSON-encoded input!

// DO this instead - read raw XML as string:
std::ifstream file("config.xml");
std::string content((std::istreambuf_iterator<char>(file)), {});
XmlData xml(content);
```

### Known Issues

The header guard endif comment references the wrong file name:
```cpp
#endif /* _ARTDAQ_DATABASE_BASICTYPES_FHICL_H_ */  // Should be XML_H
```
This is a cosmetic issue and does not affect functionality.
