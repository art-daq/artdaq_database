# data_xml.cpp

## File Overview

**Location**: `/home/user/artdaq-database/artdaq-database/BasicTypes/data_xml.cpp`

This file implements the `XmlData` class methods, including the critical conversion logic between XML and JSON formats. The implementation is very similar to `data_fhicl.cpp`, using Base64 encoding/decoding and integrating with the artdaq XML-to-JSON conversion library.

**Purpose**: Implements XmlData class functionality and XML ↔ JSON conversion logic.

## Dependencies

### BasicTypes Dependencies
- `artdaq-database/BasicTypes/data_json.h` - JsonData class
- `artdaq-database/BasicTypes/data_json_fusion.h` - Fusion adaptation for JsonData
- `artdaq-database/BasicTypes/data_xml.h` - XmlData class declaration
- `artdaq-database/BasicTypes/data_xml_fusion.h` - Fusion adaptation for XmlData
- `artdaq-database/BasicTypes/base64.h` - Base64 encoding/decoding

### DataFormats Dependencies
- `artdaq-database/DataFormats/Xml/xml_common.h` - XML common utilities
- `artdaq-database/DataFormats/Xml/xmljsondb.h` - XML↔JSON conversion functions
- `artdaq-database/DataFormats/shared_literals.h` - Shared string literals

### Standard Library
- `<utility>` - For `std::move`

### TRACE Configuration

```cpp
#define TRACE_NAME "data_xml.cpp"
```

## Regular Expressions

### Namespace: regex

```cpp
namespace regex {
constexpr auto parse_base64data = R"lit([\s\S]*"base64"\s*:\s*"(\S*?)")lit";
}
```

**Purpose**: Regular expression pattern for extracting Base64-encoded data from JSON.

**Pattern**: Identical to the pattern used in `data_fhicl.cpp`

**Pattern Breakdown**:
- `[\s\S]*` - Match any characters (including newlines)
- `"base64"` - Match literal "base64" key
- `\s*:\s*` - Match colon with optional whitespace
- `"(\S*?)"` - Capture group for the Base64 value (non-greedy)

**Example Match**:
```json
{
    "some": "data",
    "base64": "PGNvbmZpZz48L2NvbmZpZz4=",
    "other": "data"
}
```
Captures: `PGNvbmZpZz48L2NvbmZpZz4=`

## Template Specializations

### JsonData::convert_to<XmlData>

```cpp
template <>
bool JsonData::convert_to(XmlData& xml) const {
    using artdaq::database::xmljson::json_to_xml;
    return json_to_xml(json_buffer, xml.xml_buffer);
}
```

**Purpose**: Converts JSON representation to XML format.

**Parameters**:
- `xml` - XmlData object to populate (output parameter)

**Return Value**: `true` if conversion succeeded, `false` otherwise

**Implementation**:
- Uses `artdaq::database::xmljson::json_to_xml` function
- Converts `json_buffer` (JSON) → `xml.xml_buffer` (XML)

**Difference from FHICL**:
- XmlData has no file_name member, so only 2 parameters passed to conversion function
- FhiclData conversion passes 3 parameters (including file_name)

**Usage**:
```cpp
JsonData json(R"({"element": "value"})");
XmlData xml;
if (json.convert_to(xml)) {
    std::cout << "Converted: " << xml.xml_buffer << "\n";
}
```

---

### JsonData::convert_from<XmlData>

```cpp
template <>
bool JsonData::convert_from(XmlData const& xml) {
    using artdaq::database::xmljson::xml_to_json;
    return xml_to_json(xml.xml_buffer, json_buffer);
}
```

**Purpose**: Converts XML format to JSON representation.

**Parameters**:
- `xml` - XmlData object to convert from (input parameter)

**Return Value**: `true` if conversion succeeded, `false` otherwise

**Implementation**:
- Uses `artdaq::database::xmljson::xml_to_json` function
- Converts `xml.xml_buffer` (XML) → `json_buffer` (JSON)

**Usage**:
```cpp
XmlData xml("<config><param>value</param></config>");
JsonData json("");
if (json.convert_from(xml)) {
    std::cout << "JSON: " << json.json_buffer << "\n";
}
```

## XmlData Methods

### Constructor: XmlData(std::string)

```cpp
XmlData::XmlData(std::string buffer) : xml_buffer{std::move(buffer)} {}
```

**Purpose**: Constructs XmlData from an XML string.

**Parameters**:
- `buffer` - XML string (moved into member)

**Implementation**: Uses move semantics for efficiency

**Example**:
```cpp
std::string config = "<daq><param>1000</param></daq>";
XmlData xml(std::move(config));
// config is now empty, xml.xml_buffer contains the data
```

---

### String Conversion Operator

```cpp
XmlData::operator std::string const&() const {
    return xml_buffer;
}
```

**Purpose**: Provides implicit conversion to const string reference.

**Return Value**: Reference to `xml_buffer`

**Example**:
```cpp
XmlData xml("<config></config>");
std::string str = xml;  // Implicit conversion
```

---

### Constructor: XmlData(JsonData const&)

