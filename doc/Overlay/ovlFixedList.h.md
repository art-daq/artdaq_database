# ovlFixedList.h

**Path:** `artdaq-database/Overlay/ovlFixedList.h`

**Purpose:** Header-only template class providing an overlay for JSON arrays containing named elements. Supports adding and removing elements while maintaining a single list of active items. Called "Fixed" to distinguish from "Movable" lists, which maintain separate active/history arrays. Primarily used for configurations, entities, runs, comments, and attachments.

## Key Concepts

### Named Element Collections

Many database document sections contain arrays of named items:
- **configurations** - List of configuration references
- **entities** - List of entity references
- **runs** - List of run associations
- **comments** - User-added comments
- **attachments** - File attachments

This template provides type-safe management of such arrays with:
- Duplicate prevention by name
- Add/remove operations with status feedback
- Removal timestamp tracking
- Iterator support for range-based loops

### Fixed vs. Movable Lists

| Feature | ovlFixedList | ovlMovableList |
|---------|--------------|----------------|
| **JSON Structure** | Single array `[...]` | Object with two arrays `{"active": [...], "history": [...]}` |
| **Remove behavior** | Deletes from array | Moves to history array |
| **History tracking** | Via removed timestamp only | Full object preserved in history |
| **Primary use** | Configurations, entities, runs | Aliases (audit trail needed) |

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not supported; instances hold mutable references to JSON data
- **Locking:** None; caller must synchronize access if used from multiple threads

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/Overlay/common.h` | Types, result_t, comparison flags, JSON literals |
| `artdaq-database/Overlay/ovlKeyValue.h` | Base class for key-value overlay |

## Classes/Structures

### `ovlFixedList<T, mask>`

A template class extending `ovlKeyValue` that manages a JSON array of named overlay elements. Provides add, remove, wipe, and iteration operations with duplicate prevention.

**Template Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `T` | Element type | Overlay class for list elements (must have `name()` method) |
| `mask` | `std::uint32_t` | Bitmask for comparison control from `DOCUMENT_COMPARE_FLAGS` |

**Internal Type Aliases:**

```cpp
using List_t = array_t::container_type<T>;  // Container of overlay elements
using ElementUPtr_t = std::unique_ptr<T>;   // Smart pointer to element
```

**Thread Safety:** Not thread-safe

#### Methods

##### `ovlFixedList(object_t::key_type const& key, value_t& array)`

**Brief:** Constructs an overlay for a JSON array, wrapping each array element in the appropriate overlay type T.

**Parameters:**
- `key` - The JSON key identifying this array within its parent
- `array` - Reference to the JSON array value to overlay

**Preconditions:**
- `array` must be a JSON array

**Postconditions:**
- Each array element is wrapped in an overlay of type T
- The internal list mirrors the JSON array structure

**Thread Safety:** Not thread-safe

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlFixedList.h"
#include "artdaq-database/Overlay/ovlKeyValueTimeStamp.h"

using namespace artdaq::database::overlay;

// Type aliases (typically defined in ovlDatabaseRecord.h)
using ovlConfiguration = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_CONFIGURATION>;
using ovlConfigurations = ovlFixedList<ovlConfiguration, DOCUMENT_COMPARE_MUTE_CONFIGURATION>;

void createConfigList() {
  value_t recordJson = object_t{};
  auto& record = recordJson.value_as<object_t>();
  record["configurations"] = array_t{};

  ovlConfigurations configs("configurations", record.at("configurations"));
  // Now ready for add/remove operations
}
```

##### `wipe() -> void`

**Brief:** Removes all elements from the list, clearing both the JSON array and the internal overlay list.

**Postconditions:**
- The JSON array is empty
- The internal overlay list is empty

**Thread Safety:** Not thread-safe

**Example:**
```cpp
configs.wipe();  // Removes all configurations
```

##### `add(ElementUPtr_t& newEntry) -> result_t`

**Brief:** Adds a new element to the list if an element with the same name does not already exist. Returns status indicating whether the element was added or ignored.

**Parameters:**
- `newEntry` - Unique pointer to the element to add (must not be null)

