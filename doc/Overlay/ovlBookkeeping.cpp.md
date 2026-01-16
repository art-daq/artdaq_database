# ovlBookkeeping.cpp

**Path:** `artdaq-database/Overlay/ovlBookkeeping.cpp`

**Implements:** [ovlBookkeeping.h](./ovlBookkeeping.h.md)

**Purpose:** Implementation file for the ovlBookkeeping class, providing the constructor, state accessor implementations, serialization, initialization logic, and comparison operator that manages database document record bookkeeping metadata.

## Implementation Overview

This file implements the bookkeeping overlay functionality. Key implementation aspects include:

1. **Lazy Initialization** - Missing JSON fields are created with default values during construction
2. **Update Tracking** - The updates array is wrapped in overlay objects for type-safe access
3. **Mask-Aware Comparison** - The equality operator respects global comparison flags
4. **Formatted Serialization** - The `to_string()` method produces human-readable JSON output

## Key Algorithms

### Initialization Chain

The constructor uses member initializer list ordering to ensure proper initialization:

```cpp
ovlBookkeeping::ovlBookkeeping(object_t::key_type const& key, value_t& bookkeeping)
    : ovlKeyValue(key, bookkeeping),
      _initOK(init(bookkeeping)),      // First: ensure fields exist
      _updates(make_updates(bookkeeping)),  // Then: wrap updates array
      _created(map_created(bookkeeping)) {} // Finally: wrap timestamp
```

**Brief:** Constructs a bookkeeping overlay with lazy field initialization.

**Steps:**
1. Call base class constructor with key and value
2. Run `init()` to create missing fields with defaults
3. Create the updates overlay list via `make_updates()`
4. Create the timestamp overlay via `map_created()`

**Why this approach:** The initialization order ensures that fields exist before attempting to wrap them in overlays. The `_initOK` member captures the init result to enable potential error detection.

### Lazy Field Creation

The `init()` method creates missing fields with sensible defaults:

**Brief:** Initializes missing bookkeeping fields with default values.

**Steps:**
1. Verify the value is a JSON object
2. Check for `isdeleted` field; create with `false` if missing
3. Check for `isreadonly` field; create with `false` if missing
4. Return `true` on success

This ensures that all bookkeeping documents have a consistent structure regardless of their source.

