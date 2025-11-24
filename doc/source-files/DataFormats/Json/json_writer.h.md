# json_writer.h

## File Overview

This header defines a Boost.Spirit Karma-based generator for JSON documents. It converts internal AST representation (object_t) to formatted JSON text.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Json/json_writer.h`

## Dependencies

### Third-Party Libraries
- `<boost/spirit/include/karma.hpp>` - Boost.Spirit Karma generator framework

### Project Headers
- `"artdaq-database/DataFormats/Json/json_types.h"` - JSON AST types
- `"artdaq-database/DataFormats/common.h"` - Common includes

## Namespace Aliases

```cpp
using namespace boost::spirit;
```

## Generator Grammar

### json_generator_grammar

```cpp
template <typename Iter>
struct json_generator_grammar : karma::grammar<Iter, object_t()>
```

**Template Parameters**:
- `Iter` - Output iterator type (typically `std::back_insert_iterator<std::string>`)

**Base Type**: `karma::grammar<Iter, object_t()>`
- Consumes: `object_t` (JSON object AST)
- Produces: JSON text via output iterator

**Grammar Rules**:

```cpp
karma::rule<Iter, std::string()> quoted_string;
karma::rule<Iter, value_t()> value_rule;
karma::rule<Iter, key_t()> key_rule;
karma::rule<Iter, data_t()> data_rule;
karma::rule<Iter, object_t()> start, object_rule;
karma::rule<Iter, array_t()> array_rule;
```

**Grammar Definition** (from constructor):

```cpp
// Value can be any JSON type
value_rule = quoted_string | karma::long_ | karma::double_ | karma::bool_ | object_rule | array_rule;

// Quoted string: "..."
quoted_string = '"' << karma::string << '"';

// Object key (quoted string)
key_rule = quoted_string;

// Key-value pair: "key" : value
data_rule = key_rule << " : " << value_rule;

// JSON object: {\n key:value,\n ... \n}
object_rule = "{\n" << -(data_rule % ",\n") << "\n}";

// JSON array: [\n value,\n ... \n]
array_rule = "[\n" << -(value_rule % ",\n") << "\n]";

// Top-level is object
start = object_rule;
```

**Formatting**:
- Objects and arrays use newlines for readability
- Comma-separated elements
- Space after colon in key-value pairs
- No indentation (flat formatting)

## JsonWriter Class

```cpp
struct JsonWriter final {
    bool write(object_t const&, std::string&);
};
```

**Method**: `write(ast, output)`

**Parameters**:
- `ast` - Input object_t (must not be empty)
- `output` - Output string (must be empty)

**Returns**: `true` if generation succeeded, `false` otherwise

**Preconditions**:
- `ast` must not be empty
- `output` must be empty

**Behavior**:
- Creates back-insert iterator
- Creates generator grammar
- Invokes Boost.Spirit generate
- On success, swaps generated text into output parameter

## Usage Examples

### Basic Generation

```cpp
using namespace artdaq::database::json;

object_t ast;
ast["key"] = std::string("value");
ast["number"] = static_cast<integer>(42);

std::string output;
JsonWriter writer;
if (writer.write(ast, output)) {
    std::cout << output;
    // Output:
    // {
    // "key" : "value",
    // "number" : 42
    // }
}
```

### Round-trip Conversion

```cpp
std::string original = R"({"a": 1, "b": 2})";

// Parse
object_t ast;
JsonReader{}.read(original, ast);

// Generate
std::string formatted;
JsonWriter{}.write(ast, formatted);
// formatted contains pretty-printed JSON
```

### Nested Structures

```cpp
object_t config;
config["host"] = std::string("localhost");
config["port"] = static_cast<integer>(8080);

array_t numbers;
numbers.push_back(static_cast<integer>(1));
numbers.push_back(static_cast<integer>(2));

object_t root;
root["config"] = config;
root["numbers"] = numbers;

std::string json;
JsonWriter{}.write(root, json);
// Produces nested JSON with proper formatting
```

## Output Format

**Objects**:
```json
{
"key1" : value1,
"key2" : value2
}
```

**Arrays**:
```json
[
value1,
value2
]
```

**Values**:
- Strings: `"text"`
- Integers: `42`
- Decimals: `3.14`
- Booleans: `true` or `false`

## Performance Considerations

**Memory**:
- Pre-reserves 10000 bytes for output buffer
- Uses back_insert_iterator for efficient appending

**Optimization**:
- Grammar instantiated for each generation
- Could be optimized with static grammar

**Monitoring**:
- TRACE logs for outputs >512 bytes
- Stack traces for heavy operations

## Design Notes

**Formatting Choice**:
- Newlines improve readability
- No indentation keeps output compact
- Format suitable for both human and machine reading

**Type Handling**:
- Integers and decimals automatically formatted
- Booleans output as lowercase (JSON standard)
- Strings automatically quoted
- No escape sequence handling needed (Karma handles it)

## Related Files

- **json_writer.cpp** - Implementation of JsonWriter::write()
- **json_reader.h** - Complementary parser (JSON text to AST)
- **json_types.h** - AST type definitions
- **Boost.Spirit Karma documentation** - Generator framework

## Notes

- Generator requires input to be object_t (not array_t or value_t)
- Writer is non-throwing - check return value
- Output always well-formed JSON
- Boost.Fusion adaptation enables automatic rule synthesis
- Debug nodes commented out for performance
