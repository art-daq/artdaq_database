# dboperation_exportimport.cpp

## File Overview

Defines database operation functions for exportimport. Provides JSON-based interface for performing operations.

**Location**: `/home/user/artdaq-database/artdaq-database/ConfigurationDB/dboperation_exportimport.cpp`

**Lines of Code**: 582

**Purpose**: Database operation function interface

## Dependencies

### Project Headers
- `#include "artdaq-database/ConfigurationDB/common.h"`
- `#include "artdaq-database/BasicTypes/basictypes.h"`
- `#include "artdaq-database/ConfigurationDB/dboperation_exportimport.h"`
- `#include "artdaq-database/ConfigurationDB/dboperation_managedocument.h"`
- `#include "artdaq-database/ConfigurationDB/dboperation_metadata.h"`
- `#include "artdaq-database/ConfigurationDB/options_operations.h"`
- `#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"`
- `#include "artdaq-database/DataFormats/shared_literals.h"`
- `#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"`
- `#include "artdaq-database/SharedCommon/configuraion_api_literals.h"`
- ... and 1 more


## Contents

**Namespaces**: database, configuration, detail, dbcfg, jsn, db, artdaq



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
- **detail_exportimport.cpp** - Implementation
- **README.md** - Module overview

---

**Documentation generated for artdaq-database ConfigurationDB module**
