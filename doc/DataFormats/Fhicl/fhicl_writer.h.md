# fhicl_writer.h

**Path:** `artdaq-database/DataFormats/Fhicl/fhicl_writer.h`

**Purpose:** Defines Boost.Spirit Karma-based grammar templates for generating FHiCL text output, along with the `FhiclWriter` class that converts JSON AST representation back to FHiCL format with preserved comments and annotations.


## Key Concepts

### Boost.Spirit Karma Generator Framework

Boost.Spirit Karma is the output counterpart to Spirit Qi. While Qi parses text into data structures, Karma generates text from data structures. This file defines two grammar templates:

- **fhicl_generator_grammar**: Generates FHiCL syntax from the internal `table_t` AST
- **fhicl_include_generator_grammar**: Generates `#include` directive syntax

### FHiCL Output Format

The generated FHiCL follows standard FHiCL syntax:
- Key-value pairs: `key: value`
- Tables: `name: { ... }`
- Sequences: `name: [ item1, item2 ]`
- Comments: Lines starting with `#`
- Annotations: Trailing text after values (typically `// comment`)

### Round-Trip Conversion

The FhiclWriter complements FhiclReader to enable round-trip conversion:
1. FHiCL text -> JSON AST (FhiclReader)
2. Store/manipulate JSON in database
3. JSON AST -> FHiCL text (FhiclWriter)

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** FhiclWriter instances are not thread-safe; each thread should use its own instance
- **Locking:** None; callers must provide external synchronization if sharing instances

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/Fhicl/fhicl_types.h` | FHiCL AST type definitions (`table_t`, `sequence_t`, `value_t`, `atom_t`) |
| `artdaq-database/DataFormats/Json/json_types.h` | JSON AST types for input (`object_t`) |
| `artdaq-database/DataFormats/common.h` | Common utilities and TRACE macros |
| `<boost/spirit/home/karma/char/char.hpp>` | Karma character generators |
| `<boost/spirit/include/karma.hpp>` | Boost.Spirit Karma generator framework |

## Type Aliases

| Type | Definition | Description |
|------|------------|-------------|
| `include_t` | `std::string` | A single include filename |
| `includes_t` | `std::vector<include_t>` | List of include filenames |

## Classes/Structures

### `fhicl_generator_grammar<Iter>`

**Brief:** Boost.Spirit Karma grammar for generating FHiCL text from the internal `table_t` AST representation.

**Template Parameters:**
- `Iter` - Output iterator type (typically `std::back_insert_iterator<std::string>`)

**Thread Safety:** Not thread-safe; create one instance per generation operation

#### Grammar Rules

| Rule | Return Type | Purpose |
|------|-------------|---------|
| `string` | `std::string()` | Generates a string literal |
| `comment` | `std::string()` | Generates optional comment line with newline |
| `annotation` | `std::string()` | Generates optional trailing annotation with space prefix |
| `variant_value_rule` | `variant_value_t()` | Dispatches to appropriate generator for variant types |
| `annotated_value_rule` | `value_t()` | Generates value followed by optional annotation |
| `commented_key_rule` | `key_t()` | Generates optional comment followed by key |
| `annotated_atom_rule` | `atom_t()` | Generates complete key-value pair with comment/annotation |
| `table_rule` | `table_t()` | Generates nested table with braces |
| `toplevel_table` | `table_t()` | Generates top-level entries without braces |
| `sequence_rule` | `sequence_t()` | Generates sequence with brackets |
| `start` | `table_t()` | Entry point rule (delegates to `toplevel_table`) |

#### Generated FHiCL Syntax

```
# Optional comment
key: value // Optional annotation
nested_table: {
  inner_key: inner_value
}
sequence_key: [ item1, item2, item3 ]
```

#### Usage Example

```cpp
#include "artdaq-database/DataFormats/Fhicl/fhicl_writer.h"
#include <string>
#include <iostream>

using namespace artdaq::database::fhicl;

