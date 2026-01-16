# data_fhicl.cpp

**Path:** `artdaq-database/BasicTypes/data_fhicl.cpp`

**Implements:** [data_fhicl.h](./data_fhicl.h.md)

**Purpose:** Implements the FhiclData class methods and the critical conversion logic between FHiCL and JSON formats. This file provides template specializations for JsonData conversion methods and handles Base64 encoding/decoding for safe embedding of FHiCL content in JSON documents.

## Implementation Overview

This file implements a two-stage conversion process between FHiCL and JSON:

1. **FHiCL to JSON:** Parse FHiCL using the fhicljson library, Base64-encode the original FHiCL, embed both in the output JSON
2. **JSON to FHiCL:** Extract Base64 from JSON using regex, decode to intermediate JSON, convert to FHiCL using fhicljson library

The two-stage approach with Base64 encoding ensures perfect round-trip fidelity - the original FHiCL bytes are preserved exactly.

## Key Algorithms

### FHiCL to JSON Conversion

**Brief:** Converts FhiclData to JsonData by parsing the FHiCL, Base64-encoding the original content, and embedding both in the output JSON.

**Steps:**
1. Call `fhicl_to_json()` to convert FHiCL buffer to intermediate JSON structure
2. Base64-encode the original FHiCL buffer for perfect round-trip fidelity
3. Create the final JSON document containing both parsed structure and Base64 data
4. Return as JsonData

**Data Flow:**
```
fhicl_buffer --> fhicl_to_json() --> intermediate JSON
fhicl_buffer --> base64_encode() --> Base64 string
intermediate JSON + Base64 --> final JsonData
```

**Thread Safety:** Safe (operates on const input, creates new output)

### JSON to FHiCL Conversion

**Brief:** Converts JsonData to FhiclData by extracting the Base64 field, decoding it, and converting the intermediate JSON to FHiCL format.

**Steps:**
1. Validate JSON document is not empty
2. Use regex to extract the "base64" field value from JSON
3. Decode Base64 to get intermediate JSON
4. Call `json_to_fhicl()` to convert intermediate JSON to FHiCL
5. Store result in `fhicl_buffer`

**Data Flow:**
```
JsonData --> regex_search --> Base64 string
Base64 string --> base64_decode() --> intermediate JSON
intermediate JSON --> json_to_fhicl() --> fhicl_buffer
```

**Thread Safety:** Safe (operates on const input, creates new output)

**Why this approach:** The two-stage conversion with Base64 ensures:
- Perfect round-trip fidelity (original FHiCL bytes preserved exactly)
- Both structured (parsed) and raw formats available
- Safe handling of special characters in FHiCL

## Dependencies

| Include | Purpose |
|---------|---------|
| `data_json.h` | JsonData class definition |
| `data_json_fusion.h` | Boost.Fusion adaptation for JsonData |
| `data_fhicl.h` | FhiclData class declaration |
| `data_fhicl_fusion.h` | Boost.Fusion adaptation for FhiclData |
| `base64.h` | `base64_encode()` and `base64_decode()` functions |
| `DataFormats/Fhicl/fhicl_common.h` | FHiCL common utilities |
| `DataFormats/Fhicl/fhicljsondb.h` | `fhicl_to_json()` and `json_to_fhicl()` functions |
| `DataFormats/Fhicl/fhiclcpplib_includes.h` | FHiCL library headers |
| `DataFormats/shared_literals.h` | Shared string literals |
| `<utility>` | `std::move` for efficient string handling |

## Internal Constants

### Regex Pattern

```cpp
namespace regex {
constexpr auto parse_base64data = R"lit([\s\S]*"base64"\s*:\s*"(\S*?)")lit";
}
```

**Brief:** Regular expression pattern for extracting Base64-encoded data from JSON documents.

**Pattern Breakdown:**
- `[\s\S]*` - Match any characters (including newlines) before the target
- `"base64"` - Match literal "base64" key
- `\s*:\s*` - Match colon with optional surrounding whitespace
- `"(\S*?)"` - Capture group for the Base64 value (non-greedy, non-whitespace)

**Thread Safety:** N/A (compile-time constant)

**Note:** The implementation uses `results[0]` (full match) rather than `results[1]` (capture group), which includes the entire matched text. The code checks for `results.size() != 1`, which verifies that the regex matched.

## Functions

### Template Specialization: `JsonData::convert_to<FhiclData>`

