# convertfhicl2jsondb.cpp

**Path:** `artdaq-database/DataFormats/Fhicl/convertfhicl2jsondb.cpp`

**Implements:** [convertfhicl2jsondb.h](./convertfhicl2jsondb.h.md)

**Purpose:** Implements bidirectional conversion between FHiCL documents and database JSON format. This file contains the core conversion logic that transforms FHiCL AST nodes to JSON with metadata preservation, and vice versa, handling all FHiCL value types including nested structures.

## Implementation Overview

The implementation provides two main conversion functors:

1. **`fcl2jsondb`**: Converts FHiCL extended_value nodes to JSON data/metadata pairs
2. **`json2fcldb`**: Converts JSON data/metadata pairs back to FHiCL value structures

Both converters handle recursive structures (tables and sequences) and preserve comments, annotations, and protection attributes.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/common.h` | Common infrastructure and macros |
| `artdaq-database/DataFormats/Fhicl/convertfhicl2jsondb.h` | Class declarations |
| `artdaq-database/DataFormats/Fhicl/fhiclcpplib_includes.h` | FHiCL library types |
| `artdaq-database/DataFormats/Fhicl/helper_functions.h` | String conversion utilities |
| `artdaq-database/DataFormats/Json/json_types_impl.h` | JSON unwrapper implementations |
| `artdaq-database/SharedCommon/configuraion_api_literals.h` | API literal constants |
| `artdaq-database/SharedCommon/printStackTrace.h` | Stack trace utilities |
| `<boost/variant/get.hpp>` | Variant value extraction |
| `<cmath>` | Mathematical functions |

## TRACE Configuration

```cpp
#define TRACE_NAME "convertfhicl2jsondb.cpp"
```

TRACE levels used:
- Level 11: Key logging during conversion
- Level 12: Value logging during conversion
- Level 13: Protection attribute logging
- Level 14-16: JSON to FHiCL conversion details
- Level 17: Debug enable notification

## Key Algorithms

### FHiCL to JSON Conversion (fcl2jsondb::operator datapair_t)

**Brief:** Converts a single FHiCL key-value pair to a JSON data/metadata pair, recursively processing nested structures.

**Steps:**
1. Extract key and value from the FHiCL pair
2. Initialize return data structures with the key
3. Store type information in metadata object
4. Parse line number from FHiCL source info
5. Extract and store comments/annotations based on line numbers
6. Store protection attributes if present
7. Switch on FHiCL value tag to handle each type:
   - **UNKNOWN/NIL/BOOL/NUMBER/COMPLEX/TABLEID**: Store raw value directly
   - **STRING**: Store with quotation style metadata
   - **SEQUENCE**: Recursively convert each element, store in array
   - **TABLE**: Recursively convert each key-value pair, store in object
8. Return the data/metadata pair

**Why this approach:** The recursive structure mirrors FHiCL's nested nature while the parallel data/metadata structure allows complete reconstruction during reverse conversion.

### JSON to FHiCL Conversion (json2fcldb::operator fcl::value_t)

**Brief:** Converts a JSON value to a FHiCL value, using metadata to determine the correct output format.

**Steps:**
1. Check the JSON value type (bool, integer, decimal, string, object, array)
2. For primitive types, return directly wrapped in fcl::value_t
3. For strings, consult metadata for quotation style
4. For complex types, delegate to operator fcl::atom_t()

### JSON to FHiCL Atom Conversion (json2fcldb::operator fcl::atom_t)

**Brief:** Converts a JSON key-value pair with metadata to a complete FHiCL atom including key, value, comments, and annotations.

**Steps:**
1. Extract key, data, and metadata from the input tuple
2. Get type name from metadata
3. Handle special cases:
   - Table stored as string (type override)
   - `#include` directives (name override)
4. Switch on FHiCL type to construct value:
   - **UNKNOWN/NIL**: Store as raw string
   - **STRING**: Apply appropriate quoting based on metadata
   - **BOOL/NUMBER**: Handle string or native representations
   - **TABLEID/COMPLEX**: Store as string
   - **SEQUENCE**: Recursively convert elements
   - **TABLE**: Recursively convert key-value pairs
5. Construct fcl::key_t with comment
6. Apply annotation to value (except for tables)
7. Apply protection modifier to key name
8. Return the complete atom

