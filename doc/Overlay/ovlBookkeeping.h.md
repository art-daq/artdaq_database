# ovlBookkeeping.h

**Path:** `artdaq-database/Overlay/ovlBookkeeping.h`

**Purpose:** This header defines the `ovlBookkeeping` overlay class, which manages database document record bookkeeping metadata including readonly/deleted flags, creation timestamp, and complete update history. This class is central to tracking record lifecycle, enforcing access control, and maintaining a complete audit trail of all modifications.


## Key Concepts

### Access Control Pattern

The bookkeeping class enforces access control through two state flags:
- **isReadonly** - Prevents any modifications to the document record when true
- **isDeleted** - Marks the document record as soft-deleted (not physically removed)

Operations check these flags and return failure if the document record is protected, enabling policy-based access control.

### Audit Trail

Every modification to a database document record is tracked in the `updates` array:
- **Event name** - What type of change occurred (e.g., "addConfiguration", "setVersion")
- **Timestamp** - When the change was made (ISO 8601 format)
- **Value** - The data that was changed (the actual value added or modified)

This provides a complete history of all changes for debugging, compliance, and rollback purposes.

### Soft Delete Pattern

Marking a document record as deleted (rather than physically deleting it):
- Preserves the complete audit trail
- Allows "undelete" operations by clearing the flag
- Maintains referential integrity with other document records
- Enables recovery scenarios and data forensics

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** References mutable JSON, modifies shared arrays
- **Locking:** None - callers must ensure single-threaded access

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/Overlay/common.h` | Module foundation types, constants, and comparison flags |
| `artdaq-database/Overlay/ovlKeyValue.h` | Base class for all overlay objects |
| `artdaq-database/Overlay/ovlKeyValueWithDefault.h` | Template for fields with default initialization |
| `artdaq-database/Overlay/ovlTimeStamp.h` | Timestamp handling overlay for creation time |
| `artdaq-database/Overlay/ovlUpdate.h` | Update entry overlay for audit trail entries |

## Classes/Structures

### `ovlBookkeeping`

**Brief:** Overlay class that manages document record bookkeeping metadata, providing access control via readonly/deleted flags and maintaining a complete audit trail of all modifications.

**Thread Safety:** Not thread-safe

#### Type Aliases

```cpp
using updates_t = array_t::container_type<ovlUpdate>;
```

**Brief:** Container type for holding overlay objects around update entries. This is a vector-like container of `ovlUpdate` objects.

#### Constructor

##### `ovlBookkeeping(object_t::key_type const& key, value_t& bookkeeping)`

**Brief:** Constructs a bookkeeping overlay, initializing missing fields with default values. If the bookkeeping JSON is incomplete, the constructor creates any missing fields.

**Parameters:**
- `key` - The JSON key (typically `"bookkeeping"`)
- `bookkeeping` - Reference to the JSON object containing bookkeeping data

**Preconditions:**
- `bookkeeping` must be a JSON object (type_t::OBJECT)

**Postconditions:**
- Missing fields are initialized with defaults:
  - `isdeleted` = `false`
  - `isreadonly` = `false`
  - `created` = current timestamp
  - `updates` = empty array

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` | If bookkeeping is not a JSON object |

**Thread Safety:** Unsafe

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlBookkeeping.h"
#include <iostream>

using namespace artdaq::database::overlay;

