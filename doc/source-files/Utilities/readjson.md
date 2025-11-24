# readjson.cc

## Overview

`readjson` is a performance testing utility for JSON serialization and deserialization. It reads a JSON file, parses it into an abstract syntax tree (AST), and then repeatedly serializes it back to JSON to benchmark the writer performance.

## Purpose and Use Cases

- Benchmarking JSON writer performance
- Testing JSON parser/writer reliability
- Validating JSON document integrity through round-trip conversion
- Performance profiling of JSON operations
- Stress testing JSON serialization

## Location

**File**: `/home/user/artdaq-database/artdaq-database/Utilities/readjson.cc`

## Command-Line Arguments

### Required Arguments

| Argument | Short | Type | Description |
|----------|-------|------|-------------|
| `--config` | `-c` | string | Path to JSON configuration file |

### Optional Arguments

| Argument | Short | Type | Description |
|----------|-------|------|-------------|
| `--outputformat` | `-f` | string | Output file format (not used currently) |
| `--help` | `-h` | - | Display help message |

## Main Workflow/Algorithm

```
1. Parse command-line arguments
2. Read JSON file into string buffer
3. Parse JSON into object_t AST using JsonReader
4. Pre-allocate 1MB output buffer
5. Loop 100 times:
   a. Clear output buffer
   b. Serialize AST to JSON string using JsonWriter
   c. Print '+' if output > 100 bytes, else '-'
6. Return success if all iterations succeed
```

## Key Functions

### main()
Primary entry point that performs the benchmark.

**Steps**:
1. Parse command-line options
2. Read JSON file
3. Parse into AST
4. Reserve large output buffer (1,000,000 bytes)
5. Perform 100 serialization iterations
6. Print progress indicators
7. Report success/failure

## Performance Testing

### Test Configuration

**Iterations**: 100 (hardcoded)
**Buffer Size**: 1MB pre-allocated
**Progress Indicator**:
- `+` : Output size > 100 bytes
- `-` : Output size ≤ 100 bytes

### Benchmark Metrics

The utility measures:
- **Reliability**: All 100 iterations must succeed
- **Consistency**: Same AST produces consistent output
- **Performance**: Visual feedback on operation speed

### Output Interpretation

```
++++++++++++++++++++++++++++++++++++++++++++++++++++
```
100 successful serializations of large JSON

```
--------------------------------------------------
```
100 successful serializations of small JSON

```
+++++++++++++FAILURE
```
Serialization failed after 13 iterations

## Usage Examples

### Basic Performance Test
```bash
readjson -c large_config.json
```

### With Timing
```bash
time readjson -c config.json
```

### Multiple Files Test
```bash
for json in *.json; do
  echo "Testing $json:"
  readjson -c "$json"
done
```

### Stress Test Script
```bash
#!/bin/bash
for i in {1..1000}; do
  if ! readjson -c test.json > /dev/null 2>&1; then
    echo "Failed at iteration $i"
    exit 1
  fi
done
echo "All 1000 tests passed"
```

## Common Scenarios

### Validating JSON Parser
Ensure JSON can be reliably parsed and serialized:
```bash
if readjson -c test.json > /dev/null 2>&1; then
  echo "JSON parser working correctly"
else
  echo "JSON parser has issues"
fi
```

### Performance Regression Testing
Compare performance across versions:
```bash
# Version 1
time ./readjson_v1 -c large.json

# Version 2
time ./readjson_v2 -c large.json
```

### Memory Leak Detection
Run under valgrind to detect leaks:
```bash
valgrind --leak-check=full readjson -c config.json
```

### Round-Trip Validation
Verify JSON integrity through parse/serialize:
```bash
# Original
md5sum original.json

# After 100 round trips
readjson -c original.json 2>&1 | tail -1 > final.json
md5sum final.json

# Should match (assuming deterministic serialization)
```

## Implementation Details

### JSON Components Used

**JsonReader**: Parses JSON string into AST
```cpp
jsn::JsonReader reader;
jsn::object_t doc_ast;
reader.read(json, doc_ast);
```

**JsonWriter**: Serializes AST to JSON string
```cpp
jsn::JsonWriter writer;
std::string out;
writer.write(doc_ast, out);
```

### Memory Management

**Pre-allocation Strategy**:
```cpp
auto out = std::string{};
out.reserve(1000000);  // 1MB
```
Avoids repeated allocations during iterations.

**Clear vs New Allocation**:
```cpp
out.clear();  // Retains capacity
```
Efficient reuse of allocated memory.

### AST Structure

