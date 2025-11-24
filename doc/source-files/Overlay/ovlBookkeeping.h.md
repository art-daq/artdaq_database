# ovlBookkeeping.h

## File Overview

Overlay class managing database record bookkeeping metadata including readonly/deleted flags, creation timestamp, and complete update history. This class is central to tracking record lifecycle and audit trail.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/ovlBookkeeping.h`

## Purpose

Provides comprehensive tracking of record state and history:
- **State Management**: Readonly and deleted flags prevent unauthorized modifications
- **Creation Tracking**: Timestamp of record creation
- **Update History**: Complete audit trail of all changes
- **Access Control**: Enforces readonly policy before modifications

## Class Definition

```cpp
class ovlBookkeeping final : public ovlKeyValue {
  using updates_t = array_t::container_type<ovlUpdate>;

 public:
  ovlBookkeeping(object_t::key_type const& key, value_t& bookkeeping);

  // accessors
  bool& isReadonly();
  bool const& isReadonly() const;
  bool& isDeleted();
  bool const& isDeleted() const;

  // utils
  bool& markReadonly(bool const& state);
  bool& markDeleted(bool const& state);

  template <typename T>
  result_t postUpdate(std::string const& name, T const& what);

  // overrides
  std::string to_string() const override;

  // ops
  result_t operator==(ovlBookkeeping const&) const;

 private:
  updates_t make_updates(value_t& value);
  ovlTimeStamp map_created(value_t& value);
  bool init(value_t& parent);

 private:
  bool _initOK;
  updates_t _updates;
  ovlTimeStamp _created;
};
```

## JSON Structure

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

## Implementation

### Constructor and Initialization
```cpp
ovlBookkeeping::ovlBookkeeping(object_t::key_type const& key, value_t& bookkeeping)
    : ovlKeyValue(key, bookkeeping),
      _initOK(init(bookkeeping)),
      _updates(make_updates(bookkeeping)),
      _created(map_created(bookkeeping)) {}

bool ovlBookkeeping::init(value_t& parent) try {
  confirm(type(parent) == type_t::OBJECT);

  auto& obj = object_value();

  if (obj.count(jsonliteral::isdeleted) == 0) {
    obj[jsonliteral::isdeleted] = false;
  }

  if (obj.count(jsonliteral::isreadonly) == 0) {
    obj[jsonliteral::isreadonly] = false;
  }

  return true;
} catch (...) {
  confirm(false);
  throw;
}
```

**Initialization**:
- Creates `isdeleted` and `isreadonly` fields with false defaults
- Creates empty updates array if missing
- Records creation timestamp

### State Accessors
```cpp
bool& isReadonly() { return value_as<bool>(jsonliteral::isreadonly); }
bool const& isReadonly() const { return value_as<bool>(jsonliteral::isreadonly); }

bool& markReadonly(bool const& state) {
  value_as<bool>(jsonliteral::isreadonly) = state;
  return isReadonly();
}

bool& isDeleted() { return value_as<bool>(jsonliteral::isdeleted); }
bool const& isDeleted() const { return value_as<bool>(jsonliteral::isdeleted); }

