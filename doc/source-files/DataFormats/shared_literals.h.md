# shared_literals.h

**Path:** `artdaq-database/DataFormats/shared_literals.h`

**Purpose:** Defines a comprehensive set of compile-time string constants used throughout the DataFormats module and beyond. These literals represent JSON field names, MongoDB operators, data type identifiers, document structure keys, and regex patterns. Centralizing these strings ensures consistency across all data format implementations (JSON, XML, FHiCL, Conf) and simplifies refactoring.


## Key Concepts

### Compile-Time String Constants

All literals are declared as `constexpr auto`, meaning:
- They are evaluated at compile time with zero runtime overhead
- The compiler can optimize string comparisons
- Typos in literal names cause compile errors rather than runtime bugs

### Namespace Organization

The literals are organized under `artdaq::database::dataformats::literal`, with regex patterns in a nested `regex` sub-namespace.

### Cross-Format Consistency

These literals provide a common vocabulary for field names across all supported configuration formats. Whether parsing JSON, XML, or FHiCL, the same field names are used, enabling seamless format conversion.

## Thread Safety

- **Thread-safe:** Yes
- **Concurrent access:** All literals are compile-time constants with no mutable state
- **Locking:** None required

## Dependencies

| Include | Purpose |
|---------|---------|
| `<string>` | Required for `std::string` type and string literals |

## Using Directive

```cpp
using namespace std::string_literals;
```

**Brief:** Enables the `""s` suffix for creating `std::string` objects directly from string literals (C++14 feature).

**Example:**
```cpp
auto myString = "hello"s;  // Creates std::string, not const char*
```

## Constants Reference

### General Purpose Literals

| Literal | Value | Description |
|---------|-------|-------------|
| `dot` | `"."` | Dot character used as path separator in nested field access |
| `empty_json` | `"{}"` | Empty JSON object string representation |

### Document Structure Literals

These literals define the structure of documents stored in the database.

| Literal | Value | Description |
|---------|-------|-------------|
| `database_record` | `"record"` | Top-level container for a database record |
| `main` | `"main"` | Main content section of a document |
| `prolog` | `"prolog"` | Prolog section (FHiCL-specific metadata) |
| `entities` | `"entities"` | Collection of entity references |
| `entity` | `"entity"` | Single entity reference |
| `configurations` | `"configurations"` | Collection of configurations |
| `configuration` | `"configuration"` | Single configuration reference |
| `collection` | `"collection"` | Database collection name field |
| `document` | `"document"` | Document container field |

### MongoDB-Specific Literals

These literals follow MongoDB Extended JSON and query operator conventions.

| Literal | Value | Description |
|---------|-------|-------------|
| `id` | `"_id"` | MongoDB document identifier field |
| `oid` | `"$oid"` | MongoDB ObjectId wrapper in Extended JSON |
| `in` | `"$in"` | MongoDB `$in` query operator for matching any value in an array |
| `ouid` | `"ouid"` | Custom object unique identifier |
| `filter` | `"filter"` | Query filter specification field |

### Metadata Literals

| Literal | Value | Description |
|---------|-------|-------------|
| `timestamp` | `"timestamp"` | Creation or modification timestamp |
| `source` | `"source"` | Source file or origin identifier |
| `format` | `"format"` | Data format identifier (json, xml, fhicl, conf) |
| `file` | `"file"` | File path or name |
| `value` | `"value"` | Generic value container |
| `version` | `"version"` | Version number or identifier |
| `metadata` | `"metadata"` | Metadata section container |
| `origin` | `"origin"` | Origin information section |
| `data` | `"data"` | Data payload section |

### Data Type Identifiers

These literals identify the type of a value in format-neutral representations.

| Literal | Value | Description |
|---------|-------|-------------|
| `nil` | `"nil"` | Null or undefined value type |
| `string` | `"string"` | String value type |
| `boolean` | `"bool"` | Boolean value type |
| `complex` | `"complex"` | Complex/composite value type |
| `number` | `"number"` | Numeric value type (integer or floating-point) |
| `tableid` | `"tableid"` | Table identifier type |
| `type` | `"type"` | Field containing type information |
| `object` | `"object"` | Object/table/dictionary type |
| `table` | `"table"` | Table type (FHiCL terminology) |
| `array` | `"array"` | Array type |
| `sequence` | `"sequence"` | Sequence type (FHiCL terminology for arrays) |

