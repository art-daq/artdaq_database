# helper_functions.cpp

## File Overview

This implementation file provides the actual implementations of the helper utility functions declared in `helper_functions.h`. It includes sophisticated time handling, string manipulation, JSON generation, and system information gathering capabilities.

**Location**: `/home/user/artdaq-database/artdaq-database/SharedCommon/helper_functions.cpp`

## Dependencies

### System Headers
- `<sys/utsname.h>` - Unix system name structure (uname)
- `<wordexp.h>` - POSIX word expansion (shell variable expansion)
- `<chrono>` - Time utilities
- `<clocale>` - Locale support
- `<ctime>` - C time utilities
- `<fstream>` - File streams
- `<regex>` - Regular expressions

### Project Headers
- `"artdaq-database/SharedCommon/helper_functions.h"` - Function declarations
- `"artdaq-database/SharedCommon/common.h"` - Common includes
- `"artdaq-database/SharedCommon/configuraion_api_literals.h"` - String literals
- `"artdaq-database/SharedCommon/shared_exceptions.h"` - Exception types

## TRACE Configuration

```cpp
#define TRACE_NAME "helper_functions.cpp"
```

## Namespace Aliases

```cpp
namespace db = artdaq::database;
using namespace artdaq::database;
namespace apiliteral = db::configapi::literal;
```

## Function Implementations

### useFakeTime

```cpp
bool db::useFakeTime(bool useFakeTime)
```

**Implementation Details**:
- Uses static local variable to maintain state
- Default parameter is false
- State persists across function calls

**Implementation**:
```cpp
static bool _useFakeTime = useFakeTime;
return _useFakeTime;
```

**Usage Pattern**:
```cpp
// Set fake time mode
useFakeTime(true);

// Query current mode
bool isFake = useFakeTime();
```

**Purpose**: Enables deterministic timestamps for testing and debugging.

---

### set_default_locale

```cpp
void db::set_default_locale()
```

**Implementation**:
```cpp
std::setlocale(LC_ALL, apiliteral::database_format_locale);
```

**Sets**: `LC_ALL` to `"en_US.UTF-8"`

**Purpose**: Ensures consistent string formatting, time parsing, and character handling across different systems.

**Important**: Should be called early in program initialization before any locale-dependent operations.

---

### timestamp

```cpp
std::string db::timestamp()
```

**Implementation**:
```cpp
auto now = std::chrono::system_clock::now();
return db::to_string(now);
```

**Returns**: Current time in ISO 8601 format with milliseconds

**Example Output**: `"2025-11-13T10:30:45.123-0600"`

---

### to_string (time_point)

```cpp
std::string db::to_string(std::chrono::system_clock::time_point const& tp)
```

**Implementation Details**:
- Converts time_point to time_t
- Formats using strftime with custom format
- Extracts milliseconds separately
- Inserts milliseconds into formatted string
- Handles compiler differences (GCC vs Clang)

**Algorithm**:
1. Convert time_point to time_t
2. Format base timestamp using strftime: `"%FT%T.000%z"`
3. Extract milliseconds from time_point
4. Format milliseconds with snprintf (positions 30-34 in buffer)
5. Copy 3 digits of milliseconds to position 20 in main buffer
6. Return fake time if fake mode enabled
7. Otherwise return formatted timestamp

**Format String**: `"%FT%T.000%z"` produces `"YYYY-MM-DDTHH:MM:SS.000±ZZZZ"`

**Compiler-Specific Code**:
```cpp
#ifndef __clang__
#define FORMAT_DURATION_MILLISECONDS "%03ld"
#else
#define FORMAT_DURATION_MILLISECONDS "%03lld"
#endif
```
This handles differences in long integer format specifiers between GCC and Clang.

**GCC Warning Suppression**:
```cpp
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
strncpy(buff + 20, buff + 30, 3);
#pragma GCC diagnostic pop
```
Suppresses warning about intentional partial string copy.

---

### to_timepoint

```cpp
std::chrono::system_clock::time_point db::to_timepoint(std::string const& strtime)
```

**Implementation Details**:
- Parses ISO 8601 timestamp string
- Extracts milliseconds separately
- Reconstructs time_point with millisecond precision

**Algorithm**:
1. Validate input is not empty
2. Parse milliseconds from substring (positions 20-22)
3. Zero out milliseconds in copy of input string
4. Parse timestamp using strptime with format string
5. Convert tm struct to time_point
6. Add milliseconds to time_point
7. Return result or throw on parse failure