void accessBookkeeping(value_t& recordJson) {
  try {
    auto& bookkeepingJson = recordJson["bookkeeping"];
    ovlBookkeeping bookkeeping{"bookkeeping", bookkeepingJson};

    if (!bookkeeping.isReadonly()) {
      std::cout << "Record can be modified\n";
    } else {
      std::cout << "Record is readonly\n";
    }
  } catch (const std::runtime_error& e) {
    std::cerr << "Failed to access bookkeeping: " << e.what() << "\n";
  }
}
```

#### Special Member Functions

##### `ovlBookkeeping(ovlBookkeeping&&) = default`

**Brief:** Move constructor, allowing bookkeeping overlays to be moved efficiently.

##### `~ovlBookkeeping() = default`

**Brief:** Destructor. Non-virtual because this is a final class.

#### Methods

##### `isReadonly() -> bool&`

**Brief:** Returns a mutable reference to the readonly flag, allowing direct modification of the readonly state.

**Preconditions:**
- None

**Returns:** Reference to the `isreadonly` boolean field

**Postconditions:**
- None (returns reference to existing data)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If the isreadonly field is not a boolean |

**Thread Safety:** Unsafe

---

##### `isReadonly() const -> bool const&`

**Brief:** Returns a const reference to the readonly flag for read-only access.

**Preconditions:**
- None

**Returns:** Const reference to the `isreadonly` boolean field

**Postconditions:**
- None

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If the isreadonly field is not a boolean |

**Thread Safety:** Unsafe

---

##### `isDeleted() -> bool&`

**Brief:** Returns a mutable reference to the deleted flag, allowing direct modification of the deleted state.

**Preconditions:**
- None

**Returns:** Reference to the `isdeleted` boolean field

**Postconditions:**
- None (returns reference to existing data)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If the isdeleted field is not a boolean |

**Thread Safety:** Unsafe

---

##### `isDeleted() const -> bool const&`

**Brief:** Returns a const reference to the deleted flag for read-only access.

**Preconditions:**
- None

**Returns:** Const reference to the `isdeleted` boolean field

**Postconditions:**
- None

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If the isdeleted field is not a boolean |

**Thread Safety:** Unsafe

---

##### `markReadonly(bool const& state) -> bool&`

**Brief:** Sets the readonly flag to the specified state and returns the new value. Use this to lock or unlock a document record for modifications.

**Parameters:**
- `state` - The new readonly state (`true` to make readonly, `false` to allow modifications)

**Preconditions:**
- None

**Returns:** Reference to the updated readonly flag

**Postconditions:**
- The `isreadonly` field is set to `state`

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If the isreadonly field is not a boolean |

**Thread Safety:** Unsafe

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlBookkeeping.h"
#include <iostream>

using namespace artdaq::database::overlay;

void lockRecord(ovlBookkeeping& bookkeeping) {
  // Lock the document record to prevent further modifications
  bool newState = bookkeeping.markReadonly(true);
  std::cout << "Record is now " << (newState ? "locked" : "unlocked") << "\n";
}

void unlockRecord(ovlBookkeeping& bookkeeping) {
  // Unlock for modifications (requires appropriate permissions)
  bookkeeping.markReadonly(false);
}
```

---

##### `markDeleted(bool const& state) -> bool&`

**Brief:** Sets the deleted flag to the specified state and returns the new value. Use this for soft-delete operations.

**Parameters:**
- `state` - The new deleted state (`true` to mark deleted, `false` to undelete)

**Preconditions:**
- None

**Returns:** Reference to the updated deleted flag

**Postconditions:**
- The `isdeleted` field is set to `state`

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If the isdeleted field is not a boolean |

**Thread Safety:** Unsafe

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlBookkeeping.h"
#include <iostream>

using namespace artdaq::database::overlay;

