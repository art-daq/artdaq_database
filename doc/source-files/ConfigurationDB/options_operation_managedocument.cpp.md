# options_operation_managedocument.cpp

## File Overview

Implements the operation option class for managedocument operations. Provides configuration interface for setting up database operations.

**Location**: `/home/user/artdaq-database/artdaq-database/ConfigurationDB/options_operation_managedocument.cpp`

**Lines of Code**: 426

**Purpose**: Operation configuration and option handling

## Dependencies

### Project Headers
- `#include "artdaq-database/ConfigurationDB/common.h"`
- `#include "artdaq-database/ConfigurationDB/options_operation_base.h"`
- `#include "artdaq-database/ConfigurationDB/options_operation_managedocument.h"`
- `#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"`
- `#include "artdaq-database/DataFormats/shared_literals.h"`
- `#include "artdaq-database/BasicTypes/basictypes.h"`
- `#include "artdaq-database/DataFormats/Json/json_reader.h"`
- `#include "artdaq-database/DataFormats/Json/json_writer.h"`


## Contents

**Namespaces**: dbbt, artdaq, db, cf



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
