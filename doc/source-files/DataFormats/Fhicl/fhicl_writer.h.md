# fhicl_writer.h

## File Overview

Defines Boost.Spirit Karma-based generator for FHiCL documents, converting table_t AST to FHiCL text format.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Fhicl/fhicl_writer.h`

## Dependencies

### Third-Party Libraries
- `<boost/spirit/include/karma.hpp>` - Boost.Spirit Karma generator

### Project Headers
- `"artdaq-database/DataFormats/Fhicl/fhicl_types.h"` - FHiCL AST types
- `"artdaq-database/DataFormats/common.h"` - Common includes

## Generator Grammar

### fhicl_generator_grammar

```cpp
template <typename Iter>
struct fhicl_generator_grammar : karma::grammar<Iter, table_t()>
```

**Features**:
- Generates FHiCL syntax
- Preserves comments and annotations
- Formats tables and sequences
- Handles quoting appropriately

## FhiclWriter Class

```cpp
struct FhiclWriter final {
    bool write(table_t const&, std::string&);
};
```

**Method**: `write(ast, fhicl_output)`

**Parameters**:
- `ast` - Input table_t AST
- `fhicl_output` - Output FHiCL string

**Returns**: `true` on success

## Usage Example

```cpp
fhicl::table_t ast;
// ... populate ast ...

std::string fhicl;
if (fhicl::FhiclWriter{}.write(ast, fhicl)) {
    std::cout << fhicl;  // FHiCL format output
}
```

## Related Files

- **fhicl_writer.cpp** - Implementation
- **fhicl_reader.h** - Complementary reader
- **fhicl_types.h** - AST types
- **helper_functions.h** - FHiCL formatting helpers
