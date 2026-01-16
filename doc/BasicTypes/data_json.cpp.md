# data_json.cpp

**Path:** `artdaq-database/BasicTypes/data_json.cpp`

**Implements:** [data_json.h](./data_json.h.md)

**Purpose:** Implements the JsonData class methods declared in data_json.h, providing basic functionality for JSON data storage and stream I/O operations. The implementation is intentionally minimal, as JsonData serves primarily as a thin string wrapper with conversion capabilities.

## Implementation Overview

This file provides the core JsonData functionality:
- Constructor with move semantics for efficient string handling
- String conversion operators for interoperability
- Empty check method for validation
- Stream I/O operators for file operations

The file does NOT implement `convert_to<T>()` or `convert_from<T>()` template specializations - those are provided in the respective format implementation files (data_fhicl.cpp, data_xml.cpp).

## Key Algorithms

### Move-Based Construction

**Brief:** The constructor uses `std::move()` to efficiently transfer ownership of the input string, avoiding unnecessary copies for potentially large JSON documents.

```cpp
JsonData::JsonData(std::string buffer) : json_buffer{std::move(buffer)} {}
```

**Why move semantics:**
- Large JSON configurations can be megabytes in size
- Move is O(1) vs O(n) for copy
- Caller typically does not need the original string after construction

**Thread Safety:** Safe (constructor creates new object)

### Stream Reading

**Brief:** The `operator>>` uses iterator-based reading to consume the entire stream content in a single operation.

```cpp
data.json_buffer = std::string(std::istreambuf_iterator<char>(is), {});
```

**Why this approach:**
- Reads entire stream in one operation
- Works with any input stream (file, string, network)
- Minimal intermediate allocations
- Empty initializer `{}` provides the end iterator

**Thread Safety:** Unsafe (modifies data object)

## Dependencies

| Include | Purpose |
|---------|---------|
| `data_json.h` | JsonData class declaration |
| `<iostream>` | Stream I/O operations |
| `<utility>` | `std::move` for efficient string handling |

## Functions

### Constructor

```cpp
JsonData::JsonData(std::string buffer) : json_buffer{std::move(buffer)} {}
```

**Brief:** Constructs JsonData by moving the provided string into the internal buffer.

**Parameters:**
- `buffer` - String to move into `json_buffer`

**Preconditions:** None

**Postconditions:**
- `json_buffer` contains the provided string content
- Input `buffer` is in moved-from state

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_alloc` | If memory allocation fails (extremely rare) |

**Thread Safety:** Safe (constructor creates new object)

**Complexity:** O(1) - move operation is constant time

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_json.h"
#include <cassert>
#include <iostream>

using namespace artdaq::database::basictypes;

void demonstrateMoveConstruction() {
  try {
    // Move from temporary - most efficient
    JsonData json1(R"({"key": "value"})");

    // Move from existing string (original is now in moved-from state)
    std::string large = R"({"large": "configuration", "size": 1000})";
    JsonData json2(std::move(large));

    // Note: large is now in a valid but unspecified state
    // It is safe to assign to or destroy, but don't rely on its content
    std::cout << "json2 content length: " << json2.json_buffer.size() << "\n";

  } catch (const std::bad_alloc& e) {
    std::cerr << "Memory allocation failed: " << e.what() << "\n";
  }
}
```

---

### Conversion Operator (const)

```cpp
JsonData::operator std::string const&() const {
    return json_buffer;
}
```

**Brief:** Returns a const reference to the internal buffer for read-only access.

**Returns:** Const reference to `json_buffer`

**Preconditions:** None

**Postconditions:** None (no state change)

**Throws:** None

**Thread Safety:** Safe (returns const reference, no modification)

**Complexity:** O(1)

**Use Cases:**
- Passing JsonData to functions expecting `const std::string&`
- Implicit conversion in stream operations
- Read-only access to JSON content

---

### Conversion Operator (non-const)

```cpp
JsonData::operator std::string&() {
    return json_buffer;
}
```

**Brief:** Returns a mutable reference to the internal buffer, allowing direct modification.

**Returns:** Mutable reference to `json_buffer`

**Preconditions:** None

**Postconditions:** None (caller may modify buffer)

**Throws:** None

**Thread Safety:** Unsafe (allows modification)

**Complexity:** O(1)

**Warning:** Allows direct modification which bypasses any validation. Use with caution.

**Use Cases:**
- Low-level manipulation when performance is critical
- Interfacing with APIs that modify strings in place
- Building JSON incrementally

