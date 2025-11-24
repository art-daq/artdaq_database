# fhicl_reader.h

## File Overview

Defines Boost.Spirit Qi-based parser for FHiCL documents, converting FHiCL text to internal table_t AST representation.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Fhicl/fhicl_reader.h`

## Dependencies

### Third-Party Libraries
- `<boost/spirit/include/qi.hpp>` - Boost.Spirit Qi parser

### Project Headers
- `"artdaq-database/DataFormats/Fhicl/fhicl_types.h"` - FHiCL AST types
- `"artdaq-database/DataFormats/common.h"` - Common includes

## Parser Grammar

### fhicl_parser_grammar

```cpp
template <typename Iter>
struct fhicl_parser_grammar : qi::grammar<Iter, table_t(), qi::locals<std::string>>
```

**Features**:
- Handles FHiCL syntax (tables, sequences, atoms)
- Supports comments and annotations
- Processes #include directives
- Handles quoted and unquoted strings
- Recognizes numbers, booleans, nil

**Grammar Rules** include:
- `quoted_string` - Strings in quotes
- `unquoted_string` - Bare identifiers
- `atom_rule` - Key-value pairs
- `table_rule` - FHiCL tables { ... }
- `sequence_rule` - FHiCL sequences [ ... ]
- `value_rule` - Any FHiCL value
- `comment_rule` - #-style comments
- `annotation_rule` - Annotations

## FhiclReader Class

```cpp
struct FhiclReader final {
    bool read(std::string const&, table_t&);
};
```

**Method**: `read(fhicl_text, ast)`

**Parameters**:
- `fhicl_text` - FHiCL document
- `ast` - Output table_t AST

**Returns**: `true` on success

## Usage Example

```cpp
std::string fhicl = R"(
    # Configuration
    timeout: 30  # seconds
    servers: [ "host1", "host2" ]
)";

fhicl::table_t ast;
if (fhicl::FhiclReader{}.read(fhicl, ast)) {
    // ast contains parsed structure with comments
}
```

## Related Files

- **fhicl_reader.cpp** - Implementation
- **fhicl_writer.h** - Complementary writer
- **fhicl_types.h** - AST types
- **helper_functions.h** - FHiCL parsing helpers
