# JSONDocumentOverlay.h

**Path:** `artdaq-database/Overlay/JSONDocumentOverlay.h`

**Purpose:** This is a minimal convenience header that serves as the main entry point for the Overlay module. It aggregates the primary overlay functionality by including the database document record interface and common shared utilities, providing external code with a single include point to access complete database document record functionality through the overlay abstraction.


## Key Concepts

### The Overlay Pattern

The Overlay module implements the **Overlay Pattern**, where C++ objects provide type-safe, convenient access to underlying JSON data structures without data duplication. The overlay objects reference JSON directly rather than copying it, enabling efficient in-place manipulation.

Key characteristics:
- **Zero-copy:** Overlays hold references to JSON, not copies
- **Type-safe:** C++ types and methods replace raw JSON access
- **Hierarchical:** Overlays compose to represent document structure
- **Mutable:** Changes through overlays modify the underlying JSON

### Facade Pattern

This header implements the **Facade Pattern**:
- **Simplified Interface** - Users include one header, not dozens of internal files
- **Reduced Compilation Dependencies** - Changes to internal overlay files do not force recompilation of external code
- **Clear API Boundary** - Establishes what is public versus internal to the module
- **Future-Proofing** - Internal reorganization does not affect external code

### Zero-Copy Design

- **No Data Duplication** - Overlays reference JSON structures directly via references
- **Type Safety** - C++ types and methods provide safe access to JSON values
- **Lazy Evaluation** - Child overlay components are created on-demand
- **Efficient Updates** - Changes modify JSON in-place without copying

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Overlays reference mutable JSON and share a static comparison mask
- **Locking:** None - callers must ensure single-threaded access

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/Overlay/ovlDatabaseRecord.h` | Main database document record class providing complete record representation |
| `artdaq-database/SharedCommon/sharedcommon_common.h` | Common utilities, standard library includes, and shared types |

## Classes/Structures

This header does not define any new classes or functions. It serves purely as an aggregation point, providing access to the full overlay hierarchy.

### Transitively Available Types

When you include `JSONDocumentOverlay.h`, these types become available:

**Main Record Type:**
- `ovlDatabaseRecord` - The complete database document record overlay (root of the overlay hierarchy)

**Component Overlays:**
- `ovlDocument` - Provides access to the data and metadata sections
- `ovlBookkeeping` - Manages readonly/deleted flags and update history
- `ovlOrigin` - Tracks document provenance (format, source)

**Simple Field Overlays:**
- `ovlId` - Provides access to the document ID field
- `ovlVersion` - Provides access to the version string
- `ovlCollection` - Provides access to the collection name
- `ovlChangeLog` - Provides access to the changelog string

**List Overlays:**
- `ovlEntities` - List of configuration entity references
- `ovlConfigurations` - List of configuration references
- `ovlRuns` - List of run assignments
- `ovlComments` - List of comment entries
- `ovlAliases` - List of alias assignments
- `ovlAttachments` - List of attachment references

**Template Classes:**
- `ovlFixedList<T>` - Fixed-position list overlay
- `ovlMovableList<T>` - Reorderable list overlay

**Factory Function:**
- `overlay<OVL, T>()` - Creates overlay instances from JSON values

## Relationship to Other Components

### In the Overlay Module

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

### Comparison with common.h

| Aspect | common.h | JSONDocumentOverlay.h |
|--------|----------|----------------------|
| **Purpose** | Internal module foundation | External API entry point |
| **Audience** | Overlay module implementation files | External code using the overlay |
| **Contents** | Types, constants, flags, utilities | Just the main record interface |
| **Include Frequency** | Included by every overlay `.cpp` file | Included by external code once |

### Usage by Other Modules

- **ConfigurationDB** - Uses this header to access and manipulate database document records
- **Test Files** - Include this header to test record manipulation

## See Also

- [ovlDatabaseRecord.h](./ovlDatabaseRecord.h.md) - The main record class included by this header
- [common.h](./common.h.md) - Internal module foundation
- [ovlKeyValue.h](./ovlKeyValue.h.md) - Base class for all overlays

## Notes for Developers

### When to Use This Header

**Use JSONDocumentOverlay.h when:**
- Using database document records from external code (outside the Overlay module)
- You need access to the complete overlay hierarchy
- You want a clean, stable API that will not change with internal refactoring

**Use common.h when:**
- Implementing new overlay classes within the Overlay module
- Working inside the Overlay module
- You need access to comparison flags and error constants

### Example Usage

```cpp
#include "artdaq-database/Overlay/JSONDocumentOverlay.h"
#include <iostream>

