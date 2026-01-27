# data_json.h

**Path:** `artdaq-database/BasicTypes/data_json.h`

**Purpose:** Defines the JsonData structure, which is the central pivot format for all configuration data in artdaq-database. JsonData serves as both a standalone JSON container and the intermediary format for converting between FHiCL and XML, making it essential for all database storage operations.


## Key Concepts

### JSON as Pivot Format

JsonData is the central data type through which all conversions flow:

```
FHiCL <--> JSON <--> XML
              |
          Database
     (MongoDB/FileSystemDB)
```

All format conversions go through JSON, and all database storage uses JSON. This design simplifies the conversion matrix and ensures consistent database representation.

### Template Method Pattern

The `convert_to<T>()` and `convert_from<T>()` methods are templates with explicit specializations provided in format-specific implementation files:

- `JsonData::convert_to<FhiclData>()` - Implemented in `data_fhicl.cpp`
- `JsonData::convert_from<FhiclData>()` - Implemented in `data_fhicl.cpp`
- `JsonData::convert_to<XmlData>()` - Implemented in `data_xml.cpp`
- `JsonData::convert_from<XmlData>()` - Implemented in `data_xml.cpp`

### Simple Value Type

JsonData is designed as a simple value type:
- Just a string wrapper with conversion capabilities
- Does NOT parse or validate JSON syntax
- Stores JSON as raw text for maximum flexibility
- Copyable, movable, marked `final`

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Multiple readers are safe (const methods); concurrent read/write requires external synchronization
- **Locking:** No internal locking; callers must synchronize shared access

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/BasicTypes/common.h` | TRACE logging framework and Boost.Core utilities |

## Classes/Structures

### `JsonData`

**Brief:** A value type that wraps JSON-formatted string data with template conversion methods for transforming to/from other configuration formats.

```cpp
struct JsonData final {
    JsonData(std::string);

    template <typename TYPE>
    bool convert_to(TYPE&) const;

    template <typename TYPE>
    bool convert_from(TYPE const&);

    operator std::string const&() const;
    operator std::string&();

    bool empty() const;

    static constexpr auto type_version() { return "V1.0.0"; }

    std::string json_buffer;
};
```

**Thread Safety:** Not thread-safe for concurrent modification. Multiple concurrent readers (const methods only) are safe.

#### Member Variables

##### `json_buffer`

**Brief:** Holds the JSON-formatted string data. Public member allows direct access when needed.

**Type:** `std::string`

**Note:** This is a public member, allowing direct modification. Use `convert_to/from` methods when validation is needed; use direct access for performance-critical code.

#### Constructor

##### `JsonData(std::string buffer)`

**Brief:** Constructs a JsonData object from a JSON string, using move semantics for efficiency.

**Parameters:**
- `buffer` - A string containing JSON data. The string is moved into `json_buffer`. May be empty; no JSON syntax validation is performed.

**Preconditions:**
- None (accepts any string including empty)

**Postconditions:**
- `json_buffer` contains the provided string (moved)
- Input `buffer` is in valid but unspecified state (moved-from)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_alloc` | If memory allocation fails (extremely rare) |

**Thread Safety:** Safe (constructor creates new object)

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_json.h"
#include <iostream>

using namespace artdaq::database::basictypes;

void createJsonData() {
  try {
    // Create from JSON string
    JsonData json(R"({"parameter": "value", "count": 42})");

    // Check if data was created successfully
    if (json.empty()) {
      std::cerr << "Warning: Created empty JsonData\n";
    } else {
      std::cout << "Created JsonData with " << json.json_buffer.size() << " bytes\n";
    }

    // Create empty (will need population later)
    JsonData empty_json("");
    if (empty_json.empty()) {
      std::cout << "Empty JsonData created as expected\n";
    }

    // Move existing string for efficiency
    std::string large_json = R"({"large": "data"})";
    JsonData moved(std::move(large_json));
    // Note: large_json is now in moved-from state

  } catch (const std::bad_alloc& e) {
    std::cerr << "Memory allocation failed: " << e.what() << "\n";
  }
}
```

#### Template Methods

##### `convert_to<TYPE>(TYPE& target) const -> bool`

**Brief:** Converts the JSON data to another format type (FhiclData or XmlData). Template specializations are provided in the respective format implementation files.

**Parameters:**
- `target` - Object to receive the converted data (output parameter)

**Preconditions:**
- For FhiclData: JSON must contain valid structure for FHiCL conversion
- For XmlData: JSON must contain valid structure for XML conversion
- `json_buffer` should contain valid JSON (invalid JSON will cause conversion failure)

**Returns:** `true` if conversion succeeded, `false` otherwise.

**Postconditions:**
- On success: `target` contains the converted data
- On failure: `target` may be in an undefined state; do not use

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Returns false on failure instead of throwing |

**Thread Safety:** Safe (reads only, const method)

**Side Effects:** None (const method)

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_json.h"
#include "artdaq-database/BasicTypes/data_fhicl.h"
#include <iostream>

using namespace artdaq::database::basictypes;

void convertToFhicl() {
  // Start with JSON data
  JsonData json(R"({"parameter": "value", "count": 42})");

  // Validate JSON is not empty before conversion
  if (json.empty()) {
    std::cerr << "Error: Cannot convert empty JSON\n";
    return;
  }

  FhiclData fhicl;

  // Attempt conversion
  if (json.convert_to(fhicl)) {
    std::cout << "Conversion successful\n";
    std::cout << "FHiCL content:\n" << fhicl << "\n";
  } else {
    std::cerr << "Conversion failed - JSON may be malformed or incompatible\n";
    // Do NOT use fhicl here - it may be in an undefined state
  }
}
```