**Implementation**:
```cpp
auto timeinfo = std::tm();
auto milliseconds = std::chrono::milliseconds(atoi(strtime.substr(20, 3).c_str()));

auto tmptime = strtime;
tmptime.at(20) = '0';
tmptime.at(21) = '0';
tmptime.at(22) = '0';

if (strptime(tmptime.c_str(), apiliteral::timestamp_format, &timeinfo) != nullptr) {
    timeinfo.tm_isdst = -1;  // Let system determine DST
    return std::chrono::system_clock::from_time_t(std::mktime(&timeinfo)) + milliseconds;
}

throw std::invalid_argument(...);
```

**Error Handling**: Throws `std::invalid_argument` with detailed format/input info if parsing fails.

---

### confirm_iso8601_timestamp

```cpp
std::string db::confirm_iso8601_timestamp(std::string const& strtime)
```

**Implementation Details**:
- Handles both new ISO 8601 and legacy timestamp formats
- Converts legacy format to ISO 8601

**Algorithm**:
1. Validate input is not empty
2. Check if already ISO 8601 (starts with '2')
3. If yes: return unchanged
4. Attempt to parse as legacy format (`"%a %b %d %H:%M:%S %Y"`)
5. If successful: convert to ISO 8601 format
6. Otherwise: throw exception

**Legacy Format Example**: `"Mon Feb 8 14:00:30 2016"`

**Conversion**:
```cpp
auto timeinfo = std::tm();
if (strptime(strtime.c_str(), apiliteral::timestamp_format_old, &timeinfo) != nullptr) {
    timeinfo.tm_isdst = -1;
    return db::to_string(std::chrono::system_clock::from_time_t(std::mktime(&timeinfo)));
}
```

**Use Case**: Database migration from old timestamp format to new ISO 8601 format.

---

### unamejson

```cpp
std::string db::unamejson()
```

**Implementation Details**:
- Calls Unix uname() system call
- Formats result as JSON object
- Returns empty JSON on failure

**Algorithm**:
1. Create utsname structure
2. Call uname() system call
3. If fails: return "{}"
4. Build JSON with system info fields
5. Return JSON string

**Implementation**:
```cpp
struct utsname thisuname;
if (uname(&thisuname) == -1) {
    return "{}";
}

std::ostringstream oss;
oss << "{";
oss << "sysname"_quoted << ":" << quoted_(thisuname.sysname) << ",";
oss << "nodename"_quoted << ":" << quoted_(thisuname.nodename) << ",";
oss << "release"_quoted << ":" << quoted_(thisuname.release) << ",";
oss << "version"_quoted << ":" << quoted_(thisuname.version) << ",";
oss << "machine"_quoted << ":" << quoted_(thisuname.machine);
oss << "}";
return oss.str();
```

**Example Output**:
```json
{
  "sysname": "Linux",
  "nodename": "hostname.domain.com",
  "release": "4.4.0",
  "version": "#1 SMP Tue Nov 12 2024",
  "machine": "x86_64"
}
```

---

### quoted_

```cpp
std::string db::quoted_(std::string const& text, const char qchar)
```

**Implementation**:
```cpp
confirm((qchar == '\"' || qchar == '\''));
return std::string{} + qchar + text + qchar;
```

**Validation**: Confirms quote character is either double or single quote.

**Usage Example**:
```cpp
quoted_("hello", '\"');  // Returns: "hello"
quoted_("world", '\'');  // Returns: 'world'
```

---

### bool_

```cpp
std::string db::bool_(bool value)
```

**Implementation**:
```cpp
return (value ? "true" : "false");
```

**Purpose**: Convert C++ bool to JSON-compatible string literal.

---

### operator"" _quoted

```cpp
std::string db::operator"" _quoted(const char* text, std::size_t)
```

**Implementation**:
```cpp
return "\"" + std::string(text) + "\"";
```

**Usage**: String literal suffix for automatic quoting.

**Example**:
```cpp
auto key = "name"_quoted;  // Equivalent to: "\"name\""
```

---

### debrace

```cpp
std::string db::debrace(std::string s)
```

**Implementation**:
```cpp
if (s[0] == '{' && s[s.length() - 1] == '}') {
    return s.substr(1, s.length() - 2);
}
return s;
```

