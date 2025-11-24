# JSONDocumentOverlay.h

## File Overview

This is a minimal convenience header that serves as the main entry point for the Overlay module. It aggregates the primary overlay functionality by including the database record interface and common shared utilities. This header is designed to be included by external code that needs to work with database records through the overlay abstraction.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/JSONDocumentOverlay.h`

## Purpose

This header serves as a **facade** for the Overlay module, providing:

1. **Single Include Point** - External code can include just this one header to access database record functionality
2. **Minimal Dependencies** - Only includes the essential database record interface
3. **Clean API Surface** - Hides the internal complexity of individual overlay components

## Dependencies

```cpp
#include "artdaq-database/Overlay/ovlDatabaseRecord.h"
#include "artdaq-database/SharedCommon/sharedcommon_common.h"
```

### ovlDatabaseRecord.h
The main overlay interface - provides the `ovlDatabaseRecord` class which is the complete representation of a database record with all its components (document, bookkeeping, origin, etc.).

### sharedcommon_common.h
Provides common utilities, standard library includes, and shared types used throughout the project.

## Header Guard

```cpp
#ifndef _ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENT_OVERLAY_H_
#define _ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENT_OVERLAY_H_
// ... includes ...
#endif
```

**Note**: The header guard name contains `JSONUTILS` which may be a legacy naming convention from when this code was organized differently.

## Usage

### External Code Pattern

External code that needs to work with database records should include this header:

```cpp
#include "artdaq-database/Overlay/JSONDocumentOverlay.h"

using namespace artdaq::database::overlay;

void processRecord(value_t& jsonRecord) {
  // Create overlay wrapper around JSON data
  ovlDatabaseRecord record{jsonRecord};

  // Access document data
  auto& document = record.document();
  auto& data = document.data();

  // Access metadata
  auto& bookkeeping = record.bookkeeping();
  if (!bookkeeping.isReadonly()) {
    // Modify the record
  }

  // Access origin information
  auto& origin = record.origin();
  std::cout << "Format: " << origin.format() << std::endl;
}
```

### What You Get

By including this header, you get access to:

- **ovlDatabaseRecord** - The complete database record wrapper
- **All transitively included overlay classes**:
  - `ovlDocument`, `ovlBookkeeping`, `ovlOrigin`
  - `ovlId`, `ovlVersion`, `ovlCollection`
  - `ovlEntities`, `ovlConfigurations`, `ovlRuns`
  - `ovlComments`, `ovlAliases`, `ovlAttachments`
  - And all their supporting types

## Design Rationale

### Facade Pattern

This header implements the **Facade Pattern**:

**Benefits**:
1. **Simplified Interface** - Users don't need to know about internal overlay components
2. **Reduced Compilation Dependencies** - Changes to internal overlay files don't force recompilation of external code (in many cases)
3. **Clear API Boundary** - Establishes what is public vs. internal to the module
4. **Future-Proofing** - Internal reorganization doesn't affect external code

### Minimal Header

The header is intentionally minimal (only 7 lines):
- **Fast Compilation** - Less code to parse
- **Clear Purpose** - Obviously a facade/convenience header
- **Easy Maintenance** - Rare changes needed

## Comparison with common.h

### common.h vs. JSONDocumentOverlay.h

| Aspect | common.h | JSONDocumentOverlay.h |
|--------|----------|----------------------|
| **Purpose** | Internal module foundation | External API entry point |
| **Audience** | Overlay module implementation files | External code using the overlay |
| **Contents** | Types, constants, flags, utilities | Just the main record interface |
| **Include Frequency** | Included by every overlay .cpp file | Included by external code |

### When to Use Which

**Use common.h when**:
- Implementing new overlay classes
- Working inside the Overlay module
- Need access to comparison flags and error constants

**Use JSONDocumentOverlay.h when**:
- Using database records from external code
- Don't need internal overlay implementation details
- Want a clean, stable API

## Related Files

### Directly Included
- **ovlDatabaseRecord.h** - The main database record class
- **sharedcommon_common.h** - Shared utilities and common includes

### Transitively Available
When you include JSONDocumentOverlay.h, these are also available:
- **common.h** - Via ovlDatabaseRecord.h
- **ovlKeyValue.h** - Base overlay class
- **ovlDocument.h**, **ovlBookkeeping.h**, **ovlOrigin.h** - Component overlays
- **ovlFixedList.h**, **ovlMovableList.h** - List templates
- All other overlay component headers

## Usage in the Project

### Configuration API

The Configuration API uses this header to access database records:

```cpp
#include "artdaq-database/Overlay/JSONDocumentOverlay.h"

// API functions work with ovlDatabaseRecord
result_t storeConfiguration(ovlDatabaseRecord& record);
result_t retrieveConfiguration(std::string const& id, ovlDatabaseRecord& record);
```

### Testing

Test files include this header to test record manipulation:

```cpp
#include "artdaq-database/Overlay/JSONDocumentOverlay.h"

TEST(OverlayTest, DatabaseRecordCreation) {
  value_t json = parseJSON("...");
  ovlDatabaseRecord record{json};

  EXPECT_FALSE(record.bookkeeping().isReadonly());
  EXPECT_EQ(record.version().string_value(), "v1_0_0");
}
```

## Architecture Notes

### Overlay Module Structure

The Overlay module has a layered architecture:

```
External Code
     |
     | includes JSONDocumentOverlay.h
     v
ovlDatabaseRecord  (aggregates all components)
     |
     +-- ovlDocument (data + metadata)
     +-- ovlBookkeeping (tracking info)
     +-- ovlOrigin (provenance)
     +-- ovlId, ovlVersion, ovlCollection
     +-- Lists: ovlConfigurations, ovlEntities, etc.
     |
     v
ovlKeyValue (base class for all overlays)
     |
     v
JSON AST (actual data storage)
```

### Zero-Copy Design

The overlay pattern used here provides:
- **No Data Duplication** - Overlays reference JSON structures directly
- **Type Safety** - C++ types and methods over raw JSON
- **Lazy Evaluation** - Components created on-demand
- **Efficient Updates** - Changes modify JSON in-place

## Best Practices

### External Code

1. **Include only JSONDocumentOverlay.h**, not individual overlay headers
2. **Work with ovlDatabaseRecord** as the primary interface
3. **Check bookkeeping state** before modifying records
4. **Use accessor methods** rather than accessing JSON directly

### Module Implementation

1. **Don't include JSONDocumentOverlay.h** in other overlay .h files (circular dependencies)
2. **Use common.h** for internal module code
3. **Keep this header minimal** - don't add convenience functions here
4. **Update only when ovlDatabaseRecord changes** significantly

## Future Considerations

### Potential Enhancements

1. **Version Namespace** - Consider adding version namespace for API stability:
   ```cpp
   namespace artdaq::database::overlay::v1 {
     // Versioned API
   }
   ```

2. **Forward Declarations** - Could add forward declarations for common types to reduce compile-time dependencies

3. **Convenience Functions** - Could add free functions for common operations:
   ```cpp
   ovlDatabaseRecordUPtr_t createNewRecord();
   result_t compareRecords(ovlDatabaseRecord const&, ovlDatabaseRecord const&);
   ```

## Notes

- This is the recommended include for external code working with database records
- The header name suggests historical organization (JSONUTILS vs. Overlay)
- Despite the minimal content, this header is architecturally important as an API boundary
- Including this header transitively includes most of the Overlay module
- The header is header-guard protected but has no namespace declarations of its own
