# ovlId.cpp

**Path:** `artdaq-database/Overlay/ovlId.cpp`

**Implements:** [ovlId.h](./ovlId.h.md)

**Purpose:** Provides the implementation for the `ovlId` overlay class, including OID auto-generation during initialization, accessor methods, new ID generation, and maskable comparison for database document unique identifiers. The implementation ensures every document has a valid unique identifier through automatic generation during construction.

## Implementation Overview

The implementation focuses on ensuring every document has a valid unique identifier. During construction, the `init()` method checks for an existing OID and generates one if missing using `generate_oid()`. The comparison operator respects the global comparison mask to allow ignoring OIDs when comparing document content.

## Key Algorithms

### OID Initialization

The `init()` method ensures the JSON object has a valid OID:

**Steps:**
1. Confirm parent is an OBJECT type using `confirm()` assertion
2. Get reference to the object value
3. Check if `oid` field exists (`obj.count(jsonliteral::oid) == 0`)
4. If missing, generate new OID using `generate_oid()` and assign it
5. Confirm exactly one `oid` field exists
6. Return `true` on success

**Why this approach:** Guarantees every document has an ID without requiring explicit initialization by calling code. The check-then-create pattern ensures idempotency.

### Comparison with Masking

The comparison operator checks the global mask before comparing:

**Steps:**
1. Get current mask via `useCompareMask()`
2. Check if `DOCUMENT_COMPARE_MUTE_OUIDS` bit is set using bitwise AND
3. If set, return `Success()` immediately (IDs are considered equal)
4. Otherwise, perform direct comparison via `self() == other.self()`

**Why this approach:** Allows flexible comparison strategies:
- Full comparison for exact document matching
- Masked comparison for content-focused comparisons ignoring auto-generated IDs

## Internal Functions

### Constructor

```cpp
ovlId::ovlId(object_t::key_type const& key, value_t& oid)
    : ovlKeyValue(key, oid), _initOK(init(oid)) {}
```

**Brief:** Initializes base class and calls `init()` to ensure OID exists. The `_initOK` member captures the initialization result.

**Called by:** Factory functions, `ovlDatabaseRecord` constructor

**Initialization order:**
1. Base class `ovlKeyValue` with key and value reference
2. `_initOK` set by calling `init(oid)` - ensures OID field exists

### `init(value_t& parent) -> bool`

```cpp
bool ovlId::init(value_t& parent) try {
  confirm(type(parent) == type_t::OBJECT);

  auto& obj = object_value();

  if (obj.count(jsonliteral::oid) == 0) {
    obj[jsonliteral::oid] = generate_oid();
  }

  confirm(obj.count(jsonliteral::oid) == 1);

  return true;
} catch (...) {
  confirm(false);
  throw;
}
```

**Brief:** Initializes the OID field, generating a new ID if none exists. This is the core initialization logic.

**Called by:** Constructor (via member initializer list)

**Purpose:** Ensures every document has a unique identifier without requiring explicit initialization.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| Assertion failure | If parent is not OBJECT type |
| Assertion failure | If OID field count is not exactly 1 after initialization |
| Any exception | Re-thrown after logging assertion failure |

### `newId() -> bool`

```cpp
bool ovlId::newId() try {
  auto& obj = object_value();
  obj[jsonliteral::oid] = generate_oid();
  confirm(obj.count(jsonliteral::oid) == 1);

  return true;
} catch (...) {
  confirm(false);
  throw;
}
```

**Brief:** Forcibly generates a new OID, replacing any existing value. Unlike `init()`, this always generates a new ID.

**Called by:** Code that needs to regenerate document IDs (e.g., cloning documents)

**Purpose:** Allows regeneration of document IDs when needed, such as when creating copies of existing documents.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| Assertion failure | If OID field count is not exactly 1 after assignment |
| Any exception | Re-thrown after logging assertion failure |

### Accessor Methods

```cpp
std::string& ovlId::oid() {
  return value_as<std::string>(jsonliteral::oid);
}

std::string& ovlId::oid(std::string const& id) {
  confirm(id.empty());  // Note: unusual precondition - likely a bug
  oid() = id;
  return oid();
}
```