---

### empty()

```cpp
bool JsonData::empty() const {
    return json_buffer.empty();
}
```

**Brief:** Checks if the JSON buffer is empty.

**Returns:** `true` if `json_buffer.empty()`, `false` otherwise

**Preconditions:** None

**Postconditions:** None (no state change)

**Throws:** None

**Thread Safety:** Safe (const method, reads only)

**Complexity:** O(1)

---

### operator<<

```cpp
std::ostream& operator<<(std::ostream& os, JsonData const& data) {
    os << data.json_buffer;
    return os;
}
```

**Brief:** Writes the JSON content to an output stream.

**Parameters:**
- `os` - Output stream to write to
- `data` - JsonData object to output

**Returns:** Reference to the output stream (for chaining)

**Preconditions:**
- `os` should be in a valid state

**Postconditions:**
- `json_buffer` content written to stream
- Stream position advanced

**Throws:**

| Exception | Condition |
|-----------|-----------|
| Stream exceptions | If stream is configured to throw on errors |

**Thread Safety:** Safe if `data` is not concurrently modified

**Complexity:** O(n) where n is the buffer length

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_json.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace artdaq::database::basictypes;

void writeToVariousStreams() {
  JsonData json(R"({"key": "value"})");

  // Write to console
  std::cout << json << "\n";

  // Write to file with error handling
  std::ofstream file("output.json");
  if (!file) {
    std::cerr << "Error: Cannot open output file\n";
    return;
  }

  file << json;

  if (!file) {
    std::cerr << "Error: Write failed\n";
    return;
  }

  file.close();
  std::cout << "Successfully wrote to output.json\n";

  // Write to string stream
  std::ostringstream ss;
  ss << json;
  std::string copy = ss.str();
  std::cout << "Copied " << copy.size() << " bytes to string\n";
}
```

---

### operator>>

```cpp
std::istream& operator>>(std::istream& is, JsonData& data) {
    data.json_buffer = std::string(std::istreambuf_iterator<char>(is), {});
    return is;
}
```

**Brief:** Reads the entire stream content into the JSON buffer.

**Parameters:**
- `is` - Input stream to read from
- `data` - JsonData object to populate

**Returns:** Reference to the input stream (for chaining)

**Preconditions:**
- `is` should be in a valid state

**Postconditions:**
- `data.json_buffer` contains entire stream content from current position
- Any previous content in `data.json_buffer` is replaced
- Stream position is at EOF

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_alloc` | If memory allocation fails for large content |

**Thread Safety:** Unsafe (modifies `data`)

**Complexity:** O(n) where n is the stream size

**Side Effects:**
- Replaces any existing buffer content
- Consumes entire stream to EOF
- Stream position will be at EOF after read

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_json.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace artdaq::database::basictypes;

