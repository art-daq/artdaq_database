# data_fhicl.h

**Path:** `artdaq-database/BasicTypes/data_fhicl.h`

**Purpose:** Defines the FhiclData structure, which wraps FHiCL (Fermilab Hierarchical Configuration Language) formatted configuration data. FHiCL is the primary configuration language in the artdaq ecosystem, and this class provides bidirectional conversion with JSON for database storage.


## Key Concepts

### FHiCL (Fermilab Hierarchical Configuration Language)

FHiCL is a configuration language developed at Fermilab for high-energy physics experiments. It features:

- **Hierarchical structure:** Nested tables using curly braces
- **Human-readable syntax:** Similar to JSON but more flexible
- **References and includes:** Configuration reuse via `@local::` references
- **Flexible arrays:** Using square brackets

Example FHiCL syntax:
```fhicl
# Comment
module_name: {
    parameter1: "value"
    threshold: 100
    enabled: true
    channels: [0, 1, 2, 3, 4]
    nested_table: {
        inner_param: 42
    }
}
```

### JSON as Pivot Format

FhiclData converts to/from JsonData for database storage. The conversion process:
1. Parses FHiCL to an intermediate JSON structure
2. Base64-encodes the original FHiCL for perfect round-trip fidelity
3. Embeds both in the final JSON document

This ensures:
- Structured data for querying
- Perfect preservation of original FHiCL formatting
- Safe handling of special characters

### File Name Metadata

Unlike JsonData and XmlData, FhiclData includes a `fhicl_file_name` member. This metadata tracks the original source file and is useful for:
- Debugging configuration issues
- Audit trails
- Error messages that reference file locations

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Multiple readers are safe; concurrent read/write requires external synchronization
- **Locking:** No internal locking; callers must synchronize access when sharing FhiclData objects between threads

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/BasicTypes/common.h` | TRACE logging framework and Boost.Core utilities |

## Classes/Structures

### `FhiclData`

**Brief:** A value type that wraps FHiCL-formatted configuration data with bidirectional JSON conversion support and optional filename metadata.

```cpp
struct FhiclData final {
    FhiclData(std::string);
    FhiclData() = default;

    FhiclData(JsonData const&);
    operator JsonData() const;
    operator std::string const&() const;

    static constexpr auto type_version() { return "V100"; }

    std::string fhicl_buffer = "";
    std::string fhicl_file_name = "notprovided";
};
```

**Thread Safety:** Not thread-safe for concurrent modification. Multiple concurrent readers (const methods only) are safe.

#### Member Variables

##### `fhicl_buffer`

**Brief:** Holds the FHiCL-formatted configuration string containing the actual configuration data.

**Type:** `std::string`

**Default:** Empty string `""`

---

##### `fhicl_file_name`

**Brief:** Stores the original filename metadata for debugging and auditing purposes.

**Type:** `std::string`

**Default:** `"notprovided"`

**Note:** This is metadata only - it does not affect the configuration content or database storage. Set this when loading from files to enable better error messages and audit trails.

#### Constructors

##### `FhiclData(std::string buffer)`

**Brief:** Constructs a FhiclData object from a FHiCL-formatted string, using move semantics for efficiency.

**Parameters:**
- `buffer` - A string containing FHiCL configuration data. The string is moved into `fhicl_buffer`.

**Preconditions:**
- None (string may be empty, but empty strings may cause conversion failures later)

**Postconditions:**
- `fhicl_buffer` contains the provided string (moved)
- `fhicl_file_name` remains at default value `"notprovided"`
- Input `buffer` is in valid but unspecified state (moved-from)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_alloc` | If memory allocation fails (extremely rare) |

**Thread Safety:** Safe (constructor creates new object)

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_fhicl.h"
#include <iostream>

using namespace artdaq::database::basictypes;

