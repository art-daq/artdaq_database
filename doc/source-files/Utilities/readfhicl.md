# readfhicl.cc

## Overview

`readfhicl` is a utility for reading, parsing, and displaying FHiCL (Fermilab Hierarchical Configuration Language) files with various processing options. It provides capabilities for pruning, resolving, and displaying prolog sections.

## Purpose and Use Cases

- Viewing FHiCL configurations in formatted output
- Debugging FHiCL files with full resolution of references
- Examining prolog sections separately
- Testing FHiCL parsing without includes
- Validating FHiCL syntax and structure

## Location

**File**: `/home/user/artdaq-database/artdaq-database/Utilities/readfhicl.cc`

## Command-Line Arguments

### Required Arguments

| Argument | Short | Type | Description |
|----------|-------|------|-------------|
| `--config` | `-c` | string | Path to FHiCL configuration file |

### Optional Arguments

| Argument | Short | Type | Description |
|----------|-------|------|-------------|
| `--prune` | `-p` | - | Prune @nil (null) values from output |
| `--resolve` | `-r` | - | Fully resolve all references and includes |
| `--showprolog` | `-s` | - | Display prolog section separately |
| `--help` | `-h` | - | Display help message |

## Main Workflow/Algorithm

```
1. Parse command-line arguments
2. Check/set FHICL_FILE_PATH environment variable
3. Read FHiCL file content
4. If NOT fully resolving:
   a. Replace #include directives with placeholder names
   b. Enable snippet mode for partial parsing
5. Parse FHiCL document into intermediate table
6. If show prolog enabled:
   a. Output "BEGIN_PROLOG"
   b. Display all prolog entries
   c. Output "END_PROLOG"
7. Display main document content
8. Optionally prune @nil values
```

## Key Functions

### main()
Primary entry point that processes and displays FHiCL content.

**Steps**:
1. Parse command-line options
2. Validate environment (FHICL_FILE_PATH)
3. Read configuration file
4. Conditionally preprocess includes
5. Parse FHiCL into extended_value table
6. Format and display output

### operator<<(ostream&, tuple<extended_value, size_t, bool>)
Custom output operator for formatted FHiCL display.

**Formatting Rules**:
- **Tables**: Indented with braces
- **Sequences**: Bracketed with square brackets
- **Atoms**: Raw values
- **Indentation**: Spaces proportional to nesting level
- **Prolog filtering**: Only shows values matching prolog flag

### Include Processing

When `--resolve` is NOT specified, transforms include directives:

```cpp
// Before:
#include "common.fcl"

// After:
fhicl_pound_include_0: "common.fcl"
```

This allows parsing without actually including files.

## Environment Variables

### FHICL_FILE_PATH

**Purpose**: Search path for included FHiCL files

**Default**: Current directory (`.`) if not set

**Usage**:
```bash
export FHICL_FILE_PATH="/configs:/configs/common:."
readfhicl -c myconfig.fcl
```

## Display Modes

### Standard Mode (no flags)
Displays main document without prolog or nil values:
```
key1: value1
key2: {
 nested1: value2
 nested2: value3
}
```

### With Prolog Display (--showprolog)
```
BEGIN_PROLOG
prolog_var1: value1
prolog_var2: value2
END_PROLOG

main_key1: value1
main_key2: value2
```

### Pruned Mode (--prune)
Removes all `@nil` entries from output:
```
# Without --prune:
key1: value1
key2: @nil
key3: value3

# With --prune:
key1: value1
key3: value3
```

### Resolved Mode (--resolve)
Fully processes all includes and references:
- All `#include` directives processed
- All references resolved
- Complete configuration shown

### Snippet Mode (no --resolve)
Processes file as a snippet without full resolution:
- Includes converted to placeholders
- References may remain unresolved
- Faster for examining file structure

## Usage Examples

### Basic Display
```bash
readfhicl -c myconfig.fcl
```

### Show With Prolog
```bash
readfhicl -c myconfig.fcl --showprolog
```

### Fully Resolved
```bash
export FHICL_FILE_PATH="/configs:/configs/common"
readfhicl -c myconfig.fcl --resolve
```

### Pruned Output
```bash
readfhicl -c myconfig.fcl --prune
```

### All Options Combined
```bash
readfhicl -c myconfig.fcl -r -p -s
```

### Comparing Resolved vs Unresolved
```bash
# See file structure
readfhicl -c config.fcl > unresolved.txt

# See fully expanded
readfhicl -c config.fcl -r > resolved.txt

# Compare
diff unresolved.txt resolved.txt
```

## Common Scenarios

### Debugging Configuration
View exactly what FHiCL parser sees:
```bash
readfhicl -c problem.fcl -r -s
```

