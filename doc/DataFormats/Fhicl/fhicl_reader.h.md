# fhicl_reader.h

**Path:** `artdaq-database/DataFormats/Fhicl/fhicl_reader.h`

**Purpose:** Defines Boost.Spirit Qi-based grammar templates for parsing FHiCL comments and include directives, along with the `FhiclReader` class that orchestrates FHiCL document parsing and conversion to JSON AST format.


## Key Concepts

### Boost.Spirit Qi Parser Framework

Boost.Spirit Qi is a parser generator framework that allows you to define grammars directly in C++ using expression templates. This file defines two grammar templates:

- **fhicl_comments_parser_grammar**: Extracts comments (lines starting with `#` or `//`) from FHiCL text while tracking line numbers
- **fhicl_includes_parser_grammar**: Extracts `#include` directive filenames from FHiCL text

### Line Position Iterator

The `pos_iterator_t` type wraps a standard iterator to track line numbers during parsing. This enables associating comments with specific line numbers for metadata preservation.

### Comment Preservation

FHiCL comments are preserved during parsing to maintain documentation within configuration files. Comments are stored in a map keyed by line number, allowing them to be reattached during FHiCL regeneration.

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** FhiclReader instances are not thread-safe; each thread should use its own instance
- **Locking:** None; callers must provide external synchronization if sharing instances

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/Fhicl/fhicl_types.h` | FHiCL AST type definitions (`table_t`, `sequence_t`, `value_t`) |
| `artdaq-database/DataFormats/Json/json_types.h` | JSON AST types for output (`object_t`, `array_t`) |
| `artdaq-database/DataFormats/common.h` | Common utilities and TRACE macros |
| `<boost/phoenix/*.hpp>` | Phoenix library for semantic actions in Spirit grammars |
| `<boost/spirit/include/qi.hpp>` | Boost.Spirit Qi parser framework |
| `<boost/spirit/include/support_line_pos_iterator.hpp>` | Line-tracking iterator wrapper |

## Type Aliases

| Type | Definition | Description |
|------|------------|-------------|
| `comment_t` | `std::string` | A single comment string |
| `linenum_t` | `integer` | Line number type (int64_t) |
| `comments_t` | `std::map<linenum_t, comment_t>` | Map of line numbers to comments |
| `linenum_comment_t` | `std::pair<linenum_t, comment_t>` | A line number and comment pair |
| `include_t` | `std::string` | An include filename |
| `includes_t` | `std::vector<include_t>` | List of include filenames |
| `pos_iterator_t` | `line_pos_iterator<string::const_iterator>` | Line-tracking iterator |

## Classes/Structures

### `get_line_f`

**Brief:** Phoenix-compatible functor that extracts line numbers from position iterators.

**Thread Safety:** Thread-safe (stateless)

This functor is used within Spirit grammar semantic actions to capture the current line number when a comment is parsed.

#### Methods

##### `operator()(Iter const& pos_iter) -> size_t`

**Brief:** Returns the current line number from a position iterator.

**Parameters:**
- `pos_iter` - A line position iterator

**Returns:** The current line number (1-indexed)

---

### `fhicl_comments_parser_grammar<Iter>`

**Brief:** Boost.Spirit Qi grammar for extracting comments from FHiCL text while preserving line numbers.

**Template Parameters:**
- `Iter` - Iterator type (typically `pos_iterator_t`)

**Thread Safety:** Not thread-safe; create one instance per parsing operation

#### Grammar Rules

| Rule | Purpose |
|------|---------|
| `whitespace_rule` | Matches non-comment whitespace |
| `whitespace_b4quotedstring_rule` | Matches whitespace before quoted strings, avoiding comment markers |
| `double_quoted_string_rule` | Matches double-quoted string content |
| `single_quoted_string_rule` | Matches single-quoted string content |
| `padded_quoted_string_rule` | Matches quoted strings with surrounding whitespace |
| `string_rule` | Matches comment text until end of line |
| `comment_rule` | Matches a comment and captures line number |
| `comments_rule` | Top-level rule matching all comments in document |

#### Usage Example

```cpp
#include "artdaq-database/DataFormats/Fhicl/fhicl_reader.h"
#include <iostream>

using namespace artdaq::database::fhicl;

void parseComments(std::string const& fhicl_text) {
  try {
    fhicl_comments_parser_grammar<pos_iterator_t> grammar;

    auto start = pos_iterator_t(fhicl_text.begin());
    auto end = pos_iterator_t(fhicl_text.end());

    comments_t comments;

    bool success = boost::spirit::qi::phrase_parse(
      start, end, grammar, boost::spirit::qi::blank, comments
    );

    if (success) {
      for (auto const& [line, comment] : comments) {
        std::cout << "Line " << line << ": " << comment << "\n";
      }
    }
  } catch (std::exception const& e) {
    std::cerr << "Parse error: " << e.what() << "\n";
  }
}
```

---

### `fhicl_includes_parser_grammar<Iter>`

**Brief:** Boost.Spirit Qi grammar for extracting `#include` directive filenames from FHiCL text.

**Template Parameters:**
- `Iter` - Iterator type

**Thread Safety:** Not thread-safe; create one instance per parsing operation

#### Grammar Rules

| Rule | Purpose |
|------|---------|
| `include_rule` | Matches `#include` keyword with surrounding whitespace |
| `filename_rule` | Matches quoted filename after `#include` |
| `includes_rule` | Top-level rule matching all includes |

---

### `FhiclReader`

**Brief:** High-level class that orchestrates FHiCL document parsing, converting FHiCL text to a JSON AST representation with preserved comments and metadata.

**Thread Safety:** Not thread-safe; create one instance per parsing operation

#### Methods

##### `read_data(std::string const& fhicl_text, jsn::object_t& json_object) -> bool`

**Brief:** Parses FHiCL text and converts it to a JSON object containing both data and metadata nodes.

**Parameters:**
- `fhicl_text` - The FHiCL document text to parse (must not be empty)
- `json_object` - Output JSON object (must be empty on input; populated on success)

**Preconditions:**
- `fhicl_text` must not be empty
- `json_object` must be empty

**Returns:** `true` on successful parsing, `false` on failure

**Postconditions:**
- On success, `json_object` contains `data` and `metadata` nodes with prolog and main sections

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `::fhicl::exception` | When FHiCL parsing fails due to syntax errors |

**Thread Safety:** Not thread-safe

**Side Effects:**
- Sets `::shims::isSnippetMode(true)` to enable partial document parsing

**Example:**
```cpp
#include "artdaq-database/DataFormats/Fhicl/fhicl_reader.h"
#include <iostream>

using namespace artdaq::database::fhicl;
namespace jsn = artdaq::database::json;

void readFhiclDocument() {
  std::string fhicl_text = R"(
    BEGIN_PROLOG
    default_timeout: 30
    END_PROLOG

    # Main configuration
    detector: {
      name: "muon"
      threshold: 100
    }
  )";

  jsn::object_t json_result;
  FhiclReader reader;

  try {
    if (reader.read_data(fhicl_text, json_result)) {
      // json_result now contains:
      // - data.prolog: { default_timeout: 30 }
      // - data.main: { detector: { name: "muon", threshold: 100 } }
      // - metadata.prolog: type info for prolog values
      // - metadata.main: type info for main values
      std::cout << "Successfully parsed FHiCL document\n";
    } else {
      std::cerr << "Failed to parse FHiCL document\n";
    }
  } catch (::fhicl::exception const& e) {
    std::cerr << "FHiCL parse error: " << e.what() << "\n";
  }
}
```

---

##### `read_comments(std::string const& fhicl_text, jsn::array_t& json_array) -> bool`

**Brief:** Extracts all comments from FHiCL text and stores them as a JSON array with line number information.

**Parameters:**
- `fhicl_text` - The FHiCL document text to parse (must not be empty)
- `json_array` - Output JSON array (must be empty on input; populated on success)

**Preconditions:**
- `fhicl_text` must not be empty
- `json_array` must be empty

**Returns:** `true` on successful extraction (always returns true; missing comments result in a placeholder)

**Postconditions:**
- On success, `json_array` contains objects with `linenum` and `value` fields

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `::fhicl::exception` | When an unexpected FHiCL parsing error occurs |

**Thread Safety:** Not thread-safe

**Example:**
```cpp
#include "artdaq-database/DataFormats/Fhicl/fhicl_reader.h"

using namespace artdaq::database::fhicl;
namespace jsn = artdaq::database::json;

void extractComments() {
  std::string fhicl_text = R"(
    # Configuration file
    timeout: 30  // in seconds
  )";

  jsn::array_t comments;
  FhiclReader reader;

  try {
    if (reader.read_comments(fhicl_text, comments)) {
      // comments array contains:
      // [ { linenum: 2, value: "# Configuration file" },
      //   { linenum: 3, value: "// in seconds" } ]
    }
  } catch (::fhicl::exception const& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

## Functions

### `debug::FhiclReader() -> void`

**Brief:** Enables TRACE debugging output for the FhiclReader component.

**Preconditions:** None

**Returns:** Nothing

**Postconditions:**
- TRACE logging is enabled for "fhicl_reader.cpp"
- All TRACE levels are enabled

**Thread Safety:** Not thread-safe (modifies global TRACE state)

**Side Effects:**
- Modifies global TRACE control settings

**Example:**
```cpp
#include "artdaq-database/DataFormats/Fhicl/fhicl_reader.h"

// Enable debugging before parsing
artdaq::database::fhicl::debug::FhiclReader();

// Now all TLOG statements in fhicl_reader.cpp will output
```

## Relationship to Other Components

The FhiclReader class is a core component of the FHiCL-to-JSON conversion pipeline:

1. **Input**: Raw FHiCL text from files or user input
2. **Processing**: Uses fhiclcpp library via `fhiclcpplib_includes.h` for parsing
3. **Conversion**: Uses `convertfhicl2jsondb.h` functions to convert parsed FHiCL to JSON
4. **Output**: JSON AST suitable for storage in the configuration database

The reader preserves:
- Prolog/main section separation
- Comments associated with line numbers
- Type metadata for each value
- `#include` directive handling (converted to placeholder keys)

## See Also

- [fhicl_reader.cpp](./fhicl_reader.cpp.md) - Implementation details
- [fhicl_writer.h](./fhicl_writer.h.md) - Complementary writer for JSON-to-FHiCL conversion
- [fhicl_types.h](./fhicl_types.h.md) - AST type definitions
- [convertfhicl2jsondb.h](./convertfhicl2jsondb.h.md) - Core FHiCL-JSON conversion logic
- [fhicljsondb.h](./fhicljsondb.h.md) - High-level conversion API

## Notes for Developers

### Output JSON Structure

The `read_data` method produces a JSON object with this structure:

```json
{
  "data": {
    "prolog": { /* key-value pairs from BEGIN_PROLOG section */ },
    "main": { /* key-value pairs from main section */ }
  },
  "metadata": {
    "prolog": {
      "type": "table",
      "comment": "#prolog",
      "children": { /* type info for each prolog key */ }
    },
    "main": {
      "type": "table",
      "comment": "#main",
      "children": { /* type info for each main key */ }
    }
  }
}
```

### Common Pitfalls

- **Pitfall 1:** Passing a non-empty `json_object` to `read_data()` will trigger an assertion failure. Always start with an empty object.
- **Pitfall 2:** The `#include` directives are not resolved; they are converted to placeholder keys like `fhicl_pound_include_0`. The caller must handle include resolution.
- **Pitfall 3:** Comments are extracted separately from data parsing; if you need both, call both `read_data()` and `read_comments()`.

### Anti-patterns

```cpp
// DON'T do this:
jsn::object_t obj;
obj["existing"] = "data";
reader.read_data(fhicl, obj);  // Assertion failure: obj not empty

// DO this instead:
jsn::object_t obj;
if (reader.read_data(fhicl, obj)) {
  // Use obj
}
```
