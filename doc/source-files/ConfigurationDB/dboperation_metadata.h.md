# dboperation_metadata.h

## File Overview

Defines database operation functions for metadata. Provides JSON-based interface for performing operations.

**Location**: `/home/user/artdaq-database/artdaq-database/ConfigurationDB/dboperation_metadata.h`

**Lines of Code**: 38

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
- **detail_metadata.cpp** - Implementation
- **README.md** - Module overview

---

**Documentation generated for artdaq-database ConfigurationDB module**
