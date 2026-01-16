# helper_functions.h

**Path:** `artdaq-database/SharedCommon/helper_functions.h`

**Purpose:** Comprehensive utility functions for string manipulation, timestamp handling, JSON generation, environment variable expansion, and debugging assertions. These functions are used throughout artdaq-database for configuration processing, data formatting, and error handling.

## Key Concepts

### confirm() Assertion

A runtime assertion that works in both debug and release builds:
- **Debug build**: Prints stack trace and calls `assert()` (terminates)
- **Release build**: Prints stack trace and throws `runtime_exception`

```cpp
confirm(!path.empty());  // Ensure path is not empty
confirm(ptr);            // Ensure pointer is not null
```

### Fake Time Mode

For testing, timestamps can return a fixed value instead of real time:
```cpp
useFakeTime(true);       // Enable fake time
auto ts = timestamp();   // Returns fixed test timestamp
useFakeTime(false);      // Return to real time
```

### Quotation Type Enum

Used to identify the type of quotation around a string:
```cpp
enum class quotation_type_t { NONE = 0, SINGLE = 1, DOUBLE };
```

## Thread Safety

- **Thread-safe:** No (static state in `useFakeTime`)
- **Notes:** The `useFakeTime` function uses a static variable that is not thread-safe. All other functions are thread-safe as they use only local variables and const parameters.

## Dependencies

| Include | Purpose |
|---------|---------|
| `<cassert>` | Debug assertions |
| `<chrono>` | Time types (`system_clock`, `time_point`) |
| `<iostream>` | Error output for failed assertions |
| `<iterator>` | `ostream_iterator` for `to_csv` |
| `<memory>` | `unique_ptr` for `confirm` overload |
| `<sstream>` | String streams for building strings |
| `<string>` | String class |
| `shared_datatypes.h` | Type definitions (`object_id_t`) |

## Functions

### Assertion Functions

#### `confirm(std::unique_ptr<T> const& expr) -> void`

**Brief:** Runtime assertion that checks if a unique_ptr is not null. In debug builds, terminates on failure; in release builds, throws an exception.

**Parameters:**
- `expr` - Unique pointer to check for null

**Preconditions:**
- None

**Postconditions:**
- If the function returns, `expr` is guaranteed to be non-null

**Behavior:**
- If `expr` is null, prints stack trace to stderr
- Debug build: calls `assert(expr)`
- Release build: throws `runtime_exception`

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_exception` | When `expr` is null (release build only) |

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"
#include <memory>

void processDocument(std::unique_ptr<Document> doc) {
    confirm(doc);  // Terminates/throws if doc is null
    // Safe to use doc here
    doc->process();
}
```

---

#### `confirm(bool expr) -> void`

**Brief:** Runtime assertion that checks if a boolean expression is true. In debug builds, terminates on failure; in release builds, throws an exception.

**Parameters:**
- `expr` - Boolean expression to verify

**Preconditions:**
- None

**Postconditions:**
- If the function returns, `expr` was true

**Behavior:**
- If `expr` is false, prints stack trace to stderr
- Debug build: calls `assert(expr)`
- Release build: throws `runtime_exception`

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_exception` | When `expr` is false (release build only) |

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"

void loadConfiguration(const std::string& filename) {
    confirm(!filename.empty());  // Ensure filename is not empty
    confirm(filename.size() < 256);  // Ensure reasonable length
    // Safe to proceed with loading
}
```

---

### Timestamp Functions

#### `timestamp() -> std::string`

**Brief:** Returns the current time as an ISO 8601 formatted string with millisecond precision.

**Returns:** ISO 8601 timestamp string (e.g., `"2024-01-15T14:30:25.123-0500"`)

**Thread Safety:** Conditional (depends on `useFakeTime` state; fake time mode is not thread-safe)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"
#include <iostream>

void logEvent(const std::string& message) {
    std::cout << "[" << artdaq::database::timestamp() << "] " << message << "\n";
    // Output: [2024-01-15T14:30:25.123-0500] message
}
```

---

#### `to_string(system_clock::time_point const& tp) -> std::string`

**Brief:** Converts a `std::chrono::system_clock::time_point` to an ISO 8601 formatted string with millisecond precision.

**Parameters:**
- `tp` - Time point to convert

**Returns:** ISO 8601 timestamp string with millisecond precision

**Thread Safety:** Conditional (depends on `useFakeTime` state)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"
#include <chrono>

void recordEventTime() {
    auto now = std::chrono::system_clock::now();
    auto str = artdaq::database::to_string(now);
    // str contains formatted timestamp
}
```

---

