# provider_connection.cpp

**Path:** `artdaq-database/StorageProviders/FileSystemDB/provider_connection.cpp`

**Implements:** [provider_filedb.h](./provider_filedb.h.md)

**Purpose:** Implements the DBConfig and FileSystemDB connection management classes, handling database URI configuration from environment variables, automatic database initialization on first connection, and metadata creation for database discovery.

## Implementation Overview

This file provides the implementation for two key classes: `DBConfig` which manages database URI configuration, and `FileSystemDB` which provides the connection interface. The connection is lazily initialized - on first access, the implementation checks if the SystemMetadata collection exists and creates it with a metadata document if not. This metadata document enables database discovery and contains information about the database format version.

## Key Algorithms

### Environment-Based Configuration

**Steps:**
1. Start with a default URI pointing to `${ARTDAQ_DATABASE_DATADIR}/filesystemdb/<database_name>`
2. Check if `ARTDAQ_DATABASE_URI` environment variable is set
3. If set and has the `filesystemdb://` prefix, use it as the URI
4. Normalize the URI by removing any trailing slashes

**Why this approach:** Environment variables allow deployment-time configuration without code changes. The prefix check ensures the URI is intended for the FileSystemDB provider.

### Lazy Database Initialization

**Steps:**
1. On first call to `connection()`, construct the SystemMetadata collection path
2. Expand environment variables in the path
3. Strip the `filesystemdb://` URI prefix
4. Check if the SystemMetadata directory exists
5. If it exists, return the cached connection string
6. If it does not exist:
   a. Generate a new object ID
   b. Create a metadata JSON document with database info
   c. Create the SystemMetadata directory
   d. Write the metadata file as `<oid>.json`
7. Return the connection string

**Why this approach:** Lazy initialization defers the cost of filesystem operations until actually needed, and ensures the database is properly set up before any operations occur.

## Internal Functions

### `DBConfig::DBConfig() [default constructor]`

**Brief:** Constructs a database configuration by reading environment variables and establishing default URI values.

**Called by:** Application code, test harnesses

**Purpose:** Provides the primary way to create a DBConfig that respects runtime environment configuration.

**Parameters:** None

**Preconditions:** None (uses defaults if environment variables are not set)

**Returns:** A configured DBConfig instance

**Postconditions:**
- The `uri` member contains a valid `filesystemdb://` URI
- If `ARTDAQ_DATABASE_URI` was set with proper prefix, that value is used
- Otherwise, the default URI uses `${ARTDAQ_DATABASE_DATADIR}`

**Throws:** None

**Thread Safety:** Safe. Constructor only reads environment variables.

**Side Effects:**
- Reads `ARTDAQ_DATABASE_URI` environment variable
- Reads `ARTDAQ_DATABASE_DATADIR` environment variable (indirectly via default)

### `DBConfig::DBConfig(const std::string& uri_)`

**Brief:** Constructs a database configuration with an explicit URI, bypassing environment variable lookup.

**Called by:** Application code needing specific database locations, tests

**Purpose:** Allows programmatic specification of database location without environment variable dependency.

**Parameters:**
- `uri_` - The complete database URI (e.g., `filesystemdb:///data/production/configs`)

**Preconditions:**
- `uri_` must not be empty

**Returns:** A configured DBConfig instance

**Postconditions:**
- The `uri` member equals the provided `uri_` value

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` (via `confirm()`) | `uri_` is empty |

**Thread Safety:** Safe. Constructor only stores the provided value.

**Side Effects:** None

### `FileSystemDB::FileSystemDB(DBConfig config, PassKeyIdiom const&)`

**Brief:** Constructs a FileSystemDB instance with the provided configuration, initializing connection strings but not yet creating the database on disk.

**Called by:** `FileSystemDB::create()` factory method

**Purpose:** Initializes the FileSystemDB with configuration. Uses the PassKey idiom to restrict construction to authorized callers (the factory method).

**Parameters:**
- `config` - The database configuration containing the URI
- `PassKeyIdiom const&` - Access control token (unused, enforces construction restriction)

**Preconditions:**
- `config` must contain a valid URI

**Returns:** A FileSystemDB instance

**Postconditions:**
- `_config` stores the provided configuration
- `_client` contains the connection URI
- `_connection` contains the connection URI with a trailing slash

**Throws:** None directly (config validation happens in DBConfig)

**Thread Safety:** Safe. Constructor only initializes member variables.

**Side Effects:** None

### `FileSystemDB::connection() -> std::string&`

**Brief:** Returns the database connection path, creating the SystemMetadata collection and metadata document on first access if the database does not exist.

**Called by:** All storage provider operations that need the database path

**Purpose:** Provides lazy initialization of the database, ensuring the SystemMetadata collection exists before any operations are performed.

**Parameters:** None

**Preconditions:**
- The database URI must be accessible (filesystem path must be writable)

**Returns:** A reference to the connection string (path with trailing slash)

**Postconditions:**
- The SystemMetadata collection directory exists
- A metadata document exists in the SystemMetadata collection
- Subsequent calls return immediately without filesystem checks

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Directory creation fails (permissions, disk full) |
| `runtime_error` | Metadata file write fails |

**Thread Safety:** Unsafe for first call. The initial metadata creation is not atomic. Safe for subsequent calls.

**Side Effects:**
- On first call: Creates `SystemMetadata/` directory
- On first call: Creates `SystemMetadata/<oid>.json` metadata file
- Reads from filesystem to check if directory exists

## Environment Variables

| Variable | Purpose | Example |
|----------|---------|---------|
| `ARTDAQ_DATABASE_URI` | Override the default database location | `filesystemdb:///data/production/configs` |
| `ARTDAQ_DATABASE_DATADIR` | Base directory for default URI construction | `/data/artdaq` |

