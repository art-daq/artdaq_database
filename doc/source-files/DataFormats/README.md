# DataFormats Module Documentation

## Overview

The DataFormats module provides a comprehensive framework for handling multiple configuration and data formats used in the artdaq-database system. It supports JSON, XML, CONF, and FHiCL (Fermilab Hierarchical Configuration Language) formats with bidirectional conversion capabilities and unified internal representation.

## Module Structure

### Root Files

- **[common.h](common.h.md)** - Common includes and boost::variant configuration for all DataFormats
- **[shared_literals.h](shared_literals.h.md)** - String constants shared across all formats (field names, type identifiers)
- **[shared_types.h](shared_types.h.md)** - Template-based type system providing common infrastructure for all formats

### Submodules

#### [Json/](Json/)
JSON (JavaScript Object Notation) format support with Boost.Spirit parsers and generators.

**Key Files**:
- `json_types.h` - JSON AST type definitions (object, array, value)
- `json_reader.h/cpp` - Boost.Spirit Qi parser
- `json_writer.h/cpp` - Boost.Spirit Karma generator
- `convertjson2guijson.h/cpp` - Database ↔ GUI format conversion
- `json_types_impl.h` - Template implementations

#### [Xml/](Xml/)
XML format support using Boost.PropertyTree for parsing/generation.

**Key Files**:
- `xml_types.h` - Type definitions (reuses JSON AST)
- `xml_reader.h/cpp` - XML to JSON AST parser
- `xml_writer.h/cpp` - JSON AST to XML generator
- `convertxml2json.h/cpp` - XML ↔ JSON string conversion
- `xmljsondb.h/cpp` - XML ↔ database JSON conversion

#### [Conf/](Conf/)
CONF (simple configuration) format support.

**Key Files**:
- `conf_types.h` - Type definitions (reuses JSON AST)
- `conf_reader.h/cpp` - CONF to JSON AST parser
- `conf_writer.h/cpp` - JSON AST to CONF generator
- `convertconf2json.h/cpp` - CONF ↔ JSON conversion
- `confjsondb.h/cpp` - CONF ↔ database JSON conversion

#### [Fhicl/](Fhicl/)
FHiCL (Fermilab Hierarchical Configuration Language) format support with metadata preservation.

**Key Files**:
- `fhicl_types.h/cpp` - FHiCL AST with comments and annotations
- `fhicl_reader.h/cpp` - Boost.Spirit Qi parser for FHiCL
- `fhicl_writer.h/cpp` - Boost.Spirit Karma generator for FHiCL
- `convertfhicl2jsondb.h/cpp` - FHiCL ↔ database JSON with metadata
- `fhicljsondb.h/cpp` - High-level FHiCL database interface
- `helper_functions.h/cpp` - FHiCL-specific utilities
- `fhiclcpplib_includes.h` - fhiclcpp library integration

## Architecture

### Type System Hierarchy

```
shared_types.h (base templates)
    ├── json_types.h (simple variant types)
    ├── xml_types.h (reuses JSON types)
    ├── conf_types.h (reuses JSON types)
    └── fhicl_types.h (extended with metadata)
```

### Common Design Patterns

1. **AST Representation**: All formats parse to Abstract Syntax Tree representations
2. **Boost.Spirit**: JSON and FHiCL use Spirit for parsing/generation
3. **Boost.Fusion**: Structures adapted for automatic parser/generator creation
4. **Shared Infrastructure**: Common types, literals, and utilities across all formats

### Conversion Flow

```
User Format (JSON/XML/CONF/FHiCL)
    ↓ Reader
Internal AST (object_t / table_t)
    ↓ Converter
Database JSON Format (with metadata)
    ↓ Database
MongoDB Storage
```

## Key Concepts

### AST (Abstract Syntax Tree)

Each format has an internal tree representation:
- **JSON**: `json::object_t`, `json::array_t`, `json::value_t`
- **XML**: Uses JSON AST (`json::object_t`)
- **CONF**: Uses JSON AST (`json::object_t`)
- **FHiCL**: `fhicl::table_t`, `fhicl::sequence_t`, `fhicl::value_t` (with metadata)

### Metadata Preservation

**FHiCL** supports rich metadata:
- **Comments**: Attached to keys (e.g., `# This is a comment`)
- **Annotations**: Attached to values (e.g., `@default`)

Database JSON format separates:
- **Data**: The actual configuration values
- **Metadata**: Type information, comments, annotations

### Format Interoperability

All formats can convert to/from JSON, enabling:
- XML → JSON → FHiCL
- CONF → JSON → XML
- Any format → Database JSON → Any format

## Usage Examples

