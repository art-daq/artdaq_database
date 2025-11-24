# json_reader.h

## File Overview

This header defines a Boost.Spirit Qi-based parser for JSON documents. It provides a grammar that parses JSON text into the internal AST representation (object_t) and includes a comparison utility for JSON objects.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Json/json_reader.h`

## Dependencies

### Third-Party Libraries
- `<boost/spirit/include/qi.hpp>` - Boost.Spirit Qi parser framework

### Project Headers
- `"artdaq-database/DataFormats/Json/json_types.h"` - JSON AST types
- `"artdaq-database/DataFormats/common.h"` - Common includes

## Namespace Aliases

```cpp
using namespace boost::spirit;
```

## Parser Grammar

### json_parser_grammar

```cpp
template <typename Iter>
struct json_parser_grammar : qi::grammar<Iter, object_t(), ascii::space_type>
```

**Template Parameters**:
- `Iter` - Iterator type (typically `std::string::const_iterator`)

**Base Type**: `qi::grammar<Iter, object_t(), ascii::space_type>`
- Produces: `object_t` (JSON object AST)
- Skipper: `ascii::space_type` (automatically skips whitespace)

**Grammar Rules**:

```cpp
qi::rule<Iter, std::string(), ascii::space_type> quoted_string;
qi::rule<Iter, value_t(), ascii::space_type> value_rule;
qi::rule<Iter, key_t(), ascii::space_type> key_rule;
qi::rule<Iter, data_t(), ascii::space_type> data_rule;
qi::rule<Iter, object_t(), ascii::space_type> start, object_rule;
qi::rule<Iter, array_t(), ascii::space_type> array_rule;
qi::rule<Iter, std::string()> text_string;
qi::rule<Iter, std::string()> escape_rule;
```

**Grammar Definition** (from constructor):

```cpp
// Escape sequences: \\ \" \b \f \n \r \t
escape_rule = ascii::char_('\\') >> ascii::char_("\\\"bfnrt");

// Text inside quotes (with escapes)
text_string = +(escape_rule | ~ascii::char_('"'));

// Quoted string: "..."
quoted_string = qi::lexeme['"' >> *text_string >> '"'];

// JSON object: { key:value, key:value, ... }
object_rule = '{' >> -(data_rule % ',') >> '}';

// Object key (quoted string)
key_rule = quoted_string;

// Key-value pair: "key" : value
data_rule = key_rule >> ':' >> value_rule;

// JSON array: [ value, value, ... ]
array_rule = '[' >> -(value_rule % ',') >> ']';

// JSON value: object | array | string | integer | float | boolean
value_rule = object_rule | array_rule | quoted_string |
             (qi::long_ >> !qi::lit('.')) | qi::double_ | qi::bool_;

// Top-level must be object
start = object_rule;
```

**Key Features**:
- **Escape handling**: Supports standard JSON escape sequences
- **Type inference**: Automatically determines number types (integer vs decimal)
- **Integer detection**: Uses `(qi::long_ >> !qi::lit('.'))` to distinguish integers from decimals
- **Recursive**: Handles nested objects and arrays
- **Whitespace skipping**: Automatically ignores whitespace
- **Comma-separated lists**: Uses `%` operator for comma-delimited sequences

## JsonReader Class

```cpp
struct JsonReader final {
    bool read(std::string const&, object_t&);
};
```

**Method**: `read(input, ast)`

**Parameters**:
- `input` - JSON string to parse
- `ast` - Output object_t (must be empty)

**Returns**: `true` if parsing succeeded, `false` otherwise

**Preconditions**:
- `input` must not be empty
- `ast` must be empty

**Behavior**:
- Creates parser grammar
- Invokes Boost.Spirit phrase_parse
- On success, swaps parsed AST into output parameter
- On failure, leaves output parameter unchanged

## Comparison Utility

```cpp
std::pair<bool, std::string> compare_json_objects(
    std::string const& first,
    std::string const& second);
```

**Purpose**: Compares two JSON strings for equality.

**Algorithm**:
1. Parse first JSON string
2. Parse second JSON string
3. Compare resulting ASTs using operator==
4. Return comparison result with error message

**Returns**:
- `.first`: `true` if equal, `false` otherwise
- `.second`: "Success" or detailed error message

**Error Messages**:
- "Unable to read first Json buffer" - Parse error on first string
- "Unable to read second Json buffer" - Parse error on second string
- Plus any differences from AST comparison

## Usage Examples

### Basic Parsing

```cpp
using namespace artdaq::database::json;

std::string json_text = R"({"key": "value", "number": 42})";
object_t ast;

JsonReader reader;
if (reader.read(json_text, ast)) {
    // Success - ast contains parsed structure
    auto& key_value = boost::get<std::string>(ast.at("key"));
    // key_value == "value"
}
```

### Comparing JSON

```cpp
std::string json1 = R"({"a": 1, "b": 2})";
std::string json2 = R"({"a": 1, "b": 3})";

auto [equal, message] = compare_json_objects(json1, json2);
if (!equal) {
    std::cout << "JSON differs: " << message << "\n";
}
```

### Parsing Complex Structures

```cpp
std::string json = R"({
    "config": {
        "host": "localhost",
        "port": 8080,
        "enabled": true
    },
    "items": [1, 2, 3, "four"]
})";

object_t ast;
if (JsonReader{}.read(json, ast)) {
    // Access nested object
    auto& config = boost::get<object_t>(ast.at("config"));

    // Access array
    auto& items = boost::get<array_t>(ast.at("items"));
}
```

## Parsing Details

**Number Handling**:
- Integers: Matched first with `qi::long_` followed by negative lookahead for '.'
- Decimals: Matched with `qi::double_`
- Scientific notation: Supported via `qi::double_`

**String Handling**:
- Supports escape sequences: `\\`, `\"`, `\b`, `\f`, `\n`, `\r`, `\t`
- Uses `qi::lexeme` to prevent whitespace skipping inside strings

**Whitespace**:
- Automatically skipped between tokens via `ascii::space_type` skipper
- Preserved inside quoted strings via `qi::lexeme`

**Error Handling**:
- Parser returns false on syntax errors
- No exception thrown on parse failure
- Use TRACE logging to debug parse issues (enable with BOOST_SPIRIT_DEBUG_NODE)

## Performance Considerations

**Optimization**:
- Grammar compiled once per parse operation
- Could be optimized by making grammar static
- Memory pre-allocated for result buffer

**Tracing**:
- Debug nodes commented out for performance
- Enable by uncommenting BOOST_SPIRIT_DEBUG_NODE calls
- TRACE logging for large inputs (>512 bytes)

## Related Files

- **json_reader.cpp** - Implementation of JsonReader::read()
- **json_writer.h** - Complementary generator (AST to JSON text)
- **json_types.h** - AST type definitions
- **Boost.Spirit Qi documentation** - Parser framework

## Notes

- Grammar requires input to be a JSON object (not array or value)
- Parser is non-throwing - check return value
- Comparison utility is useful for testing and validation
- Boost.Fusion adaptation (in json_types.h) enables automatic rule synthesis
