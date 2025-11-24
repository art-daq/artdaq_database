# fhicl2json.cc

## Overview

`fhicl2json` is a utility for converting FHiCL (Fermilab Hierarchical Configuration Language) configuration files to JSON format. It supports extracting specific subtrees from the converted JSON output.

## Purpose and Use Cases

- Converting FHiCL configurations to JSON format
- Extracting specific configuration subtrees
- Preparing configurations for database storage
- Configuration format translation and interoperability
- Testing FHiCL parsing and JSON conversion

## Location

**File**: `/home/user/artdaq-database/artdaq-database/Utilities/fhicl2json.cc`

## Command-Line Arguments

### Required Arguments

| Argument | Short | Type | Description |
|----------|-------|------|-------------|
| `--config` | `-c` | string | Path to FHiCL configuration file |

### Optional Arguments

| Argument | Short | Type | Description |
|----------|-------|------|-------------|
| `--main` | `-m` | - | Extract only the "document.data.main" subtree |
| `--help` | `-h` | - | Display help message |

## Main Workflow/Algorithm

```
1. Parse command-line arguments
2. Check/set FHICL_FILE_PATH environment variable
3. Read FHiCL file content into buffer
4. Convert FHiCL to JSON using fhicl_to_json()
5. If --main flag set:
   a. Parse JSON as JSONDocument
   b. Navigate to "document.data.main" subtree
   c. Extract that subtree as JSON string
6. Output resulting JSON to stdout
```

## Key Functions

### main()
Primary entry point that orchestrates the conversion process.

**Steps**:
1. Parse command-line options using Boost.Program_options
2. Validate FHICL_FILE_PATH environment variable
3. Read configuration file
4. Convert FHiCL to JSON
5. Optionally extract main subtree
6. Output result

### fhicl_to_json()
```cpp
result_t fhicl_to_json(std::string const& fcl, std::string const& filename)
```

Wrapper around the library conversion function:
- Sets default locale for consistent number formatting
- Calls `artdaq::database::fhicljson::fhicl_to_json()`
- Returns `std::pair<bool, std::string>` (success, result/error)

### json_to_fhicl()
```cpp
result_t json_to_fhicl(std::string const& jsn, std::string& filename)
```

Reverse conversion (defined but not used in this utility):
- Converts JSON back to FHiCL format
- Primarily for library completeness
- May be used by other tools

## Environment Variables

### FHICL_FILE_PATH

**Purpose**: Specifies directories to search for `#include` files in FHiCL

**Default**: Current directory (`.`) if not set

**Format**: Colon-separated list of directories (Unix PATH-style)
```bash
export FHICL_FILE_PATH="/path/to/configs:/another/path:."
```

**Behavior**:
- If not set, utility prints INFO message and sets to current directory
- Used by fhicl parser to resolve `#include` directives

## Subtree Extraction

### Main Subtree Flag

When `--main` is specified, extracts specific JSON path:

```
Full JSON:                    Extracted (--main):
{                             {
  "document": {                 "key1": "value1",
    "data": {                   "key2": "value2"
      "main": {           →   }
        "key1": "value1",
        "key2": "value2"
      }
    }
  }
}
```

### JSONDocument Navigation

Uses `JSONDocument::findChild()` to navigate JSON tree:
```cpp
JSONDocument(result.second).findChild("document.data.main").to_string()
```

Path notation uses dot-separated keys for nested access.

## Usage Examples

### Basic Conversion
```bash
fhicl2json -c myconfig.fcl > myconfig.json
```

### With Include Paths
```bash
export FHICL_FILE_PATH="/configs:/configs/common:."
fhicl2json --config production.fcl > production.json
```

### Extract Main Section
```bash
fhicl2json -c myconfig.fcl -m > main_section.json
```

### Conversion with Error Handling
```bash
if fhicl2json -c config.fcl > output.json 2> errors.log; then
  echo "Conversion successful"
else
  echo "Conversion failed, see errors.log"
  cat errors.log
fi
```

### Pipeline Processing
```bash
# Convert and pretty-print
fhicl2json -c config.fcl | python -m json.tool

# Convert and validate
fhicl2json -c config.fcl | jq empty && echo "Valid JSON"

# Extract specific field
fhicl2json -c config.fcl | jq '.document.data.version'
```

## Common Scenarios

