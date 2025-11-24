# convertjson2guijson.cpp

## File Overview

Implementation of bidirectional conversion between database JSON and GUI JSON formats. Provides sophisticated merging and splitting of data and metadata sections.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Json/convertjson2guijson.cpp`

## Dependencies

### Project Headers
- `"artdaq-database/DataFormats/Json/convertjson2guijson.h"` - Declarations
- `"artdaq-database/DataFormats/Json/json_reader.h"` - JSON parsing
- `"artdaq-database/DataFormats/Json/json_writer.h"` - JSON generation
- `"artdaq-database/DataFormats/Json/json_types_impl.h"` - Type utilities
- `"artdaq-database/SharedCommon/configuraion_api_literals.h"` - Constants

## TRACE Configuration

```cpp
#define TRACE_NAME "convertjson2guijson.cpp"
```

## Key Implementations

### json_db_to_gui

```cpp
bool json_db_to_gui(std::string const& db_json, std::string& gui_json)
```

**Algorithm**:
1. Parse database JSON
2. Extract document.data and document.metadata
3. Create db2gui converter
4. Apply conversion
5. Rename result to "guidata"
6. Preserve changelog if present
7. Generate output JSON

**Error Handling**:
- Throws runtime_error on parse failure
- Validates input/output preconditions

### json_gui_to_db

```cpp
bool json_gui_to_db(std::string const& gui_json, std::string& db_json)
```

**Algorithm**:
1. Parse GUI JSON
2. Create separate data and metadata ASTs
3. Create gui2db converter
4. Apply conversion
5. Build document structure
6. Generate output JSON

## Helper Methods

### metadata_node_t::_string_value_of

Extracts string value from metadata node at specified key.

### metadata_node_t::child

Retrieves child metadata node, checking both direct children and "children" section.

### metadata_node_t::hasMetadata

Checks if metadata node is valid (not NOTSET type).

## Design Notes

**Format Compatibility**:
- Handles both flat and nested metadata structures
- Preserves changelog during conversion
- Supports FHiCL-style duplicate keys (per comment in code)

**Error Messages**:
- Detailed TRACE logging at various levels
- Runtime errors for parse failures

## Related Files

- **convertjson2guijson.h** - Declarations
- **json_reader/writer** - JSON I/O
- **json_types_impl.h** - Unwrapper implementations