```cpp
template <>
bool JsonData::convert_to(FhiclData& fhicl) const {
  using artdaq::database::fhicljson::json_to_fhicl;
  return json_to_fhicl(json_buffer, fhicl.fhicl_buffer, fhicl.fhicl_file_name);
}
```

**Brief:** Template specialization that converts JSON representation to FHiCL format by delegating to the fhicljson library.

**Called by:** `FhiclData(JsonData const&)` constructor (indirectly through conversion chain)

**Parameters:**
- `fhicl` - FhiclData object to populate with converted data (output parameter)

**Preconditions:**
- `json_buffer` should contain valid JSON with the expected structure

**Returns:** `true` if conversion succeeded, `false` otherwise

**Postconditions:**
- On success: `fhicl.fhicl_buffer` contains the converted FHiCL
- On success: `fhicl.fhicl_file_name` may be populated from JSON metadata
- On failure: `fhicl` may be in undefined state

**Throws:** None (returns false on failure)

**Thread Safety:** Safe (const method, reads only)

---

### Template Specialization: `JsonData::convert_from<FhiclData>`

```cpp
template <>
bool JsonData::convert_from(FhiclData const& fhicl) {
  using artdaq::database::fhicljson::fhicl_to_json;
  return fhicl_to_json(fhicl.fhicl_buffer, fhicl.fhicl_file_name, json_buffer);
}
```

**Brief:** Template specialization that converts FHiCL format to JSON representation by delegating to the fhicljson library.

**Called by:** `FhiclData::operator JsonData()` (indirectly through conversion chain)

**Parameters:**
- `fhicl` - FhiclData object to convert from (input parameter)

**Preconditions:**
- `fhicl.fhicl_buffer` should contain valid FHiCL syntax

**Returns:** `true` if conversion succeeded, `false` otherwise

**Postconditions:**
- On success: `json_buffer` contains the converted JSON
- On failure: `json_buffer` may be in undefined state

**Throws:** None (returns false on failure)

**Thread Safety:** Unsafe (modifies `json_buffer`)

---

### Constructor: `FhiclData::FhiclData(std::string buffer)`

```cpp
FhiclData::FhiclData(std::string buffer) : fhicl_buffer{std::move(buffer)} {}
```

**Brief:** Constructs FhiclData by moving the provided FHiCL string into the buffer.

**Parameters:**
- `buffer` - String to move into `fhicl_buffer`

**Preconditions:** None

**Postconditions:**
- `fhicl_buffer` contains the provided string content
- Input `buffer` is in moved-from state
- `fhicl_file_name` is at default value "notprovided"

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_alloc` | If memory allocation fails (extremely rare) |

**Thread Safety:** Safe (constructor creates new object)

**Complexity:** O(1) - move operation is constant time

---

### Conversion Operator: `FhiclData::operator std::string const&() const`

```cpp
FhiclData::operator std::string const&() const { return fhicl_buffer; }
```

**Brief:** Returns a const reference to the internal FHiCL buffer for read-only access.

**Returns:** Const reference to `fhicl_buffer`

**Preconditions:** None

**Postconditions:** None (no state change)

**Throws:** None

**Thread Safety:** Safe (returns const reference)

**Complexity:** O(1)

---

### Constructor: `FhiclData::FhiclData(JsonData const& document)`

**Brief:** Constructs FhiclData by extracting Base64-encoded FHiCL from a JSON document, decoding it, and converting to FHiCL format.

**Algorithm:**
1. Confirm document is not empty using `confirm()` macro
2. Log input document at TLOG(11)
3. Use regex to find "base64" field in JSON
4. Validate the regex match was found
5. Validate result structure
6. Decode Base64 to get intermediate JSON
7. Convert intermediate JSON to FHiCL using `JsonData::convert_to()`

**Parameters:**
- `document` - JsonData containing Base64-encoded FHiCL

**Preconditions:**
- `document` must not be empty
- `document.json_buffer` must contain a "base64" field

**Postconditions:**
- `fhicl_buffer` contains the decoded FHiCL configuration
- `fhicl_file_name` may be populated if JSON contains metadata

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` | If document is empty |
| `std::runtime_error` | If regex search fails (no base64 field) |
| `std::runtime_error` | If result size is unexpected |
| `std::runtime_error` | If Base64 decoding or FHiCL conversion fails |

**Thread Safety:** Safe (constructor creates new object, reads from const input)

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_fhicl.h"
#include "artdaq-database/BasicTypes/data_json.h"
#include <iostream>

using namespace artdaq::database::basictypes;

