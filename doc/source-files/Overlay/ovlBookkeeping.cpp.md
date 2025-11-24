# ovlBookkeeping.cpp

## File Overview

Implementation of ovlBookkeeping class providing record state management and update history tracking.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/ovlBookkeeping.cpp`

## Implementation Highlights

See ovlBookkeeping.h.md for complete documentation. Key implementations:

- **Initialization**: Creates default false values for readonly/deleted flags
- **State Management**: Simple boolean flag accessors and setters
- **Update Creation**: Creates overlay wrappers for update array elements
- **Timestamp Mapping**: Ensures creation timestamp exists
- **Comparison**: Comprehensive with multiple masking levels
- **Serialization**: Formats all bookkeeping components as JSON

## Related Files

- **ovlBookkeeping.h** - Class declaration
