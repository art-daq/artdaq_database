# data_json.cpp

## File Overview

**Location**: `/home/user/artdaq-database/artdaq-database/BasicTypes/data_json.cpp`

This file contains the implementation of the `JsonData` class methods declared in `data_json.h`. It provides the basic functionality for JSON data storage and stream I/O operations.

**Purpose**: Implements the core JsonData class functionality, including construction, conversion operators, and stream I/O.

## Dependencies

- `artdaq-database/BasicTypes/data_json.h` - JsonData class declaration
- `<iostream>` - Standard I/O stream operations
- `<utility>` - For `std::move`

### TRACE Configuration

```cpp
#define TRACE_NAME "data_json.cpp"
```

## Implemented Functions/Methods

### Constructor

```cpp
JsonData::JsonData(std::string buffer) : json_buffer{std::move(buffer)} {}
```

**Purpose**: Constructs a JsonData object, moving the provided string into `json_buffer`.

**Parameters**:
- `buffer` - String containing JSON data (moved, not copied)

**Implementation Notes**:
- Uses move semantics (`std::move`) for efficiency
- Avoids unnecessary string copying for large JSON documents
- Member initializer list for direct initialization

**Performance**: O(1) - move operation is constant time

**Usage Example**:
```cpp
std::string json_str = R"({"large": "document"})";
JsonData json(std::move(json_str));  // json_str is now empty
// OR
JsonData json2(R"({"inline": "json"})");  // Temporary string is moved
```

---

### Conversion Operator (const)

```cpp
JsonData::operator std::string const&() const {
    return json_buffer;
}
```

**Purpose**: Provides implicit conversion from JsonData to const string reference.

**Return Value**: Const reference to the internal `json_buffer`

**Use Cases**:
- Passing JsonData to functions expecting `const std::string&`
- Implicit conversion in stream operations
- Read-only access to JSON content

**Example**:
```cpp
void processJson(const std::string& json);

JsonData data(R"({"key": "value"})");
processJson(data);  // Implicit conversion
```

---

### Conversion Operator (non-const)

```cpp
JsonData::operator std::string&() {
    return json_buffer;
}
```

**Purpose**: Provides implicit conversion from JsonData to mutable string reference.

**Return Value**: Reference to the internal `json_buffer`

**Use Cases**:
- Modifying JSON content directly
- Passing to functions that modify strings

**Warning**: Direct modification bypasses any validation logic. Use with caution.

**Example**:
```cpp
JsonData data("{}");
std::string& buffer = data;
buffer = R"({"modified": "content"})";  // Directly modifies json_buffer
```

---

### empty

```cpp
bool JsonData::empty() const {
    return json_buffer.empty();
}
```

**Purpose**: Checks if the JSON buffer contains any data.

**Return Value**: `true` if buffer is empty, `false` otherwise

**Use Cases**:
- Validation before processing
- Checking successful data load
- Guard conditions in conversion functions

**Example**:
```cpp
JsonData json("");
if (json.empty()) {
    TLOG(1) << "Warning: empty JSON data";
    return false;
}
```

## Stream Operators

### operator<<

```cpp
std::ostream& operator<<(std::ostream& os, JsonData const& data) {
    os << data.json_buffer;
    return os;
}
```

**Purpose**: Writes JSON data to an output stream.

**Parameters**:
- `os` - Output stream (file, cout, stringstream, etc.)
- `data` - JsonData object to write

**Return Value**: Reference to the stream for chaining

**Implementation**: Simply outputs the `json_buffer` string to the stream

**Usage Examples**:

```cpp
// Console output
JsonData json(R"({"key": "value"})");
std::cout << json << "\n";

// File output
std::ofstream file("output.json");
file << json;

// String stream
std::ostringstream oss;
oss << json;
std::string result = oss.str();

// Chaining
std::cout << "JSON: " << json << ", length: " << json.json_buffer.length();
```

---

### operator>>

```cpp
std::istream& operator>>(std::istream& is, JsonData& data) {
    data.json_buffer = std::string(std::istreambuf_iterator<char>(is), {});
    return is;
}
```

**Purpose**: Reads JSON data from an input stream into a JsonData object.

**Parameters**:
- `is` - Input stream (file, cin, stringstream, etc.)
- `data` - JsonData object to populate

**Return Value**: Reference to the stream for chaining

**Implementation Details**:
- Uses `std::istreambuf_iterator<char>` to read entire stream
- Reads from current position to end of stream
- Second argument `{}` creates default-constructed end iterator
- Replaces any existing content in `json_buffer`

**Performance**: Reads entire stream into memory at once

**Usage Examples**:

