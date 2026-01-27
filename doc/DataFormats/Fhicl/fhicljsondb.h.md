# fhicljsondb.h

**Path:** `artdaq-database/DataFormats/Fhicl/fhicljsondb.h`

**Purpose:** Provides the high-level public API for bidirectional conversion between FHiCL configuration text and JSON string format. This is the primary entry point for applications that need to convert FHiCL configurations for database storage or retrieval.


## Key Concepts

### FHiCL-JSON Conversion Pipeline

This header exposes two simple functions that hide the complexity of the underlying conversion process:

1. **FHiCL to JSON** (`fhicl_to_json`): Parses FHiCL text, extracts comments and metadata, and produces a JSON string suitable for database storage
2. **JSON to FHiCL** (`json_to_fhicl`): Takes a JSON string from the database and regenerates the original FHiCL format

### Database Storage Format

The JSON output from `fhicl_to_json` includes:
- **document**: The actual configuration data (prolog and main sections)
- **comments**: Preserved comments with line numbers
- **origin**: Metadata about the source (format, filename, timestamp)
- **version**: Version information (if provided)
- **entities**: Associated entity names
- **configurations**: Associated configuration names

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Functions are not thread-safe; each call should operate on independent data
- **Locking:** None; callers must provide external synchronization if needed

## Dependencies

| Include | Purpose |
|---------|---------|
| `<string>` | `std::string` for input/output parameters |

## Functions

### `fhicl_to_json(std::string const& fhicl, std::string const& filename, std::string& json) -> bool`

**Brief:** Converts FHiCL configuration text to a JSON string suitable for database storage, preserving comments and adding metadata.

**Parameters:**
- `fhicl` - The FHiCL document text to convert (must not be empty)
- `filename` - The original filename for metadata (must not be empty)
- `json` - Output JSON string (must be empty on input; populated on success)

**Preconditions:**
- `fhicl` must not be empty
- `filename` must not be empty
- `json` must be empty

**Returns:** `true` on successful conversion, `false` on failure

**Postconditions:**
- On success, `json` contains a valid JSON string with document, comments, origin, etc.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `::fhicl::exception` | When FHiCL parsing fails due to syntax errors |

**Thread Safety:** Not thread-safe

**Side Effects:**
- Sets global `::shims::isSnippetMode(true)` (affects other FHiCL parsing in same process)

**Example:**
```cpp
#include "artdaq-database/DataFormats/Fhicl/fhicljsondb.h"
#include <iostream>

void convertFhiclToJson() {
  std::string fhicl_content = R"(
    BEGIN_PROLOG
    base_threshold: 100
    END_PROLOG

    # Detector configuration
    detector: {
      name: "muon_counter"
      threshold: @local::base_threshold
      enabled: true
    }
  )";

  std::string json_output;

  try {
    if (artdaq::database::fhicljson::fhicl_to_json(
          fhicl_content, "detector.fcl", json_output)) {
      std::cout << "Converted successfully:\n" << json_output << "\n";
      // Store json_output in database...
    } else {
      std::cerr << "Conversion failed\n";
    }
  } catch (::fhicl::exception const& e) {
    std::cerr << "FHiCL parse error: " << e.what() << "\n";
  }
}
```

---

### `json_to_fhicl(std::string const& json, std::string& fhicl, std::string& filename) -> bool`

**Brief:** Converts a JSON string (from database storage) back to FHiCL configuration text format.

**Parameters:**
- `json` - The JSON string to convert (must not be empty, must be valid database format)
- `fhicl` - Output FHiCL text (must be empty on input; populated on success)
- `filename` - Output filename extracted from origin metadata (populated on success; set to "notprovided" if not found)

**Preconditions:**
- `json` must not be empty
- `json` must contain valid database-format JSON with `document` node
- `fhicl` must be empty

**Returns:** `true` on successful conversion, `false` on failure

**Postconditions:**
- On success, `fhicl` contains valid FHiCL text
- On success, `filename` contains the original filename or "notprovided"

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `boost::bad_get` | When JSON structure does not match expected format |

**Thread Safety:** Not thread-safe

