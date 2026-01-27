# xmljsondb.cpp

**Path:** `artdaq-database/DataFormats/Xml/xmljsondb.cpp`

**Implements:** [xmljsondb.h](./xmljsondb.h.md)

**Purpose:** Implements the high-level XML/JSON conversion functions that handle the complete transformation between XML configuration format and the database's JSON storage format, including origin metadata management.

## Implementation Overview

The implementation bridges between low-level reader/writer components and the database document format:

- **`xml_to_json()`:** Uses `XmlReader` to parse XML, then `JsonWriter` to serialize with added metadata
- **`json_to_xml()`:** Uses `JsonReader` to parse JSON, then `XmlWriter` to generate XML from the document data

Both functions construct or extract the standard database document structure with `document` and `origin` sections.

## Key Algorithms

### XML to JSON Conversion (`xml_to_json`)

**Steps:**
1. Validate preconditions (XML non-empty, output empty)
2. Create a JSON root object with `document` and `origin` keys
3. Populate origin metadata:
   - `format`: "xml"
   - `source`: "xml_to_json"
   - `timestamp`: current time
4. Use `XmlReader` to parse XML into the document node
5. Use `JsonWriter` to serialize the complete structure
6. Swap result into output parameter on success

**Data flow:**
```
Input XML string
    |
    v
XmlReader::read() -> json_node (document.data contents)
    |
    v
json_root = {
  "document": json_node,
  "origin": { format, source, timestamp }
}
    |
    v
JsonWriter::write() -> output JSON string
```

### JSON to XML Conversion (`json_to_xml`)

**Steps:**
1. Validate preconditions (JSON non-empty, output empty)
2. Use `JsonReader` to parse the database JSON
3. Extract the `document` section from the parsed structure
4. Use `XmlWriter` to generate XML from the document data
5. Swap result into output parameter on success

**Data flow:**
```
Input JSON string (database format)
    |
    v
JsonReader::read() -> json_root
    |
    v
Extract json_root["document"] -> json_node
    |
    v
XmlWriter::write(json_node) -> output XML string
```

## Internal Functions

### `get_object` (lambda in `xml_to_json`)

**Brief:** Helper lambda to extract a mutable reference to a nested JSON object by key.

**Purpose:** Provides convenient access to nested objects during document construction.

**Signature:**
```cpp
auto get_object = [&json_root](std::string const& name) -> auto& {
  return boost::get<jsn::object_t>(json_root[name]);
};
```

**Usage:** Accessing `origin` and `document` sections for modification.

### `get_object` (lambda in `json_to_xml`)

**Brief:** Helper lambda to extract a const reference to a nested JSON object by key.

**Purpose:** Provides convenient access to the `document` section for extraction.

**Signature:**
```cpp
auto get_object = [&json_root](std::string const& name) -> auto& {
  return boost::get<jsn::object_t>(json_root.at(name));
};
```

