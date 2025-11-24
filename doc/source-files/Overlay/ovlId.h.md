# ovlId.h

## File Overview

Overlay class for database record unique identifiers (Object IDs). Manages OID generation, validation, and access for database records, ensuring each record has a unique identifier.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/ovlId.h`

## Class Definition

```cpp
class ovlId final : public ovlKeyValue {
 public:
  ovlId(object_t::key_type const& key, value_t& oid);

  bool newId();

  // accessors
  std::string& oid();
  std::string& oid(std::string const& id);

  // ops
  result_t operator==(ovlId const&) const;

 private:
  bool init(value_t& parent);
  bool _initOK;
};
```

## Implementation

### Constructor and Initialization
```cpp
ovlId::ovlId(object_t::key_type const& key, value_t& oid)
    : ovlKeyValue(key, oid), _initOK(init(oid)) {}

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

**Auto-Generation**: If no OID exists, generates one automatically using `generate_oid()`.

### Accessor Methods
```cpp
std::string& oid() { return value_as<std::string>(jsonliteral::oid); }

std::string& oid(std::string const& id) {
  confirm(id.empty());  // Intentional: only allows setting empty string?
  oid() = id;
  return oid();
}
```

**Note**: The setter has unusual logic - it confirms the ID is empty before setting. This may be intentional to prevent overwriting existing IDs.

### New ID Generation
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

**Purpose**: Forcibly generates a new OID, replacing existing one.

### Comparison
```cpp
result_t operator==(ovlId const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_OUIDS) == DOCUMENT_COMPARE_MUTE_OUIDS)
         ? Success()
         : self() == other.self();
}
```

**Masking**: Can ignore OID differences when `DOCUMENT_COMPARE_MUTE_OUIDS` flag is set.

## Usage

```cpp
value_t recordJson;
auto id = overlay<ovlId>(recordJson, "id");

// Access OID (auto-generated if missing)
std::cout << "OID: " << id->oid() << std::endl;

// Generate new OID
id->newId();

// Compare with masking
useCompareMask(DOCUMENT_COMPARE_MUTE_OUIDS);
auto result = id1 == id2;  // Always true (OUIDs masked)
```

## JSON Structure

```json
{
  "id": {
    "oid": "507f1f77bcf86cd799439011"
  }
}
```

## Related Files

- **ovlKeyValue.h** - Base class
- **ovlDatabaseRecord.h** - Uses ovlId for record identification
- **common.h** - DOCUMENT_COMPARE_MUTE_OUIDS flag

## Notes

- Final class (cannot be derived from)
- Auto-generates OID if missing
- OID setter has unusual empty-check behavior
- Comparison supports OID masking for testing
- OID format determined by `generate_oid()` function
