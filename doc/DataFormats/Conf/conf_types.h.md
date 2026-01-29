# conf_types.h

**Path:** `artdaq-database/DataFormats/Conf/conf_types.h`

**Purpose:** Minimal header providing type infrastructure for CONF (configuration) data format. Unlike FHiCL, the CONF module reuses JSON AST types directly rather than defining its own type system, enabling efficient format conversion.

## Key Concepts

The CONF format is a simple key-value configuration format. Rather than defining custom types, this module leverages the existing JSON Abstract Syntax Tree (AST) types:

- **Type Reuse**: CONF parsing produces `json::object_t` AST
- **Format Conversion**: Shared AST enables easy conversion between CONF, JSON, XML, and FHiCL
- **Minimal Overhead**: No additional type definitions or memory allocations

### Design Decision

The choice to reuse JSON types reflects the simple structure of CONF format:
```
key1: value1
key2: value2
nested:
  child1: value3
```

This maps directly to JSON objects without needing specialized structures for comments, annotations, or complex metadata.

## Thread Safety

- **Thread-safe:** Yes (header-only, no state)
- **Concurrent access:** Safe for concurrent includes
- **Locking:** None

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/common.h` | Common includes and boost::variant configuration |

## Classes/Functions

This header does not define any types. All CONF operations use:

| Type | Origin | Description |
|------|--------|-------------|
| `json::object_t` | Json/json_types.h | Ordered key-value table |
| `json::array_t` | Json/json_types.h | Ordered value list |
| `json::value_t` | Json/json_types.h | Variant holding string, number, bool, object, or array |
| `json::data_t` | Json/json_types.h | Key-value pair |

## Relationship to Other Components

```
conf_types.h
    |
    +-- common.h
           |
           +-- Json/json_types.h (provides actual types)
           +-- shared_types.h (base templates)
```

### Format Comparison

| Module | Has Own Types | AST Used |
|--------|---------------|----------|
| JSON | Yes | json::object_t |
| FHiCL | Yes | fhicl::table_t (with comments/annotations) |
| XML | No | json::object_t |
| CONF | No | json::object_t |

## Usage Example

```cpp
#include "artdaq-database/DataFormats/Conf/conf_types.h"
#include "artdaq-database/DataFormats/Json/json_types.h"

using namespace artdaq::database;

void buildConfAST() {
  // CONF uses JSON types directly
  json::object_t config;
  config["host"] = std::string("localhost");
  config["port"] = static_cast<integer>(8080);
  config["enabled"] = true;

  // Can be converted to JSON, XML, or FHiCL
  // using the respective writer classes
}
```

## Notes for Developers

- No need to define separate types for CONF format
- Use `json::object_t` for all CONF AST operations
- The header exists primarily for naming consistency with other format modules
- When adding new CONF features, consider whether custom types are truly needed or if JSON types suffice
- The header guard uses "XMLTYPES" for historical reasons

### Common Pitfalls

- **Including wrong types:** Remember to include `Json/json_types.h` if you need to work with the actual AST types, as this header only provides the common infrastructure

## See Also

- [conf_reader.h](./conf_reader.h.md) - Uses these types for parsing
- [conf_writer.h](./conf_writer.h.md) - Uses these types for generation
- [../Json/json_types.h](../Json/json_types.h.md) - Actual type definitions
- [../shared_types.h](../shared_types.h.md) - Base template types