**Note:** Uses `.at()` for bounds checking, unlike the mutable version.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/common.h` | `confirm()` macro, TRACE macros, `timestamp()` function |
| `artdaq-database/DataFormats/shared_literals.h` | String literal constants (`document`, `origin`, etc.) |
| `artdaq-database/DataFormats/Json/json_common.h` | `JsonReader`, `JsonWriter` |
| `artdaq-database/DataFormats/Xml/xml_common.h` | `XmlReader`, `XmlWriter` |
| `artdaq-database/DataFormats/Xml/xmljsondb.h` | Function declarations |

## TRACE Configuration

```cpp
#define TRACE_NAME "xmljsondb.cpp"
```

**Trace points:**
- Level 10: Debug enable message in `debug::XmlJson()`
- Level 12: Entry/exit logging in `xml_to_json()`
- Level 13: Entry/exit and intermediate logging in `json_to_xml()`

## Document Structure

### Created by `xml_to_json()`

```json
{
  "document": {
    "data": {
      // Parsed XML content here
    }
  },
  "origin": {
    "format": "xml",
    "source": "xml_to_json",
    "timestamp": "2024-01-15T10:30:00.000000"
  }
}
```

### Expected by `json_to_xml()`

```json
{
  "document": {
    "data": {
      // Content to convert to XML
    }
  }
  // origin and other metadata ignored
}
```

## Performance Considerations

- **Two-pass processing:** Each conversion involves parsing and then serialization, requiring the full document in memory twice
- **String operations:** Multiple string copies and allocations occur during conversion
- **No streaming:** Entire documents must fit in memory

## Error Handling Strategy

Both functions use consistent error handling:

1. **Precondition assertions:** `confirm()` checks for valid input/output state
2. **Boolean return:** Indicates overall success/failure of the conversion
3. **Exception passthrough:** Underlying reader/writer exceptions propagate to caller

**Error scenarios:**

| Scenario | `xml_to_json` | `json_to_xml` |
|----------|---------------|---------------|
| Empty input | Assertion failure | Assertion failure |
| Non-empty output | Assertion failure | Assertion failure |
| Malformed input | Exception from reader | Exception from reader |
| Conversion failure | Returns `false` | Returns `false` |

## Testing Notes

- **Unit tests:** `test/DataFormats/xmljsondb_t.cc` (or similar)
- **Key test cases:**
  - Round-trip conversion (XML -> JSON -> XML)
  - Origin metadata preservation
  - Various XML structures
  - Error handling for malformed input

## Maintenance Notes

- The `literal::document` and `literal::origin` constants must match the database document schema used by storage providers.

- The timestamp is generated using `artdaq::database::timestamp()` from `common.h`, which provides microsecond precision.

- Different TRACE levels are used for the two functions (12 vs 13) to allow selective debugging.

## Code Structure

### `xml_to_json` Implementation

```cpp
bool dbxj::xml_to_json(std::string const& xml, std::string& json) {
  confirm(!xml.empty());
  confirm(json.empty());

  TLOG(12) << "xml_to_json: begin";

  auto result = bool{false};

  // 1. Create root structure
  auto json_root = jsn::object_t{};
  json_root[literal::document] = jsn::object_t{};
  json_root[literal::origin] = jsn::object_t{};

  auto get_object = [&json_root](std::string const& name) -> auto& {
    return boost::get<jsn::object_t>(json_root[name]);
  };

  // 2. Populate origin metadata
  get_object(literal::origin)[literal::format] = std::string("xml");
  get_object(literal::origin)[literal::source] = std::string("xml_to_json");
  get_object(literal::origin)[literal::timestamp] = artdaq::database::timestamp();

  // 3. Parse XML into document
  auto& json_node = get_object(literal::document);
  auto reader = xml::XmlReader{};
  result = reader.read(xml, json_node);

  if (!result) {
    return result;
  }

  // 4. Serialize to JSON string
  auto json1 = std::string{};
  auto writer = JsonWriter{};
  result = writer.write(json_root, json1);

  if (result) {
    json.swap(json1);
  }

  TLOG(12) << "xml_to_json: end";
  return result;
}
```

### `json_to_xml` Implementation

```cpp
bool dbxj::json_to_xml(std::string const& json, std::string& xml) {
  confirm(!json.empty());
  confirm(xml.empty());

  TLOG(13) << "json_to_xml: begin";

  auto result = bool{false};

  // 1. Parse JSON
  auto json_root = jsn::object_t{};
  auto reader = JsonReader{};
  result = reader.read(json, json_root);

  if (!result) {
    TLOG(13) << "json_to_xml: Unable to read JSON buffer";
    return result;
  }

  // 2. Extract document section
  auto get_object = [&json_root](std::string const& name) -> auto& {
    return boost::get<jsn::object_t>(json_root.at(name));
  };

  auto& json_node = get_object(literal::document);

  // 3. Generate XML
  auto xml1 = std::string{};
  auto writer = XmlWriter{};
  result = writer.write(json_node, xml1);

  if (result) {
    xml.swap(xml1);
  }

  TLOG(13) << "json_to_xml: end";
  return result;
}
```

## See Also

- [xmljsondb.h](./xmljsondb.h.md) - Public API declaration
- [xml_reader.cpp](./xml_reader.cpp.md) - XML parsing implementation
- [xml_writer.cpp](./xml_writer.cpp.md) - XML generation implementation
- [../Json/json_reader.cpp](../Json/json_reader.cpp.md) - JSON parsing implementation
- [../Json/json_writer.cpp](../Json/json_writer.cpp.md) - JSON serialization implementation
- [shared_literals.h](../shared_literals.h.md) - String constant definitions
