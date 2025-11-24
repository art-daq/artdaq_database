# ovlId.cpp

## File Overview

Implementation of ovlId class methods for OID management and validation.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/ovlId.cpp`

## Implementation Details

See ovlId.h.md for complete documentation. Key points:

- Auto-generates OID on construction if missing
- Uses `generate_oid()` utility function
- Validates OID existence with assertions
- Supports OID regeneration via `newId()`
- Comparison respects DOCUMENT_COMPARE_MUTE_OUIDS mask

## Related Files

- **ovlId.h** - Class declaration
