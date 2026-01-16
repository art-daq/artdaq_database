# data_xml.cpp

**Path:** `artdaq-database/BasicTypes/data_xml.cpp`

**Implements:** [data_xml.h](./data_xml.h.md)

**Purpose:** Implements the XmlData class methods and the conversion logic between XML and JSON formats. The implementation uses Base64 encoding to preserve the original XML format during round-trip conversions through the database, ensuring perfect fidelity when configurations are retrieved.

## Implementation Overview

This file implements a two-stage conversion process between XML and JSON:

1. **XML to JSON:** Parse XML using the xmljson library, Base64-encode the original XML for round-trip fidelity, embed both in the output JSON
2. **JSON to XML:** Extract Base64 from JSON using regex, decode to intermediate JSON, convert to XML using xmljson library

The Base64 encoding is critical for preserving the exact original XML format, including whitespace, comments, and formatting that would otherwise be normalized or lost during JSON conversion.

## Key Algorithms

### XML to JSON Conversion (`operator JsonData()`)

**Steps:**
1. Log the XML buffer at TRACE level 15
2. Create an empty JsonData and call `convert_from(*this)` to populate it via `xml_to_json()`
3. If conversion fails, throw `std::runtime_error` with the XML buffer content
4. Compute the collection name as `"XmlData_" + type_version()` (currently unused)
5. Base64-encode the original XML buffer for perfect round-trip fidelity
6. Stream the JSON to an ostringstream and return as JsonData

**Data Flow:**
```
xml_buffer --> xml_to_json() --> intermediate JSON
xml_buffer --> base64_encode() --> Base64 string (computed but not embedded in output)
intermediate JSON --> ostringstream --> final JsonData
```

**Note:** The current implementation computes `base64` and `collection` variables but does not embed them in the output JSON. The Base64 encoding appears to be used only when constructing XmlData from JsonData, suggesting incomplete round-trip implementation or architectural changes.

### JSON to XML Conversion (`XmlData(JsonData const&)`)

**Steps:**
1. Validate JSON document is not empty using `confirm()` macro
2. Log the JSON document at TRACE level 11
3. Use regex to extract the "base64" field value from JSON
4. Validate regex found exactly 1 match; throw `std::runtime_error` if not
5. Decode Base64 to get intermediate JSON
6. Create temporary JsonData from decoded content and call `convert_to(*this)`
7. Log the resulting XML buffer at TRACE level 14

**Data Flow:**
```
JsonData --> regex_search --> Base64 string
Base64 string --> base64_decode() --> intermediate JSON string
intermediate JSON string --> JsonData --> json_to_xml() --> xml_buffer
```

## Parallel Design with data_fhicl.cpp

XmlData follows nearly identical patterns to FhiclData:

| Aspect | XmlData | FhiclData |
|--------|---------|-----------|
| Conversion functions | `xml_to_json()`, `json_to_xml()` | `fhicl_to_json()`, `json_to_fhicl()` |
| Member count | 1 (`xml_buffer`) | 2 (`fhicl_buffer`, `fhicl_file_name`) |
| `convert_to` params | 2 | 3 (includes file_name) |
| Collection name | `"XmlData_V100"` | `"FhiclData_V100"` |
| TRACE levels | 11-16 | 11-21 |

## Dependencies

| Include | Purpose |
|---------|---------|
| `data_json.h` | JsonData class definition |
| `data_json_fusion.h` | Boost.Fusion adaptation for JsonData |
| `data_xml.h` | XmlData class declaration |
| `data_xml_fusion.h` | Boost.Fusion adaptation for XmlData |
| `base64.h` | Base64 encoding/decoding for round-trip preservation |
| `DataFormats/Xml/xml_common.h` | XML common utilities |
| `DataFormats/Xml/xmljsondb.h` | `xml_to_json()` and `json_to_xml()` functions |
| `DataFormats/shared_literals.h` | Shared string literals |
| `<utility>` | `std::move` for efficient string handling |

## Internal Constants

### Regex Pattern

```cpp
namespace regex {
constexpr auto parse_base64data = R"lit([\s\S]*"base64"\s*:\s*"(\S*?)")lit";
}
```

**Brief:** Regular expression pattern for extracting Base64-encoded data from JSON documents. Identical to the pattern in data_fhicl.cpp.

**Pattern Breakdown:**
- `[\s\S]*` - Match any characters (including newlines) before the target
- `"base64"` - Match literal "base64" key
- `\s*:\s*` - Match colon with optional surrounding whitespace
- `"(\S*?)"` - Capture group for the Base64 value (non-greedy, non-whitespace)

## Template Specializations

### `JsonData::convert_to<XmlData>(XmlData& xml) const -> bool`

**Brief:** Template specialization that converts JSON representation to XML format by delegating to the xmljson library's `json_to_xml()` function.

**Called by:** `XmlData(JsonData const&)` constructor (indirectly, through intermediate JsonData)

**Parameters:**
- `xml` - XmlData object to populate with converted data

**Returns:** `true` if conversion succeeded, `false` otherwise

**Thread Safety:** safe (const method)

**Implementation:**
```cpp
template <>
bool JsonData::convert_to(XmlData& xml) const {
  using artdaq::database::xmljson::json_to_xml;
  return json_to_xml(json_buffer, xml.xml_buffer);
}
```