void convertJsonToFhicl() {
  // Assume json_from_db was retrieved from database
  JsonData json_from_db = database.retrieve("config_id");

  // Always validate before conversion
  if (json_from_db.empty()) {
    std::cerr << "Error: Configuration not found\n";
    return;
  }

  try {
    FhiclData fhicl(json_from_db);
    std::cout << "Converted FHiCL:\n" << fhicl << "\n";

  } catch (const std::runtime_error& e) {
    std::cerr << "Conversion failed: " << e.what() << "\n";
    // Possible causes:
    // - JSON missing "base64" field
    // - Invalid Base64 encoding
    // - Invalid intermediate JSON structure
  }
}
```

---

### Conversion Operator: `FhiclData::operator JsonData() const`

**Brief:** Converts FhiclData to JsonData by converting FHiCL to JSON and Base64-encoding the original FHiCL for round-trip fidelity.

**Algorithm:**
1. Log input FHiCL at TLOG(17)
2. Create empty JsonData
3. Convert FHiCL to JSON using `json.convert_from(*this)`
4. Throw if conversion fails
5. Build collection name from type_version() (currently unused)
6. Base64-encode original fhicl_buffer
7. Build final JSON output via ostringstream
8. Return JsonData

**Returns:** JsonData containing the FHiCL configuration in JSON format with embedded Base64

**Preconditions:**
- `fhicl_buffer` should contain valid FHiCL syntax

**Postconditions:**
- Returned JsonData contains valid JSON with embedded Base64
- Original FhiclData is unchanged

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` | If FHiCL to JSON conversion fails (invalid FHiCL syntax) |

**Thread Safety:** Safe (const method, creates new output)

**Example:**
```cpp
#include "artdaq-database/BasicTypes/data_fhicl.h"
#include "artdaq-database/BasicTypes/data_json.h"
#include <iostream>

using namespace artdaq::database::basictypes;

void convertFhiclToJson() {
  try {
    FhiclData fhicl("detector: { threshold: 100 gain: 1.5 }");
    fhicl.fhicl_file_name = "detector.fcl";

    // Implicit conversion via operator
    JsonData json = fhicl;

    // Validate conversion
    if (json.empty()) {
      std::cerr << "Warning: Conversion produced empty JSON\n";
    } else {
      std::cout << "Converted JSON size: "
                << std::string(json).size() << " bytes\n";
    }

  } catch (const std::runtime_error& e) {
    std::cerr << "Conversion failed: " << e.what() << "\n";
    // Most likely cause: invalid FHiCL syntax
  }
}
```

---

### Stream Operator: `operator>>`

```cpp
std::istream& operator>>(std::istream& is, FhiclData& data) {
  auto str = std::string(std::istreambuf_iterator<char>(is), {});
  auto json = JsonData(str);
  data = FhiclData(json);
  return is;
}
```

**Brief:** Reads JSON-encoded FHiCL from an input stream and converts to FhiclData.

**Parameters:**
- `is` - Input stream containing JSON-encoded FHiCL
- `data` - FhiclData object to populate

**Preconditions:**
- `is` should be in a valid state
- Stream content should be JSON-encoded FHiCL (not raw FHiCL text)

**Returns:** Reference to the input stream

