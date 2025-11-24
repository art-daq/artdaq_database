# storage_providers.cpp

## File Overview

This implementation file provides the `make_database_metadata()` function that generates standardized JSON metadata for new databases. This metadata is stored in the SystemMetadata collection and tracks important information about database creation and format.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/storage_providers.cpp`

**Lines of Code**: 29

**Purpose**: Generate database metadata JSON for system tracking

## Dependencies

### Project Headers
- `"artdaq-database/StorageProviders/storage_providers.h"` - Provider interface declarations
- `"artdaq-database/SharedCommon/configuraion_api_literals.h"` - Configuration constants
- `"artdaq-database/SharedCommon/helper_functions.h"` - Utility functions (timestamp, quoted_, etc.)

### Standard Library
- `<sstream>` - std::ostringstream for JSON construction

## Namespace Aliases

```cpp
namespace db = artdaq::database;
namespace apiliteral = db::configapi::literal;
```

**Purpose**: Convenience aliases to shorten long namespace paths.

---

## Function: make_database_metadata

```cpp
std::string db::make_database_metadata(std::string const& name,
                                       std::string const& uri);
```

### Purpose

Generates a JSON string containing metadata about a newly created database. This metadata is stored in the special `SystemMetadata` collection and provides crucial information for database management, migration, and auditing.

### Parameters

- **name** - The database name
  - Type: `std::string const&`
  - Must not be empty (validated with `confirm()`)
  - Example: `"artdaq_configuration_db"`

- **uri** - The database connection URI
  - Type: `std::string const&`
  - Must not be empty (validated with `confirm()`)
  - Example: `"filesystemdb:///data/artdaq/configs"`

### Returns

A JSON string (as `std::string`) containing the database metadata.

### Validation

```cpp
confirm(!name.empty());
confirm(!uri.empty());
```

Both parameters are validated using the `confirm()` macro/function:
- **Debug builds**: Assertions will fire if empty
- **Release builds**: Will throw exception if empty

### Generated JSON Structure

The function generates a JSON object with the following structure:

```json
{
  "document": {
    "name": "artdaq_configuration_db",
    "uri": "filesystemdb:///data/artdaq/configs",
    "locale": "en_US.UTF-8",
    "create_time": "2025-11-13T10:30:45.123-0600",
    "create_user": "artdaq",
    "uname": {
      "sysname": "Linux",
      "nodename": "daq-host",
      "release": "4.4.0",
      "version": "#1 SMP",
      "machine": "x86_64"
    },
    "database_format": 1
  }
}
```

### Metadata Fields

#### name
- **Source**: Function parameter
- **Type**: String
- **Purpose**: Human-readable database name
- **Example**: `"artdaq_configuration_db"`

#### uri
- **Source**: Function parameter
- **Type**: String
- **Purpose**: Connection string to access the database
- **Format**: `<provider>://<path>` or `<provider>://<host>:<port>/<database>`
- **Examples**:
  - `"filesystemdb:///data/artdaq/configs"`
  - `"mongodb://localhost:27017/artdaq_db"`
  - `"ucondb://http://ucondb-server.fnal.gov:8080/artdaq"`

#### locale
- **Source**: `apiliteral::database_format_locale` constant
- **Type**: String
- **Purpose**: Character encoding and locale for the database
- **Value**: `"en_US.UTF-8"` (standard)
- **Usage**: Ensures consistent string/time formatting

#### create_time
- **Source**: `timestamp()` function
- **Type**: String (ISO 8601 format)
- **Purpose**: Records when the database was created
- **Format**: `YYYY-MM-DDTHH:MM:SS.mmm±ZZZZ`
- **Example**: `"2025-11-13T10:30:45.123-0600"`
- **Precision**: Milliseconds

#### create_user
- **Source**: `expand_environment_variables("$USER")`
- **Type**: String
- **Purpose**: Records which user created the database
- **Example**: `"artdaq"`, `"jsmith"`
- **Usage**: Audit trail and ownership tracking

#### uname
- **Source**: `unamejson()` function
- **Type**: JSON object
- **Purpose**: System information where database was created
- **Fields**:
  - `sysname`: Operating system name (e.g., "Linux")
  - `nodename`: Hostname (e.g., "daq-host")
  - `release`: OS version/release (e.g., "4.4.0")
  - `version`: Detailed version string
  - `machine`: Hardware architecture (e.g., "x86_64")

#### database_format
- **Source**: `apiliteral::database_format_version` constant
- **Type**: Integer
- **Purpose**: Schema/format version for database migration
- **Value**: Numeric version number
- **Usage**: Allows automatic detection of old database formats

### Implementation Details

#### String Construction

The function uses `std::ostringstream` for efficient string building:

```cpp
std::ostringstream oss;
oss << "document"_quoted << ":" << "{";
oss << "name"_quoted     << ":" << quoted_(name) << ",";
// ... more fields
oss << "}";
return oss.str();
```

#### Formatting

The code uses `// clang-format off` and `// clang-format on` comments to preserve the manual formatting alignment of the JSON construction code.

#### Helper Functions Used

- **`_quoted` literal operator**: Adds quotes to string literals
  - `"document"_quoted` → `"\"document\""`

- **`quoted_()` function**: Adds quotes to runtime strings
  - `quoted_(name)` → `"\"artdaq_configuration_db\""`

- **`timestamp()`**: Generates ISO 8601 timestamp
  - Returns: `"2025-11-13T10:30:45.123-0600"`

- **`expand_environment_variables()`**: Expands shell variables
  - `expand_environment_variables("$USER")` → `"artdaq"`

- **`unamejson()`**: System information as JSON
  - Returns: `{"sysname":"Linux",...}`

### Usage Example

