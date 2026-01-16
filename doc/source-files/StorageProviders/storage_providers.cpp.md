# storage_providers.cpp

**Path:** `artdaq-database/StorageProviders/storage_providers.cpp`

**Implements:** [storage_providers.h](./storage_providers.h.md)

**Purpose:** Implements the `make_database_metadata()` function that creates a JSON-formatted metadata document for newly initialized databases. This metadata includes database name, connection URI, locale information, timestamps, and system details for tracking database provenance and supporting schema migrations.

## Implementation Overview

This file provides a single function that generates structured metadata when a new database is created. The metadata serves as an audit trail and enables future schema migrations by tracking the database format version used at creation time.

## Key Algorithms

### Metadata Generation

The `make_database_metadata()` function constructs a JSON object containing system and creation information.

**Steps:**
1. Validate that name and uri parameters are non-empty using `confirm()` macro
2. Create output stream for JSON construction
3. Write each metadata field with proper JSON formatting using `_quoted` literal and `quoted_()` function
4. Include system information from environment and `unamejson()` helper
5. Return the complete JSON string

**Why this approach:** Direct string construction is used instead of a JSON library to minimize dependencies and because the structure is simple and fixed. The `_quoted` literal and `quoted_()` function ensure proper JSON string escaping.

## Internal Functions

### `make_database_metadata(name, uri) -> std::string`

**Brief:** Creates a JSON metadata document for database initialization, capturing creation context.

**Called by:** `FileSystemDB::connection()`, `MongoDB::connection()` when initializing a new database

**Purpose:** Provides audit trail and schema versioning for database instances

**Implementation:**
```cpp
std::string db::make_database_metadata(std::string const& name, std::string const& uri) {
  confirm(!name.empty());
  confirm(!uri.empty());

  std::ostringstream oss;
  oss << "document"_quoted << ":" << "{";
  oss << "name"_quoted     << ":" << quoted_(name) << ",";
  oss << "uri"_quoted      << ":" << quoted_(uri)  << ",";
  oss << "locale"_quoted   << ":" << quoted_(apiliteral::database_format_locale) << ",";
  oss << "create_time"_quoted << ":" << quoted_(timestamp()) << ",";
  oss << "create_user"_quoted << ":" << quoted_(expand_environment_variables("$USER")) << ",";
  oss << "uname"_quoted << ":" << unamejson() << ",";
  oss << "database_format"_quoted << ":" << apiliteral::database_format_version;
  oss << "}";

  return oss.str();
}
```

### Metadata Fields

| Field | Source | Description |
|-------|--------|-------------|
| `name` | Function parameter | Database identifier name |
| `uri` | Function parameter | Full connection URI |
| `locale` | `apiliteral::database_format_locale` | System locale for consistent formatting |
| `create_time` | `timestamp()` | ISO 8601 timestamp of creation |
| `create_user` | `$USER` environment variable | Username who created the database |
| `uname` | `unamejson()` | System information (OS, hostname, architecture) |
| `database_format` | `apiliteral::database_format_version` | Schema version for migrations |

### Example Output

```json
{
  "document": {
    "name": "artdaq_config",
    "uri": "filesystemdb:///data/artdaq/artdaq_config",
    "locale": "en_US.UTF-8",
    "create_time": "2025-01-15T10:30:00.123-0600",
    "create_user": "artdaq",
    "uname": {
      "sysname": "Linux",
      "nodename": "daq-server-01",
      "release": "5.14.0-503.38.1.el9_5.x86_64",
      "version": "#1 SMP ...",
      "machine": "x86_64"
    },
    "database_format": 2
  }
}
```

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/StorageProviders/storage_providers.h` | Header file with function declaration |
| `artdaq-database/SharedCommon/configuraion_api_literals.h` | API literal constants |
| `artdaq-database/SharedCommon/helper_functions.h` | Helper utilities (`timestamp()`, `quoted_()`, `expand_environment_variables()`, `unamejson()`) |
| `<sstream>` | String stream for JSON construction |

## Performance Considerations

- **Minimal overhead:** Function is called only during database initialization (infrequent)
- **No caching:** Each call generates fresh metadata with current timestamp
- **String operations:** Uses `std::ostringstream` for efficient string building

## Error Handling Strategy

- Uses `confirm()` macro to validate preconditions
- In debug builds: assertions fire on empty parameters
- In release builds: throws exception on validation failure
- No exception handling for helper functions; errors propagate to caller

## Testing Notes

- **Unit tests:** Covered in `test/StorageProviders/storage_providers_t.cc`
- **Key test cases:**
  - Non-empty name and URI produce valid JSON
  - Empty name triggers precondition failure
  - Empty URI triggers precondition failure
  - Timestamp format is ISO 8601 compliant

## Maintenance Notes

### String Literals
The `_quoted` suffix is a user-defined literal that wraps strings in JSON quotes at compile time:
```cpp
"document"_quoted  // produces: "\"document\""
```
The `quoted_()` function does the same for runtime strings.

### Extending Metadata
To add new metadata fields:
1. Add the field to the output stream in `make_database_metadata()`
2. Consider updating `database_format_version` if the change affects consumers
3. Update any code that parses this metadata
4. Update this documentation

### Thread Safety
This function is thread-safe as it only uses local variables and reads from global constants. Multiple threads can call this function concurrently without synchronization.

## See Also

- [storage_providers.h](./storage_providers.h.md) - Header file with function declaration
- [SharedCommon/helper_functions.h](../SharedCommon/helper_functions.h.md) - Helper utilities used (`timestamp()`, `quoted_()`)
- [SharedCommon/configuraion_api_literals.h](../SharedCommon/configuraion_api_literals.h.md) - API constants
