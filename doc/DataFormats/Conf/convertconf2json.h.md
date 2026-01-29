# convertconf2json.h

**Path:** `artdaq-database/DataFormats/Conf/convertconf2json.h`

**Purpose:** Defines converter structures for bidirectional CONF to JSON AST conversion. This header provides the low-level conversion infrastructure used internally by the CONF module.

## Key Concepts

### Converter Pattern
The DataFormats module uses converter structs for format transformations:
- **conf2json**: Converts CONF AST elements to JSON AST
- **json2conf**: Converts JSON AST elements to CONF AST

These converters operate on AST nodes rather than complete documents, allowing fine-grained control over the conversion process.

### Current Status
The converter structures are defined as empty stubs. The actual conversion logic is intended to be implemented in these structures, similar to the FHiCL module's `fcl2jsondb` and `json2fcldb` converters.

## Thread Safety

- **Thread-safe:** Yes (empty structs, no state)
- **Concurrent access:** Safe
- **Locking:** None

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/common.h` | Common includes, boost::variant configuration |
| `artdaq-database/DataFormats/Conf/conf_types.h` | CONF type definitions |
| `artdaq-database/DataFormats/Json/json_types.h` | JSON AST types |

## Classes/Structures

### `conf2json`

**Brief:** Placeholder structure for CONF to JSON AST conversion logic.

```cpp
struct conf2json final {};
```

**Thread Safety:** Thread-safe (no state)

**Future Implementation:** Should provide conversion operators and methods to transform CONF format elements into JSON AST nodes, similar to `fcl2jsondb` in the FHiCL module.

### `json2conf`

**Brief:** Placeholder structure for JSON AST to CONF conversion logic.

```cpp
struct json2conf final {};
```

**Thread Safety:** Thread-safe (no state)

**Future Implementation:** Should provide conversion operators and methods to transform JSON AST nodes into CONF format elements, similar to `json2fcldb` in the FHiCL module.

## Functions

### `debug::Conf2Json() -> void`

**Brief:** Enables TRACE logging for conversion operations.

**Preconditions:** None

**Returns:** void

**Side Effects:**
- Configures TRACE with name "convertconf2json.cpp"
- Enables all trace levels

## Usage Example (Future Implementation)

```cpp
#include "artdaq-database/DataFormats/Conf/convertconf2json.h"

using namespace artdaq::database;

// Future usage pattern (when implemented):
void convertConfElement() {
  // conf2json converter;
  // json::data_t result = converter(conf_element);
}

void convertJsonElement() {
  // json2conf converter;
  // conf_element_t result = converter(json_data);
}
```

## Relationship to Other Components

```
convertconf2json.h (low-level converters)
    ^
    |
confjsondb.cpp (orchestrates conversion)
    |
    v
conf_reader.cpp / conf_writer.cpp (read/write operations)
```

### Comparison with FHiCL Module

| FHiCL Module | CONF Module (Stub) |
|--------------|-------------------|
| `fcl2jsondb` | `conf2json` |
| `json2fcldb` | `json2conf` |
| Full implementation | Empty stubs |

## Notes for Developers

- **Implementation Status:** Structures are empty stubs marked with TODO
- **Design Pattern:** Follow the FHiCL module's converter pattern when implementing
- **Namespace:** Converters are in `artdaq::database::confjson` namespace
- **Debug Function:** `debug::Conf2Json()` enables trace logging

### Common Pitfalls

- **Empty stubs:** These converters do not yet provide any functionality
- **Reference implementation:** Use `convertfhicl2jsondb.h` as a guide for implementation

## See Also

- [convertconf2json.cpp](./convertconf2json.cpp.md) - Implementation (stub)
- [conf_reader.h](./conf_reader.h.md) - Uses these converters (future)
- [conf_writer.h](./conf_writer.h.md) - Uses these converters (future)
- [../Fhicl/convertfhicl2jsondb.h](../Fhicl/convertfhicl2jsondb.h.md) - Reference implementation pattern
