# Conf Submodule Documentation

## Overview

The Conf submodule provides support for simple configuration file format, with conversion to/from JSON AST for unified processing.

## Files

### Core Types
- **[conf_types.h](conf_types.h.md)** - Minimal type definitions (reuses JSON AST)

### Parsing and Generation
- **[conf_reader.h](conf_reader.h.md)** - ConfReader class declaration
- **[conf_reader.cpp](conf_reader.cpp.md)** - CONF to JSON AST parser
- **[conf_writer.h](conf_writer.h.md)** - ConfWriter class declaration
- **[conf_writer.cpp](conf_writer.cpp.md)** - JSON AST to CONF generator

### Conversion Utilities
- **[convertconf2json.h](convertconf2json.h.md)** - CONF ↔ JSON string conversion functions
- **[convertconf2json.cpp](convertconf2json.cpp.md)** - Conversion implementation
- **[confjsondb.h](confjsondb.h.md)** - CONF ↔ database JSON conversion
- **[confjsondb.cpp](confjsondb.cpp.md)** - Database conversion implementation

### Convenience
- **[conf_common.h](conf_common.h.md)** - Aggregator header for all CONF functionality

## Design

CONF module reuses JSON AST types for efficiency and enables seamless conversion to other formats.

## CONF Format

Simple key-value format:
```
host: localhost
port: 8080
enabled: true
```

## Usage Example

```cpp
std::string conf = "host: localhost\nport: 8080\n";
std::string json;
conf_to_json(conf, json);
```

## Related Files

- [../Json/json_types.h.md](../Json/json_types.h.md) - AST types used by CONF module
