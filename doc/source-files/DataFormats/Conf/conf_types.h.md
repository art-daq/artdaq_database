# conf_types.h

## File Overview

Minimal header providing type infrastructure for CONF (configuration) data format. Like XML, CONF parsing/writing uses JSON AST types directly rather than defining its own type system.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Conf/conf_types.h`

## Dependencies

### Project Headers
- `"artdaq-database/DataFormats/common.h"` - Common includes and configuration

## Design

The CONF module reuses JSON type infrastructure:
- Parses CONF format to `json::object_t` AST
- Writes `json::object_t` AST to CONF format
- Shares internal representation with JSON and XML

This allows easy conversion between CONF, JSON, and XML formats.

## Related Files

- **conf_reader.h** - Parses CONF to json::object_t
- **conf_writer.h** - Writes json::object_t to CONF
- **json_types.h** - Actual type definitions used
- **convertconf2json.h** - CONF to JSON conversion
