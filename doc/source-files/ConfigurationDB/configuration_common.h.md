# configuration_common.h

## File Overview

This header file aggregates all common includes needed for configuration database operations. It serves as a master include file that brings together the configuration database API, operation classes, literals, and dispatch functions.

**Location**: `/home/user/artdaq-database/artdaq-database/ConfigurationDB/configuration_common.h`

**Lines of Code**: 11

**Purpose**: Master include file for configuration database functionality

## Dependencies

### Project Headers
- `"artdaq-database/ConfigurationDB/configurationdb.h"` - Core configuration database types
- `"artdaq-database/ConfigurationDB/dispatch_signatures.h"` - Dispatch function signatures
- `"artdaq-database/ConfigurationDB/options_operations.h"` - All operation option classes
- `"artdaq-database/DataFormats/shared_literals.h"` - Data format string literals
- `"artdaq-database/SharedCommon/configuraion_api_literals.h"` - Configuration API string literals

## Contents

By including this header, you get access to:

### Core Types
- `result_t` - Operation result type (bool, string)
- `data_format_t` - Enumeration of data formats
- Process exit codes

### Operation Classes
- `OperationBase` - Base class for operations
- `ManageDocumentOperation` - Document management operations
- `ManageConfigsOperation` - Configuration management operations
- `ManageAliasesOperation` - Alias management operations
- `BulkOperationsOperation` - Bulk operations

### Dispatch Functions
- Signatures for all database provider dispatch functions

### Literals and Constants
- Operation names (`readdocument`, `writedocument`, etc.)
- Provider names (`filesystem`, `mongodb`, `ucondb`)
- Option names (`collection`, `version`, `entity`, etc.)
- Filter field names
- Data format strings

## Usage

```cpp
#include "artdaq-database/ConfigurationDB/configuration_common.h"

// Now have access to all configuration database functionality
namespace cf = artdaq::database::configuration;
namespace apiliteral = artdaq::database::configapi::literal;

void example() {
    // Use operation classes
    cf::ManageDocumentOperation opts("MyApp");
    opts.operation(apiliteral::operation::readdocument);
    opts.format(cf::options::data_format_t::json);

    // Call dispatch functions
    auto result = cf::json::read_document(opts.to_JsonData());
}
```

## Design Pattern

**Facade Header Pattern**: Provides a single include that brings in an entire subsystem.

**Benefits**:
1. Simplifies client code - one include for all functionality
2. Hides internal organization from users
3. Makes refactoring easier - can reorganize internals without changing user code
4. Faster compilation in some cases (single pre-compiled header)

## Use Cases

**When to Use**:
- Internal ConfigurationDB implementation files
- Code that needs multiple configuration database features
- High-level application code using the configuration API

**When Not to Use**:
- Header files that only need specific types (use minimal includes)
- Code that only uses ConfigurationInterface (use configurationdbifc.h directly)

## Related Files

- **configurationdb.h** - Core types and result definitions
- **options_operations.h** - Aggregates all operation option classes
- **dispatch_signatures.h** - Function signatures for provider dispatch

---

**Documentation generated for artdaq-database ConfigurationDB module**