#### `to_timepoint(std::string const& strtime) -> system_clock::time_point`

**Brief:** Parses an ISO 8601 timestamp string into a `time_point`. Throws on invalid input.

**Parameters:**
- `strtime` - ISO 8601 formatted timestamp string

**Preconditions:**
- `strtime` must not be empty
- `strtime` must be in a recognized format

**Returns:** Corresponding `system_clock::time_point`

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::invalid_argument` | Empty string or format mismatch |

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"
#include <iostream>

void parseTimestamp(const std::string& ts) {
    try {
        auto tp = artdaq::database::to_timepoint(ts);
        // Use tp for time comparisons
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid timestamp: " << e.what() << "\n";
    }
}
```

---

#### `confirm_iso8601_timestamp(std::string const& strtime) -> std::string`

**Brief:** Validates and normalizes a timestamp string to ISO 8601 format, converting from legacy format if necessary.

**Parameters:**
- `strtime` - Timestamp string to validate/convert

**Preconditions:**
- `strtime` must not be empty

**Returns:** ISO 8601 formatted timestamp string

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::invalid_argument` | Empty string or unrecognized format |

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"

std::string normalizeTimestamp(const std::string& ts) {
    try {
        return artdaq::database::confirm_iso8601_timestamp(ts);
    } catch (const std::invalid_argument& e) {
        // Handle invalid format
        return "";
    }
}
```

---

#### `useFakeTime(bool) -> bool`

**Brief:** Enables or disables fake time mode for testing purposes. When enabled, `timestamp()` returns a fixed test timestamp instead of the real current time.

**Parameters:**
- `useFakeTime` - `true` to enable fake time, `false` to use real time (default: `false`)

**Returns:** Previous state of fake time mode

**Thread Safety:** Not thread-safe (uses static variable)

**Side Effects:**
- Modifies global fake time state

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"
#include <cassert>

void testTimestampConsistency() {
    artdaq::database::useFakeTime(true);   // Enable fake time
    auto ts1 = artdaq::database::timestamp();
    auto ts2 = artdaq::database::timestamp();
    assert(ts1 == ts2);  // Same fixed timestamp
    artdaq::database::useFakeTime(false);  // Restore real time
}
```

---

### String Manipulation Functions

#### `trim(std::string const& s) -> std::string`

**Brief:** Removes leading and trailing whitespace from a string.

**Parameters:**
- `s` - String to trim

**Returns:** String with leading/trailing whitespace removed

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"

auto result = artdaq::database::trim("  hello world  ");
// result == "hello world"
```

---

#### `to_lower(std::string const& c) -> std::string`

**Brief:** Converts all characters in a string to lowercase.

**Parameters:**
- `c` - String to convert

**Returns:** Lowercase copy of the string

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"

auto result = artdaq::database::to_lower("Hello World");
// result == "hello world"
```

---

#### `to_upper(std::string const& c) -> std::string`

**Brief:** Converts all characters in a string to uppercase.

**Parameters:**
- `c` - String to convert

**Returns:** Uppercase copy of the string

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"

auto result = artdaq::database::to_upper("Hello World");
// result == "HELLO WORLD"
```

---

#### `replace_all(std::string const& source, std::string const& match, std::string const& replacement) -> std::string`

**Brief:** Replaces all occurrences of a substring with another substring.

**Parameters:**
- `source` - Source string to search in
- `match` - Substring to find
- `replacement` - Substring to replace matches with

**Returns:** New string with all occurrences replaced

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"

auto result = artdaq::database::replace_all("foo bar foo", "foo", "baz");
// result == "baz bar baz"
```

---

#### `quoted_(std::string const& text, const char qchar = '"') -> std::string`

**Brief:** Wraps a string with quote characters.

**Parameters:**
- `text` - String to quote
- `qchar` - Quote character to use (default: double quote `"`)

**Preconditions:**
- `qchar` must be either `'"'` or `'\''`

**Returns:** String wrapped in quote characters

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"

auto result = artdaq::database::quoted_("hello");
// result == "\"hello\""

auto single = artdaq::database::quoted_("hello", '\'');
// single == "'hello'"
```

---

#### `operator"" _quoted(const char* text, std::size_t) -> std::string`

**Brief:** User-defined string literal that wraps text in double quotes. Provides convenient syntax for creating JSON keys.

**Parameters:**
- `text` - String literal to quote

**Returns:** String wrapped in double quotes

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"
using namespace artdaq::database;

auto key = "name"_quoted;
// key == "\"name\""
```

---

#### `dequote(std::string s) -> std::string`

**Brief:** Removes surrounding quotes (single or double) from a string if present.