**Preconditions:**
- `newEntry` must not be null

**Returns:**
- `Success(msg_Added)` - Element was added successfully
- `Success(msg_Ignored)` - Element with same name already exists

**Postconditions:**
- If added, the element is appended to the JSON array
- The internal overlay list is rebuilt to reflect the new state

**Thread Safety:** Not thread-safe

**Example:**
```cpp
auto newConfig = std::make_unique<ovlConfiguration>("config", configJson);
newConfig->name("DAQConfiguration");

auto result = configs.add(newConfig);
if (result.second == msg_Added) {
  std::cout << "Configuration added" << std::endl;
} else if (result.second == msg_Ignored) {
  std::cout << "Configuration already exists" << std::endl;
}
```

##### `remove(ElementUPtr_t& oldEntry) -> result_t`

**Brief:** Removes an element from the list by name. Adds a "removed" timestamp to the entry before removal.

**Parameters:**
- `oldEntry` - Unique pointer to an element with the name to remove (must not be null)

**Preconditions:**
- `oldEntry` must not be null
- An element with the matching name should exist in the list

**Returns:**
- `Success(msg_Removed)` - Element was removed successfully
- `Success(msg_Ignored)` - List was empty
- `Failure(msg_Missing)` - No element with matching name found

**Postconditions:**
- If removed, the element is deleted from the JSON array
- The removed entry has a "removed" timestamp added
- The internal overlay list is rebuilt

**Thread Safety:** Not thread-safe

**Example:**
```cpp
auto toRemove = std::make_unique<ovlConfiguration>("config", dummyJson);
toRemove->name("DAQConfiguration");

auto result = configs.remove(toRemove);
if (result.first) {
  std::cout << "Removed: " << result.second << std::endl;
} else {
  std::cout << "Not found: " << result.second << std::endl;
}
```

##### `begin() const -> List_t::const_iterator`

**Brief:** Returns a const iterator to the beginning of the element list for range-based iteration.

**Returns:** Const iterator to the first element

**Thread Safety:** Not thread-safe

##### `end() const -> List_t::const_iterator`

**Brief:** Returns a const iterator to the end of the element list for range-based iteration.

**Returns:** Const iterator past the last element

**Thread Safety:** Not thread-safe

**Example:**
```cpp
// Range-based iteration
for (auto const& config : configs) {
  std::cout << "Config: " << config.name() << std::endl;
  std::cout << "Assigned: " << config.assigned() << std::endl;
}

// STL algorithm usage
auto count = std::distance(configs.begin(), configs.end());
```

##### `to_string() const -> std::string` [override]

**Brief:** Serializes the list to a JSON-formatted string representation.

**Returns:** JSON string in format `{"key": [element1, element2, ...]}`

**Thread Safety:** Not thread-safe

##### `operator==(ovlFixedList const& other) const -> result_t`

**Brief:** Compares this list with another for equality. Compares list sizes first, then performs element-wise comparison.

**Parameters:**
- `other` - The list to compare against

**Returns:**
- `Success()` if mask bit is set (comparison skipped) or lists are equal
- `Failure(message)` with detailed difference description if not equal

**Thread Safety:** Not thread-safe

## Type Aliases

The following type aliases are defined in `ovlDatabaseRecord.h`:

```cpp
using ovlConfigurations = ovlFixedList<ovlConfiguration, DOCUMENT_COMPARE_MUTE_CONFIGURATION>;
using ovlEntities = ovlFixedList<ovlEntity, DOCUMENT_COMPARE_MUTE_CONFIGENTITY>;
using ovlRuns = ovlFixedList<ovlRun, DOCUMENT_COMPARE_MUTE_RUN>;
using ovlComments = ovlFixedList<ovlComment, DOCUMENT_COMPARE_MUTE_COMMENTS>;
using ovlAttachments = ovlFixedList<ovlAttachment, DOCUMENT_COMPARE_MUTE_ATTACHMENT>;
```

## Usage Examples

### Managing Configurations

