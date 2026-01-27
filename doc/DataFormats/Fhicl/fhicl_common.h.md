# fhicl_common.h

**Path:** `artdaq-database/DataFormats/Fhicl/fhicl_common.h`

**Purpose:** Convenience aggregator header providing all FHiCL DataFormat functionality in a single include. This header simplifies usage by bundling the reader, writer, and common infrastructure headers together.

## Key Concepts

The FHiCL module follows the standard DataFormats pattern where each format has:
- A reader (parsing from text to AST)
- A writer (generating text from AST)
- Type definitions (FHiCL-specific with comment/annotation support)
- Common infrastructure

This aggregator header allows developers to include all FHiCL functionality with a single `#include` statement.

## Thread Safety

- **Thread-safe:** Yes (header-only aggregation)
- **Concurrent access:** Safe for concurrent includes
- **Locking:** None

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/Fhicl/fhicl_reader.h` | FHiCL text to table_t AST parser |
| `artdaq-database/DataFormats/Fhicl/fhicl_writer.h` | table_t AST to FHiCL text generator |
| `artdaq-database/DataFormats/common.h` | Common infrastructure (boost::variant configuration, exception handling) |

## Classes/Functions

This header does not define any classes or functions directly. It serves as an aggregator for:
- `fhicl::FhiclReader` - Parser class from fhicl_reader.h
- `fhicl::FhiclWriter` - Generator class from fhicl_writer.h
- `fhicl::table_t`, `fhicl::sequence_t`, etc. - Type definitions from fhicl_types.h

## Relationship to Other Components

```
fhicl_common.h
    |
    +-- fhicl_reader.h --> Parses FHiCL to fhicl::table_t
    +-- fhicl_writer.h --> Writes fhicl::table_t to FHiCL
    +-- common.h --> Boost configuration, SharedCommon utilities
```

The FHiCL module has its own type system (`fhicl::table_t`, `fhicl::sequence_t`) that preserves comments and annotations, unlike the simpler JSON AST used by other formats.

## Usage Example

```cpp
#include "artdaq-database/DataFormats/Fhicl/fhicl_common.h"

using namespace artdaq::database;

void processFhiclFile() {
  std::string fhicl_text = R"(
    # Server configuration
    timeout: 30  # seconds
    servers: [ "host1", "host2" ]
  )";
  fhicl::table_t ast;

  // Parse FHiCL to AST
  try {
    if (fhicl::FhiclReader{}.read(fhicl_text, ast)) {
      // AST now contains parsed configuration with comments

      // Write back to FHiCL format
      std::string output;
      if (fhicl::FhiclWriter{}.write(ast, output)) {
        std::cout << "Generated FHiCL:\n" << output << "\n";
      }
    }
  } catch (std::exception const& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

## Notes for Developers

- Include this header in `.cpp` files when you need full FHiCL functionality
- For header files, prefer including specific headers to minimize dependencies
- FHiCL preserves comments and annotations through the `key_t` and `value_t` types
- Unlike CONF/XML modules, FHiCL has its own AST (not JSON-based)

### Common Pitfalls

- **Wrong AST type:** FHiCL uses `fhicl::table_t`, not `json::object_t`
- **Output parameter requirements:** Both reader and writer expect empty output parameters

## See Also

- [fhicl_reader.h](./fhicl_reader.h.md) - FHiCL parsing functionality
- [fhicl_writer.h](./fhicl_writer.h.md) - FHiCL generation functionality
- [fhicl_types.h](./fhicl_types.h.md) - Type definitions with comment/annotation support
- [../common.h](../common.h.md) - DataFormats common utilities
- [../Conf/conf_common.h](../Conf/conf_common.h.md) - Similar pattern for CONF format
