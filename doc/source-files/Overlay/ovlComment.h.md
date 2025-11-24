# ovlComment.h

## File Overview

Overlay class for configuration comments with line number tracking. Enables associating human-readable comments with specific lines in configuration files.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/ovlComment.h`

## Class Definition

```cpp
class ovlComment final : public ovlKeyValue {
 public:
  ovlComment(object_t::key_type const& key, value_t& comment);

  // accessors
  integer& linenum();
  std::string& text();
  integer const& linenum() const;
  std::string const& text() const;

  // overrides
  std::string to_string() const override;

  // ops
  result_t operator==(ovlComment const&) const;
};
```

## Implementation

### Constructor
```cpp
ovlComment::ovlComment(object_t::key_type const& key, value_t& comment)
    : ovlKeyValue(key, comment) {}
```

Simple pass-through constructor.

### Accessor Methods
```cpp
integer& linenum() { return value_as<integer>(jsonliteral::linenum); }
std::string& text() { return value_as<std::string>(jsonliteral::value); }
integer const& linenum() const { return value_as<integer>(jsonliteral::linenum); }
std::string const& text() const { return value_as<std::string>(jsonliteral::value); }
```

### Serialization
```cpp
std::string to_string() const {
  std::ostringstream oss;
  oss << "{" << quoted_(jsonliteral::linenum) << ":" << linenum() << ",";
  oss << quoted_(jsonliteral::value) << ":" << quoted_(text()) << "}";
  return oss.str();
}
```

**Example Output**: `{"linenum": 42, "value": "Configuration for production run"}`

### Comparison
```cpp
result_t operator==(ovlComment const& other) const {
  return self() == other.self();  // No masking, direct comparison
}
```

**Note**: Comments comparison doesn't check the global mask (unlike other overlays).

## Usage

```cpp
value_t commentJson;
commentJson["linenum"] = 10;
commentJson["value"] = "DAQ configuration comment";

ovlComment comment("comment", commentJson);

std::cout << "Line " << comment.linenum() << ": " << comment.text() << std::endl;

// Modify
comment.text() = "Updated comment";
comment.linenum() = 15;
```

## JSON Structure

```json
{
  "comments": [
    {"linenum": 10, "value": "Start of DAQ configuration"},
    {"linenum": 25, "value": "Trigger settings"},
    {"linenum": 50, "value": "End of configuration"}
  ]
}
```

## Related Files

- **ovlKeyValue.h** - Base class
- **ovlFixedList.h** - Comments stored in ovlFixedList (ovlComments)
- **ovlDatabaseRecord.h** - Uses ovlComments list

## Notes

- Final class
- Used within ovlFixedList<ovlComment> (ovlComments type)
- No automatic initialization of fields
- Line numbers are integers, not strings
- Text stored in "value" field (not "text")
