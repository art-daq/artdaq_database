# readjson.cc

**Path:** `artdaq-database/Utilities/readjson.cc`

**Purpose:** JSON parser and serializer performance testing utility. This tool reads a JSON file, parses it into an AST, and performs repeated serialization cycles to benchmark JSON processing performance and detect potential memory issues.

## Overview

The `readjson` utility is primarily a testing and benchmarking tool for the JSON parser and writer components. It reads a JSON file, parses it into an Abstract Syntax Tree (AST), and then repeatedly serializes it back to a string 100 times to measure performance characteristics. This is useful for regression testing and performance profiling.

## Key Concepts

### JSON Processing Pipeline

1. **Read:** Load JSON file content into memory using `read_buffer_from_file`
2. **Parse:** Convert JSON string to AST using `JsonReader`
3. **Serialize:** Convert AST back to JSON string using `JsonWriter`
4. **Benchmark:** Repeat serialization 100 times to measure throughput

### Performance Testing Use Cases

- **Benchmark JSON serialization throughput** for different document sizes
- **Detect performance regressions** after code changes
- **Memory leak detection** when run with memory profilers (e.g., Valgrind, AddressSanitizer)
- **Verify round-trip consistency** of JSON processing

### Progress Indicator

The tool outputs a progress character for each iteration:
- `+` indicates the serialized output exceeds 100 characters (typical for real documents)
- `-` indicates smaller output (possibly indicating a problem)

## Thread Safety

- **Thread Safety:** Not thread-safe
- **Concurrent Access:** Safe to run multiple instances on different files
- **Locking:** No internal locking

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/Json/json_common.h` | JSON type definitions (`object_t`, `JsonReader`, `JsonWriter`) |
| `artdaq-database/JsonDocument/JSONDocument.h` | JSON document class |
| `test/common.h` | Common test utilities, `read_buffer_from_file`, process exit codes |
| `boost/program_options.hpp` | Command-line parsing |

## Command-Line Options

| Option | Short | Required | Description |
|--------|-------|----------|-------------|
| `--config` | `-c` | Yes | Path to JSON file to process |
| `--outputformat` | `-f` | No | Output format (accepted but not currently used) |
| `--help` | `-h` | No | Display help message |

## Functions

### `main(argc, argv) -> int`

**Brief:** Entry point that parses command-line arguments, reads and parses a JSON file, then performs 100 serialization cycles while printing progress indicators to measure performance characteristics.

**Parameters:**
- `argc` - Number of command-line arguments
- `argv` - Array of command-line argument strings

**Preconditions:**
- The JSON file specified by `--config` must exist and be readable
- The file must contain valid JSON

**Returns:**
- `process_exit_code::SUCCESS` (0) - All 100 iterations completed successfully
- `process_exit_code::HELP` (1) - Help message displayed
- `process_exit_code::FAILURE` (1) - JSON parsing or serialization failed
- `process_exit_code::INVALID_ARGUMENT` (128) - Missing required arguments
- `process_exit_code::INVALID_ARGUMENT | 1` (129) - Missing `--config` argument
- `process_exit_code::UNCAUGHT_EXCEPTION` (144) - Unhandled exception occurred

**Postconditions:**
- 100 `+` or `-` characters printed to stdout (one per iteration)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `bpo::error` | Command-line parsing error (caught and reported) |
| (caught at top level) | All other exceptions caught and reported |

**Thread Safety:** Not thread-safe

**Side Effects:**
- Reads file from disk
- Allocates memory for JSON parsing and serialization
- Writes 100 progress characters to stdout

**Complexity:** O(100 * n) where n is the size of the JSON document

## Usage Examples

### Basic Performance Test

```bash
# Run JSON serialization benchmark (100 iterations)
readjson -c large_config.json
```

**Expected Output:**
```
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
```
(100 `+` characters, one per successful iteration)

### Using with Time Measurement

```bash
# Measure total execution time
time readjson -c large_config.json
```

### Memory Profiling

```bash
# Run with Valgrind for memory leak detection
valgrind --leak-check=full readjson -c config.json

# Run with AddressSanitizer (if built with USE_ASAN=1)
ASAN_OPTIONS=detect_leaks=1 readjson -c config.json
```

## Implementation Details

### Benchmark Loop

```cpp
auto out = std::string{};
out.reserve(1000000);  // Pre-allocate 1MB buffer to minimize reallocations

