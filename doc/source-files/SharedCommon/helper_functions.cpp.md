# helper_functions.cpp

**Path:** `artdaq-database/SharedCommon/helper_functions.cpp`

**Implements:** [helper_functions.h](./helper_functions.h.md)

**Purpose:** Implements the utility functions declared in `helper_functions.h`. Provides sophisticated time handling with ISO 8601 support, string manipulation, JSON generation, and system information gathering.

## Implementation Overview

This file provides implementations for a wide variety of utility functions used throughout the artdaq-database library. The implementations prioritize clarity and correctness while maintaining reasonable performance. Key implementation decisions include using POSIX functions for environment variable expansion and system information, standard library algorithms for string manipulation, and a careful approach to timestamp handling that supports millisecond precision.

## Key Algorithms

### Timestamp Formatting (`to_string`)

Timestamps use a combination of standard C++ and C time functions:

**Steps:**
1. Convert `time_point` to `time_t` using `std::chrono::system_clock::to_time_t()`
2. Format the base timestamp with `strftime()` using the ISO 8601 format from `apiliteral::timestamp_format`
3. Calculate milliseconds separately by extracting from the time_point's epoch count
4. Insert milliseconds at position 20 in the formatted string (after the seconds)
5. If fake time mode is enabled, return the fixed fake timestamp instead

**Note:** The millisecond handling requires manual string manipulation because `strftime` does not support sub-second precision.

### Timestamp Parsing (`to_timepoint`)

**Steps:**
1. Validate that the input string is not empty
2. Extract the milliseconds substring (positions 20-22)
3. Create a copy with milliseconds zeroed out (replace with "000")
4. Parse the modified string with `strptime()` to get the base time
5. Add the extracted milliseconds back to the resulting `time_point`

### Environment Variable Expansion (`expand_environment_variables`)

Uses POSIX `wordexp()` for comprehensive shell-style expansion:

**Steps:**
1. Call `wordexp()` on the input string
2. Iterate through the resulting word array
3. Concatenate all words with path separators
4. Clean up with `wordfree()`
5. Remove trailing slash if present

**Expands:**
- `$VAR` and `${VAR}` patterns
- Tilde (`~`) to home directory
- Wildcards and glob patterns

### OID Generation (`generate_oid`)

**Steps:**
1. Read a UUID from `/proc/sys/kernel/random/uuid` (Linux-specific)
2. Remove all hyphens from the UUID string
3. Truncate to 24 characters to match MongoDB ObjectID format

### OID Extraction (`extract_oid`)

Uses regex pattern matching to extract object IDs from JSON filter strings:

**Pattern:** `R"(^\{[^:]+:\s+([\s\S]+)\}$)"`

**Steps:**
1. Apply the regex pattern to the input filter
2. Validate that exactly 2 matches are found (full match + capture group)
3. Extract the second match (the captured OID value)
4. Trim trailing whitespace
5. Remove surrounding quotes if present

### String Trimming (`trim`)

Uses iterator-based algorithm for efficient single-pass operation:

**Steps:**
1. Find the first non-whitespace character from the beginning
2. Find the first non-whitespace character from the end
3. Construct a new string from those iterators

## Function Implementations

### `useFakeTime(bool) -> bool`

**Brief:** Manages the fake time mode state using a static variable. Controls whether timestamp functions return real time or a fixed test value.

**Implementation:**
```cpp
bool db::useFakeTime(bool useFakeTime = false) {
  static bool _useFakeTime = useFakeTime;
  return _useFakeTime;
}
```

**Note:** The static variable captures the initial value and returns it for subsequent calls.

**Thread Safety:** Not thread-safe (uses static variable)

---

### `set_default_locale() -> void`

**Brief:** Sets the C locale for consistent string/time handling across the application.

**Implementation:**
```cpp
void db::set_default_locale() {
  std::setlocale(LC_ALL, apiliteral::database_format_locale);
}
```

**Thread Safety:** Not thread-safe (modifies global state)

---

### `timestamp() -> std::string`

**Brief:** Returns current time in ISO 8601 format with milliseconds.

**Implementation:**
```cpp
std::string db::timestamp() {
  auto now = std::chrono::system_clock::now();
  return db::to_string(now);
}
```

**Thread Safety:** Conditional (depends on `useFakeTime` state)

---

### `to_string(time_point) -> std::string`

**Brief:** Converts time_point to ISO 8601 string with millisecond precision.

**Implementation Notes:**
- Uses `strftime` for base formatting
- Manually inserts milliseconds at position 20
- Returns fake time if fake mode is enabled
- Uses conditional compilation for clang vs GCC format specifiers

**Thread Safety:** Conditional (depends on `useFakeTime` state)

---

### `to_timepoint(string) -> time_point`

**Brief:** Parses ISO 8601 string to time_point.

