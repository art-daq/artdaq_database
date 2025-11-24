# data_fhicl.cpp

## File Overview

**Location**: `/home/user/artdaq-database/artdaq-database/BasicTypes/data_fhicl.cpp`

This file implements the `FhiclData` class methods, including the critical conversion logic between FHICL and JSON formats. The implementation handles Base64 encoding/decoding and integrates with the artdaq FHICL-to-JSON conversion library.

**Purpose**: Implements FhiclData class functionality and FHICL ↔ JSON conversion logic.

## Dependencies

### BasicTypes Dependencies
- `artdaq-database/BasicTypes/data_json.h` - JsonData class
- `artdaq-database/BasicTypes/data_json_fusion.h` - Fusion adaptation for JsonData
- `artdaq-database/BasicTypes/data_fhicl.h` - FhiclData class declaration
- `artdaq-database/BasicTypes/data_fhicl_fusion.h` - Fusion adaptation for FhiclData
- `artdaq-database/BasicTypes/base64.h` - Base64 encoding/decoding

### DataFormats Dependencies
- `artdaq-database/DataFormats/Fhicl/fhicl_common.h` - FHICL common utilities
- `artdaq-database/DataFormats/Fhicl/fhicljsondb.h` - FHICL↔JSON conversion functions
- `artdaq-database/DataFormats/Fhicl/fhiclcpplib_includes.h` - FHICL library includes
- `artdaq-database/DataFormats/shared_literals.h` - Shared string literals

### Standard Library
- `<utility>` - For `std::move`

### TRACE Configuration

```cpp
#define TRACE_NAME "data_fhicl.cpp"
```

## Regular Expressions

### Namespace: regex

```cpp
namespace regex {
constexpr auto parse_base64data = R"lit([\s\S]*"base64"\s*:\s*"(\S*?)")lit";
}
```

**Purpose**: Regular expression pattern for extracting Base64-encoded data from JSON.

**Pattern Breakdown**:
- `[\s\S]*` - Match any characters (including newlines)
- `"base64"` - Match literal "base64" key
- `\s*:\s*` - Match colon with optional whitespace
- `"(\S*?)"` - Capture group for the Base64 value (non-greedy)

**Example Match**:
```json
{
    "some": "data",
    "base64": "SGVsbG8gV29ybGQ=",
    "other": "data"
}
```
Captures: `SGVsbG8gV29ybGQ=`

## Template Specializations

### JsonData::convert_to<FhiclData>

```cpp
template <>
bool JsonData::convert_to(FhiclData& fhicl) const {
    using artdaq::database::fhicljson::json_to_fhicl;
    return json_to_fhicl(json_buffer, fhicl.fhicl_buffer, fhicl.fhicl_file_name);
}
```

**Purpose**: Converts JSON representation to FHICL format.

**Parameters**:
- `fhicl` - FhiclData object to populate (output parameter)

**Return Value**: `true` if conversion succeeded, `false` otherwise

**Implementation**:
- Uses `artdaq::database::fhicljson::json_to_fhicl` function
- Converts `json_buffer` (JSON) → `fhicl_buffer` (FHICL)
- Also populates `fhicl_file_name` if provided in JSON

**Usage**:
```cpp
JsonData json(R"({"parameter": "value"})");
FhiclData fhicl;
if (json.convert_to(fhicl)) {
    std::cout << "Converted: " << fhicl.fhicl_buffer << "\n";
}
```

---

### JsonData::convert_from<FhiclData>

```cpp
template <>
bool JsonData::convert_from(FhiclData const& fhicl) {
    using artdaq::database::fhicljson::fhicl_to_json;
    return fhicl_to_json(fhicl.fhicl_buffer, fhicl.fhicl_file_name, json_buffer);
}
```

**Purpose**: Converts FHICL format to JSON representation.

**Parameters**:
- `fhicl` - FhiclData object to convert from (input parameter)

**Return Value**: `true` if conversion succeeded, `false` otherwise

**Implementation**:
- Uses `artdaq::database::fhicljson::fhicl_to_json` function
- Converts `fhicl_buffer` (FHICL) → `json_buffer` (JSON)
- Includes `fhicl_file_name` metadata in JSON

**Usage**:
```cpp
FhiclData fhicl("parameter: value");
JsonData json("");
if (json.convert_from(fhicl)) {
    std::cout << "JSON: " << json.json_buffer << "\n";
}
```

## FhiclData Methods

### Constructor: FhiclData(std::string)

```cpp
FhiclData::FhiclData(std::string buffer) : fhicl_buffer{std::move(buffer)} {}
```

**Purpose**: Constructs FhiclData from a FHICL string.

**Parameters**:
- `buffer` - FHICL string (moved into member)

