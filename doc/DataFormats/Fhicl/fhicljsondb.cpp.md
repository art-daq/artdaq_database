# fhicljsondb.cpp

**Path:** `artdaq-database/DataFormats/Fhicl/fhicljsondb.cpp`

**Implements:** [fhicljsondb.h](./fhicljsondb.h.md)

**Purpose:** Implements the high-level FHiCL-JSON conversion functions that serve as the primary public API for converting FHiCL configurations to and from JSON format for database storage. This file orchestrates the conversion pipeline using FhiclReader, FhiclWriter, JsonReader, and JsonWriter components.

## Implementation Overview

The implementation provides three main functions:

1. **fhicl_to_ast**: Internal function that parses FHiCL to JSON AST (object_t)
2. **fhicl_to_json**: Public function that converts FHiCL text to JSON string
3. **json_to_fhicl**: Public function that converts JSON string back to FHiCL text

## Key Algorithms

### FHiCL to AST Conversion (`fhicl_to_ast`)

This internal function creates the complete JSON structure for database storage.

**Steps:**
1. Validate input preconditions (non-empty FHiCL, non-empty filename)
2. Enable snippet mode for partial document support
3. Create root JSON object with all required nodes:
   - `document` (object)
   - `comments` (array)
   - `origin` (object)
   - `version` (string, defaults to "notprovided")
   - `entities` (array)
   - `configurations` (array)
4. Populate origin metadata:
   - `format`: "fhicl"
   - `name`: filename parameter
   - `source`: "fhicl_to_json"
   - `timestamp`: current timestamp
5. Use FhiclReader to extract comments
6. Use FhiclReader to parse document data
7. Swap result into output parameter

**Why this approach:** Creating a complete JSON structure with metadata enables the database to track configuration provenance, versioning, and associations.

### FHiCL to JSON String (`fhicl_to_json`)

**Steps:**
1. Validate input preconditions
2. Call `fhicl_to_ast` to get JSON AST
3. Use JsonWriter to serialize AST to string
4. Swap result into output parameter

**Why this approach:** Separating AST creation from string serialization allows the AST to be manipulated before final output if needed.

### JSON to FHiCL (`json_to_fhicl`)

**Steps:**
1. Validate input preconditions
2. Use JsonReader to parse JSON string into AST
3. Extract `document` node from root
4. Set filename to "notprovided" (origin parsing not implemented)
5. Use FhiclWriter to generate FHiCL text from document
6. Swap result into output parameter

**Why this approach:** The reverse conversion focuses on the document content; metadata like origin and comments are not restored to the FHiCL output.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/common.h` | Common utilities, `confirm()` macro, `timestamp()` function |
| `artdaq-database/DataFormats/shared_literals.h` | String literals for JSON keys (`literal::document`, etc.) |
| `artdaq-database/DataFormats/Fhicl/fhiclcpplib_includes.h` | fhiclcpp library (for snippet mode) |
| `artdaq-database/DataFormats/Fhicl/helper_functions.h` | FHiCL utility functions |
| `artdaq-database/DataFormats/Fhicl/fhicl_common.h` | FhiclReader and FhiclWriter includes |
| `artdaq-database/DataFormats/Fhicl/fhicljsondb.h` | Function declarations |
| `artdaq-database/DataFormats/Json/json_common.h` | JsonReader and JsonWriter classes |

## Internal Functions

### `fhicl_to_ast(std::string const& fcl, std::string const& filename, jsn::object_t& json) -> bool`

**Brief:** Parses FHiCL text and creates a complete JSON AST with document content, comments, and metadata.

**Called by:** `fhicl_to_json()`

**Purpose:** Separates AST creation from string serialization for flexibility.

**Parameters:**
- `fcl` - FHiCL text to parse
- `filename` - Original filename for metadata
- `json` - Output JSON AST

**Returns:** `true` on success, `false` on failure

### `get_object` Lambda (in `fhicl_to_ast`)

**Brief:** Helper to access child objects in the JSON root by key name.

```cpp
auto get_object = [&json_root](std::string const& name) -> auto& {
  return boost::get<jsn::object_t>(json_root[name]);
};
```

## TRACE Configuration

```cpp
#define TRACE_NAME "fhicljsondb.cpp"
```

### TRACE Levels Used

| Level | Description |
|-------|-------------|
| 20-28 | `fhicl_to_ast()` flow: begin, node creation, comments, data, end |
| 30-33 | `json_to_fhicl()` flow: begin, JSON parsing, FHiCL writing |
| 34 | `debug::FhiclJson()` enable confirmation |
| 41-44 | `fhicl_to_json()` flow: begin, write, end |

## Performance Considerations

- **Two-pass parsing:** Comments and data are parsed separately by FhiclReader
- **String copies:** Multiple string copies occur during conversion; consider move semantics for large configurations
- **No caching:** Each conversion is independent; for repeated conversions of the same content, consider caching

## Error Handling Strategy

1. **Precondition validation:** Uses `confirm()` macro for input validation
2. **Comment extraction failure:** Handled gracefully; does not stop conversion
3. **Data parsing failure:** Returns `false`, does not throw
4. **JSON parsing failure:** Returns `false` from JsonReader
5. **FHiCL generation failure:** Returns `false` from FhiclWriter

## Testing Notes

- **Unit tests:** `test/DataFormats/Fhicl_t.cc`
- **Key test cases:**
  - Simple FHiCL configurations
  - Prolog and main sections
  - Comments preservation
  - Round-trip FHiCL -> JSON -> FHiCL
  - Error cases (empty input, malformed FHiCL)

## Maintenance Notes

### Origin Metadata

The `fhicl_to_ast` function populates origin metadata:

```cpp
get_object(literal::origin)[literal::format] = std::string("fhicl");
get_object(literal::origin)[literal::name] = filename;
get_object(literal::origin)[literal::source] = std::string("fhicl_to_json");
get_object(literal::origin)[literal::timestamp] = artdaq::database::timestamp();
```

The `timestamp()` function returns the current time in ISO 8601 format.

### Incomplete json_to_fhicl Implementation

The current implementation has limitations:

1. **Filename not extracted:** The `filename` output is always set to "notprovided"
2. **Comments not restored:** Comments stored in the JSON are not reinserted into the FHiCL output
3. **Origin not read:** The origin metadata is not parsed or returned

These could be enhanced if needed for full round-trip fidelity.

### Version and Association Fields

The `fhicl_to_ast` function creates empty placeholder fields:

```cpp
json_root[literal::version] = std::string{literal::notprovided};
json_root[literal::entities] = jsn::array_t();
json_root[literal::configurations] = jsn::array_t();
```

These are populated by the database layer after storage, not during conversion.

## See Also

- [fhicljsondb.h](./fhicljsondb.h.md) - Function declarations and public API documentation
- [fhicl_reader.cpp](./fhicl_reader.cpp.md) - FHiCL parsing implementation
- [fhicl_writer.cpp](./fhicl_writer.cpp.md) - FHiCL generation implementation
- [json_common.h](../Json/json_common.h.md) - JsonReader and JsonWriter classes
- [shared_literals.h](../shared_literals.h.md) - String literal definitions