```cpp
XmlData::XmlData(JsonData const& document) {
    // ... detailed implementation below
}
```

**Purpose**: Constructs XmlData by converting from JSON-encoded format.

**Process**:

1. **Validation**:
   ```cpp
   confirm(!document.empty());
   ```
   Ensures the JSON document is not empty.

2. **Logging**:
   ```cpp
   TLOG(11) << "XML document=" << document;
   ```

3. **Regex Search**:
   ```cpp
   auto ex = std::regex(regex::parse_base64data);
   auto results = std::smatch();

   if (!std::regex_search(document.json_buffer, results, ex)) {
       throw std::runtime_error("JSON to XML convertion error, regex_search()==false; JSON buffer: " + document.json_buffer);
   }
   ```
   Extracts Base64-encoded data from JSON.

4. **Validation of Results**:
   ```cpp
   if (results.size() != 1) {
       throw std::runtime_error(
           "JSON to XML convertion error, "
           "regex_search().size()!=1; JSON buffer: " +
           document.json_buffer);
   }
   ```
   Ensures exactly one match was found.

5. **Base64 Decoding**:
   ```cpp
   auto base64 = std::string(results[0]);
   TLOG(12) << "XML base64=" << base64;

   auto json = base64_decode(base64);
   TLOG(13) << "XML  json=" << json;
   ```
   Decodes the Base64 string to get intermediate JSON.

6. **JSON to XML Conversion**:
   ```cpp
   JsonData(json).convert_to(*this);
   TLOG(14) << "XML xml=" << xml_buffer;
   ```
   Converts the intermediate JSON to XML format.

**Throws**: `std::runtime_error` if:
- Document is empty
- Regex search fails
- Multiple Base64 fields found
- Conversion fails

**Complete Data Flow**:
```
JSON with Base64
    ↓ regex_search
Base64 string
    ↓ base64_decode
Intermediate JSON
    ↓ convert_to
XML buffer
```

**Note**: Process is identical to FhiclData constructor, just with XML instead of FHICL.

---

### JSON Conversion Operator

```cpp
XmlData::operator JsonData() const {
    // ... detailed implementation below
}
```

**Purpose**: Converts XmlData to JSON format with Base64 encoding.

**Process**:

1. **Logging**:
   ```cpp
   TLOG(15) << "XML xml=" << xml_buffer;
   ```

2. **XML to JSON Conversion**:
   ```cpp
   auto json = JsonData("");
   if (!json.convert_from(*this)) {
       throw std::runtime_error("XML to JSON convertion error; XML buffer: " + this->xml_buffer);
   }
   TLOG(16) << "XML  json=" << json;
   ```

3. **Collection Name Creation**:
   ```cpp
   auto collection = std::string("XmlData_") + type_version();
   // Result: "XmlData_V100"
   ```

4. **Base64 Encoding**:
   ```cpp
   auto base64 = base64_encode(xml_buffer);
   TLOG(15) << "XML base64=" << base64;  // Note: Reuses TLOG(15)
   ```

5. **Document Creation**:
   ```cpp
   std::ostringstream os;
   os << json;
   TLOG(15) << "XML document=" << os.str();  // Note: Reuses TLOG(15)

   return {os.str()};
   ```

**Throws**: `std::runtime_error` if conversion from XML to JSON fails

**Complete Data Flow**:
```
XML buffer
    ↓ convert_from
Intermediate JSON
    ↓ base64_encode (of original XML)
Base64 string
    ↓ embed in JSON
JSON with Base64
```

**Note**: Similar to FhiclData conversion operator.

## Stream Operators

### operator>>

```cpp
std::istream& operator>>(std::istream& is, XmlData& data) {
    auto str = std::string(std::istreambuf_iterator<char>(is), {});
    auto json = JsonData(str);
    data = XmlData(json);
    return is;
}
```

**Purpose**: Reads XML data from input stream (in JSON-encoded format).

**Process**:
1. Read entire stream into string
2. Create JsonData from string
3. Convert JsonData to XmlData
4. Assign to output parameter

**Important**: Input must be JSON-encoded XML, not raw XML!

**Usage**:
```cpp
std::ifstream file("config.json");
XmlData xml;
file >> xml;
```

---

### operator<<

```cpp
std::ostream& operator<<(std::ostream& os, XmlData const& data) {
    os << data.xml_buffer;
    return os;
}
```

**Purpose**: Writes raw XML string to output stream.

**Output**: Raw XML text (not JSON-encoded)

**Usage**:
```cpp
XmlData xml("<config></config>");
std::cout << xml;  // Outputs: <config></config>
```

## TRACE Logging Levels

The implementation uses different TRACE levels:

- **TLOG(11-14)**: JSON → XML conversion steps
- **TLOG(15-16)**: XML → JSON conversion steps (note: TLOG(15) is reused multiple times)

**Debug Example**:
```
TLOG(11): "XML document={...}"
TLOG(12): "XML base64=PGNvbmZpZz4=..."
TLOG(13): "XML  json={...}"
TLOG(14): "XML xml=<config>...</config>"
```

## TRACE Level Reuse

