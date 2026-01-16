# xml_reader.cpp

**Path:** `artdaq-database/DataFormats/Xml/xml_reader.cpp`

**Implements:** [xml_reader.h](./xml_reader.h.md)

**Purpose:** Implements the `XmlReader::read()` method that converts XML documents to JSON AST using Boost.PropertyTree. The implementation uses a recursive lambda to traverse the property tree and build the corresponding JSON object structure.

## Implementation Overview

The XML-to-JSON conversion follows a two-stage process:

1. **Parse XML:** Use `boost::property_tree::read_xml()` to parse the input string into a `ptree`
2. **Convert to JSON AST:** Recursively traverse the ptree and build a `jsn::object_t` structure

The result is wrapped in a `"data"` key to conform to the database document format.

## Key Algorithms

### XML to JSON AST Conversion

The core algorithm uses a recursive lambda function to traverse the Boost property tree and construct the JSON object tree.

**Steps:**
1. Create an outer JSON object with a `"data"` key containing an empty object
2. Parse the XML string into a `boost::property_tree::ptree` using `read_xml()`
3. Define a recursive `convert` lambda that processes each ptree node
4. For each node in the ptree:
   - If the node has no children (leaf node): add as a string key-value pair
   - If the node has children: create a nested object and recurse
5. Swap the result into the output parameter if successful

**Why this approach:** The recursive lambda cleanly handles arbitrarily nested XML structures without requiring external state management. Using `swap()` for the final assignment provides strong exception safety.

### Conversion Rules

| XML Structure | JSON Result |
|---------------|-------------|
| `<tag>value</tag>` | `{"tag": "value"}` |
| `<parent><child>x</child></parent>` | `{"parent": {"child": "x"}}` |
| Empty element `<tag/>` | `{"tag": {}}` |

## Internal Functions

### `convert` (lambda)

**Brief:** Recursively converts a Boost property tree node to a JSON object node.

**Called by:** `XmlReader::read()` (defined inline as a local lambda)

**Purpose:** Handles the recursive traversal of XML element hierarchies, distinguishing between leaf nodes (text content) and branch nodes (nested elements).

**Parameters:**
- `xml_tree` - The current property tree node being processed
- `json_tree` - Reference to the JSON object being built

**Behavior:**
- For leaf nodes (empty children): extracts the data string and adds as key-value
- For branch nodes: creates a nested object and recurses into children

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/common.h` | `confirm()` macro, TRACE macros |
| `artdaq-database/DataFormats/Xml/convertxml2json.h` | (unused in current impl, included for potential future use) |
| `artdaq-database/DataFormats/Xml/xml_reader.h` | Class declaration |
| `artdaq-database/DataFormats/shared_literals.h` | `literal::data` constant |
| `<boost/property_tree/ptree.hpp>` | Property tree data structure |
| `<boost/property_tree/xml_parser.hpp>` | XML parsing functions |

## TRACE Configuration

```cpp
#define TRACE_NAME "xml_reader.cpp"
```

**Trace points:**
- Level 10: Debug enable message in `debug::XmlReader()`
- Level 12: Entry/exit logging in `read()` method
- Level 12: Exception details when parsing fails

## Performance Considerations

- **Memory usage:** The entire XML document is loaded into memory twice (once as ptree, once as JSON AST). For large documents, this can be significant.
- **String copying:** Leaf values are copied as strings; no zero-copy optimization is performed.
- **No streaming:** The entire document must be parsed before any results are available.

## Error Handling Strategy

The implementation uses a combination of:

1. **Precondition assertions:** `confirm(!in.empty())` and `confirm(json_object.empty())` catch programming errors in debug builds
2. **Exception propagation:** Boost.PropertyTree exceptions are caught, logged, and re-thrown
3. **Return value:** Returns `false` for semantic failures (empty parse result) vs. exceptions for syntax errors

**Error flow:**
```
XML string -> read_xml()
  |-> xml_parser_error (malformed XML) -> catch -> log -> rethrow
  |-> success -> convert() -> empty check -> return false if empty
                           -> swap result -> return true
```

## Testing Notes

- **Unit tests:** `test/DataFormats/xml_t.cc` (or similar)
- **Key test cases:**
  - Simple single-element XML
  - Nested multi-level XML
  - Empty elements
  - Malformed XML (exception handling)
  - XML with attributes (ptree `<xmlattr>` handling)

## Maintenance Notes

- The `confirm()` macro is used instead of `assert()` for precondition checking. This integrates with the project's error handling infrastructure.

- The `literal::data` constant is defined in `shared_literals.h` and must match the key expected by downstream consumers (particularly `XmlWriter` for round-trip compatibility).

- Exception handling catches `std::exception` but the more specific `boost::property_tree::xml_parser_error` may be thrown. Consider catching this specifically if different error messages are needed.

## Code Structure

```cpp
bool XmlReader::read(std::string const& in, jsn::object_t& json_object) {
  // Precondition checks
  confirm(!in.empty());
  confirm(json_object.empty());

  try {
    // 1. Create output structure
    auto object = jsn::object_t();
    object[literal::data] = jsn::object_t();

    // 2. Parse XML to ptree
    pt::ptree xml_tree;
    std::istringstream sin(in);
    pt::read_xml(sin, xml_tree);

    // 3. Define recursive converter
    std::function<void(pt::ptree const&, jsn::object_t&)> convert;
    convert = [&convert](pt::ptree const& xml_tree, jsn::object_t& json_tree) {
      for (auto const& xml_branch : xml_tree) {
        if (xml_branch.second.empty()) {
          // Leaf node: add string value
          json_tree.push_back({xml_branch.first, std::string(xml_branch.second.data())});
        } else {
          // Branch node: create nested object and recurse
          json_tree.push_back({xml_branch.first, jsn::object_t{}});
          auto& json_branch = boost::get<jsn::object_t>(json_tree.back().value);
          convert(xml_branch.second, json_branch);
        }
      }
    };

    // 4. Execute conversion
    convert(xml_tree, json_tree);

    // 5. Check for empty result
    if (json_tree.empty()) {
      return false;
    }

    // 6. Commit result
    json_object.swap(object);
    return true;

  } catch (std::exception const& e) {
    // Log and rethrow
    TLOG(12) << "read() Caught exception message=" << e.what();
    std::cerr << "Caught exception message=" << e.what() << "\n";
    throw;
  }
}
```

## See Also

- [xml_reader.h](./xml_reader.h.md) - Public API declaration
- [xml_writer.cpp](./xml_writer.cpp.md) - Reverse conversion implementation
- [json_types.h](../Json/json_types.h.md) - JSON AST type definitions
- [External: Boost.PropertyTree XML Parser](https://www.boost.org/doc/libs/release/libs/property_tree/doc/html/boost_propertytree/parsers.html#boost_propertytree.parsers.xml_parser)
