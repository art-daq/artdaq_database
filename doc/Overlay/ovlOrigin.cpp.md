# ovlOrigin.cpp

**Path:** `artdaq-database/Overlay/ovlOrigin.cpp`

**Implements:** [ovlOrigin.h](./ovlOrigin.h.md)

**Purpose:** Provides the implementation for the `ovlOrigin` overlay class, including default value initialization, creation timestamp mapping, raw data list management, JSON serialization, and comprehensive comparison for document provenance tracking. The implementation ensures complete provenance information through auto-initialization of missing fields with sensible defaults.

## Implementation Overview

The implementation focuses on ensuring complete provenance information through auto-initialization of missing fields. The constructor delegates initialization to `init()`, maps the creation timestamp via `map_created()`, and creates the raw data list overlay. The comparison operator provides detailed difference reporting when origins don't match, identifying exactly which fields differ.

## Key Algorithms

### Default Value Initialization

The `init()` method ensures all required fields exist with sensible defaults:

**Steps:**
1. Confirm parent is an OBJECT type using `confirm()` assertion
2. For each field (`source`, `name`, `format`, `created`, `rawdatalist`):
   - Check if field exists using `obj.count(fieldname) == 0`
   - If missing, create with default value
   - Confirm exactly one field exists after creation

**Default values:**

| Field | Default | Rationale |
|-------|---------|-----------|
| `source` | `"template"` | Indicates programmatic creation, not file import |
| `name` | `"not-provided"` | Explicitly indicates filename wasn't specified |
| `format` | `"json"` | Internal storage format is JSON |
| `created` | `timestamp()` | Current time via `timestamp()` function |
| `rawdatalist` | `array_t{}` | Empty array - no raw data preserved initially |

**Why this approach:** Guarantees every origin has complete provenance information without requiring explicit initialization by calling code. Defaults are sensible for programmatic document creation.

### Comprehensive Comparison

The comparison operator provides detailed difference reporting:

**Steps:**
1. Check if `DOCUMENT_COMPARE_MUTE_ORIGIN` mask is set; if so, return `Success()` immediately
2. Initialize error message stream with header "Origin nodes disagree."
3. Compare each field individually (format, name, source)
4. Compare created timestamp via `_created == other._created`
5. Compare raw data list via `*_rawdatalist == *other._rawdatalist`
6. Aggregate all differences into error message
7. If any differences found, append debug info with full `to_string()` output
8. Return `Success()` only if no differences found

**Why this approach:** Provides actionable debugging information by identifying exactly which fields differ and showing full values for comparison.

## Internal Functions

### Constructor

```cpp
ovlOrigin::ovlOrigin(object_t::key_type const& key, value_t& origin)
    : ovlKeyValue(key, origin),
      _initOK(init(origin)),
      _created(map_created(origin)),
      _rawdatalist{overlay<ovlRawDataList, array_t>(origin, jsonliteral::rawdatalist)} {}
```

**Brief:** Initializes base class, auto-populates missing fields, maps timestamp, and creates raw data list overlay.

**Initialization order (important for dependencies):**
1. Base class `ovlKeyValue` - stores reference to JSON
2. `_initOK = init(origin)` - creates missing fields with defaults
3. `_created = map_created(origin)` - maps timestamp (requires `init()` to run first)
4. `_rawdatalist` - creates raw data list overlay

**Called by:** Factory functions, `ovlDatabaseRecord` constructor

### `init(value_t& parent) -> bool`

```cpp
bool ovlOrigin::init(value_t& parent) try {
  confirm(type(parent) == type_t::OBJECT);

  auto& obj = object_value();

  if (obj.count(jsonliteral::source) == 0) {
    obj[jsonliteral::source] = "template"s;
  }
  confirm(obj.count(jsonliteral::source) == 1);

  if (obj.count(jsonliteral::name) == 0) {
    obj[jsonliteral::name] = std::string{jsonliteral::notprovided};
  }
  confirm(obj.count(jsonliteral::name) == 1);

  if (obj.count(jsonliteral::format) == 0) {
    obj[jsonliteral::format] = "json"s;
  }
  confirm(obj.count(jsonliteral::format) == 1);

  if (obj.count(jsonliteral::created) == 0) {
    obj[jsonliteral::created] = timestamp();
  }
  confirm(obj.count(jsonliteral::created) == 1);

  if (obj.count(jsonliteral::rawdatalist) == 0) {
    obj[jsonliteral::rawdatalist] = array_t{};
  }
  confirm(obj.count(jsonliteral::rawdatalist) == 1);

  return true;
} catch (...) {
  confirm(false);
  throw;
}
```

