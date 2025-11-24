# refactorfhicl.cc

## Overview

`refactorfhicl` is an experimental utility for refactoring FHiCL configuration files by extracting nested table structures into separate files with automatic versioning. It aims to modularize large FHiCL configurations through automatic include generation.

## Purpose and Use Cases

- Refactoring large monolithic FHiCL files into smaller modules
- Extracting reusable configuration tables
- Creating versioned configuration fragments
- Automatic include directive generation
- Configuration modularization and decomposition

## Location

**File**: `/home/user/artdaq-database/artdaq-database/Utilities/refactorfhicl.cc`

## Command-Line Arguments

### Required Arguments

| Argument | Short | Type | Description |
|----------|-------|------|-------------|
| `--config` | `-c` | string | Path to FHiCL configuration file |

### Optional Arguments

| Argument | Short | Type | Description |
|----------|-------|------|-------------|
| `--prune` | `-p` | - | Prune @nil (null) values |
| `--unresolved` | `-u` | - | Do not fully resolve references |
| `--showprolog` | `-s` | - | Show FHiCL prolog section |
| `--help` | `-h` | - | Display help message |

## Main Workflow/Algorithm

```
1. Parse command-line arguments
2. Check/set FHICL_FILE_PATH environment variable
3. Create target directory: "../refactored"
4. Read FHiCL file content
5. Optionally preprocess #include directives
6. Parse FHiCL into extended_value table
7. For each table in configuration:
   a. Extract table content
   b. Generate version string based on content hash
   c. Create filename: {table_name}{version}.fcl
   d. Write table content to separate file
   e. Replace table in main file with #include directive
8. Write refactored main file to target directory
```

## Key Functions

### main()
Primary entry point for refactoring process.

**Steps**:
1. Parse command-line options
2. Validate environment
3. Create output directory
4. Read and optionally preprocess file
5. Parse FHiCL structure
6. Process and output refactored configuration

### lookup_version()
```cpp
auto lookup_version = [](auto const& name, auto const& buffer)
```

Generates version strings for extracted tables:
- Computes hash of table content
- Maintains version map per table name
- Returns `_v1`, `_v2`, etc. based on unique content
- Same content → same version (deduplication)

### operator<<(ostream&, tuple<string, extended_value, size_t, bool>)
Custom output operator with refactoring logic.

**Behavior by Type**:
- **Tables**:
  - Serialize content
  - Create versioned file
  - Output `#include` directive with reference
- **Sequences**: Output as bracketed list
- **Atoms**: Output raw values

## Refactoring Process

### Version Generation

Uses content-based versioning:
```cpp
auto hash = std::hash<std::string>{}(buffer);
```

**Version Map**:
```
TableName → [hash1, hash2, hash3, ...]
```

**Version Assignment**:
- First unique content → `_v1`
- Second unique content → `_v2`
- Duplicate content → same version (deduplicated)

### File Extraction

For each table:
```cpp
auto tmp_file = std::string{key}
    .append(lookup_version(key, ss.str()))
    .append(".fcl");

std::ofstream os(std::string{target_path}
    .append("/")
    .append(tmp_file));
os << ss.str();
```

### Include Generation

Replaces table with include:
```cpp
out << key << " : " << "{\n";
out << "#include \"" << tmp_file << "\"\n";
out << "}\n";
```

## Directory Structure

### Input
```
/some/path/
└── myconfig.fcl
```

### Output
```
/some/path/../refactored/
├── myconfig.fcl          # Main file with includes
├── physics_v1.fcl        # Extracted physics table
├── outputs_v1.fcl        # Extracted outputs table
└── producers_v1.fcl      # Extracted producers table
```

## Usage Examples

### Basic Refactoring
```bash
refactorfhicl -c large_config.fcl
ls ../refactored/
```

### With All Options
```bash
refactorfhicl -c config.fcl --prune --showprolog --unresolved
```

### Refactoring Multiple Files
```bash
for fcl in *.fcl; do
  echo "Refactoring $fcl..."
  refactorfhicl -c "$fcl"
done
```

## Common Scenarios

### Breaking Up Monolithic Config
```bash
# Before: one large 5000-line file
wc -l huge_config.fcl

# Refactor
refactorfhicl -c huge_config.fcl

# After: main file + many smaller modules
ls -la ../refactored/
wc -l ../refactored/*.fcl
```

### Creating Reusable Modules
```bash
# Refactor to extract common tables
refactorfhicl -c run1.fcl
refactorfhicl -c run2.fcl

# Same content gets same version → reusability
diff ../refactored/physics_v1.fcl ../refactored/physics_v1.fcl
```

### Version Tracking
```bash
# Modify config and refactor again
refactorfhicl -c modified_config.fcl

# New content gets new version
ls ../refactored/*_v2.fcl
```

## Implementation Notes

### Incomplete Implementation