void createFhiclData() {
  try {
    // Create from FHiCL string
    FhiclData config("daq_settings: { buffer_size: 8192 }");

    // Optionally set filename metadata for debugging
    config.fhicl_file_name = "detector_config.fcl";

    std::cout << "Configuration created:\n" << config << "\n";
    std::cout << "Source file: " << config.fhicl_file_name << "\n";

  } catch (const std::exception& e) {
    std::cerr << "Error creating FhiclData: " << e.what() << "\n";
  }
}
```

---

##### `FhiclData()` (default)

**Brief:** Creates an empty FhiclData with default values.

**Preconditions:** None

**Postconditions:**
- `fhicl_buffer` is empty string `""`
- `fhicl_file_name` is `"notprovided"`

**Throws:** None

**Thread Safety:** Safe (constructor creates new object)

---

##### `FhiclData(JsonData const& document)`

**Brief:** Constructs FhiclData by extracting and decoding FHiCL from a JSON document that contains Base64-encoded FHiCL data. This is the reverse operation of the `operator JsonData()` conversion.

**Parameters:**
- `document` - A JsonData object containing Base64-encoded FHiCL. Must not be empty and must contain a "base64" field.

**Preconditions:**
- `document` must not be empty
- `document.json_buffer` must contain a JSON structure with a "base64" field
- The "base64" field must contain valid Base64-encoded data that decodes to valid JSON

**Postconditions:**
- `fhicl_buffer` contains the decoded FHiCL configuration
- `fhicl_file_name` may be populated if the JSON contains filename metadata

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` | When `document` is empty (assertion failure) |
| `std::runtime_error` | When regex fails to find "base64" field in JSON |
| `std::runtime_error` | When regex finds unexpected number of matches |
| `std::runtime_error` | When Base64 decoding or FHiCL conversion fails |

**Thread Safety:** Safe (constructor creates new object, reads from const input)

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_fhicl.h"
#include "artdaq-database/BasicTypes/data_json.h"
#include <iostream>

using namespace artdaq::database::basictypes;

void convertFromJson() {
  // Assume we retrieved JSON from database
  JsonData json_from_db = retrieveFromDatabase("config_id");

  // Validate before conversion
  if (json_from_db.empty()) {
    std::cerr << "Error: Configuration not found in database\n";
    return;
  }

  try {
    // Convert to FHiCL
    FhiclData fhicl(json_from_db);
    std::cout << "Retrieved FHiCL configuration:\n" << fhicl << "\n";

  } catch (const std::runtime_error& e) {
    std::cerr << "Failed to convert JSON to FHiCL: " << e.what() << "\n";
    // The JSON may be malformed or missing the base64 field
  }
}
```

#### Conversion Operators

##### `operator JsonData() const`

**Brief:** Converts the FHiCL data to JSON format for database storage. The conversion includes Base64 encoding of the original FHiCL content for perfect round-trip fidelity.

**Returns:** JsonData containing the FHiCL configuration in JSON format with embedded Base64.

**Preconditions:**
- `fhicl_buffer` should contain valid FHiCL syntax

**Postconditions:**
- Returned JsonData contains the converted configuration
- Original FhiclData object is unchanged

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` | When FHiCL-to-JSON conversion fails (invalid FHiCL syntax) |

**Thread Safety:** Safe (reads only, const method)

**Side Effects:** None

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_fhicl.h"
#include "artdaq-database/BasicTypes/data_json.h"
#include <iostream>

using namespace artdaq::database::basictypes;

void storeToDatabase() {
  try {
    FhiclData fhicl("detector_module: { gain: 1.5 threshold: 100 }");
    fhicl.fhicl_file_name = "detector.fcl";

    // Convert to JsonData (implicit conversion via operator)
    JsonData json = fhicl;

    // Validate conversion succeeded
    if (json.empty()) {
      std::cerr << "Error: Conversion produced empty JSON\n";
      return;
    }

    // Store JSON in database
    std::cout << "Storing configuration, JSON size: "
              << std::string(json).size() << " bytes\n";
    // database.store("detector_config", json);

  } catch (const std::runtime_error& e) {
    std::cerr << "Conversion failed: " << e.what() << "\n";
    // The FHiCL may have syntax errors
  }
}
```

---

##### `operator std::string const&() const`

**Brief:** Provides implicit conversion to const string reference, returning the raw FHiCL content.

**Returns:** Const reference to `fhicl_buffer`.

**Preconditions:** None

**Postconditions:** None (no state change)

**Throws:** None

**Thread Safety:** Safe (returns const reference)

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_fhicl.h"
#include <iostream>

using namespace artdaq::database::basictypes;

void useFhiclAsString() {
  FhiclData fhicl("param: value\ncount: 42");

  // Implicit conversion to const string&
  const std::string& content = fhicl;
  std::cout << "FHiCL content: " << content << "\n";

  // Works with functions expecting const string&
  processString(fhicl);  // Implicit conversion
}
```

#### Static Methods

##### `type_version() -> const char*` (static constexpr)

**Brief:** Returns a version identifier string used for database collection naming and schema compatibility.

**Returns:** `"V100"` - indicates version 1.0.0 of the FhiclData schema.

**Preconditions:** None

**Postconditions:** None

**Throws:** None

