# json_writer.h

**Path:** `artdaq-database/DataFormats/Json/json_writer.h`

**Purpose:** Defines a Boost.Spirit Karma-based generator for JSON documents, converting the internal AST representation (`object_t`) into formatted JSON text. This file provides the core generation functionality for writing JSON configuration data throughout the artdaq-database system.


## Key Concepts

### Boost.Spirit Karma Generator Framework

Boost.Spirit Karma is a generator library (the inverse of Qi) that uses C++ template metaprogramming to create output generators at compile time. Key concepts:

- **Grammar**: A set of rules defining output format. In Spirit Karma, grammars are C++ structs inheriting from `karma::grammar`.
- **Rule**: A single generation rule that consumes an attribute (input value) and produces output.
- **Output Iterator**: An iterator that receives generated characters (typically `std::back_insert_iterator`).
- **Attribute**: The value consumed by the generator to produce output.

### JSON AST Types

The generator consumes these types from `json_types.h`:

- `object_t`: A JSON object (key-value pairs, ordered list)
- `array_t`: A JSON array (ordered list of values)
- `value_t`: A variant type holding any JSON value (string, number, boolean, object, array)
- `data_t`: A key-value pair (`key_t` + `value_t`)
- `key_t`: A string key (alias for `std::string`)

## Thread Safety

- **Thread-safe:** No (grammar and generator are not thread-safe)
- **Concurrent access:** Each thread must create its own `JsonWriter` instance
- **Locking:** No internal locking; callers must synchronize if sharing instances

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/Json/json_types.h` | JSON AST type definitions (`object_t`, `array_t`, `value_t`, etc.) |
| `artdaq-database/DataFormats/common.h` | Common includes, TRACE logging macros, `confirm()` assertions |
| `<boost/spirit/include/karma.hpp>` | Boost.Spirit Karma generator framework |

## Classes/Structures

### `json_generator_grammar<Iter>`

A Boost.Spirit Karma grammar that defines the generation rules for producing JSON text from AST.

**Thread Safety:** Not thread-safe. Create a new instance for each generation operation.

```cpp
template <typename Iter>
struct json_generator_grammar : karma::grammar<Iter, object_t()>
```

**Template Parameters:**
- `Iter` - Output iterator type (typically `std::back_insert_iterator<std::string>`)

**Grammar Rules:**

| Rule | Type | Description |
|------|------|-------------|
| `start` | `object_t` | Entry point; generates a JSON object |
| `object_rule` | `object_t` | Generates `{ key:value, ... }` with newlines |
| `array_rule` | `array_t` | Generates `[ value, ... ]` with newlines |
| `data_rule` | `data_t` | Generates `"key" : value` |
| `key_rule` | `key_t` | Generates a quoted string for keys |
| `value_rule` | `value_t` | Generates any JSON value type |
| `quoted_string` | `std::string` | Generates `"..."` |

**Grammar Definition:**
```cpp
value_rule = quoted_string | karma::long_ | karma::double_ | karma::bool_ | object_rule | array_rule;
quoted_string = karma::lit('"') << karma::string << karma::lit('"');
key_rule = quoted_string;
data_rule = key_rule << " : " << value_rule;
object_rule = karma::lit('{') << karma::eol << -(data_rule % (karma::lit(',') << karma::eol)) << karma::eol << karma::lit('}');
array_rule = karma::lit('[') << karma::eol << -(value_rule % (karma::lit(',') << karma::eol)) << karma::eol << karma::lit(']');
start = object_rule;
```

**Output Format:**
- Objects and arrays include newlines for readability
- No indentation (flat formatting)
- Space after colon in key-value pairs
- Comma-separated elements with newlines

### `JsonWriter`

A class that provides a simple interface for generating JSON strings from AST objects.

**Thread Safety:** Not thread-safe. Create a new instance per thread or synchronize access.

```cpp
struct JsonWriter final {
  bool write(object_t const&, std::string&);
};
```

#### Methods

##### `write(ast, output) -> bool`

**Brief:** Converts a JSON AST object to a formatted JSON string.

**Parameters:**
- `ast` - The AST object to convert. Must not be empty.
- `output` - Output parameter for the generated JSON string. Must be empty before calling.

**Preconditions:**
- `ast` must not be empty (enforced by `confirm()`)
- `output` must be empty (enforced by `confirm()`)

**Returns:** `true` if generation succeeded; `false` if generation failed.

**Postconditions:**
- On success: `output` contains the formatted JSON string
- On failure: `output` remains empty (unchanged)

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | This method is non-throwing; errors are indicated by return value |

**Thread Safety:** Not thread-safe

**Side Effects:**
- TRACE logging at levels 21-24 for debugging
- Stack trace logged for outputs larger than 512 bytes

**Complexity:** O(n) where n is the total size of the AST

**Example:**
```cpp
#include "artdaq-database/DataFormats/Json/json_writer.h"
#include "artdaq-database/DataFormats/Json/json_types.h"
#include <iostream>