**Brief:** Provides access to the OID string value. The parameterized version has an unusual requirement.

**Note on `oid(const std::string&)`:** The `confirm(id.empty())` precondition requires the input to be empty, which appears to be a bug. This makes the setter unusable for its apparent intended purpose of setting a specific OID value.

**Workaround:** Use direct assignment to `oid()` reference:
```cpp
// Instead of: id.oid("new-value");  // Will fail assertion
// Use:        id.oid() = "new-value";  // Works correctly
```

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If `oid` field is not string type |
| `std::out_of_range` | If `oid` field doesn't exist |
| Assertion failure | If non-empty string passed to setter |

### Comparison Operator

```cpp
result_t ovlId::operator==(ovlId const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_OUIDS) == DOCUMENT_COMPARE_MUTE_OUIDS)
         ? Success()
         : self() == other.self();
}
```

**Brief:** Compares OIDs, respecting the global comparison mask. Uses ternary operator for concise logic.

**Called by:** `ovlDatabaseRecord` comparison, test code

**Mask behavior:**
| Mask Setting | Behavior |
|--------------|----------|
| `DOCUMENT_COMPARE_MUTE_OUIDS` set | Returns `Success()` - IDs considered equal |
| `DOCUMENT_COMPARE_MUTE_OUIDS` not set | Performs `self() == other.self()` comparison |

## Performance Considerations

- **Construction:** O(1) for initialization plus OID generation if needed (OID generation may involve random number generation)
- **Accessor Methods:** O(1) - direct field lookup in JSON object hash map
- **`newId()`:** O(1) for OID generation and assignment
- **Comparison:** O(n) where n is OID string length (typically 24 characters)

## Error Handling Strategy

The implementation uses try-catch blocks with `confirm(false)` for error propagation:

```cpp
} catch (...) {
  confirm(false);
  throw;
}
```

This pattern:
1. Catches any exception
2. Records assertion failure for debugging/logging via `confirm(false)`
3. Re-throws the original exception unchanged

This ensures that:
- Unexpected errors are logged for debugging
- Original exception information is preserved
- Callers can catch and handle specific exceptions

## Testing Notes

- **Unit tests:** Part of Overlay module tests in `test/Overlay/`
- **Key test cases:**
  - Create ID with empty JSON (auto-generates OID)
  - Create ID with existing OID (preserves it, doesn't regenerate)
  - Generate new ID with `newId()` and verify it differs from original
  - Compare with masking disabled (different OIDs should fail)
  - Compare with masking enabled (different OIDs should succeed)
  - Verify OID format matches expected pattern (24 hex characters)

## Maintenance Notes

### OID Format

The `generate_oid()` function is expected to produce MongoDB-compatible 24-character hexadecimal strings. Example: `507f1f77bcf86cd799439011`. If the format needs to change:
1. Update `generate_oid()` in the common implementation
2. No changes needed to this file
3. Consider backward compatibility with existing stored documents

### Unusual Setter Behavior

The `oid(const std::string&)` setter has `confirm(id.empty())` which appears to be a bug:
- It requires the input to be empty
- Then assigns that empty string to the OID
- This makes the method unusable for its apparent purpose

**Recommendation:** Use direct assignment to `oid()` reference:
```cpp
id.oid() = "new-value";  // Works correctly
```

### Comparison Mask

Uses the global `useCompareMask()` function. Important considerations:
- Mask is stored in static/global state
- Changes affect all threads
- Reset mask to appropriate value after tests
- Consider using RAII wrapper for mask management in tests

### JSON Literal Dependencies

| Literal | Expected Value | Used For |
|---------|----------------|----------|
| `jsonliteral::oid` | `"oid"` | OID field key |

## See Also

- [ovlId.h](./ovlId.h.md) - Class declaration and public interface
- [ovlKeyValue.h](./ovlKeyValue.h.md) - Base class providing `value_as<T>()`
- [common.h](./common.h.md) - `generate_oid()` function and comparison masks
- [ovlDatabaseRecord.h](./ovlDatabaseRecord.h.md) - Uses `ovlId` as component
