# base64.cpp

## File Overview

**Location**: `/home/user/artdaq-database/artdaq-database/BasicTypes/base64.cpp`

This file contains the implementation of Base64 encoding and decoding algorithms. The code is based on René Nyffenegger's Base64 implementation, which is widely used and well-tested.

**Purpose**: Implements the Base64 encoding and decoding functions declared in `base64.h`.

**License**: The code is provided under a permissive license by René Nyffenegger (Copyright 2004-2008). See the header comment in the file for full license terms.

**Original Source**: http://www.adp-gmbh.ch/cpp/common/base64.html

## Dependencies

- `<iostream>` - For standard I/O operations (though not heavily used in the implementation)
- `base64.h` (implied) - The header declaring the functions implemented here

## Constants/Literals

### base64_chars

```cpp
static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";
```

**Purpose**: Defines the 64-character alphabet used for Base64 encoding.

**Details**:
- Characters 0-25: A-Z (uppercase letters)
- Characters 26-51: a-z (lowercase letters)
- Characters 52-61: 0-9 (digits)
- Characters 62-63: +/ (special characters)

This is the standard Base64 alphabet as defined in RFC 4648.

## Helper Functions

### is_base64

```cpp
static inline bool is_base64(unsigned char c)
```

**Purpose**: Checks if a character is a valid Base64 character.

**Parameters**:
- `c` - Character to check

**Return Value**: `true` if the character is alphanumeric, '+', or '/'; `false` otherwise

**Implementation**: Uses `isalnum()` to check for alphanumeric characters, plus explicit checks for '+' and '/'

**Usage**: Used internally during decoding to validate input characters.

## Functions/Methods

### Function 1: base64_encode (binary data version)

```cpp
std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len)
```

**Purpose**: Encodes binary data into a Base64 string.

**Algorithm**:
1. Processes input data in 3-byte chunks
2. Each 3-byte chunk is converted to 4 Base64 characters
3. Remaining bytes (if input length not divisible by 3) are padded with '='

**Detailed Process**:
```
Input:  3 bytes = 24 bits
Output: 4 Base64 characters (6 bits each)

Example:
Input bytes:  [0b11010110] [0b10001100] [0b11110000]
Split into 6-bit groups: [110101] [101000] [110011] [110000]
Map to Base64 chars: 1→B, 40→o, 51→z, 48→w
Output: "Bozw"
```

**Bit Manipulation Details**:
- `char_array_4[0] = (char_array_3[0] & 0xfc) >> 2` - Takes top 6 bits of first byte
- `char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4)` - Bottom 2 bits of byte 1 + top 4 bits of byte 2
- `char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6)` - Bottom 4 bits of byte 2 + top 2 bits of byte 3
- `char_array_4[3] = char_array_3[2] & 0x3f` - Bottom 6 bits of byte 3

**Padding**:
- If input length is not divisible by 3, the output is padded with '=' characters
- 1 remaining byte → 2 Base64 chars + "=="
- 2 remaining bytes → 3 Base64 chars + "="

**Return Value**: Base64-encoded string

---

### Function 2: base64_decode

```cpp
std::string base64_decode(std::string const& encoded_string)
```

**Purpose**: Decodes a Base64-encoded string back to its original binary form.

**Algorithm**:
1. Processes input in 4-character chunks
2. Each 4-character chunk is converted back to 3 bytes
3. Stops when encountering '=' padding or non-Base64 characters

**Detailed Process**:
```
Input:  4 Base64 characters (6 bits each)
Output: 3 bytes = 24 bits

Example:
Input chars: "Bozw"
Convert to indices: B→1, o→40, z→51, w→48
Indices as 6-bit: [000001] [101000] [110011] [110000]
Combine to bytes: [00000110] [10001100] [11110000]
```

**Bit Manipulation Details**:
- `char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4)` - All 6 bits of char 1 + top 2 bits of char 2
- `char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2)` - Bottom 4 bits of char 2 + top 4 bits of char 3
- `char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3]` - Bottom 2 bits of char 3 + all 6 bits of char 4

**Padding Handling**:
- The function stops processing when it encounters '=' or end of string
- Partial chunks at the end are handled by only outputting the valid decoded bytes

**Return Value**: Decoded binary data as a string

---

### Function 3: base64_encode (string version)

```cpp
std::string base64_encode(std::string const& str)
```

**Purpose**: Convenience wrapper that encodes a `std::string`.

**Implementation**: Calls the binary version of `base64_encode` after converting the string to `unsigned char*`

**Parameters**:
- `str` - The string to encode

**Return Value**: Base64-encoded string

**Usage Example**:
```cpp
std::string fhicl_config = "parameter: value";
std::string encoded = base64_encode(fhicl_config);
```

## Usage Context

This implementation is used throughout the artdaq-database project:

1. **In `data_fhicl.cpp`**:
   ```cpp
   auto base64 = base64_encode(fhicl_buffer);  // Encoding
   auto json = base64_decode(base64);          // Decoding
   ```

2. **In `data_xml.cpp`**:
   ```cpp
   auto base64 = base64_encode(xml_buffer);    // Encoding
   auto json = base64_decode(base64);          // Decoding
   ```

3. **Data Flow Example**:
   ```
   FHICL Config → base64_encode → JSON field → Database
   Database → JSON field → base64_decode → FHICL Config
   ```

## Performance Considerations

- **Memory**: Creates temporary arrays and strings; avoid encoding very large data sets repeatedly
- **Efficiency**: The algorithm processes data in chunks, making it reasonably efficient
- **Size Overhead**: Base64 encoding increases data size by ~33% (4 bytes output per 3 bytes input)

## Error Handling

- **Encoding**: No error conditions; all binary data can be encoded
- **Decoding**:
  - Invalid characters are ignored (filtered by `is_base64()`)
  - Malformed input may produce unexpected results
  - No explicit error reporting mechanism

## Notes for Developers

1. **Character Validation**: The `is_base64()` function validates characters during decoding
2. **Static Functions**: Helper functions are marked `static` to limit scope to this translation unit
3. **Bit Masking**: Understanding the bit manipulation requires knowledge of how data is split across 6-bit boundaries
4. **Padding Rules**:
   - No padding: Input length divisible by 3
   - One '=': 2 bytes remained (encoded to 3 chars + 1 padding)
   - Two '==': 1 byte remained (encoded to 2 chars + 2 padding)

5. **Thread Safety**: These functions are thread-safe as they use only local variables and a constant string

## Testing Recommendations

When working with this code, test with:
- Empty strings
- Strings of length 1, 2, 3 (to test all padding scenarios)
- Binary data containing null bytes
- Long strings (performance testing)
- Invalid Base64 input (for decode function)

## References

- RFC 4648: The Base64 Data Encoding - https://tools.ietf.org/html/rfc4648
- Original implementation: http://www.adp-gmbh.ch/cpp/common/base64.html
