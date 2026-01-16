# json_common.h

**Path:** `artdaq-database/DataFormats/Json/json_common.h`

**Purpose:** Provides a single convenience include point that aggregates all common JSON DataFormat functionality. This header bundles the JSON reader (parser), JSON writer (generator), and core DataFormats infrastructure, simplifying includes for code that needs bidirectional JSON conversion capabilities.


## Key Concepts

### Aggregator Header Pattern

This header follows the "aggregator" or "umbrella" header pattern, which is common in C++ libraries. Instead of requiring users to include multiple related headers individually, an aggregator header provides a single include that brings in all commonly-needed functionality for a subsystem.

### JSON Processing Pipeline

The JSON DataFormats module provides a complete pipeline for JSON handling:
1. **Parsing (Reading):** Convert JSON text to internal AST representation
2. **Manipulation:** Work with JSON data using type-safe structures
3. **Generation (Writing):** Convert AST back to JSON text

This header provides access to both ends of the pipeline.

## Thread Safety

- **Thread-safe:** Not applicable (header-only aggregator)
- **Concurrent access:** Thread safety depends on the included components
- **Locking:** See individual component documentation

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/Json/json_reader.h` | JSON parser functionality using Boost.Spirit Qi |
| `artdaq-database/DataFormats/Json/json_writer.h` | JSON generator functionality using Boost.Spirit Karma |
| `artdaq-database/DataFormats/common.h` | Common DataFormats infrastructure (boost::variant configuration, standard libraries) |

## Included Functionality

By including `json_common.h`, you gain access to:

### From json_reader.h

- `JsonReader` class for parsing JSON text to AST
- Grammar definitions for JSON syntax

### From json_writer.h

- `JsonWriter` class for generating JSON text from AST
- Formatting options for output

### From common.h

- JSON type definitions (`json::object_t`, `json::array_t`, `json::value_t`)
- Boost.variant configuration for recursive types
- Standard library includes

## Usage Examples

### Basic JSON Round-Trip

```cpp
#include "artdaq-database/DataFormats/Json/json_common.h"
#include <iostream>
#include <string>

void processJsonDocument() {
  using namespace artdaq::database::json;

  // Input JSON text
  std::string json_input = R"({
    "name": "detector_config",
    "version": 1,
    "parameters": {
      "threshold": 100,
      "enabled": true
    }
  })";

  // Parse JSON to AST
  object_t ast;
  JsonReader reader;

  try {
    if (!reader.read(json_input, ast)) {
      std::cerr << "Failed to parse JSON\n";
      return;
    }

    // Manipulate AST (example: access a value)
    // ... manipulation code ...

    // Generate JSON from AST
    std::string json_output;
    JsonWriter writer;
    if (!writer.write(ast, json_output)) {
      std::cerr << "Failed to generate JSON\n";
      return;
    }

    std::cout << "Processed JSON:\n" << json_output << "\n";

  } catch (const std::exception& e) {
    std::cerr << "JSON processing error: " << e.what() << "\n";
  }
}
```

### Configuration Processing

```cpp
#include "artdaq-database/DataFormats/Json/json_common.h"

namespace db = artdaq::database;
namespace jsn = artdaq::database::json;

bool validateAndReformat(const std::string& input, std::string& output) {
  jsn::object_t document;

  // Parse
  jsn::JsonReader reader;
  if (!reader.read(input, document)) {
    return false;
  }

  // Validate structure (example)
  if (document.count("version") == 0) {
    return false;  // Missing required field
  }

  // Reformat with consistent formatting
  jsn::JsonWriter writer;
  return writer.write(document, output);
}
```

## Relationship to Other Components

This aggregator header sits at the interface layer of the JSON DataFormats module:

```
User Code
    |
    v
json_common.h (this file)
    |
    +-- json_reader.h (parsing)
    |       |
    |       +-- json_types.h (AST types)
    |
    +-- json_writer.h (generation)
    |       |
    |       +-- json_types.h (AST types)
    |
    +-- common.h (infrastructure)
```

### Module Relationships

- **ConfigurationDB**: Uses JSON DataFormats for configuration storage and retrieval
- **JsonDocument**: Uses JSON types for document representation
- **FHiCL DataFormats**: Converts to/from JSON format using these types

## See Also

- [json_reader.h](./json_reader.h.md) - JSON parser implementation details
- [json_writer.h](./json_writer.h.md) - JSON generator implementation details
- [json_types.h](./json_types.h.md) - JSON AST type definitions
- [common.h](../common.h.md) - DataFormats common infrastructure
- [README.md](./README.md) - JSON module overview

## Notes for Developers

### When to Use This Header

**Use `json_common.h` when:**
- You need both parsing and generation capabilities
- You want a simple, single include for JSON functionality
- You're writing code that processes JSON documents

**Use individual headers when:**
- You only need parsing (`json_reader.h`)
- You only need generation (`json_writer.h`)
- You only need type definitions (`json_types.h`)
- Compilation time is critical (fewer includes = faster compilation)

### Common Pitfalls

- **Include order:** This header should be included before any code that uses JSON types
- **Namespace usage:** Remember to use the `artdaq::database::json` namespace or the `jsn` alias

### Best Practices

```cpp
// Prefer: Include at file scope
#include "artdaq-database/DataFormats/Json/json_common.h"

// Use namespace alias for cleaner code
namespace jsn = artdaq::database::json;

void example() {
  jsn::object_t obj;
  jsn::JsonReader reader;
  // ...
}
```
