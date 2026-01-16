# helper_functions.h

**Path:** `artdaq-database/DataFormats/Fhicl/helper_functions.h`

**Purpose:** Declares utility functions for FHiCL parsing, formatting, and conversion operations. These helpers handle string escaping/unescaping, type tag conversion, protection attribute mapping, and buffer manipulation needed for FHiCL to JSON transformations.


## Key Concepts

### FHiCL Type Tags

FHiCL uses enumerated type tags (`::fhicl::value_tag`) to identify the type of each configuration value. This header provides bidirectional conversion between these tags and their string representations for metadata storage.

### Protection Attributes

FHiCL supports protection annotations that control how values can be overridden:
- `@none`: No protection (default)
- `@protect_ignore`: Silently ignore attempts to override
- `@protect_error`: Error on override attempts

### JSON String Escaping

JSON has specific escape sequence requirements that differ from FHiCL. These functions handle the bidirectional conversion of special characters.

## Thread Safety

- **Thread-safe:** Yes (all functions are stateless)
- **Concurrent access:** Safe to call from multiple threads
- **Locking:** No locking required

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/Fhicl/fhiclcpplib_includes.h` | FHiCL library types (`::fhicl::value_tag`, `::fhicl::Protection`) |
| `artdaq-database/DataFormats/common.h` | Common infrastructure and standard library includes |

## Functions

### `isDouble(str) -> bool`

**Brief:** Determines if a string represents a double-precision floating-point number.

**Parameters:**
- `str` - The string to check

**Preconditions:**
- None

**Returns:** `true` if the string matches the decimal number pattern, `false` otherwise

**Postconditions:**
- No side effects

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | This function does not throw |

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/DataFormats/Fhicl/helper_functions.h"

void checkNumericType() {
  using namespace artdaq::database::fhicl;

  std::cout << isDouble("3.14") << "\n";    // true
  std::cout << isDouble("42") << "\n";      // false (integer)
  std::cout << isDouble("1.0e-5") << "\n";  // true
  std::cout << isDouble("hello") << "\n";   // false
}
```

---

### `unescape(str) -> std::string`

**Brief:** Removes escape sequences from a string, converting escaped quotes to regular quotes.

**Parameters:**
- `str` - The string containing escape sequences

**Returns:** A new string with escape sequences converted to their literal characters

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | This function does not throw |

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/DataFormats/Fhicl/helper_functions.h"

void processEscapedString() {
  using namespace artdaq::database::fhicl;

  std::string escaped = "He said \\\"hello\\\"";
  std::string unescaped = unescape(escaped);
  // unescaped == "He said \"hello\""
}
```

---

### `tag_as_string(tag) -> std::string`

**Brief:** Converts a FHiCL value tag enumeration to its string representation for metadata storage.

**Parameters:**
- `tag` - The FHiCL value tag (`::fhicl::value_tag`)

**Returns:** String representation of the tag

| Input Tag | Return Value |
|-----------|--------------|
| `::fhicl::NIL` | `"nil"` |
| `::fhicl::STRING` | `"string"` |
| `::fhicl::BOOL` | `"bool"` |
| `::fhicl::NUMBER` | `"number"` |
| `::fhicl::COMPLEX` | `"complex"` |
| `::fhicl::SEQUENCE` | `"sequence"` |
| `::fhicl::TABLE` | `"table"` |
| `::fhicl::TABLEID` | `"tableid"` |
| default | `"unknown"` |

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | This function does not throw |

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/DataFormats/Fhicl/helper_functions.h"

void storeTypeMetadata() {
  using namespace artdaq::database::fhicl;

  ::fhicl::value_tag tag = ::fhicl::NUMBER;
  std::string type_name = tag_as_string(tag);
  // type_name == "number"

  // Store in metadata JSON object
  metadata["type"] = type_name;
}
```

---

### `string_as_tag(name) -> ::fhicl::value_tag`

**Brief:** Converts a string representation back to a FHiCL value tag enumeration.

**Parameters:**
- `name` - String name of the tag (consumed by move)

**Preconditions:**
- `name` should be a valid tag string (nil, string, bool, number, complex, sequence, table, tableid)

**Returns:** The corresponding `::fhicl::value_tag` enumeration value

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `::fhicl::exception` | When `name` is not a recognized type string |

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/DataFormats/Fhicl/helper_functions.h"

void parseTypeMetadata() {
  using namespace artdaq::database::fhicl;

  std::string type_name = "sequence";

  try {
    ::fhicl::value_tag tag = string_as_tag(type_name);
    // tag == ::fhicl::SEQUENCE
  } catch (const ::fhicl::exception& e) {
    std::cerr << "Unknown type: " << e.what() << "\n";
  }
}
```

---

### `protection_as_string(protection) -> std::string`

**Brief:** Converts a FHiCL protection enumeration to its string representation.

**Parameters:**
- `protection` - The FHiCL protection level (`::fhicl::Protection`)

**Returns:** String representation of the protection level

| Input Protection | Return Value |
|------------------|--------------|
| `::fhicl::Protection::NONE` | `"@none"` |
| `::fhicl::Protection::PROTECT_IGNORE` | `"@protect_ignore"` |
| `::fhicl::Protection::PROTECT_ERROR` | `"@protect_error"` |
| default | `"@none"` |

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | This function does not throw |

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/DataFormats/Fhicl/helper_functions.h"

void storeProtectionMetadata() {
  using namespace artdaq::database::fhicl;

  ::fhicl::Protection prot = ::fhicl::Protection::PROTECT_ERROR;
  std::string prot_str = protection_as_string(prot);
  // prot_str == "@protect_error"
}
```

