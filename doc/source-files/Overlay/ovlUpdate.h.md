# ovlUpdate.h

**Path:** `artdaq-database/Overlay/ovlUpdate.h`

**Purpose:** Defines the `ovlUpdate` overlay class that represents a single update event in a record's audit trail. Each update tracks three components: the event name (type of change), when it occurred (timestamp), and what was changed (value). These updates are stored in `ovlBookkeeping` to provide a complete history of record modifications.

## Key Concepts

### Audit Trail Entry

Each `ovlUpdate` represents one modification event in the record's history:
- **Event** - Name describing the type of change (e.g., "addConfiguration", "setVersion")
- **Timestamp** - When the change occurred (ISO8601 format)
- **Value** - The data that was changed (typically contains a name and assigned timestamp)

### Event Types

Common event names that appear in update history:

| Event Name | Description |
|------------|-------------|
| `addConfiguration` | Configuration added to record |
| `removeConfiguration` | Configuration removed from record |
| `addEntity` | Entity added to the system |
| `removeEntity` | Entity removed from the system |
| `addAlias` | Alias assigned to a version |
| `removeAlias` | Alias removed (moved to history) |
| `setVersion` | Version number changed |
| `setCollection` | Collection assignment changed |
| `addRun` | Run number added |

### Update Entry Structure

The `ovlUpdateEntry` type alias wraps the "value" field of an update, which contains:
- **name** - Identifier of what was changed
- **assigned** - Timestamp when the change was assigned

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not safe for concurrent access; external synchronization required
- **Locking:** No internal locking

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/Overlay/common.h` | Module foundation types and comparison constants |
| `artdaq-database/Overlay/ovlKeyValue.h` | Base class for key-value overlay pattern |
| `artdaq-database/Overlay/ovlTimeStamp.h` | Timestamp overlay for event timing |
| `artdaq-database/Overlay/ovlKeyValueTimeStamp.h` | Named entry with timestamp for update value |

## Type Aliases

### `ovlUpdateEntry`

```cpp
using ovlUpdateEntry = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_UPDATE_VALUES>;
```

**Brief:** Type alias for the update entry value, which is a named value with timestamp and comparison masking capability. When `DOCUMENT_COMPARE_MUTE_UPDATE_VALUES` is set, update value comparisons are masked.

### `ovlUpdateEntryUPtr_t`

```cpp
using ovlUpdateEntryUPtr_t = std::unique_ptr<ovlUpdateEntry>;
```

**Brief:** Unique pointer type for update entry ownership.

### `ovlUpdateUPtr_t`

```cpp
using ovlUpdateUPtr_t = std::unique_ptr<ovlUpdate>;
```

**Brief:** Unique pointer type for update overlay ownership.

## Classes/Structures

### `ovlUpdate`

A final overlay class that wraps an update event JSON object, providing type-safe access to the event name, timestamp, and changed value. Inherits from `ovlKeyValue` to participate in the overlay pattern.

**Thread Safety:** Not thread-safe; external synchronization required for concurrent access.

#### Constructor

##### `ovlUpdate(object_t::key_type const& key, value_t& update)`

**Brief:** Constructs an update overlay by wrapping an existing JSON object and creating sub-overlays for timestamp and value components.

**Parameters:**
- `key` - The JSON key for this update (typically an array index when in updates array)
- `update` - Reference to the JSON object containing event, timestamp, and value fields

**Preconditions:**
- `update` must be a valid JSON object
- `update` must contain "event", "timestamp", and "value" fields

**Postconditions:**
- Update overlay is initialized with sub-overlays for timestamp and value
- Modifications through the overlay affect the underlying JSON

**Throws:**
| Exception | Condition |
|-----------|-----------|
| Assertion failure | If `update` is not an object or required fields are missing |

**Thread Safety:** Not thread-safe

#### Move Constructor and Destructor

##### `ovlUpdate(ovlUpdate&&) = default`

**Brief:** Default move constructor for transferring update overlay ownership.

##### `~ovlUpdate() = default`

**Brief:** Default destructor that releases resources.

#### Methods

##### `name() -> std::string&`

**Brief:** Returns a mutable reference to the event name, allowing modification of the event type.

**Returns:** Mutable reference to the event name string (from "event" field)

**Thread Safety:** Not thread-safe

##### `name() const -> std::string const&`

**Brief:** Returns a const reference to the event name for read-only access.

**Returns:** Const reference to the event name string

**Thread Safety:** Not thread-safe

##### `timestamp() -> std::string&`

**Brief:** Returns a mutable reference to the event timestamp, allowing modification of when the event occurred.

**Returns:** Mutable reference to the timestamp string

**Thread Safety:** Not thread-safe

##### `timestamp() const -> std::string const&`

**Brief:** Returns a const reference to the event timestamp for read-only access.

**Returns:** Const reference to the timestamp string

**Thread Safety:** Not thread-safe

##### `what() -> ovlUpdateEntry&`

**Brief:** Returns a mutable reference to the update entry value, allowing modification of what was changed.

**Returns:** Mutable reference to the update entry overlay

**Thread Safety:** Not thread-safe

##### `what() const -> ovlUpdateEntry const&`

**Brief:** Returns a const reference to the update entry value for read-only access to what was changed.

**Returns:** Const reference to the update entry overlay

**Thread Safety:** Not thread-safe

##### `to_string() const -> std::string` [override]

**Brief:** Serializes the update to a JSON string representation including event name, timestamp, and value.

**Returns:** JSON string representation of the update

**Thread Safety:** Not thread-safe

**Example Output:**
```json
{"event": "addConfiguration", "timestamp": "2025-01-15T10:30:00Z", "value": {"name": "DAQConfig", "assigned": "2025-01-15T10:30:00Z"}}
```

##### `operator==(ovlUpdate const& other) const -> result_t`

**Brief:** Compares this update with another for equality, checking timestamp, event name, and value while respecting comparison masks.

**Parameters:**
- `other` - The update to compare against

**Returns:** `Success()` if updates are equal (considering masks), `Failure(message)` with details if different

**Postconditions:**
- Returns detailed error message describing which components differ

**Thread Safety:** Not thread-safe

#### Private Methods

##### `map_timestamp(value_t& value) -> ovlTimeStamp`

**Brief:** Creates an `ovlTimeStamp` overlay for the timestamp field within the update object.

##### `map_what(value_t& value) -> ovlUpdateEntry`

**Brief:** Creates an `ovlUpdateEntry` overlay for the value field within the update object.

## Relationship to Other Components

### In the Overlay Hierarchy

```
ovlKeyValue (base)
     ^
     |
