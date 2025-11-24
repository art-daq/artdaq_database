# conf_common.h

## File Overview

Convenience aggregator header providing all CONF DataFormat functionality.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Conf/conf_common.h`

## Dependencies

### Project Headers
- `"artdaq-database/DataFormats/Conf/conf_reader.h"` - CONF parser
- `"artdaq-database/DataFormats/Conf/conf_writer.h"` - CONF generator
- `"artdaq-database/DataFormats/Conf/conf_types.h"` - Type definitions
- `"artdaq-database/DataFormats/common.h"` - Common infrastructure

## Purpose

Single include for:
- CONF parsing (to JSON AST)
- CONF generation (from JSON AST)
- Common DataFormats infrastructure

## Usage

```cpp
#include "artdaq-database/DataFormats/Conf/conf_common.h"

std::string conf = "key: value\n";
json::object_t ast;

conf::ConfReader{}.read(conf, ast);
```

## Related Files

- **conf_reader.h** - Parser
- **conf_writer.h** - Generator
- **json_types.h** - AST types
