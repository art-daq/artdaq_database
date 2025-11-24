# ovlFixedList.h

## File Overview

Header-only template class providing an overlay for JSON arrays containing named elements. Supports adding and removing elements while maintaining a single active list. Called "Fixed" to distinguish from "Movable" lists which maintain separate active/history lists.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/ovlFixedList.h`

## Purpose

Manages collections of named overlay objects (configurations, entities, comments, runs, attachments) with:
- Type-safe element access
- Add/remove operations with duplicate prevention
- Comparison with masking support
- Iterator access for range-based loops

## Template Definition

```cpp
template <typename T, std::uint32_t mask>
class ovlFixedList final : public ovlKeyValue {
  using List_t = array_t::container_type<T>;
  using ElementUPtr_t = std::unique_ptr<T>;

 public:
  ovlFixedList(object_t::key_type const& key, value_t& array);

  // utils
  void wipe();
  result_t add(ElementUPtr_t&);
  result_t remove(ElementUPtr_t&);

  typename List_t::const_iterator begin() const { return _list.begin(); }
  typename List_t::const_iterator end() const { return _list.end(); }

  // overrides
  std::string to_string() const override;

  // ops
  result_t operator==(ovlFixedList const&) const;

 private:
  List_t make_list(array_t& array);
  bool init(value_t& parent);

 private:
  bool _initOK;
  List_t _list;
};
```

## Template Parameters

| Parameter | Type | Purpose |
|-----------|------|---------|
| `T` | Element type | Overlay class for list elements (e.g., `ovlConfiguration`) |
| `mask` | `std::uint32_t` | Comparison masking flag |

## Type Aliases in ovlDatabaseRecord.h

```cpp
using ovlConfigurations = ovlFixedList<ovlConfiguration, DOCUMENT_COMPARE_MUTE_CONFIGURATION>;
using ovlEntities = ovlFixedList<ovlEntity, DOCUMENT_COMPARE_MUTE_CONFIGENTITY>;
using ovlRuns = ovlFixedList<ovlRun, DOCUMENT_COMPARE_MUTE_RUN>;
using ovlComments = ovlFixedList<ovlComment, DOCUMENT_COMPARE_MUTE_COMMENTS>;
using ovlAttachments = ovlFixedList<ovlAttachment, DOCUMENT_COMPARE_MUTE_ATTACHMENT>;
```

## Implementation

### Constructor
```cpp
template <typename T, std::uint32_t mask>
ovlFixedList<T, mask>::ovlFixedList(object_t::key_type const& key, value_t& entries)
    : ovlKeyValue(key, entries), _list(make_list(ovlKeyValue::array_value())) {}