**Parameters:**
- `s` - String to dequote

**Returns:** String with surrounding quotes removed, or original string if not quoted

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"

auto result = artdaq::database::dequote("\"hello\"");  // "hello"
auto result2 = artdaq::database::dequote("'world'");   // "world"
auto result3 = artdaq::database::dequote("noquotes");  // "noquotes"
```

---

#### `debrace(std::string s) -> std::string`

**Brief:** Removes surrounding braces `{ }` from a string if present.

**Parameters:**
- `s` - String to debrace

**Returns:** String with surrounding braces removed, or original string if not braced

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"

auto result = artdaq::database::debrace("{content}");
// result == "content"
```

---

#### `debracket(std::string s) -> std::string`

**Brief:** Removes surrounding brackets `[ ]` from a string if present.

**Parameters:**
- `s` - String to debracket

**Returns:** String with surrounding brackets removed, or original string if not bracketed

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"

auto result = artdaq::database::debracket("[content]");
// result == "content"
```

---

#### `annotate(std::string const& s) -> std::string`

**Brief:** Prepends a `#` character to create a comment annotation, handling empty strings and existing annotations.

**Parameters:**
- `s` - String to annotate

**Returns:** Annotated string with `#` prefix, null string literal if empty, or unchanged if already annotated

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"

auto result = artdaq::database::annotate("This is a comment");
// result == "#This is a comment"

auto result2 = artdaq::database::annotate("#Already annotated");
// result2 == "#Already annotated" (unchanged)
```

---

#### `quotation_type(std::string s) -> quotation_type_t`

**Brief:** Determines the type of quotation marks surrounding a string.

**Parameters:**
- `s` - String to analyze

**Returns:** `quotation_type_t::DOUBLE`, `quotation_type_t::SINGLE`, or `quotation_type_t::NONE`

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"

auto type1 = artdaq::database::quotation_type("\"hello\"");  // DOUBLE
auto type2 = artdaq::database::quotation_type("'hello'");    // SINGLE
auto type3 = artdaq::database::quotation_type("hello");      // NONE
```

---

#### `quoted<quotation_type_t Q>(std::string const& text) -> std::string`

**Brief:** Template function that wraps text with the specified quotation type.

**Template Parameters:**
- `Q` - Quotation type (`NONE`, `SINGLE`, or `DOUBLE`)

**Parameters:**
- `text` - String to quote

**Returns:** Quoted string according to template parameter

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"
using namespace artdaq::database;

auto double_quoted = quoted<quotation_type_t::DOUBLE>("hello");  // "\"hello\""
auto single_quoted = quoted<quotation_type_t::SINGLE>("hello");  // "'hello'"
auto unquoted = quoted<quotation_type_t::NONE>("hello");         // "hello"
```

---

### Comparison Functions

#### `equal(std::string const& left, std::string const& right) -> bool`

**Brief:** Compares two strings for equality with assertion checking. Asserts that both strings are non-empty.

**Parameters:**
- `left` - First string
- `right` - Second string

**Preconditions:**
- `left` must not be empty
- `right` must not be empty

**Returns:** `true` if strings are equal, `false` otherwise

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"

if (artdaq::database::equal(name, "expected")) {
    // strings match
}
```

---

#### `not_equal(std::string const& left, std::string const& right) -> bool`

**Brief:** Compares two strings for inequality.

**Parameters:**
- `left` - First string
- `right` - Second string

**Returns:** `true` if strings are not equal, `false` if equal

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"

if (artdaq::database::not_equal(name, "forbidden")) {
    // strings don't match
}
```

---

### JSON Generation Functions

#### `to_json(std::string const& key, std::string const& value) -> std::string`

**Brief:** Creates a simple JSON object with a single key-value pair.

**Parameters:**
- `key` - JSON object key
- `value` - JSON object value

**Preconditions:**
- `key` must not be empty
- `value` must not be empty

**Returns:** JSON object string `{"key":"value"}`

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"

auto json = artdaq::database::to_json("name", "test_configuration");
// json == "{\"name\":\"test_configuration\"}"
```

---

#### `to_id(std::string const& oid) -> std::string`

**Brief:** Creates a MongoDB-style document ID JSON structure.

**Parameters:**
- `oid` - Object ID string (24-character hex)

**Preconditions:**
- `oid` must not be empty

**Returns:** MongoDB-style ID JSON: `{"_id":{"$oid":"..."}}`

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"