**Implementation:**
```cpp
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

### Update Array Wrapping

The `make_updates()` method wraps each update entry in an overlay:

**Brief:** Creates or retrieves the updates array and wraps each entry in an ovlUpdate overlay.

**Steps:**
1. Verify the value is a JSON object
2. Check for `updates` field; create empty array if missing
3. Iterate through existing update entries
4. Create an `ovlUpdate` overlay for each entry
5. Return the container of overlays

**Implementation:**
```cpp
ovlBookkeeping::updates_t ovlBookkeeping::make_updates(value_t& value) {
  confirm(type(value) == type_t::OBJECT);
  auto& obj = object_value();

  if (obj.count(jsonliteral::updates) == 0) {
    obj[jsonliteral::updates] = array_t{};
  }

  confirm(obj.count(jsonliteral::updates) == 1);

  auto& updates = unwrap(obj).value_as<array_t>(jsonliteral::updates);

  auto returnValue = ovlBookkeeping::updates_t{};

  for (auto& update : updates) {
    returnValue.emplace_back(jsonliteral::update, update);
  }

  return returnValue;
}
```

### Timestamp Initialization

The `map_created()` method creates or retrieves the creation timestamp:

**Brief:** Creates or retrieves the creation timestamp and wraps it in an ovlTimeStamp overlay.

**Steps:**
1. Verify the value is a JSON object
2. Check for `created` field; create with current timestamp if missing
3. Return `ovlTimeStamp` overlay wrapping the field

**Implementation:**
```cpp
ovlTimeStamp ovlBookkeeping::map_created(value_t& value) {
  confirm(type(value) == type_t::OBJECT);
  auto& obj = object_value();

  if (obj.count(jsonliteral::created) == 0) {
    obj[jsonliteral::created] = timestamp();
  }

  confirm(obj.count(jsonliteral::created) == 1);

  return ovlTimeStamp(jsonliteral::created, obj.at(jsonliteral::created));
}
```

### Mask-Aware Comparison

The comparison operator implements multi-level masking:

**Brief:** Compares two bookkeeping overlays with mask support and detailed error reporting.

**Steps:**
1. Check `DOCUMENT_COMPARE_MUTE_BOOKKEEPING` - if set, return success immediately
2. Compare `isdeleted` and `isreadonly` flags
3. Compare creation timestamps
4. Check `DOCUMENT_COMPARE_MUTE_UPDATES` - if set, skip update history
5. Compare update array sizes
6. Compare update entries element-by-element
7. Generate detailed error message on mismatch

**Implementation:**
```cpp
result_t ovlBookkeeping::operator==(ovlBookkeeping const& other) const {
  if ((useCompareMask() & DOCUMENT_COMPARE_MUTE_BOOKKEEPING) == DOCUMENT_COMPARE_MUTE_BOOKKEEPING) {
    return Success();
  }

  std::ostringstream oss;
  oss << "\nBookkeeping nodes disagree.";
  auto noerror_pos = oss.tellp();

  if (isDeleted() != other.isDeleted()) {
    oss << "\n  isdeleted flags are different: self,other=" << bool_(isDeleted()) << "," << bool_(other.isDeleted());
  }

  if (isReadonly() != other.isReadonly()) {
    oss << "\n  isreadonly flags are different: self,other=" << bool_(isReadonly()) << "," << bool_(other.isReadonly());
  }

  auto result = _created == other._created;

  if (!result.first) {
    oss << "\n  Timestamps are different: self,other=" << quoted_(_created.timestamp()) << "," << quoted_(other._created.timestamp());
  }

  if (oss.tellp() == noerror_pos && (useCompareMask() & DOCUMENT_COMPARE_MUTE_UPDATES) == DOCUMENT_COMPARE_MUTE_UPDATES) {
    return Success();
  }

  if (_updates.size() != other._updates.size()) {
    oss << "\n  Record update histories have different size: self,other=" << _updates.size() << "," << other._updates.size();
  }

  if (oss.tellp() == noerror_pos &&
      std::equal(_updates.cbegin(), _updates.end(), other._updates.cbegin(), [&oss](auto const& first, auto const& second) -> bool {
        auto result = first == second;
        if (result.first) {
          return true;
        }
        oss << "\n  Record update histories are different: self,other=" << first.to_string() << "," << second.to_string();
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

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/Overlay/ovlBookkeeping.h` | Class declaration and template method |
| `artdaq-database/Overlay/ovlKeyValue.h` | Base class utilities and unwrap function |

## Internal Functions

### Constructor

```cpp
ovlBookkeeping::ovlBookkeeping(object_t::key_type const& key, value_t& bookkeeping)
    : ovlKeyValue(key, bookkeeping),
      _initOK(init(bookkeeping)),
      _updates(make_updates(bookkeeping)),
      _created(map_created(bookkeeping)) {}
```

**Brief:** Constructs bookkeeping overlay with lazy initialization of missing fields.

**Called by:** Client code and factory functions

**Purpose:** Creates a complete bookkeeping overlay, filling in defaults for missing fields.

**Thread Safety:** Unsafe

---

### `init(value_t& parent) -> bool`

**Brief:** Initializes missing bookkeeping fields with default values.

**Called by:** Constructor (via member initializer list)

**Purpose:** Ensures consistent document structure by creating missing fields.

**Parameters:**
- `parent` - The bookkeeping JSON object to initialize

**Returns:** `true` on success

**Throws:** Rethrows any caught exceptions after triggering assertion

**Thread Safety:** Unsafe

---

### `make_updates(value_t& value) -> updates_t`

**Brief:** Creates or retrieves the updates array and wraps each entry in an ovlUpdate overlay.

**Called by:** Constructor (via member initializer list)

**Purpose:** Provides type-safe access to update history entries.

**Parameters:**
- `value` - The bookkeeping JSON object

**Returns:** Container of ovlUpdate overlays

**Thread Safety:** Unsafe

---

### `map_created(value_t& value) -> ovlTimeStamp`

**Brief:** Creates or retrieves the creation timestamp and wraps it in an ovlTimeStamp overlay.

**Called by:** Constructor (via member initializer list)

**Purpose:** Provides typed access to the document record creation time.

**Parameters:**
- `value` - The bookkeeping JSON object

**Returns:** ovlTimeStamp overlay wrapping the created field

**Thread Safety:** Unsafe

---

### `isReadonly() -> bool&`

**Brief:** Returns mutable reference to the readonly flag.

**Called by:** Client code and postUpdate method

**Purpose:** Provides direct access to modify readonly state.

**Returns:** Reference to the isreadonly boolean

**Thread Safety:** Unsafe

---

### `isReadonly() const -> bool const&`

**Brief:** Returns const reference to the readonly flag.

**Called by:** Client code for readonly checks

**Purpose:** Provides read-only access to readonly state.

**Returns:** Const reference to the isreadonly boolean

**Thread Safety:** Unsafe

---

### `isDeleted() -> bool&`

**Brief:** Returns mutable reference to the deleted flag.

**Called by:** Client code

**Purpose:** Provides direct access to modify deleted state.

**Returns:** Reference to the isdeleted boolean

**Thread Safety:** Unsafe

---

### `isDeleted() const -> bool const&`

**Brief:** Returns const reference to the deleted flag.

**Called by:** Client code for deleted checks

**Purpose:** Provides read-only access to deleted state.

**Returns:** Const reference to the isdeleted boolean

**Thread Safety:** Unsafe

---

### `markReadonly(bool const& state) -> bool&`

**Brief:** Sets readonly flag and returns new value.

**Called by:** Client code

**Purpose:** Convenience method for setting readonly state with return value.

**Parameters:**
- `state` - New readonly state

**Returns:** Reference to the updated readonly flag

**Thread Safety:** Unsafe

---

### `markDeleted(bool const& state) -> bool&`

**Brief:** Sets deleted flag and returns new value.

**Called by:** Client code

**Purpose:** Convenience method for setting deleted state with return value.

**Parameters:**
- `state` - New deleted state

**Returns:** Reference to the updated deleted flag

**Thread Safety:** Unsafe

---

### `to_string() const -> std::string`

**Brief:** Serializes bookkeeping to formatted JSON string.

**Called by:** Client code and comparison error reporting

**Purpose:** Provides human-readable representation of bookkeeping state.

**Returns:** Formatted JSON string

**Thread Safety:** Unsafe

**Implementation:**
```cpp
std::string ovlBookkeeping::to_string() const {
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

---

### `operator==(ovlBookkeeping const& other) const -> result_t`

**Brief:** Compares two bookkeeping overlays with mask support and detailed error reporting.

**Called by:** Document record comparison operations

**Purpose:** Enables bookkeeping-aware equality testing with optional masking.

**Parameters:**
- `other` - The bookkeeping overlay to compare against

**Returns:** result_t with success/failure and difference details

**Thread Safety:** Unsafe

## Performance Considerations

- **Construction:** O(n) where n = number of existing updates (must wrap each)
- **State access (isReadonly, isDeleted):** O(1) direct field lookup
- **markReadonly/markDeleted:** O(1) field assignment
- **to_string():** O(n) - iterates all updates to build output string
- **Comparison:** O(n) - must compare all updates when not masked

## Error Handling Strategy

- **init():** Uses try-catch with `confirm(false)` to propagate errors while maintaining invariants
- **State accessors:** Rely on base class value_as<> for type checking (throws `std::bad_cast`)
- **Comparison:** Builds detailed error message describing all differences found, does not throw

## Testing Notes

- **Unit tests:** `test/Overlay/ovlBookkeeping_t.cc`
- **Key test cases:**
  - Construction with empty/partial/complete JSON
  - State flag manipulation (isReadonly, isDeleted)
  - Update posting and history tracking
  - Comparison with various mask combinations
  - Serialization output verification

## Maintenance Notes

### Template Method Location

The `postUpdate<T>()` template method is defined in the header file (`ovlBookkeeping.h`) rather than here, as required by C++ template instantiation rules. The compiler needs to see the template definition at each instantiation point.

### Error Propagation in init()

The `init()` method uses a try-catch block with `confirm(false)`:
```cpp
} catch (...) {
  confirm(false);
  throw;
}
```

This pattern:
1. Catches any exception
2. Triggers an assertion failure for debugging
3. Re-throws to propagate the error

This ensures errors are both logged (via confirm/assert) and propagated.

### Overlay List Recreation

After `postUpdate()` appends to the updates array, the overlay list is recreated:
```cpp
_updates = make_updates(value());
```

This is necessary because appending to the JSON array invalidates the existing overlay references. The overlay list must be rebuilt to point to the updated JSON array elements.

### Comparison Message Building

The comparison operator uses `std::ostringstream` and tracks position to detect when errors occurred:
```cpp
auto noerror_pos = oss.tellp();
// ... comparison logic ...
if (oss.tellp() == noerror_pos) {
  return Success();  // No differences written
}
```

This allows accumulating multiple differences before returning, providing comprehensive error reporting.

### Trailing Comma Removal

The `to_string()` method removes trailing commas using stream position manipulation:
```cpp
if (!_updates.empty()) {
  oss.seekp(-1, oss.cur);  // Move back one character
}
```

This produces valid JSON by removing the comma after the last update entry.

## See Also

- [ovlBookkeeping.h](./ovlBookkeeping.h.md) - Class declaration and postUpdate template
- [ovlKeyValue.h](./ovlKeyValue.h.md) - Base class providing value access
- [ovlUpdate.h](./ovlUpdate.h.md) - Update entry overlay class
- [ovlTimeStamp.h](./ovlTimeStamp.h.md) - Timestamp overlay class
