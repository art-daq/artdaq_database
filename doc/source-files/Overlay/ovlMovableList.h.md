# ovlMovableList.h

## File Overview

Template class for managing JSON arrays with history tracking. Unlike `ovlFixedList`, this maintains two separate arrays: "active" for current items and "history" for removed items. Primarily used for alias management where historical tracking is important.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/ovlMovableList.h`

## Purpose

Manages collections with historical tracking by:
- Maintaining active and history arrays
- Moving removed items to history (not deleting)
- Adding removal timestamps
- Supporting iteration over active items
- Providing maskable comparison

## Template Definition

```cpp
template <typename T, std::uint32_t mask>
class ovlMovableList final : public ovlKeyValue {
  using List_t = array_t::container_type<T>;
  using ElementUPtr_t = std::unique_ptr<T>;

 public:
  ovlMovableList(object_t::key_type const& key, value_t& object);

  // utils
  void wipe();
  result_t add(ElementUPtr_t&);
  result_t remove(ElementUPtr_t&);

  typename List_t::const_iterator begin() const { return _active.begin(); }
  typename List_t::const_iterator end() const { return _active.end(); }

  // overrides
  std::string to_string() const override;

  // ops
  result_t operator==(ovlMovableList const&) const;

 private:
  List_t make_list(array_t& list);
  bool init(value_t& parent);

 private:
  bool _initOK;
  List_t _active;
  List_t _history;
};
```

## Key Differences from ovlFixedList

| Feature | ovlFixedList | ovlMovableList |
|---------|--------------|----------------|
| **Structure** | Single array | Active + History arrays |
| **Remove behavior** | Deletes from array | Moves to history |
| **JSON structure** | `[...]` | `{"active": [...], "history": [...]}` |
| **Primary use** | Configurations, entities, runs | Aliases |
| **History tracking** | Via removed timestamp only | Full object preserved in history |

## Type Alias

```cpp
// In ovlDatabaseRecord.h
using ovlAliases = ovlMovableList<ovlAlias, DOCUMENT_COMPARE_MUTE_ALIAS>;
```

## Implementation

### Constructor and Initialization
```cpp
template <typename T, std::uint32_t mask>
ovlMovableList<T, mask>::ovlMovableList(object_t::key_type const& key, value_t& object)
    : ovlKeyValue(key, object),
      _initOK(init(object)),
      _active(make_list(ovlKeyValue::value_as<array_t>(jsonliteral::active))),
      _history(make_list(ovlKeyValue::value_as<array_t>(jsonliteral::history))) {}

template <typename T, std::uint32_t mask>
bool ovlMovableList<T, mask>::init(value_t& parent) try {
  confirm(type(parent) == type_t::OBJECT);

  auto& obj = object_value();

  if (obj.count(jsonliteral::active) == 0) obj[jsonliteral::active] = array_t{};
  if (obj.count(jsonliteral::history) == 0) obj[jsonliteral::history] = array_t{};

  return true;
} catch (...) {
  confirm(false);
  throw;
}
```

**Initialization**:
- Expects parent to be an object (not array like ovlFixedList)
- Creates both "active" and "history" fields if missing
- Wraps both arrays with overlay elements

### JSON Structure
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

### Adding Elements
```cpp
template <typename T, std::uint32_t mask>
result_t ovlMovableList<T, mask>::add(ElementUPtr_t& newEntry) {
  confirm(newEntry);

  // Check for duplicates in active list
  for (auto& entry : _active) {
    if (entry.name() == newEntry->name()) {
      return Success(msg_Ignored);
    }
  }

  // Add to active JSON array
  auto& entries = ovlKeyValue::value_as<array_t>(jsonliteral::active);
  entries.push_back(newEntry->value());

  // Recreate active list
  _active = make_list(entries);

  return Success(msg_Added);
}
```

**Note**: Only checks active list for duplicates, not history.

### Removing Elements
```cpp
template <typename T, std::uint32_t mask>
result_t ovlMovableList<T, mask>::remove(ElementUPtr_t& oldEntry) {
  confirm(oldEntry);

  if (_active.empty()) Success(msg_Ignored);

  // Remove from active array
  auto& entries = ovlKeyValue::value_as<array_t>(jsonliteral::active);
  auto oldCount = entries.size();

  entries.erase(std::remove_if(entries.begin(), entries.end(),
                               [&oldEntry](value_t& entry) -> bool {
                                 auto candidate = std::make_unique<T>(oldEntry->key(), entry);
                                 return candidate->name() == oldEntry->name();
                               }),
                entries.end());

  if (oldCount == entries.size()) return Failure(msg_Missing);

  confirm(oldCount - 1 == entries.size());

  _active = make_list(entries);

  // Move to history array
  auto& history = ovlKeyValue::value_as<array_t>(jsonliteral::history);

  oldCount = history.size();

  // Add removed timestamp
  oldEntry->object_value()[jsonliteral::removed] = timestamp();

  // Add to history
  history.push_back(oldEntry->value());

  confirm(oldCount + 1 == history.size());

  _history = make_list(history);

  return Success(msg_Removed);
}
```

**Removal Process**:
1. Remove from active array
2. Add "removed" timestamp to entry
3. Add entry to history array
4. Recreate both overlay lists

**Historical Preservation**: The complete entry moves to history, preserving all fields plus removal timestamp.

### Wiping
```cpp
template <typename T, std::uint32_t mask>
void ovlMovableList<T, mask>::wipe() {
  _active = ovlMovableList::List_t{};
  _history = ovlMovableList::List_t{};
}
```

Clears both active and history lists.

### Serialization
```cpp
template <typename T, std::uint32_t mask>
std::string ovlMovableList<T, mask>::to_string() const {
  std::ostringstream oss;
  oss << "{" << quoted_(key()) << ": {";

  oss << quoted_(jsonliteral::active) << ": [";
  for (auto const& entry : _active) oss << "\n" << entry.to_string() << ",";
  if (!_active.empty()) oss.seekp(-1, oss.cur);
  oss << "\n],";

  oss << quoted_(jsonliteral::history) << ": [";
  for (auto const& entry : _history) oss << "\n" << entry.to_string() << ",";
  if (!_history.empty()) oss.seekp(-1, oss.cur);
  oss << "\n]\n}\n}";

  return oss.str();
}
```

**Output**: Shows both active and history arrays in JSON format.

### Comparison
```cpp
template <typename T, std::uint32_t mask>
result_t ovlMovableList<T, mask>::operator==(ovlMovableList const& other) const {
  if ((useCompareMask() & mask) == mask) return Success();

  std::ostringstream oss;
  oss << "\n " << key() << "nodes disagree.";
  auto noerror_pos = oss.tellp();

  if (_active.size() != other._active.size())
    oss << "\n  Different active " << key() << " count: self,other="
        << _active.size() << "," << other._active.size();

  auto key_name = key();

  if (oss.tellp() == noerror_pos &&
      std::equal(_active.cbegin(), _active.end(), other._active.cbegin(),
                 [&oss, &key_name](auto const& first, auto const& second) -> bool {
                   auto result = first == second;
                   if (result.first) return true;
                   oss << "\n  " << key_name << " different: self,other="
                       << first.to_string() << "," << second.to_string();
                   return false;
                 }))
    return Success();

  oss << "\n  Debug info:";
  oss << "\n  Self  value:\n" << to_string();
  oss << "\n  Other value:\n" << other.to_string();

  return Failure(oss);
}
```

**Note**: Compares only active lists, not history. History differences are ignored.

## Usage Examples

### Managing Aliases
```cpp
value_t recordJson;
auto aliases = overlay<ovlAliases>(recordJson, "aliases");

