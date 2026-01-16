# ovlMovableList.h

**Path:** `artdaq-database/Overlay/ovlMovableList.h`

**Purpose:** Template class for managing JSON arrays with full history tracking. Unlike `ovlFixedList`, this template maintains two separate arrays: "active" for current items and "history" for removed items. When an element is removed, it is moved to the history array (not deleted), preserving a complete audit trail. Primarily used for alias management where historical tracking is essential for reproducibility.

## Key Concepts

### Active/History Pattern

This template implements a two-list pattern:
- **active** - Currently valid items
- **history** - Previously removed items with removal timestamps

This pattern is essential when:
- You need to track what was active at a specific time
- Removed items should be preserved for audit purposes
- Historical lookups are required (e.g., "what alias was active during run X?")

### JSON Structure

Unlike `ovlFixedList` which overlays a simple array, this template expects an object containing two arrays:

```json
{
  "aliases": {
    "active": [
      {"name": "production", "assigned": "2025-01-15T10:00:00Z"}
    ],
    "history": [
      {"name": "old-prod", "assigned": "2025-01-01T00:00:00Z", "removed": "2025-01-15T09:59:00Z"}
    ]
  }
}
```

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

### `ovlMovableList<T, mask>`

A template class extending `ovlKeyValue` that manages two JSON arrays (active and history) for named overlay elements. When elements are removed, they are moved to the history array with a removal timestamp.

**Template Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `T` | Element type | Overlay class for list elements (must have `name()`, `key()`, `value()`, `object_value()` methods) |
| `mask` | `std::uint32_t` | Bitmask for comparison control from `DOCUMENT_COMPARE_FLAGS` |

**Internal Type Aliases:**

```cpp
using List_t = array_t::container_type<T>;  // Container of overlay elements
using ElementUPtr_t = std::unique_ptr<T>;   // Smart pointer to element
```

**Thread Safety:** Not thread-safe

#### Methods

##### `ovlMovableList(object_t::key_type const& key, value_t& object)`

**Brief:** Constructs an overlay for a JSON object containing "active" and "history" arrays. Creates these arrays if they do not exist, then wraps each element in the appropriate overlay type T.

**Parameters:**
- `key` - The JSON key identifying this field within its parent
- `object` - Reference to the JSON object value to overlay (must be an object, not array)

**Preconditions:**
- `object` must be a JSON object (type_t::OBJECT)

**Postconditions:**
- The "active" array exists (created if missing)
- The "history" array exists (created if missing)
- Each array element is wrapped in an overlay of type T

**Thread Safety:** Not thread-safe

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlMovableList.h"
#include "artdaq-database/Overlay/ovlKeyValueTimeStamp.h"

using namespace artdaq::database::overlay;

// Type aliases (typically defined in ovlDatabaseRecord.h)
using ovlAlias = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_ALIAS, true, true>;
using ovlAliases = ovlMovableList<ovlAlias, DOCUMENT_COMPARE_MUTE_ALIAS>;

void createAliasList() {
  value_t recordJson = object_t{};
  auto& record = recordJson.value_as<object_t>();
  record["aliases"] = object_t{};  // Note: object, not array

  auto aliases = overlay<ovlAliases>(recordJson, "aliases");
  // Both "active" and "history" arrays are now created
}
```

##### `wipe() -> void`

**Brief:** Clears both active and history lists, removing all elements from both internal overlay lists.

**Postconditions:**
- Both active and history lists are empty
- Note: This clears the overlay lists but the underlying JSON may not be modified

**Thread Safety:** Not thread-safe

##### `add(ElementUPtr_t& newEntry) -> result_t`

**Brief:** Adds a new element to the active list if an element with the same name does not already exist in the active list. History is not checked for duplicates.

**Parameters:**
- `newEntry` - Unique pointer to the element to add (must not be null)

**Preconditions:**
- `newEntry` must not be null

**Returns:**
- `Success(msg_Added)` - Element was added successfully
- `Success(msg_Ignored)` - Element with same name already exists in active list

**Postconditions:**
- If added, the element is appended to the "active" JSON array
- The internal active overlay list is rebuilt

**Thread Safety:** Not thread-safe

**Example:**
```cpp
value_t aliasJson = object_t{};
aliasJson.value_as<object_t>()["name"] = "production";