---

##### `convert_from<TYPE>(TYPE const& source) -> bool`

**Brief:** Converts data from another format type into this JsonData object. Template specializations are provided in the respective format implementation files.

**Parameters:**
- `source` - Object to convert from (input parameter)

**Preconditions:**
- `source` must contain valid format data

**Returns:** `true` if conversion succeeded, `false` otherwise.

**Postconditions:**
- On success: `json_buffer` contains the converted JSON
- On failure: `json_buffer` may be in an undefined state

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Returns false on failure instead of throwing |

**Thread Safety:** Unsafe (modifies json_buffer)

**Side Effects:**
- Modifies `json_buffer` (replaces existing content)

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_json.h"
#include "artdaq-database/BasicTypes/data_fhicl.h"
#include <iostream>

using namespace artdaq::database::basictypes;

void convertFromFhicl() {
  // Start with FHiCL data
  FhiclData fhicl("parameter: value\ncount: 42");
  fhicl.fhicl_file_name = "test_config.fcl";

  // Create empty JsonData to receive conversion
  JsonData json("");

  // Attempt conversion
  if (json.convert_from(fhicl)) {
    std::cout << "Conversion successful\n";
    std::cout << "JSON content:\n" << json << "\n";

    // Validate result
    if (json.empty()) {
      std::cerr << "Warning: Conversion produced empty JSON\n";
    }
  } else {
    std::cerr << "Conversion failed - FHiCL may be malformed\n";
    // Do NOT use json here - it may be in an undefined state
  }
}
```

#### Conversion Operators

##### `operator std::string const&() const`

**Brief:** Provides implicit conversion to const string reference for read-only access to the JSON content.

**Returns:** Const reference to `json_buffer`.

**Preconditions:** None

**Postconditions:** None (no state change)

**Throws:** None

**Thread Safety:** Safe (returns const reference to internal state)

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_json.h"
#include <iostream>

using namespace artdaq::database::basictypes;

void processJson(const std::string& json_str) {
  std::cout << "Processing JSON of size: " << json_str.size() << "\n";
}

void useConstConversion() {
  JsonData json(R"({"key": "value"})");

  // Implicit conversion to const string&
  const std::string& ref = json;
  std::cout << "JSON content: " << ref << "\n";

  // Works with functions expecting const string&
  processJson(json);  // Implicit conversion
}
```

---

##### `operator std::string&()`

**Brief:** Provides implicit conversion to mutable string reference, allowing direct modification of the JSON buffer.

**Returns:** Mutable reference to `json_buffer`.

**Preconditions:** None

**Postconditions:** None (caller may modify buffer)

**Throws:** None

**Thread Safety:** Unsafe (allows modification)

**Warning:** Direct modification bypasses any validation. Use with caution.

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_json.h"
#include <iostream>

using namespace artdaq::database::basictypes;

void modifyJson(std::string& json_str) {
  // Append to existing JSON (demonstration only - not recommended)
  json_str = R"({"modified": true})";
}

