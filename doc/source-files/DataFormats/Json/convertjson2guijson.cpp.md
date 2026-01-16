# convertjson2guijson.cpp

**Path:** `artdaq-database/DataFormats/Json/convertjson2guijson.cpp`

**Implements:** [convertjson2guijson.h](./convertjson2guijson.h.md)

**Purpose:** Provides the implementation of bidirectional conversion between database JSON format and GUI JSON format. This file contains the complex transformation logic that merges or splits data and metadata sections, handling nested structures, arrays, and various value types.

## Implementation Overview

This file implements the JSON format conversion interface declared in `convertjson2guijson.h`. The implementation uses recursive visitor patterns to traverse JSON structures, merging metadata with data for GUI display or splitting combined GUI structures back into separate data and metadata sections for database storage.

The conversion handles:
- Nested objects and arrays
- Type preservation (string, number, boolean, table, sequence)
- Comment and annotation metadata
- Changelog preservation during conversion

## Key Algorithms

### Database to GUI Conversion Algorithm

The `json_db_to_gui()` function and `db2gui::operator()` implement the conversion from database format to GUI format.

**High-Level Steps:**
1. Parse input database JSON string
2. Extract `document.data` and `document.metadata` sections
3. Create `db2gui` converter with data and metadata nodes
4. Apply recursive conversion to produce GUI structure
5. Rename result section to "guidata"
6. Preserve changelog if present
7. Generate output JSON string

**Recursive Conversion Logic (`db2gui::operator()`):**
- **VALUE type**: Push value directly to GUI array
- **DATA type**: Create GUI object with name, type, comment, and value; recurse for nested structures
- **ARRAY type**: Create "children" array and recurse for each element
- **OBJECT type**: Create "children" array and recurse for each key-value pair

**Why this approach:** The recursive visitor pattern naturally handles arbitrary nesting depth while maintaining the correspondence between data values and their metadata.

### GUI to Database Conversion Algorithm

The `json_gui_to_db()` function and `gui2db::operator()` implement the reverse conversion.

**High-Level Steps:**
1. Parse input GUI JSON string
2. Extract `document.converted.guidata` section
3. Create empty data and metadata ASTs
4. Create `gui2db` converter with GUI node
5. Apply recursive conversion to split GUI structure
6. Update document with separated data and metadata
7. Generate output JSON string

**Recursive Conversion Logic (`gui2db::operator()`):**
- **ARRAY type**: Iterate through GUI elements, extracting type information
  - For "sequence"/"array": Create child arrays and recurse
  - For "table"/"object": Create child objects and recurse
  - For primitives: Extract value and create metadata entry

**Why this approach:** The GUI format encodes type information inline, allowing the converter to reconstruct the appropriate database structure during traversal.

## Internal Functions

### `metadata_node_t::_string_value_of(name)`

**Brief:** Extracts a string value from a metadata node at the specified key.

**Called by:** `typeName()`, `comment()`, `annotation()`

**Purpose:** Provides uniform access to string metadata fields.

**Implementation:**
```cpp
std::string metadata_node_t::_string_value_of(std::string const& name) const {
  confirm(!name.empty());
  confirm(_node.type() == type_t::OBJECT);

  auto const& value_table = _node.value_as<object_t>();
  auto const& type = value_table.at(name);

  return boost::get<std::string>(type);
}
```

### `metadata_node_t::child(name)`

**Brief:** Retrieves a child metadata node by name, checking both direct children and the "children" section.

**Called by:** `db2gui::operator()` during recursive conversion

**Purpose:** Locates corresponding metadata for data elements, handling both flat and nested metadata structures.

**Implementation Strategy:**
1. Try direct child lookup first
2. If not found, try lookup in "children" section
3. Return false node if not found anywhere

### `metadata_node_t::hasMetadata()`

**Brief:** Checks if the metadata node contains valid metadata.

**Returns:** `true` if type is not `NOTSET`