using namespace artdaq::database::json;

void writeConfiguration() {
  // Build AST
  object_t ast;
  ast["detector"] = object_t{};
  auto& detector = boost::get<object_t>(ast.at("detector"));
  detector["name"] = std::string("TPC");
  detector["channels"] = static_cast<integer>(1024);
  detector["enabled"] = true;

  // Generate JSON
  std::string json_output;
  JsonWriter writer;

  if (writer.write(ast, json_output)) {
    std::cout << json_output;
    // Output:
    // {
    // "detector" : {
    // "name" : "TPC",
    // "channels" : 1024,
    // "enabled" : true
    // }
    // }
  } else {
    std::cerr << "Failed to generate JSON\n";
  }
}
```

## Relationship to Other Components

This file is part of the DataFormats/Json module which provides JSON parsing and generation:

```
json_writer.h (this file)
    |
    +-- json_types.h (AST type definitions)
    |
    +-- Used by: JsonDocument, ConfigurationDB, StorageProviders
    |
    +-- Complementary: json_reader.h (parses JSON to AST)
```

The `JsonWriter` is used throughout the codebase whenever the internal AST representation needs to be serialized to JSON text for storage or transmission.

## See Also

- [json_writer.cpp](./json_writer.cpp.md) - Implementation of `JsonWriter::write()`
- [json_reader.h](./json_reader.h.md) - Complementary JSON parser (text to AST)
- [json_types.h](./json_types.h.md) - JSON AST type definitions
- [External: Boost.Spirit Karma](https://www.boost.org/doc/libs/release/libs/spirit/doc/html/spirit/karma.html) - Generator framework documentation

## Notes for Developers

### Common Pitfalls

- **Pitfall 1:** Passing a non-empty `output` parameter to `write()`. The function uses `confirm()` which will abort in debug builds. Always pass an empty `std::string`.

- **Pitfall 2:** Expecting array-only output. The grammar's start rule requires the input to be an `object_t`. To serialize an array, wrap it in an object first.

- **Pitfall 3:** Assuming thread safety. Each thread must create its own `JsonWriter` instance; the grammar is not designed for concurrent use.

### Anti-patterns

```cpp
// DON'T do this - output must be empty:
std::string output = "existing content";
JsonWriter{}.write(ast, output);  // Will fail confirm() assertion

// DO this instead - use a fresh string:
std::string output;  // Empty
JsonWriter{}.write(ast, output);  // Correct

// DON'T do this - ast must not be empty:
object_t empty_ast;
std::string output;
JsonWriter{}.write(empty_ast, output);  // Will fail confirm() assertion

// DO this instead - ensure ast has content:
object_t ast;
ast["key"] = std::string("value");
std::string output;
JsonWriter{}.write(ast, output);  // Correct
```

### Output Format Details

**Objects:**
```json
{
"key1" : value1,
"key2" : value2
}
```

**Arrays:**
```json
[
value1,
value2
]
```

**Value Types:**
- Strings: `"text"` (double-quoted)
- Integers: `42` (no decimal point)
- Decimals: `3.14` or `1.5e10` (with decimal or scientific notation)
- Booleans: `true` or `false` (lowercase)

### Performance

- Grammar is instantiated for each generation operation
- Buffer pre-allocates 10000 bytes to minimize reallocations
- Outputs larger than 512 bytes trigger stack trace logging
- For high-throughput scenarios, consider caching the grammar instance
