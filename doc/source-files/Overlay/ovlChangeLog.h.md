# ovlChangeLog.h

## File Overview

Overlay class for human-readable changelog strings. Extends `ovlStringKeyValue` to provide specialized methods for appending changelog entries.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/ovlChangeLog.h`

## Class Definition

```cpp
class ovlChangeLog final : public ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_CHANGELOG> {
 public:
  ovlChangeLog(object_t::key_type const& key, value_t& changelog);

  // accessors
  std::string& buffer();
  std::string const& buffer() const;
  std::string& buffer(std::string const& changelog);

  // utils
  std::string& append(std::string const& changelog);

  result_t operator==(ovlChangeLog const& other) const;
};
```

## Base Class

Derives from `ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_CHANGELOG>`, inheriting:
- String value storage
- Comparison masking
- Default initialization ("not-provided" if empty)

## Implementation

### Constructor
```cpp
ovlChangeLog::ovlChangeLog(object_t::key_type const& key, value_t& changelog)
    : ovlStringKeyValue(key, changelog) {}
```

### Buffer Access
```cpp
std::string& buffer() { return string_value(); }
std::string const& buffer() const { return string_value(); }

std::string& buffer(std::string const& changelog) {
  buffer() = changelog;
  return buffer();
}
```

### Appending Entries
```cpp
std::string& append(std::string const& changelog) {
  buffer() += changelog;
  return buffer();
}
```

**Simple Concatenation**: No formatting, separators, or timestamps added automatically.

### Comparison
```cpp
result_t operator==(ovlChangeLog const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_CHANGELOG) == DOCUMENT_COMPARE_MUTE_CHANGELOG)
         ? Success()
         : self() == other.self();
}
```

**Masking**: Can ignore changelog differences with `DOCUMENT_COMPARE_MUTE_CHANGELOG`.

## Usage

```cpp
value_t recordJson;
auto changelog = overlay<ovlChangeLog, std::string>(recordJson, "changelog");

// Access
std::cout << "Changelog: " << changelog->buffer() << std::endl;

// Replace entire changelog
changelog->buffer("Initial version created.\n");

// Append entries
changelog->append("Updated trigger configuration.\n");
changelog->append("Added new board reader entity.\n");

// Result:
// "Initial version created.\n"
// "Updated trigger configuration.\n"
// "Added new board reader entity.\n"
```

## Typical Changelog Pattern

```cpp
// Adding an entry with timestamp and description
std::ostringstream entry;
entry << "[" << timestamp() << "] "
      << "User: " << username << " - "
      << "Added configuration: " << configName << "\n";
changelog->append(entry.str());
```

## JSON Structure

```json
{
  "changelog": "Initial creation by admin.\nUpdated by operator on 2025-01-15.\nConfiguration modified.\n"
}
```

## Related Files

- **ovlStringKeyValue.h** - Base template class
- **ovlKeyValue.h** - Grandparent base class
- **ovlDatabaseRecord.h** - Uses ovlChangeLog
- **common.h** - DOCUMENT_COMPARE_MUTE_CHANGELOG flag

## Best Practices

1. **Add newlines** when appending entries for readability
2. **Include timestamps** in changelog entries for temporal tracking
3. **Be concise** - summary of changes, not complete details
4. **Format consistently** - use a standard entry format
5. **Use append()** rather than replacing buffer to preserve history

## Notes

- Final class (cannot be derived from)
- Human-readable format (not structured JSON)
- No automatic formatting or timestamps
- Comparison can be masked for testing
- Inherits "not-provided" default from ovlStringKeyValue
- Used for audit trail and debugging
- Complements structured update history in bookkeeping
