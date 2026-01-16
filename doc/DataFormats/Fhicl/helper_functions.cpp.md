# helper_functions.cpp

**Path:** `artdaq-database/DataFormats/Fhicl/helper_functions.cpp`

**Implements:** [helper_functions.h](./helper_functions.h.md)

**Purpose:** Implements utility functions for FHiCL parsing, formatting, and conversion operations. This file contains the implementations for string escaping, type tag conversion, protection attribute mapping, and an optimized buffer line-trimming algorithm.

## Implementation Overview

The implementation provides:

1. **Type checking:** Regex-based decimal number detection
2. **String manipulation:** Escape/unescape functions for JSON compatibility
3. **Enumeration conversion:** Bidirectional mapping between FHiCL enums and strings
4. **Buffer processing:** High-performance in-place line trimming

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/Fhicl/helper_functions.h` | Function declarations |
| `artdaq-database/DataFormats/common.h` | Common infrastructure including regex |
| `artdaq-database/DataFormats/shared_literals.h` | Type name string literals |

## TRACE Configuration

```cpp
#define TRACE_NAME "helper_functions.cpp"
```

## Key Algorithms

### Decimal Number Detection (isDouble)

**Brief:** Uses regular expression matching to determine if a string represents a floating-point number.

**Implementation:**
```cpp
bool isDouble(std::string const& str) {
  std::regex ex(literal::regex::parse_decimal);
  return std::regex_match(str, ex);
}
```

**Why this approach:** Regular expressions provide robust matching for various decimal formats including scientific notation.

---

### String Unescaping (unescape)

**Brief:** Simple regex replacement to convert escaped quotes to literal quotes.

**Implementation:**
```cpp
std::string unescape(std::string const& str) {
  return std::regex_replace(str, std::regex("\\\""), "\"");
}
```

---

### JSON String Escaping (to_json_string)

**Brief:** Iterates through each character and applies JSON escape sequences where needed.

**Steps:**
1. Create output string stream
2. For each character in input:
   - Check against escape table
   - Output escape sequence or original character
3. Return accumulated string

**Implementation:**
```cpp
std::string to_json_string(std::string const& str) {
  std::ostringstream oss;
  for (auto c : str) {
    switch (c) {
      case '"':  oss << "\\\""; break;
      case '\\': oss << "\\\\"; break;
      case '\b': oss << "\\b";  break;
      case '\f': oss << "\\f";  break;
      case '\n': oss << "\\n";  break;
      case '\r': oss << "\\r";  break;
      case '\t': oss << "\\t";  break;
      default:   oss << c;
    }
  }
  return oss.str();
}
```

---

### JSON String Unescaping (from_json_string)

**Brief:** Parses escape sequences in a JSON string and converts them to literal characters.

**Steps:**
1. Create output string stream
2. Iterate through input with position index
3. On backslash, check next character for escape sequence
4. Convert recognized sequences to literal characters
5. Return accumulated string

**Why this approach:** Manual parsing allows precise handling of all JSON escape sequences without regex overhead.

---

### Type Tag Conversion (tag_as_string, string_as_tag)

**Brief:** Simple switch/if-else chains mapping between FHiCL enumeration values and their string representations.

**tag_as_string Implementation:**
```cpp
std::string tag_as_string(::fhicl::value_tag tag) {
  switch (tag) {
    default:             return literal::unknown;
    case ::fhicl::NIL:      return literal::nil;
    case ::fhicl::STRING:   return literal::string;
    case ::fhicl::BOOL:     return literal::boolean;
    case ::fhicl::NUMBER:   return literal::number;
    case ::fhicl::COMPLEX:  return literal::complex;
    case ::fhicl::SEQUENCE: return literal::sequence;
    case ::fhicl::TABLE:    return literal::table;
    case ::fhicl::TABLEID:  return literal::tableid;
  }
}
```

**string_as_tag Implementation:**
- Handles multiple string variants for STRING type (string, string_unquoted, string_singlequoted, string_doublequoted)
- Throws `::fhicl::exception` for unrecognized type strings

---

### Protection Attribute Conversion (protection_as_string, string_as_protection)

**Brief:** Maps between FHiCL protection enumeration values and their `@`-prefixed string representations.

**Supported Values:**
- `::fhicl::Protection::NONE` <-> `"@none"`
- `::fhicl::Protection::PROTECT_IGNORE` <-> `"@protect_ignore"`
- `::fhicl::Protection::PROTECT_ERROR` <-> `"@protect_error"`

**Note:** Some protection values are commented out in the implementation (INITIAL, REPLACE, REPLACE_COMPAT, ADD_OR_REPLACE_COMPAT) suggesting they may be planned for future support.

---

### Buffer Line Trimming (buffer_rtrim_lines)

**Brief:** High-performance in-place algorithm that removes trailing whitespace from each line in a buffer.

**Algorithm:**
1. Build a compile-time lookup table for whitespace characters (space, tab, carriage return)
2. Use read/write pointer pattern for in-place modification
3. Process line by line using `memchr` for newline detection
4. Scan backwards from line end to find last non-whitespace
5. Use `memmove` to compact the buffer
6. Ensure buffer ends with exactly one newline

**Implementation Details:**

```cpp
void buffer_rtrim_lines(std::string& buffer) noexcept {
  if (buffer.empty()) return;

  // Constexpr lookup table for O(1) whitespace checking
  struct WsTable {
    alignas(64) bool data[256]{};
    constexpr WsTable() noexcept {
      data[static_cast<unsigned char>(' ')] = true;
      data[static_cast<unsigned char>('\t')] = true;
      data[static_cast<unsigned char>('\r')] = true;
    }
    [[nodiscard]] constexpr bool operator[](char c) const noexcept {
      return data[static_cast<unsigned char>(c)];
    }
  };
  static constexpr WsTable isWs{};

  // In-place processing with read/write pointers
  char* const base = buffer.data();
  char* write = base;
  const char* read = base;
  const char* const end = base + buffer.size();

  while (read < end) {
    // Find end of line
    const char* nl = static_cast<const char*>(
        std::memchr(read, '\n', static_cast<size_t>(end - read)));
    const char* lineEnd = nl ? nl : end;

    // Trim trailing whitespace
    while (lineEnd > read && isWs[lineEnd[-1]]) {
      --lineEnd;
    }

    // Move content if needed
    const auto len = static_cast<size_t>(lineEnd - read);
    if (write != read) {
      std::memmove(write, read, len);
    }
    write += len;

    // Add newline and advance
    if (nl) {
      *write++ = '\n';
      read = nl + 1;
    } else {
      break;
    }
  }

  // Ensure trailing newline
  const auto newSize = static_cast<size_t>(write - base);
  if (newSize == 0 || buffer[newSize - 1] != '\n') {
    buffer.resize(newSize + 1);
    buffer[newSize] = '\n';
  } else {
    buffer.resize(newSize);
  }
}
```

**Performance Characteristics:**
- **Time complexity:** O(n) where n is buffer size
- **Space complexity:** O(1) - in-place modification
- **Cache efficiency:** Uses cache-aligned lookup table (alignas(64))
- **Memory operations:** Uses optimized `memchr` and `memmove`

## Error Handling Strategy

### Type Conversion Errors

Both `string_as_tag` and `string_as_protection` throw `::fhicl::exception` with error code `::fhicl::parse_error` when given unrecognized input:

```cpp
throw ::fhicl::exception(::fhicl::parse_error, literal::data)
    << ("FHiCL atom type \"" + str + "\" is not implemented.");
```

### Noexcept Functions

`buffer_rtrim_lines` is marked `noexcept` and handles all edge cases internally:
- Empty buffer: Returns immediately
- No newlines: Processes as single line
- Already trimmed: Minimal overhead

## Testing Notes

- **Unit tests:** `test/DataFormats/Fhicl/helper_functions_t.cc`
- **Key test cases:**
  - Round-trip escaping (to_json_string -> from_json_string)
  - All FHiCL type tags
  - All protection levels
  - Buffer trimming edge cases (empty, no whitespace, all whitespace)

## Maintenance Notes

- The `literal` namespace alias references `artdaq::database::dataformats::literal`
- String literals for type names must match those used in `convertfhicl2jsondb.cpp`
- The commented-out protection values may need implementation if FHiCL library adds support

## See Also

- [helper_functions.h](./helper_functions.h.md) - Header file with declarations
- [convertfhicl2jsondb.cpp](./convertfhicl2jsondb.cpp.md) - Primary consumer
- [shared_literals.h](../shared_literals.h.md) - Type name string literals
