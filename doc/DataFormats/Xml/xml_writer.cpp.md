# xml_writer.cpp

**Path:** `artdaq-database/DataFormats/Xml/xml_writer.cpp`

**Implements:** [xml_writer.h](./xml_writer.h.md)

**Purpose:** Implements the `XmlWriter::write()` method that converts JSON AST to XML documents using Boost.PropertyTree. The implementation uses helper templates for type detection and a recursive lambda to traverse the JSON object tree and build the corresponding XML property tree.

## Implementation Overview

The JSON-to-XML conversion follows a two-stage process:

1. **Traverse JSON AST:** Recursively walk the JSON object tree, building a Boost `ptree`
2. **Generate XML:** Use `boost::property_tree::write_xml()` to serialize the ptree to a string

The input must have a `"data"` key; only its contents are serialized to XML.

## Key Algorithms

### JSON AST to XML Conversion

The core algorithm uses a recursive lambda function to traverse the JSON object tree and construct a Boost property tree suitable for XML output.

**Steps:**
1. Extract the `"data"` object from the input JSON AST
2. Create an empty `boost::property_tree::ptree` for XML output
3. Define a recursive `convert` lambda that processes each JSON node
4. For each key-value pair in a JSON object:
   - If the value is a leaf (VALUE type): add as a string element
   - If the value is an object: create a child ptree and recurse
5. Write the ptree to XML with formatting settings
6. Swap the result into the output parameter if successful

**Why this approach:** The recursive lambda cleanly handles arbitrarily nested JSON structures. Using Boost.PropertyTree for XML generation ensures well-formed output with proper escaping and encoding.

### Type Detection

The implementation uses template helper functions to determine value types:

| Helper Function | Purpose |
|-----------------|---------|
| `is_leaf_type<T>()` | Returns true if the value is a primitive (string, number, boolean) |
| `leaf_string_value<T>()` | Converts any leaf value to its string representation |

## Internal Functions

### `is_leaf_type<T>(T& leaf) -> bool`

**Brief:** Determines whether a JSON AST node contains a leaf value (primitive type) rather than a nested structure.

**Called by:** `convert` lambda in `XmlWriter::write()`

**Purpose:** Distinguishes between leaf nodes (which become XML text content) and branch nodes (which become nested XML elements).

**Parameters:**
- `leaf` - Reference to a JSON data node (key-value pair)

**Returns:** `true` if the node's value is a primitive type (VALUE), `false` for OBJECT or ARRAY types.

**Implementation:**
```cpp
template <typename T>
bool is_leaf_type(T& leaf) {
  confirm(!leaf.value.empty());
  return jsn::type(leaf.value) == jsn::type_t::VALUE;
}
```

### `leaf_string_value<T>(T& leaf) -> std::string`

**Brief:** Extracts the string representation of a leaf node's value using the `tostring_visitor`.

**Called by:** `convert` lambda in `XmlWriter::write()`

**Purpose:** Converts any primitive JSON value (string, integer, decimal, boolean) to a string for XML serialization.

**Parameters:**
- `leaf` - Reference to a JSON data node containing a primitive value

**Returns:** String representation of the value.

**Implementation:**
```cpp
template <typename T>
std::string leaf_string_value(T& leaf) {
  confirm(!leaf.value.empty());
  return boost::apply_visitor(jsn::tostring_visitor(), leaf.value);
}
```

### `convert` (lambda)

**Brief:** Recursively converts a JSON object node to a Boost property tree node.

**Called by:** `XmlWriter::write()` (defined inline as a local lambda)

**Purpose:** Handles the recursive traversal of JSON object hierarchies, creating the corresponding XML element structure.

**Parameters:**
- `json_tree` - The current JSON object being processed
- `xml_tree` - Reference to the property tree being built

