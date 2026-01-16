# ovlId.h

**Path:** `artdaq-database/Overlay/ovlId.h`

**Purpose:** Defines the `ovlId` overlay class for managing database document unique identifiers (Object IDs or OIDs). This class handles OID generation, validation, and access for database documents, ensuring each document has a unique identifier compatible with database backends like MongoDB. The OID is automatically generated during construction if not present in the JSON.


## Key Concepts

### Auto-Generated IDs

If no OID exists when the overlay is created, one is automatically generated using the `generate_oid()` utility function. This ensures every document has a unique identifier without requiring explicit ID assignment by the calling code. The auto-generation happens during construction, making it impossible to create a document without an ID.

### MongoDB OID Compatibility

The OID format is designed to be compatible with MongoDB ObjectId format, typically a 24-character hexadecimal string. This allows seamless storage in MongoDB while also working with FileSystemDB (file-based storage). The format is consistent across all storage providers.

### Comparison Masking

OID comparison can be masked using `DOCUMENT_COMPARE_MUTE_OUIDS`, which is useful when comparing documents where IDs are expected to differ but content should match. For example:
- Comparing a template document with a stored document
- Verifying document content after import/export
- Testing document equality ignoring database-assigned IDs

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not supported; external synchronization required
- **Locking:** None

All methods access mutable JSON data through references without synchronization. The `useCompareMask()` function uses static storage, making comparison mask changes affect all threads.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/Overlay/common.h` | Module foundation types, constants, `generate_oid()` function declaration, comparison flags |
| `artdaq-database/Overlay/ovlKeyValue.h` | Base class providing JSON key-value access and utility methods |

## Classes/Structures

### `ovlId`

A type-safe overlay for database document identifiers, inheriting from `ovlKeyValue`. Provides OID access, generation, and maskable comparison capabilities. The overlay ensures every document has a valid unique identifier.

**Thread Safety:** Not thread-safe; references mutable JSON data.

#### Constructor

##### `ovlId(object_t::key_type const& key, value_t& oid)`

**Brief:** Constructs an overlay for an ID JSON object, auto-generating an OID if none exists. The initialization ensures the document will always have a valid unique identifier.

**Parameters:**
- `key` - The JSON key under which this ID is stored (typically "id" or "_id")
- `oid` - Reference to the JSON object that will contain the OID field

**Preconditions:**
- `oid` must be a valid JSON object or convertible to one

**Postconditions:**
- The JSON object contains an `oid` field with a valid identifier
- If `oid` field was missing, a new unique ID has been generated
- The `_initOK` flag is set to `true` on successful initialization

**Throws:**
| Exception | Condition |
|-----------|-----------|
| Assertion failure | If `oid` is not an OBJECT type (via `confirm()` macro) |

**Thread Safety:** Not applicable (construction)

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlId.h"
#include <iostream>

using namespace artdaq::database::overlay;

void createDocumentWithId() {
  try {
    // Create empty JSON - OID will be auto-generated
    value_t idJson = object_t{};

    // OID auto-generated during construction
    ovlId id("id", idJson);

    std::cout << "Generated OID: " << id.oid() << std::endl;
    // Output example: Generated OID: 507f1f77bcf86cd799439011
  } catch (const std::exception& e) {
    std::cerr << "Error creating ID: " << e.what() << std::endl;
  }
}
```

#### Special Member Functions

##### `ovlId(ovlId&&) = default`

**Brief:** Default move constructor allowing transfer of overlay ownership. The moved-from object should not be used after the move.

**Thread Safety:** Not applicable (construction)

##### `~ovlId() = default`

**Brief:** Default destructor; no special cleanup required as the overlay does not own the underlying JSON data.

**Thread Safety:** Not applicable (destruction)

#### Methods

##### `newId() -> bool`

**Brief:** Forcibly generates a new OID, replacing any existing OID value. Use this when you need to regenerate a document's ID, such as when cloning a document.

**Preconditions:**
- The overlay must be properly initialized

**Returns:** `true` on success.

**Postconditions:**
- The `oid` field contains a newly generated unique identifier
- Previous OID value is overwritten and lost