**Thread Safety:** Safe (static constexpr, no state)

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_fhicl.h"
#include <iostream>
#include <string>

using namespace artdaq::database::basictypes;

void showSchemaVersion() {
  // Get schema version at compile time
  constexpr auto version = FhiclData::type_version();
  std::cout << "FhiclData schema version: " << version << "\n";

  // Used for collection naming
  std::string collection = std::string("FhiclData_") + FhiclData::type_version();
  std::cout << "Collection name: " << collection << "\n";  // "FhiclData_V100"
}
```

## Functions

### `operator>>(std::istream& is, FhiclData& data) -> std::istream&`

**Brief:** Reads FHiCL data from an input stream. The input is expected to be JSON-encoded FHiCL, not raw FHiCL text.

**Parameters:**
- `is` - Input stream containing JSON-encoded FHiCL data
- `data` - FhiclData object to populate

**Preconditions:**
- `is` should be in a valid state
- Stream content should be JSON-encoded FHiCL (as produced by `operator JsonData()`)

**Returns:** Reference to the input stream.

**Postconditions:**
- `data` contains the decoded FHiCL configuration
- Stream position is at EOF

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` | When the stream content cannot be converted to FHiCL |

**Thread Safety:** Unsafe (modifies `data`)

**Side Effects:**
- Reads entire stream content
- Stream position will be at EOF after read

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_fhicl.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace artdaq::database::basictypes;

bool loadJsonEncodedFhicl(const std::string& filepath, FhiclData& output) {
  std::ifstream file(filepath);
  if (!file) {
    std::cerr << "Error: Cannot open file: " << filepath << "\n";
    return false;
  }

  try {
    file >> output;  // Reads JSON-encoded FHiCL
    output.fhicl_file_name = filepath;
    return true;

  } catch (const std::runtime_error& e) {
    std::cerr << "Error reading FHiCL: " << e.what() << "\n";
    return false;
  }
}
```

---

### `operator<<(std::ostream& os, FhiclData const& data) -> std::ostream&`

**Brief:** Writes raw FHiCL text to the output stream (not JSON-encoded).

**Parameters:**
- `os` - Output stream
- `data` - FhiclData object to write

**Preconditions:**
- `os` should be in a valid state

**Returns:** Reference to the output stream.

**Postconditions:**
- `fhicl_buffer` content written to stream
- Stream position advanced

**Throws:**

| Exception | Condition |
|-----------|-----------|
| Stream exceptions | If stream is configured to throw on errors |

**Thread Safety:** Safe if `data` is not concurrently modified

**Side Effects:**
- Writes `fhicl_buffer` content to stream

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_fhicl.h"
#include <iostream>
#include <fstream>

using namespace artdaq::database::basictypes;

void writeFhiclToFile(const FhiclData& fhicl, const std::string& filepath) {
  std::ofstream out(filepath);
  if (!out) {
    std::cerr << "Error: Cannot open file for writing: " << filepath << "\n";
    return;
  }

  out << fhicl;  // Writes raw FHiCL text

  if (!out) {
    std::cerr << "Error: Write operation failed\n";
    return;
  }

  std::cout << "Wrote FHiCL to: " << filepath << "\n";
}
```

---

### `TraceStreamer::operator<<(const FhiclData& r)` (template specialization)

**Brief:** Enables FhiclData objects to be used directly in TRACE logging statements for debugging.

**Parameters:**
- `r` - FhiclData object to log

**Returns:** Reference to TraceStreamer (for chaining)

**Thread Safety:** Safe (TRACE logging is thread-safe)

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_fhicl.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif
#define TRACE_NAME "my_module.cpp"

using namespace artdaq::database::basictypes;

