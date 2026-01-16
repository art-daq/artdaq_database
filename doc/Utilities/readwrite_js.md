# readwrite.js

**Path:** `artdaq-database/Utilities/readwrite.js`

**Purpose:** Node.js utility script that performs round-trip format conversion testing between FHiCL and JSON formats. The script reads a FHiCL file, converts it to JSON, validates the JSON structure by parsing it, then converts it back to FHiCL and writes the result to a `.conv` file. This verifies that the conversion pipeline preserves data integrity.

## Key Concepts

### Round-Trip Conversion Testing

The script tests the complete format conversion pipeline:
1. FHiCL input -> JSON intermediate representation
2. JSON parsing to validate structure
3. JSON -> FHiCL output conversion
4. Write result to verify serialization

This ensures that data can be safely converted between formats without loss.

### Native Node.js Bindings

The script uses the `fhicljson.node` native module, which provides high-performance format conversion by directly accessing the C++ conversion libraries through Node.js bindings.

## Thread Safety

- **Thread Safety:** Single-threaded (Node.js event loop)
- **Concurrent Access:** Multiple instances can run in parallel on different files
- **Locking:** No file locking; avoid concurrent access to the same output file

## Dependencies

### Runtime Requirements

| Requirement | Purpose |
|-------------|---------|
| Node.js | JavaScript runtime environment |
| `fhicljson.node` | Native module for format conversion |

### Module Location

The script locates the native module relative to its own location:

```javascript
var libname = process.argv.slice(0)[1].replace(/\\/g, '/')
              .replace(/\/[^\/]*$/, '') + "/../lib/node_modules/fhicljson/fhicljson.node"
```

Expected path structure:
```
{script_directory}/
  readwrite.js
  ../lib/node_modules/fhicljson/
    fhicljson.node
```

## Command-Line Interface

### Synopsis

```bash
node readwrite.js <input-file>
```

### Arguments

| Argument | Required | Description |
|----------|----------|-------------|
| `input-file` | Yes | Path to the FHiCL file to process |

### Output

- On success: Writes `{input-file}.conv` with the round-trip converted FHiCL
- On failure: Prints error message to console

## Functions

### Native Module Functions

#### `mylib.tojson(filename) -> {first: boolean, second: string}`

**Brief:** Converts a FHiCL file to JSON format.

**Parameters:**
- `filename` - Path to the input FHiCL file

**Returns:** Object with:
- `first` - Boolean success indicator
- `second` - JSON string on success, or error message on failure

#### `mylib.tofhicl(filename, jsonString) -> {first: boolean, second: string}`

**Brief:** Converts JSON string to FHiCL format and writes to a file.

**Parameters:**
- `filename` - Path to the output file
- `jsonString` - JSON string to convert

**Returns:** Object with:
- `first` - Boolean success indicator
- `second` - Success message or error description

## Usage Examples

### Basic Conversion Test

```bash
# Create a test FHiCL file
cat > test.fcl << EOF
process_name: TestProcess
source: {
  module_type: RootInput
  fileNames: ["data.root"]
}
physics: {
  analyzers: {
    myAnalyzer: {
      module_type: MyAnalyzer
      threshold: 100
    }
  }
}
EOF

# Run conversion test
node readwrite.js test.fcl

# Expected output on success:
# Wrote file test.fcl.conv

# Compare input and output
diff test.fcl test.fcl.conv
```

### Batch Testing Multiple Files

```bash
#!/bin/bash
# Test all FHiCL files in a directory

for fcl in *.fcl; do
  echo "Testing $fcl..."
  if node readwrite.js "$fcl"; then
    echo "  SUCCESS: $fcl -> $fcl.conv"
  else
    echo "  FAILED: $fcl"
  fi
done
```

### Integration Testing Pipeline

```bash
# Test that round-trip conversion preserves semantics
node readwrite.js original.fcl

# Parse both files with fhicl-dump to compare
fhicl-dump original.fcl > original.dump
fhicl-dump original.fcl.conv > converted.dump
diff original.dump converted.dump && echo "Semantically equivalent"
```

### Error Handling Example

```bash
# Test with invalid FHiCL
echo "invalid { syntax" > bad.fcl
node readwrite.js bad.fcl
# Output: error message describing the parse failure
```

## Output File Format

- Output filename: `{input-filename}.conv`
- Example: `config.fcl` produces `config.fcl.conv`
- The output contains FHiCL reformatted through the JSON intermediate representation

## Error Handling

The script provides two-stage error handling:

1. **FHiCL-to-JSON conversion errors:** Reports parsing failures in the input file
2. **JSON-to-FHiCL conversion errors:** Reports serialization failures

```javascript
if(result.first == true) {
  // Parse JSON to validate
  var json = JSON.parse(result.second)

  // Convert back to FHiCL
  var result = mylib.tofhicl(out, result.second)

  if(result.first != true)
    console.log(result.second)  // Report conversion error
  else
    console.log("Wrote file " + out)  // Success
} else {
  console.log(result.second)  // Report parse error
}
```

## Common Use Cases

### Development Testing

- Verify fhicljson native module functionality after builds
- Test format conversion pipeline integrity
- Debug FHiCL parsing issues by examining intermediate JSON

### Continuous Integration

- Run as part of test suite to verify conversion libraries
- Batch test configuration files for compatibility
- Regression testing after library updates

### Web Configuration Editor Support

- The script is part of the artdaq-database web configuration editor infrastructure
- Validates that the Node.js bindings work correctly
- Used during development and deployment verification

## Common Pitfalls

- **Missing Native Module:** Ensure fhicljson.node is built and in the expected location
- **Path Issues:** The relative path calculation assumes standard installation layout
- **Node.js Version:** Native module must be compatible with installed Node.js version
- **File Permissions:** Requires read permission on input, write permission for output

## Exit Behavior

The script exits naturally when processing completes:
- Successful conversion: Prints success message
- Failed conversion: Prints error message
- Both cases: Script exits after output (no explicit exit code)

## Relationship to Other Components

- **fhicljson Native Module:** Provides the actual conversion implementation
- **artdaq_node_server:** Web server that may use similar conversion functionality
- **FHiCL C++ Libraries:** The native module wraps the C++ FHiCL parser

## See Also

- [fhicl2json.cc.md](./fhicl2json.cc.md) - C++ command-line FHiCL to JSON converter
- [conftool.py](./conftool_py.md) - Python tool with fhicl_to_json function
- [External: Node.js Native Addons](https://nodejs.org/api/addons.html) - Documentation on native modules
