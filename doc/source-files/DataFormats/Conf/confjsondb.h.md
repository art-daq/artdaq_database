# confjsondb.h

## File Overview

Provides conversion between CONF configuration format and database JSON format.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Conf/confjsondb.h`

## Conversion Functions

```cpp
bool conf_to_jsondb(std::string const& conf, std::string& json_db);
bool jsondb_to_conf(std::string const& json_db, std::string& conf);
```

### conf_to_jsondb

Converts CONF configuration to database JSON format (with metadata).

### jsondb_to_conf

Converts database JSON format to CONF configuration.

## Purpose

Bridges between:
- CONF configuration files (user-facing)
- Database JSON format (storage with metadata)

## Related Files

- **confjsondb.cpp** - Implementation
- **convertconf2json.h** - Basic CONF/JSON conversion