### String Quoting Type Literals

These literals track how strings were originally quoted in the source format.

| Literal | Value | Description |
|---------|-------|-------------|
| `string_singlequoted` | `"string_singlequoted"` | String was single-quoted in source |
| `string_doublequoted` | `"string_doublequoted"` | String was double-quoted in source |
| `string_unquoted` | `"string_unquoted"` | String was unquoted in source |

### Document Property Literals

| Literal | Value | Description |
|---------|-------|-------------|
| `empty` | `"empty"` | Indicates an empty value or container |
| `comment` | `"comment"` | Comment text associated with a field |
| `annotation` | `"annotation"` | Annotation text for a value |
| `protection` | `"protection"` | Protection level or access control |
| `isdeleted` | `"isdeleted"` | Soft-delete flag |
| `isreadonly` | `"isreadonly"` | Read-only flag |
| `converted` | `"converted"` | Indicates value was converted from another format |
| `notprovided` | `"notprovided"` | Indicates value was not provided |

### Structural Element Literals

| Literal | Value | Description |
|---------|-------|-------------|
| `children` | `"children"` | Child elements in a hierarchical structure |
| `name` | `"name"` | Name field for entities and elements |
| `values` | `"values"` | Collection of values |
| `unknown` | `"unknown"` | Unknown or unrecognized type/value |
| `include` | `"#include"` | FHiCL include directive |
| `includes` | `"includes"` | Collection of include references |
| `linenum` | `"linenum"` | Source file line number for debugging |

### Version Control and History Literals

| Literal | Value | Description |
|---------|-------|-------------|
| `changelog` | `"changelog"` | Change history log |
| `bookkeeping` | `"bookkeeping"` | Administrative metadata section |
| `updates` | `"updates"` | Collection of update records |
| `update` | `"update"` | Single update record |
| `operation` | `"operation"` | Operation type (create, update, delete) |
| `history` | `"history"` | Historical versions section |
| `event` | `"event"` | Event record in history |

### Operation Type Literals

| Literal | Value | Description |
|---------|-------|-------------|
| `assigned` | `"assigned"` | Value or alias was assigned |
| `removed` | `"removed"` | Value or alias was removed |
| `created` | `"created"` | Document was created |
| `operations` | `"operations"` | Collection of operations |
| `search` | `"search"` | Search operation type |
| `query` | `"query"` | Query specification |

### Collection and Data Literals

| Literal | Value | Description |
|---------|-------|-------------|
| `runs` | `"runs"` | Run configurations collection |
| `attachments` | `"attachments"` | File attachments section |
| `comments` | `"comments"` | User comments collection |
| `rawdatalist` | `"rawdata"` | Raw data entries |
| `aliases` | `"aliases"` | Alias definitions collection |
| `alias` | `"alias"` | Single alias reference |
| `active` | `"active"` | Active status flag |

### Configuration-Specific Literals

| Literal | Value | Description |
|---------|-------|-------------|
| `configurationtype` | `"configtype"` | Configuration type identifier |
| `gui_data` | `"guidata"` | GUI-specific data section |
| `schema` | `"schema"` | Schema definition reference |

### Composite Path Literals

Pre-built dot-notation paths for common nested field access.

| Literal | Value | Description |
|---------|-------|-------------|
| `origin_format` | `"origin.format"` | Path to format field within origin section |
| `document_data` | `"document.data"` | Path to data field within document section |

### Regex Patterns (regex namespace)

| Literal | Pattern | Description |
|---------|---------|-------------|
| `parse_decimal` | `-?\\d*\\.?\\d+e[+-]?\\d+` | Matches scientific notation floating-point numbers (e.g., `1.5e-10`, `-3.14e+2`) |
| `parse_include` | `(#include)` | Matches FHiCL include directive |

## Functions

This header defines no functions - it contains only compile-time constants.

## Usage Examples

### Accessing Literals

