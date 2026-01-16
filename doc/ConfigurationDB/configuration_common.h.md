# configuration_common.h

**Path:** `artdaq-database/ConfigurationDB/configuration_common.h`

**Purpose:** Master aggregation header that brings together all common includes needed for configuration database operations. This facade header combines the configuration database API, dispatch signatures, operation options, and string literals into a single convenient include.


## Key Concepts

This file implements the **Facade Header Pattern**, providing a single include that brings in an entire subsystem. It simplifies client code by hiding the internal organization of the ConfigurationDB module.

### What This Header Provides

- **Core Types**: `result_t`, `data_format_t`, process exit codes
- **Operation Classes**: All option classes for database operations (`OperationBase`, `ManageDocumentOperation`, etc.)
- **Dispatch Functions**: Signatures for provider-specific dispatch
- **Literals and Constants**: Operation names, provider names, option names
- **Database Operations**: All CRUD and management operations for configurations

## Thread Safety

- **Thread-safe:** Conditional
- **Concurrent access:** Individual operations are thread-safe, but operation sequences may require external synchronization
- **Locking:** No locking at the header level; individual operations handle their own thread safety

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/configurationdb.h` | Core configuration database types and all operation includes |
| `artdaq-database/ConfigurationDB/dispatch_signatures.h` | Dispatch function signatures for provider routing |
| `artdaq-database/ConfigurationDB/options_operations.h` | All operation option classes |
| `artdaq-database/DataFormats/shared_literals.h` | Data format string literals (json, fhicl, xml) |
| `artdaq-database/SharedCommon/configuraion_api_literals.h` | Configuration API string literals |

## Classes/Structures

This header aggregates access to the following classes (documented in their respective header files):

### Operation Classes

| Class | Description |
|-------|-------------|
| `OperationBase` | Base class for all operation parameter containers |
| `ManageDocumentOperation` | Parameters for document read/write operations |
| `ManageConfigsOperation` | Parameters for global configuration management |
| `ManageAliasesOperation` | Parameters for version alias operations |
| `BulkOperationsOperation` | Parameters for bulk import/export operations |

### Type Aliases

| Type | Description |
|------|-------------|
| `data_format_t` | Enumeration of supported data formats |
| `result_t` | Operation result type with success/failure and message |

## Namespace Aliases

The header provides access to commonly used namespace aliases:

### `apiliteral`

**Brief:** Shorthand alias for `artdaq::database::configapi::literal` namespace containing API string constants.

**Contains:**
- `apiliteral::operation::*` - Operation names (readdocument, writedocument, etc.)
- `apiliteral::provider::*` - Provider names (filesystem, mongo, ucon)
- `apiliteral::option::*` - Option names (collection, version, entity, etc.)

### `jsonliteral`

**Brief:** Shorthand alias for `artdaq::database::dataformats::literal` namespace containing data format constants.

## Functions

This header provides access to all database operation functions (documented in their respective headers):

### Document Operations

| Function | Description |
|----------|-------------|
| `read_document()` | Read documents from database by version, entity, or filter |
| `write_document()` | Write new documents to database |
| `find_versions()` | Find available versions of a document |
| `mark_document_readonly()` | Mark documents as read-only to prevent modification |
| `mark_document_deleted()` | Soft-delete documents by marking them as deleted |

### Configuration Operations

| Function | Description |
|----------|-------------|
| `find_configurations()` | Find available global configurations |
| `create_configuration()` | Create a new global configuration |
| `assign_configuration()` | Assign a specific version to a configuration |
| `remove_configuration()` | Remove a component from global configuration |

### Alias Operations

| Function | Description |
|----------|-------------|
| `find_version_aliases()` | Find all aliases for a configuration |
| `add_version_alias()` | Add a new alias pointing to a version |
| `remove_version_alias()` | Remove an existing alias |

### Bulk Operations

| Function | Description |
|----------|-------------|
| `export_collection()` | Export collection data to file |
| `import_collection()` | Import collection data from file |
| `export_database()` | Export entire database to file |
| `import_database()` | Import entire database from file |

## Relationship to Other Components

```
configuration_common.h (this file)
           |
           +-- configurationdb.h
           |       +-- dboperation_exportimport.h
           |       +-- dboperation_managealiases.h
           |       +-- dboperation_manageconfigs.h
           |       +-- dboperation_managedocument.h
           |       +-- dboperation_metadata.h
           |       +-- dboperation_searchcollection.h
           |
           +-- dispatch_signatures.h
           |
           +-- options_operations.h
           |
           +-- shared_literals.h
           |
           +-- configuraion_api_literals.h
```

- **configurationdb.h**: Included to provide all operation function declarations
- **dispatch_signatures.h**: Provides the dispatch pattern infrastructure for provider routing
- **options_operations.h**: Aggregates all operation option classes
- **conftoolifc.h**: Uses this header for command-line tool implementation

## See Also

- [configurationdb.h](./configurationdb.h.md) - Core database operation declarations
- [dispatch_signatures.h](./dispatch_signatures.h.md) - Dispatch function type definitions
- [options_operations.h](./options_operations.h.md) - Operation option class aggregation
- [OperationBase](./options_operation_base.h.md) - Base class for operations

## Notes for Developers

### When to Use

- **Internal ConfigurationDB implementation files**: Use this for convenience in .cpp files
- **High-level application code**: Use when you need access to the full API
- **CLI tool implementations**: The conftool uses this as its primary include

### When NOT to Use

- **Public API headers**: Use minimal includes to reduce compile-time dependencies
- **Individual operation implementations**: Include only what is needed

### Compilation Impact

Including this header brings in many transitive dependencies, which may increase compile time. For performance-critical build scenarios, consider using more targeted includes.

### Common Pitfalls

- **Pitfall 1:** Using this header in other public headers creates large dependency chains. Keep public interfaces minimal.
- **Pitfall 2:** The literals in this header are string constants, not enums. Be careful with typos as they will not be caught at compile time.

## Usage Example

```cpp
#include "artdaq-database/ConfigurationDB/configuration_common.h"
#include <iostream>

namespace cf = artdaq::database::configuration;
namespace apiliteral = artdaq::database::configapi::literal;

void performDatabaseOperation() {
  // Create operation parameters
  cf::ManageDocumentOperation opts("MyApplication");

  // Set operation type using literals
  opts.operation(apiliteral::operation::readdocument);
  opts.collection("detector_settings");
  opts.format(cf::options::data_format_t::json);

  try {
    // Access literals for logging
    std::cout << "Performing operation: " << opts.operation() << std::endl;
    std::cout << "On collection: " << opts.collection() << std::endl;

    // Perform the operation
    std::string result;
    cf::json::read_document(opts, result);
    std::cout << "Result: " << result << std::endl;
  } catch (artdaq::database::runtime_error const& e) {
    std::cerr << "Database error: " << e.what() << std::endl;
  }
}
```

### Anti-patterns

```cpp
// DON'T do this: Including in a public header
// my_public_api.h
#include "artdaq-database/ConfigurationDB/configuration_common.h"  // Heavy include!

// DO this instead: Include only what is needed
// my_public_api.h
#include "artdaq-database/ConfigurationDB/options_operation_managedocument.h"
```

---

**Documentation generated for artdaq-database ConfigurationDB module**
