# ovlUpdate.cpp

**Path:** `artdaq-database/Overlay/ovlUpdate.cpp`

**Implements:** [ovlUpdate.h](./ovlUpdate.h.md)

**Purpose:** Implements the `ovlUpdate` class, providing mapping of timestamp and value fields to sub-overlays, accessor methods for event components, serialization, and comprehensive comparison for tracking record update events in the audit trail.

## Implementation Overview

This file implements the update overlay that wraps a JSON object representing a single modification event. The implementation creates sub-overlays for the timestamp and value components using mapping methods, provides accessors that delegate to these sub-overlays or directly access JSON fields, and implements detailed comparison logic with mask support.

## Key Algorithms

### Component Mapping

**Steps:**
1. Initialize base class `ovlKeyValue` with the update key and value
2. Create timestamp sub-overlay via `map_timestamp()` method
3. Create update entry sub-overlay via `map_what()` method

**Why this approach:** Using dedicated mapping methods isolates the field extraction and validation logic, making the constructor cleaner and enabling reuse of validation patterns.

### Comparison with Masking

**Steps:**
1. Create output stream for error accumulation
2. Compare timestamps (respects `DOCUMENT_COMPARE_MUTE_TIMESTAMPS`)
3. Compare event names directly
4. If timestamp mask is set, compare only value names (not full values)
5. If timestamp mask is not set, compare full values
6. Return `Success()` if no errors accumulated, otherwise return `Failure()` with details

**Why this approach:** The branching based on timestamp mask allows comparing either the full event data or just the event names, which is useful for testing where timing may differ but semantic content should match.

## Internal Functions

### Constructor

```cpp
ovlUpdate::ovlUpdate(object_t::key_type const& key, value_t& update)
    : ovlKeyValue(key, update),
      _timestamp(map_timestamp(update)),
      _what(map_what(update)) {}
```

**Brief:** Initializes the update overlay by creating sub-overlays for timestamp and value components.

**Called by:** Client code creating update overlays, `ovlBookkeeping` when iterating updates

**Purpose:** Wraps the JSON update structure with type-safe overlays for each component.

### Accessor Methods

```cpp
std::string& ovlUpdate::name() {
  return value_as<std::string>(jsonliteral::event);
}

std::string const& ovlUpdate::name() const {
  return value_as<std::string>(jsonliteral::event);
}

std::string& ovlUpdate::timestamp() {
  return _timestamp.timestamp();
}

std::string const& ovlUpdate::timestamp() const {
  return _timestamp.timestamp();
}

ovlUpdateEntry& ovlUpdate::what() {
  return _what;
}

ovlUpdateEntry const& ovlUpdate::what() const {
  return _what;
}
```

**Brief:** Provide access to update components by delegating to appropriate fields or sub-overlays.

**Called by:** Client code reading or modifying update events

**Purpose:**
- `name()`: Direct access to "event" field via base class `value_as<>()`
- `timestamp()`: Delegation to `_timestamp` sub-overlay
- `what()`: Direct return of `_what` sub-overlay reference

### Mapping Methods

```cpp
ovlTimeStamp ovlUpdate::map_timestamp(value_t& value) {
  confirm(type(value) == type_t::OBJECT);
  auto& obj = object_value();
  confirm(obj.count(jsonliteral::timestamp) == 1);
  return ovlTimeStamp(jsonliteral::timestamp, obj.at(jsonliteral::timestamp));
}

ovlUpdateEntry ovlUpdate::map_what(value_t& value) {
  confirm(type(value) == type_t::OBJECT);
  auto& obj = object_value();
  confirm(obj.count(jsonliteral::value) == 1);
  return ovlUpdateEntry(jsonliteral::value, obj.at(jsonliteral::value));
}
```

**Brief:** Create sub-overlays for specific fields after validating their presence.

**Called by:** Constructor during initialization

**Purpose:** Isolates field extraction and validation logic, ensuring required fields exist before creating sub-overlays.

### Serialization

```cpp
std::string ovlUpdate::to_string() const {
  std::ostringstream oss;
  oss << "{";
  oss << quoted_(jsonliteral::event) << ":" << quoted_(name()) << ",";
  oss << debrace(_timestamp.to_string()) << ",";
  oss << debrace(_what.to_string());
  oss << "}";
  return oss.str();
}
```

