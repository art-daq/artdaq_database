# BasicTypes Module Documentation

**Last Updated:** 2026-01-14

## Overview

The **BasicTypes** module is a core component of the artdaq-database system that provides fundamental data structures for representing and converting between different configuration formats. It enables seamless conversion between FHICL, JSON, and XML formats, with JSON serving as the pivot format for database storage.

**Location**: `artdaq-database/BasicTypes/`

## Module Purpose

The BasicTypes module serves three primary purposes:

1. **Format Abstraction**: Provides unified interfaces for working with different configuration formats (FHICL, JSON, XML)
2. **Format Conversion**: Enables bidirectional conversion between formats through a common JSON intermediary
3. **Database Integration**: Facilitates storage of configuration data in database systems using JSON encoding with Base64

## Architecture

### Data Flow

```
┌─────────────┐         ┌──────────────┐         ┌─────────────┐
│  FHICL      │◄───────►│    JSON      │◄───────►│    XML      │
│  Config     │         │  (Pivot)     │         │   Config    │
└─────────────┘         └──────────────┘         └─────────────┘
                               │
                               ▼
                        ┌──────────────┐
                        │   Database   │
                        │   (JSON +    │
                        │   Base64)    │
                        └──────────────┘
```

### Key Design Principles

1. **JSON as Pivot**: All conversions use JSON as an intermediary format
2. **Base64 Encoding**: Original formats are Base64-encoded when embedded in JSON for database storage
3. **Value Types**: All data types are simple value types (copyable, movable)
4. **Boost.Fusion Integration**: All types are adapted for generic programming
5. **Stream I/O**: Standard stream operators for easy input/output

## Module Structure

### Core Files

| File | Purpose | Documentation |
|------|---------|---------------|
| `common.h` | Common includes and utilities | [common.h.md](common.h.md) |
| `basictypes.h` | Convenience header including all types | [basictypes.h.md](basictypes.h.md) |

### Data Type Files

#### JSON Data Type
| File | Purpose | Documentation |
|------|---------|---------------|
| `data_json.h` | JsonData class declaration | [data_json.h.md](data_json.h.md) |
| `data_json.cpp` | JsonData implementation | [data_json.cpp.md](data_json.cpp.md) |
| `data_json_fusion.h` | Boost.Fusion adaptation | [data_json_fusion.h.md](data_json_fusion.h.md) |

#### FHICL Data Type
| File | Purpose | Documentation |
|------|---------|---------------|
| `data_fhicl.h` | FhiclData class declaration | [data_fhicl.h.md](data_fhicl.h.md) |
| `data_fhicl.cpp` | FhiclData implementation & conversions | [data_fhicl.cpp.md](data_fhicl.cpp.md) |
| `data_fhicl_fusion.h` | Boost.Fusion adaptation | [data_fhicl_fusion.h.md](data_fhicl_fusion.h.md) |

#### XML Data Type
| File | Purpose | Documentation |
|------|---------|---------------|
| `data_xml.h` | XmlData class declaration | [data_xml.h.md](data_xml.h.md) |
| `data_xml.cpp` | XmlData implementation & conversions | [data_xml.cpp.md](data_xml.cpp.md) |
| `data_xml_fusion.h` | Boost.Fusion adaptation | [data_xml_fusion.h.md](data_xml_fusion.h.md) |

### Utility Files

| File | Purpose | Documentation |
|------|---------|---------------|
| `base64.h` | Base64 encoding/decoding declarations | [base64.h.md](base64.h.md) |
| `base64.cpp` | Base64 encoding/decoding implementation | [base64.cpp.md](base64.cpp.md) |

## Data Types

### JsonData

**Purpose**: Wrapper for JSON-formatted data

**Members**:
- `json_buffer` - String containing JSON data

**Key Features**:
- Template methods for conversion to/from other types
- Stream I/O operators
- Empty check
- Implicit string conversion

**Primary Role**: Pivot format for all conversions and database storage

**Documentation**: [data_json.h.md](data_json.h.md)

---

### FhiclData

**Purpose**: Wrapper for FHICL (Fermilab Hierarchical Configuration Language) data

**Members**:
- `fhicl_buffer` - String containing FHICL configuration
- `fhicl_file_name` - Optional filename metadata