auto newAlias = std::make_unique<ovlAlias>("alias", aliasJson);
newAlias->assigned() = timestamp();

auto result = aliases->add(newAlias);
if (result.second == msg_Added) {
  std::cout << "Alias added to active list" << std::endl;
}
```

##### `remove(ElementUPtr_t& oldEntry) -> result_t`

**Brief:** Removes an element from the active list and moves it to the history list with a removal timestamp. The complete element is preserved in history.

**Parameters:**
- `oldEntry` - Unique pointer to an element with the name to remove (must not be null)

**Preconditions:**
- `oldEntry` must not be null
- An element with the matching name should exist in the active list

**Returns:**
- `Success(msg_Removed)` - Element was moved to history successfully
- `Success(msg_Ignored)` - Active list was empty
- `Failure(msg_Missing)` - No element with matching name found in active list

**Postconditions:**
- If removed, the element is deleted from the "active" JSON array
- The element has a "removed" timestamp added
- The element is appended to the "history" JSON array
- Both internal overlay lists are rebuilt

**Thread Safety:** Not thread-safe

**Example:**
```cpp
value_t removeJson = object_t{};
removeJson.value_as<object_t>()["name"] = "production";

auto toRemove = std::make_unique<ovlAlias>("alias", removeJson);

auto result = aliases->remove(toRemove);
if (result.first) {
  std::cout << "Alias moved to history" << std::endl;
}
// The alias is now in history with a "removed" timestamp
```

##### `begin() const -> List_t::const_iterator`

**Brief:** Returns a const iterator to the beginning of the active element list. Does not provide access to history.

**Returns:** Const iterator to the first active element

**Thread Safety:** Not thread-safe

##### `end() const -> List_t::const_iterator`

**Brief:** Returns a const iterator to the end of the active element list.

**Returns:** Const iterator past the last active element

**Thread Safety:** Not thread-safe

**Example:**
```cpp
// Range-based iteration over active aliases only
for (auto const& alias : *aliases) {
  std::cout << "Active alias: " << alias.name() << std::endl;
  std::cout << "Assigned: " << alias.assigned() << std::endl;
}
```

##### `to_string() const -> std::string` [override]

**Brief:** Serializes both active and history lists to a JSON-formatted string representation.

**Returns:** JSON string in format `{"key": {"active": [...], "history": [...]}}`

**Thread Safety:** Not thread-safe

##### `operator==(ovlMovableList const& other) const -> result_t`

**Brief:** Compares this list with another for equality. Only compares active lists; history differences are ignored.

**Parameters:**
- `other` - The list to compare against

**Returns:**
- `Success()` if mask bit is set (comparison skipped) or active lists are equal
- `Failure(message)` with detailed difference description if active lists differ

**Thread Safety:** Not thread-safe

**Note:** History is intentionally excluded from comparison because two records with the same current state should be considered equal regardless of their history.

## Type Alias

The following type alias is defined in `ovlDatabaseRecord.h`:

```cpp
using ovlAliases = ovlMovableList<ovlAlias, DOCUMENT_COMPARE_MUTE_ALIAS>;
```

## Usage Examples

### Managing Aliases with History

```cpp
#include "artdaq-database/Overlay/ovlMovableList.h"

using namespace artdaq::database::overlay;