**Behavior**: Only removes braces if present at both ends.

---

### quotation_type

```cpp
db::quotation_type_t db::quotation_type(std::string s)
```

**Implementation**:
```cpp
if (s[0] == '\"' && s[s.length() - 1] == '\"') {
    return db::quotation_type_t::DOUBLE;
}
if (s[0] == '\'' && s[s.length() - 1] == '\'') {
    return db::quotation_type_t::SINGLE;
}
return db::quotation_type_t::NONE;
```

**Returns**: Enum indicating quote type.

---

### dequote

```cpp
std::string db::dequote(std::string s)
```

**Implementation**:
```cpp
if ((s[0] == '\"' && s[s.length() - 1] == '\"') ||
    (s[0] == '\'' && s[s.length() - 1] == '\'')) {
    return s.substr(1, s.length() - 2);
}
return s;
```

**Behavior**: Removes both single and double quotes if present at both ends.

---

### debracket

```cpp
std::string db::debracket(std::string s)
```

**Implementation**:
```cpp
if (s[0] == '[' && s[s.length() - 1] == ']') {
    return s.substr(1, s.length() - 2);
}
return s;
```

---

### annotate

```cpp
std::string db::annotate(std::string const& s)
```

**Implementation**:
```cpp
auto str = db::trim(s);

if (str[0] == '#') {
    return str;
}

if (str.empty()) {
    return apiliteral::nullstring;
}

return std::string{"#"}.append(str);
```

**Algorithm**:
1. Trim whitespace
2. If already starts with '#': return as-is
3. If empty: return empty string
4. Otherwise: prepend '#'

---

### generate_oid

```cpp
std::string db::generate_oid()
```

**Implementation Details**:
- Reads from `/proc/sys/kernel/random/uuid`
- Removes hyphens from UUID
- Truncates to 24 characters (MongoDB ObjectId length)

**Implementation**:
```cpp
TLOG(11) << "generate_oid() begin";

std::ifstream is("/proc/sys/kernel/random/uuid");

std::string oid((std::istreambuf_iterator<char>(is)),
                std::istreambuf_iterator<char>());

oid.erase(std::remove(oid.begin(), oid.end(), '-'), oid.end());
oid.resize(24);

TLOG(12) << "generate_oid() end";

return oid;
```

**Example**:
```
UUID:   550e8400-e29b-41d4-a716-446655440000
Remove: 550e8400e29b41d4a716446655440000
Resize: 550e8400e29b41d4a7164466
```

---

### to_id

```cpp
std::string db::to_id(std::string const& oid)
```

**Implementation**:
```cpp
confirm(!oid.empty());

std::ostringstream oss;
oss << "{" << "_id"_quoted << ":{";
oss << "$oid"_quoted << ":" << quoted_(oid);
oss << "} }";

return oss.str();
```

**Output Format**: `{"_id":{"$oid":"<oid>"}}`

**Purpose**: Creates MongoDB extended JSON format for ObjectId.

---

### to_json

```cpp
std::string db::to_json(std::string const& key, std::string const& value)
```

**Implementation**:
```cpp
confirm(!key.empty());
confirm(!value.empty());

std::ostringstream oss;
oss << "{" << quoted_(key) << ":" << quoted_(value) << "}";
return oss.str();
```

**Output**: Simple JSON object with one key-value pair.

---

### expand_environment_variables

```cpp
std::string db::expand_environment_variables(const std::string& var)
```

**Implementation Details**:
- Uses POSIX wordexp() for shell-style expansion
- Expands environment variables ($VAR, ${VAR})
- Expands tilde (~)
- Expands wildcards

**Implementation**:
```cpp
wordexp_t p;
char** w;

::wordexp(var.c_str(), &p, 0);

w = p.we_wordv;

std::ostringstream oss;

for (size_t i = 0; i < p.we_wordc; i++) {
    oss << w[i] << "/";
}

::wordfree(&p);

auto result = oss.str();

if (result.back() == '/') {
    result.pop_back();  // remove trailing slash
}

return result;
```

**Examples**:
```cpp
expand_environment_variables("$HOME/data");
// Returns: "/home/username/data"

expand_environment_variables("~/configs");
// Returns: "/home/username/configs"

expand_environment_variables("${ARTDAQ_DATABASE_PATH}/files");
// Returns: "/opt/artdaq/database/files"
```

**Note**: Can expand to multiple words if input contains wildcards.