**Throws:**
| Exception | Condition |
|-----------|-----------|
| Assertion failure | If internal state is invalid (via `confirm()` macro) |

**Thread Safety:** Unsafe - modifies shared mutable state

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlId.h"
#include <iostream>

using namespace artdaq::database::overlay;

void regenerateId() {
  try {
    value_t idJson = object_t{};
    ovlId id("id", idJson);

    std::cout << "Original: " << id.oid() << std::endl;

    bool success = id.newId();
    if (success) {
      std::cout << "New: " << id.oid() << std::endl;
      // IDs will be different - new unique ID generated
    }
  } catch (const std::exception& e) {
    std::cerr << "Error regenerating ID: " << e.what() << std::endl;
  }
}
```

##### `oid() -> std::string&`

**Brief:** Returns a mutable reference to the OID string value, allowing direct read and write access to the identifier.

**Preconditions:**
- The overlay must be properly initialized with an OID field

**Returns:** Reference to the OID string, allowing direct modification.

**Postconditions:**
- Modifications through the returned reference affect the underlying JSON

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If the `oid` field is not a string type |
| `std::out_of_range` | If the `oid` field does not exist |

**Thread Safety:** Unsafe - returns mutable reference

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlId.h"

using namespace artdaq::database::overlay;

void accessOid(ovlId& id) {
  try {
    // Read OID
    std::cout << "Current OID: " << id.oid() << std::endl;

    // Direct modification (use with caution)
    id.oid() = "custom_id_value_24chars";
  } catch (const std::exception& e) {
    std::cerr << "Error accessing OID: " << e.what() << std::endl;
  }
}
```

##### `oid(std::string const& id) -> std::string&`

**Brief:** Setter method with an unusual empty-check precondition. **WARNING:** Due to the implementation requiring an empty string, this method is effectively unusable for setting specific OID values.

**Parameters:**
- `id` - The new OID value to set (must be empty string due to assertion)

**Preconditions:**
- `id` must be empty (assertion: `confirm(id.empty())`)

**Returns:** Reference to the OID string after assignment.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| Assertion failure | If `id` is not empty |

**Thread Safety:** Unsafe

**Note:** This unusual behavior appears to be a bug in the implementation. The assertion `confirm(id.empty())` requires an empty string, then assigns that empty string to the OID. Use `newId()` to forcibly replace an OID, or assign directly to the reference returned by `oid()`:

```cpp
// Instead of: id.oid("new-value");  // Will fail assertion
// Use:        id.oid() = "new-value";  // Works correctly
```

##### `operator==(ovlId const& other) const -> result_t`

**Brief:** Compares this ID with another, respecting the `DOCUMENT_COMPARE_MUTE_OUIDS` mask. When masking is enabled, IDs are considered equal regardless of actual values.

**Parameters:**
- `other` - The ID to compare against

**Preconditions:**
- Both IDs must have valid underlying JSON data

**Returns:** `result_t` where `first` is `true` if IDs match (or masking is enabled), `false` otherwise.

**Postconditions:**
- Neither ID is modified

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Comparison does not throw |

**Thread Safety:** Unsafe - reads mutable JSON and static comparison mask

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlId.h"
#include <iostream>

using namespace artdaq::database::overlay;
using namespace artdaq::database::result;