**Key Features**:
- Bidirectional conversion with JSON (via Base64 encoding)
- File name tracking for debugging
- Stream I/O operators
- Implicit string conversion

**Primary Use**: artdaq configuration files

**Documentation**: [data_fhicl.h.md](data_fhicl.h.md)

---

### XmlData

**Purpose**: Wrapper for XML-formatted data

**Members**:
- `xml_buffer` - String containing XML data

**Key Features**:
- Bidirectional conversion with JSON (via Base64 encoding)
- Stream I/O operators
- Implicit string conversion
- Simpler structure than FhiclData (no filename metadata)

**Primary Use**: XML-based configurations and interoperability

**Documentation**: [data_xml.h.md](data_xml.h.md)

## Conversion Mechanisms

### FHICL ↔ JSON Conversion

**Forward (FHICL → JSON)**:
```cpp
FhiclData fhicl("parameter: value");
JsonData json = fhicl;  // Implicit conversion
```

**Process**:
1. Convert FHICL to intermediate JSON structure
2. Base64-encode original FHICL buffer
3. Embed Base64 in JSON document
4. Return JsonData

**Reverse (JSON → FHICL)**:
```cpp
JsonData json(/* JSON with Base64-encoded FHICL */);
FhiclData fhicl(json);  // Constructor
```

**Process**:
1. Extract Base64 string from JSON using regex
2. Decode Base64 to get intermediate JSON
3. Convert JSON to FHICL format
4. Store in FhiclData

**Implementation**: See [data_fhicl.cpp.md](data_fhicl.cpp.md)

---

### XML ↔ JSON Conversion

**Forward (XML → JSON)**:
```cpp
XmlData xml("<config><param>value</param></config>");
JsonData json = xml;  // Implicit conversion
```

**Process**: Same as FHICL → JSON, but with XML

**Reverse (JSON → XML)**:
```cpp
JsonData json(/* JSON with Base64-encoded XML */);
XmlData xml(json);  // Constructor
```

**Process**: Same as JSON → FHICL, but producing XML

**Implementation**: See [data_xml.cpp.md](data_xml.cpp.md)

## Base64 Encoding

### Why Base64?

Base64 encoding is used for several critical reasons:

1. **Character Safety**: Preserves special characters in FHICL/XML
2. **JSON Compatibility**: Ensures valid JSON documents
3. **Binary Data**: Handles potential binary data in configurations
4. **Data Integrity**: Guarantees round-trip fidelity

### Base64 Functions

```cpp
// Encode binary data
std::string base64_encode(unsigned char const* data, unsigned int length);

// Encode string
std::string base64_encode(std::string const& str);

// Decode Base64 string
std::string base64_decode(std::string const& encoded);
```

**Documentation**: [base64.h.md](base64.h.md), [base64.cpp.md](base64.cpp.md)

## Boost.Fusion Integration

All three data types are adapted for Boost.Fusion, enabling:

- **Generic Programming**: Write code that works with all three types
- **Serialization**: Automatic serialization/deserialization
- **Introspection**: Compile-time and runtime reflection
- **Algorithms**: Generic algorithms on struct members

### Fusion Adaptations

```cpp
// JsonData adaptation
BOOST_FUSION_ADAPT_STRUCT(cfg::JsonData, (std::string, json_buffer))

// FhiclData adaptation (only buffer, not file_name)
BOOST_FUSION_ADAPT_STRUCT(cfg::FhiclData, (std::string, fhicl_buffer))

// XmlData adaptation
BOOST_FUSION_ADAPT_STRUCT(cfg::XmlData, (std::string, xml_buffer))
```

All three types have **identical Fusion interfaces** (single string member), enabling truly generic code.

**Documentation**: [data_json_fusion.h.md](data_json_fusion.h.md), [data_fhicl_fusion.h.md](data_fhicl_fusion.h.md), [data_xml_fusion.h.md](data_xml_fusion.h.md)

## Usage Examples

### Basic Usage

```cpp
#include "artdaq-database/BasicTypes/basictypes.h"

using namespace artdaq::database::basictypes;

// Create FHICL configuration
FhiclData fhicl("daq_parameter: 1000");

// Convert to JSON for database
JsonData json = fhicl;

// Store in database
database.store("config", json);

// Retrieve from database
JsonData retrieved = database.get("config");

// Convert back to FHICL
FhiclData restored(retrieved);

// Use configuration
std::cout << restored << "\n";
```

