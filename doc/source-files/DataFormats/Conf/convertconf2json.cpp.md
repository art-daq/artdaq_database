# convertconf2json.cpp

## File Overview

Implementation of bidirectional CONF ↔ JSON string conversion.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Conf/convertconf2json.cpp`

## TRACE Configuration

```cpp
#define TRACE_NAME "convertconf2json.cpp"
```

## Implementation

### conf_to_json

Converts CONF string to JSON via ConfReader and JsonWriter.

### json_to_conf

Converts JSON string to CONF via JsonReader and ConfWriter.

## Related Files

- **convertconf2json.h** - Function declarations
- **conf_reader/writer.h** - CONF I/O
- **json_reader/writer.h** - JSON I/O