bool& markDeleted(bool const& state) {
  value_as<bool>(jsonliteral::isdeleted) = state;
  return isDeleted();
}
```

### Posting Updates (Template Method)
```cpp
template <typename T>
result_t ovlBookkeeping::postUpdate(std::string const& name, T const& what) {
  confirm(!name.empty());
  confirm(what);

  using namespace artdaq::database::result;

  if (isReadonly()) return Failure(msg_IsReadonly);

  auto& updates = ovlKeyValue::value_as<array_t>(jsonliteral::updates);

  auto newEntry = object_t{};
  newEntry[jsonliteral::event] = name;
  newEntry[jsonliteral::timestamp] = artdaq::database::timestamp();

  auto const& refvalue = what->value();

  if (type(refvalue) == type_t::OBJECT) {
    newEntry[jsonliteral::value] = refvalue;
  } else {
    auto obj = object_t{};
    obj[jsonliteral::name] = refvalue;
    obj[jsonliteral::assigned] = artdaq::database::timestamp();
    newEntry[jsonliteral::value] = obj;
  }

  value_t tmp = newEntry;
  updates.push_back(tmp);

  // Reattach AST
  _updates = make_updates(value());

  return Success(msg_Added);
}
```

**Template Parameter `T`**: Typically a unique_ptr to an overlay object.

**Update Entry Creation**:
1. Checks readonly status
2. Creates update entry with event name and timestamp
3. Handles both object and primitive value types
4. Appends to updates array
5. Recreates overlay list

### Serialization
```cpp
std::string to_string() const {
  std::ostringstream oss;
  oss << "{" << quoted_(jsonliteral::bookkeeping) << ": {\n";
  oss << quoted_(jsonliteral::isreadonly) << ":" << bool_(isReadonly()) << ",\n";
  oss << quoted_(jsonliteral::isdeleted) << ":" << bool_(isDeleted()) << ",\n";
  oss << debrace(_created.to_string()) << ",\n";
  oss << quoted_(jsonliteral::updates) << ": [";

  for (auto const& update : _updates) {
    oss << "\n" << update.to_string() << ",";
  }

  if (!_updates.empty()) {
    oss.seekp(-1, oss.cur);  // Remove trailing comma
  }

  oss << "\n]\n}\n}";

  return oss.str();
}
```

### Comparison
```cpp
result_t operator==(ovlBookkeeping const& other) const {
  if ((useCompareMask() & DOCUMENT_COMPARE_MUTE_BOOKKEEPING) == DOCUMENT_COMPARE_MUTE_BOOKKEEPING) {
    return Success();
  }

  std::ostringstream oss;
  oss << "\nBookkeeping nodes disagree.";
  auto noerror_pos = oss.tellp();

  if (isDeleted() != other.isDeleted()) {
    oss << "\n  isdeleted flags are different: self,other="
        << bool_(isDeleted()) << "," << bool_(other.isDeleted());
  }

  if (isReadonly() != other.isReadonly()) {
    oss << "\n  isreadonly flags are different: self,other="
        << bool_(isReadonly()) << "," << bool_(other.isReadonly());
  }

  auto result = _created == other._created;

  if (!result.first) {
    oss << "\n  Timestamps are different: self,other="
        << quoted_(_created.timestamp()) << "," << quoted_(other._created.timestamp());
  }

  if (oss.tellp() == noerror_pos && (useCompareMask() & DOCUMENT_COMPARE_MUTE_UPDATES) == DOCUMENT_COMPARE_MUTE_UPDATES) {
    return Success();
  }

  if (_updates.size() != other._updates.size()) {
    oss << "\n  Record update histories have different size: self,other="
        << _updates.size() << "," << other._updates.size();
  }

  if (oss.tellp() == noerror_pos &&
      std::equal(_updates.cbegin(), _updates.end(), other._updates.cbegin(),
                 [&oss](auto const& first, auto const& second) -> bool {
                   auto result = first == second;
                   if (result.first) {
                     return true;
                   }
                   oss << "\n  Record update histories are different: self,other="
                       << first.to_string() << "," << second.to_string();
                   return false;
                 })) {
    return Success();
  }

  oss << "\n  Debug info:";
  oss << "\n  Self  value:\n" << to_string();
  oss << "\n  Other value:\n" << other.to_string();

  return Failure(oss);
}
```

**Comparison Logic**:
1. Check `DOCUMENT_COMPARE_MUTE_BOOKKEEPING` mask (ignore all)
2. Compare flags (readonly, deleted)
3. Compare creation timestamps
4. Check `DOCUMENT_COMPARE_MUTE_UPDATES` mask (ignore update history)
5. Compare update arrays element-by-element

## Usage Examples

### Checking Record State
```cpp
ovlDatabaseRecord record{json};

if (record.bookkeeping().isReadonly()) {
  std::cout << "Record is readonly, modifications not allowed" << std::endl;
  return;
}