**Note**: TLOG(15) is used multiple times in the JSON conversion operator:
```cpp
TLOG(15) << "XML xml=" << xml_buffer;      // First use
TLOG(15) << "XML base64=" << base64;       // Second use (should be 17)
TLOG(15) << "XML document=" << os.str();   // Third use (should be 18)
```

This appears to be a copy-paste oversight from data_fhicl.cpp. The FhiclData version uses TLOG levels 17, 20, and 21 for these messages.

## Comparison with data_fhicl.cpp

### Similarities
- Overall structure is identical
- Uses same Base64 encoding/decoding
- Same regex pattern for extracting Base64
- Same error handling approach
- Similar TRACE logging

### Differences

| Aspect | XmlData | FhiclData |
|--------|---------|-----------|
| Conversion functions | `xml_to_json`, `json_to_xml` | `fhicl_to_json`, `json_to_fhicl` |
| Member count | 1 (`xml_buffer`) | 2 (`fhicl_buffer`, `fhicl_file_name`) |
| Conversion params | 2 (buffer only) | 3 (buffer + file name) |
| TRACE levels | Reuses TLOG(15) | Uses TLOG(17, 20, 21) |
| Collection name | "XmlData_V100" | "FhiclData_V100" |

## Usage Context

### Typical Workflow

```cpp
// 1. Create from string
XmlData xml1("<config><param>value</param></config>");

// 2. Read from file (as JSON)
std::ifstream file("config.json");
XmlData xml2;
file >> xml2;

// 3. Convert to JSON for storage
JsonData json = xml1;

// 4. Store json in database
database.store(json);

// 5. Retrieve and convert back
JsonData retrieved = database.get();
XmlData xml3(retrieved);

// 6. Use the XML
std::cout << xml3;
```

### Integration with Other BasicTypes

```cpp
// XML as alternative to FHICL
// Both use JSON as intermediate format

XmlData xml("<param>value</param>");
JsonData json1 = xml;  // XML → JSON

FhiclData fhicl("param: value");
JsonData json2 = fhicl;  // FHICL → JSON

// Both json1 and json2 can be stored in database
// Format choice depends on source system
```

## Error Handling

### Current Implementation

Similar to FhiclData:
- Throws `std::runtime_error` on conversion failures
- Checks for empty documents
- Validates regex search results

### Recommended Practices

```cpp
try {
    JsonData json = get_from_database();

    if (json.empty()) {
        TLOG(1) << "Empty JSON data";
        return;
    }

    XmlData xml(json);  // May throw
    std::cout << xml;

} catch (const std::runtime_error& e) {
    TLOG(1) << "Conversion error: " << e.what();
    // Handle error appropriately
}
```

## Performance Considerations

### String Operations

1. **Constructor**: Uses move semantics - O(1)
2. **Conversion operator**: Returns reference - O(1)
3. **Stream read**: Reads entire stream - O(n) where n is stream size
4. **Conversions**: Involve regex, Base64, XML parsing - O(n)

### Memory Usage

- Single `std::string` member - size of XML content
- Intermediate strings created during conversion
- Base64 encoding increases size by ~33%

## Design Patterns

### Parallel Design

XmlData follows the exact same design pattern as FhiclData:
- Two-stage conversion (XML ↔ JSON ↔ Base64)
- Base64 encoding for safe storage
- Template specializations for conversions
- Stream I/O operators

**Benefit**: Uniform interface for all BasicTypes

## Notes for Developers

### Why Base64?

Same reasons as FHICL:
- Preserves XML special characters
- No JSON escaping issues
- Safe storage in JSON documents
- Data integrity across conversions

### Regex Pattern

Expects JSON with a "base64" key (same as FHICL):

```json
{
    "some_field": "value",
    "base64": "encoded_xml_here"
}
```

### Collection Naming

Collection names include version:
```cpp
"XmlData_V100"
```

### Thread Safety

**Not thread-safe** for concurrent writes to the same object.

### Typo in Error Messages

Error messages say "convertion" instead of "conversion":
```cpp
"JSON to XML convertion error..."  // Should be "conversion"
```

This is consistent with data_fhicl.cpp (same typo).

## Testing Recommendations

```cpp
// Test round-trip conversion
XmlData original("<config><param>value</param></config>");
JsonData json = original;
XmlData restored(json);
assert(std::string(original) == std::string(restored));

// Test error handling
try {
    JsonData empty("");
    XmlData xml(empty);  // Should throw
    assert(false);
} catch (const std::runtime_error&) {
    // Expected
}

// Test with complex XML
XmlData complex(R"(
    <?xml version="1.0"?>
    <root>
        <nested>
            <array>
                <item>1</item>
                <item>2</item>
                <item>3</item>
            </array>
        </nested>
    </root>
)");
// Verify conversion preserves structure
```

## Related Documentation

- `data_xml.h.md` - XmlData class interface
- `data_xml_fusion.h.md` - Boost.Fusion adaptation
- `base64.cpp.md` - Base64 encoding/decoding details
- `data_json.cpp.md` - JSON data implementation
- `data_fhicl.cpp.md` - FHICL implementation (very similar pattern)