ovlUpdate (this class)
     |
     +-- ovlTimeStamp _timestamp (when the event occurred)
     +-- ovlUpdateEntry _what (what was changed)
```

### Used By

- **ovlBookkeeping** - Stores updates in the `updates` movable list

### JSON Structure

```json
{
  "updates": [
    {
      "event": "addConfiguration",
      "timestamp": "2025-01-15T10:30:00Z",
      "value": {
        "name": "DAQConfiguration",
        "assigned": "2025-01-15T10:30:00Z"
      }
    },
    {
      "event": "setVersion",
      "timestamp": "2025-01-15T10:31:00Z",
      "value": {
        "name": "v2_0_0",
        "assigned": "2025-01-15T10:31:00Z"
      }
    }
  ]
}
```

## See Also

- [ovlUpdate.cpp](./ovlUpdate.cpp.md) - Implementation details
- [ovlKeyValue.h](./ovlKeyValue.h.md) - Base class for overlay pattern
- [ovlTimeStamp.h](./ovlTimeStamp.h.md) - Timestamp overlay used for event timing
- [ovlKeyValueTimeStamp.h](./ovlKeyValueTimeStamp.h.md) - Update entry template
- [ovlBookkeeping.h](./ovlBookkeeping.h.md) - Contains update arrays

## Notes for Developers

### Usage Examples

```cpp
#include "artdaq-database/Overlay/ovlUpdate.h"
#include "artdaq-database/SharedCommon/helper_functions.h"

using namespace artdaq::database::overlay;

void createUpdate() {
  // Create JSON for an update event
  value_t updateJson = object_t{};
  updateJson["event"] = "addEntity";
  updateJson["timestamp"] = artdaq::database::timestamp();
  updateJson["value"] = object_t{};
  updateJson["value"]["name"] = "BoardReader01";
  updateJson["value"]["assigned"] = artdaq::database::timestamp();

  // Create overlay
  ovlUpdate update("update", updateJson);

  // Access components
  std::cout << "Event: " << update.name() << std::endl;
  std::cout << "When: " << update.timestamp() << std::endl;
  std::cout << "What: " << update.what().name() << std::endl;
}

void modifyUpdate() {
  value_t updateJson = /* existing update */;
  ovlUpdate update("update", updateJson);

  // Modify event type
  update.name() = "removeEntity";

  // Update timestamp
  update.timestamp() = artdaq::database::timestamp();
}

void compareUpdates() {
  value_t json1 = /* first update */;
  value_t json2 = /* second update */;

  ovlUpdate update1("update", json1);
  ovlUpdate update2("update", json2);

  // Compare with timestamp masking
  useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS);
  auto result = update1 == update2;

  if (!result.first) {
    std::cerr << "Updates differ: " << result.second << std::endl;
  }
}
```

### Common Pitfalls

- **Event names are free-form:** Event names are strings with no validation. Use consistent naming conventions.
- **Timestamp masking affects comparison:** When `DOCUMENT_COMPARE_MUTE_TIMESTAMPS` is set, only event names are compared, not the full value.

### Anti-patterns

```cpp
// DON'T do this: Inconsistent event naming
update1.name() = "add_entity";  // snake_case
update2.name() = "addEntity";   // camelCase
// These will not compare as equal!

// DO this instead: Use consistent naming convention
update1.name() = "addEntity";
update2.name() = "addEntity";
```

### Comparison Behavior

The comparison operator has special behavior based on the timestamp mask:
- **Without mask:** Compares timestamp, event name, and full value
- **With `DOCUMENT_COMPARE_MUTE_TIMESTAMPS`:** Compares only event names within the value (not timestamps)

This allows flexible comparison where timing information can be ignored when testing or comparing semantically equivalent updates.
