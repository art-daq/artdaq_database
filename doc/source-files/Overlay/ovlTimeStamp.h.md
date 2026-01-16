# ovlTimeStamp.h

**Path:** `artdaq-database/Overlay/ovlTimeStamp.h`

**Purpose:** Defines the `ovlTimeStamp` overlay class for ISO8601 timestamp strings stored in JSON. This class provides validation during construction, accessor methods for reading and writing timestamps, and comparison support with masking capability. It is used throughout the Overlay module for temporal tracking of record creation, modification, and event timing.

## Key Concepts

### ISO8601 Timestamp Format

Timestamps are stored as strings in ISO8601 format, which provides a standardized, sortable representation of date and time:
```
2025-01-15T10:30:00Z
YYYY-MM-DDTHH:MM:SSZ
```
The format includes:
- Year, month, day (YYYY-MM-DD)
- Time separator (T)
- Hours, minutes, seconds (HH:MM:SS)
- UTC timezone indicator (Z)

### Construction-Time Validation

Timestamps are validated during construction using `confirm_iso8601_timestamp()`, ensuring they conform to the ISO8601 format. Invalid timestamps will cause assertion failures.

### Comparison Masking

Timestamp comparison can be masked using the `DOCUMENT_COMPARE_MUTE_TIMESTAMPS` flag. When this mask is active, timestamp comparisons always return success, which is useful for testing and comparing records that differ only in their timestamps.

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not safe for concurrent access; external synchronization required
- **Locking:** No internal locking

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/Overlay/common.h` | Module foundation types, constants, and `DOCUMENT_COMPARE_MUTE_TIMESTAMPS` flag |
| `artdaq-database/Overlay/ovlKeyValue.h` | Base class for key-value overlay pattern |

## Classes/Structures

### `ovlTimeStamp`

A final overlay class that wraps an ISO8601 timestamp string value, providing validation, type-safe access, and maskable comparison. Inherits from `ovlKeyValue` to participate in the overlay pattern.

**Thread Safety:** Not thread-safe; external synchronization required for concurrent access.

#### Constructor

##### `ovlTimeStamp(object_t::key_type const& key, value_t& ts)`

**Brief:** Constructs a timestamp overlay by wrapping an existing JSON string value and validating it as an ISO8601 timestamp.

**Parameters:**
- `key` - The JSON key for this timestamp (e.g., "created", "timestamp", "assigned")
- `ts` - Reference to the JSON string value containing the timestamp

**Preconditions:**
- `ts` must contain a valid ISO8601 timestamp string

**Postconditions:**
- Timestamp overlay is initialized and validated
- Modifications through the overlay affect the underlying JSON

**Throws:**
| Exception | Condition |
|-----------|-----------|
| Assertion failure | If the timestamp string is not valid ISO8601 format |

**Thread Safety:** Not thread-safe

#### Move Constructor and Destructor

##### `ovlTimeStamp(ovlTimeStamp&&) = default`

**Brief:** Default move constructor for transferring timestamp overlay ownership.

##### `~ovlTimeStamp() = default`

**Brief:** Default destructor that releases resources.

#### Methods

##### `timestamp() -> std::string&`

**Brief:** Returns a mutable reference to the timestamp string, allowing direct modification of the underlying value.

**Returns:** Mutable reference to the timestamp string

**Thread Safety:** Not thread-safe

**Example:**
```cpp
ovlTimeStamp ts("created", jsonValue);
ts.timestamp() = "2025-02-20T14:00:00Z";  // Direct assignment
```

##### `timestamp() const -> std::string const&`

**Brief:** Returns a const reference to the timestamp string for read-only access.

**Returns:** Const reference to the timestamp string

**Thread Safety:** Not thread-safe

##### `timestamp(std::string const& ts) -> std::string&`

**Brief:** Sets the timestamp to a new value after validating it is non-empty, then returns a reference to the updated timestamp.

**Parameters:**
- `ts` - The new timestamp value to set

**Preconditions:**
- `ts` must be non-empty (validated via `confirm()`)

**Postconditions:**
- Timestamp is updated to the new value
- Note: Does not validate ISO8601 format on setter (only on construction)

**Returns:** Mutable reference to the updated timestamp string

**Throws:**
| Exception | Condition |
|-----------|-----------|
| Assertion failure | If `ts` is empty |

**Thread Safety:** Not thread-safe

##### `to_string() const -> std::string` [override]

**Brief:** Serializes the timestamp to a JSON string representation including the key name.

**Returns:** JSON string in format `{"key": "timestamp_value"}`

**Thread Safety:** Not thread-safe

**Example:**
```cpp
ovlTimeStamp created("created", jsonValue);
std::string serialized = created.to_string();
// Output: {"created": "2025-01-15T10:30:00Z"}
```

##### `operator==(ovlTimeStamp const& other) const -> result_t`

**Brief:** Compares this timestamp with another for equality, respecting the `DOCUMENT_COMPARE_MUTE_TIMESTAMPS` mask.

**Parameters:**
- `other` - The timestamp to compare against

**Returns:** `Success()` if timestamps are equal or if timestamp comparison is masked, `Failure(message)` otherwise

**Postconditions:**
- If `DOCUMENT_COMPARE_MUTE_TIMESTAMPS` is set in the compare mask, always returns `Success()`

**Thread Safety:** Not thread-safe

## Type Aliases

### `ovlTimeStampUPtr_t`

```cpp
using ovlTimeStampUPtr_t = std::unique_ptr<ovlTimeStamp>;
```

**Brief:** Unique pointer type alias for managing timestamp overlay ownership.

## Relationship to Other Components

### In the Overlay Hierarchy

```
ovlKeyValue (base)
     ^
     |