### JSON Parsing and Generation

```cpp
#include "artdaq-database/DataFormats/Json/json_common.h"

std::string json_text = R"({"key": "value"})";
json::object_t ast;

// Parse
json::JsonReader{}.read(json_text, ast);

// Generate
std::string output;
json::JsonWriter{}.write(ast, output);
```

### FHiCL with Metadata

```cpp
#include "artdaq-database/DataFormats/Fhicl/fhicl_common.h"

std::string fhicl = R"(
    # Timeout setting
    timeout: 30  # seconds
)";

fhicl::table_t ast;
fhicl::FhiclReader{}.read(fhicl, ast);
// Comments and annotations preserved in AST
```

### Format Conversion

```cpp
// XML to JSON
std::string xml = "<config><host>localhost</host></config>";
std::string json;
xml_to_json(xml, json);

// FHiCL to Database JSON
std::string fhicl = "host: localhost";
std::string db_json;
fhicl_to_jsondb(fhicl, db_json);
```

## Dependencies

### External Libraries

- **Boost.Spirit** - Parser (Qi) and generator (Karma) frameworks
- **Boost.Fusion** - Structure introspection and adaptation
- **Boost.Variant** - Discriminated unions for polymorphic values
- **Boost.Optional** - Optional metadata (comments, annotations)
- **Boost.PropertyTree** - XML parsing/generation (Xml module)

### Internal Dependencies

- **SharedCommon** - Common utilities, TRACE logging, exception handling
- **JsonDocument** - Higher-level document abstractions (used by converters)

## Design Principles

1. **Type Safety**: Template-based design with compile-time type checking
2. **Metadata Preservation**: Comments and annotations maintained through conversions
3. **Format Agnostic**: Unified internal representation for all formats
4. **Extensibility**: Easy to add new formats by implementing Reader/Writer
5. **Performance**: Efficient parsing with Boost.Spirit, minimal copying
6. **Interoperability**: Seamless conversion between formats

## Testing and Debugging

### TRACE Logging

All parsers and generators use TRACE for debugging:
```cpp
TLOG(level) << "Message";
```

Enable TRACE with environment variables:
```bash
export TRACE_FILE=/tmp/trace.log
export TRACE_LVLS=ALL,10
```

### Comparison Utilities

JSON types support detailed comparison:
```cpp
auto [equal, message] = obj1 == obj2;
if (!equal) {
    std::cout << "Difference: " << message << "\n";
}
```

### Debug Namespaces

Most modules provide debug utilities:
```cpp
json::debug::JSON2GUIJSON();
xml::debug::XmlReader();
```

## Performance Considerations

1. **Grammar Instantiation**: Parsers/generators instantiate grammar for each operation
   - Could be optimized with static grammars

2. **Memory Allocation**: Writers pre-allocate buffers (typically 10000 bytes)
   - Reduces reallocations for typical documents

3. **TRACE Overhead**: Stack traces logged for large inputs (>512 bytes)
   - Helps identify performance bottlenecks

4. **AST Structure**: Uses `std::list` internally for stable iterators
   - Trade-off: slower random access, better insertion/deletion

## Best Practices

1. **Include Strategy**:
   - Use `*_common.h` in .cpp files for complete functionality
   - Use specific headers in other headers to minimize dependencies

2. **Error Handling**:
   - Check return values (readers/writers return bool)
   - Use TRACE for debugging
   - Comparison operators provide detailed error messages

3. **Type Safety**:
   - Use `boost::get<T>()` with pointer syntax for safe variant access
   - Leverage static type checking with templates

4. **Metadata**:
   - FHiCL: Use comments and annotations liberally
   - Conversions preserve metadata through database JSON format

## Related Documentation

- **[Json/README.md](Json/README.md)** - JSON module details
- **[Xml/README.md](Xml/README.md)** - XML module details
- **[Conf/README.md](Conf/README.md)** - CONF module details
- **[Fhicl/README.md](Fhicl/README.md)** - FHiCL module details
- **SharedCommon documentation** - Common utilities
- **JsonDocument documentation** - Document abstractions

## Contributing

When adding support for a new format:

1. Create subdirectory under DataFormats/
2. Define types (may reuse JSON AST or create custom)
3. Implement Reader and Writer classes
4. Add conversion functions to/from JSON
5. Add database JSON conversion if needed
6. Document all files following established patterns
7. Add tests and examples

## Notes

- JSON is the pivot format - all conversions go through JSON
- FHiCL is the richest format with full metadata support
- XML and CONF are simpler, reuse JSON AST for efficiency
- Database JSON format is the canonical storage representation
- Boost.Spirit grammars are declarative and self-documenting
