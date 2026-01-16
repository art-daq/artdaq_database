# basictypes.h

**Path:** `artdaq-database/BasicTypes/basictypes.h`

**Purpose:** Convenience header that includes all BasicTypes data structures (JsonData, FhiclData, XmlData) in a single include. This is the recommended header for source files that need to work with multiple configuration formats.


## Key Concepts

### Umbrella Header Pattern

This header follows the "umbrella header" pattern common in C++ libraries. Rather than requiring users to include multiple individual headers, this single header provides access to the entire BasicTypes API. This pattern is similar to including `<iostream>` which brings in multiple stream types.

### JSON as the Pivot Format

All configuration formats in artdaq-database convert to and from JSON:

```
FHiCL <--> JSON <--> XML
              |
          Database
     (MongoDB/FileSystemDB)
```

JSON serves as the canonical storage format, enabling:
- Uniform database storage regardless of input format
- Simplified conversion logic (N formats require only 2N converters, not N^2)
- Native support for both MongoDB (BSON is JSON-based) and FileSystemDB (JSON files)

### When to Use This Header

**Use `basictypes.h` when:**
- Your source file needs multiple BasicTypes (e.g., both FhiclData and JsonData)
- You want simplified includes without tracking individual dependencies
- You are writing application code that uses format conversions
- You need to work with all three configuration formats

**Use individual headers when:**
- Your header file only needs one specific type (to minimize compile dependencies)
- Compile time is critical and you want minimal includes
- You need Boost.Fusion support (include the specific `*_fusion.h` header separately)

## Thread Safety

- **Thread-safe:** N/A (header-only, no runtime behavior)
- **Concurrent access:** All included types are safe for concurrent read access; concurrent writes require external synchronization
- **Locking:** None - see individual type documentation for thread safety details

## Dependencies

| Include | Purpose |
|---------|---------|
| `common.h` | Shared includes: TRACE logging framework, Boost.Core demangle utilities |
| `data_fhicl.h` | FhiclData structure for FHiCL (Fermilab Hierarchical Configuration Language) configurations |
| `data_json.h` | JsonData structure for JSON data - the pivot format for all conversions and database storage |
| `data_xml.h` | XmlData structure for XML configurations |

## Included Types

### JsonData

**Brief:** The central pivot format for all database storage. All other formats convert to and from JsonData.

**Key features:**
- Template `convert_to<T>()` and `convert_from<T>()` methods for format conversion
- String buffer storage with `json_buffer` member
- `empty()` check method for validation
- Implicit string conversion operators for interoperability
- Stream I/O operators for file operations

**Thread Safety:** Not thread-safe for concurrent modification; multiple readers are safe.

**Namespace:** `artdaq::database::basictypes::JsonData`

**Documentation:** [data_json.h](./data_json.h.md)

---

### FhiclData

**Brief:** Wrapper for FHiCL (Fermilab Hierarchical Configuration Language) configurations - the primary configuration format in the artdaq ecosystem.

**Key features:**
- Bidirectional conversion with JsonData via implicit conversion operators
- Optional filename metadata (`fhicl_file_name` member) for tracking source files
- Stream I/O operators for reading and writing FHiCL content
- TRACE logging integration for debugging

**Thread Safety:** Not thread-safe for concurrent modification; multiple readers are safe.

**Namespace:** `artdaq::database::basictypes::FhiclData`

**Documentation:** [data_fhicl.h](./data_fhicl.h.md)

---

### XmlData

**Brief:** Wrapper for XML configuration data, useful for interoperability with external systems.

**Key features:**
- Bidirectional conversion with JsonData via implicit conversion operators
- Stream I/O operators for reading and writing XML content
- Simpler structure than FhiclData (no filename metadata)
- TRACE logging integration for debugging

**Thread Safety:** Not thread-safe for concurrent modification; multiple readers are safe.

**Namespace:** `artdaq::database::basictypes::XmlData`

**Documentation:** [data_xml.h](./data_xml.h.md)

## Example Usage

### Basic Format Conversion with Error Handling

```cpp
#include "artdaq-database/BasicTypes/basictypes.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

using namespace artdaq::database::basictypes;

void storeConfiguration(const std::string& fhicl_content) {
  try {
    // Validate input
    if (fhicl_content.empty()) {
      std::cerr << "Error: Empty FHiCL content provided\n";
      return;
    }

    // Create FHiCL data from string
    FhiclData fhicl(fhicl_content);
    fhicl.fhicl_file_name = "detector_config.fcl";

    // Convert to JSON for database storage (implicit conversion)
    JsonData json = fhicl;

    // Check conversion succeeded
    if (json.empty()) {
      std::cerr << "Conversion produced empty JSON\n";
      return;
    }

    // Store in database (pseudo-code)
    // database.store("detector_config", json);
    std::cout << "Configuration stored successfully\n";
    std::cout << "JSON size: " << std::string(json).length() << " bytes\n";

  } catch (const std::runtime_error& e) {
    std::cerr << "Conversion error: " << e.what() << "\n";
  }
}
```

### Multi-Format Conversion

