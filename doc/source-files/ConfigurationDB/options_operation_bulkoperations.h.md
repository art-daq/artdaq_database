# options_operation_bulkoperations.h

## File Overview

Implements the operation option class for bulkoperations operations. Provides configuration interface for setting up database operations.

**Location**: `/home/user/artdaq-database/artdaq-database/ConfigurationDB/options_operation_bulkoperations.h`

**Lines of Code**: 82

**Purpose**: Operation configuration and option handling

## Dependencies

### Project Headers
- `#include <list>`
- `#include "artdaq-database/ConfigurationDB/options_operation_base.h"`
- `#include <boost/program_options.hpp>`


## Contents

**Namespaces**: database, configuration, bpo, basictypes, debug, artdaq, options, apiliteral

**Classes/Structs**: BulkOperations, JsonData



## Usage

```cpp
// Create operation options
OperationClass opts("AppName");
opts.operation("operation_name");
opts.collection("CollectionName");
opts.format(data_format_t::json);

// Use in database operations
auto result = performOperation(opts.to_JsonData());
```

## Related Files

- **options_operation_base.h** - Base class
- **options_operations.h** - Aggregate header
- **README.md** - Module overview

---

**Documentation generated for artdaq-database ConfigurationDB module**
