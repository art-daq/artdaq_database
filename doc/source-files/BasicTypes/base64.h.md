# base64.h

## File Overview

**Location**: `/home/user/artdaq-database/artdaq-database/BasicTypes/base64.h`

This header file provides the interface for Base64 encoding and decoding functionality. Base64 is a binary-to-text encoding scheme that represents binary data in an ASCII string format. This is commonly used to encode binary data for transmission over media that are designed to handle text, or for embedding binary data in text-based formats like JSON and XML.

**Purpose**: Declares functions for encoding and decoding data using the Base64 algorithm.

## Dependencies

- `<string>` - Standard C++ string library for string operations

## Key Types/Classes

No classes or types are defined in this header. It provides free functions only.

## Functions/Methods

### Function 1: base64_encode (binary data version)

```cpp
std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
```

**Purpose**: Encodes binary data (raw bytes) into a Base64-encoded string.

**Parameters**:
- `bytes_to_encode` - Pointer to the binary data to be encoded
- `in_len` - Length of the binary data in bytes

**Return Value**: A `std::string` containing the Base64-encoded representation of the input data

**Usage Example**:
```cpp
unsigned char data[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f}; // "Hello"
std::string encoded = base64_encode(data, 5);
// encoded will be "SGVsbG8="
```

**Use Cases**:
- Encoding binary configuration data for storage in text-based formats
- Embedding binary data in JSON/XML documents
- Transmitting binary data through text-only channels

---

### Function 2: base64_encode (string version)

```cpp
std::string base64_encode(std::string const& str);
```

**Purpose**: Encodes a string into a Base64-encoded string. This is a convenience overload that accepts a `std::string` instead of raw bytes.

**Parameters**:
- `str` - The string to be encoded

**Return Value**: A `std::string` containing the Base64-encoded representation of the input string

**Usage Example**:
```cpp
std::string data = "Hello, World!";
std::string encoded = base64_encode(data);
// encoded will be "SGVsbG8sIFdvcmxkIQ=="
```

**Use Cases**:
- Quick encoding of string data without manual pointer/length management
- Encoding text data before embedding in configuration documents

---

### Function 3: base64_decode

```cpp
std::string base64_decode(std::string const& encoded_string);
```

**Purpose**: Decodes a Base64-encoded string back into its original form.

**Parameters**:
- `encoded_string` - The Base64-encoded string to decode

**Return Value**: A `std::string` containing the decoded data

**Usage Example**:
```cpp
std::string encoded = "SGVsbG8sIFdvcmxkIQ==";
std::string decoded = base64_decode(encoded);
// decoded will be "Hello, World!"
```

**Use Cases**:
- Decoding configuration data stored in Base64 format
- Extracting embedded binary data from JSON/XML documents
- Receiving binary data that was transmitted through text channels

## Constants/Literals

None defined in the header file.

## Usage Context

This file is used throughout the artdaq-database project wherever configuration data needs to be:

1. **Stored in text formats**: The BasicTypes module uses Base64 encoding to embed FHICL and XML configurations within JSON documents (see `data_fhicl.cpp` and `data_xml.cpp`).

2. **Data format conversions**: When converting between different configuration formats (FHICL ↔ JSON, XML ↔ JSON), Base64 encoding ensures binary or special characters are preserved correctly.

3. **Database storage**: Base64 encoding allows binary configuration data to be stored in database fields that may have text constraints.

**Files that include this header**:
- `data_fhicl.cpp` - Uses Base64 to encode/decode FHICL data when converting to/from JSON
- `data_xml.cpp` - Uses Base64 to encode/decode XML data when converting to/from JSON

## Implementation Details

The implementation is provided in `base64.cpp` (see base64.cpp.md for details on the encoding/decoding algorithms).

## Notes for Developers

- **Character Set**: Base64 uses the standard character set: A-Z, a-z, 0-9, +, / with = as padding
- **Output Size**: Base64 encoding increases data size by approximately 33% (4 characters per 3 bytes of input)
- **Padding**: The output is padded with '=' characters to ensure the length is a multiple of 4
- **Thread Safety**: These are pure functions with no shared state, making them thread-safe
- **Error Handling**: The decode function assumes valid Base64 input; invalid characters may produce unexpected results

## Header Guards

```cpp
#ifndef _ARTDAQ_DATABASE_BASE64_CODEC_H_
#define _ARTDAQ_DATABASE_BASE64_CODEC_H_
```

Standard include guard prevents multiple inclusion.