**Throws:** `std::invalid_argument` on empty string or format mismatch

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"
#include <iostream>

void parseAndValidate(const std::string& timestamp) {
    try {
        auto tp = artdaq::database::to_timepoint(timestamp);
        std::cout << "Valid timestamp parsed successfully\n";
    } catch (const std::invalid_argument& e) {
        std::cerr << "Parse error: " << e.what() << "\n";
    }
}
```

---

### `confirm_iso8601_timestamp(string) -> std::string`

**Brief:** Validates and normalizes timestamps to ISO 8601 format.

**Implementation Notes:**
- Strings starting with '2' are assumed to be ISO 8601 and returned as-is
- Other formats are parsed using the legacy format and converted

**Thread Safety:** Safe

---

### `unamejson() -> std::string`

**Brief:** Returns system information as JSON.

**Implementation:**
```cpp
std::string db::unamejson() {
  struct utsname thisuname;
  if (uname(&thisuname) == -1) {
    return "{}";
  }
  // Builds JSON with sysname, nodename, release, version, machine
}
```

**Thread Safety:** Safe

---

### `quoted_(text, qchar) -> std::string`

**Brief:** Wraps text with specified quote character.

**Implementation:**
```cpp
std::string db::quoted_(std::string const& text, const char qchar) {
  confirm((qchar == '\"' || qchar == '\''));
  return std::string{} + qchar + text + qchar;
}
```

**Thread Safety:** Safe

---

### `bool_(value) -> std::string`

**Brief:** Converts boolean to string literal.

**Implementation:**
```cpp
std::string db::bool_(bool value) {
  return (value ? "true" : "false");
}
```

**Thread Safety:** Safe

---

### `operator"" _quoted(text, size) -> std::string`

**Brief:** User-defined literal for double-quoted strings.

**Implementation:**
```cpp
std::string db::operator"" _quoted(const char* text, std::size_t) {
  return "\"" + std::string(text) + "\"";
}
```

**Thread Safety:** Safe

---

### `debrace(s) -> std::string`

**Brief:** Removes surrounding braces.

**Implementation:**
```cpp
std::string db::debrace(std::string s) {
  if (s[0] == '{' && s[s.length() - 1] == '}') {
    return s.substr(1, s.length() - 2);
  }
  return s;
}
```

**Thread Safety:** Safe

---

### `quotation_type(s) -> quotation_type_t`

**Brief:** Determines the quotation type of a string.

**Implementation:** Checks first and last characters for matching quotes.

**Thread Safety:** Safe

---

### `dequote(s) -> std::string`

**Brief:** Removes surrounding quotes (single or double).

**Implementation:** Similar to `debrace`, checks for matching quote characters.

**Thread Safety:** Safe

---

### `debracket(s) -> std::string`

**Brief:** Removes surrounding brackets.

**Implementation:** Similar to `debrace`, checks for `[` and `]`.

**Thread Safety:** Safe

---

### `annotate(s) -> std::string`

**Brief:** Prepends `#` to create a comment annotation.

**Implementation:**
- Trims input first
- Returns null string literal for empty input
- Preserves existing `#` prefix
- Prepends `#` for other strings

**Thread Safety:** Safe

---

### `generate_oid() -> std::string`

**Brief:** Generates unique 24-character hex ID.

**Implementation:**
```cpp
std::string db::generate_oid() {
  std::ifstream is("/proc/sys/kernel/random/uuid");
  std::string oid((std::istreambuf_iterator<char>(is)),
                   std::istreambuf_iterator<char>());
  oid.erase(std::remove(oid.begin(), oid.end(), '-'), oid.end());
  oid.resize(24);
  return oid;
}
```

**Thread Safety:** Safe

---

### `to_id(oid) -> std::string`

**Brief:** Creates MongoDB-style ID JSON structure.

**Implementation:** Uses `_quoted` literal and `quoted_()` to build the JSON structure.

**Thread Safety:** Safe

---

### `to_json(key, value) -> std::string`

**Brief:** Creates simple JSON key-value object.

**Implementation:**
```cpp
std::string db::to_json(std::string const& key, std::string const& value) {
  confirm(!key.empty());
  confirm(!value.empty());
  std::ostringstream oss;
  oss << "{" << quoted_(key) << ":" << quoted_(value) << "}";
  return oss.str();
}
```

**Thread Safety:** Safe

---

### `expand_environment_variables(var) -> std::string`

**Brief:** Expands shell-style environment variables using POSIX `wordexp()`.

**Security Note:** `wordexp()` can execute shell commands - sanitize inputs in security-sensitive contexts.

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"
#include <iostream>