```cpp
#include "artdaq-database/Overlay/ovlFixedList.h"

using namespace artdaq::database::overlay;

void manageConfigurations() {
  value_t recordJson = object_t{};
  auto& record = recordJson.value_as<object_t>();
  record["configurations"] = array_t{};

  auto configs = overlay<ovlConfigurations, array_t>(recordJson, "configurations");

  // Add a configuration
  value_t configJson = object_t{};
  configJson.value_as<object_t>()["name"] = "DAQConfig";
  auto newConfig = std::make_unique<ovlConfiguration>("config", configJson);

  auto result = configs->add(newConfig);
  std::cout << "Add result: " << result.second << std::endl;

  // Iterate through configurations
  for (auto const& config : *configs) {
    std::cout << "Config: " << config.name() << std::endl;
  }

  // Remove a configuration
  value_t removeJson = object_t{};
  removeJson.value_as<object_t>()["name"] = "DAQConfig";
  auto removeConfig = std::make_unique<ovlConfiguration>("config", removeJson);

  result = configs->remove(removeConfig);
  std::cout << "Remove result: " << result.second << std::endl;
}
```

### Entity Management

```cpp
void manageEntities() {
  value_t recordJson = object_t{};
  auto& record = recordJson.value_as<object_t>();
  record["entities"] = array_t{};

  auto entities = overlay<ovlEntities, array_t>(recordJson, "entities");

  // Add entity
  value_t entityJson = object_t{};
  entityJson.value_as<object_t>()["name"] = "BoardReader01";
  auto entity = std::make_unique<ovlEntity>("entity", entityJson);
  entities->add(entity);

  // Check contents
  std::cout << "Entity count: " << std::distance(entities->begin(), entities->end()) << std::endl;
}
```

## Performance Considerations

| Operation | Complexity | Notes |
|-----------|------------|-------|
| **Constructor** | O(n) | Wraps each array element |
| **add** | O(n) | Duplicate check + append + list rebuild |
| **remove** | O(n) | Search + erase + list rebuild |
| **begin/end** | O(1) | Iterator access |
| **Comparison** | O(n) | Element-wise comparison |

## Relationship to Other Components

This template is a core building block for database record management:
- Used by `ovlDatabaseRecord` to manage multiple list fields
- Works with `ovlKeyValueTimeStamp` and similar element types
- Provides the "simple list" pattern where history is not needed
- Complements `ovlMovableList` for cases requiring audit trails

## See Also

- [ovlKeyValue.h](./ovlKeyValue.h.md) - Base class providing core overlay functionality
- [ovlMovableList.h](./ovlMovableList.h.md) - Alternative with active/history tracking
- [ovlKeyValueTimeStamp.h](./ovlKeyValueTimeStamp.h.md) - Common element type for lists
- [common.h](./common.h.md) - Defines `DOCUMENT_COMPARE_FLAGS` enum

## Notes for Developers

### Common Pitfalls

- **Element type requirements:** The element type `T` must have a `name()` method returning a string for duplicate detection.
- **Iterator invalidation:** Iterators are invalidated after any add/remove/wipe operation because the internal list is rebuilt.
- **Unique pointer ownership:** The `add()` method takes a reference to a unique pointer but does not transfer ownership; it copies the underlying JSON value.

### Design Notes

- This is a header-only template with no corresponding .cpp file
- The class is `final` and cannot be derived from
- List reconstruction after modification ensures reference correctness
- Removed entries get a timestamp but are actually deleted from the array

### Anti-patterns

```cpp
// DON'T: Store iterators across modifications
auto it = configs.begin();
configs.add(newConfig);  // Invalidates iterator!
std::cout << it->name();  // Undefined behavior!

// DO: Re-obtain iterators after modification
configs.add(newConfig);
for (auto const& config : configs) {  // Fresh iterators
  std::cout << config.name() << std::endl;
}

// DON'T: Pass null pointer to add/remove
std::unique_ptr<ovlConfiguration> nullPtr;
configs.add(nullPtr);  // Will assert!

// DO: Ensure pointer is valid
auto validPtr = std::make_unique<ovlConfiguration>(...);
configs.add(validPtr);  // OK
```