**Precedence:**
1. `ARTDAQ_DATABASE_URI` (if set with `filesystemdb://` prefix)
2. Default: `filesystemdb://${ARTDAQ_DATABASE_DATADIR}/filesystemdb/test_configuration_db`

**Example Configuration:**
```bash
# Production configuration
export ARTDAQ_DATABASE_URI="filesystemdb:///data/production/configs"

# Development configuration using DATADIR
export ARTDAQ_DATABASE_DATADIR="/home/user/artdaq"
# Results in: filesystemdb:///home/user/artdaq/filesystemdb/test_configuration_db
```

## URI Format

**Valid URI formats:**
```
filesystemdb:///absolute/path/to/database
filesystemdb://$ARTDAQ_DATABASE_DIR/configs
filesystemdb:///${HOME}/artdaq/db
```

**URI Processing Flow:**
1. Environment variables are expanded via `expand_environment_variables()`
2. The `filesystemdb://` prefix is stripped for filesystem operations
3. Trailing slashes are normalized (removed from input, added to connection path)

**Note:** The URI must use an absolute path after the scheme. Relative paths are not supported.

## Metadata Document Format

The SystemMetadata document created on first connection has this structure:

```json
{
  "database": {
    "name": "artdaq",
    "uri": "filesystemdb:///expanded/path/to/database"
  },
  "_id": {
    "$oid": "507f1f77bcf86cd799439011"
  }
}
```

This document enables:
- Database discovery by searching for SystemMetadata collections
- Database format version tracking (via the metadata structure)
- Identification of the owning application

## Performance Considerations

- **First connection:** O(1) filesystem check + O(1) directory creation + O(1) file write
- **Subsequent connections:** O(1) return of cached value

The first connection incurs filesystem overhead, but this is a one-time cost per database instance. The connection string is cached in `_connection` for fast subsequent access.

## Error Handling Strategy

Errors during connection are fatal since the database cannot function without a valid connection path. All errors propagate as `runtime_error` exceptions with descriptive messages.

**Error categories:**
1. **Configuration errors**: Empty URI (detected by `confirm()`)
2. **Filesystem errors**: Permission denied, disk full (from `mkdir()` and `write_buffer_to_file()`)

## Testing Notes

- **Unit tests:** `test/StorageProviders/FileSystemDB/`
- **Key test cases:**
  - Default configuration from environment
  - Explicit URI configuration
  - First-time database creation
  - Existing database connection
  - Invalid permissions handling

## Maintenance Notes

### PassKey Idiom
The constructor uses the PassKey idiom (`PassKeyIdiom const&`) to restrict who can construct FileSystemDB instances. Only the `FileSystemDB::create()` factory method has access to the PassKey, ensuring controlled instantiation.

### Thread Safety Gap
The `connection()` method has a race condition window during first-time initialization. If two threads call `connection()` simultaneously on a new database, both may attempt to create the SystemMetadata collection. This is mitigated by:
- The directory creation being idempotent (second creation succeeds silently)
- Metadata file writes using unique OIDs

However, external synchronization is recommended for multi-threaded initialization scenarios.

## See Also

- [provider_filedb.h](./provider_filedb.h.md) - Header file with class declarations
- [provider_filedb_readwrite.cpp](./provider_filedb_readwrite.cpp.md) - Document read/write operations
- [filesystem_functions.cpp](./filesystem_functions.cpp.md) - Low-level filesystem utilities
- [storage_providers.h](../storage_providers.h.md) - Base provider interface
- [helper_functions.h](../../SharedCommon/helper_functions.h.md) - `expand_environment_variables()` function

---

**Last Updated:** 2026-01-14
