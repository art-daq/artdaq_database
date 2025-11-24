# json_reader.cpp

## File Overview

Implementation of the JsonReader class and compare_json_objects function using Boost.Spirit Qi parser framework.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Json/json_reader.cpp`

## Dependencies

### Project Headers
- `"artdaq-database/DataFormats/Json/json_reader.h"` - JsonReader declarations
- `"artdaq-database/DataFormats/common.h"` - Common includes and TRACE

## TRACE Configuration

```cpp
#define TRACE_NAME "json_reader.cpp"
```

## Implementation

### JsonReader::read

```cpp
bool JsonReader::read(std::string const& in, object_t& ast)
```

**Preconditions**:
```cpp
confirm(!in.empty());
confirm(ast.empty());
```

**Algorithm**:
1. Log parse start (TLOG level 20)
2. Check input size, log stack trace if >512 bytes (expensive operation warning)
3. Log input string (TLOG level 22)
4. Create parser grammar instance
5. Call Boost.Spirit phrase_parse
6. On success, swap buffer into output ast
7. Log completion

**TRACE Logging**:
- `TLOG(20)`: Parse start
- `TLOG(21)`: Heavy read warning (>512 bytes) with stack trace
- `TLOG(22)`: Input content
- `TLOG(23)`: Parse end

**Returns**: Boolean indicating parse success

**Note**: Stack trace logged for large inputs to identify performance bottlenecks.

### compare_json_objects

```cpp
std::pair<bool, std::string> compare_json_objects(
    std::string const& first,
    std::string const& second)
```

**Preconditions**:
```cpp
confirm(!first.empty());
confirm(!second.empty());
```

**Algorithm**:
1. Create empty AST objects for both inputs
2. Parse first JSON string
3. If parse fails, return error for first buffer
4. Parse second JSON string
5. If parse fails, return error for second buffer
6. Compare ASTs using operator==
7. Return comparison result

**Returns**: Pair of (success, message)

**Error Messages**:
- "Unable to read first Json buffer" - First parse failed
- "Unable to read second Json buffer" - Second parse failed
- Plus any messages from AST comparison operator==

## Usage Example

```cpp
std::string json = R"({"key": "value"})";
object_t ast;

if (JsonReader{}.read(json, ast)) {
    // Parse succeeded
} else {
    // Parse failed
}
```

## Performance Notes

- Stack traces logged for inputs >512 bytes
- Grammar instantiated for each parse (could be optimized)
- TRACE logging can be adjusted via TLOG levels

## Related Files

- **json_reader.h** - JsonReader declarations and grammar
- **json_types.h** - AST type definitions
- **json_writer.cpp** - Complementary writer implementation