**Example:**
```cpp
#include "artdaq-database/DataFormats/Fhicl/fhicljsondb.h"
#include <iostream>

void convertJsonToFhicl(std::string const& json_from_db) {
  std::string fhicl_output;
  std::string filename;

  try {
    if (artdaq::database::fhicljson::json_to_fhicl(
          json_from_db, fhicl_output, filename)) {
      std::cout << "Original file: " << filename << "\n";
      std::cout << "FHiCL content:\n" << fhicl_output << "\n";
    } else {
      std::cerr << "Conversion failed\n";
    }
  } catch (std::exception const& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

---

### `debug::FhiclJson() -> void`

**Brief:** Enables TRACE debugging output for the fhicljsondb component.

**Preconditions:** None

**Returns:** Nothing

**Postconditions:**
- TRACE logging is enabled for "fhicljsondb.cpp"
- All TRACE levels are enabled

**Thread Safety:** Not thread-safe (modifies global TRACE state)

**Side Effects:**
- Modifies global TRACE control settings

---

### `debug::FCL2JSONDB() -> void`

**Brief:** Enables TRACE debugging output for the convertfhicl2jsondb component.

**Preconditions:** None

**Returns:** Nothing

**Postconditions:**
- TRACE logging is enabled for the conversion layer
- All TRACE levels are enabled

**Thread Safety:** Not thread-safe (modifies global TRACE state)

**Side Effects:**
- Modifies global TRACE control settings

---

### `fhicl::debug::FhiclReader() -> void`

**Brief:** Enables TRACE debugging output for the FhiclReader component.

**Note:** This function is also declared in `fhicl_reader.h`; the declaration here provides convenient access without additional includes.

---

### `fhicl::debug::FhiclWriter() -> void`

**Brief:** Enables TRACE debugging output for the FhiclWriter component.

**Note:** This function is also declared in `fhicl_writer.h`; the declaration here provides convenient access without additional includes.

## Relationship to Other Components

This header is the primary public interface for FHiCL conversion. It sits at the top of the conversion layer:

```
Application Code
       |
       v
+------------------+
|   fhicljsondb.h  |  <-- This file (public API)
+------------------+
       |
       v
+------------------+     +------------------+
| FhiclReader      |     | FhiclWriter      |
+------------------+     +------------------+
       |                        |
       v                        v
+------------------+     +------------------+
| fhiclcpp library |     | Karma grammar    |
+------------------+     +------------------+
```

The functions in this header:
- Use `FhiclReader` for FHiCL parsing
- Use `FhiclWriter` for FHiCL generation
- Use `JsonReader` and `JsonWriter` for JSON serialization
- Add origin metadata (format, filename, timestamp)

## See Also

- [fhicljsondb.cpp](./fhicljsondb.cpp.md) - Implementation details
- [fhicl_reader.h](./fhicl_reader.h.md) - Low-level FHiCL parsing
- [fhicl_writer.h](./fhicl_writer.h.md) - Low-level FHiCL generation
- [convertfhicl2jsondb.h](./convertfhicl2jsondb.h.md) - Element-level conversion functors
- [json_common.h](../Json/json_common.h.md) - JSON reader/writer classes

## Notes for Developers

### JSON Output Format

The `fhicl_to_json` function produces JSON with this structure:

```json
{
  "document": {
    "data": {
      "prolog": { /* prolog key-value pairs */ },
      "main": { /* main key-value pairs */ }
    },
    "metadata": {
      "prolog": { /* prolog type information */ },
      "main": { /* main type information */ }
    }
  },
  "comments": [
    { "linenum": 1, "value": "# Comment text" }
  ],
  "origin": {
    "format": "fhicl",
    "name": "filename.fcl",
    "source": "fhicl_to_json",
    "timestamp": "2024-01-15T10:30:00Z"
  },
  "version": "notprovided",
  "entities": [],
  "configurations": []
}
```

### Common Pitfalls

- **Pitfall 1:** Passing a non-empty output string to either function will trigger an assertion failure.
- **Pitfall 2:** The `json_to_fhicl` function expects the full database JSON format, not just the document node.
- **Pitfall 3:** The filename parameter in `json_to_fhicl` is an output parameter, not input.

### Anti-patterns

```cpp
// DON'T do this:
std::string json = "existing";
fhicl_to_json(fhicl, "file.fcl", json);  // Assertion failure

// DO this instead:
std::string json;
if (fhicl_to_json(fhicl, "file.fcl", json)) {
  // Use json
}
```

### Round-Trip Example

```cpp
#include "artdaq-database/DataFormats/Fhicl/fhicljsondb.h"
#include <cassert>

void roundTripTest() {
  std::string original_fhicl = "timeout: 30\nenabled: true";

  // FHiCL -> JSON
  std::string json;
  bool ok = artdaq::database::fhicljson::fhicl_to_json(
    original_fhicl, "test.fcl", json);
  assert(ok);

  // JSON -> FHiCL
  std::string restored_fhicl;
  std::string filename;
  ok = artdaq::database::fhicljson::json_to_fhicl(
    json, restored_fhicl, filename);
  assert(ok);

  // Content should be semantically equivalent
  // (whitespace and formatting may differ)
}
```
