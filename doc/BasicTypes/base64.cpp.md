# base64.cpp

**Path:** `artdaq-database/BasicTypes/base64.cpp`

**Implements:** [base64.h](./base64.h.md)

**Purpose:** Implements the Base64 encoding and decoding algorithms used throughout artdaq-database for safely embedding configuration data in JSON documents while preserving the original format during round-trip conversions. The implementation is based on Rene Nyffenegger's well-tested Base64 code.

## Implementation Overview

This file implements the standard Base64 encoding scheme as defined in RFC 4648. The algorithm processes data in 3-byte chunks for encoding (producing 4 Base64 characters) and 4-character chunks for decoding (producing 3 bytes).

The primary purpose is to enable perfect round-trip preservation of FHiCL and XML configurations. When a configuration is stored in the database:
1. The original text is Base64-encoded and stored alongside the parsed JSON representation
2. On retrieval, the Base64 is decoded to recover the exact original, byte-for-byte

## Key Algorithms

### Base64 Encoding Algorithm

**Brief:** Converts every 3 bytes of input into 4 Base64 characters by splitting 24 bits into four 6-bit groups.

**Steps:**
1. Read input bytes in groups of 3
2. Split the 24 bits (3 bytes x 8 bits) into four 6-bit values using bit manipulation
3. Map each 6-bit value (0-63) to a character in the Base64 alphabet
4. If input length is not divisible by 3, pad output with '=' characters
5. Return the encoded string

**Bit Manipulation Details:**
```
Input bytes:     byte0        byte1        byte2
Bit positions:   76543210     76543210     76543210

Output chars:    char0        char1        char2        char3
Bit positions:   543210       543210       543210       543210

Mapping:
  char0 = (byte0 & 0xfc) >> 2                           // bits 7-2 of byte0
  char1 = ((byte0 & 0x03) << 4) + ((byte1 & 0xf0) >> 4) // bits 1-0 of byte0 + bits 7-4 of byte1
  char2 = ((byte1 & 0x0f) << 2) + ((byte2 & 0xc0) >> 6) // bits 3-0 of byte1 + bits 7-6 of byte2
  char3 = byte2 & 0x3f                                  // bits 5-0 of byte2
```

**Padding Rules:**
- 0 remaining bytes: No padding needed
- 1 remaining byte: Output 2 Base64 chars + "=="
- 2 remaining bytes: Output 3 Base64 chars + "="

**Why this approach:** The standard Base64 algorithm is well-documented, widely compatible, and efficient. Processing in 3-byte chunks minimizes overhead while maintaining simplicity.

### Base64 Decoding Algorithm

**Brief:** Reverses the encoding process, converting every 4 Base64 characters back to 3 bytes.

**Steps:**
1. Read Base64 characters in groups of 4
2. Filter out non-Base64 characters using `is_base64()` helper
3. Look up each character's 6-bit value using `std::string::find()` on `base64_chars`
4. Combine four 6-bit values into three 8-bit bytes using bit manipulation
5. Handle padding ('=') characters by terminating early and reducing output bytes
6. Return the decoded bytes as a string

**Bit Manipulation Details (reverse of encoding):**
```
char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4)
char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2)
char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3]
```

**Error Handling:** Invalid characters (not in Base64 alphabet and not '=') are silently skipped via the `is_base64()` filter. This provides tolerance for whitespace but may mask errors in malformed input.

## Internal Functions

### `is_base64(unsigned char c) -> bool`

**Brief:** Tests whether a character is a valid Base64 data character (alphanumeric, '+', or '/').

**Called by:** `base64_decode()` in the main decode loop

**Purpose:** Filters out whitespace, padding ('='), and invalid characters during decoding. Note that '=' is intentionally NOT considered a Base64 character by this function - it is handled separately as the termination signal in the decode loop condition.

**Implementation:**
```cpp
static inline bool is_base64(unsigned char c) {
  return ((isalnum(c) != 0) || (c == '+') || (c == '/'));
}
```

**Note:** Uses `isalnum()` from `<cctype>` (via `<iostream>`) which is locale-dependent. In practice, Base64 only uses ASCII alphanumerics, so this works correctly in all locales.

## Static Data

### `base64_chars`

**Brief:** The Base64 alphabet string used for encoding and decoding lookups.

**Value:** `"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"`

**Purpose:**
- During encoding: `base64_chars[6_bit_value]` gives the corresponding character
- During decoding: `base64_chars.find(char)` gives the 6-bit value

**Note:** This is the standard Base64 alphabet. The URL-safe variant (using '-' and '_' instead of '+' and '/') is not supported.

## Implemented Functions

### `base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) -> std::string`

**Brief:** Primary encoding function that processes the input byte array.

