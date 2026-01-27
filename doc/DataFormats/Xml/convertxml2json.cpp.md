# convertxml2json.cpp

**Path:** `artdaq-database/DataFormats/Xml/convertxml2json.cpp`

**Implements:** [convertxml2json.h](./convertxml2json.h.md)

**Purpose:** Implements the debug function for the XML/JSON conversion module. This is a minimal implementation file that currently only provides TRACE debugging configuration. The actual conversion logic resides in `xmljsondb.cpp`.

## Implementation Overview

This file contains:

1. A TODO comment indicating planned XML-to-JSON conversion code
2. The `debug::Xml2Json()` function for enabling TRACE output

The tag types (`xml2json`, `json2xml`) declared in the header are empty structs with no methods to implement.

## Key Algorithms

### TRACE Debug Enabling

The `debug::Xml2Json()` function configures TRACE logging for the module:

**Steps:**
1. Set the TRACE name to `"convertxml2json.cpp"`
2. Enable all trace levels (0xFFFFFFFFFFFFFFFF for both slow and fast paths)
3. Configure trace modes for output

```cpp
void artdaq::database::xmljson::debug::Xml2Json() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::xml2json trace_enable";
}
```

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/common.h` | TRACE macros and common definitions |
| `artdaq-database/DataFormats/Xml/convertxml2json.h` | Header with declarations |
| `artdaq-database/DataFormats/Xml/xml_common.h` | XML reader/writer includes |

## TRACE Configuration

```cpp
#define TRACE_NAME "convertxml2json.cpp"
```

**Trace points:**
- Level 10: Debug enable confirmation message

## Internal Functions

### `debug::Xml2Json()`

**Brief:** Configures TRACE debugging for the XML/JSON conversion module.

**Called by:** Application code when detailed conversion debugging is needed.

**Purpose:** Enables all trace levels for this module, allowing developers to see detailed log output during conversion operations.

**TRACE settings applied:**
- Name: `"convertxml2json.cpp"`
- Level mask (slow path): All bits set (all levels enabled)
- Level mask (fast path): All bits set (all levels enabled)
- Mode M: Project-specific mode setting
- Mode S: Project-specific mode setting

## Performance Considerations

- This file adds minimal overhead; the debug function is only called when explicitly invoked
- TRACE overhead depends on enabled levels and output configuration

## Error Handling Strategy

No error handling is needed in this file - the debug function cannot fail in any meaningful way.

## Testing Notes

- **Unit tests:** Implicitly tested through conversion function tests
- **Debug function:** Can be tested by enabling trace and verifying output

## Maintenance Notes

### TODO Comment

The file contains a TODO comment:
```cpp
// TODO(mu2etrg): xml to json conversion code
```

This suggests that additional conversion functionality was planned for this file but not implemented. The actual conversion is handled by:
- `XmlReader` / `XmlWriter` for AST-level conversion
- `xmljsondb.cpp` for string-to-string conversion with metadata

### Minimal Implementation

This is intentionally a minimal implementation file. The tag types in the header (`xml2json`, `json2xml`) are empty structs that don't require method implementations. If these types are extended in the future with member functions, their implementations would be added here.

### TRACE Mode Constants

The `trace_mode::modeM` and `trace_mode::modeS` constants are defined elsewhere in the TRACE infrastructure. They control output formatting and destinations.

## Code Structure

```cpp
#include "artdaq-database/DataFormats/common.h"
#include "artdaq-database/DataFormats/Xml/convertxml2json.h"
#include "artdaq-database/DataFormats/Xml/xml_common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "convertxml2json.cpp"

// TODO(mu2etrg): xml to json conversion code

void artdaq::database::xmljson::debug::Xml2Json() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::xml2json trace_enable";
}
```

## Relationship to Other Components

This file is part of the XML conversion subsystem but plays a supporting role:

```
convertxml2json.h  -- declares tag types and debug function
        |
        v
convertxml2json.cpp -- implements debug function only (this file)
        |
        v
Used by: Debugging/development tools that need trace output
```

**For actual conversion, see:**
- `xmljsondb.cpp` - High-level string conversion
- `xml_reader.cpp` / `xml_writer.cpp` - AST-level conversion

## See Also

- [convertxml2json.h](./convertxml2json.h.md) - Header with type declarations
- [xmljsondb.cpp](./xmljsondb.cpp.md) - Actual conversion implementation
- [xml_reader.cpp](./xml_reader.cpp.md) - XML to JSON AST conversion
- [xml_writer.cpp](./xml_writer.cpp.md) - JSON AST to XML conversion
- [External: TRACE Documentation](https://cdcvs.fnal.gov/redmine/projects/trace/wiki) - TRACE logging system

## Notes for Developers

### Adding Conversion Code

If implementing the TODO for direct conversion in this file, consider:

1. Whether the functionality duplicates `xmljsondb.cpp`
2. Whether tag-type template dispatch is needed
3. Consistency with existing conversion patterns

### Debug Function Usage

```cpp
// In your test or application code:
#include "artdaq-database/DataFormats/Xml/convertxml2json.h"

int main() {
  // Enable detailed tracing for XML/JSON conversion
  artdaq::database::xmljson::debug::Xml2Json();

  // Now perform conversions - trace output will be generated
  // ...

  return 0;
}
```

### Multiple Debug Functions

The XML module has multiple debug functions for different components:
- `xml::debug::XmlReader()` - XML parsing trace
- `xml::debug::XmlWriter()` - XML generation trace
- `xmljson::debug::Xml2Json()` - Conversion trace
- `xmljson::debug::XmlJson()` - High-level conversion trace (in xmljsondb.cpp)

Enable the appropriate ones based on which component you're debugging.