if (record.bookkeeping().isDeleted()) {
  std::cout << "Record is marked as deleted" << std::endl;
  return;
}
```

### Marking Record Readonly
```cpp
auto& bookkeeping = record.bookkeeping();
bookkeeping.markReadonly(true);

// Future modifications will fail
auto result = record.addConfiguration(config);
// result.first == false, result.second == msg_IsReadonly
```

### Posting Updates
```cpp
auto& bookkeeping = record.bookkeeping();

// Add configuration update
auto config = std::make_unique<ovlConfiguration>(/*...*/);
config->name("DAQConfiguration");

auto result = bookkeeping.postUpdate("addConfiguration", config);

if (result.first) {
  std::cout << "Update recorded: " << result.second << std::endl;
}
```

### Checking Creation Time
```cpp
auto& bookkeeping = record.bookkeeping();
std::cout << "Record created: " << bookkeeping._created.timestamp() << std::endl;
// Note: _created is private, access through serialization or derived accessor
```

## Design Rationale

### Readonly Protection

**Why readonly flag?**
- Prevents accidental modification of production records
- Enforces immutability for archived configurations
- Provides access control mechanism
- Can be used for version control (mark old versions readonly)

**Enforcement**: Operations check readonly status and return failure if set.

### Deleted vs Actual Deletion

**Why mark deleted rather than delete?**
- Preserves audit trail
- Allows "undelete" operations
- Maintains referential integrity
- Enables soft-delete pattern for recovery

### Complete Update History

**Why track all updates?**
- Audit trail for compliance
- Debugging - understand how record evolved
- Rollback capability - can reconstruct past states
- Performance analysis - identify frequently modified records

### Template postUpdate Method

**Why template?**
- Accepts any overlay type (Configuration, Entity, etc.)
- Type-safe - compiler ensures correct usage
- Flexible - works with both object and primitive values
- Single implementation for all update types

## State Lifecycle

```
Created (readonly=false, deleted=false)
    |
    v
Modified (updates added)
    |
    +---> Marked Readonly (readonly=true)
    |         |
    |         v
    |     Immutable (modifications fail)
    |
    +---> Marked Deleted (deleted=true)
          |
          v
      Soft-Deleted (not physically removed)
```

## Common Patterns

### Safe Modification
```cpp
auto& bk = record.bookkeeping();

if (bk.isReadonly() || bk.isDeleted()) {
  return Failure("Cannot modify protected record");
}

// Proceed with modifications
record.addConfiguration(config);
```

### Tracking Changes
```cpp
// Before modification
auto& bk = record.bookkeeping();
size_t beforeCount = bk._updates.size();

// Make changes
record.setVersion(newVersion);
record.addEntity(entity);

// After modification
size_t afterCount = bk._updates.size();
std::cout << "Added " << (afterCount - beforeCount) << " updates" << std::endl;
```

## Performance Considerations

- **postUpdate**: O(1) append + O(n) list recreation
- **State checks**: O(1) boolean access
- **Comparison**: O(n) where n = number of updates

## Thread Safety

Not thread-safe:
- References mutable JSON
- Updates modify shared array
- No synchronization

## Related Files

- **ovlKeyValue.h** - Base class
- **ovlUpdate.h** - Update entry type
- **ovlTimeStamp.h** - Creation timestamp
- **ovlDatabaseRecord.h** - Uses bookkeeping for access control
- **common.h** - Mask constants

## Best Practices

1. **Always check readonly/deleted** before modifications
2. **Use markReadonly** for production records
3. **Post updates** for all significant changes
4. **Provide meaningful event names** in postUpdate
5. **Don't manually modify updates array** - use postUpdate
6. **Consider masking in tests** - use DOCUMENT_COMPARE_MUTE_BOOKKEEPING

## Notes

- Final class (cannot be derived from)
- Central to record access control
- Updates array grows unbounded (consider archiving strategy)
- Comparison supports multiple masking levels
- Template method enables type-safe update posting
- Creation timestamp auto-generated if missing
- Readonly enforcement is policy-based, not enforced at JSON level