**Implementation**: Uses move semantics for efficiency

**Example**:
```cpp
std::string config = "daq_param: 1000";
FhiclData fhicl(std::move(config));
// config is now empty, fhicl.fhicl_buffer contains the data
```

---

### String Conversion Operator

```cpp
FhiclData::operator std::string const&() const {
    return fhicl_buffer;
}
```

**Purpose**: Provides implicit conversion to const string reference.

**Return Value**: Reference to `fhicl_buffer`

**Example**:
```cpp
FhiclData fhicl("param: value");
std::string str = fhicl;  // Implicit conversion
```

---

### Constructor: FhiclData(JsonData const&)

```cpp
FhiclData::FhiclData(JsonData const& document) {
    // ... detailed implementation below
}
```

**Purpose**: Constructs FhiclData by converting from JSON-encoded format.

**Process**:

1. **Validation**:
   ```cpp
   confirm(!document.empty());
   ```
   Ensures the JSON document is not empty.

2. **Logging**:
   ```cpp
   TLOG(11) << "FHICL document=" << document;
   ```

3. **Regex Search**:
   ```cpp
   auto ex = std::regex(regex::parse_base64data);
   auto results = std::smatch();

   if (!std::regex_search(document.json_buffer, results, ex)) {
       throw std::runtime_error("JSON to FHICL convertion error, regex_search()==false...");
   }
   ```
   Extracts Base64-encoded data from JSON.

4. **Validation of Results**:
   ```cpp
   if (results.size() != 1) {
       throw std::runtime_error("JSON to FHICL convertion error, regex_search().size()!=1...");
   }
   ```
   Ensures exactly one match was found.

5. **Base64 Decoding**:
   ```cpp
   auto base64 = std::string(results[0]);
   TLOG(14) << "FHICL base64=" << base64;

   auto json = base64_decode(base64);
   TLOG(15) << "FHICL  json=" << json;
   ```
   Decodes the Base64 string to get intermediate JSON.

6. **JSON to FHICL Conversion**:
   ```cpp
   JsonData(json).convert_to(*this);
   TLOG(16) << "FHICL fhicl=" << fhicl_buffer;
   ```
   Converts the intermediate JSON to FHICL format.

**Throws**: `std::runtime_error` if:
- Document is empty
- Regex search fails
- Multiple Base64 fields found
- Conversion fails

**Complete Data Flow**:
```
JSON with Base64
    ↓ regex_search
Base64 string
    ↓ base64_decode
Intermediate JSON
    ↓ convert_to
FHICL buffer
```

---

### JSON Conversion Operator

```cpp
FhiclData::operator JsonData() const {
    // ... detailed implementation below
}
```

**Purpose**: Converts FhiclData to JSON format with Base64 encoding.

**Process**:

1. **Logging**:
   ```cpp
   TLOG(17) << "FHICL fhicl=" << fhicl_buffer;
   ```

2. **FHICL to JSON Conversion**:
   ```cpp
   auto json = JsonData("");
   if (!json.convert_from(*this)) {
       throw std::runtime_error("FHICL to JSON convertion error...");
   }
   TLOG(19) << "FHICL  json=" << json;
   ```

3. **Collection Name Creation**:
   ```cpp
   auto collection = std::string("FhiclData_") + type_version();
   // Result: "FhiclData_V100"
   ```

4. **Base64 Encoding**:
   ```cpp
   auto base64 = base64_encode(fhicl_buffer);
   TLOG(20) << "FHICL base64=" << base64;
   ```

5. **Document Creation**:
   ```cpp
   std::ostringstream os;
   os << json;
   TLOG(21) << "FHICL document=" << os.str();

   return {os.str()};
   ```

**Throws**: `std::runtime_error` if conversion from FHICL to JSON fails

**Complete Data Flow**:
```
FHICL buffer
    ↓ convert_from
Intermediate JSON
    ↓ base64_encode (of original FHICL)
Base64 string
    ↓ embed in JSON
JSON with Base64
```

## Stream Operators

### operator>>

```cpp
std::istream& operator>>(std::istream& is, FhiclData& data) {
    auto str = std::string(std::istreambuf_iterator<char>(is), {});
    auto json = JsonData(str);
    data = FhiclData(json);
    return is;
}
```

**Purpose**: Reads FHICL data from input stream (in JSON-encoded format).

**Process**:
1. Read entire stream into string
2. Create JsonData from string
3. Convert JsonData to FhiclData
4. Assign to output parameter

**Important**: Input must be JSON-encoded FHICL, not raw FHICL!

---

### operator<<

```cpp
std::ostream& operator<<(std::ostream& os, FhiclData const& data) {
    os << data.fhicl_buffer;
    return os;
}
```

