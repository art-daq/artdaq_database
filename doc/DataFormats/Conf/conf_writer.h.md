# conf_writer.h

**Path:** `artdaq-database/DataFormats/Conf/conf_writer.h`

**Purpose:** Defines the ConfWriter class for generating CONF (configuration) format text from a JSON Abstract Syntax Tree (AST) representation. This enables database-stored configurations to be exported back to CONF format.

## Key Concepts

### Writer Pattern
The DataFormats module uses a consistent Reader/Writer pattern across all formats:
- **Reader**: Parses text format into internal AST
- **Writer**: Generates text format from AST (`write()` method)

### JSON AST to CONF
ConfWriter converts JSON AST:
```cpp
json::object_t {
    "data": json::object_t {
        "host": "localhost",
        "port": 8080
    }
}
```

Into CONF format:
```
host: localhost
port: 8080
```

## Thread Safety

- **Thread-safe:** Yes
- **Concurrent access:** ConfWriter is stateless; multiple instances can generate concurrently
- **Locking:** None required

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/common.h` | Common includes, boost::variant configuration |
| `artdaq-database/DataFormats/Conf/conf_types.h` | CONF type definitions (placeholder) |
| `artdaq-database/DataFormats/Json/json_types.h` | JSON AST types used as input |

## Classes/Structures

### `ConfWriter`

Generator class for converting JSON AST to CONF format text.

**Thread Safety:** Thread-safe (stateless)

#### Methods

##### `write(jsn::object_t const&, std::string&) -> bool`

**Brief:** Generates CONF format text from a JSON object AST, extracting content from the "data" key.

**Parameters:**
- `ast` - Input JSON object AST (must not be empty, must contain "data" key)
- `conf_output` - Output CONF format text (must be empty on entry)

**Preconditions:**
- `ast` must not be empty (enforced by `confirm()`)
- `conf_output` must be empty (enforced by `confirm()`)
- `ast` should contain a "data" key with object value

**Returns:** `true` on successful generation, `false` on failure.

**Postconditions:**
- On success, `conf_output` contains generated CONF text
- On failure, `conf_output` is unchanged

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::out_of_range` | If "data" key is missing from AST |
| `boost::bad_get` | If "data" value is not an object |

**Thread Safety:** Safe (stateless)

**Example:**
```cpp
#include "artdaq-database/DataFormats/Conf/conf_writer.h"

using namespace artdaq::database;

void generateConfFile() {
  json::object_t ast;
  ast["data"] = json::object_t{};
  auto& data = boost::get<json::object_t>(ast["data"]);
  data["host"] = std::string("localhost");
  data["port"] = static_cast<integer>(8080);

  std::string conf_output;

  try {
    conf::ConfWriter writer;
    if (writer.write(ast, conf_output)) {
      std::cout << "Generated CONF:\n" << conf_output;
    } else {
      std::cerr << "Failed to generate CONF\n";
    }
  } catch (std::exception const& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
}
```

## Functions

### `debug::ConfWriter() -> void`

**Brief:** Enables TRACE logging for ConfWriter operations at all levels.

**Preconditions:** None

**Returns:** void

**Side Effects:**
- Configures TRACE logging with name "conf_writer.cpp"
- Enables all trace levels

**Example:**
```cpp
// Enable debugging before generation
conf::debug::ConfWriter();

// Now generation will produce TRACE output
conf::ConfWriter writer;
writer.write(ast, output);
```

## Relationship to Other Components

```
json::object_t (AST)
    |
    v
ConfWriter::write()
    |
    v
CONF Text

Conversion Chain:
FHiCL --> FhiclReader --> json::object_t --> ConfWriter --> CONF
JSON --> JsonReader --> json::object_t --> ConfWriter --> CONF
XML --> XmlReader --> json::object_t --> ConfWriter --> CONF
```

## Notes for Developers

- **Current Status**: The implementation contains TODO markers; generation logic is minimal (stub)
- **Input Structure**: Expects `{"data": {...}}` structure with configuration inside "data" node
- **TRACE Integration**: Uses TRACE_NAME "conf_writer.cpp" for logging

### Common Pitfalls

- **Missing "data" key:** Will throw if AST does not contain "data" key
- **Wrong type for "data":** Will throw if "data" is not a json::object_t
- **Non-empty output:** Will fail `confirm()` if output string is not empty
- **Stub implementation:** Current implementation always returns false

## See Also

- [conf_writer.cpp](./conf_writer.cpp.md) - Implementation details
- [conf_reader.h](./conf_reader.h.md) - Complementary reader class
- [conf_types.h](./conf_types.h.md) - Type definitions
- [../Json/json_writer.h](../Json/json_writer.h.md) - Similar pattern for JSON format