```cpp
// Read from file
std::ifstream file("config.json");
JsonData json("");
file >> json;

// Read from string stream
std::istringstream iss(R"({"config": "value"})");
JsonData json2("");
iss >> json2;

// Read from stdin
JsonData json3("");
std::cin >> json3;
```

**Important Notes**:
- Reads **entire stream** content, not line-by-line
- Existing content is completely replaced
- No JSON validation is performed during read
- Stream position will be at EOF after successful read

## Usage Context

### Typical Workflow

```cpp
// 1. Create from string
JsonData json1(R"({"create": "from string"})");

// 2. Read from file
std::ifstream file("config.json");
JsonData json2("");
file >> json2;

// 3. Check if empty
if (!json2.empty()) {
    // 4. Write to output
    std::cout << json2;

    // 5. Get as string
    std::string str = json2;  // Implicit conversion
}
```

### Integration with Other BasicTypes

```cpp
// JSON as intermediate format
FhiclData fhicl("param: value");

// FHICL → JSON
JsonData json("");
json.convert_from(fhicl);

// Process JSON
std::cout << "JSON representation: " << json << "\n";

// JSON → XML
XmlData xml(json);
```

## Error Handling

### Current Implementation

The current implementation has **minimal error handling**:
- No JSON syntax validation
- No exception throwing
- No error status reporting

### Implications

1. **Invalid JSON**: Can be stored and passed around
2. **I/O Failures**: Stream state must be checked externally
3. **Empty Data**: Use `empty()` method to check

### Recommended Practices

```cpp
// Check stream state
std::ifstream file("config.json");
if (!file) {
    TLOG(1) << "Failed to open file";
    return;
}

JsonData json("");
file >> json;

if (!file.eof() && file.fail()) {
    TLOG(1) << "Failed to read JSON";
    return;
}

if (json.empty()) {
    TLOG(1) << "Empty JSON data";
    return;
}

// Validate JSON syntax here if needed
// (use external JSON parser)
```

## Performance Considerations

### String Operations

1. **Constructor**: Uses move semantics - O(1)
2. **Conversion operators**: Return references - O(1)
3. **empty()**: Checks string length - O(1)
4. **Stream read**: Reads entire stream - O(n) where n is stream size

### Memory Usage

- Single `std::string` member - size of JSON content
- No parsing overhead (stored as raw text)
- Large JSON documents consume memory proportional to size

### Optimization Tips

```cpp
// Good: Move when possible
JsonData json(std::move(large_string));

// Good: Reserve space if building JSON
std::string buffer;
buffer.reserve(estimated_size);
// ... build JSON ...
JsonData json(std::move(buffer));

// Avoid: Unnecessary copies
std::string str = get_json();
JsonData json1(str);           // Copy
JsonData json2(get_json());    // Move (if get_json returns by value)
```

## Design Patterns

### Value Type

JsonData is designed as a **value type**:
- Copyable and movable
- No virtual functions
- Simple data member
- Marked `final` to prevent inheritance

### Transparent Wrapper

Provides **transparent access** to underlying string:
- Implicit conversions
- Direct member access
- Minimal abstraction overhead

## Notes for Developers

### Why So Simple?

The implementation is intentionally minimal:
- **JSON parsing** is handled by external libraries (not BasicTypes)
- **Validation** is done at conversion time
- **Storage** is the primary concern here

### Extension Points

To add functionality:
1. **Validation**: Add `validate()` method that uses JSON parser
2. **Formatting**: Add `pretty_print()` method
3. **Querying**: Add methods to extract values (or use external library)

### Thread Safety

The class is **not thread-safe**:
- Multiple readers: Safe (const methods)
- Single writer: Safe
- Concurrent read/write: Unsafe (requires external synchronization)

## Testing Recommendations

Test cases should cover:

```cpp
// 1. Empty JSON
JsonData empty("");
assert(empty.empty());

// 2. Valid JSON
JsonData valid(R"({"key": "value"})");
assert(!valid.empty());

// 3. Stream I/O
std::stringstream ss;
ss << valid;
JsonData read("");
ss >> read;
assert(std::string(read) == std::string(valid));

// 4. Move semantics
std::string str = "test";
JsonData moved(std::move(str));
assert(str.empty());  // Original string is now empty

// 5. Conversions
std::string as_string = moved;
assert(as_string == "test");
```

## Related Documentation

- `data_json.h.md` - Class declaration and interface
- `data_json_fusion.h.md` - Boost.Fusion adaptation
- `data_fhicl.cpp.md` - FHICL conversion implementations
- `data_xml.cpp.md` - XML conversion implementations