### Examining Includes
See which variables come from includes:
```bash
# Without resolution (shows placeholders)
readfhicl -c config.fcl

# With resolution (shows expanded)
readfhicl -c config.fcl -r
```

### Validating Syntax
Quick syntax check:
```bash
if readfhicl -c test.fcl > /dev/null 2>&1; then
  echo "Valid FHiCL syntax"
else
  echo "Syntax error in test.fcl"
fi
```

### Extracting Prolog
Get just the prolog section:
```bash
readfhicl -c config.fcl -s | \
  sed -n '/BEGIN_PROLOG/,/END_PROLOG/p'
```

### Cleaning Configuration
Remove nil values for cleaner output:
```bash
readfhicl -c messy.fcl -p > clean.fcl
```

## FHiCL Structure Handling

### Tables
Displayed with braces and indentation:
```
physics: {
 producers: {
  generator: {
   module_type: "MyGenerator"
  }
 }
}
```

### Sequences
Displayed with square brackets:
```
trigger_paths: [ path1 path2 path3 ]

outputs: [
 {
  module_type: "RootOutput"
  fileName: "output.root"
 }
]
```

### Prolog Section
Special BEGIN_PROLOG/END_PROLOG block:
```
BEGIN_PROLOG
standard_producer: {
  module_type: "StandardProducer"
  param1: 10
}
END_PROLOG

physics.producers.myprod: @local::standard_producer
```

### Nil Values
FHiCL's null representation:
```
# Explicitly unset
unset_value: @nil

# Can be pruned with --prune flag
```

## Error Handling

### Parse Errors
FHiCL syntax errors result in exception:
```
Error: Parse error at line 42: unexpected token
```

### File Not Found
Missing configuration file:
```
Error: Cannot open file: config.fcl
```

### Include Errors (with --resolve)
Missing included files:
```
Error: Include file not found: common.fcl
Check: FHICL_FILE_PATH environment variable
```

### Exit Codes
- `0`: Success
- `-1`: Command-line parsing error
- `1`: Help displayed
- `2`: No configuration file specified
- `UNCAUGHT_EXCEPTION`: Parsing or processing error

## Implementation Details

### Extended Value Table
Uses `fhicl::extended_value` and `fhicl::intermediate_table`:
- Preserves structure during parsing
- Maintains prolog information
- Supports recursive traversal

### Snippet Mode
```cpp
::shims::isSnippetMode(true);
```
Enables parsing without full resolution, useful for examining file structure.

### Include Preprocessing
Regex-based replacement of include directives:
```cpp
std::regex{"(#include\\s)([^'\"]*)"};
// Replaces: #include file.fcl
// With: fhicl_pound_include_0: file.fcl
```

### Recursive Display
Custom operator<< recursively traverses nested structures:
- Tables: Iterate over entries, indent output
- Sequences: Iterate over elements, bracket output
- Atoms: Direct value output

## Dependencies

- `artdaq-database/SharedCommon/printStackTrace.h`
- `artdaq-database/SharedCommon/process_exit_codes.h`
- `cetlib/includer.h` - FHiCL include handling
- `fhiclcpp/extended_value.h` - FHiCL value representation
- `fhiclcpp/intermediate_table.h` - FHiCL table structure
- `fhiclcpp/parse.h` - FHiCL parser
- `fhiclcpp/parse_shims.h` - Parser utilities
- Boost.Program_options for argument parsing

## Related Utilities

- **fhicl2json**: Converts FHiCL to JSON format
- **refactorfhicl**: Refactors FHiCL by extracting tables
- **conftool**: Database operations with FHiCL support

## Performance Considerations

### Snippet Mode vs Full Resolution
- **Snippet mode** (default): Fast, minimal processing
- **Full resolution** (--resolve): Slower, processes all includes

### Memory Usage
- Entire file loaded into memory
- Proportional to file size and include depth
- Reasonable for typical configuration files (< 10MB)

### Output Generation
- Recursive traversal of structure
- Memory allocation for string formatting
- Output buffering by iostream

## Debugging Tips

### Troubleshooting Include Problems
```bash
# Set verbose include path
export FHICL_FILE_PATH="/a:/b:/c"
echo "Search path: $FHICL_FILE_PATH"

# Try resolved mode
readfhicl -c config.fcl -r 2>&1 | grep -i include
```

### Finding Reference Issues
```bash
# Compare snippet vs resolved
readfhicl -c config.fcl > snippet.txt
readfhicl -c config.fcl -r > resolved.txt
diff snippet.txt resolved.txt
```

### Locating Nil Values
```bash
# Show all entries
readfhicl -c config.fcl > all.txt

# Show without nils
readfhicl -c config.fcl -p > nonils.txt

# See what was removed
diff all.txt nonils.txt | grep '@nil'
```