ovlTimeStamp (this class)
```

### Used By

- **ovlBookkeeping** - Creation timestamp for records (`created` field)
- **ovlOrigin** - Document creation timestamp
- **ovlUpdate** - Event timestamp in update history
- **ovlKeyValueTimeStamp** - Assigned timestamp for named entries
- **ovlComment** - Comment creation timestamp

## See Also

- [ovlTimeStamp.cpp](./ovlTimeStamp.cpp.md) - Implementation details
- [ovlKeyValue.h](./ovlKeyValue.h.md) - Base class for overlay pattern
- [common.h](./common.h.md) - `DOCUMENT_COMPARE_MUTE_TIMESTAMPS` constant
- [ovlUpdate.h](./ovlUpdate.h.md) - Uses timestamp for event tracking
- [ovlBookkeeping.h](./ovlBookkeeping.h.md) - Uses timestamp for record creation time

## Notes for Developers

### Usage Examples

```cpp
#include "artdaq-database/Overlay/ovlTimeStamp.h"
#include "artdaq-database/SharedCommon/helper_functions.h"

using namespace artdaq::database::overlay;

void createTimestamp() {
  // Create JSON with timestamp
  value_t json = object_t{};
  json["created"] = artdaq::database::timestamp();  // Current time

  // Create overlay
  ovlTimeStamp created("created", json.at("created"));

  // Read timestamp
  std::cout << "Created: " << created.timestamp() << std::endl;
}

void updateTimestamp() {
  value_t json = object_t{};
  json["modified"] = "2025-01-15T10:30:00Z";

  ovlTimeStamp modified("modified", json.at("modified"));

  // Update via setter
  modified.timestamp(artdaq::database::timestamp());

  // Or via direct assignment
  modified.timestamp() = "2025-01-16T08:00:00Z";
}

void compareWithMasking() {
  value_t json1 = object_t{};
  value_t json2 = object_t{};
  json1["ts"] = "2025-01-15T10:30:00Z";
  json2["ts"] = "2025-01-16T11:45:00Z";

  ovlTimeStamp ts1("ts", json1.at("ts"));
  ovlTimeStamp ts2("ts", json2.at("ts"));

  // Without masking - will detect difference
  auto result = ts1 == ts2;  // Returns Failure

  // With masking - ignores timestamp differences
  useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS);
  result = ts1 == ts2;  // Returns Success
}
```

### Common Pitfalls

- **Validation only at construction:** ISO8601 format is validated during construction, but the setter only checks for non-empty. Direct modification via `timestamp()` reference bypasses all validation.
- **Empty string rejection:** The setter rejects empty strings, but the direct reference accessor does not.

### Anti-patterns

```cpp
// DON'T do this: Direct modification bypasses validation
ts.timestamp() = "invalid-timestamp";  // No validation!

// DO this instead: Validate before setting
std::string newTs = "2025-01-15T10:30:00Z";
if (is_valid_iso8601(newTs)) {  // Validate first
  ts.timestamp(newTs);
}
```

### Thread Safety Notes

This class is not thread-safe because:
1. It holds a reference to mutable JSON data
2. No internal synchronization mechanisms
3. Modifications affect shared JSON storage

Always use external synchronization when accessing timestamp overlays from multiple threads.
