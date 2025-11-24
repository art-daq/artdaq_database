# ovlDatabaseRecord.cpp

## File Overview

Implementation of the ovlDatabaseRecord class, providing the concrete implementations for all database record operations including component management, state control, and comparison.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/ovlDatabaseRecord.cpp`

## Implementation Highlights

See ovlDatabaseRecord.h.md for complete documentation. The implementation provides:

### Component Management
- Add/remove operations for configurations, entities, aliases, runs
- Readonly/deleted state checking before all modifications
- Automatic update posting to bookkeeping
- Swap operations for efficient component replacement

### State Management
- Readonly and deleted flag manipulation
- Combined state queries (isReadonlyOrDeleted)
- State enforcement in all modification operations

### Comparison
- Comprehensive component-by-component comparison
- Aggregated error messages showing all differences
- Supports multiple levels of masking

### Version and Collection Management
- Duplicate detection (ignores unchanged values)
- Swap-based replacement
- Update tracking

## Key Implementation Details

All modification methods follow this pattern:
1. Validate input (confirm not null)
2. Check readonly/deleted state
3. Perform operation on component list
4. Check operation result
5. Post update to bookkeeping
6. Return result

This ensures consistency, access control, and complete audit trails.

## Related Files

- **ovlDatabaseRecord.h** - Class declaration
- All component header files
