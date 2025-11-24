# configurationdb.h

## File Overview

Master include header that aggregates all database operation function declarations for the ConfigurationDB module. This header provides access to all database operations through their JSON-based interfaces.

**Location**: `/home/user/artdaq-database/artdaq-database/ConfigurationDB/configurationdb.h`

**Lines of Code**: 22

**Purpose**: Aggregate header for all database operation function interfaces

## Dependencies

### Project Headers
- `"dboperation_exportimport.h"` - Export/import operations
- `"dboperation_managealiases.h"` - Alias management operations
- `"dboperation_manageconfigs.h"` - Configuration management operations
- `"dboperation_managedocument.h"` - Document management operations
- `"dboperation_metadata.h"` - Metadata operations
- `"dboperation_searchcollection.h"` - Search operations

## Namespace: artdaq::database::configuration::json

Declares the trace enablement function in the `json` sub-namespace.

## Functions

### enable_trace
```cpp
void enable_trace()
```

**Purpose**: Enable trace logging for all configuration database operations.

**Usage**:
```cpp
#include "artdaq-database/ConfigurationDB/configurationdb.h"

artdaq::database::configuration::json::enable_trace();
// All database operations will now log at full verbosity
```

## Included Functionality

By including this header, you get access to all database operation functions across categories:

### Document Management
- `read_document()` - Read documents from database
- `write_document()` - Write documents to database
- `find_versions()` - Find document versions
- `mark_document_readonly()` - Mark documents as read-only

### Configuration Management
- `find_configurations()` - Find available configurations
- `create_configuration()` - Create new global configuration
- `remove_configuration()` - Remove global configuration
- `configuration_composition()` - Get configuration makeup

### Alias Management
- `find_aliases()` - Find configuration aliases
- `add_alias()` - Add new alias
- `remove_alias()` - Remove alias

### Metadata & Search
- `list_databases()` - List available databases
- `list_collections()` - List collections in database
- `database_metadata()` - Get database metadata
- `search_collection()` - Search within collection

### Bulk Operations
- `export_configuration()` - Export configurations
- `import_configuration()` - Import configurations

## Usage Example

```cpp
#include "artdaq-database/ConfigurationDB/configurationdb.h"

namespace cf = artdaq::database::configuration;

// Enable full logging
cf::json::enable_trace();

// Now call any database operation function
// (actual calls require proper setup and parameters)
```

## Related Files

- **dboperation_*.h** - Individual operation function declarations
- **dispatch_*.h** - Provider-specific implementations
- **configuration_common.h** - Higher-level includes with options classes

**Documentation generated for artdaq-database ConfigurationDB module**
