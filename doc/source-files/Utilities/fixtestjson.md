# fixtestjson.cc

## Overview

`fixtestjson` is a specialized utility for migrating JSON documents to use string values exclusively. It converts all non-string primitive values (numbers, booleans) to their string representations, which may be required for certain database schema versions.

## Purpose and Use Cases

- Migrating JSON documents to string-only schemas
- Fixing type mismatches in test data
- Converting databases that require string values
- Schema migration for older database formats
- Data normalization to string representation

## Location

**File**: `/home/user/artdaq-database/artdaq-database/Utilities/fixtestjson.cc`

## Command-Line Arguments

### Required Arguments

| Argument | Short | Type | Description |
|----------|-------|------|-------------|
| `--config` | `-c` | string | Path to JSON file to fix |

### Optional Arguments

| Argument | Short | Type | Description |
|----------|-------|------|-------------|
| `--outputformat` | `-f` | string | Output file format (not currently used) |
| `--help` | `-h` | - | Display help message |

## Main Workflow/Algorithm

```
1. Parse command-line arguments
2. Read JSON file into string buffer
3. Parse JSON into AST (object_t)
4. Navigate to "document.data" node
5. Recursively convert all values to strings:
   a. For objects: recurse into each property
   b. For arrays: recurse into each element
   c. For primitive values: apply tostring_visitor()
6. Serialize modified AST back to JSON
7. Overwrite original file with converted JSON
```

## Key Functions

### main()
Primary entry point that orchestrates the conversion.

**Steps**:
1. Parse command-line options
2. Read JSON file
3. Parse into AST
4. Locate document.data node
5. Apply string migration
6. Serialize back to JSON
7. Write to file

### migrate_to_string_value()
```cpp
bool migrate_to_string_value(jsn::value_t& value)
```

Recursively converts values to strings:

**By Value Type**:
- **ARRAY**: Recurse into each element
- **OBJECT**: Recurse into each property
- **VALUE**: Apply `tostring_visitor()` to convert to string

**Returns**: `true` on success, `false` on unsupported type

## Type Migration

### Conversion Examples

**Numbers to Strings**:
```json
// Before:
{"count": 42, "rate": 3.14}

// After:
{"count": "42", "rate": "3.14"}
```

**Booleans to Strings**:
```json
// Before:
{"enabled": true, "debug": false}

// After:
{"enabled": "true", "debug": "false"}
```

**Nested Structures**:
```json
// Before:
{
  "config": {
    "channels": [1, 2, 3],
    "active": true
  }
}

// After:
{
  "config": {
    "channels": ["1", "2", "3"],
    "active": "true"
  }
}
```

### Preserved Types
- **Strings**: Already strings, unchanged
- **Objects**: Structure preserved, contents converted
- **Arrays**: Structure preserved, contents converted

## Document Structure

### Expected JSON Format

The utility expects database document format:
```json
{
  "document": {
    "data": {
      // This section is migrated
      "field1": 123,
      "field2": true
    }
  }
}
```

### Navigation

Uses nested property access:
```cpp
auto& document = get_SubNode(doc_ast, literal::document);
// Then accesses: document.at(literal::data)
```

Where `literal::document` and `literal::data` are string constants.

## File Operations

### In-Place Modification

**Warning**: Overwrites the original file:
```cpp
db::write_buffer_to_file(json_converted, file_name)
```

**No Backup Created**: Original data is lost if conversion fails or produces incorrect results.

## Usage Examples

### Basic Conversion
```bash
fixtestjson -c document.json
```

### Batch Conversion
```bash
for json in test_data/*.json; do
  echo "Converting $json..."
  cp "$json" "$json.backup"  # Create backup first!
  fixtestjson -c "$json"
done
```

### With Backup
```bash
# Safe usage pattern
cp important.json important.json.backup
fixtestjson -c important.json
# Verify result before deleting backup
diff -u important.json.backup important.json
```

### Verification
```bash
# Before conversion
jq '.document.data' before.json

# Convert
cp before.json after.json
fixtestjson -c after.json

# Compare
jq '.document.data' after.json
```

## Common Scenarios

### Preparing Test Data
Convert test JSON files to match schema requirements:
```bash
for test in tests/*.json; do
  echo "Fixing $test"
  cp "$test" "$test.orig"
  fixtestjson -c "$test"
done
```

### Database Schema Migration
Migrate entire database collection:
```bash
#!/bin/bash
DB_PATH="/var/lib/artdaq/database/TestCollection"
for doc in "$DB_PATH"/*.json; do
  [[ "$doc" == */index.json ]] && continue  # Skip index
  echo "Migrating $doc"
  cp "$doc" "$doc.backup"
  fixtestjson -c "$doc"
done
```

### Fixing Import Failures
When database rejects numeric values:
```bash
# Import fails with type error
conftool --import data.json  # Error: expected string, got number

# Fix the data
cp data.json data.json.orig
fixtestjson -c data.json

# Retry import
conftool --import data.json  # Success
```

## Error Handling

### Parse Failures
If JSON is malformed:
```cpp
if (!jsn::JsonReader{}.read(json, doc_ast)) {
  confirm(false);  // Assertion failure
}
```

### Migration Failures
If unsupported type encountered:
```cpp
if (!migrate_to_string_value(document.at(literal::data))) {
  return process_exit_code::FAILURE;
}
```

### Write Failures
If file cannot be written:
```cpp
if (!db::write_buffer_to_file(json_converted, file_name)) {
  confirm(false);  // Assertion failure
}
```