### Generic Programming with Fusion

```cpp
#include "artdaq-database/BasicTypes/data_fhicl_fusion.h"
#include "artdaq-database/BasicTypes/data_json_fusion.h"
#include "artdaq-database/BasicTypes/data_xml_fusion.h"
#include <boost/fusion/include/for_each.hpp>

// Generic function works with all three types
template<typename ConfigType>
bool validate(const ConfigType& config) {
    bool valid = true;
    boost::fusion::for_each(config, [&valid](const auto& buffer) {
        valid = valid && !buffer.empty();
    });
    return valid;
}

// Usage
FhiclData fhicl("param: value");
JsonData json("{}");
XmlData xml("<config/>");

bool fhicl_valid = validate(fhicl);  // Works
bool json_valid = validate(json);    // Works
bool xml_valid = validate(xml);      // Works
```

### Format Conversion

```cpp
// FHICL to XML (via JSON)
FhiclData fhicl("parameter: value");
JsonData json1 = fhicl;       // FHICL → JSON
XmlData xml(json1);           // JSON → XML

// XML to FHICL (via JSON)
XmlData xml2("<param>value</param>");
JsonData json2 = xml2;        // XML → JSON
FhiclData fhicl2(json2);      // JSON → FHICL
```

### Stream I/O

```cpp
// Read from file (JSON-encoded)
std::ifstream in("config.json");
FhiclData fhicl;
in >> fhicl;

// Write to file (raw format)
std::ofstream out("config.fcl");
out << fhicl;

// Console output
std::cout << "Config: " << fhicl << "\n";
```

## Error Handling

### Exceptions

All conversions can throw `std::runtime_error`:

```cpp
try {
    JsonData json = get_from_database();
    FhiclData fhicl(json);  // May throw
    // Use fhicl...
} catch (const std::runtime_error& e) {
    TLOG(1) << "Conversion failed: " << e.what();
    // Handle error
}
```

### Common Error Scenarios

1. **Empty Data**: JSON document is empty
2. **Missing Base64**: No "base64" field in JSON
3. **Invalid Base64**: Malformed Base64 encoding
4. **Parse Errors**: Invalid FHICL/XML syntax
5. **Regex Failures**: Multiple or missing Base64 fields

### Best Practices

```cpp
// Check for empty data
JsonData json = get_data();
if (json.empty()) {
    TLOG(1) << "Empty JSON data";
    return false;
}

// Wrap conversions in try-catch
try {
    FhiclData fhicl(json);
    return process(fhicl);
} catch (const std::runtime_error& e) {
    TLOG(1) << "Error: " << e.what();
    return false;
}
```

## TRACE Logging

All BasicTypes files use TRACE for debug logging:

```cpp
#define TRACE_NAME "data_fhicl.cpp"  // or data_json.cpp, etc.

TLOG(11) << "Converting JSON to FHICL...";
TLOG(12) << "Base64: " << base64_str;
TLOG(13) << "Result: " << result;
```

### TRACE Levels Used

- **TLOG(10-21)**: Conversion process steps
- **TLOG(1-5)**: Errors and important messages
- Higher numbers = more detailed/verbose

## Dependencies

### External Libraries

- **Boost.Fusion**: Struct adaptation and generic programming
- **Boost.Core**: Type demangling utilities
- **TRACE**: artdaq logging framework

### Internal Dependencies

- **DataFormats/Fhicl**: FHICL ↔ JSON conversion library
- **DataFormats/Xml**: XML ↔ JSON conversion library
- **DataFormats**: Shared literals and utilities

### Standard Library

- `<string>`: String operations
- `<iostream>`: Stream I/O
- `<regex>`: Base64 extraction from JSON
- `<utility>`: Move semantics

## Thread Safety

**None of the BasicTypes classes are thread-safe** for concurrent modification:

- ✅ **Multiple readers**: Safe (const methods)
- ✅ **Single writer**: Safe
- ❌ **Concurrent read/write**: Unsafe - requires external synchronization
- ❌ **Concurrent writes**: Unsafe - requires external synchronization

## Performance Considerations

### Memory

- Data stored as strings (not parsed structures)
- Base64 encoding increases size by ~33%
- Intermediate strings created during conversions
- Move semantics used where possible

### Speed