void useMutableConversion() {
  JsonData json(R"({"key": "value"})");

  // Get mutable reference - use with caution!
  std::string& buffer = json;
  buffer = R"({"new_key": "new_value"})";  // Direct modification

  // Or use with functions that modify
  modifyJson(json);  // Implicit conversion

  std::cout << "Modified JSON: " << json << "\n";
}
```

#### Methods

##### `empty() const -> bool`

**Brief:** Checks if the JSON buffer is empty.

**Returns:** `true` if `json_buffer.empty()`, `false` otherwise.

**Preconditions:** None

**Postconditions:** None (no state change)

**Throws:** None

**Thread Safety:** Safe (const method, reads only)

**Complexity:** O(1)

**Use Cases:**
- Validation before processing
- Checking if data was successfully loaded
- Guard conditions in conversion functions

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_json.h"
#include <iostream>

using namespace artdaq::database::basictypes;

void processConfiguration(const JsonData& json) {
  // Always check for empty before processing
  if (json.empty()) {
    std::cerr << "Error: Configuration not found or empty\n";
    return;
  }

  // Safe to process
  std::cout << "Processing configuration of size: "
            << std::string(json).size() << " bytes\n";
}
```

---

##### `type_version() -> const char*` (static constexpr)

**Brief:** Returns a version identifier string used for database schema compatibility and collection naming.

**Returns:** `"V1.0.0"` - indicates version 1.0.0 of the JsonData schema.

**Preconditions:** None

**Postconditions:** None

**Throws:** None

**Thread Safety:** Safe (static constexpr, no state)

**Note:** JsonData uses a different version format ("V1.0.0") than FhiclData/XmlData ("V100"). This is intentional for distinguishing schema versions.

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_json.h"
#include <iostream>
#include <string>

using namespace artdaq::database::basictypes;

void showSchemaVersion() {
  // Get schema version at compile time
  constexpr auto version = JsonData::type_version();
  std::cout << "JsonData schema version: " << version << "\n";

  // Use for collection naming
  std::string collection = std::string("configurations_") + JsonData::type_version();
  std::cout << "Collection name: " << collection << "\n";
}
```

## Functions

### `operator<<(std::ostream& os, JsonData const& data) -> std::ostream&`

**Brief:** Writes the JSON data to an output stream.

**Parameters:**
- `os` - Output stream to write to
- `data` - JsonData object to write

**Preconditions:**
- `os` must be in a valid state

**Returns:** Reference to the output stream (for chaining).

**Postconditions:**
- `json_buffer` content written to stream
- Stream position advanced

**Throws:**

| Exception | Condition |
|-----------|-----------|
| Stream exceptions | If stream is configured to throw on errors |

**Thread Safety:** Safe if `data` is not concurrently modified

**Side Effects:**
- Writes `json_buffer` content to stream
- Modifies stream position

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_json.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace artdaq::database::basictypes;

void writeJsonToOutput() {
  JsonData json(R"({"key": "value", "count": 42})");

  // Write to console
  std::cout << "JSON content: " << json << "\n";

  // Write to file with error handling
  std::ofstream out("config.json");
  if (!out) {
    std::cerr << "Error: Cannot open file for writing\n";
    return;
  }
  out << json;
  if (!out) {
    std::cerr << "Error: Write operation failed\n";
    return;
  }
  out.close();

  // Write to string stream
  std::ostringstream ss;
  ss << json;
  std::string copy = ss.str();
  std::cout << "Copied to string of size: " << copy.size() << "\n";
}
```

---

### `operator>>(std::istream& is, JsonData& data) -> std::istream&`

**Brief:** Reads the entire stream content into the JSON buffer.

**Parameters:**
- `is` - Input stream to read from
- `data` - JsonData object to populate

**Preconditions:**
- `is` should be in a valid state
- Stream should contain JSON data

**Returns:** Reference to the input stream (for chaining).

**Postconditions:**
- `data.json_buffer` contains entire stream content from current position
- Any previous content in `data.json_buffer` is replaced
- Stream position is at EOF

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_alloc` | If memory allocation fails for large content |

**Thread Safety:** Unsafe (modifies `data`)

**Side Effects:**
- Reads entire stream from current position to EOF
- Replaces any existing content in `json_buffer`
- Stream position will be at EOF after read

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_json.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace artdaq::database::basictypes;

bool loadJsonFromFile(const std::string& filepath, JsonData& output) {
  std::ifstream file(filepath);
  if (!file) {
    std::cerr << "Error: Cannot open file: " << filepath << "\n";
    return false;
  }

  try {
    file >> output;  // Reads entire file

    // Check for read errors (not EOF, which is expected)
    if (file.bad()) {
      std::cerr << "Error: Read operation failed\n";
      return false;
    }

    // Validate result
    if (output.empty()) {
      std::cerr << "Warning: File was empty\n";
    }

    return true;

  } catch (const std::bad_alloc& e) {
    std::cerr << "Error: Memory allocation failed: " << e.what() << "\n";
    return false;
  }
}

void readFromStringStream() {
  std::istringstream ss(R"({"key": "value"})");
  JsonData json("");

  ss >> json;

  if (!json.empty()) {
    std::cout << "Read JSON: " << json << "\n";
  }
}
```