**Brief:** Creates default values for all missing origin fields. This is the core initialization logic that ensures complete provenance.

**Called by:** Constructor (via member initializer list)

**Purpose:** Guarantees every origin has complete fields for provenance tracking.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| Assertion failure | If parent is not OBJECT type |
| Assertion failure | If any field count is not exactly 1 after initialization |
| Any exception | Re-thrown after logging assertion failure |

### `map_created(value_t& parent) -> ovlTimeStamp`

```cpp
ovlTimeStamp ovlOrigin::map_created(value_t& parent) {
  confirm(type(parent) == type_t::OBJECT);
  auto& obj = object_value();
  confirm(obj.count(jsonliteral::created) == 1);

  return ovlTimeStamp(jsonliteral::created, obj.at(jsonliteral::created));
}
```

**Brief:** Creates an `ovlTimeStamp` overlay for the creation timestamp field. Must be called after `init()` has ensured the field exists.

**Called by:** Constructor (after `init()`)

**Preconditions:** `init()` must have been called first to ensure `created` field exists.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| Assertion failure | If parent is not OBJECT type |
| Assertion failure | If `created` field doesn't exist |

### Accessor Methods

```cpp
std::string& ovlOrigin::format() {
  return value_as<std::string>(jsonliteral::format);
}

std::string& ovlOrigin::name() {
  return value_as<std::string>(jsonliteral::name);
}

std::string& ovlOrigin::source() {
  return value_as<std::string>(jsonliteral::source);
}

std::string const& ovlOrigin::format() const {
  return value_as<std::string>(jsonliteral::format);
}

std::string const& ovlOrigin::name() const {
  return value_as<std::string>(jsonliteral::name);
}

std::string const& ovlOrigin::source() const {
  return value_as<std::string>(jsonliteral::source);
}
```

**Brief:** Provides mutable and const access to origin string fields via base class `value_as<T>()`.

**Called by:** All code accessing origin properties

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If field has wrong type (unexpected after init) |
| `std::out_of_range` | If field doesn't exist (unexpected after init) |

### Serialization

```cpp
std::string ovlOrigin::to_string() const {
  std::ostringstream oss;
  oss << "{" << quoted_(jsonliteral::origin) << ":{\n";
  oss << quoted_(jsonliteral::format) << ":" << quoted_(format()) << ",\n";
  oss << quoted_(jsonliteral::name) << ":" << quoted_(name()) << ",\n";
  oss << quoted_(jsonliteral::source) << ":" << quoted_(source()) << ",\n";
  oss << debrace(_created.to_string()) << ",\n";
  oss << debrace(_rawdatalist->to_string()) << "\n";
  oss << "}}";

  return oss.str();
}
```

**Brief:** Produces JSON representation of all origin fields including nested components (timestamp and raw data list).

**Called by:** Debugging code, comparison error messages, logging

**Uses helper functions:**
- `quoted_(str)` - Wraps string in quotes
- `debrace(str)` - Removes outer braces from nested JSON

### Comparison Operator

```cpp
result_t ovlOrigin::operator==(ovlOrigin const& other) const {
  if ((useCompareMask() & DOCUMENT_COMPARE_MUTE_ORIGIN) == DOCUMENT_COMPARE_MUTE_ORIGIN) {
    return Success();
  }

  std::ostringstream oss;
  oss << "\nOrigin nodes disagree.";
  auto noerror_pos = oss.tellp();

  if (format() != other.format()) {
    oss << "\n  Formats are different: self,other=" << quoted_(format()) << "," << quoted_(other.format());
  }

  if (name() != other.name()) {
    oss << "\n  File names are different: self,other=" << quoted_(name()) << "," << quoted_(other.name());
  }

  if (source() != other.source()) {
    oss << "\n  Sources are different: self,other=" << quoted_(source()) << "," << quoted_(other.source());
  }

  auto result = _created == other._created;
  if (!result.first) {
    oss << "\n  Timestamps are different: self,other=" << quoted_(_created.timestamp()) << "," << quoted_(other._created.timestamp());
  }

  result = *_rawdatalist == *other._rawdatalist;
  if (!result.first) {
    oss << result.second;
  }

  if (oss.tellp() == noerror_pos) {
    return Success();
  }

  oss << "\n  Debug info:";
  oss << "\n  Self  value: " << to_string();
  oss << "\n  Other value: " << other.to_string();

  return Failure(oss);
}
```

