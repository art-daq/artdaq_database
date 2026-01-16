# fhicl_reader.cpp

**Path:** `artdaq-database/DataFormats/Fhicl/fhicl_reader.cpp`

**Implements:** [fhicl_reader.h](./fhicl_reader.h.md)

**Purpose:** Implements the FhiclReader class methods for parsing FHiCL documents into JSON AST representation. This file contains the core logic for converting FHiCL text (including prolog sections, comments, and include directives) into a structured JSON format suitable for database storage.

## Implementation Overview

The implementation uses a multi-stage parsing approach:

1. **Comment Extraction**: Parse comments using Boost.Spirit Qi grammar to preserve documentation
2. **Include Directive Transformation**: Replace `#include` directives with placeholder keys to prevent parsing errors
3. **FHiCL Parsing**: Use fhiclcpp library to parse the transformed document into an intermediate table
4. **Prolog/Main Separation**: Iterate through parsed elements, separating prolog and main section content
5. **JSON Conversion**: Convert each FHiCL element to JSON format with accompanying metadata

## Key Algorithms

### FHiCL to JSON Conversion (`read_data`)

**Steps:**
1. Validate input preconditions (non-empty input, empty output)
2. Parse comments from the original text using `fhicl_comments_parser_grammar`
3. Transform `#include` directives to placeholder keys using regex substitution
4. Enable snippet mode for partial document support
5. Parse the transformed FHiCL using `::fhicl::parse_document()`
6. Create output JSON structure with `data` and `metadata` root nodes
7. Process prolog section: iterate elements where `in_prolog == true`
8. Process main section: iterate elements where `in_prolog == false`
9. For each element, call `fcl2jsondb` to generate data and metadata pairs

**Why this approach:**
- Separating comment extraction from data parsing allows comment preservation without modifying the fhiclcpp parser
- Include directive transformation is necessary because fhiclcpp attempts to resolve includes, which may not be available
- Prolog/main separation is essential for configuration management (prolog defines reusable values, main uses them)

### Comment Extraction (`read_comments`)

**Steps:**
1. Validate input preconditions
2. Create position-tracking iterator wrapper
3. Parse using `fhicl_comments_parser_grammar`
4. If no comments found, add placeholder "No comments"
5. Convert each comment to JSON object with line number and escaped value
6. Validate JSON string round-trip conversion (log warnings for non-reversible conversions)

**Why this approach:** Comments are parsed separately to avoid interference with FHiCL syntax parsing. The line number tracking enables comment reassociation during FHiCL regeneration.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/common.h` | Common utilities, `confirm()` macro, TRACE logging |
| `artdaq-database/DataFormats/Fhicl/convertfhicl2jsondb.h` | `fcl2jsondb` functor for FHiCL-to-JSON element conversion |
| `artdaq-database/DataFormats/Fhicl/fhicl_reader.h` | Class declarations and grammar templates |
| `artdaq-database/DataFormats/Fhicl/fhicl_types.h` | FHiCL AST type definitions |
| `artdaq-database/DataFormats/Fhicl/fhiclcpplib_includes.h` | fhiclcpp library functions |
| `artdaq-database/DataFormats/Fhicl/helper_functions.h` | `to_json_string()`, `from_json_string()` utilities |
| `artdaq-database/DataFormats/Json/json_types.h` | JSON AST types |

## Internal Functions

### `make_SubNode` Lambda

**Brief:** Creates a child JSON object node within a parent object and returns a reference to it.

**Called by:** `read_data()` for creating nested structure

**Purpose:** Simplifies creation of the nested `data.prolog`, `data.main`, `metadata.prolog.children`, etc. structure.

```cpp
auto make_SubNode = [](auto& parent, auto const& child_name) -> auto& {
  parent[child_name] = jsn::object_t();
  return boost::get<jsn::object_t>(parent[child_name]);
};
```

## TRACE Configuration

```cpp
#define TRACE_NAME "fhicl_reader.cpp"
```

### TRACE Levels Used

| Level | Description |
|-------|-------------|
| 20-32 | `read_data()` flow: begin, comment parsing, regex, parse_document, prolog/main conversion |
| 41-49 | `read_comments()` flow: begin, phrase_parse, comment integration |
| 50 | Debug enable confirmation |

## Performance Considerations

- **Memory pre-allocation:** The input string is copied and reserve() is called to avoid reallocations during regex replacement
- **Regex compilation:** The `#include` regex is compiled once per call; for high-volume parsing, consider caching
- **Iteration approach:** Uses range-based for loops which are efficient for the underlying list containers
- **No caching:** Comments are re-parsed even when `read_data()` also needs them; consider refactoring if both are frequently needed together

## Error Handling Strategy

1. **Precondition validation:** Uses `confirm()` macro to assert input/output contract requirements
2. **Comment parse failure:** Gracefully handled by inserting placeholder "No comments"
3. **FHiCL parse errors:** `::fhicl::exception` is caught, logged to stderr, and re-thrown for caller handling
4. **Non-reversible string conversion:** Logged as warning via TRACE but does not fail the operation

## Testing Notes

- **Unit tests:** `test/DataFormats/Fhicl_t.cc`
- **Key test cases:**
  - Round-trip: FHiCL -> JSON -> FHiCL preserves content
  - Prolog sections parsed correctly
  - Comments preserved with line numbers
  - Include directives converted to placeholders
  - Various FHiCL value types (strings, numbers, booleans, tables, sequences)

## Maintenance Notes

### Include Directive Handling

The current implementation replaces `#include` with placeholder keys:
```cpp
auto regex = std::regex{"(#include\\s)([^'\"]*)"};
// Replaces: #include "file.fcl"
// With:     fhicl_pound_include_0:
```

This is a workaround because:
1. fhiclcpp tries to resolve includes, which may fail if files aren't available
2. Include information needs to be preserved for regeneration
3. The placeholder format allows round-trip conversion

### Snippet Mode

The call to `::shims::isSnippetMode(true)` is essential. Without it:
- Partial FHiCL documents without required top-level structure will fail to parse
- The mode is set globally, which could affect other parsing operations in the same process

### Comment JSON Encoding

Comments are JSON-encoded using `to_json_string()` and decoded using `from_json_string()`. The round-trip validation logs warnings for comments that cannot be perfectly preserved (e.g., certain escape sequences).

## See Also

- [fhicl_reader.h](./fhicl_reader.h.md) - Class and grammar declarations
- [fhicl_writer.cpp](./fhicl_writer.cpp.md) - Complementary JSON-to-FHiCL implementation
- [convertfhicl2jsondb.h](./convertfhicl2jsondb.h.md) - `fcl2jsondb` functor documentation
- [helper_functions.h](./helper_functions.h.md) - String conversion utilities
