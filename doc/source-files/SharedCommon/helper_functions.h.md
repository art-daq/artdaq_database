# helper_functions.h

## File Overview

This header file declares a comprehensive set of helper utility functions used throughout the artdaq-database project. It provides functionality for string manipulation, time/timestamp handling, type conversion, JSON generation, environment variable expansion, and debugging assertions.

**Location**: `/home/user/artdaq-database/artdaq-database/SharedCommon/helper_functions.h`

## Dependencies

### Standard Library
- `<cassert>` - Assertion support
- `<chrono>` - Time and duration types
- `<iostream>` - Standard I/O
- `<iterator>` - Iterator utilities
- `<memory>` - Smart pointers
- `<sstream>` - String streams
- `<string>` - String class

### Project Headers
- `"artdaq-database/SharedCommon/shared_datatypes.h"` - Common type definitions

## Namespace: debug

This namespace contains debugging utilities available globally (not in artdaq::database).

### getStackTrace
```cpp
std::string getStackTrace();
```

**Purpose**: Get current call stack trace as a string.

**Returns**: String containing formatted stack trace

**Usage**: Primarily used for debugging and error reporting

---

## Confirm Macro/Function

The `confirm()` function provides runtime assertions with stack trace support:

### Debug Build (NDEBUG not defined)

```cpp
template <typename T>
inline void confirm(std::unique_ptr<T> const& expr);

inline void confirm(bool expr);
```

**Behavior**:
- Checks condition
- If false: prints stack trace to stderr
- Calls `assert()` which terminates program

### Release Build (NDEBUG defined)

```cpp
template <typename T>
inline void confirm(std::unique_ptr<T> const& expr);

inline void confirm(bool expr);
```

**Behavior**:
- Checks condition
- If false: prints stack trace and throws `runtime_exception`

**Usage Example**:
```cpp
confirm(!path.empty());  // Ensure path is not empty
confirm(ptr);            // Ensure pointer is not null
```

**Design Note**: The dual behavior allows detection of assertion failures in release builds instead of undefined behavior.

---

## Namespace: artdaq::database

### Type Definitions

#### quotation_type_t
```cpp
enum class quotation_type_t {
    NONE = 0,
    SINGLE = 1,
    DOUBLE = 2
};
```

**Purpose**: Represents the type of quotes surrounding a string.