### Preparing for Database Import
Convert FHiCL to JSON before database storage:
```bash
for fcl in *.fcl; do
  fhicl2json -c "$fcl" > "${fcl%.fcl}.json"
done
```

### Configuration Validation
Verify FHiCL syntax by attempting conversion:
```bash
if fhicl2json -c suspicious.fcl > /dev/null 2>&1; then
  echo "Valid FHiCL syntax"
else
  echo "Invalid FHiCL syntax"
fi
```

### Comparing Configurations
Convert both to JSON and use JSON diff tools:
```bash
fhicl2json -c config1.fcl > /tmp/config1.json
fhicl2json -c config2.fcl > /tmp/config2.json
diff <(jq -S . /tmp/config1.json) <(jq -S . /tmp/config2.json)
```

### Extracting Specific Configuration
Get just the main configuration content:
```bash
fhicl2json -c full_config.fcl -m | jq . > main_config.json
```

## Error Handling

### Conversion Failures
- Invalid FHiCL syntax → Error message to stderr
- Missing include files → Error with filename
- Parse errors → Diagnostic information

### Exit Codes
- `SUCCESS` (0): Conversion successful
- `INVALID_ARGUMENT` (-1): Command-line parsing error
- `HELP` (1): Help message displayed
- `INVALID_ARGUMENT` (2): No configuration file specified
- `FAILURE`: Conversion failed
- `UNCAUGHT_EXCEPTION`: Unhandled exception

### Error Output
All errors go to stderr, leaving stdout clean for JSON output:
```bash
# Redirect separately
fhicl2json -c config.fcl 2> errors.txt > output.json

# Or discard errors
fhicl2json -c config.fcl 2> /dev/null > output.json
```

## FHiCL Features Supported

The conversion supports full FHiCL syntax including:
- Nested tables and sequences
- Include directives
- References and substitutions
- Prolog sections
- Comments (not preserved in JSON)
- Type annotations

## JSON Output Format

### Standard Output
Complete database document format with metadata:
```json
{
  "document": {
    "data": {
      "main": { /* configuration content */ }
    }
  }
}
```

### Main-Only Output (--main)
Just the configuration content:
```json
{
  /* configuration content directly */
}
```

## Implementation Notes

### Locale Handling
Sets default locale for consistent number formatting:
```cpp
artdaq::database::set_default_locale();
```
Ensures consistent decimal point representation across systems.

### File Reading
Uses iostream iterator for efficient file reading:
```cpp
std::ifstream is(file_name);
std::string buffer((std::istreambuf_iterator<char>(is)),
                   std::istreambuf_iterator<char>());
```

### Error Propagation
Uses `result_t = std::pair<bool, std::string>` for error handling:
- First element: success/failure boolean
- Second element: result string or error message

## Dependencies

- `artdaq-database/ConfigurationDB/configurationdb.h`
- `artdaq-database/DataFormats/Fhicl/fhicljsondb.h`
- `artdaq-database/DataFormats/Json/json_common.h`
- `artdaq-database/JsonDocument/JSONDocument.h`
- `artdaq-database/SharedCommon/printStackTrace.h`
- `artdaq-database/SharedCommon/process_exit_codes.h`
- Boost.Program_options for argument parsing

## Related Utilities

- **readfhicl**: Reads and displays FHiCL with various options
- **refactorfhicl**: Refactors FHiCL files by extracting tables
- **bulkloader**: Uses FHiCL to JSON conversion internally
- **conftool**: Can convert between formats as part of database operations

## Performance Considerations

### Efficiency
- Single-pass conversion
- In-memory processing (entire file loaded)
- Memory usage proportional to file size

### Scalability
- Suitable for configuration files (typically < 1MB)
- Not optimized for very large files
- Memory-bound for huge configurations

### Bottlenecks
- File I/O (reading input)
- FHiCL parsing complexity (includes, references)
- JSON serialization

## Troubleshooting

### Common Issues

**Missing include files**:
```
Error: #include file not found: common.fcl
Solution: Set FHICL_FILE_PATH to include directory
```

**Invalid FHiCL syntax**:
```
Error: Parse error at line 42
Solution: Check FHiCL syntax, missing braces, etc.
```

**Empty output**:
```
Cause: Conversion failed, check stderr
Solution: Run with 2>&1 to see error messages
```

**Path not found (--main)**:
```
Cause: JSON doesn't have expected structure
Solution: Verify JSON structure, omit --main flag
```