### `db2gui::operator()`

**Brief:** Recursive converter that merges data and metadata into GUI format.

**Called by:** `json_db_to_gui()` and recursively by itself

**Purpose:** Core transformation logic for database-to-GUI conversion.

**Switch Cases:**
- `VALUE`: Simple value push to array
- `DATA`: Create annotated GUI object
- `ARRAY`: Handle array with "children" section
- `OBJECT`: Handle object with nested "children"

### `gui2db::operator()`

**Brief:** Recursive converter that splits GUI format into data and metadata.

**Called by:** `json_gui_to_db()` and recursively by itself

**Purpose:** Core transformation logic for GUI-to-database conversion.

## Performance Considerations

- **Recursive Depth:** Deep nesting in configurations can lead to deep recursion. Consider stack limits for very complex configurations.

- **Memory:** Each conversion creates multiple temporary AST objects. For large configurations, memory usage scales with document size.

- **String Operations:** Metadata field lookups involve string comparisons. For very large configurations with many fields, this could be a bottleneck.

## Error Handling Strategy

- **Parse Failures:** Throws `runtime_error` with descriptive message if JSON parsing fails.

- **Missing Structure:** Throws `runtime_error` if required document structure is missing (e.g., no "document.data" section).

- **Metadata Lookup Failures:** Silently handles missing metadata by using default values (e.g., "unknown" type, empty comment).

- **Exception Propagation:** Exceptions during recursive conversion are caught, logged via TRACE, and re-thrown.

## Testing Notes

- **Unit tests:** `test/DataFormats/Json/convertjson2guijson_t.cc`
- **Key test cases:**
  - Simple flat configurations
  - Nested objects
  - Arrays of primitives and objects
  - Changelog preservation
  - Round-trip conversion (db->gui->db)
  - Missing metadata handling

## Maintenance Notes

- **TRACE Levels:** The implementation uses TRACE levels 20-62 for detailed logging. Enable specific levels for debugging conversion issues.

- **Literal Constants:** String constants from `shared_literals.h` and `configuraion_api_literals.h` are used throughout. Changes to these constants require updates here.

- **Duplicate Keys:** The code comment notes that duplicate JSON keys are allowed for FHiCL compatibility. This is non-standard JSON but intentional.

## TRACE Logging Levels

| Level Range | Component | Purpose |
|-------------|-----------|---------|
| 20-24 | `metadata_node_t` | Metadata lookup and access |
| 25-30 | `json_db_to_gui` | Top-level db-to-gui conversion |
| 31-45 | `db2gui::operator()` | Recursive db-to-gui transformation |
| 46-53 | `json_gui_to_db` | Top-level gui-to-db conversion |
| 54-61 | `gui2db::operator()` | Recursive gui-to-db transformation |
| 62 | `debug::JSON2GUIJSON` | Debug mode enable |

## Implementation Details

### Type Mapping

| Database Type | GUI Type Field |
|---------------|----------------|
| `std::string` | `"string"` |
| `integer` | `"number"` |
| `decimal` | `"number"` |
| `bool` | `"bool"` |
| `object_t` | `"table"` |
| `array_t` | `"sequence"` |

### Annotation Handling

The implementation uses `artdaq::database::annotate()` to process annotation strings during GUI-to-DB conversion. This function normalizes annotation format for storage.

### Changelog Preservation

Both conversion directions preserve the changelog field if present:
- `db2gui`: Copies changelog from document root to converted section
- `gui2db`: Copies changelog from converted section back to document root

## See Also

- [convertjson2guijson.h](./convertjson2guijson.h.md) - Header file with class declarations
- [json_reader.h](./json_reader.h.md) - JSON parsing used by conversion
- [json_writer.h](./json_writer.h.md) - JSON generation used by conversion
- [json_types_impl.h](./json_types_impl.h.md) - Unwrapper template implementations
- [shared_literals.h](../shared_literals.h.md) - String constants used in conversion
