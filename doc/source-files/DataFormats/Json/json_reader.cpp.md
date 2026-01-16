# json_reader.cpp

**Path:** `artdaq-database/DataFormats/Json/json_reader.cpp`

**Implements:** [json_reader.h](./json_reader.h.md)

**Purpose:** Provides the implementation of the `JsonReader::read()` method and the `compare_json_objects()` utility function. This file contains the actual Boost.Spirit Qi parsing logic that converts JSON text into the internal AST representation.

## Implementation Overview

This file implements the JSON parsing interface declared in `json_reader.h`. The implementation uses Boost.Spirit Qi's `phrase_parse` function with the grammar defined in the header to parse JSON text. It includes performance monitoring via TRACE logging and stack traces for large inputs.

## Key Algorithms

### JSON Parsing Algorithm

The `JsonReader::read()` method parses JSON text using Boost.Spirit Qi.

**Steps:**
1. Validate preconditions (non-empty input, empty output AST)
2. Log parse operation start
3. Log stack trace if input exceeds 512 bytes (performance monitoring)
4. Create grammar instance for current iterator type
5. Call `qi::phrase_parse()` with the grammar and whitespace skipper
6. On success, swap parsed buffer into output parameter
7. Log completion and return result

**Why this approach:** Using `swap()` instead of assignment ensures exception safety and efficient transfer of the parsed data. The buffer pattern allows the original output parameter to remain unchanged if parsing fails.

### JSON Comparison Algorithm

The `compare_json_objects()` function compares two JSON strings by parsing and comparing their ASTs.

**Steps:**
1. Validate preconditions (both inputs non-empty)
2. Create empty AST objects for both inputs
3. Parse first JSON string; return error pair if parsing fails
4. Parse second JSON string; return error pair if parsing fails
5. Compare ASTs using `operator==` (defined in `json_types.h`)
6. Return comparison result

**Why this approach:** Parsing both strings to AST before comparison ensures that semantically equivalent JSON (e.g., different whitespace formatting) compares as equal. The AST comparison provides detailed difference information.

## Internal Functions

### `JsonReader::read()`

**Brief:** Core parsing implementation that invokes Boost.Spirit Qi parser on JSON input.

**Called by:** External callers via the public API

**Purpose:** Wraps Boost.Spirit's `phrase_parse` with proper precondition validation, logging, and buffer management.

**Implementation Details:**

```cpp
bool JsonReader::read(std::string const& in, object_t& ast) {
  confirm(!in.empty());      // Input must not be empty
  confirm(ast.empty());      // Output must be empty

  TLOG(20) << "read() begin";

  // Performance monitoring for large inputs
  if (in.size() > 512) {
    TLOG(21) << "heavy read() " << debug::getStackTrace();
  }

  TLOG(22) << "read() in=<" << in << ">";

  auto result = bool(false);
  object_t buffer;

  // Create grammar instance
  json_parser_grammar<std::string::const_iterator> grammar;
  std::string::const_iterator start = in.begin();

  // Invoke parser
  result = qi::phrase_parse(start, in.end(), grammar, ascii::space, buffer);

  // Transfer result on success
  if (result) {
    ast.swap(buffer);
  }

  TLOG(23) << "write() end ";  // Note: comment says "write" but this is read

  return result;
}
```

### `compare_json_objects()`

**Brief:** Parses two JSON strings and compares their AST representations.

**Called by:** Test code, validation utilities

**Purpose:** Provides a convenient way to compare JSON documents for equality, ignoring formatting differences.

## Performance Considerations

- **Grammar Instantiation:** A new grammar instance is created for each parse operation. For high-frequency parsing, this could be optimized by caching the grammar.

- **Memory:** Uses a local buffer and `swap()` to minimize memory allocation overhead.

- **Large Input Detection:** Inputs larger than 512 bytes trigger stack trace logging to help identify performance bottlenecks in production.

- **Buffer Pre-allocation:** The implementation does not pre-allocate the buffer; for very large JSON documents, consider pre-reserving capacity.

## Error Handling Strategy

- **Precondition Failures:** The `confirm()` macro validates preconditions. In debug builds, violations will abort; in release builds, behavior is undefined.

- **Parse Failures:** Parse errors are indicated by a `false` return value. The output parameter remains unchanged on failure.

- **Exception Safety:** The implementation is exception-safe; if `phrase_parse` throws, the output parameter is unmodified.

- **Error Messages:** The `compare_json_objects()` function returns descriptive error messages for parse failures.

## Testing Notes

- **Unit tests:** `test/DataFormats/Json/json_t.cc`
- **Key test cases:**
  - Basic object parsing
  - Nested objects and arrays
  - Number type inference (integer vs. decimal)
  - String escape sequences
  - Empty objects and arrays
  - Comparison of equivalent JSON with different formatting

## Maintenance Notes

- **TRACE Levels:** The implementation uses TRACE levels 20-23 for logging. Adjust `trace_mode` settings to enable/disable specific levels.

- **Log Message Typo:** Line 41 logs "write() end" but this is the read implementation. This is a minor documentation inconsistency in the source.

- **Performance Threshold:** The 512-byte threshold for stack trace logging is hardcoded. Consider making this configurable for different deployment environments.

## TRACE Logging Levels

| Level | Message | Purpose |
|-------|---------|---------|
| 20 | `read() begin` | Parse operation start |
| 21 | `heavy read() <stacktrace>` | Performance monitoring for large inputs (>512 bytes) |
| 22 | `read() in=<...>` | Input content for debugging |
| 23 | `write() end` | Parse operation complete (note: typo in message) |

## See Also

- [json_reader.h](./json_reader.h.md) - Header file with `JsonReader` declaration and grammar
- [json_writer.cpp](./json_writer.cpp.md) - Complementary JSON generator implementation
- [json_types.h](./json_types.h.md) - AST type definitions and comparison operators
