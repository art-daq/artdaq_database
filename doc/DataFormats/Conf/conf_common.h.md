# conf_common.h

**Path:** `artdaq-database/DataFormats/Conf/conf_common.h`

**Purpose:** Convenience aggregator header that provides all CONF DataFormat functionality in a single include. This header simplifies usage by bundling the reader, writer, and common infrastructure headers together.

## Key Concepts

The CONF module follows the standard DataFormats pattern where each format has:
- A reader (parsing from text to AST)
- A writer (generating text from AST)
- Type definitions (reuses JSON AST)
- Common infrastructure

This aggregator header allows developers to include all CONF functionality with a single `#include` statement.

**Note:** The CONF format implementation is currently a stub with TODO placeholders for future development. The reader and writer classes exist but contain incomplete conversion logic.

## Thread Safety

- **Thread-safe:** Yes (header-only aggregation)
- **Concurrent access:** Safe for concurrent includes
- **Locking:** None

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/Conf/conf_reader.h` | CONF text to JSON AST parser |
| `artdaq-database/DataFormats/Conf/conf_writer.h` | JSON AST to CONF text generator |
| `artdaq-database/DataFormats/common.h` | Common infrastructure (boost::variant configuration, exception handling) |

## Classes/Functions

This header does not define any classes or functions directly. It serves as an aggregator for:
- `conf::ConfReader` - Parser class from conf_reader.h
- `conf::ConfWriter` - Generator class from conf_writer.h

## Relationship to Other Components

```
conf_common.h
    |
    +-- conf_reader.h --> Parses CONF to json::object_t
    +-- conf_writer.h --> Writes json::object_t to CONF
    +-- common.h --> Boost configuration, SharedCommon utilities
```

The CONF module shares the JSON AST (`json::object_t`) with other format modules, enabling seamless conversion between CONF, JSON, XML, and FHiCL formats.

## Usage Example

```cpp
#include "artdaq-database/DataFormats/Conf/conf_common.h"

using namespace artdaq::database;

void processConfFile() {
  std::string conf_text = "host: localhost\nport: 8080\n";
  json::object_t ast;

  // Parse CONF to JSON AST
  try {
    if (conf::ConfReader{}.read(conf_text, ast)) {
      // AST now contains parsed configuration

      // Write back to CONF format
      std::string output;
      if (conf::ConfWriter{}.write(ast, output)) {
        std::cout << "Generated CONF:\n" << output << "\n";
      }
    }
  } catch (std::exception const& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

## Notes for Developers

- Include this header in `.cpp` files when you need full CONF functionality
- For header files, prefer including specific headers to minimize dependencies
- The CONF module is a stub implementation with TODO markers for future development
- CONF format uses JSON AST types directly, no separate type definitions needed
- The header guard contains "XML" for historical reasons but this is the CONF module

### Common Pitfalls

- **Incomplete implementation:** The CONF reader/writer are stubs; do not rely on them for production use
- **Output parameter requirements:** Both reader and writer expect empty output parameters

## See Also

- [conf_reader.h](./conf_reader.h.md) - CONF parsing functionality
- [conf_writer.h](./conf_writer.h.md) - CONF generation functionality
- [conf_types.h](./conf_types.h.md) - Type definitions
- [../common.h](../common.h.md) - DataFormats common utilities
- [../Fhicl/fhicl_common.h](../Fhicl/fhicl_common.h.md) - Similar pattern for FHiCL format
