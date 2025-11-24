# json_writer.cpp

## File Overview

Implementation of the JsonWriter class using Boost.Spirit Karma generator framework.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Json/json_writer.cpp`

## Dependencies

### Project Headers
- `"artdaq-database/DataFormats/Json/json_writer.h"` - JsonWriter declarations
- `"artdaq-database/DataFormats/common.h"` - Common includes and TRACE

## TRACE Configuration

```cpp
#define TRACE_NAME "json_writer.cpp"
```

## Implementation

### JsonWriter::write

```cpp
bool JsonWriter::write(object_t const& ast, std::string& out)
```

**Preconditions**:
```cpp
confirm(out.empty());
confirm(!ast.empty());
```

**Algorithm**:
1. Log write start (TLOG level 21)
2. Create output buffer, reserve 10000 bytes
3. Create back_insert_iterator
4. Instantiate generator grammar
5. Call Boost.Spirit generate
6. On success, swap buffer into output parameter
7. Log output (TLOG level 22)
8. Check output size, log stack trace if >512 bytes
9. Log completion (TLOG level 24)

**Memory Management**:
```cpp
auto buffer = std::string();
buffer.reserve(10000);
```
Pre-allocates buffer to minimize reallocations.

**TRACE Logging**:
- `TLOG(21)`: Write start
- `TLOG(22)`: Output content
- `TLOG(23)`: Heavy write warning (>512 bytes) with stack trace
- `TLOG(24)`: Write end

**Returns**: Boolean indicating generation success

**Performance Monitoring**:
- Stack traces logged for large outputs to identify bottlenecks
- Pre-allocation reduces memory allocation overhead

## Usage Example

```cpp
object_t ast;
ast["key"] = std::string("value");

std::string output;
if (JsonWriter{}.write(ast, output)) {
    // output contains JSON text
}
```

## Implementation Details

**Buffer Strategy**:
- Initial reserve of 10000 bytes
- Grows automatically if needed
- Swap used for efficient transfer

**Error Handling**:
- Non-throwing - returns false on failure
- Leaves output parameter unchanged on failure

**Debugging**:
- Comprehensive TRACE logging
- Stack traces for performance analysis

## Related Files

- **json_writer.h** - JsonWriter declarations and grammar
- **json_reader.cpp** - Complementary reader implementation
- **json_types.h** - AST type definitions