**Brief:** Compares all origin fields with detailed difference reporting. Uses `tellp()` to track whether any differences were found.

**Called by:** `ovlDatabaseRecord` comparison, test code

## Performance Considerations

- **Construction:** O(1) for field initialization plus timestamp generation if needed
- **Accessor Methods:** O(1) - direct field lookup in JSON object hash map
- **Serialization:** O(n) where n is total size of all fields and raw data list
- **Comparison:** O(n) where n is total size of fields plus raw data list comparison

## Error Handling Strategy

Uses try-catch blocks with `confirm(false)` in `init()`:

```cpp
} catch (...) {
  confirm(false);
  throw;
}
```

This pattern:
1. Catches any exception during initialization
2. Logs assertion failure via `confirm(false)` for debugging
3. Re-throws the original exception unchanged

Ensures that unexpected errors are logged while preserving original exception information.

## Testing Notes

- **Unit tests:** Part of Overlay module tests in `test/Overlay/`
- **Key test cases:**
  - Create origin with empty JSON (uses all defaults)
  - Create origin with partial fields (defaults for missing only)
  - Create origin with all fields specified (no defaults used)
  - Compare equal origins (same values)
  - Compare different origins (verify difference messages for each field)
  - Test with `DOCUMENT_COMPARE_MUTE_ORIGIN` masking enabled
  - Verify timestamp initialization uses current time

## Maintenance Notes

### Default Value Locations

Default values are defined in the `init()` method. To change defaults:
1. Locate the field initialization in `init()`
2. Modify the default value
3. Update documentation to reflect new default

**Current defaults:**
| Field | Location in `init()` | Current Default |
|-------|---------------------|-----------------|
| `source` | Line ~47 | `"template"s` |
| `name` | Line ~52 | `jsonliteral::notprovided` |
| `format` | Line ~57 | `"json"s` |
| `created` | Line ~62 | `timestamp()` |
| `rawdatalist` | Line ~67 | `array_t{}` |

### Comparison Message Format

The comparison operator produces structured error messages for debugging:

```
Origin nodes disagree.
  Formats are different: self,other="json","fcl"
  File names are different: self,other="config1.json","config2.fcl"
  Sources are different: self,other="file","template"
  Debug info:
  Self  value: {"origin":{...}}
  Other value: {"origin":{...}}
```

This format:
- Identifies which fields differ
- Shows both values side-by-side
- Includes full JSON for detailed inspection

### Raw Data List

Managed as `ovlFixedList<ovlRawData>` but stored as a private member with unique_ptr. Not directly accessible from header; access through internal methods only. The raw data list:
- Stores original source content during format conversions
- Is typically empty for simple document creation
- Uses `DOCUMENT_COMPARE_MUTE_RAWDATA` for element-level masking
- Uses `DOCUMENT_COMPARE_MUTE_SEARCH` for list-level masking

### JSON Literal Dependencies

| Literal | Expected Value | Used For |
|---------|----------------|----------|
| `jsonliteral::origin` | `"origin"` | Key in parent object |
| `jsonliteral::format` | `"format"` | Format field key |
| `jsonliteral::name` | `"name"` | Name field key |
| `jsonliteral::source` | `"source"` | Source field key |
| `jsonliteral::created` | `"created"` | Timestamp field key |
| `jsonliteral::rawdatalist` | `"rawdatalist"` | Raw data list key |
| `jsonliteral::notprovided` | `"not-provided"` | Default name value |

## See Also

- [ovlOrigin.h](./ovlOrigin.h.md) - Class declaration and type aliases
- [ovlKeyValue.h](./ovlKeyValue.h.md) - Base class providing `value_as<T>()`
- [ovlTimeStamp.h](./ovlTimeStamp.h.md) - Timestamp overlay used for `_created`
- [ovlFixedList.h](./ovlFixedList.h.md) - List container for raw data
- [common.h](./common.h.md) - JSON literal constants and comparison masks
- [ovlDatabaseRecord.h](./ovlDatabaseRecord.h.md) - Uses `ovlOrigin` as component