**Values**:
- `NONE` - No quotes
- `SINGLE` - Single quotes (')
- `DOUBLE` - Double quotes (")

---

## Time and Timestamp Functions

### timestamp
```cpp
std::string timestamp();
```

**Purpose**: Get current timestamp as ISO 8601 formatted string.

**Returns**: Current time in format `"2017-07-18T12:48:10.123-0500"`

**Usage Example**:
```cpp
auto now = artdaq::database::timestamp();
// Returns: "2025-11-13T10:30:45.123-0600"
```

---

### to_string
```cpp
std::string to_string(system_clock::time_point const& tp);
```

**Purpose**: Convert a chrono time_point to ISO 8601 formatted string.

**Parameters**:
- `tp` - Time point to convert

**Returns**: Formatted timestamp string with milliseconds

**Format**: `YYYY-MM-DDTHH:MM:SS.mmm±ZZZZ`

**Usage Example**:
```cpp
auto tp = std::chrono::system_clock::now();
auto str = artdaq::database::to_string(tp);
```

---

### to_timepoint
```cpp
system_clock::time_point to_timepoint(std::string const& strtime);
```

**Purpose**: Parse ISO 8601 timestamp string to chrono time_point.

**Parameters**:
- `strtime` - Timestamp string to parse

**Returns**: Corresponding time_point

**Throws**: `std::invalid_argument` if format doesn't match

**Usage Example**:
```cpp
auto tp = artdaq::database::to_timepoint("2017-07-18T12:48:10.123-0500");
```

---

### confirm_iso8601_timestamp
```cpp
std::string confirm_iso8601_timestamp(std::string const& strtime);
```

**Purpose**: Validate and convert timestamp to ISO 8601 format (handles legacy formats).

**Parameters**:
- `strtime` - Timestamp string (ISO 8601 or legacy format)

**Returns**: ISO 8601 formatted timestamp string

**Behavior**:
- If already ISO 8601: returns unchanged
- If legacy format: converts to ISO 8601
- Otherwise: throws exception

**Legacy Format**: `"Mon Feb 8 14:00:30 2016"`

---

### useFakeTime
```cpp
bool useFakeTime(bool useFakeTime = false);
```

**Purpose**: Enable/disable fake time mode for testing.

**Parameters**:
- `useFakeTime` - true to enable fake time, false for real time

**Returns**: Current fake time setting

**Behavior**: When enabled, all timestamp functions return a fixed fake time string instead of real time.

**Usage Example**:
```cpp
artdaq::database::useFakeTime(true);  // Enable for testing
auto ts = timestamp();                 // Returns fake time
useFakeTime(false);                    // Disable
```

---

## String Manipulation Functions

### quoted_
```cpp
std::string quoted_(std::string const& text, const char qchar = '\"');
```

**Purpose**: Wrap a string with quotation marks.

**Parameters**:
- `text` - String to quote
- `qchar` - Quote character (' or ")

**Returns**: Quoted string

**Usage Example**:
```cpp
auto s1 = quoted_("hello");       // Returns: "hello"
auto s2 = quoted_("world", '\''); // Returns: 'world'
```

---

### operator"" _quoted
```cpp
std::string operator"" _quoted(const char* text, std::size_t);
```

**Purpose**: String literal operator for automatic double-quoting.

**Returns**: Double-quoted string

**Usage Example**:
```cpp
auto key = "name"_quoted;  // Returns: "\"name\""
```

---

### debrace
```cpp
std::string debrace(std::string s);
```

**Purpose**: Remove surrounding braces from string.

**Parameters**:
- `s` - String potentially surrounded by { }

**Returns**: String without braces (or unchanged if no braces)

**Usage Example**:
```cpp
debrace("{data}");   // Returns: "data"
debrace("data");     // Returns: "data"
```

---

### dequote
```cpp
std::string dequote(std::string s);
```

**Purpose**: Remove surrounding quotes (single or double) from string.

**Parameters**:
- `s` - String potentially quoted

**Returns**: String without quotes (or unchanged if no quotes)

**Usage Example**:
```cpp
dequote("\"text\"");  // Returns: "text"
dequote("'text'");    // Returns: "text"
dequote("text");      // Returns: "text"
```

---

### debracket
```cpp
std::string debracket(std::string s);
```

**Purpose**: Remove surrounding brackets from string.

**Parameters**:
- `s` - String potentially surrounded by [ ]

**Returns**: String without brackets (or unchanged if no brackets)

**Usage Example**:
```cpp
debracket("[array]");  // Returns: "array"
debracket("array");    // Returns: "array"
```

---

### annotate
```cpp
std::string annotate(std::string const& s);
```

**Purpose**: Add '#' prefix to a string (for comments/annotations).

**Parameters**:
- `s` - String to annotate

**Returns**: String with '#' prefix

**Behavior**:
- If already starts with '#': returns unchanged
- If empty: returns empty string
- Otherwise: prepends '#'

**Usage Example**:
```cpp
annotate("comment");   // Returns: "#comment"
annotate("#comment");  // Returns: "#comment"
annotate("");          // Returns: ""
```

---

### quotation_type
```cpp
quotation_type_t quotation_type(std::string text);
```

**Purpose**: Determine what type of quotes surround a string.

**Parameters**:
- `text` - String to check

**Returns**: quotation_type_t enum value

**Usage Example**:
```cpp
quotation_type("\"text\"");  // Returns: quotation_type_t::DOUBLE
quotation_type("'text'");    // Returns: quotation_type_t::SINGLE
quotation_type("text");      // Returns: quotation_type_t::NONE
```

---

### quoted (template)
```cpp
template <quotation_type_t Q>
std::string quoted(std::string const& text);
```

**Purpose**: Quote a string based on compile-time template parameter.

**Template Parameters**:
- `Q` - Quotation type (NONE, SINGLE, or DOUBLE)

**Returns**: Appropriately quoted string

**Usage Example**:
```cpp
auto s1 = quoted<quotation_type_t::DOUBLE>("text");  // "text"
auto s2 = quoted<quotation_type_t::SINGLE>("text");  // 'text'
auto s3 = quoted<quotation_type_t::NONE>("text");    // text
```

---

### trim
```cpp
std::string trim(std::string const& s);
```

**Purpose**: Remove leading and trailing whitespace.

**Parameters**:
- `s` - String to trim

**Returns**: Trimmed string

**Usage Example**:
```cpp
trim("  hello  ");     // Returns: "hello"
trim("\t\ntext\n");    // Returns: "text"
```

---

### to_lower
```cpp
std::string to_lower(std::string const& c);
```

**Purpose**: Convert string to lowercase.

**Parameters**:
- `c` - String to convert

**Returns**: Lowercase string

**Usage Example**:
```cpp
to_lower("HELLO World");  // Returns: "hello world"
```

---

### to_upper
```cpp
std::string to_upper(std::string const& c);
```

**Purpose**: Convert string to uppercase.

**Parameters**:
- `c` - String to convert

**Returns**: Uppercase string

**Usage Example**:
```cpp
to_upper("hello World");  // Returns: "HELLO WORLD"
```

---

### replace_all
```cpp
std::string replace_all(std::string const& source,
                       std::string const& match,
                       std::string const& replacement);
```

**Purpose**: Replace all occurrences of a substring.

**Parameters**:
- `source` - Original string
- `match` - Substring to find
- `replacement` - Replacement string

**Returns**: String with all replacements made

**Usage Example**:
```cpp
replace_all("hello world hello", "hello", "hi");
// Returns: "hi world hi"
```

---

## Boolean and Conversion Functions

### bool_
```cpp
std::string bool_(bool value);
```

**Purpose**: Convert boolean to JSON-compatible string.

**Parameters**:
- `value` - Boolean value

**Returns**: "true" or "false" (as string)

**Usage Example**:
```cpp
bool_(true);   // Returns: "true"
bool_(false);  // Returns: "false"
```

---

## Locale Functions

### set_default_locale
```cpp
void set_default_locale();
```

**Purpose**: Set the C locale to the database's default locale ("en_US.UTF-8").

**Usage**: Call at program startup to ensure consistent string/time handling.

**Usage Example**:
```cpp
int main() {
    artdaq::database::set_default_locale();
    // ... rest of program
}
```

---

## String Comparison Functions

### equal
```cpp
bool equal(std::string const& left, std::string const& right);
```

**Purpose**: Compare two strings for equality.

**Parameters**:
- `left` - First string
- `right` - Second string

**Returns**: true if equal, false otherwise

**Note**: This seems redundant with operator== but may be used for consistency or function pointer contexts.

---

### not_equal
```cpp
bool not_equal(std::string const& left, std::string const& right);
```

**Purpose**: Compare two strings for inequality.

**Parameters**:
- `left` - First string
- `right` - Second string

**Returns**: true if not equal, false otherwise

---

## Environment and System Functions

### expand_environment_variables
```cpp
std::string expand_environment_variables(const std::string& var);
```

**Purpose**: Expand environment variables and shell patterns in a path string.

**Parameters**:
- `var` - String containing environment variables (e.g., "$HOME/data")

**Returns**: Expanded string

**Usage Example**:
```cpp
auto path = expand_environment_variables("$HOME/artdaq/configs");
// Returns: "/home/username/artdaq/configs"
```

---

### unamejson
```cpp
std::string unamejson();
```

**Purpose**: Get system information as JSON string.

**Returns**: JSON object with system information (sysname, nodename, release, version, machine)

**Usage Example**:
```cpp
auto sysinfo = unamejson();
// Returns: {"sysname":"Linux","nodename":"host","release":"4.4.0",...}
```

---

## Object ID Functions

### generate_oid
```cpp
std::string generate_oid();
```

**Purpose**: Generate a unique object ID (24-character hex string).

**Returns**: Unique object ID string

**Implementation**: Uses /proc/sys/kernel/random/uuid

**Usage Example**:
```cpp
auto id = generate_oid();  // Returns: "507f1f77bcf86cd799439011"
```

---

### extract_oid
```cpp
object_id_t extract_oid(std::string const& filter);
```

**Purpose**: Extract object ID from a JSON filter string using regex.

**Parameters**:
- `filter` - JSON string containing object ID

**Returns**: Extracted object ID

**Throws**: std::logic_error or runtime_error if extraction fails

**Usage Example**:
```cpp
auto oid = extract_oid("{\"_id\": \"507f1f77bcf86cd799439011\"}");
// Returns: "507f1f77bcf86cd799439011"
```

---

## JSON Generation Functions

### to_id
```cpp
std::string to_json(std::string const& oid);
```

**Purpose**: Convert object ID to MongoDB-style JSON _id object.

**Parameters**:
- `oid` - Object ID string

**Returns**: JSON string: `{"_id":{"$oid":"<oid>"}}`

**Usage Example**:
```cpp
auto json = to_id("507f1f77bcf86cd799439011");
// Returns: {"_id":{"$oid":"507f1f77bcf86cd799439011"}}
```

---

### to_json
```cpp
std::string to_json(std::string const& key, std::string const& value);
```

**Purpose**: Create simple JSON object with one key-value pair.

**Parameters**:
- `key` - JSON key
- `value` - JSON value

**Returns**: JSON string: `{"key":"value"}`

**Usage Example**:
```cpp
auto json = to_json("name", "component1");
// Returns: {"name":"component1"}
```

---

## Template Functions

### to_csv
```cpp
template <typename T>
std::string to_csv(T const& data);
```

**Purpose**: Convert a container of strings to comma-separated values.

**Template Parameters**:
- `T` - Container type (must have begin(), end(), empty())

**Parameters**:
- `data` - Container of strings

**Returns**: Comma-separated string

**Usage Example**:
```cpp
std::vector<std::string> items = {"apple", "banana", "cherry"};
auto csv = to_csv(items);  // Returns: "apple,banana,cherry"
```

---

### static_cast_as_uint8_t
```cpp
template <typename T>
constexpr std::uint8_t static_cast_as_uint8_t(T const& t);
```

**Purpose**: Safely cast any type to uint8_t at compile time.

**Template Parameters**:
- `T` - Source type

**Parameters**:
- `t` - Value to cast

**Returns**: Value as uint8_t

**Usage Example**:
```cpp
auto byte = static_cast_as_uint8_t(42);
auto flag = static_cast_as_uint8_t(true);
```

---

## Usage Context

These helper functions are used throughout the artdaq-database project for:

1. **Configuration Processing**: String manipulation, quoting, JSON generation
2. **Timestamp Management**: Consistent time formatting across database operations
3. **Error Handling**: Stack traces and assertions
4. **Path Processing**: Environment variable expansion
5. **Data Formatting**: CSV generation, case conversion
6. **System Information**: Collecting system metadata
7. **Testing**: Fake time mode for reproducible tests

## Design Patterns

1. **Namespace Organization**: Global debug functions separate from artdaq::database functions
2. **Template Functions**: Generic algorithms work with any container type
3. **Const Correctness**: Most functions take const references
4. **String Literals**: User-defined literal `_quoted` for convenience
5. **Dual-Mode Assertions**: confirm() behaves differently in debug vs release

## Related Files

- **helper_functions.cpp** - Implementation of these functions
- **shared_datatypes.h** - Type definitions like object_id_t
- **printStackTrace.h** - Stack trace implementation
- **shared_exceptions.h** - Exception types used
