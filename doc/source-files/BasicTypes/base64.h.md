# base64.h

**Path:** `artdaq-database/BasicTypes/base64.h`

**Purpose:** Declares functions for Base64 encoding and decoding of binary data and strings. Base64 encoding is essential for safely embedding FHiCL and XML configuration data within JSON documents stored in the database, ensuring character safety and preserving the original format during round-trip conversions.


## Key Concepts

### Base64 Encoding

Base64 is a binary-to-text encoding scheme that represents binary data using 64 printable ASCII characters (A-Z, a-z, 0-9, +, /). It is used in artdaq-database to:

1. **Preserve Original Format**: FHiCL and XML configurations are Base64-encoded before storage, ensuring the exact original content (including whitespace, comments, and formatting) can be perfectly restored after retrieval
2. **Enable Round-Trip Conversion**: Guarantees that `decode(encode(original)) == original` for any input
3. **JSON Compatibility**: Makes any binary or text data safe for embedding in JSON strings without complex escaping
4. **Character Safety**: Handles characters like quotes, backslashes, newlines, and control characters that would require complex escaping in JSON

### Why Base64 for Configuration Storage

When storing FHiCL or XML in JSON format, the original configuration undergoes parsing and transformation. While the semantic content is preserved, formatting details like:
- Indentation and whitespace
- Comments
- Line endings
- Specific numeric formatting

...may be altered. By also storing a Base64-encoded copy of the original, the exact byte-for-byte original can always be recovered.

### Size Overhead

Base64 encoding increases data size by approximately 33% (4 output bytes per 3 input bytes). For a 1KB configuration, expect approximately 1.33KB of Base64 output.

## Thread Safety

- **Thread-safe:** Yes
- **Concurrent access:** All functions are pure functions with no shared state, making them safe for concurrent use from multiple threads
- **Locking:** None required - functions operate only on their input parameters and local variables

## Dependencies

| Include | Purpose |
|---------|---------|
| `<string>` | Standard C++ string class for input parameters and return values |

## Functions

### `base64_encode(unsigned char const* bytes, unsigned int length) -> std::string`

**Brief:** Encodes a byte array into a Base64-encoded string representation suitable for safe embedding in JSON documents.

**Parameters:**
- `bytes` - Pointer to the first byte of data to encode. Must not be null if length > 0. May contain any byte values including null bytes.
- `length` - Number of bytes to encode. May be 0, which returns an empty string. Maximum practical limit is constrained by available memory.

**Preconditions:**
- If `length > 0`, `bytes` must point to valid, readable memory containing at least `length` bytes

**Returns:** A Base64-encoded string. Output length is `((length + 2) / 3) * 4` characters, consisting only of characters from the Base64 alphabet (A-Z, a-z, 0-9, +, /) plus padding (=).

**Postconditions:**
- Returned string contains only valid Base64 characters
- Returned string can be decoded back to original bytes using `base64_decode()`
- Output length is always a multiple of 4

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::bad_alloc` | When memory allocation for the result string fails |

**Thread Safety:** safe

**Side Effects:**
- None - pure function with no side effects

**Complexity:** O(n) where n is the input length

**Example:**
```cpp
#include "artdaq-database/BasicTypes/base64.h"
#include <iostream>
#include <cstring>

void encodeBinaryData() {
  // Encode binary data containing special characters
  unsigned char binary_data[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x00, 0x57, 0x6f, 0x72, 0x6c, 0x64};
  std::string encoded = base64_encode(binary_data, sizeof(binary_data));
  std::cout << "Encoded: " << encoded << "\n";  // Output: "SGVsbG8AV29ybGQ="

  // Verify round-trip
  std::string decoded = base64_decode(encoded);
  bool matches = (decoded.size() == sizeof(binary_data)) &&
                 (memcmp(decoded.data(), binary_data, sizeof(binary_data)) == 0);
  std::cout << "Round-trip successful: " << (matches ? "yes" : "no") << "\n";
}
```

---

### `base64_encode(std::string const& str) -> std::string`

**Brief:** Convenience overload that encodes a string into Base64 format by treating its contents as raw bytes. This is the primary function used when encoding FHiCL and XML configuration buffers for database storage.

**Parameters:**
- `str` - The string to encode. May be empty, may contain null characters or any binary data. The entire string content is encoded, including embedded null characters.

**Preconditions:**
- None

**Returns:** A Base64-encoded string representing the input string's byte content.

**Postconditions:**
- Returned string contains only Base64 characters plus optional padding
- `base64_decode(result)` returns the original string exactly

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::bad_alloc` | When memory allocation for the result string fails |

**Thread Safety:** safe

**Side Effects:**
- None

**Complexity:** O(n) where n is the string length

**Example:**
```cpp
#include "artdaq-database/BasicTypes/base64.h"
#include <iostream>

void encodeFhiclConfig() {
  // Encode a FHiCL configuration snippet
  std::string fhicl_config = R"(
    daq_settings: {
      buffer_size: 8192
      timeout_ms: 1000
      channels: [0, 1, 2, 3]
    }
  )";

  std::string encoded = base64_encode(fhicl_config);

  std::cout << "Original length: " << fhicl_config.length() << " bytes\n";
  std::cout << "Encoded length: " << encoded.length() << " bytes\n";
  std::cout << "Overhead: " << (100.0 * encoded.length() / fhicl_config.length() - 100) << "%\n";

  // Verify round-trip preserves exact content including whitespace
  std::string decoded = base64_decode(encoded);
  if (decoded == fhicl_config) {
    std::cout << "Round-trip successful - configuration preserved exactly\n";
  }
}
```

