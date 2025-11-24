# convertconf2json.h

## File Overview

Provides high-level string-to-string conversion functions between CONF and JSON formats.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Conf/convertconf2json.h`

## Dependencies

### Project Headers
- `"artdaq-database/DataFormats/common.h"` - Common includes

## Conversion Functions

```cpp
namespace artdaq {
namespace database {

bool conf_to_json(std::string const& conf, std::string& json);
bool json_to_conf(std::string const& json, std::string& conf);

}  // namespace database
}  // namespace artdaq
```

### conf_to_json

**Purpose**: Converts CONF format to JSON.

**Parameters**:
- `conf` - Input CONF string
- `json` - Output JSON string (must be empty)

**Returns**: `true` on success

### json_to_conf

**Purpose**: Converts JSON to CONF format.

**Parameters**:
- `json` - Input JSON string
- `conf` - Output CONF string (must be empty)

**Returns**: `true` on success

## Usage Example

```cpp
std::string conf = "host: localhost\nport: 8080\n";
std::string json;

if (conf_to_json(conf, json)) {
    std::cout << json;  // {"host":"localhost","port":8080}
}
```

## Related Files

- **convertconf2json.cpp** - Implementation
- **conf_reader/writer.h** - CONF I/O
- **json_reader/writer.h** - JSON I/O