Uses `jsn::object_t` from artdaq-database JSON library:
- Hierarchical structure
- Type-safe node representation
- Supports nested objects and arrays

### Commented Debug Code

Lines 57-61 contain commented interactive debugging:
```cpp
// int a;
// std::cout << "Continue ?";
// std::cin>>a;
// if(a==0)
//     return process_exit_code::FAILURE;
```
Allows manual inspection/breakpoint during development.

## Error Handling

### Parse Failure
If JSON cannot be parsed:
```cpp
if (!reader.read(json, doc_ast)) {
    return process_exit_code::FAILURE;
}
```
Returns immediately with failure code.

### Serialization Failure
If any of 100 writes fail:
```cpp
if (!writer.write(doc_ast, out)) {
    return process_exit_code::FAILURE;
}
```
Stops immediately, prints partial progress.

### Exit Codes
- `SUCCESS` (0): All 100 iterations succeeded
- `INVALID_ARGUMENT`: Command-line error
- `HELP`: Help displayed
- `INVALID_ARGUMENT | 1`: No config file specified
- `FAILURE`: Parse or serialization failure
- `UNCAUGHT_EXCEPTION`: Unhandled exception

## Performance Characteristics

### Bottlenecks
1. **JSON serialization**: O(n) in document size
2. **String operations**: Memory allocation/copying
3. **AST traversal**: Recursive structure walking

### Optimization Strategies
1. **Buffer pre-allocation**: Reduces allocations
2. **Clear instead of rebuild**: Retains capacity
3. **Single parse, multiple writes**: Amortizes parse cost

### Expected Performance

Typical results (varies by hardware/document):
- **Small JSON** (< 1KB): < 0.1 seconds for 100 iterations
- **Medium JSON** (10-100KB): 0.5-2 seconds
- **Large JSON** (> 1MB): 5-20 seconds

## Limitations

### Fixed Iteration Count
Hardcoded to 100 iterations:
```cpp
for (int i = 0; i < 100; i++) {
```
Not configurable via command line.

### No Timing Output
Visual progress only, no numeric timing:
- Cannot directly measure milliseconds per operation
- Need external timing tool (`time` command)

### Unused Parameter
`--outputformat` parsed but not used:
```cpp
("outputformat,f", bpo::value<std::string>(), "Output file format.")
```
Possibly for future enhancement.

### No Output File
Results not saved, only validated:
- Cannot inspect serialized JSON
- Only pass/fail indication
- Output discarded after each iteration

## Testing Applications

### Unit Testing
Verify JSON library correctness:
```bash
# Test suite
test_json_parser() {
  readjson -c "$1" > /dev/null
}
```

### Performance Baseline
Establish performance metrics:
```bash
# Baseline measurement
echo "Small JSON:" && time readjson -c small.json
echo "Large JSON:" && time readjson -c large.json
```

### Continuous Integration
Add to CI pipeline:
```yaml
- name: Test JSON Performance
  run: |
    readjson -c test_data/config.json
    if [ $? -ne 0 ]; then exit 1; fi
```

### Fuzz Testing
Test with generated JSON:
```bash
# Generate random JSON
python generate_json.py > random.json

# Test
readjson -c random.json
```

## Dependencies

- `artdaq-database/DataFormats/Json/json_common.h`
- `artdaq-database/JsonDocument/JSONDocument.h`
- `test/common.h`
- Boost.Program_options for argument parsing

## Related Utilities

- **fhicl2json**: Converts FHiCL to JSON (creates JSON files to test)
- **fixtestjson**: Migrates JSON to string values
- **conftool**: Uses JSON for database operations

## Potential Enhancements

Future improvements could include:
1. Configurable iteration count
2. Timing statistics output
3. Memory usage reporting
4. Output file option for inspection
5. Comparison with original (validation mode)
6. Progress bar instead of +/- characters
7. JSON validation checks
8. Streaming mode for very large files
9. Parallel serialization testing
10. Regression detection (compare with baseline)

## Debugging Tips

### Tracking Failures
```bash
# Verbose output
readjson -c problem.json 2>&1 | tee log.txt
# Check where it stops
```

### Memory Profiling
```bash
# Check for leaks
valgrind --leak-check=full --show-leak-kinds=all \
  readjson -c config.json
```

### Performance Profiling
```bash
# CPU profiling
perf record readjson -c large.json
perf report
```

### Comparing Performance
```bash
# Before optimization
time readjson -c test.json > /dev/null 2>&1

# After optimization
time readjson_optimized -c test.json > /dev/null 2>&1
```