void debugWithTrace() {
  FhiclData fhicl("param: value\nthreshold: 100");
  fhicl.fhicl_file_name = "debug_config.fcl";

  TLOG(10) << "Processing configuration: " << fhicl;
  TLOG(11) << "Source file: " << fhicl.fhicl_file_name;
}
```

## Relationship to Other Components

### Data Flow

```
FHiCL Config File --> FhiclData --> JsonData --> Database (MongoDB/FileSystemDB)
Database --> JsonData --> FhiclData --> FHiCL Config File
```

### Module Dependencies

- **data_fhicl.cpp** - Implements constructors, conversion operators, and template specializations
- **data_fhicl_fusion.h** - Provides Boost.Fusion adaptation for generic programming
- **data_json.h** - JsonData is the pivot format for all conversions
- **base64.h** - Used internally for encoding FHiCL in JSON documents
- **DataFormats/Fhicl/** - Provides underlying FHiCL parsing and conversion

## See Also

- [data_fhicl.cpp](./data_fhicl.cpp.md) - Implementation details
- [data_fhicl_fusion.h](./data_fhicl_fusion.h.md) - Boost.Fusion adaptation
- [data_json.h](./data_json.h.md) - JsonData pivot format
- [basictypes.h](./basictypes.h.md) - Umbrella header
- [External: FHiCL documentation](https://cdcvs.fnal.gov/redmine/projects/fhicl-cpp) - FHiCL language reference

## Notes for Developers

### Common Pitfalls

- **Pitfall 1:** Forgetting to set `fhicl_file_name` when loading from files. This metadata is useful for debugging error messages.
- **Pitfall 2:** Expecting `operator>>` to read raw FHiCL text. It reads JSON-encoded FHiCL. For raw FHiCL, construct directly from a string read from the file.
- **Pitfall 3:** Not handling exceptions during conversion. Always wrap conversions in try-catch when dealing with untrusted input.
- **Pitfall 4:** Passing an empty JsonData to the constructor. This will throw a runtime error.

### Reading Raw FHiCL Files

The `operator>>` expects JSON-encoded FHiCL, not raw FHiCL text. To read raw FHiCL files:

```cpp
#include "artdaq-database/BasicTypes/data_fhicl.h"
#include <fstream>
#include <iostream>

using namespace artdaq::database::basictypes;

bool loadRawFhicl(const std::string& filepath, FhiclData& output) {
  std::ifstream file(filepath);
  if (!file) {
    std::cerr << "Error: Cannot open file: " << filepath << "\n";
    return false;
  }

  try {
    // Read raw FHiCL content as a string
    std::string content((std::istreambuf_iterator<char>(file)), {});

    if (content.empty()) {
      std::cerr << "Error: File is empty\n";
      return false;
    }

    // Create FhiclData from the raw string
    output = FhiclData(content);
    output.fhicl_file_name = filepath;

    return true;

  } catch (const std::exception& e) {
    std::cerr << "Error loading FHiCL: " << e.what() << "\n";
    return false;
  }
}
```

### Anti-patterns

```cpp
// DON'T do this - ignoring conversion errors:
JsonData json = database.get("config");
FhiclData fhicl(json);  // May throw if JSON is invalid!

// DO this instead - handle conversion errors:
try {
  JsonData json = database.get("config");
  if (json.empty()) {
    std::cerr << "Configuration not found\n";
    return;
  }
  FhiclData fhicl(json);
  processConfig(fhicl);
} catch (const std::runtime_error& e) {
  std::cerr << "Configuration error: " << e.what() << "\n";
}

// DON'T do this - reading raw FHiCL with operator>>:
std::ifstream file("config.fcl");  // Raw FHiCL file
FhiclData fhicl;
file >> fhicl;  // WRONG - expects JSON-encoded input!

// DO this instead - read raw FHiCL as string:
std::ifstream file("config.fcl");
if (!file) {
  std::cerr << "Cannot open file\n";
  return;
}
std::string content((std::istreambuf_iterator<char>(file)), {});
FhiclData fhicl(content);
fhicl.fhicl_file_name = "config.fcl";

// DON'T do this - forgetting to set filename metadata:
FhiclData fhicl(loadedContent);
// Later, error messages won't know which file caused the problem

// DO this instead - always set filename when loading from files:
FhiclData fhicl(loadedContent);
fhicl.fhicl_file_name = filepath;
```

### Round-Trip Conversion

FhiclData supports perfect round-trip conversion through JSON:

```cpp
#include "artdaq-database/BasicTypes/data_fhicl.h"
#include "artdaq-database/BasicTypes/data_json.h"
#include <iostream>
#include <cassert>

using namespace artdaq::database::basictypes;

void testRoundTrip() {
  try {
    // Original FHiCL
    FhiclData original("module: { param: value count: 42 }");
    original.fhicl_file_name = "test.fcl";

    // Convert to JSON
    JsonData json = original;

    if (json.empty()) {
      std::cerr << "Error: Conversion to JSON failed\n";
      return;
    }

    // Convert back to FHiCL
    FhiclData restored(json);

    // Content should be equivalent (formatting may differ)
    std::cout << "Original:\n" << original << "\n";
    std::cout << "Restored:\n" << restored << "\n";

  } catch (const std::runtime_error& e) {
    std::cerr << "Round-trip test failed: " << e.what() << "\n";
  }
}
```