- Conversions involve: regex search, Base64 encode/decode, format parsing
- Stream I/O reads entire content into memory
- No lazy loading or streaming for large documents

### Optimization Tips

1. **Use Move Semantics**: Pass strings with `std::move`
2. **Cache Conversions**: Don't convert repeatedly
3. **Check Empty**: Use `empty()` before processing
4. **Batch Operations**: Convert multiple configs together if possible

## Testing Recommendations

When working with BasicTypes, test:

1. **Round-Trip Conversions**: Ensure data survives conversion cycles
2. **Edge Cases**: Empty strings, very large strings, special characters
3. **Error Handling**: Invalid JSON, missing Base64, malformed data
4. **Stream I/O**: Read/write from various stream types
5. **Generic Code**: Test template functions with all three types

### Example Test

```cpp
// Test round-trip conversion
FhiclData original("parameter: value");
JsonData json = original;
FhiclData restored(json);
assert(std::string(original) == std::string(restored));

// Test empty data handling
JsonData empty("");
assert(empty.empty());
try {
    FhiclData fhicl(empty);  // Should throw
    assert(false);
} catch (const std::runtime_error&) {
    // Expected
}
```

## Version Information

All data types provide a `type_version()` method:

```cpp
FhiclData::type_version()  // Returns "V100"
JsonData::type_version()   // Returns "V1.0.0"
XmlData::type_version()    // Returns "V100"
```

These versions are used for database collection naming and schema evolution.

## Migration and Evolution

### Adding New Members

If adding members to existing types:

1. Update the struct definition
2. Update Boost.Fusion adaptation (if needed)
3. Update conversions to handle new members
4. Increment `type_version()`
5. Test backward compatibility

### Adding New Formats

To add a new configuration format (e.g., YAML):

1. Create `data_yaml.h` and `data_yaml.cpp`
2. Implement conversions with JsonData
3. Create Fusion adaptation in `data_yaml_fusion.h`
4. Follow the same pattern as existing types
5. Update `basictypes.h` to include new type

## Known Issues

1. **Typo in Error Messages**: "convertion" instead of "conversion" in some error messages
2. **TRACE Level Reuse**: `data_xml.cpp` reuses TLOG(15) multiple times
3. **Header Guard Comment**: `data_xml.h` has incorrect comment on final endif

These are cosmetic issues that don't affect functionality.

## Best Practices Summary

1. ✅ **Use `basictypes.h`** in source files for convenience
2. ✅ **Use specific headers** in header files to minimize dependencies
3. ✅ **Handle exceptions** when converting untrusted data
4. ✅ **Check `empty()`** before processing
5. ✅ **Use move semantics** when passing strings
6. ✅ **Set `fhicl_file_name`** when loading FHICL from files
7. ✅ **Include Fusion headers** only when needed for generic programming
8. ✅ **Validate data** before creating BasicTypes objects

## For Junior Developers

### Quick Start

1. **Include the module**: `#include "artdaq-database/BasicTypes/basictypes.h"`
2. **Choose your format**: FhiclData, JsonData, or XmlData
3. **Create objects**: Pass configuration strings to constructors
4. **Convert as needed**: Use implicit conversions or constructors
5. **Handle errors**: Wrap conversions in try-catch

### Common Patterns

**Pattern 1: Load config from file, store in database**
```cpp
std::ifstream file("config.json");
FhiclData fhicl;
file >> fhicl;

JsonData json = fhicl;
database.store("my_config", json);
```

**Pattern 2: Retrieve from database, use as config**
```cpp
JsonData json = database.get("my_config");
FhiclData fhicl(json);
std::cout << fhicl;
```

**Pattern 3: Format conversion**
```cpp
XmlData xml = load_xml_from_somewhere();
JsonData json = xml;
FhiclData fhicl(json);  // Now in FHICL format
```

## Further Reading

- Individual file documentation (see links above)
- artdaq project documentation for FHICL language details
- Boost.Fusion documentation for advanced generic programming
- RFC 4648 for Base64 specification

## Contributing

When modifying BasicTypes:

1. Maintain consistency across all three types (JSON, FHICL, XML)
2. Update all related documentation
3. Add tests for new functionality
4. Follow existing code style and patterns
5. Update `type_version()` if changing data structures

## Contact

For questions about BasicTypes, consult:
- This documentation
- artdaq-database project documentation
- artdaq project maintainers
