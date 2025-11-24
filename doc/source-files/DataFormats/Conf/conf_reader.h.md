# conf_reader.h

## File Overview

Defines the ConfReader class for parsing CONF (configuration) format into JSON AST representation.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Conf/conf_reader.h`

## Dependencies

### Project Headers
- `"artdaq-database/DataFormats/common.h"` - Common includes
- `"artdaq-database/DataFormats/Json/json_types.h"` - Target AST types
- `"artdaq-database/DataFormats/Conf/conf_types.h"` - CONF type definitions

## ConfReader Class

```cpp
struct ConfReader final {
    bool read(std::string const&, jsn::object_t&);
};
```

**Method**: `read(conf_string, ast)`

**Parameters**:
- `conf_string` - CONF format text
- `ast` - Output JSON object AST (must be empty)

**Returns**: `true` on success, `false` on failure

**Behavior**: Parses CONF format (key-value configuration) into JSON object AST.

## Debug Utilities

```cpp
namespace debug {
    void ConfReader();
}
```

## Usage Example

```cpp
std::string conf = "host: localhost\nport: 8080\n";
json::object_t ast;

if (conf::ConfReader{}.read(conf, ast)) {
    // ast contains parsed configuration
}
```

## Related Files

- **conf_reader.cpp** - Implementation
- **conf_writer.h** - Complementary writer
- **json_types.h** - AST types