---

### `TraceStreamer::operator<<(const JsonData& r)` (template specialization)

**Brief:** Enables JsonData objects to be used directly in TRACE logging statements for debugging.

**Parameters:**
- `r` - JsonData object to log

**Returns:** Reference to TraceStreamer (for chaining).

**Thread Safety:** Safe (TRACE logging is thread-safe)

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_json.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif
#define TRACE_NAME "my_module.cpp"

using namespace artdaq::database::basictypes;

void debugWithTrace() {
  JsonData json(R"({"debug": "data"})");

  // Log JSON content directly
  TLOG(10) << "Processing configuration: " << json;
  TLOG(11) << "Configuration size: " << std::string(json).size() << " bytes";
}
```

## Relationship to Other Components

### Central Role in BasicTypes

```
+-----------+           +-----------+           +---------+
| FhiclData |<--------->|  JsonData |<--------->|  XmlData |
+-----------+   convert +-----+-----+   convert +---------+
                              |
                              v
                         Database
                    (MongoDB/FileSystemDB)
```

### Files Using JsonData

- **data_fhicl.cpp** - Implements `convert_to<FhiclData>` and `convert_from<FhiclData>`
- **data_xml.cpp** - Implements `convert_to<XmlData>` and `convert_from<XmlData>`
- **data_json_fusion.h** - Boost.Fusion adaptation for generic programming
- **ConfigurationDB module** - Uses JsonData for all storage operations
- **StorageProviders** - Store and retrieve JsonData to/from databases

## See Also

- [data_json.cpp](./data_json.cpp.md) - Implementation details
- [data_json_fusion.h](./data_json_fusion.h.md) - Boost.Fusion adaptation
- [data_fhicl.h](./data_fhicl.h.md) - FHiCL format wrapper
- [data_xml.h](./data_xml.h.md) - XML format wrapper
- [basictypes.h](./basictypes.h.md) - Umbrella header

## Notes for Developers

### No JSON Validation

The class does NOT validate JSON syntax:
- Invalid JSON can be stored and passed around
- Validation occurs during conversion or by external parsers
- This is intentional for performance and flexibility

```cpp
// This compiles and runs - no validation!
JsonData invalid("this is { not valid } json");

// Validation happens when you try to use it
FhiclData fhicl;
if (!invalid.convert_to(fhicl)) {
  // Conversion fails due to invalid JSON
  std::cerr << "Conversion failed - check JSON syntax\n";
}
```

### Common Pitfalls

- **Pitfall 1:** Assuming JSON is validated on construction. It is not - validate externally if needed.
- **Pitfall 2:** Using `operator std::string&()` without understanding it bypasses validation. Direct modification can corrupt the JSON structure.
- **Pitfall 3:** Forgetting to check `empty()` before processing data from database.
- **Pitfall 4:** Not handling the moved-from state after moving a string into the constructor.

### Anti-patterns

```cpp
// DON'T do this - no empty check:
JsonData json = database.get("config");
FhiclData fhicl(json);  // May fail if json is empty!

// DO this instead:
JsonData json = database.get("config");
if (json.empty()) {
  std::cerr << "Configuration not found\n";
  return;
}
try {
  FhiclData fhicl(json);
  // Use fhicl...
} catch (const std::runtime_error& e) {
  std::cerr << "Conversion error: " << e.what() << "\n";
}

// DON'T do this - assuming valid JSON:
JsonData json(userInput);  // User input may be invalid!
process(json);

// DO this instead - validate or handle conversion failure:
JsonData json(userInput);
FhiclData fhicl;
if (!json.convert_to(fhicl)) {
  std::cerr << "Invalid JSON input\n";
  return;
}
process(fhicl);

// DON'T do this - ignoring stream errors:
std::ifstream file("config.json");
JsonData json("");
file >> json;
// Using json without checking if file was opened or read succeeded

// DO this instead:
std::ifstream file("config.json");
if (!file) {
  std::cerr << "Cannot open file\n";
  return;
}
JsonData json("");
file >> json;
if (file.bad()) {
  std::cerr << "Read error\n";
  return;
}
if (json.empty()) {
  std::cerr << "File was empty\n";
  return;
}
```