auto id = artdaq::database::to_id("507f1f77bcf86cd799439011");
// id == "{\"_id\":{\"$oid\":\"507f1f77bcf86cd799439011\"} }"
```

---

#### `generate_oid() -> std::string`

**Brief:** Generates a unique 24-character hexadecimal object ID using the system's random UUID generator.

**Returns:** 24-character hexadecimal string

**Thread Safety:** Safe

**Side Effects:**
- Reads from `/proc/sys/kernel/random/uuid` (Linux-specific)

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"

auto oid = artdaq::database::generate_oid();
// oid is something like "507f1f77bcf86cd7994390"
```

---

### Environment Functions

#### `expand_environment_variables(const std::string& var) -> std::string`

**Brief:** Expands shell-style environment variables (`$VAR` and `${VAR}`) in a string using POSIX `wordexp()`.

**Parameters:**
- `var` - String containing environment variable references

**Returns:** String with environment variables expanded

**Thread Safety:** Safe

**Side Effects:**
- Reads environment variables

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"

auto path = artdaq::database::expand_environment_variables("$HOME/data");
// path == "/home/username/data"

auto uri = artdaq::database::expand_environment_variables("${ARTDAQ_DATABASE_URI}");
// uri == value of ARTDAQ_DATABASE_URI environment variable
```

---

#### `unamejson() -> std::string`

**Brief:** Returns system information (from `uname`) as a JSON object. Provides hostname, kernel version, and architecture details.

**Returns:** JSON object with system info, or `"{}"` on error

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"

auto info = artdaq::database::unamejson();
// info == {"sysname":"Linux","nodename":"host","release":"5.14.0",...}
```

---

#### `set_default_locale() -> void`

**Brief:** Sets the C locale to `"en_US.UTF-8"` for consistent string and time handling across the application.

**Thread Safety:** Not thread-safe (modifies global locale)

**Side Effects:**
- Modifies global locale settings

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"

int main() {
    artdaq::database::set_default_locale();  // Call early in main()
    // ... rest of application
}
```

---

### Utility Functions

#### `bool_(bool value) -> std::string`

**Brief:** Converts a boolean value to its string representation (`"true"` or `"false"`).

**Parameters:**
- `value` - Boolean value to convert

**Returns:** `"true"` or `"false"`

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"

auto str = artdaq::database::bool_(true);   // "true"
auto str2 = artdaq::database::bool_(false); // "false"
```

---

#### `extract_oid(std::string const& filter) -> object_id_t`

**Brief:** Extracts an object ID from a JSON filter string using regex parsing.

**Parameters:**
- `filter` - JSON filter string containing an object ID

**Returns:** Extracted object ID string

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::logic_error` | Regex search failed |
| `runtime_error` | Unexpected number of regex matches |

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"
#include <iostream>

void findDocument(const std::string& filter) {
    try {
        auto oid = artdaq::database::extract_oid(filter);
        std::cout << "Found OID: " << oid << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Failed to extract OID: " << e.what() << "\n";
    }
}
```

---

#### `to_csv<T>(T const& data) -> std::string`

**Brief:** Template function that converts a container of strings to a comma-separated values string.

**Template Parameters:**
- `T` - Container type (must support `begin()`, `end()`, `empty()`)

**Parameters:**
- `data` - Container of strings

**Returns:** Comma-separated string, or empty string if container is empty

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"
#include <vector>

std::vector<std::string> items = {"apple", "banana", "cherry"};
auto csv = artdaq::database::to_csv(items);
// csv == "apple,banana,cherry"
```

---

#### `static_cast_as_uint8_t<T>(T const& t) -> std::uint8_t`

**Brief:** Constexpr template function for safe casting to `uint8_t`.

**Template Parameters:**
- `T` - Source type

**Parameters:**
- `t` - Value to cast

**Returns:** Value cast to `std::uint8_t`

**Thread Safety:** Safe

**Example:**
```cpp
#include "artdaq-database/SharedCommon/helper_functions.h"

auto byte = artdaq::database::static_cast_as_uint8_t(65);
// byte == 65 as uint8_t
```

---

## Relationship to Other Components

- [helper_functions.cpp](./helper_functions.cpp.md) - Implementation file
- [shared_datatypes.h](./shared_datatypes.h.md) - Provides `object_id_t`, `timestamp_t`
- [printStackTrace.h](./printStackTrace.h.md) - Provides `getStackTrace()` used by `confirm()`
- [shared_exceptions.h](./shared_exceptions.h.md) - Exception types for release-build `confirm()`
- Used by ConfigurationDB, StorageProviders, and Utilities modules

## See Also

- [helper_functions.cpp](./helper_functions.cpp.md) - Implementation details
- [configuraion_api_literals.h](./configuraion_api_literals.h.md) - Timestamp format constants