## Internal Functions

### `need_quotes(text) -> bool`

**Brief:** Determines if a string value requires quoting in FHiCL output.

**Called by:** `json2fcldb::operator fcl::value_t()`, `json2fcldb::operator fcl::atom_t()`

**Purpose:** Checks if the string contains characters that require quoting in FHiCL syntax.

**Implementation:**
```cpp
bool need_quotes(std::string const& text) {
  return text.find_first_of(" .%()/-\\") != std::string::npos;
}
```

### Lambda: `parse_linenum`

**Brief:** Extracts the line number from FHiCL source info string.

**Called by:** `fcl2jsondb::operator datapair_t()`

**Purpose:** Parses strings like "filename:42" to extract the line number for comment association.

### Lambda: `annotation_at`

**Brief:** Retrieves the annotation (inline comment) at a specific line number.

**Called by:** `comment_at`, `add_comment_annotation`

**Purpose:** Looks up the comments map to find any annotation on a given line.

### Lambda: `comment_at`

**Brief:** Retrieves the leading comment for a specific line, filtering out annotations and includes.

**Called by:** `add_comment_annotation`

**Purpose:** Distinguishes between leading comments and inline annotations.

### Lambda: `add_comment_annotation`

**Brief:** Adds comment or annotation fields to the metadata object.

**Called by:** `fcl2jsondb::operator datapair_t()`

**Purpose:** Stores comment/annotation in metadata, using null string for missing values.

## Error Handling Strategy

### Exception Translation

The `fcl2jsondb::operator datapair_t()` function wraps conversion in a try-catch block that translates various exceptions:

```cpp
} catch (boost::bad_lexical_cast const& e) {
  throw ::fhicl::exception(::fhicl::cant_insert, self.first) << e.what();
} catch (boost::bad_numeric_cast const& e) {
  throw ::fhicl::exception(::fhicl::cant_insert, self.first) << e.what();
} catch (::fhicl::exception const& e) {
  throw ::fhicl::exception(::fhicl::cant_insert, self.first, e);
} catch (std::exception const& e) {
  throw ::fhicl::exception(::fhicl::cant_insert, self.first) << e.what();
}
```

This ensures all errors are reported as `::fhicl::exception` with the problematic key name for debugging.

### Silent Error Handling for Missing Metadata

The `json2fcldb` operators catch `std::out_of_range` exceptions when accessing optional metadata fields:

```cpp
try {
  auto const& children = boost::get<jsn::object_t>(metadata_object.at(literal::children));
  // Process children...
} catch (std::out_of_range const&) {
  // Field missing, continue without children
}
```

## Template Specializations

### `unwrapper<fcl::value_t>::value_as<T>()`

**Brief:** Enables extraction of typed values from FHiCL value variants.

```cpp
template <>
template <typename T>
T& unwrapper<fcl::value_t>::value_as() {
  return boost::get<T>(any.value);
}
```

**Purpose:** Provides consistent unwrapper interface for FHiCL types.

## Performance Considerations

- **Recursive processing:** Deep FHiCL structures may cause deep recursion; stack overflow possible for extremely nested configurations
- **String operations:** Comment and annotation extraction involves string parsing for each value
- **Memory allocation:** New JSON objects created for each converted node

## Testing Notes

- **Unit tests:** `test/DataFormats/Fhicl/convertfhicl2jsondb_t.cc`
- **Key test cases:**
  - Round-trip conversion (FHiCL -> JSON -> FHiCL)
  - Comment preservation
  - All FHiCL types (NIL, BOOL, NUMBER, STRING, SEQUENCE, TABLE)
  - Protection attributes
  - Prolog handling
  - Special characters in strings

## Maintenance Notes

- The `include` variable (`"fhicl_pound_include"`) is used to detect and handle `#include` directives specially
- The typo in protection string `"@none"` must match throughout the codebase
- Line number parsing assumes FHiCL source info format `"filename:linenum"`

## See Also

- [convertfhicl2jsondb.h](./convertfhicl2jsondb.h.md) - Header file with declarations
- [fhicl_types.h](./fhicl_types.h.md) - FHiCL AST type definitions
- [helper_functions.h](./helper_functions.h.md) - String escaping utilities
- [json_types_impl.h](../Json/json_types_impl.h.md) - JSON unwrapper implementations
