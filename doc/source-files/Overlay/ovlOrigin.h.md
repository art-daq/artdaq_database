# ovlOrigin.h

## File Overview

Overlay class tracking document provenance - where the configuration came from, in what format, and when it was created. Maintains raw data list for preserving original source formats.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/ovlOrigin.h`

## Purpose

Tracks configuration document origins:
- **Format**: File format (json, fcl, xml, etc.)
- **Name**: Source filename or identifier
- **Source**: Origin type (file, template, database, etc.)
- **Created**: When document was first created
- **Raw Data List**: Preserves original source data in various formats

## Class Definition

```cpp
class ovlOrigin final : public ovlKeyValue {
 public:
  ovlOrigin(object_t::key_type const& key, value_t& origin);

  // accessors
  std::string& format();
  std::string const& format() const;

  std::string& name();
  std::string const& name() const;

  std::string& source();
  std::string const& source() const;

  // overrides
  std::string to_string() const override;

  // ops
  result_t operator==(ovlOrigin const&) const;

 private:
  ovlTimeStamp map_created(value_t& value);
  bool init(value_t& parent);

 private:
  bool _initOK;
  ovlTimeStamp _created;
  ovlRawDataListUPtr_t _rawdatalist;
};
```

## Type Aliases

```cpp
using ovlRawData = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_RAWDATA>;
using ovlRawDataList = ovlFixedList<ovlRawData, DOCUMENT_COMPARE_MUTE_SEARCH>;
```

## JSON Structure

```json
{
  "origin": {
    "format": "json",
    "name": "daq_config_v1.json",
    "source": "file",
    "created": "2025-01-15T10:00:00Z",
    "rawdatalist": [
      "originalFCLcontent...",
      "convertedJSONcontent..."
    ]
  }
}
```

## Implementation

### Initialization
```cpp
bool ovlOrigin::init(value_t& parent) try {
  confirm(type(parent) == type_t::OBJECT);

  auto& obj = object_value();

  if (obj.count(jsonliteral::source) == 0) {
    obj[jsonliteral::source] = "template"s;
  }

  if (obj.count(jsonliteral::name) == 0) {
    obj[jsonliteral::name] = std::string{jsonliteral::notprovided};
  }

  if (obj.count(jsonliteral::format) == 0) {
    obj[jsonliteral::format] = "json"s;
  }

  if (obj.count(jsonliteral::created) == 0) {
    obj[jsonliteral::created] = timestamp();
  }

  if (obj.count(jsonliteral::rawdatalist) == 0) {
    obj[jsonliteral::rawdatalist] = array_t{};
  }

  return true;
} catch (...) {
  confirm(false);
  throw;
}
```

**Defaults**:
- format: "json"
- name: "not-provided"
- source: "template"
- created: current timestamp
- rawdatalist: empty array

### Accessors
```cpp
std::string& format() { return value_as<std::string>(jsonliteral::format); }
std::string& name() { return value_as<std::string>(jsonliteral::name); }
std::string& source() { return value_as<std::string>(jsonliteral::source); }
```

### Comparison
```cpp
result_t operator==(ovlOrigin const& other) const {
  if ((useCompareMask() & DOCUMENT_COMPARE_MUTE_ORIGIN) == DOCUMENT_COMPARE_MUTE_ORIGIN) {
    return Success();
  }

  // Compares format, name, source, created timestamp, and raw data list
}
```

## Usage

```cpp
ovlDatabaseRecord record{json};

auto& origin = record.origin();

// Set origin information
origin.format() = "fcl";
origin.name() = "production_config.fcl";
origin.source() = "file";

std::cout << "From: " << origin.name() << std::endl;
std::cout << "Format: " << origin.format() << std::endl;
std::cout << "Source: " << origin.source() << std::endl;
std::cout << "Created: " << origin._created.timestamp() << std::endl;
```

## Common Values

### Format
- `"json"` - JSON format (default)
- `"fcl"` - FHiCL configuration language
- `"xml"` - XML format
- `"txt"` - Plain text

### Source
- `"file"` - Loaded from file
- `"template"` - Created from template (default)
- `"database"` - Retrieved from database
- `"gui"` - Created via GUI
- `"api"` - Created via API

## Design Rationale

### Why Track Provenance?

- **Reproducibility**: Know exactly where configuration came from
- **Debugging**: Trace issues back to source
- **Conversion Tracking**: Track format conversions (FCL → JSON)
- **Audit Trail**: Who/what/when created the configuration

### Raw Data List

Preserves original source data:
- Original FCL before JSON conversion
- Multiple format versions
- Enables format round-tripping
- Supports validation against originals

## Related Files

- **ovlKeyValue.h** - Base class
- **ovlTimeStamp.h** - Creation timestamp
- **ovlStringKeyValue.h** - For raw data entries
- **ovlFixedList.h** - For raw data list
- **ovlDatabaseRecord.h** - Uses origin tracking

## Notes

- Final class
- Auto-initializes with sensible defaults
- Raw data list typically empty unless format conversion occurred
- Comparison can be masked for testing
- Name and format are informational, not validated
- Source is free-form string