**Postconditions:**
- `data` contains the decoded FHiCL configuration
- Stream position is at EOF

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` | If JSON is empty or missing base64 field |
| `std::runtime_error` | If conversion to FHiCL fails |

**Thread Safety:** Unsafe (modifies `data`)

**Warning:** This reads JSON-encoded FHiCL, NOT raw FHiCL text. For raw FHiCL, read as string and construct FhiclData directly.

---

### Stream Operator: `operator<<`

```cpp
std::ostream& operator<<(std::ostream& os, FhiclData const& data) {
  os << data.fhicl_buffer;
  return os;
}
```

**Brief:** Writes raw FHiCL content to an output stream.

**Parameters:**
- `os` - Output stream
- `data` - FhiclData object to write

**Preconditions:**
- `os` should be in a valid state

**Returns:** Reference to the output stream

**Postconditions:**
- `fhicl_buffer` content written to stream
- Stream position advanced

**Throws:**

| Exception | Condition |
|-----------|-----------|
| Stream exceptions | If stream is configured to throw on errors |

**Thread Safety:** Safe if `data` is not concurrently modified

**Complexity:** O(n) where n is buffer length

## Performance Considerations

### String Operations
- Multiple string copies/moves during conversion
- Regex search on potentially large JSON documents
- Base64 encoding/decoding adds ~33% overhead to data size

### Memory Usage
- Intermediate strings created during conversion chain
- Large configurations may require significant temporary memory
- Both parsed JSON and Base64 data stored in output

### Recommendations
- Cache converted data if used multiple times
- Avoid repeated conversions in loops
- Consider memory constraints for very large configurations

## Error Handling Strategy

| Error Case | Handling |
|------------|----------|
| Empty JSON document | Throws `std::runtime_error` via `confirm()` macro |
| Missing "base64" field | Throws `std::runtime_error` with JSON content in message |
| Unexpected regex result size | Throws `std::runtime_error` |
| Invalid Base64 data | May produce corrupted FHiCL (Base64 decode is lenient) |
| Invalid FHiCL syntax | `fhicl_to_json()` returns false, throws `std::runtime_error` |

**Note:** Error messages contain "convertion" (typo preserved from original code) instead of "conversion".

## Thread Safety

**Not thread-safe:**

| Access Pattern | Safety |
|----------------|--------|
| Multiple readers (const methods) | Safe |
| Single writer | Safe |
| Concurrent read/write | Unsafe - requires external synchronization |

## Testing Notes

- **Unit tests:** Located in BasicTypes test suite
- **Key test scenarios:**
  - Round-trip conversion: FHiCL -> JSON -> FHiCL produces identical output
  - Empty input handling
  - Complex nested FHiCL structures
  - FHiCL with special characters (quotes, newlines)
  - Error cases (empty JSON, missing base64, invalid FHiCL)

**Example test:**
```cpp
#include "artdaq-database/BasicTypes/data_fhicl.h"
#include "artdaq-database/BasicTypes/data_json.h"
#include <cassert>
#include <iostream>

using namespace artdaq::database::basictypes;

void testRoundTrip() {
  try {
    // Test round-trip conversion
    FhiclData original(R"(
      module: {
        nested: {
          array: [1, 2, 3]
          string_param: "value with \"quotes\""
        }
      }
    )");

    JsonData json = original;
    if (json.empty()) {
      std::cerr << "Test failed: Conversion to JSON produced empty result\n";
      return;
    }

    FhiclData restored(json);

    // Note: Whitespace may differ, but semantic content should match
    std::cout << "Original:\n" << original << "\n";
    std::cout << "Restored:\n" << restored << "\n";
    std::cout << "Round-trip test completed\n";

  } catch (const std::runtime_error& e) {
    std::cerr << "Test failed: " << e.what() << "\n";
  }
}

void testEmptyJsonHandling() {
  try {
    JsonData empty("");
    FhiclData fhicl(empty);  // Should throw
    std::cerr << "Test failed: Expected exception for empty JSON\n";
  } catch (const std::runtime_error& e) {
    std::cout << "Empty JSON test passed: " << e.what() << "\n";
  }
}
```

## TRACE Logging

This file uses TRACE levels 11-21 for debugging conversion steps:

| Level | Purpose |
|-------|---------|
| TLOG(11) | Input JSON document |
| TLOG(12) | Regex search failure diagnostic |
| TLOG(13) | Unexpected regex result size diagnostic |
| TLOG(14) | Extracted Base64 string |
| TLOG(15) | Decoded intermediate JSON |
| TLOG(16) | Resulting FHiCL buffer |
| TLOG(17) | Input FHiCL buffer |
| TLOG(18) | FHiCL to JSON conversion error |
| TLOG(19) | Intermediate JSON result |
| TLOG(20) | Base64-encoded string |
| TLOG(21) | Final JSON document |

## Maintenance Notes

### Collection Naming
Collection names include version for schema evolution:
```cpp
auto collection = std::string("FhiclData_") + type_version();
// Result: "FhiclData_V100"
```

**Note:** The `collection` variable is computed but not used in the current implementation - it appears to be preparation for future schema versioning.

### Known Issues
- "convertion" typo in error messages (preserved for backwards compatibility)
- `collection` variable is unused
- Regex approach may be fragile if JSON structure changes
- Uses `results[0]` (full match) rather than `results[1]` (capture group)

## See Also

- [data_fhicl.h](./data_fhicl.h.md) - Public interface
- [data_json.h](./data_json.h.md) - JsonData class
- [base64.h](./base64.h.md) - Base64 encoding functions
- [DataFormats/Fhicl/](../DataFormats/Fhicl/README.md) - FHiCL conversion library
