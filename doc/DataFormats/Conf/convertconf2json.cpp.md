# convertconf2json.cpp

**Path:** `artdaq-database/DataFormats/Conf/convertconf2json.cpp`

**Implements:** [convertconf2json.h](./convertconf2json.h.md)

**Purpose:** Implementation file for CONF to JSON conversion utilities. Currently contains only debug setup with placeholder for future conversion logic implementation.

## Implementation Overview

This file is a minimal stub containing:
- TRACE configuration for debugging
- TODO comment for conversion code implementation

The actual CONF to JSON AST conversion logic is not yet implemented.

## Key Algorithms

None implemented yet. The file contains a TODO marker indicating where conversion logic should be added.

## Internal Functions

### `debug::Conf2Json()`

**Brief:** Configures TRACE logging for the CONF to JSON conversion module.

**Called by:** Debug/test code

**Implementation:**
```cpp
void artdaq::database::confjson::debug::Conf2Json() {
    TRACE_CNTL("name", TRACE_NAME);
    TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
    TRACE_CNTL("modeM", trace_mode::modeM);
    TRACE_CNTL("modeS", trace_mode::modeS);

    TLOG(10) << "artdaq::database::conf2json trace_enable";
}
```

**TRACE Configuration:**
| Setting | Value | Description |
|---------|-------|-------------|
| name | "convertconf2json.cpp" | Module name for filtering |
| lvlset | 0xFFFFFFFFFFFFFFFFLL | All levels enabled |
| modeM | trace_mode::modeM | Memory mode |
| modeS | trace_mode::modeS | Slow mode |

## Performance Considerations

- **Current implementation:** Minimal overhead (only debug function)
- **Future considerations:** Conversion logic will need efficient type checking and value transformation

## Error Handling Strategy

Not yet implemented. Future implementation should follow the pattern in `convertfhicl2jsondb.cpp`.

## Testing Notes

- **Unit tests:** Would be in `test/DataFormats/Conf/`
- **Key test cases:**
  - CONF element to JSON conversion
  - JSON element to CONF conversion
  - Type preservation

## Maintenance Notes

### TRACE Configuration Pattern

```cpp
#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "convertconf2json.cpp"
```

This pattern ensures consistent TRACE naming even when headers define their own TRACE_NAME.

### Future Implementation Guide

When implementing the CONF to JSON conversion, follow the FHiCL module pattern:

```cpp
// Example implementation pattern (from FHiCL module):

conf2json::operator datapair_t() {
    // 1. Extract key and value from CONF element
    // 2. Create JSON data_t for data
    // 3. Create JSON data_t for metadata
    // 4. Return pair of (data, metadata)
}

json2conf::operator conf_element_t() {
    // 1. Extract key and value from JSON element
    // 2. Check type metadata
    // 3. Create appropriate CONF element
    // 4. Return CONF element
}
```

### TODO Marker

The file contains: `// TODO conf to json conversion code`

This indicates the location where conversion logic should be implemented.

## See Also

- [convertconf2json.h](./convertconf2json.h.md) - Converter declarations
- [../Fhicl/convertfhicl2jsondb.cpp](../Fhicl/convertfhicl2jsondb.cpp.md) - Reference implementation
- [conf_reader.cpp](./conf_reader.cpp.md) - Future user of conf2json
- [conf_writer.cpp](./conf_writer.cpp.md) - Future user of json2conf