**Implementation Details:**
- Uses two fixed-size arrays (`char_array_3[3]` and `char_array_4[4]`) for chunk processing
- Processes complete 3-byte chunks in main loop
- Handles remaining 1 or 2 bytes with proper padding after loop
- Returns result by value (relies on RVO/NRVO for efficiency)

### `base64_encode(std::string const& str) -> std::string`

**Brief:** Convenience wrapper that calls the byte array version.

**Implementation:**
```cpp
std::string base64_encode(std::string const& str) {
  return base64_encode(reinterpret_cast<const unsigned char*>(str.c_str()), str.length());
}
```

### `base64_decode(std::string const& encoded_string) -> std::string`

**Brief:** Decodes a Base64 string back to original bytes.

**Implementation Details:**
- Uses index-based iteration (`in_`) through the input string
- Skips non-Base64 characters automatically via loop condition
- Stops processing when '=' padding is encountered
- Uses `std::string::find()` for reverse lookup (O(64) per character)
- Handles incomplete final groups (1-3 remaining characters)

## Performance Considerations

### Memory Usage
- **Encoding:** Allocates a single result string, grown incrementally by appending characters
- **Decoding:** Similarly allocates one result string with incremental growth
- **Temporary buffers:** Only small fixed-size arrays (3-4 bytes) on the stack
- **Predictable output size:** `encode_size = ((input_size + 2) / 3) * 4`

### Speed Characteristics
- **Single-pass processing:** Both encode and decode are O(n) single-pass algorithms
- **Character lookup:** Decoding uses `std::string::find()` which is O(64) per character lookup - acceptable for the small alphabet size
- **Inline helper:** `is_base64()` is marked `inline` for zero-overhead validation

### Potential Optimizations (not implemented)
1. Use a 256-entry lookup table for O(1) decode instead of `std::string::find()`
2. Reserve output string capacity upfront based on input size
3. Process 4 bytes at a time using SIMD instructions for large inputs

## Error Handling Strategy

The implementation takes a **lenient approach** to error handling:

| Scenario | Behavior |
|----------|----------|
| Invalid characters in decode input | Silently skipped (filtered by `is_base64()`) |
| Truncated Base64 input | Processed as far as possible, partial output returned |
| Whitespace in decode input | Silently skipped |
| Memory allocation failure | Standard `std::bad_alloc` exception propagates |
| Empty input | Returns empty string |

**Rationale:** This lenient approach prioritizes robustness over strict validation. Configuration data encoded by this library will always be valid, and the tolerance for whitespace handles formatted JSON. However, this means corrupted or malicious input may produce unexpected (but not dangerous) output.

## Testing Notes

- **Unit tests:** Located in the BasicTypes test suite
- **Key test cases to verify:**
  - Empty input produces empty output
  - Single byte (tests 2-char output + "==" padding)
  - Two bytes (tests 3-char output + "=" padding)
  - Three bytes (tests 4-char output, no padding)
  - Large inputs (performance and memory handling)
  - Binary data containing null bytes (0x00)
  - All possible byte values (0x00-0xFF comprehensive test)
  - Round-trip: `decode(encode(x)) == x` for all inputs

## Code Attribution

This implementation is based on code from:
- **Author:** Rene Nyffenegger
- **Website:** http://www.adp-gmbh.ch/cpp/common/base64.html
- **Copyright:** 2004-2008
- **License:** zlib-style license (see source file header)

The license permits:
- Use in commercial and non-commercial applications
- Modification and redistribution
- No warranty is provided

Required: Origin must not be misrepresented; modified versions must be marked as such.

## Maintenance Notes

### Historical Context
The Base64 implementation was adopted early in artdaq-database development to solve the problem of safely embedding FHiCL configurations (which can contain special characters, quotes, and newlines) in JSON documents stored in MongoDB and FileSystemDB. It also ensures that the exact original configuration can be recovered, preserving formatting, comments, and whitespace that may be meaningful to users.

### Known Limitations
1. Not URL-safe: Uses '+' and '/' which have special meaning in URLs
2. No streaming interface: Entire output must fit in memory
3. Decoding is lenient: Invalid input does not throw exceptions
4. Uses `std::string::find()` for decode lookup (slower than lookup table)

### Compatibility
- **C++ Standard:** Requires C++11 or later (uses `{}` initializer)
- **Platform:** No platform-specific code; works on Linux, macOS, Windows
- **Binary Safety:** Handles any byte sequence including embedded nulls

### Dependencies
The only dependency is `<iostream>` which is included for `isalnum()` function used in `is_base64()`.

## See Also

- [base64.h](./base64.h.md) - Public interface documentation
- [data_fhicl.cpp](./data_fhicl.cpp.md) - Uses Base64 for FHiCL-to-JSON conversion and round-trip preservation
- [data_xml.cpp](./data_xml.cpp.md) - Uses Base64 for XML-to-JSON conversion and round-trip preservation
- [External: RFC 4648](https://tools.ietf.org/html/rfc4648) - Base64 specification
