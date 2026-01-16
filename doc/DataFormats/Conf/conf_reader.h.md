# conf_reader.h

**Path:** `artdaq-database/DataFormats/Conf/conf_reader.h`

**Purpose:** Defines the ConfReader class for parsing CONF (configuration) format text into a JSON Abstract Syntax Tree (AST) representation. This enables CONF files to be processed, converted, and stored using the unified DataFormats infrastructure.

## Key Concepts

### Reader Pattern
The DataFormats module uses a consistent Reader/Writer pattern across all formats:
- **Reader**: Parses text format into internal AST (`read()` method)
- **Writer**: Generates text format from AST (`write()` method)

### CONF to JSON AST
ConfReader converts CONF format:
```
host: localhost
port: 8080
```

Into JSON AST:
```cpp
json::object_t {
    "data": json::object_t {
        "host": "localhost",
        "port": "8080"
    }
}
```

## Thread Safety

- **Thread-safe:** Yes
- **Concurrent access:** ConfReader is stateless; multiple instances can parse concurrently
- **Locking:** None required

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/common.h` | Common includes, boost::variant configuration |
| `artdaq-database/DataFormats/Conf/conf_types.h` | CONF type definitions (placeholder) |
| `artdaq-database/DataFormats/Json/json_types.h` | JSON AST types used as output |

## Classes/Structures

### `ConfReader`

Parser class for converting CONF format text to JSON AST.

**Thread Safety:** Thread-safe (stateless)

#### Methods

##### `read(std::string const&, jsn::object_t&) -> bool`

**Brief:** Parses CONF format text into a JSON object AST representation, wrapping the parsed data under a "data" key.

**Parameters:**
- `conf_string` - Input CONF format text (must not be empty)
- `ast` - Output JSON object AST (must be empty on entry)

**Preconditions:**
- `conf_string` must not be empty (enforced by `confirm()`)
- `ast` must be empty (enforced by `confirm()`)

**Returns:** `true` on successful parse, `false` on failure.

**Postconditions:**
- On success, `ast` contains `{"data": {...}}` structure with parsed content
- On failure, `ast` is unchanged

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::exception` | Re-thrown after logging if parsing fails internally |

**Thread Safety:** Safe (stateless)

**Example:**
```cpp
#include "artdaq-database/DataFormats/Conf/conf_reader.h"

using namespace artdaq::database;

void parseConfFile(std::string const& conf_text) {
  json::object_t ast;

  try {
    conf::ConfReader reader;
    if (reader.read(conf_text, ast)) {
      // Access parsed data
      auto& data = boost::get<json::object_t>(ast.at("data"));
      std::cout << "Parse successful\n";
    } else {
      std::cerr << "Failed to parse CONF\n";
    }
  } catch (std::exception const& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
}
```

## Functions

### `debug::ConfReader() -> void`

**Brief:** Enables TRACE logging for ConfReader operations at all levels.

**Preconditions:** None

**Returns:** void

**Side Effects:**
- Configures TRACE logging with name "conf_reader.cpp"
- Enables all trace levels

**Example:**
```cpp
// Enable debugging before parsing
conf::debug::ConfReader();

// Now parsing will produce TRACE output
conf::ConfReader reader;
reader.read(text, ast);
```

## Relationship to Other Components

```
CONF Text
    |
    v
ConfReader::read()
    |
    v
json::object_t (AST)
    |
    +---> JsonWriter --> JSON Text
    +---> XmlWriter --> XML Text
    +---> FhiclWriter --> FHiCL Text
    +---> ConfWriter --> CONF Text (round-trip)
```

## Notes for Developers

- **Current Status**: The implementation contains TODO markers; parsing logic is minimal (stub)
- **Error Handling**: Exceptions are caught, logged via TLOG, printed to stderr, and re-thrown
- **Output Structure**: Creates `{"data": {...}}` wrapper around parsed content
- **TRACE Integration**: Uses TRACE_NAME "conf_reader.cpp" for logging

### Common Pitfalls

- **Empty input:** Will fail `confirm()` if input string is empty
- **Non-empty output:** Will fail `confirm()` if output AST is not empty
- **Stub implementation:** Current implementation does not actually parse CONF syntax

## See Also

- [conf_reader.cpp](./conf_reader.cpp.md) - Implementation details
- [conf_writer.h](./conf_writer.h.md) - Complementary writer class
- [conf_types.h](./conf_types.h.md) - Type definitions
- [../Json/json_reader.h](../Json/json_reader.h.md) - Similar pattern for JSON format
