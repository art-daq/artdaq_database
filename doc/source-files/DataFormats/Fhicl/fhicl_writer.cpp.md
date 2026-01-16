# fhicl_writer.cpp

**Path:** `artdaq-database/DataFormats/Fhicl/fhicl_writer.cpp`

**Implements:** [fhicl_writer.h](./fhicl_writer.h.md)

**Purpose:** Implements the FhiclWriter class method for converting JSON AST representation back to FHiCL text format. This file handles prolog/main section generation, include directive restoration, and post-processing of generated output.

## Implementation Overview

The implementation follows a structured conversion process:

1. **Input Validation**: Verify preconditions on input JSON and output string
2. **Prolog Section Generation**: Convert prolog data with BEGIN_PROLOG/END_PROLOG markers
3. **Main Section Generation**: Convert main data without special markers
4. **Include Directive Restoration**: Replace placeholder keys with `#include` syntax
5. **Output Cleanup**: Trim trailing whitespace from generated lines

## Key Algorithms

### JSON to FHiCL Conversion (`write_data`)

**Steps:**
1. Validate preconditions using `confirm()` macro
2. Extract `data` and `metadata` nodes from input JSON
3. Create empty output buffer
4. **Process Prolog Section:**
   - Extract prolog data and metadata
   - For each element, call `json2fcldb` to convert to FHiCL AST
   - If prolog is non-empty, prepend "BEGIN_PROLOG\n" and append "\nEND_PROLOG\n"
   - Generate FHiCL text using `fhicl_generator_grammar`
5. **Process Main Section:**
   - Extract main data and metadata
   - For each element, call `json2fcldb` to convert to FHiCL AST
   - Generate FHiCL text using `fhicl_generator_grammar`
6. **Post-process:**
   - Use regex to restore `#include` directives from placeholders
   - Trim trailing whitespace from each line
7. Swap output buffer into result string

**Why this approach:**
- Separating prolog and main sections maintains FHiCL semantic structure
- Using the same grammar for both sections ensures consistent output formatting
- Post-processing allows clean handling of edge cases without complicating the grammar

### Include Directive Restoration

The reader converts `#include` directives to placeholder keys during parsing. This implementation restores them:

```cpp
auto regex = std::regex{"(#include\\s*:)([^\"]*)"};
// Input:  #include: "filename.fcl"
// Output: #include
```

**Note:** The current implementation has a limitation - it only restores the `#include` keyword but may not preserve the original filename. The regex replacement converts `#include:` back to `#include ` but the filename handling depends on the placeholder structure.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/common.h` | Common utilities, `confirm()` macro, TRACE logging |
| `artdaq-database/DataFormats/Fhicl/fhiclcpplib_includes.h` | fhiclcpp library (for consistency, though not directly used) |
| `artdaq-database/DataFormats/Fhicl/convertfhicl2jsondb.h` | `json2fcldb` functor, `extra_opts`, `valuetuple_t` |
| `artdaq-database/DataFormats/Fhicl/fhicl_types.h` | FHiCL AST types (`table_t`, `atom_t`) |
| `artdaq-database/DataFormats/Fhicl/fhicl_writer.h` | Class declaration and grammar template |
| `artdaq-database/DataFormats/Fhicl/helper_functions.h` | `buffer_rtrim_lines()` utility |
| `artdaq-database/DataFormats/Json/json_types.h` | JSON AST types |

## Internal Functions

### `get_SubNode` Lambda

**Brief:** Extracts a child JSON object from a parent object by key name.

**Called by:** `write_data()` for navigating JSON structure

**Purpose:** Provides const-correct access to nested JSON objects with type casting.

```cpp
auto get_SubNode = [](auto& parent, auto const& child_name) -> auto const& {
  return boost::get<jsn::object_t>(parent.at(child_name));
};
```

## TRACE Configuration

```cpp
#define TRACE_NAME "fhicl_writer.cpp"
```

### TRACE Levels Used

| Level | Description |
|-------|-------------|
| 11 | `write_data()` begin |
| 12 | `write_data()` end, debug enable confirmation |

## Performance Considerations

- **Buffer pre-allocation:** `buffer.reserve(buffer.size() + 512)` before regex processing reduces reallocations
- **Grammar instantiation:** The `fhicl_generator_grammar` is instantiated twice (once for prolog, once for main); consider caching if performance-critical
- **Regex processing:** The regex is compiled per call; for high-volume generation, consider static caching
- **String swap:** Uses `swap()` instead of copy assignment for efficient output transfer

## Error Handling Strategy

1. **Precondition validation:** Uses `confirm()` macro to assert:
   - Output string is empty
   - Input JSON object is not empty
2. **Generation failure:** Returns `false` if Karma `generate()` fails
3. **Type mismatch:** Unhandled; relies on `boost::get` throwing `boost::bad_get` for invalid JSON structure

## Testing Notes

- **Unit tests:** `test/DataFormats/Fhicl_t.cc`
- **Key test cases:**
  - Round-trip: JSON -> FHiCL -> JSON preserves content
  - Empty prolog section (no BEGIN_PROLOG markers generated)
  - Non-empty prolog section (markers present)
  - Complex nested tables and sequences
  - Include directive restoration

## Maintenance Notes

### Prolog Section Handling

The prolog section is only wrapped with markers if it contains data:

```cpp
if (static_cast<unsigned int>(!data.empty()) != 0u) {
  buffer.append("BEGIN_PROLOG\n");
  // ... generate content ...
  buffer.append("\nEND_PROLOG\n");
}
```

The cast to `unsigned int` is a safeguard against potential boolean conversion issues, though it could be simplified.

### Include Directive Restoration Limitation

The current regex replacement is incomplete:

```cpp
auto regex = std::regex{"(#include\\s*:)([^\"]*)"};
std::for_each(std::sregex_iterator(...), std::sregex_iterator(),
              [&buffer](auto& m) { buffer.replace(m.position(), m.length(), "#include "); });
```

This replaces `#include:` with `#include ` but discards the captured group `([^\"]*)`. The original filename from the placeholder may need additional handling for complete restoration.

### Line Trimming

The `buffer_rtrim_lines()` function from `helper_functions.h` removes trailing whitespace from each line. This ensures clean output regardless of how the grammar generates whitespace.

## See Also

- [fhicl_writer.h](./fhicl_writer.h.md) - Class declaration and grammar template
- [fhicl_reader.cpp](./fhicl_reader.cpp.md) - Complementary FHiCL-to-JSON implementation
- [convertfhicl2jsondb.h](./convertfhicl2jsondb.h.md) - `json2fcldb` functor documentation
- [helper_functions.h](./helper_functions.h.md) - `buffer_rtrim_lines()` utility