**Brief:** Produces a JSON representation by combining event name with serialized sub-components.

**Called by:** Debugging and logging code, parent overlay serialization

**Purpose:** Creates human-readable JSON output using `debrace()` to merge sub-component output.

### Comparison Operator

```cpp
result_t ovlUpdate::operator==(ovlUpdate const& other) const {
  std::ostringstream oss;
  oss << "\nUpdate events disagree.";
  auto noerror_pos = oss.tellp();

  auto result = _timestamp == other._timestamp;
  if (!result.first) {
    oss << "\n  Timestamps are different: self,other="
        << quoted_(_timestamp.timestamp()) << "," << quoted_(other._timestamp.timestamp());
  }

  if (name() != other.name()) {
    oss << "\n  Events are different: self,other="
        << quoted_(name()) << "," << quoted_(other.name());
  }

  if ((useCompareMask() & DOCUMENT_COMPARE_MUTE_TIMESTAMPS) == DOCUMENT_COMPARE_MUTE_TIMESTAMPS) {
    auto const& name = _what.value_as<std::string>(jsonliteral::name);
    auto const& otherName = other._what.value_as<std::string>(jsonliteral::name);
    if (name != otherName) {
      oss << "\n  Event names are different: self,other="
          << quoted_(name) << "," << quoted_(otherName);
    }
  } else {
    result = _what == other._what;
    if (!result.first) {
      oss << "\n  Event data are different: self,other="
          << what().to_string() << "," << other.what().to_string();
    }
  }

  if (oss.tellp() == noerror_pos) {
    return Success();
  }

  return Failure(oss);
}
```

**Brief:** Compares all update components with detailed error messages, respecting the timestamp mask.

**Called by:** Parent overlay comparison operators

**Purpose:** Provides comprehensive comparison with clear error messages for debugging.

## Performance Considerations

- **Minimal overhead:** Sub-overlays are stored by value, not pointer
- **Validation cost:** Field presence validated during construction
- **No dynamic allocation:** Uses references to existing JSON storage

## Error Handling Strategy

- Uses `confirm()` macro for precondition validation (type and field presence)
- Returns `result_t` pairs with success/failure status and detailed error messages
- Aggregates multiple errors into single failure message

## Testing Notes

- **Unit tests:** Located in `test/Overlay/` directory
- **Key test cases:**
  - Update construction from JSON
  - Event name access and modification
  - Timestamp access and modification
  - Comparison with and without masking
  - Missing field handling (should fail on construction)

## Maintenance Notes

### Serialization Helper

The `debrace()` function removes the outer `{}` from serialized sub-components:
- `_timestamp.to_string()` returns `{"timestamp": "2025-01-15T10:30:00Z"}`
- `debrace()` produces `"timestamp": "2025-01-15T10:30:00Z"`

This allows proper nesting in the output JSON without double braces.

### Comparison Mask Behavior

The comparison operator has two modes:
1. **Normal mode:** Compares timestamp, event name, and full value
2. **Masked mode (`DOCUMENT_COMPARE_MUTE_TIMESTAMPS`):** Skips timestamp comparison and only compares names within values

This is important for testing where:
- Timestamps will naturally differ between test runs
- The semantic content (what was changed) should match

### Event Field Access

The `name()` method accesses the "event" field directly via `value_as<std::string>(jsonliteral::event)` rather than through a sub-overlay. This is because:
- The event name is a simple string, not a complex structure
- No special validation or transformation is needed
- Direct access is more efficient

### Helper Functions Used

- `quoted_()` - Wraps a string in double quotes for JSON output
- `debrace()` - Removes outer braces from a JSON string for proper nesting
- `value_as<>()` - Base class template method for typed field access

## See Also

- [ovlUpdate.h](./ovlUpdate.h.md) - Class declaration and type aliases
- [ovlKeyValue.h](./ovlKeyValue.h.md) - Base class providing `value_as<>()`
- [ovlTimeStamp.cpp](./ovlTimeStamp.cpp.md) - Timestamp sub-overlay implementation
- [ovlKeyValueTimeStamp.h](./ovlKeyValueTimeStamp.h.md) - Update entry template
- [ovlBookkeeping.cpp](./ovlBookkeeping.cpp.md) - Uses update overlays in lists