void softDeleteRecord(ovlBookkeeping& bookkeeping) {
  if (bookkeeping.isReadonly()) {
    std::cerr << "Cannot delete readonly record\n";
    return;
  }
  bookkeeping.markDeleted(true);
  std::cout << "Record marked as deleted\n";
}
```

---

##### `postUpdate<T>(std::string const& name, T const& what) -> result_t`

**Brief:** Records a modification to the update history, tracking what changed and when. This template method accepts any overlay type and creates an audit trail entry.

**Template Parameters:**
- `T` - The overlay type being recorded (must be a smart pointer with a `value()` method returning `value_t const&`)

**Parameters:**
- `name` - Event name describing the update (e.g., `"addConfiguration"`, `"setVersion"`)
- `what` - The overlay object containing the changed data (typically a `unique_ptr`)

**Preconditions:**
- `name` must not be empty
- `what` must be a valid pointer (not null)
- Document record must not be readonly

**Returns:** `result_t` with success/failure status:
- On success: `{true, "Added"}`
- On failure: `{false, error_message}`

**Postconditions:**
- On success, a new entry is added to the updates array with:
  - `event`: the provided name
  - `timestamp`: current time
  - `value`: the changed data

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Errors returned via result_t |

**Thread Safety:** Unsafe

**Side Effects:**
- Modifies the updates array in the underlying JSON
- Recreates the internal overlay list to synchronize with JSON

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlBookkeeping.h"
#include "artdaq-database/Overlay/ovlVersion.h"
#include <iostream>

using namespace artdaq::database::overlay;

void trackVersionChange(ovlBookkeeping& bookkeeping, value_t& recordJson) {
  try {
    // Create an overlay for the version that was changed
    auto version = overlay<ovlVersion, std::string>(recordJson, "version");
    version->value("v2.0.0");

    // Record the change in bookkeeping
    auto result = bookkeeping.postUpdate("setVersion", version);
    if (!result.first) {
      std::cerr << "Failed to post update: " << result.second << "\n";
    } else {
      std::cout << "Version change recorded in audit trail\n";
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

---

##### `to_string() const -> std::string` [override]

**Brief:** Serializes the bookkeeping metadata to a formatted JSON string representation. Includes all fields: isreadonly, isdeleted, created timestamp, and all updates.

**Preconditions:**
- None

**Returns:** JSON string containing isreadonly, isdeleted, created timestamp, and all updates

**Postconditions:**
- None (read-only operation)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Returns empty or partial string on serialization errors |

**Thread Safety:** Unsafe

---

##### `operator==(ovlBookkeeping const& other) const -> result_t`

**Brief:** Compares two bookkeeping overlays for equality, respecting comparison mask flags. Provides detailed difference information.

**Parameters:**
- `other` - The bookkeeping overlay to compare against

**Preconditions:**
- None

**Returns:** `result_t` pair where:
- `first` is `true` if equal (or masked), `false` otherwise
- `second` contains detailed differences if not equal

**Postconditions:**
- None (read-only comparison)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Comparison does not throw |

**Thread Safety:** Unsafe

**Comparison Logic:**
1. If `DOCUMENT_COMPARE_MUTE_BOOKKEEPING` flag is set, returns success immediately
2. Compares `isreadonly` and `isdeleted` flags
3. Compares creation timestamps
4. If `DOCUMENT_COMPARE_MUTE_UPDATES` flag is set, skips update history comparison
5. Compares update arrays element-by-element

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlBookkeeping.h"
#include <iostream>

using namespace artdaq::database::overlay;

void compareBookkeeping(ovlBookkeeping const& bk1, ovlBookkeeping const& bk2) {
  auto result = bk1 == bk2;
  if (result.first) {
    std::cout << "Bookkeeping metadata matches\n";
  } else {
    std::cout << "Bookkeeping differs: " << result.second << "\n";
  }
}
```

## Type Aliases

### `ovlBookkeepingUPtr_t`

```cpp
using ovlBookkeepingUPtr_t = std::unique_ptr<ovlBookkeeping>;
```

**Brief:** Convenience type alias for unique pointer to ovlBookkeeping.

## Relationship to Other Components

### In the Overlay Hierarchy

```
ovlKeyValue (base)
     ^
     |
ovlBookkeeping (this class)
     |
     +-- ovlTimeStamp _created (creation timestamp)
     +-- updates_t _updates (array of ovlUpdate objects)
```

### Used By

- **ovlDatabaseRecord** - Contains bookkeeping as one of its components
- **All document record modification methods** - Check `isReadonly()` and `isDeleted()` before modifications

### JSON Structure

```json
{
  "bookkeeping": {
    "isreadonly": false,
    "isdeleted": false,
    "created": "2025-01-15T10:00:00Z",
    "updates": [
      {
        "event": "addConfiguration",
        "timestamp": "2025-01-15T10:30:00Z",
        "value": {"name": "DAQConfig", "assigned": "2025-01-15T10:30:00Z"}
      },
      {
        "event": "setVersion",
        "timestamp": "2025-01-15T10:31:00Z",
        "value": {"name": "v2_0_0", "assigned": "2025-01-15T10:31:00Z"}
      }
    ]
  }
}
```

## See Also

