# options_operations.h

**Path:** `artdaq-database/ConfigurationDB/options_operations.h`

**Purpose:** Convenience header that aggregates all operation options headers, providing a single include for accessing the complete operation options API. This header simplifies usage by combining `OperationBase`, `ManageDocumentOperation`, `ManageConfigsOperation`, `ManageAliasesOperation`, and `BulkOperations` classes in one include.


## Key Concepts

### Header Aggregation Pattern
This header follows a common C++ pattern of providing a single "umbrella" header that includes all related headers from a subsystem. This simplifies client code by reducing the number of includes needed and ensuring all related types are available together.

### Operation Options Hierarchy
The included headers define the following class hierarchy:

```
OperationBase (abstract base)
    |
    +-- ManageDocumentOperation  (document-level operations)
    +-- ManageConfigsOperation   (configuration-level operations)
    +-- ManageAliasesOperation   (alias management operations)

BulkOperations (standalone container for multiple operations)
```

## Thread Safety

- **Thread-safe:** N/A (header-only aggregation)
- See individual headers for thread safety of contained classes

## Dependencies

This header includes the following headers, providing access to all operation option classes:

| Include | Purpose |
|---------|---------|
| `options_operation_base.h` | Base class for all operation options |
| `options_operation_bulkoperations.h` | Bulk operations container class |
| `options_operation_managealiases.h` | Alias management operation options |
| `options_operation_manageconfigs.h` | Configuration management operation options |
| `options_operation_managedocument.h` | Document management operation options |

## Thread Safety

- **Thread-safe:** N/A (header-only aggregation)
- See individual headers for thread safety of contained classes:
  - `OperationBase`: Not thread-safe
  - `ManageDocumentOperation`: Not thread-safe
  - `ManageConfigsOperation`: Not thread-safe
  - `ManageAliasesOperation`: Not thread-safe
  - `BulkOperations`: Not thread-safe

## Included Classes Summary

| Class | Purpose | Primary Use Case |
|-------|---------|------------------|
| `OperationBase` | Common operation parameters (provider, collection, format) | Base for all operations |
| `ManageDocumentOperation` | Document-specific parameters (version, entity, run, source file) | Reading/writing individual documents |
| `ManageConfigsOperation` | Configuration-specific parameters (lighter than document) | Listing/managing configurations |
| `ManageAliasesOperation` | Alias parameters (version alias, config alias, run) | Creating/managing aliases |
| `BulkOperations` | Container for multiple operations | Batch processing |

## Usage

### Basic Usage

```cpp
#include "artdaq-database/ConfigurationDB/options_operations.h"

using namespace artdaq::database::configuration;

void performOperations() {
  // All operation types available via single include

  // Document operation
  auto docOpts = ManageDocumentOperation{"myapp"};
  docOpts.operation("writedocument");
  docOpts.entity("MyComponent");
  docOpts.version("v1.0");

  // Configuration operation
  auto cfgOpts = ManageConfigsOperation{"myapp"};
  cfgOpts.operation("listconfigs");

  // Alias operation
  auto aliasOpts = ManageAliasesOperation{"myapp"};
  aliasOpts.operation("addversionalias");
  aliasOpts.versionAlias("production");

  // Bulk operations
  auto bulkOpts = BulkOperations{"myapp"};
  bulkOpts.readJsonData(jsonData);
}
```

### Selecting Specific Headers

For minimal compilation dependencies, you can include individual headers instead:

```cpp
// Only need document operations
#include "artdaq-database/ConfigurationDB/options_operation_managedocument.h"

// Only need bulk operations
#include "artdaq-database/ConfigurationDB/options_operation_bulkoperations.h"
```

## Operation Hierarchy

```
OperationBase (base functionality)
    +-- operation()       - Operation name (read, write, etc.)
    +-- provider()        - Storage provider (filesystem, mongodb)
    +-- collection()      - Target collection name
    +-- format()          - Data format (json, fhicl, xml, gui)
    +-- queryFilter()     - Search filter
    +-- resultFileName()  - Output file path
    |
    +-- ManageDocumentOperation (document-level operations)
    |       +-- version()
    |       +-- entity()
    |       +-- run()
    |       +-- configuration()
    |       +-- sourceFileName()
    |
    +-- ManageConfigsOperation (configuration management)
    |       +-- version()
    |       +-- entity()
    |       +-- configuration()
    |
    +-- ManageAliasesOperation (alias management)
            +-- version()
            +-- versionAlias()
            +-- entity()
            +-- run()
            +-- configuration()
            +-- configurationAlias()

BulkOperations (container - not derived from OperationBase)
    +-- bulkOperations()  - JSON specification
    +-- begin()/end()     - Iteration over contained operations
```

## Relationship to Other Components

This header is used by:
- **ConfigurationDB API implementation** - For handling all operation types
- **CLI tools** - For unified access to operation options
- **Test files** - For testing operation option handling

The aggregated headers provide the options-handling foundation used by:
- `configurationdbifc.h` - Main API interface
- `dboperation_*.h` - Database operation implementations
- `dispatch_*.h` - Operation dispatching logic

## See Also

- [options_operation_base.h](./options_operation_base.h.md) - Base class documentation
- [options_operation_managedocument.h](./options_operation_managedocument.h.md) - Document operations
- [options_operation_manageconfigs.h](./options_operation_manageconfigs.h.md) - Configuration operations
- [options_operation_managealiases.h](./options_operation_managealiases.h.md) - Alias operations
- [options_operation_bulkoperations.h](./options_operation_bulkoperations.h.md) - Bulk operations

## Notes for Developers

### When to Use This Header

**Use this header when:**
- You need multiple operation types in the same file
- You want simplified includes for client code
- You're writing tests that cover multiple operation types
- You're implementing dispatch functions that handle multiple operations

**Use individual headers when:**
- You only need one operation type
- Compilation time is critical
- You want minimal header dependencies
- Writing header files (minimize transitive dependencies)

### Include Guard

The header uses the include guard `_ARTDAQ_DATABASE_CONFIGURATIONDB_OPTIONS_OPERATIONS_H_` to prevent multiple inclusion.

### Design Pattern

**Aggregate Header Pattern**: Simplifies includes by providing a single header for related functionality.

**Benefits**:
1. **Single Include**: Users include one header instead of five
2. **Organized**: Logically groups related operation classes
3. **Maintainable**: Easy to add new operation types
4. **Clear Dependencies**: Explicit list of what's included

---

**Documentation generated for artdaq-database ConfigurationDB module**
