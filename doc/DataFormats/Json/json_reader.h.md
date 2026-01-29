# json_reader.h

**Path:** `artdaq-database/DataFormats/Json/json_reader.h`

**Purpose:** Defines a Boost.Spirit Qi-based parser for JSON documents, converting JSON text into the internal AST representation (`object_t`). This file provides the core parsing functionality for reading JSON configuration data throughout the artdaq-database system, along with a utility function for comparing two JSON objects.


## Key Concepts

### Boost.Spirit Qi Parser Framework

Boost.Spirit Qi is a parser generator library that uses C++ template metaprogramming to create parsers at compile time. Key concepts:

- **Grammar**: A set of rules defining valid syntax. In Spirit Qi, grammars are C++ structs inheriting from `qi::grammar`.
- **Rule**: A single parsing rule that matches input and produces an attribute (output value).
- **Skipper**: A parser that automatically skips certain characters (like whitespace) between tokens.
- **Attribute**: The value produced by a successful parse. Rules can synthesize complex types.

### JSON AST Types

The parser produces an Abstract Syntax Tree (AST) using these types from `json_types.h`:

- `object_t`: A JSON object (key-value pairs, ordered list)
- `array_t`: A JSON array (ordered list of values)
- `value_t`: A variant type holding any JSON value (string, number, boolean, object, array)
- `data_t`: A key-value pair (`key_t` + `value_t`)
- `key_t`: A string key (alias for `std::string`)

## Thread Safety

- **Thread-safe:** No (grammar and parser are not thread-safe)
- **Concurrent access:** Each thread must create its own `JsonReader` instance
- **Locking:** No internal locking; callers must synchronize if sharing instances

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/Json/json_types.h` | JSON AST type definitions (`object_t`, `array_t`, `value_t`, etc.) |
| `artdaq-database/DataFormats/common.h` | Common includes, TRACE logging macros, `confirm()` assertions |
| `<boost/spirit/include/qi.hpp>` | Boost.Spirit Qi parser framework |

## Classes/Structures

### `json_parser_grammar<Iter>`

A Boost.Spirit Qi grammar that defines the syntax rules for parsing JSON documents.

**Thread Safety:** Not thread-safe. Create a new instance for each parse operation.

```cpp
template <typename Iter>
struct json_parser_grammar : qi::grammar<Iter, object_t(), ascii::space_type>
```

**Template Parameters:**
- `Iter` - Iterator type for input (typically `std::string::const_iterator`)

**Grammar Rules:**

| Rule | Type | Description |
|------|------|-------------|
| `start` | `object_t` | Entry point; matches a JSON object |
| `object_rule` | `object_t` | Matches `{ key:value, ... }` |
| `array_rule` | `array_t` | Matches `[ value, ... ]` |
| `data_rule` | `data_t` | Matches `"key" : value` |
| `key_rule` | `key_t` | Matches a quoted string for keys |
| `value_rule` | `value_t` | Matches any JSON value type |
| `quoted_string` | `std::string` | Matches `"..."` with escape handling |
| `text_string` | `std::string` | Matches text inside quotes |
| `escape_rule` | `std::string` | Matches escape sequences (`\\`, `\"`, `\b`, `\f`, `\n`, `\r`, `\t`) |

**Grammar Definition:**
```cpp
escape_rule = ascii::char_('\\') >> ascii::char_("\\\"bfnrt");
text_string = +(escape_rule | ~ascii::char_('"'));
quoted_string = qi::lexeme['"' >> *text_string >> '"'];
object_rule = '{' >> -(data_rule % ',') >> '}';
key_rule = quoted_string;
data_rule = key_rule >> ':' >> value_rule;
array_rule = '[' >> -(value_rule % ',') >> ']';
value_rule = object_rule | array_rule | quoted_string |
             (qi::long_ >> !qi::lit('.')) | qi::double_ | qi::bool_;
start = object_rule;
```

### `JsonReader`

A class that provides a simple interface for parsing JSON strings into AST objects.

**Thread Safety:** Not thread-safe. Create a new instance per thread or synchronize access.

```cpp
struct JsonReader final {
  bool read(std::string const&, object_t&);
};
```

#### Methods

##### `read(input, ast) -> bool`

**Brief:** Parses a JSON string and populates the output AST object with the parsed structure.

**Parameters:**
- `input` - The JSON string to parse. Must not be empty.
- `ast` - Output parameter for the parsed AST. Must be empty before calling.

**Preconditions:**
- `input` must not be empty (enforced by `confirm()`)
- `ast` must be empty (enforced by `confirm()`)

**Returns:** `true` if parsing succeeded and the AST was populated; `false` if parsing failed.

**Postconditions:**
- On success: `ast` contains the parsed JSON structure
- On failure: `ast` remains empty (unchanged)

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | This method is non-throwing; errors are indicated by return value |

**Thread Safety:** Not thread-safe

**Side Effects:**
- TRACE logging at levels 20-23 for debugging
- Stack trace logged for inputs larger than 512 bytes

