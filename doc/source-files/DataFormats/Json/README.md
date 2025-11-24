# Json Submodule Documentation

## Overview

The Json submodule provides comprehensive JSON support with Boost.Spirit-based parsing and generation, AST manipulation, and conversion utilities between database and GUI JSON formats.

## Files

### Core Types
- **[json_types.h](json_types.h.md)** - JSON AST type definitions (object_t, array_t, value_t) and visitor patterns
- **[json_types.cpp](json_types.cpp.md)** - Comparison operators and unwrapper specializations
- **[json_types_impl.h](json_types_impl.h.md)** - Template implementations for type access

### Parsing and Generation
- **[json_reader.h](json_reader.h.md)** - Boost.Spirit Qi parser grammar and JsonReader class
- **[json_reader.cpp](json_reader.cpp.md)** - JsonReader implementation
- **[json_writer.h](json_writer.h.md)** - Boost.Spirit Karma generator grammar and JsonWriter class
- **[json_writer.cpp](json_writer.cpp.md)** - JsonWriter implementation

### Conversion Utilities
- **[convertjson2guijson.h](convertjson2guijson.h.md)** - Database ↔ GUI JSON conversion classes
- **[convertjson2guijson.cpp](convertjson2guijson.cpp.md)** - Conversion implementation

### Convenience
- **[json_common.h](json_common.h.md)** - Aggregator header including all JSON functionality

## Key Concepts

### AST Types

```cpp
object_t  // JSON object: map of key-value pairs
array_t   // JSON array: list of values
value_t   // JSON value: variant of object, array, string, number, bool
data_t    // Key-value pair
```

### Database vs GUI Format

**Database Format**:
```json
{
  "document": {
    "data": { "key": "value" },
    "metadata": { "key": { "type": "string" } }
  }
}
```

**GUI Format**:
```json
{
  "guidata": {
    "key": {
      "value": "value",
      "type": "string"
    }
  }
}
```

## Usage Examples

See [main DataFormats README](../README.md) for usage examples.

## Related Files

- [../shared_types.h.md](../shared_types.h.md) - Base type templates
- [../shared_literals.h.md](../shared_literals.h.md) - String constants
