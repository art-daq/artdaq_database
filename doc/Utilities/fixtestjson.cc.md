# fixtestjson.cc

**Path:** `artdaq-database/Utilities/fixtestjson.cc`

**Purpose:** JSON value type migration utility that converts all values in the `document.data` subtree to string type. This tool ensures schema compatibility when the database expects all configuration values to be stored as strings rather than native JSON types.

## Overview

The `fixtestjson` utility reads a JSON document, recursively converts all non-container values (numbers, booleans, null) to their string representations, and writes the modified document back to the same file. This is useful for migrating documents to a schema that uniformly stores all values as strings.

**Warning:** This tool overwrites the original file in place. Always back up your data before running.

## Key Concepts

### String Value Schema

Some database schemas require all configuration values to be stored as strings, even if they represent numbers or booleans. This design choice:
- Ensures consistent handling across different value types
- Avoids type-related issues during format conversions (e.g., FHiCL to JSON)
- Simplifies schema validation

### Recursive Migration

The `migrate_to_string_value` function recursively processes the JSON AST:
- **Arrays:** Each element is processed recursively
- **Objects:** Each value is processed recursively
- **Primitive Values:** Converted to string representation using `tostring_visitor`

### Target Subtree

Only the `document.data` subtree is processed. This preserves document metadata while converting all configuration data values to strings.

### In-Place Modification

Unlike most utilities that output to stdout or a new file, this tool:
1. Reads the original file
2. Modifies the in-memory AST
3. Writes back to the same file path

## Thread Safety

- **Thread Safety:** Not thread-safe
- **Concurrent Access:** Do not run multiple instances on the same file
- **Locking:** No file locking; relies on exclusive access

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/Json/json_common.h` | JSON type definitions, `JsonReader`, `JsonWriter`, `tostring_visitor` |
| `test/common.h` | Common test utilities, `read_buffer_from_file`, `write_buffer_to_file`, process exit codes |
| `boost/program_options.hpp` | Command-line parsing |

## Command-Line Options

| Option | Short | Required | Description |
|--------|-------|----------|-------------|
| `--config` | `-c` | Yes | Path to JSON file to modify (will be overwritten) |
| `--outputformat` | `-f` | No | Output format (accepted but not currently used) |
| `--help` | `-h` | No | Display help message |

## Functions

### `main(argc, argv) -> int`

**Brief:** Entry point that parses command-line arguments, reads a JSON file, navigates to the `document.data` subtree, recursively converts all values to strings, and writes the modified document back to the same file.

**Parameters:**
- `argc` - Number of command-line arguments
- `argv` - Array of command-line argument strings

**Preconditions:**
- The JSON file specified by `--config` must exist and be readable/writable
- The file must contain valid JSON with `document.data` structure

**Returns:**
- `process_exit_code::SUCCESS` (0) - Migration completed successfully
- `process_exit_code::HELP` (1) - Help message displayed
- `process_exit_code::FAILURE` (1) - Migration failed
- `process_exit_code::INVALID_ARGUMENT` (128) - Missing required arguments
- `process_exit_code::INVALID_ARGUMENT | 1` (129) - Missing `--config` argument
- `process_exit_code::UNCAUGHT_EXCEPTION` (144) - Unhandled exception occurred

**Postconditions:**
- The original file is overwritten with the migrated content
- All values in `document.data` are strings

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `bpo::error` | Command-line parsing error (caught and reported) |
| (assertion failure) | JSON parsing or writing fails (via `confirm()`) |
| (caught at top level) | All other exceptions caught and reported |

**Thread Safety:** Not thread-safe; single-threaded execution

**Side Effects:**
- Reads and overwrites the specified file
- May cause assertion failure and program termination on errors

### `migrate_to_string_value(value) -> bool`

**Brief:** Recursively converts all primitive values (numbers, booleans, null) in a JSON value tree to their string representations, preserving the structure of arrays and objects.

**Parameters:**
- `value` - Reference to a JSON value to convert in-place

**Preconditions:**
- `value` must be a valid JSON value (object, array, or primitive)

**Returns:**
- `true` if migration succeeded for this value and all its children
- `false` if an unknown type is encountered

**Postconditions:**
- All primitive values in the tree are converted to strings
- Container structure (arrays, objects) is preserved

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Does not throw; returns false on error |

**Thread Safety:** Not thread-safe (modifies shared data structures)

**Side Effects:**
- Modifies the `value` parameter in-place

**Example:**
```cpp
jsn::value_t value = 42;  // Integer
bool success = migrate_to_string_value(value);
// value is now "42" (string)

jsn::object_t obj;
obj["count"] = 100;
obj["enabled"] = true;
jsn::value_t obj_value = obj;
migrate_to_string_value(obj_value);
// obj["count"] is now "100", obj["enabled"] is now "true"
```

## Usage Examples

### Convert JSON Values to Strings

```bash
# IMPORTANT: Backup first!
cp config.json config.json.bak

# Convert all values to strings
fixtestjson -c config.json
```

### Verify Changes

```bash
# Compare before and after
diff config.json.bak config.json
```

### Batch Processing

```bash
# Process multiple files (with backups)
for f in *.json; do
  cp "$f" "$f.bak"
  fixtestjson -c "$f"
