# dboperation_searchcollection.cpp

## File Overview

Defines database operation functions for searchcollection. Provides JSON-based interface for performing operations.

**Location**: `/home/user/artdaq-database/artdaq-database/ConfigurationDB/dboperation_searchcollection.cpp`

**Lines of Code**: 69

**Purpose**: Database operation function interface

## Dependencies

### Project Headers
- `#include "artdaq-database/ConfigurationDB/common.h"`
- `#include "artdaq-database/BasicTypes/basictypes.h"`
- `#include "artdaq-database/ConfigurationDB/dboperation_searchcollection.h"`
- `#include "artdaq-database/ConfigurationDB/options_operations.h"`
- `#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"`
- `#include "artdaq-database/DataFormats/shared_literals.h"`


## Contents

**Namespaces**: database, configuration, detail, dbcfg, artdaq



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
- **detail_searchcollection.cpp** - Implementation
- **README.md** - Module overview

---

**Documentation generated for artdaq-database ConfigurationDB module**
