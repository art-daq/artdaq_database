# json_common.h

## File Overview

This is a convenience aggregator header that provides a single include point for all common JSON DataFormat functionality. It bundles the JSON reader, writer, and core DataFormats includes.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Json/json_common.h`

## Dependencies

### Project Headers
- `"artdaq-database/DataFormats/Json/json_reader.h"` - JSON parser functionality
- `"artdaq-database/DataFormats/Json/json_writer.h"` - JSON generator functionality
- `"artdaq-database/DataFormats/common.h"` - Common DataFormats headers

## Purpose

This header simplifies inclusion of JSON functionality by providing:
1. **JSON Parsing** - Via json_reader.h (Boost.Spirit Qi parser)
2. **JSON Generation** - Via json_writer.h (Boost.Spirit Karma generator)
3. **Common Infrastructure** - Via common.h (boost::variant configuration, standard libraries)

## Usage

```cpp
#include "artdaq-database/DataFormats/Json/json_common.h"

// Now you have access to:
// - JsonReader class
// - JsonWriter class
// - json::object_t, json::array_t, json::value_t types
// - All common DataFormats infrastructure
```

## Design Rationale

**Benefits**:
- Single include for complete JSON functionality
- Reduces boilerplate in source files
- Ensures consistent inclusion of related headers

**Use Cases**:
- Converting between JSON text and AST
- Reading and writing JSON documents
- Working with JSON data structures

## Related Files

- **json_reader.h** - Parser implementation
- **json_writer.h** - Generator implementation
- **json_types.h** - JSON type definitions
- **common.h** - DataFormats common infrastructure

## Best Practices

Include this header in .cpp files that need both reading and writing JSON:

```cpp
#include "artdaq-database/DataFormats/Json/json_common.h"

void processJson(const std::string& input, std::string& output) {
    json::object_t ast;
    json::JsonReader{}.read(input, ast);

    // Process ast...

    json::JsonWriter{}.write(ast, output);
}
```

## Notes

- This is a pure aggregator header with no code
- Includes both parser and generator for bidirectional conversion
- Automatically includes necessary type definitions and common infrastructure
