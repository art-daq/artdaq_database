# common.h

## File Overview

This is a simple header file that includes common definitions and utilities used throughout the ConfigurationDB module. It acts as a convenience header that pulls in shared functionality from the SharedCommon module.

**Location**: `/home/user/artdaq-database/artdaq-database/ConfigurationDB/common.h`

**Lines of Code**: 7

**Purpose**: Include common shared utilities and definitions for ConfigurationDB

## Dependencies

### Project Headers
- `"artdaq-database/SharedCommon/sharedcommon_common.h"` - Common utilities shared across all modules

## Contents

This header file serves as a passthrough, providing access to:
- TRACE logging macros
- Common type definitions
- Exception classes
- Utility functions from SharedCommon

## Usage

```cpp
#include "artdaq-database/ConfigurationDB/common.h"

// Now have access to:
// - TLOG() logging macros
// - trace_mode enums
// - confirm() debug assertions
// - Exception types
```

## Design Pattern

**Convenience Header Pattern**: Simplifies includes by providing a single header that brings in all commonly needed functionality.

**Benefits**:
1. Reduces repetitive includes
2. Centralizes common dependencies
3. Makes it easier to add/remove common functionality module-wide

## Related Files

- **sharedcommon_common.h** - Actual implementation of common utilities
- All ConfigurationDB implementation files use this header

---

**Documentation generated for artdaq-database ConfigurationDB module**
