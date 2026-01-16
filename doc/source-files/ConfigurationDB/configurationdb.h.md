# configurationdb.h

**Path:** `artdaq-database/ConfigurationDB/configurationdb.h`

**Purpose:** Master include header that aggregates all database operation function declarations for the ConfigurationDB module. This header provides access to all database operations through their JSON-based interfaces and declares the trace enablement function for debugging.


## Key Concepts

This header serves as the central aggregation point for all database operation interfaces. It includes all `dboperation_*.h` headers, providing a single include for accessing the complete set of database operations.

### Included Operation Categories

| Category | Header | Description |
|----------|--------|-------------|
| Export/Import | `dboperation_exportimport.h` | Bulk data transfer operations for collections and databases |
| Alias Management | `dboperation_managealiases.h` | Version alias operations (add, remove, find) |
| Configuration Management | `dboperation_manageconfigs.h` | Global configuration operations (create, assign, remove) |
| Document Management | `dboperation_managedocument.h` | Individual document CRUD operations |
| Metadata | `dboperation_metadata.h` | Database and collection information retrieval |
| Search | `dboperation_searchcollection.h` | Collection searching operations |

## Thread Safety

- **Thread-safe:** Yes (individual operations)
- **Concurrent access:** Operations can be called concurrently from multiple threads
- **Locking:** Individual operations handle their own internal locking as needed

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/dboperation_exportimport.h` | Export/import collection and database operations |
| `artdaq-database/ConfigurationDB/dboperation_managealiases.h` | Version alias management operations |
| `artdaq-database/ConfigurationDB/dboperation_manageconfigs.h` | Global configuration management operations |
| `artdaq-database/ConfigurationDB/dboperation_managedocument.h` | Document read/write/mark operations |
| `artdaq-database/ConfigurationDB/dboperation_metadata.h` | Database/collection metadata operations |
| `artdaq-database/ConfigurationDB/dboperation_searchcollection.h` | Collection search operations |

## Functions

### Namespace: `artdaq::database::configuration::json`

#### `enable_trace() -> void`

**Brief:** Enables verbose trace logging for all configuration database operations. Call this function early in application startup to enable detailed diagnostic output for debugging database operations.

**Parameters:** None

**Preconditions:**
- TRACE logging system must be initialized

**Returns:** void

**Postconditions:**
- All database operations will output detailed trace logs
- Trace levels are set to maximum verbosity

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | This function does not throw exceptions |

**Thread Safety:** Safe - can be called from any thread

**Side Effects:**
- Modifies global TRACE configuration
- Increases logging output significantly
- May impact performance due to logging overhead

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/configurationdb.h"
#include <iostream>

int main(int argc, char* argv[]) {
  // Enable trace logging for debugging
  artdaq::database::configuration::json::enable_trace();

  // All subsequent database operations will log at full verbosity
  std::cout << "Trace logging enabled for database operations" << std::endl;

  // Your database operations here...
  return 0;
}
```

## Included Functionality

By including this header, you get access to all database operation functions in the `artdaq::database::configuration::json` namespace:

### Document Management Functions

| Function | Description |
|----------|-------------|
| `read_document()` | Read documents from database by version, entity, or filter |
| `write_document()` | Write new documents to database |
| `find_versions()` | Find available versions of a document |
| `mark_document_readonly()` | Mark documents as read-only to prevent modification |
| `mark_document_deleted()` | Soft-delete documents by marking them as deleted |

### Configuration Management Functions

| Function | Description |
|----------|-------------|
| `find_configurations()` | Find available global configurations |
| `create_configuration()` | Create a new global configuration |
| `assign_configuration()` | Assign a specific version to a configuration |
| `remove_configuration()` | Remove a component from global configuration |
| `configuration_composition()` | Get the complete makeup of a configuration |

### Alias Management Functions

| Function | Description |
|----------|-------------|
| `find_version_aliases()` | Find all aliases for a configuration |
| `add_version_alias()` | Add a new alias pointing to a version |
| `remove_version_alias()` | Remove an existing alias |

### Metadata and Search Functions

| Function | Description |
|----------|-------------|
| `list_databases()` | List all available databases |
| `list_collections()` | List all collections in a database |
| `read_dbinfo()` | Get detailed database information |
| `search_collection()` | Search for documents within a collection |

### Bulk Operations Functions

| Function | Description |
|----------|-------------|
| `export_collection()` | Export all documents from a collection |
| `import_collection()` | Import documents into a collection |
| `export_database()` | Export an entire database |
| `import_database()` | Import an entire database |

## Relationship to Other Components

```
configurationdb.h (this file)
       |
       +-- dboperation_exportimport.h
       |
       +-- dboperation_managealiases.h
       |
       +-- dboperation_manageconfigs.h
       |
       +-- dboperation_managedocument.h
       |
       +-- dboperation_metadata.h
       |
       +-- dboperation_searchcollection.h
```

- **dboperation_*.h files**: Includes all operation headers to provide complete API access
- **dispatch_*.h files**: Operations internally route to provider-specific dispatch functions
- **configuration_common.h**: Higher-level include that adds options classes on top of this

## See Also

- [dboperation_managedocument.h](./dboperation_managedocument.h.md) - Document CRUD operations
- [dboperation_manageconfigs.h](./dboperation_manageconfigs.h.md) - Configuration management
- [dboperation_managealiases.h](./dboperation_managealiases.h.md) - Alias management
- [dboperation_exportimport.h](./dboperation_exportimport.h.md) - Bulk operations
- [dboperation_metadata.h](./dboperation_metadata.h.md) - Metadata operations
- [dboperation_searchcollection.h](./dboperation_searchcollection.h.md) - Search operations
- [configuration_common.h](./configuration_common.h.md) - Full API aggregation with options classes

## Notes for Developers

### Trace Logging

Call `enable_trace()` early in application startup for debugging. This is especially useful when:
- Debugging provider connectivity issues
- Tracing document transformations
- Understanding query execution flow
- Diagnosing performance problems

### Operation Patterns

All operations in this module follow a consistent pattern:
1. Accept operation parameters via an options class (e.g., `ManageDocumentOperation`)
2. Validate parameters and determine target provider
3. Dispatch to provider-specific implementation
4. Return results as JSON or operation result type

### Provider Routing

Operations automatically dispatch to the configured storage provider based on:
- The `ARTDAQ_DATABASE_URI` environment variable
- The provider setting in the options object

### Common Pitfalls

- **Pitfall 1:** Calling `enable_trace()` in production can significantly impact performance due to logging overhead. Use sparingly.
- **Pitfall 2:** The operations require properly initialized options objects. Forgetting to set required fields (like collection name) will result in errors.

## Usage Example

```cpp
#include "artdaq-database/ConfigurationDB/configurationdb.h"
#include "artdaq-database/ConfigurationDB/options_operation_managedocument.h"
#include <iostream>

namespace cf = artdaq::database::configuration;

int main() {
  // Enable full logging for debugging (optional)
  cf::json::enable_trace();

  // Create operation options
  cf::ManageDocumentOperation opts("MyApplication");
  opts.operation("readdocument");
  opts.collection("detector_config");
  opts.format(cf::options::data_format_t::json);
  opts.version("v1.0");

  try {
    // Read document from database
    std::string result;
    auto success = cf::json::read_document(opts, result);

    if (success) {
      std::cout << "Document retrieved: " << result << std::endl;
    } else {
      std::cerr << "Failed to read document" << std::endl;
    }
  } catch (artdaq::database::runtime_error const& e) {
    std::cerr << "Database error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
```

---

**Documentation generated for artdaq-database ConfigurationDB module**