```

Creates overlay wrappers for each element in the JSON array.

### List Creation
```cpp
template <typename T, std::uint32_t mask>
typename ovlFixedList<T, mask>::List_t ovlFixedList<T, mask>::make_list(array_t& entries) {
  auto returnValue = ovlFixedList<T, mask>::List_t{};

  for (auto& entry : entries)
    returnValue.push_back({jsonliteral::value, entry});

  return returnValue;
}
```

Wraps each JSON array element in the appropriate overlay type.

### Adding Elements
```cpp
template <typename T, std::uint32_t mask>
result_t ovlFixedList<T, mask>::add(ElementUPtr_t& newEntry) {
  confirm(newEntry);

  // Check for duplicates
  for (auto& entry : _list) {
    if (entry.name() == newEntry->name()) {
      return Success(msg_Ignored);  // Already exists
    }
  }

  // Add to JSON array
  auto& entries = ovlKeyValue::array_value();
  entries.push_back(newEntry->value());

  // Recreate overlay list
  _list = make_list(entries);

  return Success(msg_Added);
}
```

**Behavior**:
- Checks for duplicate names (ignores if exists)
- Adds to underlying JSON array
- Recreates overlay wrappers to maintain references
- Returns success with appropriate message

### Removing Elements
```cpp
template <typename T, std::uint32_t mask>
result_t ovlFixedList<T, mask>::remove(ElementUPtr_t& oldEntry) {
  confirm(oldEntry);

  if (_list.empty()) Success(msg_Ignored);

  auto& entries = ovlKeyValue::array_value();
  auto oldCount = entries.size();

  // Remove matching entry
  entries.erase(std::remove_if(entries.begin(), entries.end(),
                               [&oldEntry](value_t& entry) -> bool {
                                 auto candidateEntry = std::make_unique<T>(jsonliteral::value, entry);
                                 return candidateEntry->name() == oldEntry->name();
                               }),
                entries.end());

  if (oldCount == entries.size()) return Failure(msg_Missing);

  confirm(oldCount == entries.size() + 1);

  // Add removed timestamp
  oldEntry->object_value()[jsonliteral::removed] = timestamp();

  _list = make_list(entries);

  return Success(msg_Removed);
}
```

**Behavior**:
- Searches array for matching name
- Uses erase-remove idiom for efficient removal
- Adds "removed" timestamp to the removed entry
- Recreates overlay list
- Returns failure if not found

### Wiping List
```cpp
template <typename T, std::uint32_t mask>
void ovlFixedList<T, mask>::wipe() {
  auto& entries = ovlKeyValue::array_value();
  auto empty = array_t{};
  entries.swap(empty);
  _list = make_list(entries);
}
```

Clears all elements from the list.

### Serialization
```cpp
template <typename T, std::uint32_t mask>
std::string ovlFixedList<T, mask>::to_string() const {
  std::ostringstream oss;
  oss << "{" << quoted_(key()) << ": [";

  for (auto const& entry : _list) oss << "\n" << entry.to_string() << ",";

  if (!_list.empty()) oss.seekp(-1, oss.cur);  // Remove trailing comma

  oss << "\n]}";

  return oss.str();
}
```

### Comparison
```cpp
template <typename T, std::uint32_t mask>
result_t ovlFixedList<T, mask>::operator==(ovlFixedList const& other) const {
  if ((useCompareMask() & mask) == mask) return Success();

  std::ostringstream oss;
  oss << "\nEntry lists disagree.";
  auto noerror_pos = oss.tellp();

  if (_list.size() != other._list.size())
    oss << "\n  Entry " << key() << " have different size: self,other="
        << _list.size() << "," << other._list.size();

  if (oss.tellp() == noerror_pos &&
      std::equal(_list.cbegin(), _list.end(), other._list.cbegin(),
                 [&oss](auto const& first, auto const& second) -> bool {
                   auto result = first == second;
                   if (result.first) return true;
                   oss << result.second;
                   return false;
                 }))
    return Success();
  else
    oss << "\n  Entry lists are different";

  oss << "\n  Debug info:";
  oss << "\n  Self  value: " << to_string();
  oss << "\n  Other value: " << other.to_string();

  return Failure(oss);
}
```

**Comparison Logic**:
1. Check mask - if masked, always equal
2. Compare list sizes
3. Use `std::equal` with custom comparator
4. Aggregate error messages from element comparisons

### Iterator Support
```cpp
typename List_t::const_iterator begin() const { return _list.begin(); }
typename List_t::const_iterator end() const { return _list.end(); }
```

Enables range-based for loops:
```cpp
ovlConfigurations configs(...);
for (auto const& config : configs) {
  std::cout << config.name() << std::endl;
}
```

## Usage Examples

### Managing Configurations
```cpp
value_t recordJson;
auto configs = overlay<ovlConfigurations, array_t>(recordJson, "configurations");

// Add a configuration
auto newConfig = std::make_unique<ovlConfiguration>(/*...*/);
newConfig->name("DAQConfig");
auto result = configs->add(newConfig);

if (result.first) {
  std::cout << "Added: " << result.second << std::endl;
}

// Iterate through configurations
for (auto const& config : *configs) {
  std::cout << "Config: " << config.name() << std::endl;
}

// Remove a configuration
auto removeConfig = std::make_unique<ovlConfiguration>(/*...*/);
removeConfig->name("DAQConfig");
configs->remove(removeConfig);
```

### Managing Entities
```cpp
auto entities = overlay<ovlEntities, array_t>(recordJson, "entities");

// Add entity
auto entity = std::make_unique<ovlEntity>(/*...*/);
entity->name("BoardReader01");
entities->add(entity);

// Check contents
std::cout << "Entity count: " << std::distance(entities->begin(), entities->end()) << std::endl;
```

## Design Rationale

### Why "Fixed" List?

Called "Fixed" to distinguish from `ovlMovableList`:
- **Fixed**: Single array, removed items deleted
- **Movable**: Two arrays (active/history), removed items moved to history

### Duplicate Prevention

Adding duplicate names returns success with "Ignored" message:
- Prevents accidental duplicates
- Idempotent operation
- Caller can check message to determine if actually added

### List Reconstruction

After modifying the JSON array, the overlay list is reconstructed:
- Maintains correct references to JSON
- Simple, predictable behavior
- Avoids complex reference tracking

### Removed Timestamp

When removing, adds a "removed" timestamp:
- Tracks when removal occurred
- Useful for audit trails
- Doesn't prevent actual removal from array

## Performance Considerations

- **Add**: O(n) duplicate check + O(1) append + O(n) list recreation = O(n)
- **Remove**: O(n) search + O(n) erase + O(n) list recreation = O(n)
- **Iteration**: O(1) begin/end, O(n) full iteration
- **Comparison**: O(n) element-wise comparison

## Related Files

- **ovlKeyValue.h** - Base class
- **ovlMovableList.h** - Alternative with history tracking
- **ovlDatabaseRecord.h** - Defines list type aliases
- **common.h** - Mask constants

## Best Practices

1. Use type aliases (ovlConfigurations) not raw template
2. Check result of add/remove operations
3. Use range-based for loops for iteration
4. Let template handle JSON array synchronization
5. Don't store iterators across modifications

## Notes

- Header-only template
- Final class (cannot be derived from)
- Elements must have `name()` method
- Duplicate detection by name only
- List recreation ensures reference correctness
- Supports STL algorithms via iterators