void compareIds() {
  try {
    value_t json1 = object_t{};
    ovlId id1("id", json1);

    value_t json2 = object_t{};
    ovlId id2("id", json2);  // Different auto-generated OID

    // Full comparison - will fail (different OIDs)
    useCompareMask(0);
    auto result = id1 == id2;
    if (!result.first) {
      std::cout << "IDs differ (expected)" << std::endl;
    }

    // Masked comparison - will succeed (OIDs ignored)
    useCompareMask(DOCUMENT_COMPARE_MUTE_OUIDS);
    result = id1 == id2;
    if (result.first) {
      std::cout << "IDs match with masking" << std::endl;
    }
  } catch (const std::exception& e) {
    std::cerr << "Error comparing IDs: " << e.what() << std::endl;
  }
}
```

## Type Aliases

```cpp
using ovlIdUPtr_t = std::unique_ptr<ovlId>;
```

Unique pointer type for managing `ovlId` instances. Used when ownership transfer is needed.

## Private Members

### `_initOK`

**Type:** `bool`

**Brief:** Tracks whether initialization completed successfully during construction. Set by the `init()` method.

### `init(value_t& parent) -> bool`

**Brief:** Private initialization method that ensures the OID field exists, generating one if missing. Called during construction.

**Called by:** Constructor (via member initializer list)

**Steps:**
1. Confirm parent is OBJECT type
2. If `oid` field missing, generate new OID
3. Confirm exactly one `oid` field exists
4. Return `true` on success

**Throws:** Assertion failure on type mismatch or initialization failure

## Relationship to Other Components

### In the Overlay Hierarchy

```
ovlKeyValue (base)
     ^
     |
ovlId (this class)
```

### Used By

- **ovlDatabaseRecord** - Contains `_id` as one of its 14 components, ensuring every document has a unique identifier.

### JSON Structure

```json
{
  "id": {
    "oid": "507f1f77bcf86cd799439011"
  }
}
```

The nested structure (`id` containing `oid`) allows for future extension with additional ID-related metadata.

## Notes for Developers

### Usage Example

```cpp
#include "artdaq-database/Overlay/ovlId.h"
#include "artdaq-database/Overlay/ovlDatabaseRecord.h"
#include <iostream>

using namespace artdaq::database::overlay;
using namespace artdaq::database::result;

void demonstrateIdUsage() {
  try {
    // Create document record (auto-generates ID)
    value_t recordJson = object_t{};
    ovlDatabaseRecord record{recordJson};

    // Access ID through record
    std::cout << "Document OID: " << record.id().oid() << std::endl;

    // Generate new ID for cloning
    record.id().newId();
    std::cout << "Clone OID: " << record.id().oid() << std::endl;

    // Compare documents with and without ID masking
    value_t otherJson = object_t{};
    ovlDatabaseRecord other{otherJson};

    // Without masking - IDs will differ
    useCompareMask(0);
    auto result = record.id() == other.id();
    std::cout << "Without mask: " << (result.first ? "equal" : "different") << std::endl;

    // With masking - IDs ignored
    useCompareMask(DOCUMENT_COMPARE_MUTE_OUIDS);
    result = record.id() == other.id();
    std::cout << "With mask: " << (result.first ? "equal" : "different") << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}
```

### Common Pitfalls

- **Unusual Setter Behavior:** The `oid(const std::string&)` setter has an assertion that requires an empty string argument, making it unusable for setting specific OID values; use direct assignment to `oid()` reference instead.
- **Auto-Generation:** OID is auto-generated during construction if missing; don't assume a document has no ID after creation.
- **Comparison Masking:** Remember to set the comparison mask appropriately before comparing documents. The mask is global and affects all comparisons.
- **Static Mask:** `useCompareMask()` uses static storage, so mask changes affect all threads.

### Anti-patterns

```cpp
// DON'T: Try to set OID with non-empty string via setter
id.oid("new-id-value");  // Assertion failure!

// DO: Use newId() or direct assignment
id.newId();  // Generate new unique ID
// or
id.oid() = "specific-id-value";  // Direct assignment to reference

// DON'T: Forget to reset comparison mask after testing
useCompareMask(DOCUMENT_COMPARE_MUTE_OUIDS);
// ... test code ...
// Other code may now compare incorrectly

// DO: Reset mask after use
useCompareMask(DOCUMENT_COMPARE_MUTE_OUIDS);
// ... test code ...
useCompareMask(0);  // Reset to full comparison
```

## See Also

- [ovlId.cpp](./ovlId.cpp.md) - Implementation file with method details
- [ovlKeyValue.h](./ovlKeyValue.h.md) - Base class providing field access
- [common.h](./common.h.md) - `generate_oid()` function and comparison masks
- [ovlDatabaseRecord.h](./ovlDatabaseRecord.h.md) - Uses `ovlId` as a component