done
```

## Implementation Details

### Migration Function

```cpp
bool migrate_to_string_value(jsn::value_t& value) {
  switch (jsn::type(value)) {
    case jsn::type_t::ARRAY: {
      auto& data = boost::get<jsn::array_t>(value);
      for (auto& element : data) {
        if (!migrate_to_string_value(element)) {
          return false;
        }
      }
    } break;

    case jsn::type_t::OBJECT: {
      auto& data = boost::get<jsn::object_t>(value);
      for (auto& element : data) {
        if (!migrate_to_string_value(element.value)) {
          return false;
        }
      }
    } break;

    case jsn::type_t::VALUE:
      // Convert primitive value to string
      value = boost::apply_visitor(jsn::tostring_visitor(), value);
      break;

    default:
      return false;  // Unknown type
  }
  return true;
}
```

### Processing Flow

1. **Read File:**
   ```cpp
   auto json = std::string{};
   db::read_buffer_from_file(json, file_name);
   ```

2. **Parse:**
   ```cpp
   jsn::object_t doc_ast;
   jsn::JsonReader{}.read(json, doc_ast);
   ```

3. **Navigate to Data Subtree:**
   ```cpp
   auto& document = get_SubNode(doc_ast, literal::document);
   ```

4. **Migrate Values:**
   ```cpp
   migrate_to_string_value(document.at(literal::data));
   ```

5. **Serialize:**
   ```cpp
   auto json_converted = std::string{};
   jsn::JsonWriter{}.write(doc_ast, json_converted);
   ```

6. **Overwrite File:**
   ```cpp
   db::write_buffer_to_file(json_converted, file_name);
   ```

### Before and After Examples

**Before (`config.json`):**
```json
{
  "document": {
    "data": {
      "threshold": 100,
      "enabled": true,
      "ratio": 0.75,
      "count": null,
      "items": [1, 2, 3],
      "settings": {
        "timeout": 30,
        "retry": false
      }
    },
    "metadata": {
      "version": "1.0"
    }
  }
}
```

**After:**
```json
{
  "document": {
    "data": {
      "threshold": "100",
      "enabled": "true",
      "ratio": "0.75",
      "count": "null",
      "items": ["1", "2", "3"],
      "settings": {
        "timeout": "30",
        "retry": "false"
      }
    },
    "metadata": {
      "version": "1.0"
    }
  }
}
```

Note: The `metadata` section is unchanged because only `document.data` is processed.

## Exit Codes

| Code | Constant | Meaning |
|------|----------|---------|
| 0 | `process_exit_code::SUCCESS` | Migration completed successfully |
| 1 | `process_exit_code::HELP` | Help message displayed |
| 1 | `process_exit_code::FAILURE` | Migration failed |
| 128 | `process_exit_code::INVALID_ARGUMENT` | Missing required arguments |
| 129 | `process_exit_code::INVALID_ARGUMENT \| 1` | Missing `--config` argument |
| 144 | `process_exit_code::UNCAUGHT_EXCEPTION` | Unhandled exception occurred |

## Error Handling

The tool uses assertions (`confirm()`) for critical failures:

```cpp
if (!jsn::JsonReader{}.read(json, doc_ast)) {
  confirm(false);  // Assertion failure - abrupt termination
}
```

**Important:** These assertions will terminate the program immediately rather than providing graceful error messages. This tool is designed for development/testing use, not production.

## Limitations

- **Data Subtree Only:** Only processes `document.data`; other subtrees are unchanged
- **In-Place Overwrite:** Overwrites the original file without creating a backup
- **Assertion-Based Errors:** Parse or write failures cause abrupt termination
- **String Formatting:** Floating-point numbers may lose precision in string conversion
- **No Dry Run:** Cannot preview changes before applying them

## Common Pitfalls

- **No Backup:** The tool overwrites the original file with no undo option; always backup first
- **Wrong Subtree:** Only `document.data` is processed; if your data is elsewhere, it won't be converted
- **Assertion Failures:** Parse or write failures terminate without helpful error messages
- **Floating Point:** String conversion of floats may affect precision (e.g., `0.1` might become `"0.10000000000000001"`)
- **Null Values:** `null` becomes the string `"null"`, not an empty string

### Anti-patterns

```bash
# DON'T: Run without backup
fixtestjson -c important.json  # Original is gone if something goes wrong

# DO: Always backup first
cp important.json important.json.bak
fixtestjson -c important.json
```

## Safety Recommendations

1. **Always backup files before running**
   ```bash
   cp original.json original.json.bak
   ```

2. **Test on a copy first**
   ```bash
   cp original.json test_copy.json
   fixtestjson -c test_copy.json
   cat test_copy.json | jq .  # Review output
   ```

3. **Verify the output matches expectations**
   ```bash
   diff original.json.bak original.json
   ```

4. **Use version control**
   ```bash
   git add original.json
   git commit -m "Before fixtestjson migration"
   fixtestjson -c original.json
   git diff original.json  # Review changes
   ```

## Relationship to Other Components

- **JsonReader/JsonWriter:** Uses these for JSON parsing and serialization
- **tostring_visitor:** Uses this Boost.Variant visitor for value-to-string conversion
- **readjson:** Related utility for JSON testing (read-only)
- **migrate_database:** Similar concept but operates on entire databases

## See Also

- [readjson.cc.md](./readjson.cc.md) - JSON parsing performance testing
- [migrate_database.cc.md](./migrate_database.cc.md) - Database schema migration
- [DataFormats/Json/json_types.cpp](../DataFormats/Json/json_types.cpp.md) - JSON type implementations including `tostring_visitor`