// Add an alias
auto newAlias = std::make_unique<ovlAlias>(/*...*/);
newAlias->name("production");
newAlias->assigned() = timestamp();
aliases->add(newAlias);

// Remove an alias (moves to history)
auto oldAlias = std::make_unique<ovlAlias>(/*...*/);
oldAlias->name("production");
aliases->remove(oldAlias);

// Iterate active aliases only
for (auto const& alias : *aliases) {
  std::cout << "Active alias: " << alias.name() << std::endl;
}
```

### Accessing History
```cpp
// History is private, access via JSON if needed
auto& aliasObj = recordJson["aliases"];
auto& history = aliasObj["history"];
// Process history array...
```

## Design Rationale

### Why Separate Active/History?

**Advantages**:
1. **Complete History** - Full entry preserved, not just name/timestamp
2. **Queryable** - Can search history for when aliases were active
3. **Audit Trail** - Maintains complete record of changes
4. **Restoration** - Can potentially restore from history

**Disadvantages**:
1. **More Storage** - Keeps removed entries
2. **More Complex** - Two arrays to manage
3. **Slower** - More data to process

### Why Only for Aliases?

Aliases represent production state:
- Need to know what alias pointed to historically
- Critical for reproducing past runs
- History helps debugging production issues

Other entities (configurations, entities) don't need this level of history - their removal is tracked via timestamp alone.

### Why Compare Only Active?

Two records are "equal" if their current state matches:
- Historical differences don't affect current functionality
- Reduces false "different" results
- Focuses on functional equality

## Performance Considerations

- **Add**: O(n) duplicate check + O(1) append + O(n) list recreation
- **Remove**: O(n) search + O(n) erase + O(1) history append + O(n) recreations
- **Iteration**: Only over active list
- **Storage**: O(n + h) where n=active, h=history

## Related Files

- **ovlKeyValue.h** - Base class
- **ovlFixedList.h** - Simpler alternative without history
- **ovlDatabaseRecord.h** - Uses for ovlAliases
- **common.h** - Mask constants

## Best Practices

1. Use for data requiring historical tracking
2. Keep history reasonable size (consider archiving old history)
3. Don't assume iterators cover history (only active)
4. Use ovlFixedList if history not needed
5. Document why history tracking is necessary

## Notes

- Header-only template
- Final class (cannot be derived from)
- Comparison ignores history list
- Iterator support only for active list
- History never automatically cleaned
- JSON structure is object with two arrays, not a single array
- Used exclusively for aliases in current codebase
- Could be used for other entities requiring audit trails
