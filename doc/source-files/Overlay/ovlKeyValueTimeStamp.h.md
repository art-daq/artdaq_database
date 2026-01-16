# ovlKeyValueTimeStamp.h

**Path:** `artdaq-database/Overlay/ovlKeyValueTimeStamp.h`

**Purpose:** Template class that combines a required name field with optional assigned and removed timestamp fields. This overlay is used for tracking named entities with temporal metadata such as configurations, aliases, entities, and runs throughout the database document structure.

## Key Concepts

### Named Entities with Timestamps

Many database record fields represent named items that need temporal tracking:
- **Configurations** - When a configuration was added to the document
- **Aliases** - When an alias was assigned and potentially removed
- **Entities** - When an entity reference was added
- **Runs** - When a run was associated with the document

This template provides a reusable pattern for all such fields.

### Template Parameters

The class uses three template parameters:
1. **mask** - A bitmask for selective comparison (see `DOCUMENT_COMPARE_FLAGS` in `common.h`)
2. **A** (default: true) - Whether to include an "assigned" timestamp
3. **R** (default: false) - Whether to include a "removed" timestamp

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not supported; instances hold mutable references to JSON data
- **Locking:** None; caller must synchronize access if used from multiple threads

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/Overlay/common.h` | Types, result_t, comparison flags, JSON literals |
| `artdaq-database/Overlay/ovlKeyValue.h` | Base class for key-value overlay |
| `artdaq-database/Overlay/ovlTimeStamp.h` | Used for assigned/removed timestamp fields |

## Classes/Structures

### `ovlKeyValueTimeStamp<mask, A, R>`

A template class extending `ovlKeyValue` that wraps a JSON object containing a name field and optional timestamp fields. The class automatically creates missing timestamp fields during initialization.

**Template Parameters:**

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `mask` | `std::uint32_t` | (required) | Bitmask for comparison control from `DOCUMENT_COMPARE_FLAGS` |
| `A` | `bool` | `true` | Include "assigned" timestamp field |
| `R` | `bool` | `false` | Include "removed" timestamp field |

**Thread Safety:** Not thread-safe

#### Methods

##### `ovlKeyValueTimeStamp(object_t::key_type const& key, value_t& value)`

**Brief:** Constructs an overlay for a JSON object containing name and timestamp fields. Validates that the object has a "name" field and creates any missing timestamp fields.

**Parameters:**
- `key` - The JSON key identifying this field within its parent object
- `value` - Reference to the JSON value to overlay (must be an object)

**Preconditions:**
- `value` must be a JSON object (type_t::OBJECT)
- The object must contain a "name" field

**Postconditions:**
- If `A == true`, the "assigned" field exists (created if missing with current timestamp)
- If `R == true`, the "removed" field exists (created if missing with current timestamp)

**Throws:**
| Exception | Condition |
|-----------|-----------|
| assertion failure | If `value` is not an object or "name" field is missing |

**Thread Safety:** Not thread-safe

##### `name() -> std::string&`

**Brief:** Returns a mutable reference to the name field value, allowing both read and write access.

**Returns:** Reference to the string value stored in the "name" field

**Thread Safety:** Not thread-safe

##### `name() const -> std::string const&`

**Brief:** Returns a const reference to the name field value for read-only access.

**Returns:** Const reference to the string value stored in the "name" field

**Thread Safety:** Not thread-safe

##### `name(std::string const& name) -> std::string&`

**Brief:** Sets the name field to a new value and returns a reference to it.

**Parameters:**
- `name` - New name value (must not be empty)

**Preconditions:**
- `name` parameter must not be empty

**Returns:** Reference to the updated name field value

**Throws:**
| Exception | Condition |
|-----------|-----------|
| assertion failure | If `name` parameter is empty |

**Thread Safety:** Not thread-safe

##### `assigned() -> std::string&`

**Brief:** Returns a mutable reference to the assigned timestamp value. Only valid when template parameter `A == true`.

**Preconditions:**
- Template parameter `A` must be `true`

**Returns:** Reference to the assigned timestamp string

**Throws:**
| Exception | Condition |
|-----------|-----------|
| assertion failure | If `A == false` |

**Thread Safety:** Not thread-safe

##### `removed() -> std::string&`

**Brief:** Returns a mutable reference to the removed timestamp value. Only valid when template parameter `R == true`.

**Preconditions:**
- Template parameter `R` must be `true`

**Returns:** Reference to the removed timestamp string

**Throws:**
| Exception | Condition |
|-----------|-----------|
| assertion failure | If `R == false` |

**Thread Safety:** Not thread-safe

##### `to_string() const -> std::string` [override]

**Brief:** Serializes the overlay to a JSON-formatted string containing the name and any enabled timestamp fields.

**Returns:** JSON string representation, e.g., `{"name": "myconfig", "assigned": "2025-01-15T10:30:00Z"}`

**Thread Safety:** Not thread-safe

##### `operator==(ovlKeyValueTimeStamp const& other) const -> result_t`

**Brief:** Compares this overlay with another for equality. Compares name fields and, if enabled, the assigned and removed timestamps.

**Parameters:**
- `other` - The overlay to compare against

**Returns:** `Success()` if equal, `Failure(message)` with detailed difference description if not

**Thread Safety:** Not thread-safe

## Type Aliases

The following type aliases are defined in `ovlDatabaseRecord.h`:

```cpp
// Name + assigned timestamp (most common pattern)
using ovlConfiguration = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_CONFIGURATION>;
using ovlRun = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_RUN>;
using ovlEntity = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_CONFIGENTITY>;