void resolvePath() {
    auto path = artdaq::database::expand_environment_variables("$HOME/configs");
    std::cout << "Resolved path: " << path << "\n";
}
```

---

### `equal(left, right) -> bool`

**Brief:** String equality comparison with assertion checking.

**Implementation:**
```cpp
bool db::equal(std::string const& left, std::string const& right) {
  confirm(!left.empty());
  confirm(!right.empty());
  return left == right;
}
```

**Thread Safety:** Safe

---

### `not_equal(left, right) -> bool`

**Brief:** String inequality comparison.

**Implementation:** Returns `!equal(left, right)`.

**Thread Safety:** Safe

---

### `extract_oid(filter) -> object_id_t`

**Brief:** Extracts OID from JSON filter using regex.

**Regex Pattern:** `R"(^\{[^:]+:\s+([\s\S]+)\}$)"`

**Throws:**
- `std::logic_error` on regex search failure
- `runtime_error` on unexpected match count

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"
#include <iostream>

void processFilter(const std::string& filter) {
    try {
        auto oid = artdaq::database::extract_oid(filter);
        std::cout << "Extracted OID: " << oid << "\n";
    } catch (const std::logic_error& e) {
        std::cerr << "Regex failed: " << e.what() << "\n";
    } catch (const artdaq::database::runtime_error& e) {
        std::cerr << "Extraction failed: " << e.what() << "\n";
    }
}
```

---

### `trim(s) -> std::string`

**Brief:** Removes leading and trailing whitespace.

**Implementation:** Uses `find_if_not` with `std::isspace` predicate from both ends.

**Thread Safety:** Safe

---

### `to_lower(c) -> std::string`

**Brief:** Converts string to lowercase.

**Implementation:**
```cpp
std::string db::to_lower(std::string const& c) {
  auto s = c;
  std::transform(s.begin(), s.end(), s.begin(), ::tolower);
  return s;
}
```

**Thread Safety:** Safe

---

### `to_upper(c) -> std::string`

**Brief:** Converts string to uppercase.

**Implementation:** Similar to `to_lower`, uses `::toupper`.

**Thread Safety:** Safe

---

### `replace_all(source, match, replacement) -> std::string`

**Brief:** Replaces all occurrences of a substring.

**Implementation:**
- Uses efficient single-pass algorithm
- Pre-allocates memory based on match length
- Uses `find()` to locate matches and `append()` to build result

**Thread Safety:** Safe

---

### Template Specializations

```cpp
template <>
std::string quoted<quotation_type_t::NONE>(std::string const& text) {
  return text;
}

template <>
std::string quoted<quotation_type_t::SINGLE>(std::string const& text) {
  return quoted_(text, '\'');
}
```

**Brief:** Template specializations for the `quoted` function template, providing specific behavior for `NONE` and `SINGLE` quotation types.

**Thread Safety:** Safe

## Performance Considerations

- `replace_all()` pre-allocates memory for efficiency
- `trim()` uses iterators for single-pass operation
- `generate_oid()` reads from `/proc` filesystem (I/O overhead)
- Timestamp formatting involves multiple string operations

## Error Handling Strategy

Functions use `confirm()` assertions for precondition checking on inputs like empty strings or null pointers. Some functions throw specific exceptions:
- `std::invalid_argument` for format errors in timestamp parsing
- `std::logic_error` and `runtime_error` for regex failures

## Dependencies

| Include | Purpose |
|---------|---------|
| `<sys/utsname.h>` | System info (`uname()`) |
| `<wordexp.h>` | Shell variable expansion |
| `<chrono>` | Time utilities |
| `<clocale>` | Locale support |
| `<ctime>` | Time formatting (`strftime`, `strptime`) |
| `<fstream>` | File I/O for OID generation |
| `<regex>` | OID extraction |
| `helper_functions.h` | Declarations |
| `common.h` | TRACE logging macros |
| `configuraion_api_literals.h` | Timestamp formats, locale constants |
| `shared_exceptions.h` | Exception types |

## TRACE Configuration

The file defines `TRACE_NAME` as `"helper_functions.cpp"`. TRACE logging is used in:
- `generate_oid()` - Entry/exit logging at levels 11-12
- `extract_oid()` - Regex result logging at levels 13-15

## Thread Safety

Most functions are thread-safe as they use only local variables and const parameters. Exceptions:
- `useFakeTime()` uses a static variable (not thread-safe for concurrent mode changes)
- `set_default_locale()` modifies global state

## Security Note

`expand_environment_variables()` uses `wordexp()` which can execute shell commands embedded in the input string. Sanitize inputs when using in security-sensitive contexts.

## Testing Notes

- **Unit tests:** Functions are tested indirectly through ConfigurationDB tests
- **Key test cases:** Timestamp formatting/parsing, string manipulation, OID generation

## See Also

- [helper_functions.h](./helper_functions.h.md) - Function declarations
- [configuraion_api_literals.h](./configuraion_api_literals.h.md) - Timestamp format strings, locale constants
- [printStackTrace.h](./printStackTrace.h.md) - Stack trace used by `confirm()`