---

### `string_as_protection(name) -> ::fhicl::Protection`

**Brief:** Converts a string representation back to a FHiCL protection enumeration.

**Parameters:**
- `name` - String name of the protection level (consumed by move)

**Returns:** The corresponding `::fhicl::Protection` enumeration value

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `::fhicl::exception` | When `name` is not a recognized protection string |

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/DataFormats/Fhicl/helper_functions.h"

void parseProtectionMetadata() {
  using namespace artdaq::database::fhicl;

  std::string prot_str = "@protect_ignore";

  try {
    ::fhicl::Protection prot = string_as_protection(prot_str);
    // prot == ::fhicl::Protection::PROTECT_IGNORE
  } catch (const ::fhicl::exception& e) {
    std::cerr << "Unknown protection: " << e.what() << "\n";
  }
}
```

---

### `to_json_string(str) -> std::string`

**Brief:** Escapes a string for safe inclusion in JSON, converting special characters to their escape sequences.

**Parameters:**
- `str` - The raw string to escape

**Returns:** A new string with JSON escape sequences applied

**Escaped Characters:**

| Character | Escape Sequence |
|-----------|-----------------|
| `"` | `\"` |
| `\` | `\\` |
| backspace | `\b` |
| form feed | `\f` |
| newline | `\n` |
| carriage return | `\r` |
| tab | `\t` |

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | This function does not throw |

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/DataFormats/Fhicl/helper_functions.h"

void escapeForJson() {
  using namespace artdaq::database::fhicl;

  std::string raw = "Line 1\nLine 2\tTabbed";
  std::string escaped = to_json_string(raw);
  // escaped == "Line 1\\nLine 2\\tTabbed"
}
```

---

### `from_json_string(str) -> std::string`

**Brief:** Unescapes a JSON string, converting escape sequences back to their literal characters.

**Parameters:**
- `str` - The JSON-escaped string to unescape

**Returns:** A new string with escape sequences converted to literal characters

**Unescaped Sequences:**

| Escape Sequence | Character |
|-----------------|-----------|
| `\\` | `\` |
| `\"` | `"` |
| `\b` | backspace |
| `\f` | form feed |
| `\n` | newline |
| `\r` | carriage return |
| `\t` | tab |

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | This function does not throw |

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/DataFormats/Fhicl/helper_functions.h"

void unescapeFromJson() {
  using namespace artdaq::database::fhicl;

  std::string escaped = "Line 1\\nLine 2\\tTabbed";
  std::string raw = from_json_string(escaped);
  // raw == "Line 1\nLine 2\tTabbed"
}
```

---

### `buffer_rtrim_lines(buffer) -> void`

**Brief:** Trims trailing whitespace from each line in a buffer in-place, ensuring the buffer ends with a newline.

**Parameters:**
- `buffer` - Reference to the string buffer to modify (modified in-place)

**Preconditions:**
- None (empty buffers are handled)

**Postconditions:**
- Each line in `buffer` has trailing spaces, tabs, and carriage returns removed
- `buffer` ends with exactly one newline character

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | This function is marked `noexcept` |

**Thread Safety:** Safe (but caller must ensure exclusive access to buffer)

**Side Effects:**
- Modifies the input buffer in-place

**Complexity:** O(n) where n is the buffer size

**Example:**
```cpp
#include "artdaq-database/DataFormats/Fhicl/helper_functions.h"

void cleanupFhiclBuffer() {
  using namespace artdaq::database::fhicl;

  std::string buffer = "key: value   \n  nested: data\t\r\n";
  buffer_rtrim_lines(buffer);
  // buffer == "key: value\n  nested: data\n"
}
```

## Relationship to Other Components

This header provides utility functions used throughout the FHiCL DataFormats module:

- **convertfhicl2jsondb.h/cpp**: Uses `to_json_string`, `from_json_string`, `tag_as_string`, `string_as_tag`, `protection_as_string`, `string_as_protection`
- **fhicl_reader.h/cpp**: Uses `isDouble` for type detection
- **fhicl_writer.h/cpp**: Uses `buffer_rtrim_lines` for output formatting

## See Also

- [helper_functions.cpp](./helper_functions.cpp.md) - Implementation details
- [convertfhicl2jsondb.h](./convertfhicl2jsondb.h.md) - Primary consumer of these utilities
- [fhicl_types.h](./fhicl_types.h.md) - FHiCL AST type definitions
- [shared_literals.h](../shared_literals.h.md) - Literal string constants

## Notes for Developers

### Common Pitfalls

- **Escape sequence symmetry:** Always use `to_json_string` and `from_json_string` as a pair; mixing with other escape functions may cause double-escaping
- **Tag string case:** Tag strings are lowercase; using "STRING" instead of "string" will cause `string_as_tag` to throw

### Anti-patterns

```cpp
// DON'T: Mix escape functions
std::string bad = from_json_string(unescape(str));  // May miss some escapes

// DO: Use the appropriate function for the context
std::string good = from_json_string(json_str);  // For JSON context
std::string good2 = unescape(fhicl_str);        // For FHiCL context
```
