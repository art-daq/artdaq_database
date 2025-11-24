# dboperation_managedocument.h

## File Overview

Defines database operation functions for managedocument. Provides JSON-based interface for performing operations.

**Location**: `/home/user/artdaq-database/artdaq-database/ConfigurationDB/dboperation_managedocument.h`

**Lines of Code**: 48

**Purpose**: Database operation function interface

## Dependencies

### Project Headers
- `#include "artdaq-database/SharedCommon/returned_result.h"`


## Contents

**Namespaces**: json, database, configuration, detail, debug, artdaq, opts

**Classes/Structs**: ManageDocumentOperation



## Usage

```cpp
// Call database operation function
auto result = operation_function(query_json);
if (result.first) {
    // Success
} else {
    // Handle error: result.second
}
```

## Related Files

- **configurationdb.h** - Master include
- **detail_managedocument.cpp** - Implementation
- **README.md** - Module overview

---

**Documentation generated for artdaq-database ConfigurationDB module**