---

### `base64_decode(std::string const& encoded) -> std::string`

**Brief:** Decodes a Base64-encoded string back to its original byte representation. This function is used when extracting FHiCL or XML configurations from JSON documents retrieved from the database, restoring the exact original format.

**Parameters:**
- `encoded` - The Base64-encoded string to decode. Should contain only valid Base64 characters (A-Z, a-z, 0-9, +, /, =). Invalid characters are silently ignored.

**Preconditions:**
- For correct results, input should be valid Base64 produced by `base64_encode()`
- Invalid characters in input are silently filtered, which may produce unexpected results

**Returns:** The decoded string containing the original byte sequence.

**Postconditions:**
- If input was produced by `base64_encode()`, output equals the original data exactly
- Output may contain any byte values including null characters

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::bad_alloc` | When memory allocation for the result string fails |

**Thread Safety:** safe

**Side Effects:**
- None

**Complexity:** O(n) where n is the encoded string length

**Example:**
```cpp
#include "artdaq-database/BasicTypes/base64.h"
#include <iostream>

void decodeFromDatabase() {
  // Simulate retrieving Base64-encoded configuration from database
  std::string encoded_from_db = "cGFyYW1ldGVyOiB2YWx1ZQo=";

  std::string decoded = base64_decode(encoded_from_db);
  std::cout << "Decoded configuration:\n" << decoded << "\n";

  // Handle potentially corrupted data
  std::string potentially_invalid = getUserInput();
  std::string result = base64_decode(potentially_invalid);
  // Note: Invalid characters are silently ignored
  // Always validate the decoded content if dealing with untrusted input
  if (result.empty() && !potentially_invalid.empty()) {
    std::cerr << "Warning: Decode produced empty result from non-empty input\n";
  }
}
```

## Relationship to Other Components

The Base64 functions are fundamental utilities used by the data conversion classes to preserve original configuration formats during round-trips:

```
FhiclData::operator JsonData()  -->  base64_encode(fhicl_buffer)  -->  JSON document
FhiclData(JsonData const&)      <--  base64_decode(base64_field)  <--  JSON document

XmlData::operator JsonData()    -->  base64_encode(xml_buffer)    -->  JSON document
XmlData(JsonData const&)        <--  base64_decode(base64_field)  <--  JSON document
```

- **data_fhicl.cpp**: Uses `base64_encode()` when converting FhiclData to JsonData, and `base64_decode()` when constructing FhiclData from JsonData
- **data_xml.cpp**: Uses the same pattern for XML configuration handling

## See Also

- [base64.cpp](./base64.cpp.md) - Implementation details and algorithm explanation
- [data_fhicl.cpp](./data_fhicl.cpp.md) - Primary consumer for FHiCL encoding
- [data_xml.cpp](./data_xml.cpp.md) - Primary consumer for XML encoding
- [External: RFC 4648](https://tools.ietf.org/html/rfc4648) - The Base64 encoding specification

## Notes for Developers

### Why Base64 is Critical for Round-Trips

When FHiCL or XML is converted to JSON for storage, the transformation process normalizes the data. For example:

```
Original FHiCL:
  parameter: 100    # important comment
  other:     value

After JSON round-trip (without Base64):
  parameter: 100
  other: value

With Base64 preservation:
  Exact original restored including comment and spacing
```

### Common Pitfalls

- **Pitfall 1:** Assuming encoded output length equals input length. Base64 encoding increases size by approximately 33% (4 output bytes per 3 input bytes).
- **Pitfall 2:** Modifying Base64 strings after encoding. Any character change will corrupt the decoded data.
- **Pitfall 3:** Expecting validation of input to `base64_decode()`. Invalid characters are silently ignored, which may produce unexpected results for malformed input.

### Anti-patterns

```cpp
// DON'T do this - modifying Base64 string:
std::string encoded = base64_encode(data);
encoded[0] = 'X';  // Corrupts the decoded output!
std::string decoded = base64_decode(encoded);  // Will not match original

// DO this instead - treat Base64 strings as immutable:
std::string encoded = base64_encode(data);
// Store and retrieve without modification
std::string decoded = base64_decode(encoded);  // Matches original

// DON'T do this - assuming validation:
std::string user_input = getUntrustedInput();
std::string decoded = base64_decode(user_input);
processConfig(decoded);  // May have unexpected content!

// DO this instead - validate after decoding:
std::string user_input = getUntrustedInput();
std::string decoded = base64_decode(user_input);
if (isValidConfiguration(decoded)) {
  processConfig(decoded);
}
```

### Global Scope Declaration

Note that the Base64 functions are declared in the global namespace (not within `artdaq::database` or any other namespace). This is intentional to provide simple, reusable utility functions without namespace qualification.
