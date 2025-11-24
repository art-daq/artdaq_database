# options_operations.h

## File Overview

This header file aggregates all operation option classes used in the ConfigurationDB module. It provides a single include point for accessing all operation configuration classes.

**Location**: `/home/user/artdaq-database/artdaq-database/ConfigurationDB/options_operations.h`

**Lines of Code**: 11

**Purpose**: Aggregate header for all operation option classes

## Dependencies

### Project Headers
- `"artdaq-database/ConfigurationDB/options_operation_base.h"` - Base class for all operations
- `"artdaq-database/ConfigurationDB/options_operation_bulkoperations.h"` - Bulk operation options
- `"artdaq-database/ConfigurationDB/options_operation_managealiases.h"` - Alias management options
- `"artdaq-database/ConfigurationDB/options_operation_manageconfigs.h"` - Configuration management options
- `"artdaq-database/ConfigurationDB/options_operation_managedocument.h"` - Document management options

## Included Classes

### OperationBase
Base class providing common functionality for all operation types:
- Provider selection (filesystem, mongodb, ucondb)
- Operation name (read, write, overwrite, etc.)
- Data format (json, fhicl, xml, gui, db, csv)
- Collection name
- Query filter
- Result file name
- Conversion to/from JSON
- Program options parsing

### ManageDocumentOperation
Options for document-level operations:
- Read document
- Write document
- Overwrite document
- Mark document read-only
- Find versions
- List collections
- Search collections

### ManageConfigsOperation
Options for configuration management:
- Find configurations
- Add configuration
- Remove configuration
- Assign configuration to global config
- Get configuration composition

### ManageAliasesOperation
Options for alias management:
- Find aliases
- Add alias
- Remove alias
- Assign alias to global config

### BulkOperationsOperation
Options for bulk operations:
- Export configurations
- Import configurations
- Bulk document operations

## Usage

```cpp
#include "artdaq-database/ConfigurationDB/options_operations.h"

using artdaq::database::configuration::ManageDocumentOperation;
using artdaq::database::configuration::ManageConfigsOperation;
using artdaq::database::configuration::options::data_format_t;

void example() {
    // Document operation
    ManageDocumentOperation doc_opts("MyApp");
    doc_opts.operation("readdocument");
    doc_opts.collection("ComponentConfigs");
    doc_opts.version("v1.0");
    doc_opts.format(data_format_t::json);

    // Configuration operation
    ManageConfigsOperation cfg_opts("MyApp");
    cfg_opts.operation("findconfigs");
    cfg_opts.configuration("Run12345");
    cfg_opts.format(data_format_t::gui);
}
```

## Design Pattern

**Aggregate Header Pattern**: Simplifies includes by providing a single header for related functionality.

**Benefits**:
1. **Single Include**: Users include one header instead of five
2. **Organized**: Logically groups related operation classes
3. **Maintainable**: Easy to add new operation types
4. **Clear Dependencies**: Explicit list of what's included

## When to Use

**Use this header when**:
- You need multiple operation types in one file
- You're implementing dispatch functions that handle multiple operations
- You want to avoid managing multiple includes

**Use specific headers when**:
- You only need one operation type (faster compilation)
- Writing header files (minimize dependencies)
- Creating modular components

## Operation Hierarchy

```
OperationBase (base functionality)
├── ManageDocumentOperation (document-level operations)
├── ManageConfigsOperation (configuration management)
├── ManageAliasesOperation (alias management)
└── BulkOperationsOperation (bulk operations)
```

## Related Files

- **options_operation_base.h** - Base class implementation
- **options_operation_managedocument.h** - Document operation details
- **options_operation_manageconfigs.h** - Configuration operation details
- **options_operation_managealiases.h** - Alias operation details
- **options_operation_bulkoperations.h** - Bulk operation details

---

**Documentation generated for artdaq-database ConfigurationDB module**