```cpp
#include "artdaq-database/DataFormats/shared_literals.h"

// Create namespace alias for convenience
namespace literal = artdaq::database::dataformats::literal;

void processDocument(json::object_t& doc) {
    // Access fields using literals
    auto& metadata = doc[literal::metadata];
    auto& version = doc[literal::version];
    auto format = doc[literal::format].get<std::string>();

    // Check data type
    if (format == literal::string) {
        // Handle string type
    }
}
```

### Building MongoDB Queries

```cpp
#include "artdaq-database/DataFormats/shared_literals.h"
#include <sstream>

namespace literal = artdaq::database::dataformats::literal;

std::string buildObjectIdQuery(std::string const& objectId) {
    std::ostringstream query;
    query << "{\"" << literal::id << "\":{\""
          << literal::oid << "\":\"" << objectId << "\"}}";
    return query.str();
    // Result: {"_id":{"$oid":"<objectId>"}}
}

std::string buildInQuery(std::vector<std::string> const& ids) {
    std::ostringstream query;
    query << "{\"" << literal::id << "\":{\"" << literal::in << "\":[";
    // ... add ids ...
    query << "]}}";
    return query.str();
}
```

### Type Checking

```cpp
#include "artdaq-database/DataFormats/shared_literals.h"

namespace literal = artdaq::database::dataformats::literal;

void processValue(std::string const& typeField, json::value_t const& value) {
    if (typeField == literal::string) {
        auto str = boost::get<std::string>(value);
        // Handle string
    } else if (typeField == literal::number) {
        // Handle number (could be integer or decimal)
    } else if (typeField == literal::boolean) {
        auto flag = boost::get<bool>(value);
        // Handle boolean
    } else if (typeField == literal::nil) {
        // Handle null value
    }
}
```

### Using Regex Patterns

```cpp
#include "artdaq-database/DataFormats/shared_literals.h"
#include <regex>

namespace literal = artdaq::database::dataformats::literal;

bool isScientificNotation(std::string const& input) {
    std::regex pattern(literal::regex::parse_decimal);
    return std::regex_match(input, pattern);
}

bool containsIncludeDirective(std::string const& line) {
    std::regex pattern(literal::regex::parse_include);
    return std::regex_search(line, pattern);
}
```

## Relationship to Other Components

```
shared_literals.h
    |
    +-- Used by Json/json_types.h (field names for JSON documents)
    +-- Used by Xml/xml_types.h (element and attribute names)
    +-- Used by Fhicl/fhicl_types.h (FHiCL table and sequence keys)
    +-- Used by Conf/conf_types.h (CONF format field names)
    +-- Used by Overlay/*.h (document overlay field access)
    +-- Used by JsonDocument/*.h (document builder field names)
```

## See Also

- [shared_types.h](./shared_types.h.md) - Type definitions that use these literals as field names
- [common.h](./common.h.md) - Common header for DataFormats module
- [Json/json_types.h](./Json/json_types.h.md) - JSON-specific types using these literals
- [Overlay/JSONDocumentOverlay.h](../Overlay/JSONDocumentOverlay.h.md) - Document overlays that access fields using these literals

## Notes for Developers

### Common Pitfalls

- **Pitfall 1:** Do not use `using namespace artdaq::database::dataformats::literal;` in header files - it pollutes the namespace. Use a namespace alias instead.
- **Pitfall 2:** The `boolean` literal value is `"bool"`, not `"boolean"`. Similarly, `configurationtype` is `"configtype"`.

### Best Practices

1. **Use namespace aliases:** Create a short alias for cleaner code:
   ```cpp
   namespace literal = artdaq::database::dataformats::literal;
   ```

2. **Prefer literals over raw strings:** Using literals prevents typos:
   ```cpp
   // Good - typo causes compile error
   doc[literal::versiom];  // Error: 'versiom' is not a member

   // Bad - typo causes runtime bug
   doc["versiom"];  // Compiles but accesses wrong field
   ```

3. **Use composite paths for nested access:**
   ```cpp
   // Using composite literal
   auto format = getNestedValue(doc, literal::origin_format);

   // Equivalent to
   auto format = doc[literal::origin][literal::format];
   ```

### Extending the Literals

When adding new literals:
1. Add them to the appropriate category section
2. Use `constexpr auto` for compile-time evaluation
3. Follow existing naming conventions (lowercase, descriptive)
4. Update this documentation
