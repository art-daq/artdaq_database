# json_writer.cpp

**Path:** `artdaq-database/DataFormats/Json/json_writer.cpp`

**Implements:** [json_writer.h](./json_writer.h.md)

**Purpose:** Provides the implementation of the `JsonWriter::write()` method. This file contains the actual Boost.Spirit Karma generation logic that converts the internal AST representation into formatted JSON text.

## Implementation Overview

This file implements the JSON generation interface declared in `json_writer.h`. The implementation uses Boost.Spirit Karma's `generate` function with the grammar defined in the header to produce JSON text. It includes performance monitoring via TRACE logging and stack traces for large outputs, as well as memory optimization through buffer pre-allocation.

## Key Algorithms

### JSON Generation Algorithm

The `JsonWriter::write()` method generates JSON text using Boost.Spirit Karma.

**Steps:**
1. Validate preconditions (non-empty input AST, empty output string)
2. Log generation operation start
3. Create output buffer with 10000 bytes pre-allocated
4. Create back_insert_iterator for efficient appending
5. Instantiate generator grammar for current iterator type
6. Call `karma::generate()` with the grammar
7. On success, swap generated buffer into output parameter
8. Log output content
9. Log stack trace if output exceeds 512 bytes (performance monitoring)
10. Log completion and return result

**Why this approach:**
- Pre-allocating 10000 bytes reduces memory reallocations for typical configurations
- Using `swap()` ensures exception safety and efficient transfer
- The buffer pattern allows the original output parameter to remain unchanged if generation fails

## Internal Functions

### `JsonWriter::write()`

**Brief:** Core generation implementation that invokes Boost.Spirit Karma generator on AST input.

**Called by:** External callers via the public API

**Purpose:** Wraps Boost.Spirit's `generate` with proper precondition validation, logging, and buffer management.

**Implementation Details:**

```cpp
bool JsonWriter::write(object_t const& ast, std::string& out) {
  confirm(out.empty());    // Output must be empty
  confirm(!ast.empty());   // AST must not be empty
  TLOG(21) << "write() begin ";

  auto result = bool(false);
  auto buffer = std::string();
  buffer.reserve(10000);   // Pre-allocate for performance

  auto sink = std::back_insert_iterator<std::string>(buffer);

  json_generator_grammar<decltype(sink)> grammar;

  result = karma::generate(sink, grammar, ast);

  if (result) {
    out.swap(buffer);
  }
  TLOG(22) << "write() out=<" << out << ">";

  // Performance monitoring for large outputs
  if (out.size() > 512) {
    TLOG(23) << "heavy write() " << debug::getStackTrace();
  }

  TLOG(24) << "write() end ";
  return result;
}
```

## Performance Considerations

- **Buffer Pre-allocation:** The buffer is pre-allocated with 10000 bytes (`buffer.reserve(10000)`), which is sufficient for most configuration documents and eliminates reallocations during generation.

- **Grammar Instantiation:** A new grammar instance is created for each generation operation. For high-frequency generation, this could be optimized by caching the grammar.

- **Back Insert Iterator:** Using `std::back_insert_iterator` provides efficient character appending without bounds checking overhead.

- **Large Output Detection:** Outputs larger than 512 bytes trigger stack trace logging to help identify performance bottlenecks in production.

- **Swap Optimization:** The `swap()` operation is O(1) and avoids copying the generated content.

## Error Handling Strategy

- **Precondition Failures:** The `confirm()` macro validates preconditions. In debug builds, violations will abort; in release builds, behavior is undefined.

- **Generation Failures:** Generation errors are indicated by a `false` return value. The output parameter remains unchanged on failure.

- **Exception Safety:** The implementation is exception-safe; if `generate` throws, the output parameter is unmodified.

## Testing Notes

- **Unit tests:** `test/DataFormats/Json/json_t.cc`
- **Key test cases:**
  - Basic object generation
  - Nested objects and arrays
  - Various value types (strings, integers, decimals, booleans)
  - Empty objects and arrays
  - Round-trip testing (read then write, write then read)

## Maintenance Notes

- **TRACE Levels:** The implementation uses TRACE levels 21-24 for logging. Adjust `trace_mode` settings to enable/disable specific levels.

- **Buffer Size:** The 10000-byte pre-allocation is hardcoded. For systems with very large configurations, consider increasing this value or making it configurable.

- **Performance Threshold:** The 512-byte threshold for stack trace logging is hardcoded. Consider making this configurable for different deployment environments.

## TRACE Logging Levels

| Level | Message | Purpose |
|-------|---------|---------|
| 21 | `write() begin` | Generation operation start |
| 22 | `write() out=<...>` | Generated output for debugging |
| 23 | `heavy write() <stacktrace>` | Performance monitoring for large outputs (>512 bytes) |
| 24 | `write() end` | Generation operation complete |

## Memory Management

The implementation uses a careful memory management strategy:

1. **Pre-allocation:** `buffer.reserve(10000)` allocates memory upfront
2. **Efficient Appending:** `back_insert_iterator` appends without bounds checking
3. **Zero-copy Transfer:** `swap()` transfers ownership without copying
4. **Automatic Cleanup:** Local buffer is automatically destroyed on exit

This approach minimizes memory allocations and ensures predictable performance for typical workloads.

## See Also

- [json_writer.h](./json_writer.h.md) - Header file with `JsonWriter` declaration and grammar
- [json_reader.cpp](./json_reader.cpp.md) - Complementary JSON parser implementation
- [json_types.h](./json_types.h.md) - AST type definitions