using namespace artdaq::database::overlay;

void manipulateRecord(value_t& recordJson) {
  try {
    // Create overlay wrapping the JSON
    ovlDatabaseRecord record{"record", recordJson};

    // Access bookkeeping
    if (record.bookkeeping()->isReadonly()) {
      std::cerr << "Record is readonly, cannot modify\n";
      return;
    }

    // Access document data
    auto& document = record.document();

    // Set version
    record.version()->value("v2.0.0");

    // Add a comment
    record.addComment("Updated configuration for new run");

    // Mark as readonly when done
    record.bookkeeping()->markReadonly(true);

    std::cout << "Record successfully modified\n";
  } catch (const std::runtime_error& e) {
    std::cerr << "Failed to manipulate record: " << e.what() << "\n";
  } catch (const std::bad_cast& e) {
    std::cerr << "Type mismatch in JSON structure: " << e.what() << "\n";
  }
}
```

### Creating and Accessing Document Records

```cpp
#include "artdaq-database/Overlay/JSONDocumentOverlay.h"
#include "artdaq-database/DataFormats/Json/json_reader.h"
#include <iostream>

using namespace artdaq::database::overlay;
using namespace artdaq::database::json;

void processDocumentRecord() {
  // Parse JSON from string
  std::string jsonStr = R"({
    "document": {
      "data": {"threshold": 100},
      "metadata": {"name": "DAQConfig"}
    },
    "bookkeeping": {
      "isreadonly": false,
      "isdeleted": false
    }
  })";

  value_t recordJson;
  try {
    JsonReader reader;
    if (!reader.read(jsonStr, recordJson)) {
      std::cerr << "Failed to parse JSON\n";
      return;
    }

    // Create overlay
    ovlDatabaseRecord record{"record", recordJson};

    // Check state before modification
    if (record.bookkeeping()->isDeleted()) {
      std::cerr << "Cannot modify deleted record\n";
      return;
    }

    // Access nested data
    auto& docOverlay = record.document();
    std::cout << "Document overlay created successfully\n";

    // Compare two records
    value_t otherJson = recordJson;  // Copy for comparison
    ovlDatabaseRecord otherRecord{"record", otherJson};

    auto result = record == otherRecord;
    if (result.first) {
      std::cout << "Records are equal\n";
    } else {
      std::cout << "Records differ: " << result.second << "\n";
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

### Common Pitfalls

- **Lifetime Management:** The overlay holds a reference to JSON. The JSON must outlive the overlay. Returning an overlay to a local JSON variable causes undefined behavior.
- **Thread Safety:** Do not access overlays from multiple threads without synchronization.
- **Static Mask:** The comparison mask is global and affects all comparisons. Set it once at program startup.

### Anti-patterns

```cpp
// DON'T: JSON destroyed before overlay
ovlDatabaseRecord& createRecord() {
  value_t json = object_t{};
  return ovlDatabaseRecord{"record", json};  // Dangling reference!
}

// DO: JSON outlives overlay
void useRecord(value_t& json) {
  ovlDatabaseRecord record{"record", json};
  // Use record...
} // record destroyed before json

// DON'T: Return overlay by value (still references local JSON)
ovlDatabaseRecord createRecordBad() {
  value_t json = object_t{};
  return ovlDatabaseRecord{"record", json};  // json destroyed after return!
}

// DO: Accept JSON by reference, work with overlay locally
void processRecord(value_t& externalJson) {
  ovlDatabaseRecord record{"record", externalJson};
  // Safe - externalJson outlives record
  // ... do work ...
}
```

### Header Guard Note

The header guard name `_ARTDAQ_DATABASE_JSONUTILS_JSONDOCUMENT_OVERLAY_H_` contains "JSONUTILS" which is a legacy naming convention from when this code was organized differently. The guard name is kept for backward compatibility.

### Best Practices

1. **Include only JSONDocumentOverlay.h** for external code, not individual overlay headers
2. **Work with ovlDatabaseRecord** as the primary interface
3. **Check bookkeeping state** before modifying records
4. **Use accessor methods** rather than accessing JSON directly
5. **Ensure JSON lifetime** exceeds overlay lifetime
6. **Handle exceptions** - overlay operations can throw on type mismatches
7. **Set comparison mask early** if you need non-default comparison behavior