```cpp
#include "artdaq-database/StorageProviders/storage_providers.h"

// Create metadata for a new FileSystem database
std::string metadata = artdaq::database::make_database_metadata(
    "my_config_db",
    "filesystemdb:///data/configs"
);

// Write to SystemMetadata collection
JSONDocument metaDoc;
metaDoc.setCollection(artdaq::database::system_metadata);
metaDoc.setData(metadata);
provider->writeDocument(metaDoc);
```

### Usage in Provider Initialization

Typically called during database creation:

```cpp
// In FileSystemDB provider
void FileSystemDBProvider::initializeDatabase(std::string const& path) {
    // Create database directory structure
    createDirectories(path);

    // Generate and store metadata
    std::string uri = "filesystemdb://" + path;
    std::string metadata = make_database_metadata(getDatabaseName(), uri);

    // Write to SystemMetadata
    writeSystemMetadata(metadata);
}
```

### Error Handling

#### Validation Errors

If either parameter is empty:
- **Debug build**: Program terminates with assertion failure and stack trace
- **Release build**: Throws exception (runtime_exception)

```cpp
// This will fail validation
try {
    auto metadata = make_database_metadata("", "uri");  // Empty name
} catch (std::exception const& e) {
    // Handle error
}
```

#### Environment Variable Expansion

If `$USER` is not defined:
- `expand_environment_variables()` returns empty string
- No error thrown (empty create_user field)
- Check implementation of `expand_environment_variables()` for details

### Thread Safety

This function is **thread-safe** because:
1. No shared state is modified
2. All operations are on local variables
3. Helper functions are thread-safe
4. Uses const references for parameters

Multiple threads can safely call this function concurrently.

### Performance Considerations

#### Efficiency
- **String Building**: Uses ostringstream for efficient concatenation
- **Small JSON**: Generated JSON is typically < 500 bytes
- **System Calls**: One system call for uname, one for timestamp
- **Memory**: Single allocation for the ostringstream buffer

#### Optimization

The function is not called frequently (only during database creation), so optimization is not critical. Current implementation prioritizes:
- Readability
- Correctness
- Maintainability

### Testing Considerations

#### Unit Testing

To test this function:

```cpp
// Test basic functionality
auto metadata = make_database_metadata("test_db", "file:///tmp/test");
assert(metadata.find("\"name\":\"test_db\"") != std::string::npos);
assert(metadata.find("\"uri\":\"file:///tmp/test\"") != std::string::npos);

// Test validation
try {
    make_database_metadata("", "uri");
    assert(false);  // Should not reach here
} catch (...) {
    // Expected
}
```

#### Fake Time Mode

For reproducible testing:

```cpp
// Enable fake time mode
artdaq::database::useFakeTime(true);

auto metadata = make_database_metadata("db", "uri");
// create_time will be a fixed fake timestamp

// Disable fake time
artdaq::database::useFakeTime(false);
```

### Design Rationale

#### JSON Format

The metadata is generated as a JSON string rather than using a JSON library:
- **Simplicity**: Small, fixed structure doesn't need complex library
- **Performance**: Direct string construction is faster
- **Dependencies**: Reduces dependency on heavy JSON libraries
- **Control**: Exact control over formatting

#### SystemMetadata Collection

The metadata is stored in a special `SystemMetadata` collection:
- **Separation**: Keeps system info separate from user data
- **Discovery**: Easy to find database metadata
- **Migration**: Format version enables database upgrades
- **Auditing**: Tracks creation information

#### Included Information

The specific fields were chosen for:
- **name/uri**: Essential for database identification
- **locale**: Ensures consistent string/date handling
- **create_time/user**: Audit trail
- **uname**: Debugging and support (identify system issues)
- **database_format**: Forward compatibility and migration

### Related Files

- **storage_providers.h** - Function declaration
- **SharedCommon/helper_functions.h** - Helper function implementations
- **SharedCommon/configuraion_api_literals.h** - Constants used
- **FileSystemDB/provider_filedb.cpp** - Example usage
- **MongoDB/provider_mongodb.cpp** - Example usage

### Migration Considerations

When the `database_format` version changes:
1. Update `apiliteral::database_format_version`
2. Add migration code to detect old format
3. Document format changes
4. Provide upgrade path for existing databases

### Best Practices

#### Always Validate Parameters
```cpp
// GOOD: Parameters are validated
std::string meta = make_database_metadata(name, uri);

// BAD: Don't assume parameters are valid
// Will fail at runtime if empty
```

#### Store Immediately
```cpp
// GOOD: Store metadata immediately after creation
auto metadata = make_database_metadata(name, uri);
storeSystemMetadata(metadata);

// BAD: Don't delay storage
// Database may be used without metadata
```

#### Use Standard Collection Name
```cpp
// GOOD: Use the constant
metaDoc.setCollection(artdaq::database::system_metadata);

// BAD: Don't hardcode
// metaDoc.setCollection("SystemMetadata");
```

---

## Complete Example

```cpp
#include "artdaq-database/StorageProviders/storage_providers.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include <iostream>

void createNewDatabase(std::string const& name, std::string const& path) {
    // Generate database URI
    std::string uri = "filesystemdb://" + path;

    // Generate metadata
    std::string metadata = artdaq::database::make_database_metadata(name, uri);

    // Print metadata (for debugging)
    std::cout << "Database metadata:\n" << metadata << std::endl;

    // Create provider and initialize database
    auto provider = createFileSystemProvider(path);

    // Store metadata in SystemMetadata collection
    JSONDocument metaDoc;
    metaDoc.setCollection(artdaq::database::system_metadata);
    metaDoc.setData(metadata);

    auto id = provider->writeDocument(metaDoc);
    std::cout << "Metadata stored with ID: " << id << std::endl;
}
```

---

**Documentation generated for artdaq-database StorageProviders module**