### Exit Codes
- `SUCCESS` (0): Conversion successful
- `INVALID_ARGUMENT`: Command-line error
- `HELP`: Help displayed
- `INVALID_ARGUMENT | 1`: No config file specified
- `FAILURE`: Migration failed (unsupported type)
- `UNCAUGHT_EXCEPTION`: Unhandled exception

## Implementation Details

### Visitor Pattern

Uses `tostring_visitor()`:
```cpp
value = boost::apply_visitor(jsn::tostring_visitor(), value);
```

Boost.Variant visitor that converts any variant type to string.

### Lambda Helper

Uses lambda for navigation:
```cpp
auto get_SubNode = [](auto& parent, auto const& child_name) -> auto& {
  return boost::get<jsn::object_t>(parent.at(child_name));
};
```

Safely extracts nested object nodes with type checking.

### Recursive Processing

The `migrate_to_string_value()` function:
1. Identifies value type
2. Recurses for containers (objects, arrays)
3. Converts for leaf values
4. Returns success/failure

### In-Place Modification

The conversion modifies the AST in-place:
```cpp
bool migrate_to_string_value(jsn::value_t& value)  // Note: non-const reference
```

## Safety Considerations

### Data Loss Risk

**Critical**: This utility overwrites original files!

**Recommended Safety Measures**:
1. Always backup before conversion
2. Test on copies first
3. Verify results before deploying
4. Keep backups until thoroughly tested

### Irreversibility

**String to Number is Lossy**:
```json
// Original (precision preserved):
{"pi": 3.141592653589793}

// After conversion (string):
{"pi": "3.141592653589793"}

// Cannot distinguish from:
{"pi": "3.141592653589793"}  // Was already a string
```

Metadata about original types is lost.

### Validation Recommended

After conversion:
```bash
# Verify JSON is valid
jq empty converted.json || echo "Invalid JSON!"

# Verify schema compliance
validate_schema converted.json schema.json

# Spot-check values
jq '.document.data | to_entries | .[] | select(.value | type != "string")' converted.json
```

## Use Cases

### When Needed

This utility addresses specific scenarios:

1. **Schema Requirement**: Database expects all string values
2. **Test Data Migration**: Old test data has mixed types
3. **Import Compatibility**: Importer rejects non-string values
4. **Legacy Database**: Older database version requirements

### When NOT Needed

Don't use if:
- Database supports typed values (most do)
- Precise numeric types needed (scientific data)
- Type information is semantically important
- Schema allows mixed types

## Performance Considerations

### Efficiency
- Single-pass recursive traversal
- In-memory processing
- File I/O is main bottleneck

### Scalability
- Memory usage proportional to JSON size
- Suitable for typical document sizes (< 10MB)
- Large documents may cause memory pressure

### Bottlenecks
1. File I/O (read and write)
2. JSON parsing
3. JSON serialization
4. Recursive traversal

## Dependencies

- `artdaq-database/DataFormats/Json/json_common.h`
- `test/common.h`
- Boost.Variant (for visitor pattern)
- Boost.Program_options

## Related Utilities

- **migrate_database**: More comprehensive database migration
- **readjson**: Testing JSON parsing (can verify results)
- **fhicl2json**: Creates JSON that may need fixing

## Alternative Approaches

### Schema-Based Conversion
More robust alternative:
```python
import json

def convert_by_schema(data, schema):
    # Convert based on schema type definitions
    # Preserves semantic type information
    pass
```

### Selective Conversion
Convert only specific fields:
```python
def convert_selective(data, fields_to_convert):
    # Only convert specified fields
    # Leaves others unchanged
    pass
```

### Validation + Conversion
Validate before converting:
```bash
# Validate first
if ! validate_json "$file"; then
  echo "Invalid JSON, skipping"
  exit 1
fi

# Then convert
fixtestjson -c "$file"

# Validate again
validate_json "$file"
```

## Troubleshooting

### Common Issues

**File unchanged after running**:
```
Cause: Parse or migration failure (silent)
Solution: Check exit code, look for error messages
```

**Values still not strings**:
```
Cause: Values outside document.data not converted
Solution: Only document.data is migrated
```

**JSON corrupted**:
```
Cause: Write failure or serialization issue
Solution: Restore from backup, check disk space/permissions
```

**Type conversion unexpected**:
```
Cause: tostring_visitor behavior
Solution: Check specific conversion rules for your types
```

## Best Practices

### Safe Conversion Workflow

```bash
#!/bin/bash
# Safe conversion script

FILE="$1"
BACKUP="${FILE}.backup.$(date +%Y%m%d_%H%M%S)"

# 1. Create backup
cp "$FILE" "$BACKUP"

# 2. Validate original
if ! jq empty "$FILE" 2>/dev/null; then
  echo "Original file is invalid JSON"
  exit 1
fi

# 3. Convert
if ! fixtestjson -c "$FILE"; then
  echo "Conversion failed, restoring backup"
  mv "$BACKUP" "$FILE"
  exit 1
fi

# 4. Validate result
if ! jq empty "$FILE" 2>/dev/null; then
  echo "Result is invalid JSON, restoring backup"
  mv "$BACKUP" "$FILE"
  exit 1
fi

# 5. Verify all values are strings
if jq '.document.data | .. | select(type != "string" and type != "object" and type != "array")' "$FILE" | grep -q .; then
  echo "Warning: Some non-string values remain"
fi

echo "Conversion successful. Backup saved as $BACKUP"
```

### Testing

Before production use:
1. Test on sample data
2. Verify database accepts converted data
3. Check application behavior with string values
4. Ensure no precision loss in critical fields
5. Document conversion for reproducibility