// Name + assigned + removed timestamps (for tracking deletions)
using ovlAlias = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_ALIAS, true, true>;
```

## Usage Examples

### Basic Configuration Entry

```cpp
#include "artdaq-database/Overlay/ovlKeyValueTimeStamp.h"
#include "artdaq-database/DataFormats/Json/json_types.h"

using namespace artdaq::database::overlay;
using namespace artdaq::database::json;

void createConfiguration() {
  // Create JSON structure with required name field
  value_t configJson = object_t{};
  auto& obj = configJson.value_as<object_t>();
  obj["name"] = "DAQConfiguration";
  // Note: assigned timestamp will be auto-created if missing

  try {
    ovlConfiguration config("configuration", configJson);

    std::cout << "Config name: " << config.name() << std::endl;
    std::cout << "Assigned at: " << config.assigned() << std::endl;
    std::cout << "JSON: " << config.to_string() << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}
```

### Alias with Removal Tracking

```cpp
#include "artdaq-database/Overlay/ovlKeyValueTimeStamp.h"

using namespace artdaq::database::overlay;

void manageAlias() {
  value_t aliasJson = object_t{};
  auto& obj = aliasJson.value_as<object_t>();
  obj["name"] = "production";
  // Both assigned and removed timestamps auto-created

  ovlAlias alias("alias", aliasJson);

  // Track when assigned and when removed
  std::cout << "Assigned: " << alias.assigned() << std::endl;
  std::cout << "Removed: " << alias.removed() << std::endl;
}
```

### Comparing Configurations

```cpp
#include "artdaq-database/Overlay/ovlKeyValueTimeStamp.h"

using namespace artdaq::database::overlay;

void compareConfigs() {
  value_t json1 = object_t{};
  json1.value_as<object_t>()["name"] = "config1";

  value_t json2 = object_t{};
  json2.value_as<object_t>()["name"] = "config2";

  ovlConfiguration config1("cfg", json1);
  ovlConfiguration config2("cfg", json2);

  auto result = config1 == config2;
  if (!result.first) {
    std::cout << "Configs differ: " << result.second << std::endl;
  }
}
```

## Relationship to Other Components

This template is a specialized extension of `ovlKeyValue` that adds:
- Automatic timestamp field management via `ovlTimeStamp`
- Compile-time configuration of which timestamps to include
- Name field validation and access

It is used by `ovlFixedList` and `ovlMovableList` to manage collections of named, timestamped entities within database records.

## See Also

- [ovlKeyValue.h](./ovlKeyValue.h.md) - Base class providing key-value overlay functionality
- [ovlTimeStamp.h](./ovlTimeStamp.h.md) - Used for timestamp field management
- [ovlFixedList.h](./ovlFixedList.h.md) - Container for lists of timestamped entries
- [ovlMovableList.h](./ovlMovableList.h.md) - Container with active/history tracking
- [common.h](./common.h.md) - Defines `DOCUMENT_COMPARE_FLAGS` enum

## Notes for Developers

### Common Pitfalls

- **Missing name field:** The constructor will assert if the "name" field does not exist in the JSON object. Always ensure the name is set before creating the overlay.
- **Accessing disabled timestamps:** Calling `assigned()` when `A == false` or `removed()` when `R == false` triggers an assertion failure.
- **Empty names:** Setting an empty name via `name("")` triggers an assertion.

### Design Notes

- This is a header-only template with no corresponding .cpp file
- The class is `final` and cannot be derived from
- Different template parameter combinations create incompatible types
- Timestamps are created with the current system time if missing
- The mask parameter enables selective comparison in testing and validation scenarios

### Anti-patterns

```cpp
// DON'T: Create overlay without name field
value_t json = object_t{};
ovlConfiguration config("cfg", json);  // Will assert! No "name" field

// DO: Ensure name exists first
value_t json = object_t{};
json.value_as<object_t>()["name"] = "myconfig";
ovlConfiguration config("cfg", json);  // OK

// DON'T: Access removed() on type without R=true
ovlConfiguration config("cfg", json);
config.removed();  // Will assert! R defaults to false

// DO: Use correct type alias
ovlAlias alias("alias", json);  // Has R=true
alias.removed();  // OK
```