void manageAliases() {
  value_t recordJson = object_t{};
  auto& record = recordJson.value_as<object_t>();
  record["aliases"] = object_t{};

  auto aliases = overlay<ovlAliases>(recordJson, "aliases");

  // Add an alias
  value_t aliasJson = object_t{};
  aliasJson.value_as<object_t>()["name"] = "production";
  auto newAlias = std::make_unique<ovlAlias>("alias", aliasJson);
  newAlias->assigned() = timestamp();
  aliases->add(newAlias);

  // Later: Remove the alias (moves to history)
  value_t removeJson = object_t{};
  removeJson.value_as<object_t>()["name"] = "production";
  auto oldAlias = std::make_unique<ovlAlias>("alias", removeJson);
  aliases->remove(oldAlias);

  // Iterate active aliases only
  for (auto const& alias : *aliases) {
    std::cout << "Active: " << alias.name() << std::endl;
  }

  // History is preserved in the JSON but not directly iterable
  // Access via JSON if needed for auditing
  auto& aliasObj = recordJson.value_as<object_t>()["aliases"];
  auto& history = aliasObj.value_as<object_t>()["history"];
  // Process history array...
}
```

### Checking History via JSON

```cpp
void auditAliasHistory(value_t& recordJson) {
  auto& aliases = recordJson.value_as<object_t>()["aliases"];
  auto& history = aliases.value_as<object_t>()["history"];
  auto& historyArray = history.value_as<array_t>();

  for (auto& entry : historyArray) {
    auto& obj = entry.value_as<object_t>();
    std::cout << "Historical alias: " << obj["name"].value_as<std::string>() << std::endl;
    std::cout << "  Assigned: " << obj["assigned"].value_as<std::string>() << std::endl;
    std::cout << "  Removed: " << obj["removed"].value_as<std::string>() << std::endl;
  }
}
```

## Comparison: ovlFixedList vs. ovlMovableList

| Feature | ovlFixedList | ovlMovableList |
|---------|--------------|----------------|
| **JSON Structure** | `[...]` (array) | `{"active": [...], "history": [...]}` (object) |
| **Remove behavior** | Deletes element | Moves to history |
| **History tracking** | Only removed timestamp | Full object preserved |
| **Storage cost** | O(active) | O(active + history) |
| **Use case** | configs, entities, runs | aliases |
| **Comparison** | Compares all elements | Compares only active |

## Performance Considerations

| Operation | Complexity | Notes |
|-----------|------------|-------|
| **Constructor** | O(n+h) | Wraps active and history elements |
| **add** | O(n) | Duplicate check in active + append + rebuild |
| **remove** | O(n+h) | Search + erase + history append + rebuilds |
| **begin/end** | O(1) | Iterator access (active only) |
| **Comparison** | O(n) | Active list comparison only |

Where n = active count, h = history count

## Relationship to Other Components

This template is specifically designed for alias management in `ovlDatabaseRecord`:
- Preserves complete history for audit and debugging
- Enables "time travel" queries (what alias was active at time T)
- Supports production environment tracking

The history preservation is essential because:
- Aliases represent production state
- Debugging issues requires knowing historical alias bindings
- Reproducibility of past runs requires knowing what alias pointed to

## See Also

- [ovlKeyValue.h](./ovlKeyValue.h.md) - Base class providing core overlay functionality
- [ovlFixedList.h](./ovlFixedList.h.md) - Simpler alternative without history tracking
- [ovlKeyValueTimeStamp.h](./ovlKeyValueTimeStamp.h.md) - Element type used for aliases
- [common.h](./common.h.md) - Defines `DOCUMENT_COMPARE_FLAGS` enum

## Notes for Developers

### Common Pitfalls

- **Expects object, not array:** Unlike `ovlFixedList`, the constructor expects a JSON object that will contain "active" and "history" arrays, not a JSON array directly.
- **Iterator only covers active:** The `begin()`/`end()` iterators only access the active list. History must be accessed via the underlying JSON if needed.
- **History is not compared:** Two overlays with the same active list but different histories are considered equal.

### Design Notes

- This is a header-only template with no corresponding .cpp file
- The class is `final` and cannot be derived from
- History is never automatically cleaned (grows unbounded)
- Used exclusively for aliases in the current codebase

### Anti-patterns

```cpp
// DON'T: Pass an array instead of object
value_t arrayValue = array_t{};
ovlAliases aliases("aliases", arrayValue);  // Will assert!

// DO: Pass an object (active/history arrays created automatically)
value_t objectValue = object_t{};
ovlAliases aliases("aliases", objectValue);  // OK

// DON'T: Assume iterators cover history
for (auto const& alias : aliases) {
  // This only iterates active aliases, not history!
}

// DO: Access history via JSON if needed
auto& historyArray = record["aliases"]["history"].value_as<array_t>();

// DON'T: Rely on comparison detecting history differences
// History is intentionally ignored during comparison
```

### Why Only for Aliases?

Aliases represent production deployment state, and historical tracking is critical:
- Need to know what configuration a "production" alias pointed to last week
- Essential for debugging production issues
- Required for reproducing past data-taking runs

Other lists (configurations, entities, runs) can use simpler `ovlFixedList` because:
- They track what was added, not what was replaced
- Removed timestamp is sufficient for basic tracking
- Full history preservation would be excessive storage overhead
