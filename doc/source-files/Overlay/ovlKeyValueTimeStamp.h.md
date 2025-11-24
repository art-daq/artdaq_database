# ovlKeyValueTimeStamp.h

## File Overview

Template class combining a name field with optional timestamp fields (assigned/removed). Used for tracking named entities with temporal metadata such as configurations, aliases, entities, and runs.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/ovlKeyValueTimeStamp.h`

## Purpose

Provides a reusable pattern for JSON objects that have:
- A required `name` field
- Optional `assigned` timestamp
- Optional `removed` timestamp
- Maskable comparison

## Template Definition

```cpp
template <std::uint32_t mask, bool A = true, bool R = false>
class ovlKeyValueTimeStamp final : public ovlKeyValue {
 public:
  ovlKeyValueTimeStamp(object_t::key_type const& key, value_t& value);

  // accessors
  std::string& name();
  std::string const& name() const;
  std::string& name(std::string const& name);

  std::string& assigned();  // Available if A == true
  std::string& removed();   // Available if R == true

  // overrides
  std::string to_string() const override;

  // ops
  result_t operator==(ovlKeyValueTimeStamp const&) const;

 private:
  ovlTimeStampUPtr_t map_assigned(value_t& value);
  ovlTimeStampUPtr_t map_removed(value_t& value);
  bool init(value_t& parent);

 private:
  bool _initOK;
  ovlTimeStampUPtr_t _assigned;
  ovlTimeStampUPtr_t _removed;
};
```

## Template Parameters

| Parameter | Type | Default | Purpose |
|-----------|------|---------|---------|
| `mask` | `std::uint32_t` | Required | Comparison masking flag |
| `A` | `bool` | `true` | Include "assigned" timestamp |
| `R` | `bool` | `false` | Include "removed" timestamp |

## Type Aliases in ovlDatabaseRecord.h

```cpp
// Standard name + assigned
using ovlConfiguration = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_CONFIGURATION>;
using ovlRun = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_RUN>;
using ovlEntity = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_CONFIGENTITY>;

// Name + assigned + removed (for tracking deletions)
using ovlAlias = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_ALIAS, true, true>;
```

## Implementation

### Constructor and Initialization
```cpp
template <std::uint32_t mask, bool A, bool R>
ovlKeyValueTimeStamp<mask, A, R>::ovlKeyValueTimeStamp(object_t::key_type const& key,
                                                        value_t& value)
    : ovlKeyValue(key, value),
      _initOK(init(value)),
      _assigned(map_assigned(value)),
      _removed(map_removed(value)) {}

template <std::uint32_t mask, bool A, bool R>
bool ovlKeyValueTimeStamp<mask, A, R>::init(value_t& parent) try {
  confirm(type(parent) == type_t::OBJECT);

  auto& obj = object_value();

  confirm(obj.count(jsonliteral::name) == 1);  // Name must exist

  if (A) {
    if (obj.count(jsonliteral::assigned) == 0)
      obj[jsonliteral::assigned] = timestamp();
    confirm(obj.count(jsonliteral::assigned) == 1);
  }

  if (R) {
    if (obj.count(jsonliteral::removed) == 0)
      obj[jsonliteral::removed] = timestamp();
    confirm(obj.count(jsonliteral::removed) == 1);
  }

  return true;
} catch (...) {
  confirm(false);
  throw;
}
```

**Initialization Logic**:
1. Confirms object has required "name" field
2. If `A == true`: Creates "assigned" timestamp if missing
3. If `R == true`: Creates "removed" timestamp if missing

### Name Accessors
```cpp
template <std::uint32_t mask, bool A, bool R>
std::string& ovlKeyValueTimeStamp<mask, A, R>::name() {
  return value_as<std::string>(jsonliteral::name);
}

template <std::uint32_t mask, bool A, bool R>
std::string& ovlKeyValueTimeStamp<mask, A, R>::name(std::string const& name) {
  confirm(!name.empty());
  auto& returnValue = value_as<std::string>(jsonliteral::name);
  returnValue = name;
  return returnValue;
}
```

### Timestamp Accessors
```cpp
template <std::uint32_t mask, bool A, bool R>
std::string& ovlKeyValueTimeStamp<mask, A, R>::assigned() {
  confirm(A);  // Runtime check that template allows this
  return _assigned->timestamp();
}