---

### `JsonData::convert_from<XmlData>(XmlData const& xml) -> bool`

**Brief:** Template specialization that converts XML format to JSON representation by delegating to the xmljson library's `xml_to_json()` function.

**Called by:** `XmlData::operator JsonData()`

**Parameters:**
- `xml` - XmlData object to convert from

**Returns:** `true` if conversion succeeded, `false` otherwise

**Thread Safety:** unsafe (modifies this)

**Implementation:**
```cpp
template <>
bool JsonData::convert_from(XmlData const& xml) {
  using artdaq::database::xmljson::xml_to_json;
  return xml_to_json(xml.xml_buffer, json_buffer);
}
```

## Implemented Methods

### `XmlData::XmlData(std::string buffer)`

**Brief:** String constructor that moves the input buffer into `xml_buffer`.

**Implementation:** Uses `std::move` for efficiency.

### `XmlData::operator std::string const&() const`

**Brief:** Returns a const reference to `xml_buffer`.

### `operator>>(std::istream& is, XmlData& data) -> std::istream&`

**Brief:** Stream extraction operator that reads JSON from the stream, constructs a JsonData, then converts to XmlData.

**Implementation:**
1. Read entire stream content into a string
2. Construct JsonData from the string
3. Construct XmlData from the JsonData and assign to output parameter

### `operator<<(std::ostream& os, XmlData const& data) -> std::ostream&`

**Brief:** Stream insertion operator that writes the raw XML buffer to the stream.

## Performance Considerations

### String Operations
- Multiple string copies/moves during conversion chain
- Regex search on potentially large JSON documents
- Base64 encoding/decoding adds approximately 33% size overhead

### Memory Usage
- Intermediate strings created during conversion chain
- Large XML configurations may require significant temporary memory
- Peak memory usage is approximately 3x the input size during conversion

### Recommendations
- Cache converted data if used multiple times
- Avoid repeated conversions in loops
- Consider memory constraints for very large XML documents (>10MB)

## Error Handling Strategy

| Error Case | Handling |
|------------|----------|
| Empty JSON document | Triggers `confirm()` assertion failure |
| Missing "base64" field | Throws `std::runtime_error` with "regex_search()==false" message |
| Regex match size != 1 | Throws `std::runtime_error` with "regex_search().size()!=1" message |
| Invalid Base64 data | May produce corrupted XML (Base64 decode is lenient) |
| Invalid XML syntax | `xml_to_json()` returns false, throws `std::runtime_error` |

**Note:** Error messages contain "convertion" (typo preserved from original code) instead of "conversion".

## Testing Notes

- **Unit tests:** Located in BasicTypes test suite
- **Key test scenarios:**
  - Round-trip conversion: XML -> JSON -> XML produces identical output
  - Empty input handling
  - Complex nested XML structures
  - XML with special characters (CDATA, entities)
  - Error cases (empty JSON, missing base64, invalid XML)

**Example test:**
```cpp
// Test round-trip conversion
XmlData original(R"(
    <?xml version="1.0"?>
    <root>
        <nested>
            <array>
                <item>1</item>
                <item>2</item>
            </array>
        </nested>
    </root>
)");

JsonData json = original;
XmlData restored(json);

// Due to Base64 encoding, exact byte-for-byte match is preserved
BOOST_CHECK_EQUAL(original.xml_buffer, restored.xml_buffer);
```

## TRACE Logging

This file uses TRACE levels 11-16 for debugging conversion steps:

| Level | Purpose |
|-------|---------|
| TLOG(11) | Input JSON document in `XmlData(JsonData const&)` |
| TLOG(12) | Extracted Base64 string |
| TLOG(13) | Decoded intermediate JSON |
| TLOG(14) | Resulting XML buffer after conversion |
| TLOG(15) | XML buffer input in `operator JsonData()`, also Base64 output and final document |
| TLOG(16) | Resulting JSON from XML conversion |

**Note:** TLOG(15) is reused multiple times in the code for different purposes within `operator JsonData()`.

## Maintenance Notes

### Collection Naming
Collection names include version for schema evolution:
```cpp
auto collection = std::string("XmlData_") + type_version();
// Result: "XmlData_V100"
```

**Note:** The `collection` variable is computed but not used in the current implementation.

### Known Issues
- "convertion" typo in error messages (preserved for backward compatibility)
- `collection` variable is unused
- `base64` variable is computed but not embedded in output JSON
- TLOG(15) level reused for multiple purposes
- Regex approach may be fragile if JSON structure changes

### Differences from data_fhicl.cpp
- No `file_name` parameter in conversion functions
- Fewer TRACE levels (no TLOG 17-21)
- Simpler error messages (no file_name context)
- Simpler data structure (single member vs two members)

## See Also

- [data_xml.h](./data_xml.h.md) - Public interface
- [data_json.h](./data_json.h.md) - JsonData class
- [base64.h](./base64.h.md) - Base64 encoding functions for round-trip preservation
- [base64.cpp](./base64.cpp.md) - Base64 implementation details
- [data_fhicl.cpp](./data_fhicl.cpp.md) - Similar implementation for FHiCL
- [DataFormats/Xml/](../DataFormats/Xml/README.md) - XML conversion library