**Purpose**: Writes raw FHICL string to output stream.

**Output**: Raw FHICL text (not JSON-encoded)

**Usage**:
```cpp
FhiclData fhicl("param: value");
std::cout << fhicl;  // Outputs: param: value
```

## Usage Context

### Conversion Flow Examples

#### Example 1: FHICL to Database

```cpp
// 1. Start with FHICL configuration
FhiclData fhicl(R"(
    daq_settings: {
        buffer_size: 8192
    }
)");

// 2. Convert to JSON (with Base64 encoding)
JsonData json = fhicl;

// 3. JSON now contains:
//    - Intermediate JSON representation
//    - Base64-encoded original FHICL
//    Ready for database storage
```

#### Example 2: Database to FHICL

```cpp
// 1. Retrieve JSON from database
JsonData json = get_from_database();

// 2. Convert to FHICL
FhiclData fhicl(json);
// Internally:
//   - Extracts Base64 from JSON
//   - Decodes Base64
//   - Converts to FHICL

// 3. Use the FHICL configuration
std::cout << fhicl;
```

## Error Handling

### Exception Safety

All conversions can throw `std::runtime_error`:

```cpp
try {
    JsonData json = get_data();
    FhiclData fhicl(json);  // May throw
    // Use fhicl...
} catch (const std::runtime_error& e) {
    TLOG(1) << "Conversion failed: " << e.what();
}
```

### Common Error Scenarios

1. **Empty JSON**: `confirm(!document.empty())` fails
2. **Missing Base64**: Regex doesn't find "base64" field
3. **Multiple Base64 fields**: Regex finds multiple matches
4. **Invalid Base64**: Decoding fails (malformed data)
5. **FHICL Parse Error**: Invalid FHICL syntax

## TRACE Logging Levels

The implementation uses different TRACE levels:

- **TLOG(11-16)**: JSON → FHICL conversion steps
- **TLOG(17-21)**: FHICL → JSON conversion steps

**Debug Example**:
```
TLOG(11): "FHICL document={...}"
TLOG(14): "FHICL base64=SGVsbG8="
TLOG(15): "FHICL  json={...}"
TLOG(16): "FHICL fhicl=parameter: value"
```

## Performance Considerations

### Multiple Conversions

The FHICL ↔ JSON conversion involves multiple steps:
1. String operations (regex, base64)
2. FHICL parsing
3. JSON generation
4. Memory allocations

**Optimization**: Cache converted data if used multiple times.

### Memory Usage

- Stores both FHICL buffer and (temporarily) JSON representation
- Base64 encoding increases size by ~33%
- Intermediate strings are created during conversion

## Design Patterns

### Two-Stage Conversion

FHICL ↔ JSON conversion uses a two-stage approach:

**Stage 1**: FHICL ↔ Intermediate JSON
- Uses external library (`fhicljsondb`)
- Handles FHICL parsing/generation

**Stage 2**: Intermediate JSON ↔ Base64-encoded JSON
- Uses Base64 encoding
- Embeds FHICL in JSON for database storage

**Why Two Stages?**
- Preserves both FHICL syntax and structured JSON
- Allows database storage of FHICL as JSON
- Maintains data integrity

## Notes for Developers

### Why Base64?

Base64 encoding ensures:
- FHICL special characters are preserved
- No JSON escaping issues
- Binary data (if present) is handled correctly
- Data integrity across conversions

### Regex Pattern

The regex `parse_base64data` expects JSON with a "base64" key:

```json
{
    "some_field": "value",
    "base64": "encoded_data_here"
}
```

If your JSON structure differs, update the regex pattern.

### Collection Naming

Collection names include version:
```cpp
"FhiclData_V100"
```

This allows schema evolution in the database.

### Thread Safety

The conversion functions are **not thread-safe** when operating on the same object.

## Testing Recommendations

```cpp
// Test round-trip conversion
FhiclData original("param: value");
JsonData json = original;
FhiclData restored(json);
assert(std::string(original) == std::string(restored));

// Test error handling
try {
    JsonData empty("");
    FhiclData fhicl(empty);  // Should throw
    assert(false);
} catch (const std::runtime_error&) {
    // Expected
}

// Test with complex FHICL
FhiclData complex(R"(
    module: {
        nested: {
            array: [1, 2, 3]
        }
    }
)");
// Verify conversion preserves structure
```

## Related Documentation

- `data_fhicl.h.md` - FhiclData class interface
- `data_fhicl_fusion.h.md` - Boost.Fusion adaptation
- `base64.cpp.md` - Base64 encoding/decoding details
- `data_json.cpp.md` - JSON data implementation