template <std::uint32_t mask, bool A, bool R>
std::string& ovlKeyValueTimeStamp<mask, A, R>::removed() {
  confirm(R);  // Runtime check that template allows this
  return _removed->timestamp();
}
```

**Safety**: Accessing timestamps not enabled by template parameters triggers assertion.

### Serialization
```cpp
template <std::uint32_t mask, bool A, bool R>
std::string ovlKeyValueTimeStamp<mask, A, R>::to_string() const {
  std::ostringstream oss;
  oss << "{";
  oss << quoted_(jsonliteral::name) << ":" << quoted_(name());

  if (A) oss << "," << debrace(_assigned->to_string());
  if (R) oss << "," << debrace(_removed->to_string());

  oss << "}";
  return oss.str();
}
```

**Example Output**:
```json
{"name": "myconfig", "assigned": "2025-01-15T10:30:00Z"}
```

### Comparison
```cpp
template <std::uint32_t mask, bool A, bool R>
result_t ovlKeyValueTimeStamp<mask, A, R>::operator==(ovlKeyValueTimeStamp const& other) const {
  std::ostringstream oss;
  oss << "\n" << key() << " records disagree.";
  auto noerror_pos = oss.tellp();

  if (name() != other.name())
    oss << "\n  " << key() << "names are different: self,other="
        << quoted_(name()) << "," << quoted_(other.name());

  if (A) {
    auto result = *_assigned == *(other._assigned);
    if (!result.first)
      oss << "\n  Assigned timestamps are different:\n  " << result.second;
  }

  if (R) {
    auto result = *_removed == *(other._assigned);
    if (!result.first)
      oss << "\n  Assigned timestamps are different:\n  " << result.second;
  }

  if (oss.tellp() == noerror_pos) return Success();

  oss << "\n  Debug info:";
  oss << "\n  Self  value: " << to_string();
  oss << "\n  Other value: " << other.to_string();

  return Failure(oss);
}
```

## Usage Examples

### Configuration Entry
```cpp
value_t configJson;
configJson["name"] = "DAQConfiguration";
configJson["assigned"] = timestamp();

ovlConfiguration config("configuration", configJson);
std::cout << "Config: " << config.name() << std::endl;
std::cout << "Assigned: " << config.assigned() << std::endl;
```

### Alias with Removal Tracking
```cpp
value_t aliasJson;
aliasJson["name"] = "production";
aliasJson["assigned"] = timestamp();
aliasJson["removed"] = timestamp();  // Will be updated when removed

ovlAlias alias("alias", aliasJson);
// Can track both when assigned and when removed
```

### Entity Tracking
```cpp
value_t entityJson;
entityJson["name"] = "BoardReader01";
// assigned timestamp will be auto-created if missing

ovlEntity entity("entity", entityJson);
```

## Design Rationale

### Template Flexibility

The three template parameters provide:
1. **mask** - Different semantic types even with same structure
2. **A (assigned)** - Most entities track when they were added
3. **R (removed)** - Some entities need removal tracking (aliases)

### Default Values for Timestamps

Automatically creating timestamps prevents:
- Missing timestamp errors
- Inconsistent initialization
- Complex initialization code in calling code

### Name Required

The `name` field must already exist - not created by template. This ensures:
- Caller provides meaningful name
- No accidental empty-named entries
- Explicit intent

## Common Use Cases

| Type | Mask | A | R | Purpose |
|------|------|---|---|---------|
| ovlConfiguration | CONFIGURATION | true | false | Track configurations |
| ovlEntity | CONFIGENTITY | true | false | Track configuration entities |
| ovlRun | RUN | true | false | Track data-taking runs |
| ovlAlias | ALIAS | true | true | Track aliases with removal history |
| ovlUpdateEntry | UPDATE_VALUES | true | false | Track update entries |

## Related Files

- **ovlKeyValue.h** - Base class
- **ovlTimeStamp.h** - Used for timestamp fields
- **ovlDatabaseRecord.h** - Defines type aliases
- **common.h** - Mask constants

## Best Practices

1. Use type aliases, not raw template instantiation
2. Choose appropriate template parameters for semantic meaning
3. Ensure "name" field exists before construction
4. Let template handle timestamp initialization
5. Don't call `removed()` on types where `R == false`

## Notes

- Header-only template
- Final class (cannot be derived from)
- Requires name field to pre-exist in JSON
- Auto-creates timestamp fields if missing
- Different template parameters create incompatible types
- Timestamp comparison respects global mask