**Behavior:**
- For leaf values: uses `ptree::add()` to create a text element
- For object values: uses `ptree::add_child()` to create a nested element and recurses

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/common.h` | `confirm()` macro, TRACE macros |
| `artdaq-database/DataFormats/shared_literals.h` | `literal::data` constant |
| `artdaq-database/DataFormats/Xml/convertxml2json.h` | (included for consistency) |
| `artdaq-database/DataFormats/Xml/xml_writer.h` | Class declaration |
| `<boost/property_tree/ptree.hpp>` | Property tree data structure |
| `<boost/property_tree/xml_parser.hpp>` | XML generation functions |

## TRACE Configuration

```cpp
#define TRACE_NAME "xml_writer.cpp"
```

**Trace points:**
- Level 10: Debug enable message in `debug::XmlWriter()`
- Level 12: Entry/exit logging in `write()` method
- Level 12: Exception details when generation fails

**Note:** The exception logging message incorrectly says "read()" but occurs in the `write()` method.

## Performance Considerations

- **Memory usage:** The JSON AST and property tree exist simultaneously in memory during conversion. For large documents, this doubles memory requirements.
- **String conversion:** All values are converted to strings before XML generation, even if they were originally numeric types.
- **Formatting overhead:** The XML writer adds indentation and formatting, which increases output size and processing time.

## Error Handling Strategy

The implementation uses a combination of:

1. **Precondition assertions:** `confirm(out.empty())` and `confirm(!json_object.empty())` catch programming errors
2. **Exception propagation:** Boost and STL exceptions are caught, logged, and re-thrown
3. **Return value:** Returns `false` for semantic failures (empty output) vs. exceptions for structural errors

**Error flow:**
```
JSON AST -> extract "data" key
  |-> out_of_range/bad_get (missing/wrong type) -> catch -> log -> rethrow
  |-> success -> convert() -> write_xml() -> empty check -> return false if empty
                                          -> swap result -> return true
```

## XML Output Format

The generated XML has the following characteristics:

- **Declaration:** `<?xml version="1.0" encoding="utf-8"?>`
- **Indentation:** 4 spaces per level
- **Encoding:** UTF-8

**Settings used:**
```cpp
pt::xml_writer_make_settings<std::string>(' ', 4)
```

## Testing Notes

- **Unit tests:** `test/DataFormats/xml_t.cc` (or similar)
- **Key test cases:**
  - Simple single-key JSON
  - Nested multi-level JSON
  - Round-trip conversion (XML -> JSON -> XML)
  - Missing "data" key (exception handling)
  - Various value types (strings, numbers, booleans)

## Maintenance Notes

- The `is_leaf_type` and `leaf_string_value` templates are defined at file scope but are not exposed in the header. They could be moved to an anonymous namespace for better encapsulation.

- The exception message in the catch block says "read()" but should say "write()". This is a minor bug in the logging.

- The `literal::data` constant must match the key used by `XmlReader` and other components for round-trip compatibility.

## Code Structure

```cpp
bool XmlWriter::write(jsn::object_t const& json_object, std::string& out) {
  // Precondition checks
  confirm(out.empty());
  confirm(!json_object.empty());

  try {
    // 1. Extract "data" section
    auto const& json_tree = boost::get<jsn::object_t>(json_object.at(literal::data));

    // 2. Create property tree for XML
    pt::ptree xml_tree;

    // 3. Define recursive converter
    std::function<void(jsn::object_t const&, pt::ptree&)> convert;
    convert = [&convert](jsn::object_t const& json_tree, pt::ptree& xml_tree) {
      for (auto const& json_branch : json_tree) {
        if (is_leaf_type(json_branch)) {
          // Leaf: add as text element
          xml_tree.add(json_branch.key, leaf_string_value(json_branch));
        } else {
          // Object: create child element and recurse
          auto& xml_branch = xml_tree.add_child(json_branch.key, {});
          auto const& json_branch_value = boost::get<jsn::object_t>(json_branch.value);
          convert(json_branch_value, xml_branch);
        }
      }
    };

    // 4. Execute conversion
    convert(json_tree, xml_tree);

    // 5. Generate XML string
    std::ostringstream sout;
    pt::write_xml(sout, xml_tree, pt::xml_writer_make_settings<std::string>(' ', 4));
    auto buffer = sout.str();

    // 6. Check for empty result
    if (buffer.empty()) {
      return false;
    }

    // 7. Commit result
    out.swap(buffer);
    return true;

  } catch (std::exception const& e) {
    TLOG(12) << "read() Caught exception message=" << e.what();  // Note: says "read" but is write
    std::cerr << "Caught exception message=" << e.what() << "\n";
    throw;
  }
}
```

## See Also

- [xml_writer.h](./xml_writer.h.md) - Public API declaration
- [xml_reader.cpp](./xml_reader.cpp.md) - Reverse conversion implementation
- [json_types.h](../Json/json_types.h.md) - JSON AST type definitions and visitors
- [External: Boost.PropertyTree XML Parser](https://www.boost.org/doc/libs/release/libs/property_tree/doc/html/boost_propertytree/parsers.html#boost_propertytree.parsers.xml_parser)