```cpp
#include "artdaq-database/BasicTypes/basictypes.h"
#include <iostream>

using namespace artdaq::database::basictypes;

void convertBetweenFormats() {
  try {
    // Load XML configuration
    XmlData xml(R"(
      <config>
        <param name="threshold">100</param>
        <param name="enabled">true</param>
      </config>
    )");

    // Validate XML is not empty
    if (std::string(xml).empty()) {
      std::cerr << "Error: XML configuration is empty\n";
      return;
    }

    // Convert to JSON (pivot format)
    JsonData json = xml;

    if (json.empty()) {
      std::cerr << "Error: XML to JSON conversion produced empty result\n";
      return;
    }

    // Convert to FHiCL
    FhiclData fhicl(json);

    // Output in FHiCL format
    std::cout << "Converted to FHiCL:\n" << fhicl << "\n";

  } catch (const std::runtime_error& e) {
    std::cerr << "Format conversion failed: " << e.what() << "\n";
  }
}
```

### Reading from Files with Error Handling

```cpp
#include "artdaq-database/BasicTypes/basictypes.h"
#include <fstream>
#include <iostream>

using namespace artdaq::database::basictypes;

bool loadFhiclFromFile(const std::string& filepath, FhiclData& output) {
  try {
    std::ifstream file(filepath);
    if (!file) {
      std::cerr << "Error: Cannot open file: " << filepath << "\n";
      return false;
    }

    // Read raw FHiCL content
    std::string content((std::istreambuf_iterator<char>(file)), {});

    if (content.empty()) {
      std::cerr << "Error: File is empty: " << filepath << "\n";
      return false;
    }

    // Create FhiclData from string (not using operator>>)
    output = FhiclData(content);
    output.fhicl_file_name = filepath;

    return true;

  } catch (const std::exception& e) {
    std::cerr << "Error loading FHiCL file: " << e.what() << "\n";
    return false;
  }
}
```

## Relationship to Other Components

```
                    basictypes.h (umbrella)
                          |
          +---------------+---------------+
          |               |               |
    data_json.h     data_fhicl.h    data_xml.h
          |               |               |
          +-------+-------+-------+-------+
                  |
              common.h
                  |
          +-------+-------+
          |               |
       trace.h    boost/core/demangle.hpp
```

This header is the primary entry point for:
- **ConfigurationDB module**: Uses BasicTypes for all storage operations
- **Utilities**: CLI tools for format conversion (fhicl2json, etc.)
- **User applications**: Code working with configuration data

## See Also

- [common.h](./common.h.md) - Shared utilities included by all BasicTypes headers
- [data_json.h](./data_json.h.md) - JsonData type documentation (pivot format)
- [data_fhicl.h](./data_fhicl.h.md) - FhiclData type documentation
- [data_xml.h](./data_xml.h.md) - XmlData type documentation
- [data_json_fusion.h](./data_json_fusion.h.md) - Boost.Fusion support for generic programming
- [README.md](./README.md) - Module overview

## Notes for Developers

### Common Pitfalls

- **Pitfall 1:** Including this header in other headers unnecessarily increases compile times. Use specific headers (`data_json.h`, etc.) in `.h` files.
- **Pitfall 2:** This header does NOT include Fusion adapters. If you need Boost.Fusion support for generic programming, you must also include the specific `*_fusion.h` headers.
- **Pitfall 3:** Forgetting to handle exceptions from format conversions. Always wrap conversion operations in try-catch blocks when dealing with untrusted input.

### Compilation Dependencies

Including this header brings in:
- `<string>` from standard library (via data types)
- TRACE logging framework (via common.h)
- Boost.Core `demangle.hpp` (via common.h)

The header does NOT include:
- Boost.Fusion adapters (include `*_fusion.h` separately)
- DataFormats converters (linked at compile time via .cpp files)
- Base64 utilities (internal implementation detail)

### Best Practices

```cpp
// GOOD: Use basictypes.h in source files for convenience
// mycode.cpp
#include "artdaq-database/BasicTypes/basictypes.h"

// GOOD: Use specific headers in header files to minimize dependencies
// mycode.h
#include "artdaq-database/BasicTypes/data_json.h"  // Only JsonData needed

// GOOD: Handle conversion errors
try {
  JsonData json = fhicl;
  if (json.empty()) {
    // Handle empty result
  }
} catch (const std::runtime_error& e) {
  // Handle error appropriately
}

// BAD: Ignoring conversion errors
JsonData json = fhicl;  // May throw std::runtime_error!
```

### Anti-patterns

```cpp
// DON'T do this - including basictypes.h in a header when only one type is needed:
// myheader.h
#include "artdaq-database/BasicTypes/basictypes.h"  // Slow compilation
class MyClass {
  artdaq::database::basictypes::JsonData data_;
};

// DO this instead - include only what you need:
// myheader.h
#include "artdaq-database/BasicTypes/data_json.h"  // Faster compilation
class MyClass {
  artdaq::database::basictypes::JsonData data_;
};

// DON'T do this - no validation or error handling:
void processConfig(const std::string& input) {
  FhiclData fhicl(input);
  JsonData json = fhicl;  // May throw!
  // Use json...
}

// DO this instead - validate and handle errors:
bool processConfig(const std::string& input) {
  if (input.empty()) {
    return false;
  }
  try {
    FhiclData fhicl(input);
    JsonData json = fhicl;
    if (json.empty()) {
      return false;
    }
    // Use json...
    return true;
  } catch (const std::runtime_error& e) {
    std::cerr << "Processing error: " << e.what() << "\n";
    return false;
  }
}
```