---

### equal / not_equal

```cpp
bool db::equal(std::string const& left, std::string const& right)
{
    confirm(!left.empty());
    confirm(!right.empty());
    return left == right;
}

bool db::not_equal(std::string const& left, std::string const& right)
{
    return !equal(left, right);
}
```

**Purpose**: Wrapper functions for string comparison with validation.

---

### extract_oid

```cpp
db::object_id_t db::extract_oid(std::string const& filter)
```

**Implementation Details**:
- Uses regex to extract value from JSON-like filter
- Dequotes the extracted value
- Logs extraction steps

**Regex Pattern**: `R"(^\{[^:]+:\s+([\s\S]+)\}$)"`
- Matches: `{key: value}`
- Captures: `value`

**Implementation**:
```cpp
auto ex = std::regex(R"(^\{[^:]+:\s+([\s\S]+)\}$)");

auto results = std::smatch();

if (!std::regex_search(filter, results, ex)) {
    throw std::logic_error(std::string("Regex ouid search failed; JSON buffer:") + filter);
}

if (results.size() != 2) {
    // Log results
    throw runtime_error(...);
}

auto match = std::string(results[1]);

match.erase(match.find_last_not_of(" \n\r\t") + 1);  // Trim trailing whitespace

auto dequote = [](auto s) {
    if (s[0] == '"' && s[s.length() - 1] == '"') {
        return s.substr(1, s.length() - 2);
    }
    return s;
};

match = dequote(match);

return match;
```

**Example**:
```cpp
extract_oid("{\"_id\": \"507f1f77bcf86cd799439011\"}");
// Returns: "507f1f77bcf86cd799439011"
```

---

### trim

```cpp
std::string db::trim(std::string const& s)
```

**Implementation**:
```cpp
auto wsfront = std::find_if_not(s.begin(), s.end(), [](int c) { return std::isspace(c); });
return std::string(wsfront,
    std::find_if_not(s.rbegin(), std::string::const_reverse_iterator(wsfront),
                     [](int c) { return std::isspace(c); }).base());
```

**Algorithm**:
1. Find first non-whitespace from front
2. Find first non-whitespace from back
3. Return substring between these positions

**Efficient**: Single-pass algorithm using iterators.

---

### to_lower / to_upper

```cpp
std::string db::to_lower(std::string const& c)
{
    auto s = c;
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

std::string db::to_upper(std::string const& c)
{
    auto s = c;
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}
```

**Implementation**: Uses std::transform with ::tolower/::toupper.

---

### replace_all

```cpp
std::string db::replace_all(std::string const& source,
                            std::string const& match,
                            std::string const& replacement)
```

**Implementation Details**:
- Efficient algorithm that minimizes string allocations
- Pre-reserves memory
- Single pass through source string

**Algorithm**:
```cpp
auto retValue = std::string{};
auto match_len = match.size();
retValue.reserve(match_len);

std::size_t last, first = 0;

while (std::string::npos != (last = source.find(match, first))) {
    retValue.append(source, first, last - first);  // Append before match
    retValue.append(replacement);                   // Append replacement
    first = last + match_len;                       // Move past match
}

retValue.append(source, first, std::string::npos); // Append remainder

return retValue;
```

**Example**:
```cpp
replace_all("hello world hello", "hello", "hi");
// Returns: "hi world hi"
```

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

**Purpose**: Specialized implementations for different quote types.

## Performance Considerations

1. **String Operations**: Most functions minimize copying using references and move semantics where appropriate

2. **Memory Allocation**: Functions like replace_all pre-reserve memory to reduce reallocations

3. **Static Variables**: useFakeTime uses static local for persistent state without global variable overhead

4. **Iterator Usage**: trim() uses iterators for efficient traversal

5. **System Calls**: uname() and wordexp() involve system calls which are relatively expensive

## Security Considerations

1. **wordexp()**: Can execute arbitrary shell commands if input contains command substitutions
   - Should sanitize input or use safer alternatives

2. **File Operations**: generate_oid() reads from /proc which could fail in containers

3. **Regex**: extract_oid() could be vulnerable to ReDoS with malicious input patterns

## Related Files

- **helper_functions.h** - Function declarations
- **configuraion_api_literals.h** - Constants for timestamp formats and locales
- **shared_exceptions.h** - Exception types
- **printStackTrace.h** - Stack trace functionality used by confirm()
