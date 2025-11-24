# ovlUpdate.h

## File Overview

Overlay class representing a single update event in a record's update history. Tracks what changed, when it changed, and the event name describing the change.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/ovlUpdate.h`

## Class Definition

```cpp
class ovlUpdate final : public ovlKeyValue {
 public:
  ovlUpdate(object_t::key_type const& key, value_t& update);

  // accessors
  std::string& name();
  std::string const& name() const;
  std::string& timestamp();
  std::string const& timestamp() const;

  ovlUpdateEntry& what();
  ovlUpdateEntry const& what() const;

  // overrides
  std::string to_string() const override;

  // ops
  result_t operator==(ovlUpdate const&) const;

 private:
  ovlUpdateEntry map_what(value_t& value);
  ovlTimeStamp map_timestamp(value_t& value);

 private:
  ovlTimeStamp _timestamp;
  ovlUpdateEntry _what;
};
```

## Component Types

```cpp
using ovlUpdateEntry = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_UPDATE_VALUES>;
```

An update entry is a named value with timestamp tracking.

## Implementation

### Constructor
```cpp
ovlUpdate::ovlUpdate(object_t::key_type const& key, value_t& update)
    : ovlKeyValue(key, update),
      _timestamp(map_timestamp(update)),
      _what(map_what(update)) {}
```

### Mapping Methods
```cpp
ovlTimeStamp map_timestamp(value_t& value) {
  confirm(type(value) == type_t::OBJECT);
  auto& obj = object_value();
  confirm(obj.count(jsonliteral::timestamp) == 1);
  return ovlTimeStamp(jsonliteral::timestamp, obj.at(jsonliteral::timestamp));
}

ovlUpdateEntry map_what(value_t& value) {
  confirm(type(value) == type_t::OBJECT);
  auto& obj = object_value();
  confirm(obj.count(jsonliteral::value) == 1);
  return ovlUpdateEntry(jsonliteral::value, obj.at(jsonliteral::value));
}
```

### Accessor Methods
```cpp
std::string& name() { return value_as<std::string>(jsonliteral::event); }
std::string& timestamp() { return _timestamp.timestamp(); }
ovlUpdateEntry& what() { return _what; }
```

### Serialization
```cpp
std::string to_string() const {
  std::ostringstream oss;
  oss << "{";
  oss << quoted_(jsonliteral::event) << ":" << quoted_(name()) << ",";
  oss << debrace(_timestamp.to_string()) << ",";
  oss << debrace(_what.to_string());
  oss << "}";
  return oss.str();
}
```

**Example Output**:
```json
{
  "event": "addConfiguration",
  "timestamp": "2025-01-15T10:30:00Z",
  "value": {"name": "DAQConfig", "assigned": "2025-01-15T10:30:00Z"}
}
```

### Comparison
```cpp
result_t operator==(ovlUpdate const& other) const {
  // Compares timestamp, event name, and event data
  // Respects DOCUMENT_COMPARE_MUTE_TIMESTAMPS mask
}
```

## JSON Structure

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

## Usage

```cpp
value_t updateJson;
updateJson["event"] = "addEntity";
updateJson["timestamp"] = timestamp();
updateJson["value"]["name"] = "BoardReader01";

ovlUpdate update("update", updateJson);

std::cout << "Event: " << update.name() << std::endl;
std::cout << "When: " << update.timestamp() << std::endl;
std::cout << "What: " << update.what().name() << std::endl;
```

## Update Event Types

Common event names:
- `"addConfiguration"` - Configuration added
- `"removeConfiguration"` - Configuration removed
- `"addEntity"` - Entity added
- `"removeEntity"` - Entity removed
- `"addAlias"` - Alias added
- `"removeAlias"` - Alias removed
- `"setVersion"` - Version changed
- `"setCollection"` - Collection changed
- `"addRun"` - Run added

## Related Files

- **ovlKeyValue.h** - Base class
- **ovlTimeStamp.h** - For timestamp tracking
- **ovlKeyValueTimeStamp.h** - For update entry
- **ovlBookkeeping.h** - Uses ovlUpdate in update history
- **common.h** - Mask constants

## Notes

- Final class
- Used in ovlBookkeeping update arrays
- Tracks complete audit trail of record changes
- Comparison can mask timestamp differences
- Event names are free-form strings