- [ovlBookkeeping.cpp](./ovlBookkeeping.cpp.md) - Implementation file
- [ovlKeyValue.h](./ovlKeyValue.h.md) - Base class
- [ovlUpdate.h](./ovlUpdate.h.md) - Update entry overlay
- [ovlTimeStamp.h](./ovlTimeStamp.h.md) - Timestamp overlay
- [ovlDatabaseRecord.h](./ovlDatabaseRecord.h.md) - Parent document record class

## Notes for Developers

### State Lifecycle

```
Created (readonly=false, deleted=false)
    |
    v
Modified (updates added via postUpdate)
    |
    +---> Marked Readonly (readonly=true)
    |         |
    |         v
    |     Immutable (all modifications fail with msg_IsReadonly)
    |
    +---> Marked Deleted (deleted=true)
          |
          v
      Soft-Deleted (document record hidden but not removed)
```

### Complete Usage Example

```cpp
#include "artdaq-database/Overlay/JSONDocumentOverlay.h"
#include <iostream>

using namespace artdaq::database::overlay;

void demonstrateBookkeeping(value_t& recordJson) {
  try {
    // Create the document record overlay
    ovlDatabaseRecord record{"record", recordJson};

    // Access bookkeeping
    auto& bookkeeping = *record.bookkeeping();

    // Check state before modifications
    if (bookkeeping.isReadonly()) {
      std::cerr << "Cannot modify: document record is readonly\n";
      return;
    }

    if (bookkeeping.isDeleted()) {
      std::cerr << "Cannot modify: document record is deleted\n";
      return;
    }

    // Make some changes and record them
    auto version = overlay<ovlVersion, std::string>(recordJson, "version");
    version->value("v2.0.0");

    auto result = bookkeeping.postUpdate("setVersion", version);
    if (!result.first) {
      std::cerr << "Failed to record update: " << result.second << "\n";
      return;
    }

    // Lock the document record when done
    bookkeeping.markReadonly(true);
    std::cout << "Document record locked after modification\n";

    // Display bookkeeping state
    std::cout << bookkeeping.to_string() << "\n";

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

### Performance Considerations

- **postUpdate:** O(1) append + O(n) list recreation for overlay synchronization
- **State checks (isReadonly, isDeleted):** O(1) direct boolean access
- **Comparison:** O(n) where n = number of updates in history
- **to_string():** O(n) - serializes all update entries

### Common Pitfalls

- **Forgetting Readonly Check:** Always check `isReadonly()` before modifications. The `postUpdate` method checks this, but other modifications may not.
- **Update History Growth:** The updates array grows unbounded; consider archiving strategy for long-lived document records.
- **Mask Dependencies:** Comparison results depend on global mask state. Be aware of mask settings in tests.

### Anti-patterns

```cpp
// DON'T: Modify without checking readonly
bookkeeping.markDeleted(true);  // May succeed even if intended to be immutable

// DO: Check readonly first
if (!bookkeeping.isReadonly()) {
  bookkeeping.markDeleted(true);
} else {
  std::cerr << "Cannot modify readonly document record\n";
}

// DON'T: Modify updates array directly
auto& json = bookkeeping.value();
json["updates"].push_back(...);  // Breaks overlay synchronization!

// DO: Use postUpdate
auto result = bookkeeping.postUpdate("eventName", overlayPtr);
```

### Best Practices

1. **Always check readonly/deleted** before modifications
2. **Use markReadonly** for production document records to prevent accidental changes
3. **Post updates** for all significant changes to maintain audit trail
4. **Provide meaningful event names** in postUpdate (e.g., "addConfiguration" not "update")
5. **Do not manually modify updates array** - use postUpdate to maintain consistency
6. **Consider masking in tests** - use `DOCUMENT_COMPARE_MUTE_BOOKKEEPING` to simplify test comparisons
7. **Handle errors from postUpdate** - check the result_t before proceeding

### Important Notes

- **Final class** - Cannot be derived from (use composition instead)
- **Central to access control** - All modification operations should check bookkeeping state
- **Updates array grows unbounded** - Consider periodic archiving for long-lived document records
- **Comparison supports multiple masking levels** - Granular control over what is compared
- **Template method enables type-safe update posting** - Works with any overlay type
- **Creation timestamp auto-generated** - Set to current time if missing during construction
- **Readonly enforcement is policy-based** - Not enforced at JSON level, requires caller cooperation