void generateFhicl(table_t const& ast) {
  std::string output;
  auto sink = std::back_insert_iterator<std::string>(output);

  fhicl_generator_grammar<decltype(sink)> grammar;

  try {
    if (boost::spirit::karma::generate(sink, grammar, ast)) {
      std::cout << "Generated FHiCL:\n" << output << "\n";
    } else {
      std::cerr << "Generation failed\n";
    }
  } catch (std::exception const& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

---

### `fhicl_include_generator_grammar<Iter>`

**Brief:** Boost.Spirit Karma grammar for generating `#include` directives from a list of filenames.

**Template Parameters:**
- `Iter` - Output iterator type

**Thread Safety:** Not thread-safe; create one instance per generation operation

#### Grammar Rules

| Rule | Return Type | Purpose |
|------|-------------|---------|
| `include_rule` | `include_t()` | Generates a single filename |
| `includes_rule` | `includes_t()` | Generates newline-separated `#include` directives |

#### Generated Output Format

```
#include "file1.fcl"
#include "file2.fcl"
```

---

### `FhiclWriter`

**Brief:** High-level class that converts JSON AST representation back to FHiCL text format, handling both prolog and main sections with proper BEGIN_PROLOG/END_PROLOG markers.

**Thread Safety:** Not thread-safe; create one instance per writing operation

#### Methods

##### `write_data(jsn::object_t const& json_object, std::string& out) -> bool`

**Brief:** Converts a JSON object (containing data and metadata nodes) back to FHiCL text format.

**Parameters:**
- `json_object` - Input JSON object with `data` and `metadata` nodes (must not be empty)
- `out` - Output string for generated FHiCL (must be empty on input; populated on success)

**Preconditions:**
- `json_object` must not be empty
- `json_object` must contain `data.prolog`, `data.main`, `metadata.prolog`, `metadata.main` structure
- `out` must be empty

**Returns:** `true` on successful generation, `false` on failure

**Postconditions:**
- On success, `out` contains valid FHiCL text
- Prolog section is wrapped in BEGIN_PROLOG/END_PROLOG if non-empty
- Include directive placeholders are restored to `#include` syntax

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `boost::bad_get` | When JSON structure does not match expected format |

**Thread Safety:** Not thread-safe

**Example:**
```cpp
#include "artdaq-database/DataFormats/Fhicl/fhicl_writer.h"
#include "artdaq-database/DataFormats/Fhicl/fhicl_reader.h"
#include <iostream>

namespace jsn = artdaq::database::json;
using namespace artdaq::database::fhicl;

void roundTripExample() {
  // Original FHiCL
  std::string original_fhicl = R"(
    BEGIN_PROLOG
    default_port: 5000
    END_PROLOG

    server: {
      port: @local::default_port
      host: "localhost"
    }
  )";

  // Parse to JSON
  jsn::object_t json_ast;
  FhiclReader reader;

  try {
    if (!reader.read_data(original_fhicl, json_ast)) {
      std::cerr << "Failed to parse FHiCL\n";
      return;
    }

    // Modify JSON if needed...

    // Generate FHiCL from JSON
    std::string regenerated_fhicl;
    FhiclWriter writer;

    if (writer.write_data(json_ast, regenerated_fhicl)) {
      std::cout << "Regenerated FHiCL:\n" << regenerated_fhicl << "\n";
    } else {
      std::cerr << "Failed to generate FHiCL\n";
    }
  } catch (std::exception const& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

## Functions

### `debug::FhiclWriter() -> void`

**Brief:** Enables TRACE debugging output for the FhiclWriter component.

**Preconditions:** None

**Returns:** Nothing

**Postconditions:**
- TRACE logging is enabled for "fhicl_writer.cpp"
- All TRACE levels are enabled

**Thread Safety:** Not thread-safe (modifies global TRACE state)

**Side Effects:**
- Modifies global TRACE control settings

**Example:**
```cpp
#include "artdaq-database/DataFormats/Fhicl/fhicl_writer.h"

// Enable debugging before generation
artdaq::database::fhicl::debug::FhiclWriter();

// Now all TLOG statements in fhicl_writer.cpp will output
```

## Relationship to Other Components

The FhiclWriter class complements FhiclReader to complete the FHiCL conversion pipeline:

```
FHiCL Text ──[FhiclReader]──> JSON AST ──[Database]──> JSON AST ──[FhiclWriter]──> FHiCL Text
```

The writer:
- Uses `json2fcldb` functor from `convertfhicl2jsondb.h` for element-level conversion
- Generates `BEGIN_PROLOG`/`END_PROLOG` markers for prolog sections
- Restores `#include` directives from placeholder keys
- Trims trailing whitespace from generated lines

## See Also

- [fhicl_writer.cpp](./fhicl_writer.cpp.md) - Implementation details
- [fhicl_reader.h](./fhicl_reader.h.md) - Complementary reader for FHiCL-to-JSON conversion
- [fhicl_types.h](./fhicl_types.h.md) - AST type definitions used by the grammar
- [convertfhicl2jsondb.h](./convertfhicl2jsondb.h.md) - `json2fcldb` functor for JSON-to-FHiCL element conversion
- [External: Boost.Spirit Karma](https://www.boost.org/doc/libs/release/libs/spirit/doc/html/spirit/karma.html) - Karma documentation

## Notes for Developers

### Expected Input JSON Structure

The `write_data` method expects JSON with this structure:

```json
{
  "data": {
    "prolog": { /* key-value pairs for prolog section */ },
    "main": { /* key-value pairs for main section */ }
  },
  "metadata": {
    "prolog": {
      "children": { /* type info for each prolog key */ }
    },
    "main": {
      "children": { /* type info for each main key */ }
    }
  }
}
```

### Common Pitfalls

- **Pitfall 1:** Passing a non-empty `out` string to `write_data()` will trigger an assertion failure. Always start with an empty string.
- **Pitfall 2:** The method expects metadata for each data key; missing metadata will cause `boost::bad_get` exceptions.
- **Pitfall 3:** Include directive placeholders (`fhicl_pound_include_N`) are only partially restored; the original filename may not be preserved.

### Anti-patterns

```cpp
// DON'T do this:
std::string out = "existing content";
writer.write_data(json, out);  // Assertion failure: out not empty

// DO this instead:
std::string out;
if (writer.write_data(json, out)) {
  // Use out
}
```

### Grammar Extension Points

To add support for new FHiCL syntax elements:
1. Add the new type to `fhicl_types.h`
2. Add a new rule to `fhicl_generator_grammar`
3. Update `variant_value_rule` to dispatch to the new rule
4. Update BOOST_FUSION_ADAPT_STRUCT if needed for the new type