**Warning**: The code has commented-out sections:
```cpp
// Lines 54-57: Table iteration commented out
// ::fhicl::extended_value::table_t const& tab = value;
// for (auto& entry : tab) {
//   ss << std::make_tuple<...>(entry.first, entry.second, ...);
// }
```

This means **table content extraction is disabled**:
- Empty content written to extracted files
- Version based on empty string
- All tables get same hash → same version
- **Functionality is broken**

### Current State

As implemented, the utility:
1. Creates output directory ✓
2. Parses FHiCL ✓
3. Identifies tables ✓
4. Creates include directives ✓
5. **Does NOT extract actual table content** ✗
6. Writes empty extracted files ✗

### To Make Functional

Uncomment lines 54-57 to enable content extraction:
```cpp
if (value.is_a(::fhicl::TABLE)) {
  out << key << " : " << "{\n";
  std::stringstream ss;
  ::fhicl::extended_value::table_t const& tab = value;
  for (auto& entry : tab) {
    ss << std::make_tuple<std::string const&, fhicl::extended_value const&,
         std::size_t, bool>(entry.first, entry.second, il + 1, in_prolog);
  }
  // ... rest of code
}
```

## Design Patterns

### Content-Based Versioning
- Hash-based deduplication
- Automatic version increment
- Version map persists across tables

### File Generation
- Lazy file creation (on first write)
- Separate file per unique table version
- Main file references extracted files

### Include Replacement
- In-place table → include transformation
- Maintains FHiCL structure validity
- Preserves table names as keys

## Environment Variables

### FHICL_FILE_PATH

Same as other FHiCL utilities:
- Search path for includes
- Defaults to current directory
- Colon-separated list

## Error Handling

### Directory Creation
```cpp
if (!artdaq::database::mkdir(target_path)) {
  std::cerr << "ERROR: unable to create the \"" << target_path << "\" directory.";
  return 3;
}
```

### Exit Codes
- `0`: Success
- `-1`: Command-line parsing error
- `1`: Help displayed
- `2`: No configuration file specified
- `3`: Unable to create output directory
- `UNCAUGHT_EXCEPTION`: Parse or processing error

## Limitations

### Fixed Output Directory
Hardcoded to `../refactored`:
```cpp
auto target_path = std::string{"../refactored"};
```
Not configurable via command line.

### No Depth Control
Extracts all top-level tables:
- Cannot control which tables to extract
- Cannot specify extraction depth
- All-or-nothing approach

### No Nested Extraction
Only processes top-level tables:
- Nested tables not extracted to separate files
- Could result in still-large extracted files

### Single-Level Include
Include directives not nested:
```
main.fcl includes table1.fcl
table1.fcl contains nested tables (not further extracted)
```

### Broken Table Extraction
As noted, table content extraction is commented out.

## Dependencies

- `artdaq-database/SharedCommon/fileststem_functions.h` (note typo: "fileststem")
- `artdaq-database/SharedCommon/printStackTrace.h`
- `artdaq-database/SharedCommon/process_exit_codes.h`
- `cetlib/includer.h`
- `fhiclcpp/extended_value.h`
- `fhiclcpp/intermediate_table.h`
- `fhiclcpp/parse.h`
- `fhiclcpp/parse_shims.h`
- Boost.Program_options

## Related Utilities

- **readfhicl**: Displays FHiCL structure (useful for planning refactoring)
- **fhicl2json**: Alternative format conversion
- **conftool**: Manages configurations (may work with refactored files)

## Potential Enhancements

To make this utility fully functional and useful:

1. **Fix Table Extraction**: Uncomment lines 54-57
2. **Configurable Output Path**: Add command-line option
3. **Selective Extraction**: Choose which tables to extract
4. **Depth Control**: Specify nesting levels
5. **Recursive Extraction**: Extract nested tables too
6. **Dry-Run Mode**: Preview without writing files
7. **Include Path Handling**: Adjust includes based on output location
8. **Progress Reporting**: Show extraction progress
9. **Validation**: Verify refactored config parses correctly
10. **Merge Detection**: Warn if same version has different content

## Testing Recommendations

Before using in production:

1. **Fix the bug**: Uncomment table iteration code
2. **Test on simple config**: Verify content extraction works
3. **Test version generation**: Ensure deduplication works
4. **Test include resolution**: Verify refactored config parses
5. **Compare parsed results**: Original vs refactored should be equivalent

```bash
# Test equivalence
readfhicl -c original.fcl -r > original_resolved.txt
cd ../refactored
readfhicl -c original.fcl -r > refactored_resolved.txt
diff original_resolved.txt refactored_resolved.txt
```

## Status

**Current Status**: **Experimental / Incomplete**

The utility is not production-ready due to commented-out code. It appears to be a work-in-progress or prototype. Use with caution and expect to need modifications.
