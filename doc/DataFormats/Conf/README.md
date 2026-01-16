# Conf Submodule Documentation

## Overview

The Conf submodule provides support for simple configuration file format parsing and generation, with bidirectional conversion to JSON AST for unified processing within the DataFormats framework. This module enables CONF files to be stored in the database and converted to other formats (JSON, XML, FHiCL).

**Note**: This module is currently a stub implementation with the infrastructure in place for future development.

**Location:** `artdaq-database/DataFormats/Conf/`

## Architecture

### Design Decisions

The CONF module makes several key design choices:

1. **Type Reuse**: Uses JSON AST types (`json::object_t`) directly instead of defining custom types
2. **Minimal Implementation**: Provides framework structure with TODO markers for future development
3. **Format Agnostic**: Shares internal representation with JSON and XML modules

### Component Diagram

```
CONF Text Input
    |
    v
ConfReader::read()
    |
    v
json::object_t (AST)
    |
    +--> conf_to_json() --> Database JSON (with metadata)
    |
    +--> JsonWriter --> JSON Text
    |
    +--> XmlWriter --> XML Text
    |
    +--> FhiclWriter --> FHiCL Text
```

## Files

### Core Types
- **[conf_types.h](conf_types.h.md)** - Minimal type definitions (reuses JSON AST)

### Aggregator
- **[conf_common.h](conf_common.h.md)** - Convenience header for all CONF functionality

### Parsing and Generation
- **[conf_reader.h](conf_reader.h.md)** - ConfReader class declaration
- **[conf_reader.cpp](conf_reader.cpp.md)** - CONF to JSON AST parser (stub)
- **[conf_writer.h](conf_writer.h.md)** - ConfWriter class declaration
- **[conf_writer.cpp](conf_writer.cpp.md)** - JSON AST to CONF generator (stub)

### Conversion Utilities
- **[convertconf2json.h](convertconf2json.h.md)** - Low-level converter structures (stub)
- **[convertconf2json.cpp](convertconf2json.cpp.md)** - Converter implementation (stub)
- **[confjsondb.h](confjsondb.h.md)** - High-level CONF/JSON conversion API
- **[confjsondb.cpp](confjsondb.cpp.md)** - Database conversion implementation

## CONF Format

The CONF format is a simple key-value configuration format:

```
# Comment line
host: localhost
port: 8080
timeout: 30

database:
  name: mydb
  user: admin
```

### Format Characteristics
- Key-value pairs separated by colon
- Hierarchical structure through indentation
- Comment lines starting with `#`
- Simple, human-readable syntax

## API Reference

### High-Level Functions

```cpp
#include "artdaq-database/DataFormats/Conf/confjsondb.h"

using namespace artdaq::database;

// Convert CONF to database JSON
std::string conf = "host: localhost\n";
std::string json;
bool success = confjson::conf_to_json(conf, json);

// Convert database JSON to CONF
std::string restored_conf;
success = confjson::json_to_conf(json, restored_conf);
```

### Low-Level Classes

```cpp
#include "artdaq-database/DataFormats/Conf/conf_common.h"

using namespace artdaq::database;

// Parse CONF to AST
conf::ConfReader reader;
json::object_t ast;
bool success = reader.read(conf_text, ast);

// Generate CONF from AST
conf::ConfWriter writer;
std::string conf_output;
success = writer.write(ast, conf_output);
```

## Usage Examples

### Basic Conversion

```cpp
#include "artdaq-database/DataFormats/Conf/confjsondb.h"

using namespace artdaq::database;

void convertConfig() {
  std::string conf = "host: localhost\nport: 8080\n";
  std::string json;

  if (confjson::conf_to_json(conf, json)) {
    std::cout << "Converted to JSON: " << json << std::endl;
  } else {
    std::cerr << "Conversion failed" << std::endl;
  }
}
```

### Round-Trip Conversion

```cpp
#include "artdaq-database/DataFormats/Conf/conf_common.h"

using namespace artdaq::database;

void roundTrip() {
  // Parse CONF
  std::string conf_input = "key: value\n";
  json::object_t ast;
  conf::ConfReader{}.read(conf_input, ast);

  // Modify AST...

  // Generate CONF
  std::string conf_output;
  conf::ConfWriter{}.write(ast, conf_output);
}
```

## Implementation Status

| Component | Status | Notes |
|-----------|--------|-------|
| conf_types.h | Complete | Reuses JSON types |
| conf_common.h | Complete | Aggregator header |
| conf_reader.h | Complete | Interface defined |
| conf_reader.cpp | Stub | TODO: parsing logic |
| conf_writer.h | Complete | Interface defined |
| conf_writer.cpp | Stub | TODO: generation logic |
| convertconf2json.h | Stub | Empty structures |
| convertconf2json.cpp | Stub | Debug only |
| confjsondb.h | Complete | API defined |
| confjsondb.cpp | Partial | Uses reader/writer stubs |

## Thread Safety

All classes and functions in this module are thread-safe:
- **ConfReader**: Stateless, can be used concurrently
- **ConfWriter**: Stateless, can be used concurrently
- **conf_to_json/json_to_conf**: Stateless free functions

## Related Documentation

- [../Json/README.md](../Json/README.md) - JSON module (provides AST types)
- [../Xml/README.md](../Xml/README.md) - XML module (similar architecture)
- [../Fhicl/README.md](../Fhicl/README.md) - FHiCL module (reference implementation)
- [../README.md](../README.md) - DataFormats module overview

## Debugging

Enable TRACE logging for CONF operations:

```cpp
#include "artdaq-database/DataFormats/Conf/confjsondb.h"

using namespace artdaq::database;

// Enable debugging
confjson::debug::enableConfJson();
conf::debug::ConfReader();
conf::debug::ConfWriter();
```

## Contributing

When implementing the CONF parsing/generation:

1. Reference the FHiCL module for implementation patterns
2. Use `conf2json` and `json2conf` structures for node conversion
3. Maintain type information in metadata (like FHiCL does)
4. Add comprehensive TRACE logging
5. Handle errors with appropriate exceptions
6. Update documentation as features are completed

---

**Last Updated:** 2026-01-14