**Example:**
```cpp
#include "artdaq-database/DataFormats/Json/json_reader.h"
#include <iostream>

using namespace artdaq::database::json;

void parseConfiguration() {
  std::string json_text = R"({
    "detector": {
      "name": "TPC",
      "channels": 1024,
      "enabled": true
    }
  })";

  object_t ast;
  JsonReader reader;

  if (reader.read(json_text, ast)) {
    // Access parsed data
    auto& detector = boost::get<object_t>(ast.at("detector"));
    auto& name = boost::get<std::string>(detector.at("name"));
    std::cout << "Detector: " << name << "\n";  // Output: Detector: TPC
  } else {
    std::cerr << "Failed to parse JSON configuration\n";
  }
}
```

## Functions

### `compare_json_objects(first, second) -> std::pair<bool, std::string>`

**Brief:** Compares two JSON strings for structural and value equality by parsing both and comparing their AST representations.

**Parameters:**
- `first` - First JSON string to compare. Must not be empty.
- `second` - Second JSON string to compare. Must not be empty.

**Preconditions:**
- `first` must not be empty (enforced by `confirm()`)
- `second` must not be empty (enforced by `confirm()`)

**Returns:** A pair where:
- `.first` is `true` if both JSON documents are equal, `false` otherwise
- `.second` is an error/difference message (empty on exact match, descriptive on difference)

**Postconditions:**
- Return value accurately reflects the comparison result

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Parse errors are returned as error messages in the result pair |

**Thread Safety:** Safe to call concurrently (creates internal reader instances)

**Example:**
```cpp
#include "artdaq-database/DataFormats/Json/json_reader.h"
#include <iostream>

using namespace artdaq::database::json;

void validateConfiguration() {
  std::string expected = R"({"threshold": 100, "enabled": true})";
  std::string actual = R"({"threshold": 100, "enabled": false})";

  auto [equal, message] = compare_json_objects(expected, actual);

  if (!equal) {
    std::cerr << "Configuration mismatch: " << message << "\n";
    // Handle the difference
  } else {
    std::cout << "Configurations match\n";
  }
}

void handleParseErrors() {
  std::string valid_json = R"({"key": "value"})";
  std::string invalid_json = R"({"key": value})";  // Missing quotes

  auto [equal, message] = compare_json_objects(valid_json, invalid_json);

  if (!equal) {
    if (message.find("Unable to read") != std::string::npos) {
      std::cerr << "Parse error: " << message << "\n";
    }
  }
}
```

## Relationship to Other Components

This file is part of the DataFormats/Json module which provides JSON parsing and generation:

```
json_reader.h (this file)
    |
    +-- json_types.h (AST type definitions)
    |
    +-- Used by: JsonDocument, ConfigurationDB, StorageProviders
    |
    +-- Complementary: json_writer.h (generates JSON from AST)
```

The `JsonReader` is used throughout the codebase whenever JSON text needs to be converted to the internal AST representation for manipulation or storage.

## See Also

- [json_reader.cpp](./json_reader.cpp.md) - Implementation of `JsonReader::read()` and `compare_json_objects()`
- [json_writer.h](./json_writer.h.md) - Complementary JSON generator (AST to text)
- [json_types.h](./json_types.h.md) - JSON AST type definitions
- [External: Boost.Spirit Qi](https://www.boost.org/doc/libs/release/libs/spirit/doc/html/spirit/qi.html) - Parser framework documentation

## Notes for Developers

### Common Pitfalls

- **Pitfall 1:** Passing a non-empty `ast` parameter to `read()`. The function uses `confirm()` which will abort in debug builds. Always pass an empty `object_t`.

- **Pitfall 2:** Expecting array-only JSON to parse. The grammar's start rule requires the top-level to be an object (`{}`). Arrays at the top level will fail to parse.

- **Pitfall 3:** Assuming thread safety. Each thread must create its own `JsonReader` instance; the grammar is not designed for concurrent use.

### Anti-patterns

```cpp
// DON'T do this - ast must be empty:
object_t ast;
ast["existing"] = std::string("data");
JsonReader{}.read(json_str, ast);  // Will fail confirm() assertion

// DO this instead - use a fresh ast:
object_t ast;  // Empty
JsonReader{}.read(json_str, ast);  // Correct

// DON'T do this - top-level must be object:
std::string array_json = R"([1, 2, 3])";
object_t ast;
JsonReader{}.read(array_json, ast);  // Will return false

// DO this instead - wrap in object if needed:
std::string object_json = R"({"items": [1, 2, 3]})";
object_t ast;
JsonReader{}.read(object_json, ast);  // Correct
```

### Parsing Details

**Number Handling:**
- Integers are matched with `qi::long_` followed by negative lookahead for `.` to prevent matching decimals
- Decimals are matched with `qi::double_`
- Scientific notation is supported via `qi::double_`

**String Handling:**
- Escape sequences supported: `\\`, `\"`, `\b`, `\f`, `\n`, `\r`, `\t`
- Unicode escapes (`\uXXXX`) are not explicitly handled by the grammar
- `qi::lexeme` prevents whitespace skipping inside strings

**Performance:**
- Grammar is instantiated for each parse operation
- For high-throughput scenarios, consider caching the grammar instance
- Inputs larger than 512 bytes trigger stack trace logging for performance monitoring
