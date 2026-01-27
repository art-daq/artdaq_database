# confjsondb.h

**Path:** `artdaq-database/DataFormats/Conf/confjsondb.h`

**Purpose:** Provides high-level conversion functions between CONF configuration format and database JSON format with metadata. This header defines the public API for CONF-to-JSON and JSON-to-CONF conversions that include origin information and timestamps.

## Key Concepts

### Database JSON Format
The database JSON format wraps configuration data with metadata:
```json
{
    "document": { "data": { ... } },
    "origin": {
        "format": "conf",
        "source": "conf_to_json",
        "timestamp": "2024-01-15T12:00:00Z"
    }
}
```

This format provides:
- **Traceability**: Origin information tracks source format
- **Auditing**: Timestamps record conversion time
- **Consistency**: Standard structure for all formats

### Conversion Flow
```
CONF Text --> conf_to_json() --> Database JSON (with metadata)
Database JSON --> json_to_conf() --> CONF Text
```

## Thread Safety

- **Thread-safe:** Yes
- **Concurrent access:** All functions are stateless and can be called concurrently
- **Locking:** None required

## Dependencies

| Include | Purpose |
|---------|---------|
| `<string>` | String type for input/output |

## Functions

### `confjson::conf_to_json(std::string const&, std::string&) -> bool`

**Brief:** Converts CONF configuration text to database JSON format with origin metadata and timestamp.

**Parameters:**
- `conf` - Input CONF format text (must not be empty)
- `json` - Output JSON string (must be empty on entry)

**Preconditions:**
- `conf` must not be empty
- `json` must be empty

**Returns:** `true` on success, `false` on failure.

**Postconditions:**
- On success, `json` contains database-format JSON with metadata

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::exception` | Propagated from underlying reader/writer |

**Thread Safety:** Safe (stateless)

**Side Effects:**
- Generates timestamp using `artdaq::database::timestamp()`

**Output Structure:**
```json
{
    "document": { "data": { ... } },
    "origin": {
        "format": "conf",
        "source": "conf_to_json",
        "timestamp": "<current_time>"
    }
}
```

**Example:**
```cpp
#include "artdaq-database/DataFormats/Conf/confjsondb.h"

using namespace artdaq::database;

void convertConfToJson() {
  std::string conf = "host: localhost\nport: 8080\n";
  std::string json;

  if (confjson::conf_to_json(conf, json)) {
    std::cout << "Converted JSON:\n" << json << "\n";
  } else {
    std::cerr << "Conversion failed\n";
  }
}
```

### `confjson::json_to_conf(std::string const&, std::string&) -> bool`

**Brief:** Converts database JSON format to CONF configuration text, extracting data from the document structure.

**Parameters:**
- `json` - Input JSON string (must not be empty)
- `conf` - Output CONF format text (must be empty on entry)

**Preconditions:**
- `json` must not be empty
- `conf` must be empty
- `json` must contain valid database JSON format with "document" key

**Returns:** `true` on success, `false` on failure.

**Postconditions:**
- On success, `conf` contains generated CONF text

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::exception` | Propagated from underlying reader/writer |

**Thread Safety:** Safe (stateless)

**Example:**
```cpp
#include "artdaq-database/DataFormats/Conf/confjsondb.h"

using namespace artdaq::database;

void convertJsonToConf(std::string const& json) {
  std::string conf;

  if (confjson::json_to_conf(json, conf)) {
    std::cout << "Converted CONF:\n" << conf << "\n";
  } else {
    std::cerr << "Conversion failed\n";
  }
}
```

### `confjson::debug::enableConfJson() -> void`

**Brief:** Enables TRACE logging for CONF-JSON conversion operations.

**Preconditions:** None

**Returns:** void

**Side Effects:**
- Configures TRACE logging with name "confjsondb.cpp"
- Enables all trace levels

### `conf::debug::ConfReader() -> void`

**Brief:** Enables TRACE logging for ConfReader operations.

**See Also:** [conf_reader.h](./conf_reader.h.md)

### `conf::debug::ConfWriter() -> void`

**Brief:** Enables TRACE logging for ConfWriter operations.

**See Also:** [conf_writer.h](./conf_writer.h.md)

## Relationship to Other Components

```
User CONF File
    |
    v
conf_to_json()
    |
    +-- ConfReader::read() --> json::object_t
    +-- Add origin metadata
    +-- JsonWriter::write() --> JSON string
    |
    v
Database Storage
    |
    v
json_to_conf()
    |
    +-- JsonReader::read() --> json::object_t
    +-- Extract document.data
    +-- ConfWriter::write() --> CONF string
    |
    v
User CONF File
```

## Notes for Developers

- **Metadata Preservation**: Origin information is added during conf_to_json and available in output
- **Timestamp**: Uses `artdaq::database::timestamp()` for current time
- **Error Propagation**: Returns false on any step failure
- **Namespace**: Functions are in `artdaq::database::confjson` namespace
- **Stub Status**: Conversion relies on ConfReader/ConfWriter which are stubs

### Common Pitfalls

- **Empty input:** Will return false if input is empty
- **Non-empty output:** Will fail `confirm()` if output is not empty
- **Stub implementation:** Actual conversions are not implemented

## See Also

- [confjsondb.cpp](./confjsondb.cpp.md) - Implementation details
- [conf_reader.h](./conf_reader.h.md) - CONF parsing
- [conf_writer.h](./conf_writer.h.md) - CONF generation
- [../Fhicl/fhicljsondb.h](../Fhicl/fhicljsondb.h.md) - Similar pattern for FHiCL
