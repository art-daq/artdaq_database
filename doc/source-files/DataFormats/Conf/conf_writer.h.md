# conf_writer.h

## File Overview

Defines the ConfWriter class for generating CONF format from JSON AST representation.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Conf/conf_writer.h`

## Dependencies

### Project Headers
- `"artdaq-database/DataFormats/common.h"` - Common includes
- `"artdaq-database/DataFormats/Json/json_types.h"` - Source AST types
- `"artdaq-database/DataFormats/Conf/conf_types.h"` - CONF type definitions

## ConfWriter Class

```cpp
struct ConfWriter final {
    bool write(jsn::object_t const&, std::string&);
};
```

**Method**: `write(ast, conf_output)`

**Parameters**:
- `ast` - Input JSON object AST (must not be empty)
- `conf_output` - Output CONF format string (must be empty)

**Returns**: `true` on success, `false` on failure

**Behavior**: Converts JSON object AST to CONF key-value format.

## Debug Utilities

```cpp
namespace debug {
    void ConfWriter();
}
```

## Usage Example

```cpp
json::object_t ast;
ast["host"] = std::string("localhost");
ast["port"] = static_cast<integer>(8080);

std::string conf;
if (conf::ConfWriter{}.write(ast, conf)) {
    std::cout << conf;  // CONF format output
}
```

## Related Files

- **conf_writer.cpp** - Implementation
- **conf_reader.h** - Complementary reader
- **json_types.h** - AST types