for (int i = 0; i < 100; i++) {
  out.clear();  // Reuse buffer, don't reallocate

  if (!writer.write(doc_ast, out)) {
    return process_exit_code::FAILURE;
  }

  std::cout << (out.size() > 100 ? "+" : "-");
}
```

### Processing Steps

1. **File Reading:**
   ```cpp
   auto json = std::string{};
   db::read_buffer_from_file(json, file_name);
   ```

2. **Parsing:**
   ```cpp
   auto reader = jsn::JsonReader{};
   jsn::object_t doc_ast;
   if (!reader.read(json, doc_ast)) {
     return process_exit_code::FAILURE;
   }
   ```

3. **Serialization Loop:**
   ```cpp
   auto writer = jsn::JsonWriter{};
   for (int i = 0; i < 100; i++) {
     out.clear();
     if (!writer.write(doc_ast, out)) {
       return process_exit_code::FAILURE;
     }
     std::cout << (out.size() > 100 ? "+" : "-");
   }
   ```

## Exit Codes

| Code | Constant | Meaning |
|------|----------|---------|
| 0 | `process_exit_code::SUCCESS` | All 100 iterations completed successfully |
| 1 | `process_exit_code::HELP` | Help message displayed |
| 1 | `process_exit_code::FAILURE` | JSON parsing or serialization failed |
| 128 | `process_exit_code::INVALID_ARGUMENT` | Missing required arguments |
| 129 | `process_exit_code::INVALID_ARGUMENT \| 1` | Missing `--config` argument |
| 144 | `process_exit_code::UNCAUGHT_EXCEPTION` | Unhandled exception occurred |

## Performance Considerations

- **Buffer Pre-allocation:** The output buffer is pre-allocated to 1MB to minimize memory reallocations during the benchmark
- **Buffer Reuse:** Uses `clear()` instead of creating a new string each iteration to measure serialization performance without allocation overhead
- **100 Iterations:** Provides a reasonable sample for performance measurement while keeping execution time manageable

### Interpreting Results

- **Consistent `+` output:** Normal operation, JSON is valid and large enough
- **Any `-` output:** May indicate very small JSON or potential issues
- **Process exit with FAILURE:** Parse error on first iteration, or write error during benchmark
- **Slow execution:** May indicate performance regression in JsonWriter

## Use Cases

1. **Performance Benchmarking:**
   ```bash
   # Compare performance between versions
   time ./old_version/readjson -c test.json
   time ./new_version/readjson -c test.json
   ```

2. **Regression Testing:**
   ```bash
   # Part of CI/CD pipeline
   readjson -c test_fixtures/large_document.json || exit 1
   ```

3. **Memory Profiling:**
   ```bash
   # Check for memory leaks in repeated operations
   valgrind readjson -c config.json
   ```

4. **Stress Testing:**
   ```bash
   # Run with a very large JSON file
   readjson -c /path/to/huge_configuration.json
   ```

## Error Handling

Parsing and serialization errors cause immediate exit with `FAILURE` code:

```cpp
if (!reader.read(json, doc_ast)) {
  return process_exit_code::FAILURE;
}

// ...

if (!writer.write(doc_ast, out)) {
  return process_exit_code::FAILURE;
}
```

Uncaught exceptions are reported with diagnostic information:

```cpp
} catch (...) {
  std::cerr << "Process exited with error: " << ::debug::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}
```

## Common Pitfalls

- **Large Files:** Very large JSON files may cause memory issues (watch for `std::bad_alloc`)
- **Malformed JSON:** The tool will fail on the first parsing attempt if JSON is invalid
- **Output Format Option:** The `-f` option is accepted but not used; it has no effect on output
- **No Timing Output:** The tool doesn't output timing information; use external tools like `time`

## Relationship to Other Components

- **JsonReader/JsonWriter:** Tests these core JSON processing classes from DataFormats/Json
- **JSONDocument:** Uses the JSON document representation
- **fixtestjson:** Related utility that modifies JSON value types
- **DataFormats/Json:** The module containing the JSON parsing implementation being tested

## Commented Code Note

The source contains commented-out interactive debugging code:

```cpp
// int a;
// std::cout << "Continue ?";
// std::cin>>a;
// if(a==0)
//         return process_exit_code::FAILURE;
```

This was likely used during development to pause before intensive operations for attaching debuggers or profilers.

## See Also

- [fixtestjson.cc.md](./fixtestjson.cc.md) - JSON value type conversion utility
- [DataFormats/Json/json_reader.cpp](../DataFormats/Json/json_reader.cpp.md) - JSON parser implementation
- [DataFormats/Json/json_writer.cpp](../DataFormats/Json/json_writer.cpp.md) - JSON serializer implementation
- [JSONDocument.h](../JsonDocument/JSONDocument.h.md) - JSON document class