bool readFromFile(const std::string& filepath, JsonData& output) {
  // Open file
  std::ifstream file(filepath);
  if (!file) {
    std::cerr << "Error: Cannot open file: " << filepath << "\n";
    return false;
  }

  try {
    // Read entire file content
    file >> output;

    // Check for read errors (note: EOF is expected after reading)
    if (file.bad()) {
      std::cerr << "Error: Stream read failed\n";
      return false;
    }

    // Validate we got some content
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

**Important Notes:**
- Reads **entire stream** content, not token-by-token
- No JSON validation during read
- Stream errors must be checked via stream state after the operation

## Performance Considerations

| Operation | Complexity | Notes |
|-----------|------------|-------|
| Constructor | O(1) | Move semantics - constant time |
| Conversion operators | O(1) | Return references |
| `empty()` | O(1) | String length check |
| `operator<<` | O(n) | n = buffer size |
| `operator>>` | O(n) | n = stream size |

### Memory Usage

- Single `std::string` member
- No parsing overhead (stored as raw text)
- Memory proportional to JSON content size

### Optimization Opportunities

- Pre-reserve buffer capacity if size is known
- Use `operator>>` with memory-mapped files for large documents
- Avoid repeated conversions; cache converted data

## Error Handling Strategy

The implementation has minimal error handling by design:

| Scenario | Behavior |
|----------|----------|
| Empty input to constructor | Creates empty JsonData (no error) |
| Move from empty string | Creates empty JsonData (no error) |
| Stream read failure | Stream state indicates error; `json_buffer` may be partial |
| Stream at EOF | Returns what was read (may be empty) |

**Recommendations for callers:**
```cpp
#include "artdaq-database/BasicTypes/data_json.h"
#include <fstream>
#include <iostream>

using namespace artdaq::database::basictypes;

bool safeLoadJson(const std::string& filepath, JsonData& output) {
  // 1. Open file with error check
  std::ifstream file(filepath);
  if (!file) {
    std::cerr << "Error: Failed to open file: " << filepath << "\n";
    return false;
  }

  try {
    // 2. Read content
    file >> output;

    // 3. Check for stream errors (bad bit indicates severe error)
    if (file.bad()) {
      std::cerr << "Error: Stream read failed\n";
      return false;
    }

    // 4. Check for empty result
    if (output.empty()) {
      std::cerr << "Warning: Empty JSON data loaded\n";
      // May or may not be an error depending on use case
    }

    return true;

  } catch (const std::bad_alloc& e) {
    std::cerr << "Error: Memory allocation failed: " << e.what() << "\n";
    return false;
  }
}
```

## Thread Safety

**Not thread-safe:**

| Access Pattern | Safety |
|----------------|--------|
| Multiple readers (const methods) | Safe |
| Single writer | Safe |
| Concurrent read/write | Unsafe - requires external synchronization |

## Testing Notes

- **Unit tests:** Located in BasicTypes test suite
- **Key test cases:**

```cpp
#include "artdaq-database/BasicTypes/data_json.h"
#include <sstream>
#include <cassert>
#include <iostream>

using namespace artdaq::database::basictypes;

void testJsonData() {
  // 1. Empty JSON
  {
    JsonData empty("");
    assert(empty.empty());
    std::cout << "Test 1 passed: Empty JSON\n";
  }

  // 2. Non-empty JSON
  {
    JsonData valid(R"({"key": "value"})");
    assert(!valid.empty());
    std::cout << "Test 2 passed: Non-empty JSON\n";
  }

  // 3. Stream I/O round-trip
  {
    std::stringstream ss;
    JsonData original(R"({"test": 123})");

    ss << original;

    JsonData read("");
    ss.seekg(0);  // Reset to beginning
    ss >> read;

    assert(std::string(read) == std::string(original));
    std::cout << "Test 3 passed: Stream round-trip\n";
  }

  // 4. Move semantics
  {
    std::string source = R"({"moved": true})";
    size_t original_size = source.size();
    JsonData moved(std::move(source));

    assert(!moved.empty());
    assert(moved.json_buffer.size() == original_size);
    std::cout << "Test 4 passed: Move semantics\n";
  }

  // 5. Conversion operators
  {
    JsonData json(R"({"key": "value"})");

    const std::string& const_ref = json;
    assert(!const_ref.empty());

    std::string& mutable_ref = json;
    mutable_ref = R"({"modified": true})";
    assert(json.json_buffer == R"({"modified": true})");

    std::cout << "Test 5 passed: Conversion operators\n";
  }

  std::cout << "All tests passed!\n";
}
```

## Relationship to Other Components

### Conversion Flow

JsonData is the hub through which format conversions flow:

```cpp
// FHiCL to JSON (implemented in data_fhicl.cpp)
FhiclData fhicl("param: value");
JsonData json("");
json.convert_from(fhicl);

// JSON to XML (implemented in data_xml.cpp)
XmlData xml;
json.convert_to(xml);
```

### Integration Points

- **data_fhicl.cpp**: Provides `convert_to<FhiclData>` and `convert_from<FhiclData>`
- **data_xml.cpp**: Provides `convert_to<XmlData>` and `convert_from<XmlData>`
- **Storage providers**: Read/write JsonData to databases

## Maintenance Notes

### Design Decisions

1. **No validation**: JSON syntax is not validated on construction or I/O. This is intentional for:
   - Performance (avoiding parse overhead when not needed)
   - Flexibility (allowing partial/streaming use cases)
   - Separation of concerns (validation is done by converters)

2. **Move semantics**: Constructor takes by value and moves to enable both move and copy semantics efficiently.

3. **Stream reading**: Uses `istreambuf_iterator` to read entire stream content without parsing.

### Known Limitations

- No JSON validation on construction
- `operator>>` reads entire stream (may be memory-intensive for very large files)
- No partial/streaming read support

## See Also

- [data_json.h](./data_json.h.md) - Public interface
- [data_json_fusion.h](./data_json_fusion.h.md) - Boost.Fusion adaptation
- [data_fhicl.cpp](./data_fhicl.cpp.md) - FHiCL conversion specializations
- [data_xml.cpp](./data_xml.cpp.md) - XML conversion specializations
